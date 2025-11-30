#include <math.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/sync.h"
#include "hardware/flash.h"

#include "button.h"

#include "config.h"
#include "protocol.h"

// R, G, B, W, IR
uint8_t color[5] = {0, 0, 0, 0, 0};
int8_t trim[4] = {0, 0, 0, 0};
uint8_t rgb_preset[3] = {255, 255, 255};

uint32_t pwm_wrap;

OperatingMode operating_mode = OperatingModeOff;
uint32_t led_enable = 0;

uint32_t adc_reporting_timer = 1;
int32_t led_temp_mdegc = 0;
int32_t vbus_mv = 0;

// Camera

void camera_activate_shutter()
{
  gpio_put(PinShutter, true);
  sleep_ms(ShutterPulseDelayMs);
  gpio_put(PinShutter, false);
}

// PWM

void update_pwm_pin(uint32_t pin, uint8_t level, uint8_t scale1, uint8_t scale2)
{
  uint slice_num = pwm_gpio_to_slice_num(pin);
  uint channel = pwm_gpio_to_channel(pin);
  uint32_t level_scaled = (pwm_wrap * ((uint32_t)level * scale1) / 255) / 255;
  level_scaled = (level_scaled * scale2) / 255;
  pwm_set_chan_level(slice_num, channel, level_scaled);
}

void update_pwm()
{
  // ensure white and IR channels exclude other channels
  if (color[3] > 0)
  {
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
    color[4] = 0;
  }
  else if (color[4] > 0)
  {
      color[0] = 0;
      color[1] = 0;
      color[2] = 0;
      color[3] = 0;
  }

  // reduced power operating mode allows one channel at a time, or multichannel RGB at reduced power
  uint8_t scale = 255;

  if (operating_mode == OperatingModeReducedPower)
  {
    uint32_t n_channels = 0;
    for (uint32_t i = 0; i < 5; i++)
    {
      if (color[i] > 0)
        n_channels++;
    }
    if (n_channels > 1)
      scale = ReducedPowerLevelRGB;
    else
      scale = ReducedPowerLevelSingle;
  }

  // led_enable overrides all other settings
  if (led_enable == 0)
    scale = 0;

  update_pwm_pin(PinR1, color[0], (uint32_t)(trim[0] > 0 ? 255 - trim[0] : 255), scale);
  update_pwm_pin(PinG1, color[1], (uint32_t)(trim[1] > 0 ? 255 - trim[1] : 255), scale);
  update_pwm_pin(PinB1, color[2], (uint32_t)(trim[2] > 0 ? 255 - trim[2] : 255), scale);
  update_pwm_pin(PinW1, color[3], (uint32_t)(trim[3] > 0 ? 255 - trim[3] : 255), scale);
  update_pwm_pin(PinR2, color[0], (uint32_t)(trim[0] < 0 ? 255 + trim[0] : 255), scale);
  update_pwm_pin(PinG2, color[1], (uint32_t)(trim[1] < 0 ? 255 + trim[1] : 255), scale);
  update_pwm_pin(PinB2, color[2], (uint32_t)(trim[2] < 0 ? 255 + trim[2] : 255), scale);
  update_pwm_pin(PinW2, color[3], (uint32_t)(trim[3] < 0 ? 255 + trim[3] : 255), scale);
  update_pwm_pin(PinIR, color[4], 255, 255);
}

// ADC

float thermistor_voltage_to_temp(float v, const float lookup_table[][2])
{
  // convert voltage to temperature using a lookup table with linear interpolation
  // lookup table format:
  // float ThermistorLookUpTable[][2] = {
  //     {V, degrees C}, {V, degrees C}, ...
  // }
  // entries are assumed to be in descending order by voltage

  uint32_t count = sizeof(lookup_table) / sizeof(lookup_table[0]);

  if (v >= lookup_table[0][0])
    return lookup_table[0][1];

  if (v <= lookup_table[count - 1][0])
    return lookup_table[count - 1][1];

  for (uint32_t i = 0; i < count - 1; i++)
  {
    float v1 = lookup_table[i][0];
    float t1 = lookup_table[i][1];
    float v2 = lookup_table[i + 1][0];
    float t2 = lookup_table[i + 1][1];

    if (v <= v1 && v >= v2)
    {
      float f = (v - v2) / (v1 - v2);
      return t2 + f * (t1 - t2);
    }
  }

  return 0.0f;
}

int32_t adc_get_led_temp_mdegc()
{
  adc_select_input(PinADCThermistor);
  float raw_v = (float)adc_read() * ADCVPerCount;
  return (int32_t)(thermistor_voltage_to_temp(raw_v, ThermistorLookUpTable) * 1e3);
}

int32_t adc_get_vbus_mv()
{
  adc_select_input(PinADCVBUSSense);
  float raw_v = (float)adc_read() * ADCVPerCount;
  return (int32_t)(raw_v * ADCVBUSSenseScale * 1e3);
}

// Flash
void write_preset_to_flash()
{
  uint32_t irq_state = save_and_disable_interrupts();

  uint8_t page_buffer[FLASH_PAGE_SIZE];
  memset(page_buffer, 0xFF, sizeof(page_buffer));
  memcpy(page_buffer, rgb_preset, 3);

  flash_range_erase(FLASH_PERSIST_OFFSET, FLASH_SECTOR_SIZE);
  flash_range_program(FLASH_PERSIST_OFFSET, page_buffer, FLASH_PAGE_SIZE);

  restore_interrupts(irq_state);
}

