# big scanlight control interface documentation

## USB connection

Scanlight enumerates as a USB Serial CDC device, which can be accessed by, for example, the WebSerial API or PySerial.

## communication protocol overview

This information is current as of firmware version 1.0.0 (ID 0 as reported by `PKT_D2H_FW_VERSION`.)

Both host-to-device and device-to-host communications use the same packet format. The data length byte may be zero, in which case it is the last byte in the packet.

| byte index | description |
|---|---|
| 0 | start byte (always 0xFE) |
| 1 | packet header |
| 2 | packet data length |
| >= 3 | packet data |

### valid packet headers

#### host-to-device

```
PKT_H2D_SET_COLOR = 0
PKT_H2D_GET_DEFAULT_RGB = 1
PKT_H2D_GET_FW_VERSION = 2
PKT_H2D_SHUTTER_PULSE = 3
PKT_H2D_DFU_MODE = 4
PKT_H2D_SET_TRIM = 5
PKT_H2D_GET_TRIM = 6
```

#### device-to-host

```
PKT_D2H_LED_TEMP = 1
PKT_D2H_VBUS = 2
PKT_D2H_FW_VERSION = 3
PKT_D2H_DEFAULT_RGB = 4
PKT_D2H_TRIM = 5
```

### web app initial communication flow

1. Host sends `PKT_H2D_GET_FW_VERSION` and waits for response. If device firmware version ID is less than the latest available version ID, notify user.
2. Host sends `PKT_H2D_GET_DEFAULT_RGB` and `PKT_H2D_GET_TRIM` and waits for response. Response values are used to populate GUI.

## packet reference

### host-to-device

#### PKT_H2D_SET_COLOR

|properties||
|---|---|
| header | 0 |
| data length | 6 |

| data byte index | description |
|---|---|
| 0 | red channel value (0 to 255) |
| 1 | green channel value (0 to 255) |
| 2 | blue channel value (0 to 255) |
| 3 | white channel value (0 to 255) |
| 4 | IR channel value (0 to 255) |
| 5 | save preset flag (0 to 1) |

Requests the device to update the LED driver state. The device firmware performs input validation, ensuring that maximum power is reduced if operating more than one channel at a time with an insufficient power supply and preventing the white and IR channels from being operated at the same time as the RGB channels. If the 'save preset flag' byte is set to 1, the red, green, and blue channel settings will be saved to nonvolatile memory as the default RGB setting which is used at power-on. The application software should only set this byte to 1 when absolutely needed, as the nonvolatile memory has a finite write cycle lifespan.

#### PKT_H2D_GET_DEFAULT_RGB

|properties||
|---|---|
| header | 1 |
| data length | 0 |

Requests the device to respond with `PKT_D2H_DEFAULT_RGB`.

#### PKT_H2D_GET_FW_VERSION

|properties||
|---|---|
| header | 2 |
| data length | 0 |

Requests the device to respond with `PKT_D2H_FW_VERSION`.

#### PKT_H2D_SHUTTER_PULSE

|properties||
|---|---|
| header | 3 |
| data length | 1 |

| data byte index | description |
|---|---|
| 0 | shutter pulse length in 10-millisecond units (0 to 255) |

Requests the device to output a camera shutter trigger pulse. Timing of pulse length is handled by the device firmware, and timing between pulses is to be handled by the application software.
With Fujifilm mirrorless cameras, the minimum shutter pulse length is around 300ms and the minimum pulse interval is around 1000ms in single-shot drive mode. Using a bracketing or continuous drive mode on the camera is necessary for faster shutter actuation. Experimentation with other camera brands is needed.

#### PKT_H2D_DFU_MODE

|properties||
|---|---|
| header | 4 |
| data length | 0 |

Requests the device to enter device firmware upgrade (DFU) mode. USB serial connection will be lost when this packet is processed by the device.

#### PKT_H2D_SET_TRIM

|properties||
|---|---|
| header | 5 |
| data length | 4 |

| data byte index | description |
|---|---|
| 0 | red channel value (-127 to 127 - two's complement format) |
| 1 | green channel value (-127 to 127 - two's complement format) |
| 2 | blue channel value (-127 to 127 - two's complement format) |
| 3 | white channel value (-127 to 127 - two's complement format) |

Requests the device to set the red, green, blue, and white channel trimming settings (the difference between the left and right side LED driver PWM levels). New trimming settings are automatically saved to nonvolatile memory when received.

#### PKT_H2D_GET_TRIM

|properties||
|---|---|
| header | 6 |
| data length | 0 |

Requests the device to respond with `PKT_D2H_TRIM`.

### device-to-host

#### PKT_D2H_LED_TEMP

|properties||
|---|---|
| header | 1 |
| data length | 4 |

| data byte index | description |
|---|---|
| 0-4 | LED temperature in millidegrees Celsius (32-bit signed integer in two's complement format) |

Telemetry packet sent automatically by the device every 200ms. *Note: not supported on rev1 hardware (driver PCB PN 25d902a)*

#### PKT_D2H_VBUS

|properties||
|---|---|
| header | 2 |
| data length | 4 |

| data byte index | description |
|---|---|
| 0-4 | power USB port VBUS voltage in millivolts (32-bit signed integer in two's complement format) |

Telemetry packet sent automatically by the device every 200ms. *Note: there is no need for the application software to do anything in response to this packet. USB port voltage changes are handled by the device firmware.*

#### PKT_D2H_FW_VERSION

|properties||
|---|---|
| header | 3 |
| data length | 4 |

| data byte index | description |
|---|---|
| 0-4 | device firmware version ID (32-bit signed integer in two's complement format) |

Reports the device firmware version ID.

#### PKT_D2H_DEFAULT_RGB

|properties||
|---|---|
| header | 4 |
| data length | 3 |

| data byte index | description |
|---|---|
| 0 | red channel value (0 to 255) |
| 1 | green channel value (0 to 255) |
| 2 | blue channel value (0 to 255) |

Reports the default RGB settings (used at power-on) stored in nonvolatile memory.

#### PKT_D2H_TRIM

|properties||
|---|---|
| header | 5 |
| data length | 4 |

| data byte index | description |
|---|---|
| 0 | red channel value (-127 to 127 - two's complement format) |
| 1 | green channel value (-127 to 127 - two's complement format) |
| 2 | blue channel value (-127 to 127 - two's complement format) |
| 3 | white channel value (-127 to 127 - two's complement format) |

Reports the current channel trimming settings stored in nonvolatile memory.
