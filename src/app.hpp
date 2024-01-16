#pragma once

using namespace std;

#include <string>
#include "ihex.hpp"
#include "mfile.hpp"
#include "volz.hpp"
#include "comm.hpp"

struct pApp {
  string f_name;
  string ee_name;
  int    id;
  int    can_id;
  int    pcb;
  bool   write;
  bool   check;
  bool   reset;
  bool   unsecured;
  int    bitrate;
  #if defined(__linux__)
  int    adapter;
  #else
  Comm::adapter adapter;
  #endif
};

class App : public Volz {
public:
  App(const pApp& param) : Volz(param.id, param.can_id, param.pcb),
                           parameters(param), cPcan(param.bitrate, param.adapter) {};
  ~App() {};
  void process();
private:
  pApp parameters;
  CommFilter filter;
  iHex hexfile;
  mFile mapfile;
  Comm cPcan;
  void do_sync();
  void write(bool secured);
  void check();
  void save_settings();
  void do_reset();
};

int APP_Execute(pApp& parameters);
