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

#include "DLEDController.h"

DLEDController::DLEDController(void)
{
    type = LEDType::notset;
    colorOrder = LEDColorOrder::Flat;
    bytesPerLED = 0;
}

DLEDController::~DLEDController(void)
{
    //
}

bool DLEDController::SetLEDType(LEDType newtype)
{
    bytesPerLED = 0; // bytesPerLED != 0 is the marker for initialization

    type = newtype;

    return SetProperties();
}

bool DLEDController::SetProperties(void)
{
    /* Timings are from datasheets. DLED_WS281x timings should be good.
     * See https://cpldcpu.wordpress.com for interesting investigations about timings. */

    switch (type) {
        case LEDType::WS2812:
            wsT0H = 350; wsT0L = 800; wsT1H = 700; wsT1L = 600; wsTRS = 50000;
            colorOrder = LEDColorOrder::GRB;
            bytesPerLED = 3;
            SetTimingsForRMT();
            break;

        case LEDType::WS2812B:
        case LEDType::WS2813:
        case LEDType::WS2815:
            wsT0H = 300; wsT0L = 1090; wsT1H = 1090; wsT1L = 320; wsTRS = 280000;
            colorOrder = LEDColorOrder::GRB;
            bytesPerLED = 3;
            SetTimingsForRMT();
            break;

        case LEDType::WS2812D:
            wsT0H = 400; wsT0L = 850; wsT1H = 800; wsT1L = 450; wsTRS = 50000;
            colorOrder = LEDColorOrder::GRB;
            bytesPerLED = 3;
            SetTimingsForRMT();
            break;

        case LEDType::WS281x:
            wsT0H = 400; wsT0L = 850; wsT1H = 850; wsT1L = 400; wsTRS = 50000;
            colorOrder = LEDColorOrder::GRB;
            bytesPerLED = 3;
            SetTimingsForRMT();
            break;

        default:
            wsT0H = 0; wsT0L = 0; wsT1H = 0; wsT1L = 0; wsTRS = 0;
            colorOrder = LEDColorOrder::Flat;
            bytesPerLED = 0;
            break;
    }

    return (bytesPerLED == 0) ? false : true;
}

void DLEDController::SetTimingsForRMT(void)
{
    // ESP32 RMT, as configured by ESP32RMTChannel.ConfigureForWS2812x uses a RMT clock duration of 50 ns.

    uint16_t rmt_clk_duration = 50;

	rmtLO.level0 = 1;
	rmtLO.level1 = 0;
	rmtLO.duration0 = wsT0H / rmt_clk_duration;
	rmtLO.duration1 = wsT0L / rmt_clk_duration;

	rmtHI.level0 = 1;
	rmtHI.level1 = 0;
	rmtHI.duration0 = wsT1H / rmt_clk_duration;
	rmtHI.duration1 = wsT1L / rmt_clk_duration;

	rmtLR.level0 = 1;
	rmtLR.level1 = 0;
	rmtLR.duration0 = wsT0H / rmt_clk_duration;
	rmtLR.duration1 = wsTRS / rmt_clk_duration;

	rmtHR.level0 = 1;
	rmtHR.level1 = 0;
	rmtHR.duration0 = wsT1H / rmt_clk_duration;
	rmtHR.duration1 = wsTRS / rmt_clk_duration;
}

void DLEDController::SetLEDs(uint8_t* data, uint16_t length, ESP32RMTChannel *channel)
{
    if (data == nullptr) return;
    if (length == 0) return;
    if (channel == nullptr) return;
    if (bytesPerLED == 0) return;

	uint8_t* pixelColor0 = data;
	uint8_t* pixelColor1 = data;
	uint8_t* pixelColor2 = data;
	uint8_t* pixelColor3 = data;

    switch (colorOrder) {
        case LEDColorOrder::GRB:
    		pixelColor0++;
	    	pixelColor2 += 2;
            break;
        case LEDColorOrder::GRBW:
            pixelColor0++;
            pixelColor2 += 2;
            pixelColor3 += 3;
            break;

        default:
            pixelColor0 = nullptr;
            break;
    }
    if (pixelColor0 == nullptr) {
	    return;
    }

    channelBuffer = channel->GetDataBuffer();
    if (channelBuffer == nullptr) return;
    channelBufLen = channel->GetDataBufferLen();
    if (channelBufLen == 0) return;

	channelIndex = 0;
	uint16_t i = 0;
    while (i < length) {
		                       SetRMTItemsFromByte(*pixelColor0);
        if (bytesPerLED > 1) { SetRMTItemsFromByte(*pixelColor1); }
        if (bytesPerLED > 2) { SetRMTItemsFromByte(*pixelColor2); }
		if (bytesPerLED > 3) { SetRMTItemsFromByte(*pixelColor3); }

        pixelColor0 += bytesPerLED;
        pixelColor1 += bytesPerLED;
        pixelColor2 += bytesPerLED;
        pixelColor3 += bytesPerLED;
                  i += bytesPerLED;
	}

	// change last bit to include reset time
	channelIndex--;
	if (channelBuffer[channelIndex].val == rmtHI.val) {
		channelBuffer[channelIndex] = rmtHR;
	}
	else {
		channelBuffer[channelIndex] = rmtLR;
	}

    channel->SendData();
}

void DLEDController::SetRMTItemsFromByte(uint8_t value)
{
	uint8_t mask = 0x80;

	while (mask != 0){
        if (channelIndex < channelBufLen)
            channelBuffer[channelIndex++] =
                ((value & mask) != 0) ? rmtHI : rmtLO;
		mask = mask >> 1;
	}
}

