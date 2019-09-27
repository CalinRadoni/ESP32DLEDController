# DLED Controller

Controls *WS2812B type* digital LEDs using the RMT peripheral of ESP32.

This implementation uses the RMT driver from `ESP-IDF` and uses one `RMT` channel 
for a LED strip **leaving the other channels free, under the control of the RMT driver**.
Here are some key points of this implementation:

- the control of the LEDs is separated from the control of the `RMT` peripheral;
- the other `RMT` channels can be used as needed;
- the `RMT` peripheral is used through `ESP-IDF`'s own driver.

Right now the whole negative point is that this code uses a lot of RAM, *because of the 
way the driver and peripheral works*, but this will change *if/when* the driver will be 
updated with a hook in transmission code.

## About timings

Timings are from datasheets.
According to [Worldsemi's datasheets](http://www.world-semi.com) there are a few timings sets:

- one for WS2812
- one for WS2812B, WS2812C, WS2812S, WS2813 and WS2815
- one for WS2812D

In [Tim's Blog (cpldcpu)](https://cpldcpu.wordpress.com) are some interesting investigations about timings of various digital LEDs.

## Dependencies

[Espressif IoT Development Framework](https://github.com/espressif/esp-idf).

## Hardware details

The code was tested on the following boards:

- ESP32-DevKitC board with a level convertor made from two 74HCT1G04 inverters
- Dev33-DLED - uses SN74LV1T34 logic level shifter connected to an ESP32-WROOM-32 module

and:

- a LED strip with 300 WS2812B digital LEDs
- Mean Well EPS-65S-5 power source

## Development Environment

Main tools used where [Espressif IoT Development Framework](https://github.com/espressif/esp-idf) and *ESP32 toolchain for Linux*
(see [Standard Setup of Toolchain for Linux](https://github.com/espressif/esp-idf/blob/master/docs/get-started/linux-setup.rst)
for the latest version).

Editing was done in [Visual Studio Code](https://code.visualstudio.com).

Version control with [Git](https://git-scm.com).

## License

`DLED Controller` software and documentation is released under the [GNU GPLv3](http://www.gnu.org/licenses/gpl-3.0.html) License. See the `LICENSE-GPLv3.txt` file.

## Note

This should be a faster and more efficient version of the one published by me previously in the (esp32_digitalLEDs)[https://github.com/CalinRadoni/esp32_digitalLEDs] repository.
