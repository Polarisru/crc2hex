#include <iomanip>
#include "app.hpp"
#include "crc.hpp"
#include "progress.hpp"
#include "CErrorNumber.hpp"

void App::calc_crc()
{
  const std::vector<uint8_t>& retrievedData = hexfile.get();
  CRC crc_mod;

  /**< Create a modifiable copy of data vector */
  out_data.assign(retrievedData.begin(), retrievedData.end());

  if (len > hexfile.getLast())
  {
    /**< Append filling values */
    size_t crc_extra_len = len - hexfile.getLast();
    out_data.insert(out_data.end(), crc_extra_len, fill_val);
  }

  std::cout << "Calculated CRC:\t\t";

  switch (type)
  {
    case crc_type::CRC16:
      (void)crc_mod.calculateCRC16(out_data, len - hexfile.getFirst());
      crc = crc_mod.getCRC16();
      std::cout << "0x" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << crc;
      break;
    case crc_type::CRC32:
      (void)crc_mod.calculateCRC32(out_data, len - hexfile.getFirst());
      crc = crc_mod.getCRC32();
      std::cout << "0x" << std::setw(8) << std::setfill('0') << std::uppercase << std::hex << crc;
      break;
  }

  std::cout << std::endl;
}

/** \brief Compare firmware with firmware from device
 *
 * \return void
 *
 */
void App::add_crc(const std::string& filename)
{
  std::vector<uint8_t> data;

  std::cout << "Appending CRC:\t\t";

  if (addr == 0)
  {
    addr = hexfile.getLast();
  }

  switch (type)
  {
    case crc_type::CRC16:
      if (parameters.endianness == "BIG")
      {
        data.push_back((uint8_t)(crc >> 8));
        data.push_back((uint8_t)(crc & 0xFF));
      } else
      {
        data.push_back((uint8_t)(crc & 0xFF));
        data.push_back((uint8_t)(crc >> 8));
      }

      break;
    case crc_type::CRC32:
      if (parameters.endianness == "BIG")
      {
        data.push_back((uint8_t)(crc >> 24));
        data.push_back((uint8_t)(crc >> 16));
        data.push_back((uint8_t)(crc >> 8));
        data.push_back((uint8_t)(crc & 0xFF));
      } else
      {
        data.push_back((uint8_t)(crc & 0xFF));
        data.push_back((uint8_t)(crc >> 8));
        data.push_back((uint8_t)(crc >> 16));
        data.push_back((uint8_t)(crc >> 24));
      }
      break;
    default:
      std::cout << "Wrong CRC type!";
      return;
  }
  hexfile.append(filename, addr, data);

  std::cout << "OK";
}

void App::write_hex(const std::string& filename)
{
  hexfile.write(filename, hexfile.getFirst(), out_data);
}

/** \brief Main application process, perform actions from command line
 *
 * \return int
 *
 */
void App::process()
{
  if (parameters.type == "CRC16")
  {
    type = crc_type::CRC16;
  } else
  if (parameters.type == "CRC32")
  {
    type = crc_type::CRC32;
  } else
  {
    throw CErrorNumber::number::wrongParameter;
  }
  try
  {
    addr = std::stoi(parameters.at_addr, nullptr, 16);
  }
  catch (const std::invalid_argument& e)
  {
    throw CErrorNumber::number::wrongParameter;
  }
  try
  {
    len = std::stoi(parameters.len, nullptr, 16);
    if (addr == 0)
    {
      len = 0;
    }
  }
  catch (const std::invalid_argument& e)
  {
    throw CErrorNumber::number::wrongParameter;
  }
  try
  {
    fill_val = static_cast<uint8_t>(std::stoi(parameters.fill_value, nullptr, 16));
  }
  catch (const std::invalid_argument& e)
  {
    throw CErrorNumber::number::wrongParameter;
  }

  if (hexfile.load(parameters.i_fname) == false)
  {
    std::cout << "Can't open file: " << parameters.i_fname << std::endl;
    throw CErrorNumber::number::hexfileError;
  }

  std::cout << "File: " << parameters.i_fname << "\t\tSize: " << hexfile.size() << " bytes" << std::endl;
  std::cout << "CRC type: " << parameters.type << "\t\tAdd at: ";
  if (addr == 0)
  {
    std::cout << "END";
  } else
  {
    std::cout << "0x" << std::setw(8) << std::setfill('0') << std::uppercase << std::hex << addr;
  }
  std::cout << std::endl;

  calc_crc();

  try
  {
    write_hex(parameters.o_fname);
  }
  catch (...)
  {
    std::cout << "Can't open file: " << parameters.o_fname << std::endl;
    throw CErrorNumber::number::fileNotWritten;
  }

  add_crc(parameters.o_fname);
}
