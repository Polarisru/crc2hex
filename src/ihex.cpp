#include <iomanip>
#include "ihex.hpp"

bool iHex::load(const string &filename)
{
  first_addr = UINT_MAX;
  //uint32_t last_addr = 0;

  m_data.clear();

  ifstream infile(filename);
  if (!infile.is_open())
  {
    return false;
  }

  Line line;

  while (getline(infile, line))
  {
    uint8_t type = line.getType();
    if (type == static_cast<uint8_t>(RecordType::Data))
    {
      uint32_t offset = line.getOffset();
      /**< Hex file can have offset and must be aligned to address 0 */
      if (offset < first_addr)
      {
        first_addr = offset;
      }
      uint32_t size = line.getLen();
      while (m_data.size() < offset + size)
      {
        m_data.push_back(0xff);
      }
      for (uint32_t i = 0; i < size; i++)
      {
        m_data[i + offset] = line.getData(i);
      }
    }
  }

  m_data.erase(m_data.begin(), m_data.begin() + first_addr);

  file_name = filename;

  return true;
}

string conv_to_string(uint16_t address, RecordType type, const vector<uint8_t> &data)
{
  stringstream ss;
  uint8_t crc = 0;

  /**< Add size */
  ss << ":" << setw(2) << setfill('0') << uppercase << hex << static_cast<int>(data.size());
  /**< Add address and record type */
  ss << setw(4) << address << setw(2) << static_cast<int>(type);
  crc += static_cast<uint8_t>(data.size());
  crc += (uint8_t)(address >> 8);
  crc += (uint8_t)(address & 0xFF);
  crc += static_cast<uint8_t>(type);
  /**< Process data */
  for (size_t i = 0; i < data.size(); i++)
  {
    ss << setw(2) << static_cast<int>(data[i]);
    crc += data[i];
  }
  crc = (uint8_t)(0x100 - crc);
  /**< Add CRC */
  ss << setw(2) << static_cast<int>(crc) << endl;

  return ss.str();
}

bool iHex::append(const uint32_t &addr, const vector<uint8_t> &data)
{
  uint32_t address = addr;

  ifstream infile(file_name);
  if (!infile.is_open())
  {
    return false;
  }

  /**< Read all lines from the file into a vector */
  std::vector<std::string> lines;
  std::string line;

  while (std::getline(infile, line))
  {
    lines.push_back(line);
  }

  /**< Close the input file */
  infile.close();

  /**< Add "_crc" to a filename */
  size_t dotPosition = file_name.find_last_of('.');
  std::string newfile = file_name.substr(0, dotPosition) +
                        "_crc" + file_name.substr(dotPosition);

  /**< Open the file in output mode (trunc flag erases the content) */
  std::ofstream outfile(newfile, std::ios::trunc);

  if (!outfile.is_open())
  {
    return false;
  }

  /**< Write all lines back to the file, stopping before the end line */
  for (size_t i = 0; i < lines.size(); i++)
  {
    Line line1 = lines[i];
    if (line1.getType() == static_cast<uint8_t>(RecordType::EndOfFile))
    {
      //if (address > UINT16_MAX)
      //{
        vector<uint8_t> temp;
        temp.push_back((uint8_t)(address >> 24));
        temp.push_back((uint8_t)(address >> 16));
        outfile << conv_to_string(0, RecordType::ExtendedLinearAddress, temp);
        address &= UINT16_MAX;
      //}
      size_t chunkSize = 0x10;
      auto it = data.begin();
      for (size_t j = 0; j < data.size(); j += chunkSize)
      {
        size_t len = std::min(chunkSize, static_cast<size_t>(std::distance(it, data.end())));
        std::vector<uint8_t> chunk(it, it + len);
        outfile << conv_to_string(address, RecordType::Data, chunk);
        address += chunkSize;
      }
    }
    outfile << lines[i] << endl;
  }

  /**< Close the output file */
  outfile.close();

  return true;
}


