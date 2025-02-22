// Copyright 2024 [Your Name]
#include "ir_MitsubishiPJZ.h"
#include <algorithm>
#include "IRremoteESP8266.h"
#include "IRtext.h"
#include "IRutils.h"
#include "IRrecv.h"
#include "IRsend.h"

IRMitsubishiPJZ::IRMitsubishiPJZ(uint16_t pin, bool inverted, bool use_modulation)
    : _irsend(pin, inverted, use_modulation) {
  stateReset();
}

void IRMitsubishiPJZ::stateReset() {
  uint8_t reset[kMitsubishiAC_PJZ502StateLength] = {
      0x23, 0xCB, 0x26, 0x01, 0x00, 0x20, 0x08, 0x06, 
      0x30, 0x45, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x1A}; // Example initial state
  memcpy(remote_state, reset, kMitsubishiAC_PJZ502StateLength);
}

void IRMitsubishiPJZ::send(uint16_t repeat) {
  checksum();
  _irsend.sendMitsubishiAC_PJZ502(remote_state, kMitsubishiAC_PJZ502StateLength, repeat);
}

void IRMitsubishiPJZ::begin() {
  _irsend.begin();
}

void IRsend::sendMitsubishiAC_PJZ502(const uint8_t data[], 
                                    const uint16_t nbytes,
                                    const uint16_t repeat) {
  sendGeneric(kMitsubishiAC_PJZ502HdrMark, kMitsubishiAC_PJZ502HdrSpace,
              kMitsubishiAC_PJZ502BitMark, kMitsubishiAC_PJZ502OneSpace,
              kMitsubishiAC_PJZ502BitMark, kMitsubishiAC_PJZ502ZeroSpace,
              kMitsubishiAC_PJZ502BitMark, kMitsubishiAC_PJZ502Gap,
              data, nbytes, 38, false, repeat, 50);
}


void IRMitsubishiPJZ::setPower(bool state) {
  irutils::setBit(&remote_state[kMitsubishiAC_PJZ502PowerByte], 
                  kMitsubishiAC_PJZ502PowerOffset, state);
}

void IRMitsubishiPJZ::setTemp(uint8_t temp) {
  temp = std::min(std::max(temp, kMitsubishiAC_PJZ502MinTemp),
                  kMitsubishiAC_PJZ502MaxTemp);
  remote_state[kMitsubishiAC_PJZ502TempByte] = temp - kMitsubishiAC_PJZ502MinTemp;
  checksum();
}

uint8_t IRMitsubishiPJZ::getTemp() {
  return remote_state[kMitsubishiAC_PJZ502TempByte] + kMitsubishiAC_PJZ502MinTemp;
}

void IRMitsubishiPJZ::setMode(uint8_t mode) {
  irutils::setBits(&remote_state[kMitsubishiAC_PJZ502ModeByte], kLowNibble, 
                   kMitsubishiAC_PJZ502ModeBitsSize, mode);
}

void IRMitsubishiPJZ::setFan(uint8_t speed) {
  irutils::setBits(&remote_state[kMitsubishiAC_PJZ502FanByte], kLowNibble,
                   kMitsubishiAC_PJZ502FanSize, speed);
}

void IRMitsubishiPJZ::setSwingV(uint8_t position) {
  irutils::setBits(&remote_state[kMitsubishiAC_PJZ502SwingVByte], kLowNibble,
                   kMitsubishiAC_PJZ502SwingVSize, position);
}

void IRMitsubishiPJZ::setSwingH(uint8_t position) {
  irutils::setBits(&remote_state[kMitsubishiAC_PJZ502SwingVSize], kLowNibble,
                   kMitsubishiAC_PJZ502SwingHSize, position);
} 

void IRMitsubishiPJZ::setTurbo(bool state) {
  irutils::setBit(&remote_state[kMitsubishiAC_PJZ502TurboByte], 
                  kMitsubishiAC_PJZ502TurboOffset, state);
}

void IRMitsubishiPJZ::setLight(bool state) {
  irutils::setBit(&remote_state[kMitsubishiAC_PJZ502LightByte], 
                  kMitsubishiAC_PJZ502LightOffset, state);
}

