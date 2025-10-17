#pragma once

using namespace std;

#include <string>
#include "ihex.hpp"
#include "crc.hpp"

struct pApp {
  string i_fname;
  string o_fname;
  string type;
  string at_addr;
  string len;
  string fill_value;
  string endianness;
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
  void add_crc(const string& filename);
  void write_hex(const string& filename);
};
