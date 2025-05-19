idf_component_register(
  SRCS "main.c" "macrolev.c" "usb.c" "sensors.c" "config_storage.c"
  INCLUDE_DIRS "."
  PRIV_REQUIRES esp_adc esp_timer esp_driver_gpio spiffs json
)
