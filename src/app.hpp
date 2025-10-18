#pragma once

#include <string>
#include "ihex.hpp"
#include "crc.hpp"

struct pApp {
  std::string i_fname;
  std::string o_fname;
  std::string type;
  std::string at_addr;
  std::string len;
  std::string fill_value;
  std::string endianness;
};

class App {
public:
  App(const pApp& param);
  ~App() = default;
  void process();
private:
  iHex hexfile;
  crc_type type = crc_type::CRC16;
  uint32_t addr = 0;
  uint32_t len = 0;
  uint32_t crc = 0;
  uint8_t  fill_val = 0xFFU;
  bool is_big_endian = false;
  std::vector<uint8_t> out_data;
  std::string out_file;

  void calc_crc();
  void add_crc(const std::string& filename);
  void write_hex(const std::string& filename);
};
