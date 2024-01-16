#pragma once

#include <cstdint>
#include <vector>

using namespace std;

const uint16_t CRC16_INIT_VAL = 0xffff;

class CRC16 {
public:
  CRC16() : crc(CRC16_INIT_VAL) {};
  uint16_t calc(const vector<uint8_t>& data, size_t len);
private:
  uint16_t crc;
};
