idf_component_register(
  SRCS "main.c" "keyboard.c" "config.c" "hid.c" "usb_descriptors.c"
  INCLUDE_DIRS "."
  PRIV_REQUIRES esp_adc esp_timer esp_driver_gpio
)
