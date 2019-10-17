/**
This file is part of DLEDController esp-idf component from
pax-devices (https://github.com/CalinRadoni/pax-devices)
Copyright (C) 2019+ by Calin Radoni

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

// TODO: git move DStrip in Pixels


#ifndef DStrip_H
#define DStrip_H

#include <stddef.h>
#include <stdint.h>

enum class DStripGeometry
{
    Line,   /*!< All the pixel are in a single line */
    Grid    /*!< The pixels are in a grid */
};

struct DStripDescription
{
    uint16_t   stripLen;    /*!< number of pixels in the strip */

    uint8_t*   data;        /*!< color values are kept here in RGB or RGBW order */
    uint16_t   dataLen;     /*!< number of bytes of data buffer */
	uint8_t    bytesPerLED; /*!< number of bytes per LED */

    DStripGeometry geometry;
    uint16_t       nrows;   /*!< number of rows */
    uint16_t       ncols;   /*!< number of columns */
};

class DStrip
{
public:
    DStrip(void);
    ~DStrip(void);

    /**
     * @brief Data regarding the strip of LEDs
     *
     * For increased usage speed this is declared here as public variable.
     */
    DStripDescription description;

    /**
     * @brief Create and set the strip data
     *
     * @param bytesPerLED The number of bytes for one digital LED.
     * @param stripLength The number of digital LEDs.
     * @param maxccv      The maximum value allowed for a color component.
     */
    bool Create(uint8_t bytesPerLED, uint16_t stripLength, uint8_t maxccv);

    /**
     * @brief Set the strip's geometry
     *
     * @param[in] geometry
     * @param[in] rows     Number of rows
     * @param[in] cols     Number of columns
     */
    bool SetGeometry(DStripGeometry geometry, uint16_t rows, uint16_t cols);

    /**
     * @brief Destroy the buffers and set strip's type to NULL
     */
    void Destroy(void);

    /**
     * @brief Set pixel's color using r, g and b components
     */
    void SetPixel(uint16_t idx, uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Set pixel's color using r, g, b and w components
     */
    void SetPixel(uint16_t idx, uint8_t r, uint8_t g, uint8_t b, uint8_t w);

    /**
     * @brief Set pixel's color using a WRGB 32-bit value
     */
    void SetPixel(uint16_t idx, uint32_t color);

    /**
     * @brief Set pixel's color using r, g and b components
     */
    void SetPixel(uint16_t row, uint16_t col, uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Set pixel's color using r, g, b and w components
     */
    void SetPixel(uint16_t row, uint16_t col, uint8_t r, uint8_t g, uint8_t b, uint8_t w);

    /**
     * @brief Set pixel's color using a WRGB 32-bit value
     */
    void SetPixel(uint16_t row, uint16_t col, uint32_t color);

    /**
     * @brief Set a pixel's color from a simple rainbow palette.
     *
     * Set a pixel's color to a color from a color palette.
     * The color palette is a simple rainbow made from r-g, g-b and b-r combinations
     * with a number of (6 * maxComponentVal) colors.
     *
     * @param[in]  pixelIdx  Index of pixel to be set.
     * @param[in]  colorIdx  Index of color in the color pallete.
     */
    void SetColorByIndex(uint16_t pixelIdx, uint16_t colorIdx);

    /**
     * @brief Set a rainbow style sequence
     *
     * Creates a rainbow slice. Internally this function calls SetColorByIndex.
     *
     * @param[in] step Index of rainbow seqence.
     *
     * @code{c}
     * uint16_t step = 0;
     * while (true) {
     *     RainbowStep(step++);
     *     ... // send the pixels to physical LEDs
     *     vTaskDelay(300 / portTICK_PERIOD_MS); // wait a little between sequences
     * }
     * @endcode
     */
    void RainbowStep(uint16_t step);

    /**
     * @brief Moves a pixel back and forth
     *
     * Moves a pixel back and forth changing its color through red, green and blue.
     * There are (6 * length) steps.
     *
     * @param[in] step The step of the moving sequence.
     *
     * @code{c}
     * uint16_t step = 0;
     * while (true) {
     *     MovePixel(step++);
     *     ... // send the pixels to LEDs
     *     vTaskDelay(300 / portTICK_PERIOD_MS); // wait a little between sequences
     * }
     * @endcode
     */
    void MovePixel(uint16_t step);

private:
	uint8_t maxCCV; /*!< maximum value allowed for a color component */

};

#endif