void read_preset_from_flash()
{
  const uint8_t *flash_ptr = (const uint8_t *)(XIP_BASE + FLASH_PERSIST_OFFSET);
  memcpy(rgb_preset, flash_ptr, 3);

  if (rgb_preset[0] == 0 && rgb_preset[1] == 0 && rgb_preset[2] == 0)
  {
    rgb_preset[0] = 255;
    rgb_preset[1] = 255;
    rgb_preset[2] = 255;
  }
}

// Button handler

void on_button_change(button_t *button_p)
{
  button_t *button = (button_t *)button_p;
  if (button->state)
    return; // Ignore button release

  switch (button->pin)
  {
    case PinButton1:
      // LED on/off toggle
      led_enable = !led_enable;
      update_pwm();
      break;
    case PinButton2:
      // RGB/white mode toggle
      if (color[3] > 0)
      {
        color[0] = rgb_preset[0];
        color[1] = rgb_preset[1];
        color[2] = rgb_preset[2];
        color[3] = 0;
      }
      else
      {
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
        color[3] = 255;
      }
      update_pwm();
      break;
  }
}

// Main loop

void tick(uint32_t millis, uint32_t dt_micros)
{
  button_poll_events();
  protocol_tick();

  if (protocol_data.incoming_packet_ready != 0)
  {
    protocol_data.incoming_packet_ready = 0;
    switch (protocol_data.incoming_packet_header)
    {
      case PKT_H2D_SET_COLOR:
        memcpy(color, protocol_data.incoming_packet_data, 5);
        if (protocol_data.incoming_packet_data[5] != 0)
        {
          memcpy(rgb_preset, protocol_data.incoming_packet_data, 3);
          write_preset_to_flash();
        }
        led_enable = 1;
        update_pwm();
        break;
      case PKT_H2D_GET_DEFAULT_RGB:
        protocol_send_packet(PKT_D2H_DEFAULT_RGB, rgb_preset, 3);
        break;
      case PKT_H2D_GET_FW_VERSION:
        protocol_send_packet_int32(PKT_D2H_FW_VERSION, FirmwareVersionID);
        break;
    }
  }

  if (millis >= adc_reporting_timer)
  {
    led_temp_mdegc = adc_get_led_temp_mdegc();
    vbus_mv = adc_get_vbus_mv();

    OperatingMode new_operating_mode;

    if (vbus_mv < USBVBUSThreshold5V)
      new_operating_mode = OperatingModeOff;
    else if (vbus_mv > USBVBUSThreshold9V)
      new_operating_mode = OperatingModeFullPower;
    else
      new_operating_mode = OperatingModeReducedPower;

    if (new_operating_mode != operating_mode)
    {
      if (new_operating_mode == OperatingModeOff)
      {
        // undervoltage protection - ensure that LED drivers are off before USB is connected
        new_operating_mode = OperatingModeOff;
        // clear color setting
        for (uint32_t i = 0; i < 5; i++)
        {
          color[i] = 0;
        }
      }

      if (operating_mode == OperatingModeOff)
      {
        // visual self test when power is connected
        led_enable = 1;
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
        color[3] = 32;
        update_pwm();
        busy_wait_ms(150);
        color[0] = 32;
        color[1] = 0;
        color[2] = 0;
        color[3] = 0;
        update_pwm();
        busy_wait_ms(150);
        color[0] = 0;
        color[1] = 32;
        color[2] = 0;
        update_pwm();
        busy_wait_ms(150);
        color[0] = 0;
        color[1] = 0;
        color[2] = 32;
        update_pwm();
        busy_wait_ms(150);
        // set color to RGB "white" but off by default
        color[0] = rgb_preset[0];
        color[1] = rgb_preset[1];
        color[2] = rgb_preset[2];
        led_enable = 0;
      }

      operating_mode = new_operating_mode;
      update_pwm();
    }

    protocol_send_packet_int32(PKT_D2H_LED_TEMP, led_temp_mdegc);
    protocol_send_packet_int32(PKT_D2H_VBUS, vbus_mv);
    adc_reporting_timer = millis + ADCReportingIntervalMs;
  }
}

int main()
{
  stdio_init_all();

  read_preset_from_flash();

  adc_init();
  adc_gpio_init(PinADCGPIOOffset + PinADCThermistor);
  adc_gpio_init(PinADCGPIOOffset + PinADCVBUSSense);

  gpio_init(PinShutter);
  gpio_set_dir(PinShutter, GPIO_OUT);
  gpio_put(PinShutter, false);

  button_system_init();

  button_t *button_1 = create_button(PinButton1, on_button_change);
  button_t *button_2 = create_button(PinButton2, on_button_change);

  // setup PWM pins
  uint32_t f_sys = clock_get_hz(clk_sys);
  uint32_t clk_div = 4;
  pwm_wrap = (f_sys / (clk_div * PWMFreqHz)) - 1;

  for (int i = 0; i < PWM_PIN_COUNT; i++)
  {
    gpio_set_function(PWMPins[i], GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(PWMPins[i]);
    uint channel = pwm_gpio_to_channel(PWMPins[i]);

    pwm_set_clkdiv_int_frac(slice_num, clk_div, 0);
    pwm_set_wrap(slice_num, pwm_wrap);

    pwm_set_chan_level(slice_num, channel, 0);
    pwm_set_enabled(slice_num, true);
  }

  uint64_t micros;
  uint32_t dt;

  while (true)
  {
    micros = time_us_64();
    tick(us_to_ms(micros), dt);
    dt = time_us_64() - micros;
    if (dt < TickIntervalUs)
    {
      absolute_time_t target;
      update_us_since_boot(&target, micros + TickIntervalUs);
      busy_wait_until(target);
    }
  }
}
