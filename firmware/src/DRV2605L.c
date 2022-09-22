#include "DRV2605L.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pin_config.h"
#include <stdio.h>

/**************************************************************************/
/*!
  @brief Read an 8-bit register.
  @param reg The register to read.
  @return 8-bit value of the register.
*/
/**************************************************************************/
uint8_t readRegister8(uint8_t reg) {
  uint8_t buffer[1] = {reg};
  i2c_write_blocking(I2C_INSTANCE, DRV2605_ADDR, buffer, 1, true);
  i2c_read_blocking(I2C_INSTANCE, DRV2605_ADDR, buffer, 1, false);
  return buffer[0];
}

/**************************************************************************/
/*!
  @brief Write an 8-bit register.
  @param reg The register to write.
  @param val The value to write.
*/
/**************************************************************************/
void writeRegister8(uint8_t reg, uint8_t val) {
  uint8_t buffer[2] = {reg, val};
  i2c_write_blocking(I2C_INSTANCE, DRV2605_ADDR, buffer, 2, false);
}

/**************************************************************************/
/*!
  @brief Select the haptic waveform to use.
  @param slot The waveform slot to set, from 0 to 7
  @param w The waveform sequence value, refers to an index in the ROM library.

    Playback starts at slot 0 and continues through to slot 7, stopping if it
  encounters a value of 0. A list of available waveforms can be found in
  section 11.2 of the datasheet: http://www.adafruit.com/datasheets/DRV2605.pdf
*/
/**************************************************************************/
void drv2605l_setWaveform(uint8_t slot, uint8_t w) {
  writeRegister8(DRV2605_REG_WAVESEQ1 + slot, w);
}

/**************************************************************************/
/*!
  @brief Select the waveform library to use.
  @param lib Library to use, 0 = Empty, 1-5 are ERM, 6 is LRA.

    See section 7.6.4 in the datasheet for more details:
  http://www.adafruit.com/datasheets/DRV2605.pdf
*/
/**************************************************************************/
void drv2605l_selectLibrary(uint8_t lib) {
  writeRegister8(DRV2605_REG_LIBRARY, lib);
}

/**************************************************************************/
/*!
  @brief Start playback of the waveforms (start moving!).
*/
/**************************************************************************/
void drv2605l_go() { writeRegister8(DRV2605_REG_GO, 1); }

/**************************************************************************/
/*!
  @brief Stop playback.
*/
/**************************************************************************/
void drv2605l_stop() { writeRegister8(DRV2605_REG_GO, 0); }

/**************************************************************************/
/*!
  @brief Set the device mode.
  @param mode Mode value, see datasheet section 7.6.2:
  http://www.adafruit.com/datasheets/DRV2605.pdf

    0: Internal trigger, call go() to start playback\n
    1: External trigger, rising edge on IN pin starts playback\n
    2: External trigger, playback follows the state of IN pin\n
    3: PWM/analog input\n
    4: Audio\n
    5: Real-time playback\n
    6: Diagnostics\n
    7: Auto calibration
*/
/**************************************************************************/
void drv2605l_setMode(uint8_t mode) {
  writeRegister8(DRV2605_REG_MODE, mode);
}

/**************************************************************************/
/*!
  @brief Set the realtime value when in RTP mode, used to directly drive the
  haptic motor.
  @param rtp 8-bit drive value.
*/
/**************************************************************************/
void drv2605l_setRealtimeValue(uint8_t rtp) {
  writeRegister8(DRV2605_REG_RTPIN, rtp);
}

/**************************************************************************/
/*!
  @brief Use ERM (Eccentric Rotating Mass) mode.
*/
/**************************************************************************/
void drv2605l_useERM() {
  writeRegister8(DRV2605_REG_FEEDBACK,
                 readRegister8(DRV2605_REG_FEEDBACK) & 0x7F);
}

/**************************************************************************/
/*!
  @brief Use LRA (Linear Resonance Actuator) mode.
*/
/**************************************************************************/
void drv2605l_useLRA() {
  writeRegister8(DRV2605_REG_FEEDBACK,
                 readRegister8(DRV2605_REG_FEEDBACK) | 0x80);
}

void drv2605l_init() {
  i2c_init(I2C_INSTANCE, 100 * 1000);
  gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(PIN_I2C_SDA);
  gpio_pull_up(PIN_I2C_SCL);

  sleep_us(250);
  uint8_t id = readRegister8(DRV2605_REG_STATUS);
  // Serial.print("Status 0x"); Serial.println(id, HEX);

  writeRegister8(DRV2605_REG_MODE, 0x00); // out of standby

  writeRegister8(DRV2605_REG_RTPIN, 0x00); // no real-time-playback

  writeRegister8(DRV2605_REG_WAVESEQ1, 1); // strong click
  writeRegister8(DRV2605_REG_WAVESEQ2, 0); // end sequence

  writeRegister8(DRV2605_REG_OVERDRIVE, 0); // no overdrive

  writeRegister8(DRV2605_REG_SUSTAINPOS, 0);
  writeRegister8(DRV2605_REG_SUSTAINNEG, 0);
  writeRegister8(DRV2605_REG_BREAK, 0);
  writeRegister8(DRV2605_REG_AUDIOMAX, 0x64);

  // ERM open loop

  // turn off N_ERM_LRA
  writeRegister8(DRV2605_REG_FEEDBACK,
                 readRegister8(DRV2605_REG_FEEDBACK) & 0x7F);
  // turn on ERM_OPEN_LOOP
  writeRegister8(DRV2605_REG_CONTROL3,
                 readRegister8(DRV2605_REG_CONTROL3) | 0x20);
}