void IRMitsubishiPJZ::setSleep(bool state) {
  irutils::setBit(&remote_state[kMitsubishiAC_PJZ502SleepByte], 
                  kMitsubishiAC_PJZ502SleepOffset, state);
} 



bool IRMitsubishiPJZ::validChecksum(uint8_t* state, const uint16_t length) {
  if (length < 3) return false;  // Not enough bytes to have a checksum.
  uint8_t sum = 0;
  // Checksum is simple addition of all bytes except the last one (checksum byte)
  for (uint8_t i = 0; i < length - 1; i++) sum += state[i];
  return (sum & 0xFF) == state[length - 1];
}

void IRMitsubishiPJZ::checksum() {
  uint8_t sum = 0;
  uint8_t length = kMitsubishiAC_PJZ502StateLength;
  for (uint8_t i = 0; i < length - 1; i++) sum += remote_state[i];
  remote_state[length - 1] = sum & 0xFF;
}

// Convert the internal state into a human readable string.
String IRMitsubishiPJZ::toString(const uint8_t* state) {
  String result = "";
  result.reserve(100);  // Prevent memory fragmentation
  
  result += irutils::addBoolToString(getPowerFromState(state), kPowerStr, false);
  result += irutils::addModeToString(
      getModeFromState(state),  // Current mode
      kMitsubishiAC_PJZ502Auto,   // Auto value
      kMitsubishiAC_PJZ502Cool,   // Cool value
      kMitsubishiAC_PJZ502Heat,   // Heat value
      kMitsubishiAC_PJZ502Dry,    // Dry value
      kMitsubishiAC_PJZ502Fan);   // Fan value
  
  result += irutils::addTempToString(getTempFromState(state));
  result += irutils::addFanToString(
      getFanFromState(state),      // Current fan speed
      kMitsubishiAC_PJZ502FanHigh,  // High value
      kMitsubishiAC_PJZ502FanLow,   // Low value
      kMitsubishiAC_PJZ502FanAuto,  // Auto value
      kMitsubishiAC_PJZ502FanMed,   // Medium value
      kMitsubishiAC_PJZ502FanTop);  // Maximum value

  const uint8_t swingv = getSwingVFromState(state);
  result += ", Swing(V): ";
  switch (swingv) {
    case kMitsubishiAC_PJZ502SwingVAuto:
      result += "Auto";
      break;
    case kMitsubishiAC_PJZ502SwingVHighest:
      result += "Highest";
      break;
    case kMitsubishiAC_PJZ502SwingVHigh:
      result += "High";
      break;
    case kMitsubishiAC_PJZ502SwingVLow:
      result += "Low";
      break;
    case kMitsubishiAC_PJZ502SwingVLowest:
      result += "Lowest";
      break;
    default:
      result += "Unknown";
  }
  
  return result;
}

// Convert the A/C state to it's common stdAc::state_t equivalent.
stdAc::state_t IRMitsubishiPJZ::toCommon() {
  stdAc::state_t result{};
  result.protocol = decode_type_t::MITSUBISHI_AC_PJZ502;
  result.power = getPower();
  result.mode = toCommonMode(getMode());
  result.celsius = true;
  result.degrees = getTemp();
  result.fanspeed = toCommonFanSpeed(getFan());
  result.swingv = toCommonSwingV(getSwingV());
  return result;
}

// Add conversion methods for toCommon()
stdAc::opmode_t IRMitsubishiPJZ::toCommonMode(const uint8_t mode) {
  switch (mode) {
    case kMitsubishiAC_PJZ502Cool: return stdAc::opmode_t::kCool;
    case kMitsubishiAC_PJZ502Heat: return stdAc::opmode_t::kHeat;
    case kMitsubishiAC_PJZ502Dry:  return stdAc::opmode_t::kDry;
    case kMitsubishiAC_PJZ502Fan:  return stdAc::opmode_t::kFan;
    default:                        return stdAc::opmode_t::kAuto;
  }
}

