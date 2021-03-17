/**
This file is part of ESP32DLEDController esp-idf component
(https://github.com/CalinRadoni/ESP32DLEDController)
Copyright (C) 2021 by Calin Radoni

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

#include "DStripData.h"

#include <new>

DStripData::DStripData(void)
{
    data = nullptr;
    dataLength = 0;
    stripLength = 0;
}

DStripData::~DStripData()
{
    Destroy();
}

bool DStripData::Create(uint16_t numberOfLEDs)
{
    Destroy();

    if (numberOfLEDs == 0) return false;

    stripLength = numberOfLEDs;
    dataLength = stripLength * bytesPerLED;
	data = new (std::nothrow) uint8_t[dataLength];
	if (data == nullptr) {
        dataLength = 0;
        stripLength = 0;
        return false;
	}

    return true;
}

void DStripData::Destroy(void)
{
    if (data != nullptr) {
        delete[] data;
        data = nullptr;
    }
    dataLength = 0;
    stripLength = 0;
}
