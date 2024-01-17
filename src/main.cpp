#include <iostream>
#include <string>

using namespace std;

#include "cxxopts.hpp"
#include "app.hpp"
#include "CErrorNumber.hpp"
#include "version.hpp"

//static const int DFLT_BITRATE = 500000;

const char title[] = "[---Add CRC to a Intel HEX file---]";

void parse(int argc, const char* argv[], pApp& params)
{
  cxxopts::Options options("crc2hex", title);

  options
      .set_width(70)
      .set_tab_expansion()
      .allow_unrecognised_options();
  options.add_options()
      ("t,type", "CRC type (CRC16/CRC32)", cxxopts::value<string>(), "CRC")
      ("f,file", "Intel HEX file", cxxopts::value<string>(), "FILE")
      ("a,address", "Add to address: xxxx", cxxopts::value<string>()->default_value("F000"), "XXXXh")
      ("l,length", "Length of data block: xxxx", cxxopts::value<string>()->default_value("F000"), "XXXXh")
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

  if (result.count("f"))
  {
    // Set HEX file name
    params.f_name = result["f"].as<string>();
  }

  if (result.count("a"))
  {
    // Set device ID
    params.at_addr = result["a"].as<string>();
  } else
  {
    params.at_addr = "0";
  }

  if (result.count("l"))
  {
    // Set device ID
    params.len = result["l"].as<string>();
  } else
  {
    params.len = "0";
  }

  if (result.count("t"))
  {
    // Set HEX file name
    params.type = result["t"].as<string>();
  } else
  {
    params.type = "CRC16";
  }
}

int main(int argc, const char* argv[])
{
  try
  {
    pApp params;
    parse(argc, argv, params);
    cout << title << endl;
    if (params.f_name.empty())
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
  catch (CErrorNumber::number num)
  {
    cout << "ERROR: " << CErrorNumber::name(num) << endl;
    return static_cast<int>(num);
  }
  return 0;
}
