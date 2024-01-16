#include <iostream>
#include <string>

using namespace std;

#include "cxxopts.hpp"
#include "app.hpp"
#include "CErrorNumber.hpp"
#include "version.hpp"

static const int DFLT_BITRATE = 500000;

const char title[] = "[---Bootloader application for Volz CAN---]";

void parse(int argc, const char* argv[], pApp& params)
{
  cxxopts::Options options("volz_can_boot", title);

  options
      .set_width(70)
      .set_tab_expansion()
      .allow_unrecognised_options();
  options.add_options()
      ("w,write", "Write firmware")
      ("c,check", "Check firmware")
      ("r,reset", "Reset after success")
      ("f,file", "Firmware file", cxxopts::value<string>(), "FILE")
      ("i,id", "Unit identity", cxxopts::value<int>()->default_value("1"), "0..15")
      ("b,bid", "Base CAN ID", cxxopts::value<string>()->default_value("3E0"), "000h..7E0h")
      ("p,pcb", "PCB identity", cxxopts::value<int>()->default_value("1"), "0..3")
      #if defined(__linux__)
      ("a,adapter", "SocketCAN adapter: can0..can2", cxxopts::value<int>()->default_value("0"), "0..2")
      #else
      ("s,speed", "CAN bitrate", cxxopts::value<int>()->default_value("500000"), "VALUE")
      ("a,adapter", "CAN adapter: 0-Peak, 1-Kvaser", cxxopts::value<int>()->default_value("0"), "0..1")
      #endif
      ("e,eeprom", "EEPROM file", cxxopts::value<string>(), "EE_FILE")
      ("u,unsec", "Unsecured transmission")
      ("v,version", "Print software version")
      ("h,help", "Print usage screen")
  ;

  auto result = options.parse(argc, argv);

  if (result.count("help") || (argc == 1))
  {
    // Print help screen
    std::cout << options.help() << std::endl;
    exit(0);
  }

  if (result.count("v") && (argc == 2))
  {
    // Print version info
    cout << title << endl;
    Version::print();
    exit(0);
  }

  params.write = result.count("w");
  params.check = result.count("c");
  params.reset = result.count("r");
  params.unsecured = result.count("u");
  params.can_id = stoi(result["b"].as<string>(), 0, 16);

  if (result.count("f"))
  {
    // Set HEX file name
    params.f_name = result["f"].as<string>();
  }

  if (result.count("e"))
  {
    // Set EEPROM file name
    params.ee_name = result["e"].as<string>();
  } else
  {
    params.ee_name.clear();
  }

  if (result.count("i"))
  {
    // Set device ID
    params.id = result["i"].as<int>();
  } else
  {
    params.id = 0;
  }

  #if defined (__linux__)
  if (result.count("a"))
  {
    // Set adapter type
    params.adapter = result["a"].as<int>();
  } else
  {
    params.adapter = 0;
  }
  #else
  if (result.count("a"))
  {
    // Set adapter type
    params.adapter = static_cast<Comm::adapter>(result["a"].as<int>());
  } else
  {
    params.adapter = Comm::adapter::Peak;
  }
  #endif

  #if !defined(__linux__)
  if (result.count("s"))
  {
    // Set CAN bitrate
    params.bitrate = result["s"].as<int>();
  } else
  {
    params.bitrate = DFLT_BITRATE;
  }
  #endif

  if (result.count("p"))
  {
    // Set PCB ID (for duplex actuators)
    params.pcb = result["p"].as<int>();
  } else
  {
    params.pcb = 1;
  }
}

int main(int argc, const char* argv[])
{
  try
  {
    pApp params;
    parse(argc, argv, params);
    cout << title << endl;
    if (!params.write && !params.check)
    {
      cout << "Nothing to do, exiting" << endl;
      exit(0);
    }
    App app(params);
    app.process();
  }
  catch (const cxxopts::OptionException& e)
  {
    cout << "ERROR: " << e.what() << endl;
    exit(1);
  }
  catch (volz::CErrorNumber::number num)
  {
    cout << "ERROR: " << volz::CErrorNumber::name(num) << endl;
    return static_cast<int>(num);
  }
  return 0;
}
