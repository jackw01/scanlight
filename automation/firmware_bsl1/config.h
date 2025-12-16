#include "pico/stdlib.h"

#include "hardware/flash.h"

static const int32_t FirmwareVersionID = 0;

static const uint32_t PinR1 = 2;
static const uint32_t PinG1 = 3;
static const uint32_t PinB1 = 4;
static const uint32_t PinW1 = 1;
static const uint32_t PinR2 = 6;
static const uint32_t PinG2 = 9;
static const uint32_t PinB2 = 10;
static const uint32_t PinW2 = 5;
static const uint32_t PinIR = 11;
static const uint32_t PinButton1 = 16; // top button
static const uint32_t PinButton2 = 17; // bottom button
static const uint32_t PinShutter = 25;
static const uint32_t PinADCGPIOOffset = 26;
static const uint32_t PinADCVBUSSense = 0;
static const uint32_t PinADCThermistor = 1;

static const float ADCVPerCount = 3.3 / 4095.0;
static const float ADCVBUSSenseScale = (5.1 + 20.0) / 5.1;

static const float ThermistorLookUpTable[][2] = {
  // {V, degrees C}
  {2.9, -5.0},
  {2.45, 15.0},
  {1.66, 43.0},
  {0.89, 75.0},
  {0.41, 111.0},
  {0.21, 150.0}
};

typedef enum
{
  OperatingModeOff,
  OperatingModeFullPower,
  OperatingModeReducedPower,
} OperatingMode;

#define PWM_PIN_COUNT 9
static const uint32_t PWMPins[PWM_PIN_COUNT] = {PinR1, PinG1, PinB1, PinW1, PinR2, PinG2, PinB2, PinW2, PinIR};
static const uint32_t PWMFreqHz = 40000;
static const uint32_t USBVBUSThreshold5V = 4000;
static const uint32_t USBVBUSThreshold9V = 8000;
static const uint8_t ReducedPowerLevelRGB = 153;
static const uint8_t ReducedPowerLevelSingle = 204;

static const uint32_t TickIntervalUs = 10000;
static const uint32_t ADCReportingIntervalMs = 200;

#define FLASH_PERSIST_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#define FLASH_PERSIST_ADDR_RGB_PRESET 0
#define FLASH_PERSIST_ADDR_TRIM 8
