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

#include "ESP32RMT.h"

#include <new>
#include "esp_log.h"
#include "driver/rmt.h"
#include "soc/rmt_struct.h"
#include "driver/gpio.h"

// -----------------------------------------------------------------------------

static const char *TAG  = "ESP32RMT";

/*
 * Standard APB clock (as needed by WiFi and BT to work) is 80 MHz.
 * This gives a 12.5 ns * rmt_config_t::clk_div for computing RMT durations, like this:
 *         duration = required_duration_in_ns / (12.5 * rmt_config_t::clk_div)
 */
const uint8_t  rmt_clk_divider  = 4;
const uint16_t rmt_clk_duration = 50; // ns,  12.5 * 4( = config.clk_div in rmt_config_for_digital_led_strip)

// -----------------------------------------------------------------------------

ESP32RMT::ESP32RMT(void)
{
	strip = nullptr;

    rmtLO.val = 0; rmtHI.val = 0;
    rmtLR.val = 0; rmtHR.val = 0;
    uglyBuffer = nullptr;
	uglyLen = 0;
}

ESP32RMT::~ESP32RMT(void)
{
	Destroy();
}

void ESP32RMT::Destroy(void)
{
	if (uglyBuffer != nullptr) {
		delete[] uglyBuffer;
		uglyBuffer = nullptr;
	}
	uglyLen = 0;

	strip = nullptr;
}

bool ESP32RMT::Create(DStrip* inStrip)
{
	Destroy();

	if (inStrip == nullptr) return true;

	strip = inStrip;

	if (strip->description.dataLen == 0) {
		strip = nullptr;
		ESP_LOGE(TAG, "strip length == 0");
		return false;
	}

    /* for every pixel are needed `8 * (strip's bytesPerLED)` bits
     * for every bit is needed a `rmt_item32_t` */
    uglyLen = strip->description.stripLen * 8 * strip->description.bytesPerLED;
	uglyBuffer = new (std::nothrow) rmt_item32_t[uglyLen];
	if (uglyBuffer == nullptr) {
		uglyLen = 0;
		strip = nullptr;
		ESP_LOGE(TAG, "uglyBuffer allocation failed !");
		return false;
    }

	rmtLO.level0 = 1;
	rmtLO.level1 = 0;
	rmtLO.duration0 = strip->description.T0H / rmt_clk_duration;
	rmtLO.duration1 = strip->description.T0L / rmt_clk_duration;

	rmtHI.level0 = 1;
	rmtHI.level1 = 0;
	rmtHI.duration0 = strip->description.T1H / rmt_clk_duration;
	rmtHI.duration1 = strip->description.T1L / rmt_clk_duration;

	rmtLR.level0 = 1;
	rmtLR.level1 = 0;
	rmtLR.duration0 = strip->description.T0H / rmt_clk_duration;
	rmtLR.duration1 = strip->description.TRS / rmt_clk_duration;

	rmtHR.level0 = 1;
	rmtHR.level1 = 0;
	rmtHR.duration0 = strip->description.T1H / rmt_clk_duration;
	rmtHR.duration1 = strip->description.TRS / rmt_clk_duration;

	return true;
}

void ESP32RMT::SetGPIO(void)
{
    gpio_pad_select_gpio(gpio_number);
    gpio_set_direction(  gpio_number, GPIO_MODE_OUTPUT);
    gpio_set_level(      gpio_number, 0);
}

