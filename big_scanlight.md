# big scanlight info & user manual

## features
* Illuminated area dimensions: 108x134mm (4.25x5.25in)
* Suitable for use with 35mm, medium format, and 4x5 film
* 20 each of deep red (665nm), green (525nm), deep blue (455nm), and 95CRI white, and 16 infrared (850nm) LEDs mounted on aluminum PCB for optimal heat dissipation
* Up to 15EV brightness (1/500s at f/8 ISO 100) in RGB combined mode, 14EV in white mode
* Better light uniformity over medium format frame sizes than scanlight v2/v3
* Diffuser made from fingerprint- and scratch-resistant textured acrylic
* 3D printed ABS housing
* Fully controllable via USB with web app
* On/off and mode toggle buttons on light source for standalone usage
* Compatible with accessories made for Negative Supply 4x5" light sources
* Simple 35mm and medium format film carriers available
* Powered using any USB-C power source (at least 9V at 18W required for full brightness)
* Open source hardware and software

## instructions

![](<images/big_scanlight_diagram.svg>)

### basic operation

The light source is powered via the **left USB-C port**. A USB-C PD power source that can supply at least 2A at 9V (e.g. most USB-C phone, tablet, and laptop chargers) is required to operate at full brightness; however, the light will still operate at reduced brightness from any USB power source capable of delivering at least 2A at 5V (e.g. almost all USB chargers from the last 10 years.)

*It is not recommended to power the light from a USB port on a computer; a dedicated USB power supply should be used instead.*

Press the **upper button** to toggle the light on and off. Press the **lower button** to toggle between narrowband RGB and 95CRI white modes. The buttons work even when the light is not connected to a computer. The default RGB channel brightnesses in this mode can be configured from the GUI, as explained below.

To adjust color and brightness and automate the process of scanning red, green, and blue channels separately, the light source can be connected to a computer via the **right USB-C port** and controlled with the web app located [here](https://jackw01.github.io/scanlight/automation/app_bsl/dist/index.html). The web app requires a Chromium-based web browser.

In the **Manual Control** panel of the web app, the red, green, and blue channel brightnesses can be adjusted and the color channels can be turned on individually or together. In **RGB** (narrowband trichromatic) mode, the red, green, and blue LEDs are on at the same time. In **White** mode, only the 95CRI white LEDs are on. The red, green, blue, and infrared LEDs can also be turned on individually; the RGB channel brightness adjustments still apply in this case.

The **RGB Presets** panel of the web app allows RGB channel brightness settings to be stored to and loaded from web browser local storage, using the **Load**, **Create**, **Rename**, and **Delete** buttons.

The **Set as Default** and **Load Default** buttons refer to the default RGB preset, which is stored onboard the light source. This functionality is separate from presets stored in the web app and sets the RGB channel brightnesses used when the light is first powered on and when it is not connected to a computer.

### remote shutter release usage

The **3.5mm jack** can be connected to a camera so that the shutter can be remotely triggered from the web app. The length of the shutter trigger pulse and the delay after are both configurable. The default settings should work for most use cases. Operation in the **R,G,B** mode is shown in the timing diagram below; the other modes work similarly.

![](<images/shutter_timing_diagram.svg>)

A cable with a male 3.5mm plug (otherwise known as a headphone connector) on one end and a camera-specific connector on the other end is required to use this functionality. A wide variety of these cables are available from online retailers; I cannot vouch for the quality of any of them or verify that a specific cable will work, as wiring diagrams are almost never published by the manufacturers.

Because the 3.5mm jack is not electrically isolated from the USB ports, it is recommended out of an abundance of caution that the camera should not be connected to the same USB power supply or computer as the light source when using the remote shutter control function. For most cameras, doing so will not cause any issues, but because there is no formal standard for how remote shutter cables or connectors are wired, some cameras may exist that could be damaged by such a setup.

### firmware updates

The web app will automatically notify if a firmware update is available and show a button to put the device into firmware update mode. To manually enter firmware update mode, press and hold down the **DFU mode button** while connecting the **right USB-C port** to a computer. The light source will show up as a USB storage device, and a firmware binary file can be copied to this device to update the firmware.

### cleaning precautions

Cleaning the acrylic diffuser panel with cleaning solutions containing high concentrations of isopropanol, other organic solvents, or ammonia can cause crazing or cracking of the acrylic. If needed, use a cleaner meant for eyeglasses or a diluted household all-purpose cleaner like Simple Green. Do not spray liquids directly on the light source.

## technical details

### optical design

### PCBs

### firmware and web app
