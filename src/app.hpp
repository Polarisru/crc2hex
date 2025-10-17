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
  App(const pApp& param) : parameters(param) {};
  ~App() {};
  void process();
private:
  pApp parameters;
  iHex hexfile;
  crc_type type;
  uint32_t addr;
  uint32_t len;
  uint32_t crc;
  uint8_t  fill_val;
  std::vector<uint8_t> out_data;

  void calc_crc();
  void add_crc(const std::string& filename);
  void write_hex(const std::string& filename);
};
