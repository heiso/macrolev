idf_component_register(
  SRCS "main.c" "macrolev.c"
  INCLUDE_DIRS "."
  PRIV_REQUIRES esp_adc esp_timer esp_driver_gpio
)