stdAc::fanspeed_t IRMitsubishiPJZ::toCommonFanSpeed(const uint8_t speed) {
  switch (speed) {
    case kMitsubishiAC_PJZ502FanTop:  return stdAc::fanspeed_t::kMax;
    case kMitsubishiAC_PJZ502FanHigh: return stdAc::fanspeed_t::kHigh;
    case kMitsubishiAC_PJZ502FanMed:  return stdAc::fanspeed_t::kMedium;
    case kMitsubishiAC_PJZ502FanLow:  return stdAc::fanspeed_t::kLow;
    default:                           return stdAc::fanspeed_t::kAuto;
  }
}

stdAc::swingv_t IRMitsubishiPJZ::toCommonSwingV(const uint8_t pos) {
  switch (pos) {
    case kMitsubishiAC_PJZ502SwingVHighest: return stdAc::swingv_t::kHighest;
    case kMitsubishiAC_PJZ502SwingVHigh:    return stdAc::swingv_t::kHigh;
    case kMitsubishiAC_PJZ502SwingVLowest:  return stdAc::swingv_t::kLowest;
    default:                                 return stdAc::swingv_t::kAuto;
  }
}

#if DECODE_MITSUBISHI_AC_PJZ502
bool IRrecv::decodeMitsubishiACPJZ502(decode_results *results, 
                                     uint16_t offset,
                                     const uint16_t nbits,
                                     const bool strict) {
  Serial.println(F("Attempting Mitsubishi PJZ502 decode"));
  
  Serial.print(F("Offset: "));
  Serial.println(offset);
  Serial.print(F("nbits: "));
  Serial.println(nbits);
  Serial.print(F("results->rawlen: "));
  Serial.println(results->rawlen);

  // Convert raw values to microseconds
  uint32_t raw_header_mark = results->rawbuf[offset] * kRawTick;
  uint32_t raw_header_space = results->rawbuf[offset + 1] * kRawTick;
  
  Serial.print(F("Raw Header Mark (μs): "));
  Serial.println(raw_header_mark);
  Serial.print(F("Raw Header Space (μs): "));
  Serial.println(raw_header_space);
  Serial.print(F("Expected Header Mark (μs): "));
  Serial.println(kMitsubishiAC_PJZ502HdrMark);
  Serial.print(F("Expected Header Space (μs): "));
  Serial.println(kMitsubishiAC_PJZ502HdrSpace);

  // Replace the header mark check with absolute tolerance
  if (abs(int(results->rawbuf[offset] * kRawTick - kMitsubishiAC_PJZ502HdrMark)) > 50) {  // Allow 50μs difference
    Serial.print(F("Header mark failed to match. Difference: "));
    Serial.print(abs(int(results->rawbuf[offset] * kRawTick - kMitsubishiAC_PJZ502HdrMark)));
    Serial.println(F("μs"));
    return false;
  }

  // Also update the header space check to use absolute tolerance
  if (abs(int(results->rawbuf[offset + 1] * kRawTick - kMitsubishiAC_PJZ502HdrSpace)) > 50) {
    Serial.print(F("Header space failed to match. Difference: "));
    Serial.print(abs(int(results->rawbuf[offset + 1] * kRawTick - kMitsubishiAC_PJZ502HdrSpace)));
    Serial.println(F("μs"));
    return false;
  }

  Serial.println(F("Header matched successfully"));

  // Try decoding the data using matchBytes
  uint16_t pos = offset + 2;  // Skip header
  uint16_t remaining = results->rawlen - pos;

  // Decode the data bytes
  uint16_t used = matchBytes(&(results->rawbuf[pos]),
                            results->state,
                            remaining,
                            nbits / 8,  // Number of bytes to decode
                            504,        // Mark
                            3482,       // One space
                            504,        // Mark
                            1486,       // Zero space
                            _tolerance + 15,
                            0,          // No excess
                            true);      // MSBfirst

  if (!used) {
    Serial.println(F("Data decode failed"));
    return false;
  }

  results->decode_type = MITSUBISHI_AC_PJZ502;
  results->bits = nbits;
  return true;
}
#endif  // DECODE_MITSUBISHI_AC_PJZ502