/**
This file is part of ESP32DLEDController esp-idf component
(https://github.com/CalinRadoni/ESP32DLEDController)
Copyright (C) 2019 by Calin Radoni

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DLEDController_H
#define DLEDController_H

#include <stddef.h>
#include <stdint.h>
#include "DStripData.h"
#include "ESP32RMTChannel.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/**
 * @brief The type of digital LEDs.
 */
enum class LEDType
{
    notset,
    WS2812,
    WS2812B,
    WS2812D,
    WS2813,
    WS2815,
    WS281x     /*!< This value should work for all WS281* and clones */
};

/**
 * @brief The color order used by digital LEDs
 */
enum class LEDColorOrder
{
    Flat,           /*!< To send the bytes as they are */
    GRB,            /*!< WS281x, NeoPixels, ... */
    GRBW            /*!< RGBW digital LEDs using GRBW format */
};

// -----------------------------------------------------------------------------

class DLEDController
{
public:
    DLEDController(void);
    ~DLEDController(void);

    bool SetLEDType(LEDType newtype);

    void SetMutex(SemaphoreHandle_t);

    void SetLEDs(DStripData &stripData, ESP32RMTChannel &channel);

protected:
    LEDType type;
    SemaphoreHandle_t mutex;

    /**
     * @brief Set the properties based on LEDType
     */
    bool SetProperties(void);

    LEDColorOrder colorOrder;
    uint8_t       bytesPerLED;

    uint16_t wsT0H, wsT0L; /*!< timings of the ws communication protocol */
    uint16_t wsT1H, wsT1L; /*!< timings of the ws communication protocol */
    uint32_t wsTRS;        /*!< reset timing of the ws communication protocol */

    /**
     * @brief Set timings for ESP32's RMT peripheral
     *
     * ESP32 RMT, as configured by ESP32RMTChannel.ConfigureForWS2812x uses a RMT clock duration of 50 ns.
     */
    void SetTimingsForRMT(void);

	rmt_item32_t rmtLO, rmtHI; /*!< Values required for RMT to send 0 and 1 */
    rmt_item32_t rmtLR, rmtHR; /*!< Values required for RMT to send 0 and 1 including reset */

    uint32_t channelBufLen;
    rmt_item32_t *channelBuffer;
    uint32_t channelIndex;

    /**
     * @brief Set RMT items from byte
     *
     * This function sets 8 RMT items in `channelBuffer` starting with `channelIndex`.
     * `channelIndex` is increased accordingly.
     */
   	void SetRMTItemsFromByte(uint8_t value);

private:

};

#endif
