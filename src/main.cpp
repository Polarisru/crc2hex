#include <iostream>
#include <string>

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
      ("t,type", "CRC type (CRC16/CRC32)", cxxopts::value<std::string>(), "CRC")
      ("i,input", "Input Intel HEX file", cxxopts::value<std::string>(), "FILE")
      ("o,output", "Input Intel HEX file", cxxopts::value<std::string>(), "FILE")
      ("f,fill", "Fill all gaps with hex value: xx", cxxopts::value<std::string>()->default_value("FF"), "XXh")
      ("a,address", "Add to address: xxxx", cxxopts::value<std::string>()->default_value("F000"), "XXXXh")
      ("l,length", "Length of data block: xxxx", cxxopts::value<std::string>()->default_value("F000"), "XXXXh")
      ("e,endianness", "Set endianness of appended CRC: BIG/LITTLE", cxxopts::value<std::string>()->default_value("BIG"), "XXXX")
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
    std::cout << title << std::endl;
    Version::print();
    exit(0);
  }

  if (result.count("i"))
  {
    // Set input HEX file name
    params.i_fname = result["i"].as<std::string>();
  }

  if (result.count("o"))
  {
    // Set output HEX file name
    params.o_fname = result["o"].as<std::string>();
  } else
  {
    params.o_fname = params.i_fname;
  }

  // Hex value to fill gaps
  params.fill_value = result["f"].as<std::string>();

  if (result.count("a"))
  {
    // Set address for CRC placement, 0 means the physical end of file
    params.at_addr = result["a"].as<std::string>();
  } else
  {
    params.at_addr = "0";
  }

  if (result.count("l"))
  {
    // Set length of the HEX file (will be padded with fill_value), 0 means no padding
    params.len = result["l"].as<std::string>();
  } else
  {
    params.len = "0";
  }

  if (result.count("t"))
  {
    // Set CRC type (CRC16/CRC32)
    params.type = result["t"].as<std::string>();
  } else
  {
    params.type = "CRC16";
  }

  if (result.count("e"))
  {
    // Set endianness (BIG/LITTLE)
    params.endianness = result["e"].as<std::string>();
  } else
  {
    params.endianness = "BIG";
  }
}

int main(int argc, const char* argv[])
{
  try
  {
    pApp params;
    parse(argc, argv, params);
    std::cout << title << std::endl;
    if (params.i_fname.empty())
    {
      std::cout << "Nothing to do, exiting" << std::endl;
      exit(0);
    }
    App app(params);
    app.process();
  }
  catch (const cxxopts::OptionException& e)
  {
    std::cout << "ERROR: " << e.what() << std::endl;
    exit(1);
  }
  catch (const CErrorNumber& e)
  {
    std::cout << "ERROR: " << e.what() << std::endl;
    return e.getErrorCode();
  }
  return 0;
}
