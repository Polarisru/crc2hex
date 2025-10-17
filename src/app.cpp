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

  cout << "Calculated CRC:\t\t";

  switch (type)
  {
    case crc_type::CRC16:
      (void)crc_mod.calculateCRC16(out_data, len - hexfile.getFirst());
      crc = crc_mod.getCRC16();
      cout << "0x" << setw(4) << setfill('0') << uppercase << hex << crc;
      break;
    case crc_type::CRC32:
      (void)crc_mod.calculateCRC32(out_data, len - hexfile.getFirst());
      crc = crc_mod.getCRC32();
      cout << "0x" << setw(8) << setfill('0') << uppercase << hex << crc;
      break;
  }

  cout << endl;
}

/** \brief Compare firmware with firmware from device
 *
 * \return void
 *
 */
void App::add_crc(const string& filename)
{
  vector<uint8_t> data;

  cout << "Appending CRC:\t\t";

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
      cout << "Wrong CRC type!";
      return;
  }
  hexfile.append(filename, addr, data);

  cout << "OK";
}

void App::write_hex(const string& filename)
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
    cout << "Can't open file: " << parameters.i_fname << endl;
    throw CErrorNumber::number::hexfileError;
  }

  cout << "File: " << parameters.i_fname << "\t\tSize: " << hexfile.size() << " bytes" << endl;
  cout << "CRC type: " << parameters.type << "\t\tAdd at: ";
  if (addr == 0)
  {
    cout << "END";
  } else
  {
    cout << "0x" << setw(8) << setfill('0') << uppercase << hex << addr;
  }
  cout << endl;

  calc_crc();

  try
  {
    write_hex(parameters.o_fname);
  }
  catch (...)
  {
    cout << "Can't open file: " << parameters.o_fname << endl;
    throw CErrorNumber::number::fileNotWritten;
  }

  add_crc(parameters.o_fname);
}