bool ESP32RMT::Configure(gpio_num_t inGPIONumber, rmt_channel_t inChannel)
{

    gpio_number = inGPIONumber;
    channel = inChannel;

	SetGPIO();

    rmt_config_t config;

    config.rmt_mode = rmt_mode_t::RMT_MODE_TX;
    config.channel  = channel;
    config.clk_div  = rmt_clk_divider;
    config.gpio_num = gpio_number;

    /* One memory block is 64 words * 32 bits each; the type is rmt_item32_t (defined in rmt_struct.h).
    A channel can use more memory blocks by taking from the next channels, so channel 0 can have 8
    memory blocks and channel 7 just one. */
    config.mem_block_num = 1;

    config.tx_config.loop_en              = false;
    config.tx_config.carrier_en           = false;
    config.tx_config.carrier_freq_hz      = 0;
    config.tx_config.carrier_duty_percent = 0;
    config.tx_config.carrier_level        = rmt_carrier_level_t::RMT_CARRIER_LEVEL_LOW;
    config.tx_config.idle_level           = rmt_idle_level_t::RMT_IDLE_LEVEL_LOW;
    config.tx_config.idle_output_en       = true;

    // stop this rmt channel
    esp_err_t err;

	err = rmt_rx_stop(channel);
    if (err != ESP_OK) {
    	ESP_LOGE(TAG, "[0x%x] rmt_rx_stop failed", err);
    	return false;
    }
    err = rmt_tx_stop(channel);
    if (err != ESP_OK) {
    	ESP_LOGE(TAG, "[0x%x] rmt_tx_stop failed", err);
    	return false;
    }

    // disable rmt interrupts for this channel
    rmt_set_rx_intr_en(    channel, 0);
    rmt_set_err_intr_en(   channel, 0);
    rmt_set_tx_intr_en(    channel, 0);
    rmt_set_tx_thr_intr_en(channel, 0, 0xffff);
    // set rmt memory to normal (not power-down) mode
    err = rmt_set_mem_pd(channel, false);
    if (err != ESP_OK) {
    	ESP_LOGE(TAG, "[0x%x] rmt_set_mem_pd failed", err);
    	return false;
    }

    /* The rmt_config function internally:
     * - enables the RMT module by calling periph_module_enable(PERIPH_RMT_MODULE);
     * - sets data mode with rmt_set_data_mode(RMT_DATA_MODE_MEM);
     * - associates the gpio pin with the rmt channel using rmt_set_pin(channel, mode, gpio_num);
     */
    err = rmt_config(&config);
    if (err != ESP_OK) {
    	ESP_LOGE(TAG, "[0x%x] rmt_config failed", err);
    	return false;
    }

    err = rmt_driver_install(channel, 0, 0);
    if (err != ESP_OK) {
    	ESP_LOGE(TAG, "[0x%x] rmt_driver_install failed", err);
    	return false;
    }

    return true;
}

void ESP32RMT::Byte_to_rmtitem(uint8_t data, uint16_t idx)
{
	uint8_t mask = 0x80;

	while (mask != 0){
		uglyBuffer[idx++] =
			((data & mask) != 0) ? rmtHI : rmtLO;
		mask = mask >> 1;
	}
}

bool ESP32RMT::Send(void)
{
	if (uglyBuffer == nullptr)  return false;
	if (uglyLen == 0)           return false;
	if (strip == nullptr)       return false;

	uint8_t* pixelColor0 = strip->description.data;
	uint8_t* pixelColor1 = strip->description.data;
	uint8_t* pixelColor2 = strip->description.data;
	uint8_t* pixelColor3 = strip->description.data;
    uint8_t bpp = 0;

    switch (strip->description.colorType) {
        case DColorType::GRB:
    		pixelColor0++;
	    	pixelColor2 += 2;
            bpp = 3;
            break;
        case DColorType::GRBW:
            pixelColor0++;
            pixelColor2 += 2;
            pixelColor3 += 3;
            bpp = 4;
            break;

        default:
            bpp = 0;
            break;
    }
    if (bpp == 0 || bpp > 4) {
	    return false;
    }

	uint16_t didx = 0;
	for (uint16_t i = 0; i < strip->description.stripLen; i++) {
		Byte_to_rmtitem(*pixelColor0, didx);
        didx += 8;
        pixelColor0 += bpp;

        if (bpp > 1) {
		    Byte_to_rmtitem(*pixelColor1, didx);
            didx += 8;
            pixelColor1 += bpp;
        }
        if (bpp > 2) {
		    Byte_to_rmtitem(*pixelColor2, didx);
            didx += 8;
            pixelColor2 += bpp;
        }
		if (bpp > 3) {
			Byte_to_rmtitem(*pixelColor3, didx);
            didx += 8;
            pixelColor3 += bpp;
		}
	}

	// change last bit to include reset time
	didx--;
	if (uglyBuffer[didx].val == rmtHI.val) {
		uglyBuffer[didx] = rmtHR;
	}
	else {
		uglyBuffer[didx] = rmtLR;
	}

	esp_err_t err = rmt_write_items(channel, uglyBuffer, uglyLen, true);
	return (err == ESP_OK) ? true : false;
}
