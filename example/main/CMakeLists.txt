set(COMPONENT_SRCS "main.cpp")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_PRIV_REQUIRES nvs_flash ESP32DLEDController)

register_component()
