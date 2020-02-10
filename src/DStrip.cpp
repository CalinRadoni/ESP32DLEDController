/**
This file is part of ESP32DLEDController esp-idf component
(https://github.com/CalinRadoni/ESP32DLEDController)
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

#include "DStrip.h"

#include <new>
#include <cstring>

DStrip::DStrip(void)
{
    description.stripLen = 0;

    description.data = nullptr;
    description.dataLen = 0;
    description.bytesPerLED = 0;

    description.geometry = DStripGeometry::Line;
    description.nrows = 0;
    description.ncols = 0;

    maxCCV = 0;
}

DStrip::~DStrip(void)
{
    Destroy();
}

void DStrip::Destroy(void)
{
    description.stripLen = 0;

    if (description.data != nullptr) {
        delete[] description.data;
        description.data = nullptr;
    }
    description.dataLen = 0;
    description.bytesPerLED = 0;

    description.geometry = DStripGeometry::Line;
}

bool DStrip::Create(uint8_t bytesPerLED, uint16_t stripLength, uint8_t maxccv)
{
    Destroy();

	if (stripLength == 0) return false;
	if (bytesPerLED == 0) return false;

    description.bytesPerLED = bytesPerLED;

    description.dataLen = description.bytesPerLED * stripLength;
	description.data = new (std::nothrow) uint8_t[description.dataLen];
	if (description.data == nullptr) {
        description.dataLen = 0;
        description.bytesPerLED = 0;
        return false;
	}

    memset(description.data, 0, description.dataLen);

    description.stripLen = stripLength;

    description.geometry = DStripGeometry::Line;
    description.nrows = 1;
    description.ncols = description.stripLen;

    maxCCV = maxccv;

    return true;
}


bool DStrip::SetGeometry(DStripGeometry geometry, uint16_t rows, uint16_t cols)
{
    if (rows == 0) return false;
    if (cols == 0) return false;

    description.geometry = geometry;
    description.nrows = rows;
    description.ncols = cols;

    return true;
}

void DStrip::SetPixel(uint16_t idx, uint8_t r, uint8_t g, uint8_t b)
{
    if (description.data == nullptr) return;
    if (description.bytesPerLED != 3) return;
    if (idx >= description.stripLen) return;

    uint8_t* pixel = description.data;
    pixel += idx * description.bytesPerLED;

    *pixel = r; pixel++;
    *pixel = g; pixel++;
    *pixel = b;
}

void DStrip::SetPixel(uint16_t idx, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    if (description.data == nullptr) return;
    if (description.bytesPerLED != 4) return;
    if (idx >= description.stripLen) return;

    uint8_t* pixel = description.data;
    pixel += idx * description.bytesPerLED;

    *pixel = r; pixel++;
    *pixel = g; pixel++;
    *pixel = b; pixel++;
    *pixel = w;
}

void DStrip::SetPixel(uint16_t idx, uint32_t color)
{
    if (description.data == nullptr) return;
    if (idx >= description.stripLen) return;

    uint8_t* pixel = description.data;
    pixel += idx * description.bytesPerLED;

    *pixel = (uint8_t) ((color >> 16) & 0x000000FF);
    if (description.bytesPerLED > 1) {
        pixel++;
        *pixel = (uint8_t) ((color >> 8) & 0x000000FF);
    }
    if (description.bytesPerLED > 2) {
        pixel++;
        *pixel = (uint8_t) (color & 0x000000FF);
    }
    if (description.bytesPerLED > 3) {
        pixel++;
        *pixel = (uint8_t) ((color >> 24) & 0x000000FF);
    }
}

void DStrip::SetPixel(uint16_t row, uint16_t col, uint8_t r, uint8_t g, uint8_t b)
{
    if (description.data == nullptr) return;
    if (description.bytesPerLED != 3) return;

    uint16_t idx = row * description.ncols + col;
    if (idx >= description.stripLen) return;

    uint8_t* pixel = description.data;
    pixel += idx * description.bytesPerLED;

    *pixel = r; pixel++;
    *pixel = g; pixel++;
    *pixel = b;
}

void DStrip::SetPixel(uint16_t row, uint16_t col, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    if (description.data == nullptr) return;
    if (description.bytesPerLED != 4) return;

    uint16_t idx = row * description.ncols + col;
    if (idx >= description.stripLen) return;

    uint8_t* pixel = description.data;
    pixel += idx * description.bytesPerLED;

    *pixel = r; pixel++;
    *pixel = g; pixel++;
    *pixel = b; pixel++;
    *pixel = w;
}

void DStrip::SetPixel(uint16_t row, uint16_t col, uint32_t color)
{
    if (description.data == nullptr) return;

    uint16_t idx = row * description.ncols + col;
    if (idx >= description.stripLen) return;

    uint8_t* pixel = description.data;
    pixel += idx * description.bytesPerLED;

    *pixel = (uint8_t) ((color >> 16) & 0x000000FF);
    if (description.bytesPerLED > 1) {
        pixel++;
        *pixel = (uint8_t) ((color >> 8) & 0x000000FF);
    }
    if (description.bytesPerLED > 2) {
        pixel++;
        *pixel = (uint8_t) (color & 0x000000FF);
    }
    if (description.bytesPerLED > 3) {
        pixel++;
        *pixel = (uint8_t) ((color >> 24) & 0x000000FF);
    }
}

/*
 * Maximum number of colors is: 6 * max_cc_val;
 * r max. g /   b 0
 * r \    g max
 * r 0    g max b /
 * r 0    g \   b max
 * r /    g 0   b max
 * r max  g 0   b \
 */
