// Copyright 2024 [Your Name]
// Supports:
//   Brand: Mitsubishi,  Model: PJZ502A030G remote

#ifndef IR_MITSUBISHI_PJZ_H_
#define IR_MITSUBISHI_PJZ_H_

#include <Arduino.h>  // Add this at the top for String type
#include "IRremoteESP8266.h"
#include "IRsend.h"
#ifdef UNIT_TEST
#include "IRsend_test.h"
#endif

// Protocol-specific constants
const uint8_t kMitsubishiAC_PJZ502PowerByte = 5;
const uint8_t kMitsubishiAC_PJZ502PowerOffset = 5;  // 0b00100000
const uint8_t kMitsubishiAC_PJZ502ModeByte = 6;
const uint8_t kMitsubishiAC_PJZ502TempByte = 7;
const uint8_t kMitsubishiAC_PJZ502FanByte = 9;
const uint8_t kMitsubishiAC_PJZ502SwingVByte = 10;

// Size constants
const uint8_t kMitsubishiAC_PJZ502ModeBitsSize = 3;  // Bits
const uint8_t kMitsubishiAC_PJZ502FanSize = 4;  // Bits
const uint8_t kMitsubishiAC_PJZ502SwingVSize = 4;  // Bits
const uint8_t kMitsubishiAC_PJZ502SwingHSize = 4;  // Bits

// Mode values
const uint8_t kMitsubishiAC_PJZ502Auto = 0x4;
const uint8_t kMitsubishiAC_PJZ502Cool = 0x1;
const uint8_t kMitsubishiAC_PJZ502Heat = 0x2;
const uint8_t kMitsubishiAC_PJZ502Dry = 0x3;
const uint8_t kMitsubishiAC_PJZ502Fan = 0x5;

// Fan speeds
const uint8_t kMitsubishiAC_PJZ502FanAuto = 0x0;
const uint8_t kMitsubishiAC_PJZ502FanHigh = 0x1;
const uint8_t kMitsubishiAC_PJZ502FanMed = 0x2;
const uint8_t kMitsubishiAC_PJZ502FanLow = 0x3;
const uint8_t kMitsubishiAC_PJZ502FanTop = 0x4;

// Vertical swing positions
const uint8_t kMitsubishiAC_PJZ502SwingVAuto = 0x0;
const uint8_t kMitsubishiAC_PJZ502SwingVHighest = 0x1;
const uint8_t kMitsubishiAC_PJZ502SwingVHigh = 0x2;
const uint8_t kMitsubishiAC_PJZ502SwingVLow = 0x3;
const uint8_t kMitsubishiAC_PJZ502SwingVLowest = 0x4;

// Temperature limits
const uint8_t kMitsubishiAC_PJZ502MinTemp = 16;  // 16C
const uint8_t kMitsubishiAC_PJZ502MaxTemp = 31;  // 31C

const uint8_t kMitsubishiAC_PJZ502TurboByte = 11;
const uint8_t kMitsubishiAC_PJZ502TurboOffset = 5;
const uint8_t kMitsubishiAC_PJZ502LightByte = 12;
const uint8_t kMitsubishiAC_PJZ502LightOffset = 5;
const uint8_t kMitsubishiAC_PJZ502SleepByte = 13;
const uint8_t kMitsubishiAC_PJZ502SleepOffset = 5;

class IRMitsubishiPJZ {
 public:
  explicit IRMitsubishiPJZ(uint16_t pin, bool inverted = false, bool use_modulation = true);
  void send(uint16_t repeat = kMitsubishiAC_PJZ502MinRepeat);
  void begin();
  void setPower(bool state);
  bool getPower();
  void setTemp(uint8_t temp);
  uint8_t getTemp();
  void setFan(uint8_t speed);
  uint8_t getFan();
  void setMode(uint8_t mode);
  uint8_t getMode();
  void setSwingV(uint8_t position);
  uint8_t getSwingV();
  uint8_t* getRaw();
  void setRaw(uint8_t* data);
  static bool validChecksum(uint8_t* state, const uint16_t length);
  stdAc::state_t toCommon();
  String toString();
  static uint8_t convertMode(const stdAc::opmode_t mode);
  static uint8_t convertFan(const stdAc::fanspeed_t speed);
  static uint8_t convertSwingV(const stdAc::swingv_t position);
  static uint8_t convertSwingH(const stdAc::swingh_t position);
  static uint8_t convertTurbo(const bool turbo);
  static uint8_t convertLight(const bool light);
  static uint8_t convertSleep(const bool sleep);
  static uint8_t convertClock(const int16_t clock);

  void setSwingH(uint8_t position);
  void setTurbo(bool turbo);
  void setLight(bool light);
  void setSleep(bool sleep);
  void setClock(int16_t clock);
  // Add these static methods
  static bool getPowerFromState(const uint8_t* state);
  static uint8_t getModeFromState(const uint8_t* state);
  static uint8_t getTempFromState(const uint8_t* state);
  static uint8_t getFanFromState(const uint8_t* state);
  static uint8_t getSwingVFromState(const uint8_t* state);
  static String toString(const uint8_t* state);
  static stdAc::opmode_t toCommonMode(const uint8_t mode);
  static stdAc::fanspeed_t toCommonFanSpeed(const uint8_t speed);
  static stdAc::swingv_t toCommonSwingV(const uint8_t pos);


 private:
#ifndef UNIT_TEST
  IRsend _irsend;
#else
  IRsendTest _irsend;
#endif
  uint8_t remote_state[kMitsubishiAC_PJZ502StateLength];
  void stateReset();
  void checksum();
};

#endif  // IR_MITSUBISHI_PJZ_H_ 