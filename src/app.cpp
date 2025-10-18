#include <iomanip>
#include "app.hpp"
#include "crc.hpp"
#include "progress.hpp"
#include "CErrorNumber.hpp"

App::App(const pApp& param)
{
  if (param.type == "CRC16")
  {
    type = crc_type::CRC16;
  } else
  if (param.type == "CRC32")
  {
    type = crc_type::CRC32;
  } else
  {
    throw CErrorNumber(number::wrongParameter);
  }
  try
  {
    addr = std::stoi(param.at_addr, nullptr, 16);
    len = std::stoi(param.len, nullptr, 16);
    if (addr == 0)
    {
      len = 0;
    }
    fill_val = static_cast<uint8_t>(std::stoi(param.fill_value, nullptr, 16));
  }
  catch (const std::invalid_argument& e)
  {
    throw CErrorNumber(number::wrongParameter);
  }

  if (hexfile.load(param.i_fname) == false)
  {
    throw CErrorNumber(number::hexfileError, "Can't open file: " + param.i_fname);
  }

  is_big_endian = (param.endianness == "BIG");

  out_file = param.o_fname;

  std::cout << "File: " << param.i_fname << "\t\tSize: " << hexfile.size() << " bytes" << std::endl;
  std::cout << "CRC type: " << param.type << "\t\tAdd at: ";
  if (addr == 0)
  {
    std::cout << "END";
  } else
  {
    std::cout << "0x" << std::setw(8) << std::setfill('0') << std::uppercase << std::hex << addr;
  }
  std::cout << std::endl;
}

void App::calc_crc()
{
  const std::vector<uint8_t>& retrievedData = hexfile.get();
  CRC crc_mod;

  /**< Create a modifiable copy of data vector */
  out_data.assign(retrievedData.begin(), retrievedData.end());

  if (len > hexfile.getEndAddress())
  {
    /**< Append filling values */
    size_t crc_extra_len = len - hexfile.getEndAddress();
    out_data.insert(out_data.end(), crc_extra_len, fill_val);
  }

  std::cout << "Calculated CRC:\t\t";

  switch (type)
  {
    case crc_type::CRC16:
      (void)crc_mod.calculateCRC16(out_data, len - hexfile.getStartAddress());
      crc = crc_mod.getCRC16();
      std::cout << "0x" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << crc;
      break;
    case crc_type::CRC32:
      (void)crc_mod.calculateCRC32(out_data, len - hexfile.getStartAddress());
      crc = crc_mod.getCRC32();
      std::cout << "0x" << std::setw(8) << std::setfill('0') << std::uppercase << std::hex << crc;
      break;
  }

  std::cout << std::endl;
}

static void serialize_crc(std::vector<uint8_t>& data,
                          uint32_t value,
                          size_t size_in_bytes,
                          bool is_big_endian)
{
  if (is_big_endian)
  {
    // High byte first
    for (int i = size_in_bytes - 1; i >= 0; --i)
    {
      data.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFFU));
    }
  } else
  {
    // Low byte first
    for (size_t i = 0; i < size_in_bytes; ++i)
    {
      data.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFFU));
    }
  }
}

/** \brief Compare firmware with firmware from device
 *
 * \return void
 *
 */
void App::add_crc(const std::string& filename)
{
  std::vector<uint8_t> data;
  std::cout << "Appending CRC.. ";

  if (addr == 0)
  {
    addr = hexfile.getEndAddress();
  }

  size_t crc_size_bytes;
  switch (type)
  {
    case crc_type::CRC16:
      crc_size_bytes = 2;
      break;
    case crc_type::CRC32:
      crc_size_bytes = 4;
      break;
    default:
      std::cout << "Wrong CRC type!" << std::endl;
      return;
  }

  // Call the single helper function
  serialize_crc(data, crc, crc_size_bytes, is_big_endian);

  hexfile.append(filename, addr, data);
  std::cout << "OK" << std::endl;
}

void App::write_hex(const std::string& filename)
{
  hexfile.write(filename, hexfile.getStartAddress(), out_data);
}

/** \brief Main application process, perform actions from command line
 *
 * \return int
 *
 */
void App::process()
{
  calc_crc();

  try
  {
    write_hex(out_file);
  }
  catch (...)
  {
    throw CErrorNumber(number::hexfileError, "Can't write to file: " + out_file);
  }

  add_crc(out_file);
}
