#pragma once

#include <cstdint>
#include <vector>

using namespace std;

enum class crc_type {
  CRC16,
  CRC32
};

const uint16_t CRC16_INIT_VAL = 0xFFFFU;
const uint32_t CRC32_INIT_VAL = 0xFFFFFFFFU;

class CRC {
public:
  //CRC() : crc16(CRC16_INIT_VAL) {};
  //CRC() : crc32(CRC32_INIT_VAL) {};
  uint16_t calculateCRC16(const vector<uint8_t>& data, size_t len);
  uint32_t calculateCRC32(const vector<uint8_t>& data, size_t len);
  uint16_t getCRC16() {return crc16;};
  uint32_t getCRC32() {return crc32 ^ 0xFFFFFFFFU;};
  void addCRC16(uint8_t data);
  void addCRC32(uint8_t data);
private:
  uint16_t crc16;
  uint32_t crc32;
};
