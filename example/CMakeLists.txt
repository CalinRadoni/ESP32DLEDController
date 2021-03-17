cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS
    ".."
    "$ENV{CodeLib_PATH}/ESP32/ESP32RMT")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(SUPPORTED_TARGETS esp32)

project(Example-ESP32DLEDController)
