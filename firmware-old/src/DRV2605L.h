#ifndef _DRV2605_H
#define _DRV2605_H

#include <stdio.h>

#define DRV2605_ADDR 0x5A ///< Device I2C address

#define DRV2605_REG_STATUS 0x00       ///< Status register
#define DRV2605_REG_MODE 0x01         ///< Mode register
#define DRV2605_MODE_INTTRIG 0x00     ///< Internal trigger mode
#define DRV2605_MODE_EXTTRIGEDGE 0x01 ///< External edge trigger mode
#define DRV2605_MODE_EXTTRIGLVL 0x02  ///< External level trigger mode
#define DRV2605_MODE_PWMANALOG 0x03   ///< PWM/Analog input mode
#define DRV2605_MODE_AUDIOVIBE 0x04   ///< Audio-to-vibe mode
#define DRV2605_MODE_REALTIME 0x05    ///< Real-time playback (RTP) mode
#define DRV2605_MODE_DIAGNOS 0x06     ///< Diagnostics mode
#define DRV2605_MODE_AUTOCAL 0x07     ///< Auto calibration mode

#define DRV2605_REG_RTPIN 0x02    ///< Real-time playback input register
#define DRV2605_REG_LIBRARY 0x03  ///< Waveform library selection register
#define DRV2605_REG_WAVESEQ1 0x04 ///< Waveform sequence register 1
#define DRV2605_REG_WAVESEQ2 0x05 ///< Waveform sequence register 2
#define DRV2605_REG_WAVESEQ3 0x06 ///< Waveform sequence register 3
#define DRV2605_REG_WAVESEQ4 0x07 ///< Waveform sequence register 4
#define DRV2605_REG_WAVESEQ5 0x08 ///< Waveform sequence register 5
#define DRV2605_REG_WAVESEQ6 0x09 ///< Waveform sequence register 6
#define DRV2605_REG_WAVESEQ7 0x0A ///< Waveform sequence register 7
#define DRV2605_REG_WAVESEQ8 0x0B ///< Waveform sequence register 8

#define DRV2605_REG_GO 0x0C          ///< Go register
#define DRV2605_REG_OVERDRIVE 0x0D   ///< Overdrive time offset register
#define DRV2605_REG_SUSTAINPOS 0x0E  ///< Sustain time offset, positive register
#define DRV2605_REG_SUSTAINNEG 0x0F  ///< Sustain time offset, negative register
#define DRV2605_REG_BREAK 0x10       ///< Brake time offset register
#define DRV2605_REG_AUDIOCTRL 0x11   ///< Audio-to-vibe control register
#define DRV2605_REG_AUDIOLVL 0x12    ///< Audio-to-vibe minimum input level register
#define DRV2605_REG_AUDIOMAX 0x13    ///< Audio-to-vibe maximum input level register
#define DRV2605_REG_AUDIOOUTMIN 0x14 ///< Audio-to-vibe minimum output drive register
#define DRV2605_REG_AUDIOOUTMAX 0x15 ///< Audio-to-vibe maximum output drive register
#define DRV2605_REG_RATEDV 0x16      ///< Rated voltage register
#define DRV2605_REG_CLAMPV 0x17      ///< Overdrive clamp voltage register
#define DRV2605_REG_AUTOCALCOMP 0x18 ///< Auto-calibration compensation result register
#define DRV2605_REG_AUTOCALEMP 0x19  ///< Auto-calibration back-EMF result register
#define DRV2605_REG_FEEDBACK 0x1A    ///< Feedback control register
#define DRV2605_REG_CONTROL1 0x1B    ///< Control1 Register
#define DRV2605_REG_CONTROL2 0x1C    ///< Control2 Register
#define DRV2605_REG_CONTROL3 0x1D    ///< Control3 Register
#define DRV2605_REG_CONTROL4 0x1E    ///< Control4 Register
#define DRV2605_REG_VBAT 0x21        ///< Vbat voltage-monitor register
#define DRV2605_REG_LRARESON 0x22    ///< LRA resonance-period register