void DStrip::SetColorByIndex(uint16_t pixelIdx, uint16_t colorIdx)
{
    if (description.data == nullptr) return;
    if (pixelIdx >= description.stripLen) return;

    if (maxCCV == 0) {
        SetPixel(pixelIdx, 0, 0, 0);
        return;
    }

    uint8_t seq = (colorIdx / maxCCV) % 6;
    uint8_t idx =  colorIdx % maxCCV;

    switch (seq) {
        case 0: SetPixel(pixelIdx, maxCCV,       idx,          0);            break;
        case 1: SetPixel(pixelIdx, maxCCV - idx, maxCCV,       0);            break;
        case 2: SetPixel(pixelIdx, 0,            maxCCV,       idx);          break;
        case 3: SetPixel(pixelIdx, 0,            maxCCV - idx, maxCCV);       break;
        case 4: SetPixel(pixelIdx, idx,          0,            maxCCV);       break;
        case 5: SetPixel(pixelIdx, maxCCV,       0,            maxCCV - idx); break;
    }
}

void DStrip::RainbowStep(uint16_t step)
{
    if (description.data == nullptr) return;
    if (description.stripLen == 0) return;

    for (uint16_t idx = 0; idx < description.stripLen; idx++)
        SetColorByIndex(idx, idx + step);
}

void DStrip::MovePixel(uint16_t step)
{
    if (description.data == nullptr) return;
    if (description.stripLen == 0) return;

    uint8_t  seq = (step / description.stripLen) % 6;
    uint16_t idx =  step % description.stripLen;
    uint8_t r, g, b, c1, c2;

    c1 = maxCCV;
    c2 = maxCCV / 2;

    switch (seq) {
        case 0:  r = c1; g =  0; b =  0; break;
        case 1:  r =  0; g = c1; b =  0; break;
        case 2:  r =  0; g =  0; b = c1; break;
        case 3:  r = c2; g = c2; b =  0; break;
        case 4:  r =  0; g = c2; b = c2; break;
        default: r = c2; g =  0; b = c2; break;
    }

    if (seq & 0x01) {
        idx = description.stripLen - idx - 1;
    }

    uint8_t* pixel = description.data;
    for (uint16_t i = 0; i < description.stripLen; i++) {
        if (i == idx) {
            *pixel = r; pixel++;
            *pixel = g; pixel++;
            *pixel = b; pixel++;
        }
        else {
            *pixel = *pixel / 2; pixel++;
            *pixel = *pixel / 2; pixel++;
            *pixel = *pixel / 2; pixel++;
        }
    }
}
