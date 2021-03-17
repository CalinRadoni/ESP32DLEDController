# ESP32 DLED Controller

![Build with ESP-IDF v4.1](https://github.com/CalinRadoni/ESP32DLEDController/workflows/Build%20with%20ESP-IDF%20v4.1/badge.svg)

Using the RMT peripheral of ESP32, controls *WS2812 type* digital LEDs: WS2812, WS2812B, WS2812C, WS2812D, WS2812S, WS2813 and WS2815. SK6812 should work with *WS281x* mode.

This implementation uses the RMT driver from `ESP-IDF` and uses one `RMT` channel 
for a LED strip **leaving the other channels free, under the control of the RMT driver**.
Here are some key points of this implementation:

- the control of the LEDs is separated from the control of the `RMT` peripheral;
- the other `RMT` channels can be used as needed;
- the `RMT` peripheral is used through `ESP-IDF`'s own driver.

Right now the negative point is that this code uses a lot of RAM, *because of the
way the driver and peripheral works*.
There is a way to add a hook in the driver's transmission code but considering the
multithreading architecture of FreeRTOS this may add jitter creating visual artifacts,
depending on application.

## Breaking changes

### DLEDController v2

- uses DStripData as input stream.
- DStrip class was obsoleted and removed.

See the `example` for simple usage.

## About timings

Timings are from datasheets.
According to [Worldsemi's datasheets](http://www.world-semi.com) there are a few timings sets:

- one for WS2812
- one for WS2812B, WS2812C, WS2812S, WS2813 and WS2815
- one for WS2812D

In [Tim's Blog (cpldcpu)](https://cpldcpu.wordpress.com) are some interesting investigations about timings of various digital LEDs.

## Dependencies

- [Espressif IoT Development Framework](https://github.com/espressif/esp-idf)
- [ESP32RMT](https://github.com/CalinRadoni/ESP32RMT)

## Example code

In the `example` directory is an application that should control 64 Digital LEDs of WS2812 type, connected to the GPIO pin 14 through a non-inverting level converter.

## Hardware details

The code ( with [ESP32RMT](https://github.com/CalinRadoni/ESP32RMT) ) was tested on the following boards:

- ESP32-DevKitC board with a level convertor made from two 74HCT1G04 inverters
- Dev33-DLED - uses SN74LV1T34 logic level shifter connected to an ESP32-WROOM-32 module
- Dev34-DLEDGrid - uses SN74LV1T34 logic level shifter connected to an ESP32-WROOM-32 module

and:

- a LED strip with 300 WS2812B digital LEDs + Mean Well EPS-65S-5 power source
- a 8x8 LED board with WS2812 digital LEDs + Sony VTC6 18650 Battery

## Development Environment

Currently uses the latest stable version of [Espressif IoT Development Framework](https://github.com/espressif/esp-idf), v4.1 as of December 2020.

Editing was done in [Visual Studio Code](https://code.visualstudio.com).

Version control with [Git](https://git-scm.com).

## License

This software and its documentation are released under the [GNU GPLv3](http://www.gnu.org/licenses/gpl-3.0.html) License. See the `LICENSE-GPLv3.txt` file.

## Note

This is a faster and more efficient version of the one published by me previously in the [esp32_digitalLEDs](https://github.com/CalinRadoni/esp32_digitalLEDs) repository.
