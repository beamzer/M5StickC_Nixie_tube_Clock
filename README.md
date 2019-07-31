# Nixie tube clock running on a M5Stick-C
Origninal from [macsbug](https://macsbug.wordpress.com/2019/06/06/m5stickc-nixie-tube-clock/)
and put onto [github](https://github.com/McOrts/M5StickC_Nixie_tube_Clock) by [McOrts](https://github.com/McOrts) 

Mods by [me](https://github.com/beamzer):
- addedd a HH:MM watchface and removed the MM:SS watchface (but it's still there in the code)
- addedd blinking dots in most mode to indicate seconds
- addedd brightness control through the side button
- addedd a new mode which displays HH:MM and a small seconds bar below

-----------
This is a Nixie clock tube display which runs on a [M5Stick-C](https://www.banggood.com/search/m5stickc.html?p=6E2214592974201405G7) (C stands for colour display).
The link points to banggood where i bought mine and also shows some cheap add-on modules like a speaker, pir-sensor and temp/humidity sensor (link contains affiliate code)

<img src="https://github.com/beamzer/M5StickC_Nixie_tube_Clock/blob/master/images/M5Stick-C.jpg" width="250" align="right" />
This device is amazing because it's small, relatively cheap and packed with sensors to play with. It's based on an ESP32-Pico with a small LCD display of 80 × 160 pixel, an 85mAH battery and lots of sensors (see below for an overview). 

## Outcome

<img src="https://github.com/beamzer/M5StickC_Nixie_tube_Clock/blob/master/images/Nixie_Clock_M5StickC.gif" width="300"/>

Switch between watchfaces using the M5 switch. Some examples:  
<img src="https://github.com/beamzer/M5StickC_Nixie_tube_Clock/blob/master/images/Nixie_Clock_M5StickC_m1.gif" width="200"/>  
<img src="https://github.com/beamzer/M5StickC_Nixie_tube_Clock/blob/master/images/Nixie_Clock_M5StickC_m3.gif" width="200"/>  

## How to

<img src="hhttps://github.com/beamzer/M5StickC_Nixie_tube_Clock/blob/master/images/M5Stick_Arduino_IDE.PNG" width="300" align="right" />

Install the Arduino IDE and configure it for the M5StickC: [M5StickC Quick Start - Arduino Win](https://docs.m5stack.com/#/en/quick_start/m5stickc/m5stickc_quick_start_with_arduino_Windows?id=_2-copy-the-following-esp32-boards-manager-url-to-additional-boards-manager-urls-hit-ok) 

Now you only need to load the sketch, compile it and download it to the M5Stick whith the USB-C adaptor. 

### Update Nixie Tube images

<img src="hhttps://github.com/beamzer/M5StickC_Nixie_tube_Clock/blob/master/lcd-image-converter/lcd_convert_s.jpg" align="right" />

Execute the lcd-image-converter.exe include in this repository.
1. Png or Jpg is read from File->Open Menu. The image will be displayed when loaded. 
2. Select: Option->Conversions Menu. The top Preset: selects "Color R5G6B5". 
3. Select: Image tap and select “8 bit” for Block size :. Press "OK". 
4. The example "vfd_35x67_8.c" is output by "Convert ..." in File-Convert Menu. 
Know in advance the destination path. 
Tick: Import All Images You can also output all the images with “Convert ... all”. 
5. Edit .c converted file. You will get an error as it is, so fix it with your editor. 
- Change: "Static const uint8_t image_data_vfd_35x67 [4690] = {" at the top of the file deleting "image_data_" as result of "static const uint8_t vfd_35x67_8 [4690] = {". 
- At the bottom of delete de entire line: "const tImage vfd_35x67 = {image_data_vfd_35x67, 35, 67, 8};
- Save the example "vfd_35x67_8".c

### Hardware components
The [M5Stick-C](https://m5stack.com/products/stick-c) is a device with multiple components as processor, battery, sensors, switches (3) and two LEDs (on Red LED and an InfraRed LED. The processor features are:

|   |   |
|---|---|
|Microcontroler|ESP32 Pico|
|Frequency|240MHz|
|Flash|4MB|
|RAM|320KB|

- **I2C BM8563** : Realizes the RTC function. Address: 0x15. 
- **AXP192** :  power management IC. 
- **ST7735S** : LCD IPS Display 80 × 160 pixel, 0,96"
- **SH200Q** : IMU Movement and positioning 6 axis inertial sensor.
- **SPM1423** : PDM Micropohne
- **IR** : IR Transmitter

### Software components
- [Arduino IDE](https://www.hackster.io/arduino/products/arduino-ide?ref=project-8e87cc)
- ICONs files: [vfd_18x34.c](https://github.com/McOrts/M5StickC_Nixie_tube_Clock/blob/master/vfd_18x34.c), [vfd_35x67.c](https://github.com/McOrts/M5StickC_Nixie_tube_Clock/blob/master/vfd_35x67.c) <img src="https://github.com/McOrts/M5StickC_Nixie_tube_Clock/blob/master/images/vfd_35x67_5.png" align="left" />

