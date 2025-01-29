#pragma once

using namespace std;

#include <string>
#include "ihex.hpp"
#include "crc.hpp"

struct pApp {
  string f_name;
  string type;
  string at_addr;
  string len;
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
  void calc();
  void add();
};
