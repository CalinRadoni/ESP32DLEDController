cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS
    "../ESP32DLEDController"
    "../../ESP32RMT/ESP32RMT"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(Example-ESP32DLEDController)
