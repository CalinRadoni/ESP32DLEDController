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

#ifndef DStripData_H
#define DStripData_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Creates a memory buffer for a LED Strip with 4 bytes / LED.
 */
class DStripData
{
public:
    DStripData(void);
    virtual ~DStripData();

    bool Create(uint16_t numberOfLEDs);
    void Destroy(void);

    uint8_t* Data(void) { return data; }
    uint16_t Length(void) { return length; }

protected:
    uint8_t *data;
    uint16_t length;
};

#endif
