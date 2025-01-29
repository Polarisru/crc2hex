#include <iomanip>
#include "app.hpp"
#include "crc.hpp"
#include "progress.hpp"
#include "CErrorNumber.hpp"

const int pageSize       = 256;
const int syncTimeout    = 10;
const int blTimeout      = 70;
const int blTimeoutWrite = 100;
const int blTimeoutSync  = 40;
const int maxErrValue    = 5;

void App::calc()
{
  const std::vector<uint8_t>& retrievedData = hexfile.get();
  CRC crc1;
  size_t crc_extra_len;
  if (len <= hexfile.size())
  {
    crc_extra_len = 0;
  } else
  {
    crc_extra_len = len - hexfile.size();
  }

  cout << "Calculating CRC:\t";

  switch (type)
  {
    case crc_type::CRC16:
      (void)crc1.calculateCRC16(retrievedData, hexfile.size());
      while (crc_extra_len > 0)
      {
        crc1.addCRC16(0xFF);
        crc_extra_len--;
      }
      crc = crc1.getCRC16();
      cout << "0x" << setw(4) << setfill('0') << uppercase << hex << crc;
      break;
    case crc_type::CRC32:
      (void)crc1.calculateCRC32(retrievedData, hexfile.size());
      while (crc_extra_len > 0)
      {
        crc1.addCRC32(0xFF);
        crc_extra_len--;
      }
      crc = crc1.getCRC32();
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
void App::add()
{
  vector<uint8_t> data;

  cout << "Appending CRC:";

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
  }
  hexfile.append(addr, data);

  cout << "\t\tOK";
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
    throw CErrorNumber::number::hexfileError;
  }
  try
  {
    addr = std::stoi(parameters.at_addr, nullptr, 16);
  }
  catch (const std::invalid_argument& e)
  {
    throw CErrorNumber::number::hexfileError;
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
    throw CErrorNumber::number::hexfileError;
  }

  if (hexfile.load(parameters.f_name) == false)
  {
    cout << "Can't open file: " << parameters.f_name << endl;
    throw CErrorNumber::number::hexfileError;
  }

  cout << "File: " << parameters.f_name << "\t\tSize: " << hexfile.size() << " bytes" << endl;
  cout << "CRC type: " << parameters.type << "\t\tAdd at: ";
  if (addr == 0)
  {
    cout << "END";
  } else
  {
    cout << "0x" << setw(8) << setfill('0') << uppercase << hex << addr;
  }
  cout << endl;

  calc();

//  if (parameters.write)
//  {
//    write(!parameters.unsecured);
//  }

  add();
}