// Effects:
// 1 − Strong Click - 100%
// 2 − Strong Click - 60%
// 3 − Strong Click - 30%
// 4 − Sharp Click - 100%
// 5 − Sharp Click - 60%
// 6 − Sharp Click - 30%
// 7 − Soft Bump - 100%
// 8 − Soft Bump - 60%
// 9 − Soft Bump - 30%
// 10 − Double Click - 100%
// 11 − Double Click - 60%
// 12 − Triple Click - 100%
// 13 − Soft Fuzz - 60%
// 14 − Strong Buzz - 100%
// 15 − 750 ms Alert 100%
// 16 − 1000 ms Alert 100%
// 17 − Strong Click 1 - 100%
// 18 − Strong Click 2 - 80%
// 19 − Strong Click 3 - 60%
// 20 − Strong Click 4 - 30%
// 21 − Medium Click 1 - 100%
// 22 − Medium Click 2 - 80%
// 23 − Medium Click 3 - 60%
// 24 − Sharp Tick 1 - 100%
// 25 − Sharp Tick 2 - 80%
// 26 − Sharp Tick 3 – 60%
// 27 − Short Double Click Strong 1 – 100%
// 28 − Short Double Click Strong 2 – 80%
// 29 − Short Double Click Strong 3 – 60%
// 30 − Short Double Click Strong 4 – 30%
// 31 − Short Double Click Medium 1 – 100%
// 32 − Short Double Click Medium 2 – 80%
// 33 − Short Double Click Medium 3 – 60%
// 34 − Short Double Sharp Tick 1 – 100%
// 35 − Short Double Sharp Tick 2 – 80%
// 36 − Short Double Sharp Tick 3 – 60%
// 37 − Long Double Sharp Click Strong 1 – 100%
// 38 − Long Double Sharp Click Strong 2 – 80%
// 39 − Long Double Sharp Click Strong 3 – 60%
// 40 − Long Double Sharp Click Strong 4 – 30%
// 41 − Long Double Sharp Click Medium 1 – 100%
// 42 − Long Double Sharp Click Medium 2 – 80%
// 43 − Long Double Sharp Click Medium 3 – 60%
// 44 − Long Double Sharp Tick 1 – 100%
// 45 − Long Double Sharp Tick 2 – 80%
// 46 − Long Double Sharp Tick 3 – 60%
// 47 − Buzz 1 – 100%
// 48 − Buzz 2 – 80%
// 49 − Buzz 3 – 60%
// 50 − Buzz 4 – 40%
// 51 − Buzz 5 – 20%
// 52 − Pulsing Strong 1 – 100%
// 53 − Pulsing Strong 2 – 60%
// 54 − Pulsing Medium 1 – 100%
// 55 − Pulsing Medium 2 – 60%
// 56 − Pulsing Sharp 1 – 100%
// 57 − Pulsing Sharp 2 – 60%
// 58 − Transition Click 1 – 100%
// 59 − Transition Click 2 – 80%
// 60 − Transition Click 3 – 60%
// 61 − Transition Click 4 – 40%
// 62 − Transition Click 5 – 20%
// 63 − Transition Click 6 – 10%
// 64 − Transition Hum 1 – 100%
// 65 − Transition Hum 2 – 80%
// 66 − Transition Hum 3 – 60%
// 67 − Transition Hum 4 – 40%
// 68 − Transition Hum 5 – 20%
// 69 − Transition Hum 6 – 10%
// 70 − Transition Ramp Down Long Smooth 1 – 100 to 0%
// 71 − Transition Ramp Down Long Smooth 2 – 100 to 0%
// 72 − Transition Ramp Down Medium Smooth 1 – 100 to 0%
// 73 − Transition Ramp Down Medium Smooth 2 – 100 to 0%
// 74 − Transition Ramp Down Short Smooth 1 – 100 to 0%
// 75 − Transition Ramp Down Short Smooth 2 – 100 to 0%
// 76 − Transition Ramp Down Long Sharp 1 – 100 to 0%
// 77 − Transition Ramp Down Long Sharp 2 – 100 to 0%
// 78 − Transition Ramp Down Medium Sharp 1 – 100 to 0%
// 79 − Transition Ramp Down Medium Sharp 2 – 100 to 0%
// 80 − Transition Ramp Down Short Sharp 1 – 100 to 0%
// 81 − Transition Ramp Down Short Sharp 2 – 100 to 0%
// 82 − Transition Ramp Up Long Smooth 1 – 0 to 100%
// 83 − Transition Ramp Up Long Smooth 2 – 0 to 100%
// 84 − Transition Ramp Up Medium Smooth 1 – 0 to 100%
// 85 − Transition Ramp Up Medium Smooth 2 – 0 to 100%
// 86 − Transition Ramp Up Short Smooth 1 – 0 to 100%
// 87 − Transition Ramp Up Short Smooth 2 – 0 to 100%
// 88 − Transition Ramp Up Long Sharp 1 – 0 to 100%
// 89 − Transition Ramp Up Long Sharp 2 – 0 to 100%
// 90 − Transition Ramp Up Medium Sharp 1 – 0 to 100%
// 91 − Transition Ramp Up Medium Sharp 2 – 0 to 100%
// 92 − Transition Ramp Up Short Sharp 1 – 0 to 100%
// 93 − Transition Ramp Up Short Sharp 2 – 0 to 100%
// 94 − Transition Ramp Down Long Smooth 1 – 50 to 0%
// 95 − Transition Ramp Down Long Smooth 2 – 50 to 0%
// 96 − Transition Ramp Down Medium Smooth 1 – 50 to 0%
// 97 − Transition Ramp Down Medium Smooth 2 – 50 to 0%
// 98 − Transition Ramp Down Short Smooth 1 – 50 to 0%
// 99 − Transition Ramp Down Short Smooth 2 – 50 to 0%
// 100 − Transition Ramp Down Long Sharp 1 – 50 to 0%
// 101 − Transition Ramp Down Long Sharp 2 – 50 to 0%
// 102 − Transition Ramp Down Medium Sharp 1 – 50 to 0%
// 103 − Transition Ramp Down Medium Sharp 2 – 50 to 0%
// 104 − Transition Ramp Down Short Sharp 1 – 50 to 0%
// 105 − Transition Ramp Down Short Sharp 2 – 50 to 0%
// 106 − Transition Ramp Up Long Smooth 1 – 0 to 50%
// 107 − Transition Ramp Up Long Smooth 2 – 0 to 50%
// 108 − Transition Ramp Up Medium Smooth 1 – 0 to 50%
// 109 − Transition Ramp Up Medium Smooth 2 – 0 to 50%
// 110 − Transition Ramp Up Short Smooth 1 – 0 to 50%
// 111 − Transition Ramp Up Short Smooth 2 – 0 to 50%
// 112 − Transition Ramp Up Long Sharp 1 – 0 to 50%
// 113 − Transition Ramp Up Long Sharp 2 – 0 to 50%
// 114 − Transition Ramp Up Medium Sharp 1 – 0 to 50%
// 115 − Transition Ramp Up Medium Sharp 2 – 0 to 50%
// 116 − Transition Ramp Up Short Sharp 1 – 0 to 50%
// 117 − Transition Ramp Up Short Sharp 2 – 0 to 50%
// 118 − Long buzz for programmatic stopping – 100%
// 119 − Smooth Hum 1 (No kick or brake pulse) – 50%
// 120 − Smooth Hum 2 (No kick or brake pulse) – 40%
// 121 − Smooth Hum 3 (No kick or brake pulse) – 30%
// 122 − Smooth Hum 4 (No kick or brake pulse) – 20%
// 123 − Smooth Hum 5 (No kick or brake pulse) – 10%

/**************************************************************************/
/*!
  @brief The DRV2605 driver class.
*/
/**************************************************************************/
void drv2605l_init();
void writeRegister8(uint8_t reg, uint8_t val);
uint8_t readRegister8(uint8_t reg);
void drv2605l_setWaveform(uint8_t slot, uint8_t w);
void drv2605l_selectLibrary(uint8_t lib);
void drv2605l_go(void);
void drv2605l_stop(void);
void drv2605l_setMode(uint8_t mode);
void drv2605l_setRealtimeValue(uint8_t rtp);
// Select ERM (Eccentric Rotating Mass) or LRA (Linear Resonant Actuator)
// vibration motor The default is ERM, which is more common
void drv2605l_useERM();
void drv2605l_useLRA();

#endif
