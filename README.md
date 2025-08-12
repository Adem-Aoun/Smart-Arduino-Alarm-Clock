# Smart Arduino Alarm Clock

A digital alarm clock with OLED display, RTC module, and intuitive controls.

## Features

- Real-time clock display with date
- Customizable alarm with audio alert
- OLED display (128x64)
- Button and potentiometer controls
- Visual LED indicators

## Hardware

- Arduino Uno/Nano
- DS3231 RTC Module
- SSD1306 OLED Display
- 4x Push Buttons
- Potentiometer
- Buzzer
- 2x LEDs

## Wiring

```
Pin 2  -> SET Button
Pin 3  -> UP Button  
Pin 4  -> DOWN Button
Pin 5  -> MODE Button
Pin 8  -> Status LED
Pin 9  -> Alarm LED
Pin 10 -> Buzzer
A0     -> Potentiometer
A4/A5  -> RTC & OLED (I2C)
```

## Libraries

```cpp
RTClib
Adafruit_SSD1306
Adafruit_GFX
Bounce2
```

## Usage

- **MODE**: Switch between clock/time setting/alarm setting
- **SET**: Confirm changes or stop alarm
- **UP/DOWN**: Adjust alarm time
- **Potentiometer**: Set time values

## Build

Clone and open in PlatformIO or Arduino IDE. Install required libraries and upload.

## License

Open Source - feel free to modify and share!
