#include "pico/stdlib.h"

#include "hardware/flash.h"

#define FW_VERSION_ID 1

#if !defined(HW_VERSION_BSL1) && !defined(HW_VERSION_SL4)
#define HW_VERSION_BSL1
#endif

typedef enum
{
  OperatingModeOff,
  OperatingMode5V0P5A,
  OperatingMode5V1P5A,
  OperatingMode5V3A,
  OperatingMode9V,
} OperatingMode;

static const float ADCVPerCount = 3.3 / 4095.0;

static const float ThermistorLookUpTable[][2] = {
    // {V, degrees C}
    {2.9, -5.0},
    {2.45, 15.0},
    {1.66, 43.0},
    {0.89, 75.0},
    {0.41, 111.0},
    {0.21, 150.0}};

static const uint32_t PWMFreqHz = 40000;
static const uint32_t USBVBUSThreshold5V = 4400;
static const uint32_t USBVBUSThreshold9V = 7800;
static const uint32_t USBCCThresholdRdUSB = 200;
static const uint32_t USBCCThresholdRd1p5 = 660;
static const uint32_t USBCCThresholdRd3p0 = 1230;
static const uint32_t OverTemperatureThresholdMdegc = 80000;

static const uint32_t TickIntervalUs = 10000;
static const uint32_t ADCReportingIntervalMs = 200;

#define FLASH_PERSIST_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#define FLASH_PERSIST_ADDR_RGB_PRESET 0
#define FLASH_PERSIST_ADDR_TRIM 8

#ifdef HW_VERSION_BSL1

#define HW_VERSION_ID 0

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

#define PWM_PIN_COUNT 9
static const uint32_t PWMPins[PWM_PIN_COUNT] = {PinR1, PinG1, PinB1, PinW1, PinR2, PinG2, PinB2, PinW2, PinIR};

static const int8_t DefaultTrimR = 0;
static const int8_t DefaultTrimG = 0;
static const int8_t DefaultTrimB = 0;
static const int8_t DefaultTrimW = 17;

// LED driver power limits - lookup by operating mode
static const uint8_t PowerLimitRGB[5] = {0, 153, 153, 153, 255};
static const uint8_t PowerLimitSingleChannel[5] = {0, 204, 204, 204, 255};

static const float ADCVBUSSenseScale = (5.1 + 20.0) / 5.1;

#endif

#ifdef HW_VERSION_SL4

#define HW_VERSION_ID 1

static const uint32_t PinR1 = 2;
static const uint32_t PinG1 = 3;
static const uint32_t PinB1 = 4;
static const uint32_t PinW1 = 1;
static const uint32_t PinButton1 = 16; // top button
static const uint32_t PinButton2 = 17; // bottom button
static const uint32_t PinShutter = 25;
static const uint32_t PinADCGPIOOffset = 26;
static const uint32_t PinADCVBUSSense = 0;
static const uint32_t PinADCThermistor = 1;
static const uint32_t PinADCUSBCC1 = 2;
static const uint32_t PinADCUSBCC2 = 3;

#define PWM_PIN_COUNT 4
static const uint32_t PWMPins[PWM_PIN_COUNT] = {PinR1, PinG1, PinB1, PinW1};

static const int8_t DefaultTrimR = 0;
static const int8_t DefaultTrimG = 0;
static const int8_t DefaultTrimB = 0;
static const int8_t DefaultTrimW = 0;

// LED driver power limits - lookup by operating mode
// Decided not to limit power for 500mA/1.5A USB modes, since it may be too confusing for users
static const uint8_t PowerLimitRGB[5] = {0, 255, 255, 255, 255};
static const uint8_t PowerLimitSingleChannel[5] = {0, 255, 255, 255, 255};
//static const uint8_t PowerLimitRGB[5] = {0, 82, 234, 255, 255};
//static const uint8_t PowerLimitSingleChannel[5] = {0, 170, 255, 255, 255};

static const float ADCVBUSSenseScale = 2.0;

#endif
