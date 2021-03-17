/**
This file is part of ESP32DLEDController esp-idf component
(https://github.com/CalinRadoni/ESP32DLEDController)
Copyright (C) 2020 by Calin Radoni

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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "ESP32RMTChannel.h"
#include "DStripData.h"
#include "DLEDController.h"

#include "colorutils.h"

#include "sdkconfig.h"

const char* TAG = "Init";

static const uint8_t  cfgOutputPin = 14;    // the GPIO where LEDs are connected
static const uint8_t  cfgChannel   = 0;     // ESP32 RMT's channel [0 ... 7]
static const uint16_t cfgLEDcount  = 64;    // 64 LEDS

const uint8_t  cfgMaxCCV    = 32;    // maximum value allowed for color component

DStripData stripData;
DLEDController LEDcontroller;
ESP32RMTChannel rmtChannel;

extern "C" {

    void delay_ms(uint32_t ms)
    {
        if (ms != 0) {
            vTaskDelay(ms / portTICK_PERIOD_MS);
        }
    }

    static void DisplayTask(void *taskParameter) {
        stripData.Create(cfgLEDcount);
        rmtChannel.Initialize((rmt_channel_t)cfgChannel, (gpio_num_t)cfgOutputPin, cfgLEDcount * 24);
        rmtChannel.ConfigureForWS2812x();
        LEDcontroller.SetLEDType(LEDType::WS2812);

        uint16_t step = 0;
        while (step < 6 * stripData.StripLength()) {
            MovePixel(step);
            step++;
            LEDcontroller.SetLEDs(stripData, rmtChannel);
            delay_ms(20);
        }

        step = 0;
        for (;;) {
            RainbowStep(step);
            step++;
            LEDcontroller.SetLEDs(stripData, rmtChannel);
            delay_ms(20);
        }

        vTaskDelete(NULL);
    }

    void app_main() {
        esp_err_t err = nvs_flash_init();
        if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            err = nvs_flash_erase();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%d nvs_flash_erase", err);
            }
            else {
                err = nvs_flash_init();
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "%d nvs_flash_init", err);
                }
            }
        }

        TaskHandle_t xHandleDisplayTask = NULL;
        xTaskCreate(DisplayTask, "Display task", 2048, NULL, uxTaskPriorityGet(NULL) + 5, &xHandleDisplayTask);
        if (xHandleDisplayTask != NULL) {
            ESP_LOGI(TAG, "Display task created.");
        }
        else {
            ESP_LOGE(TAG, "Failed to create the display task !");
        }
    }
}
