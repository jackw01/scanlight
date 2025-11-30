#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"

static const uint32_t PinR = 7;
static const uint32_t PinG = 8;
static const uint32_t PinB = 9;
static const uint32_t PinShutter = 14;
static const uint32_t RGBPins[] = {PinR, PinG, PinB};

static const uint32_t PWMFreqHz = 40000;

static const uint32_t ShutterPulseDelayMs = 100;
static const uint32_t TickIntervalUs = 1000;

uint8_t color[3] = { 255, 255, 255 };
uint32_t pwm_wrap;

// Camera

void camera_activate_shutter() {
  gpio_put(PinShutter, true);
  sleep_ms(ShutterPulseDelayMs);
  gpio_put(PinShutter, false);
}

// PWM

void update_pwm() {
  for (int i = 0; i < 3; i++) {
    uint slice_num = pwm_gpio_to_slice_num(RGBPins[i]);
    uint channel = pwm_gpio_to_channel(RGBPins[i]);
    // Duty cycle = wrap * percent / 100
    uint32_t level = (pwm_wrap * color[i]) / 255;
    pwm_set_chan_level(slice_num, channel, level);
  }
}

// UART

static const char PacketStartByte = 254;
static const char PacketEndByte = 255;

enum CmdType {
    CmdTypeSet = 0,
    CmdTypeReset,
    CmdTypeMax
};

char packet_type;
char incoming_packet[128];
uint32_t packet_index = 0;
uint32_t packet_end;

void handle_uart_packet() {
  if (packet_type == CmdTypeSet) {
    color[0] = incoming_packet[0];
    color[1] = incoming_packet[1];
    color[2] = incoming_packet[2];
    update_pwm();
  }
}

void handle_uart_input(char c) {
  if (packet_index == 0 && c == PacketStartByte) {
    // Packet start
    packet_index++;
  } else if (packet_index == 1) {
    // Packet type
    if (c < CmdTypeMax) {
      packet_type = c;
      packet_index++;
    } else packet_index = 0;
  } else if (packet_index == 2) {
      // Data Length
      packet_end = c + 2;
      packet_index++;
  } else if (packet_index >= 3 && packet_index < packet_end && packet_index - 3 < sizeof(incoming_packet) - 1) {
    // Data start
    incoming_packet[packet_index - 3] = c;
    packet_index++;
  } else {
    // End
    incoming_packet[packet_index - 3] = c;
    incoming_packet[packet_index - 2] = 0;
    handle_uart_packet();
    packet_index = 0;
  }
}

// Main loop

void tick(uint32_t millis, uint32_t dt_micros) {
  while (true) {
    int32_t c = getchar_timeout_us(0);
    if (c != PICO_ERROR_TIMEOUT) handle_uart_input(c);
    else break;
  }
}

int main() {
  stdio_init_all();

  gpio_init(PinShutter);
  gpio_set_dir(PinShutter, GPIO_OUT);
  gpio_put(PinShutter, false);

  uint32_t f_sys = clock_get_hz(clk_sys);
  uint32_t clk_div = 4;
  pwm_wrap = (f_sys / (clk_div * PWMFreqHz)) - 1;

  for (int i = 0; i < 3; i++) {
    gpio_set_function(RGBPins[i], GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(RGBPins[i]);
    uint channel = pwm_gpio_to_channel(RGBPins[i]);

    pwm_set_clkdiv_int_frac(slice_num, clk_div, 0);
    pwm_set_wrap(slice_num, pwm_wrap);

    pwm_set_chan_level(slice_num, channel, pwm_wrap);
    pwm_set_enabled(slice_num, true);
  }

  // visual self test at startup
  busy_wait_ms(100);
  color[0] = 255;
  color[1] = 0;
  color[2] = 0;
  update_pwm();
  busy_wait_ms(150);
  color[0] = 0;
  color[1] = 255;
  color[2] = 0;
  update_pwm();
  busy_wait_ms(150);
  color[0] = 0;
  color[1] = 0;
  color[2] = 255;
  update_pwm();
  busy_wait_ms(150);
  color[0] = 255;
  color[1] = 255;
  color[2] = 255;
  update_pwm();

  uint64_t micros;
  uint32_t dt;

  while (true) {
    micros = time_us_64();
    tick(us_to_ms(micros), dt);
    dt = time_us_64() - micros;
    if (dt < TickIntervalUs) {
      absolute_time_t target;
      update_us_since_boot(&target, micros + TickIntervalUs);
      busy_wait_until(target);
    }
  }
}
