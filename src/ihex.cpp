#include <iomanip>
#include <filesystem>
#include "ihex.hpp"

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
  ss << setw(2) << static_cast<int>(crc);

  return ss.str();
}

// Compute Intel HEX checksum for one record
static uint8_t ihex_checksum(uint8_t byte_count, uint16_t addr, uint8_t rectype, const uint8_t* data, size_t len)
{
  uint32_t sum = byte_count;
  sum += static_cast<uint8_t>(addr >> 8);
  sum += static_cast<uint8_t>(addr & 0xFFU);
  sum += rectype;
  for (size_t i = 0; i < len; ++i)
  {
    sum += data[i];
  }
  return static_cast<uint8_t>((~sum + 1) & 0xFFU);
}

// Format one Intel HEX record line
static string make_record(uint8_t byte_count, uint16_t addr, uint8_t rectype, const uint8_t* data, size_t len)
{
  std::ostringstream oss;
  oss << ':';
  oss << std::uppercase << std::hex << std::setfill('0');
  oss << std::setw(2) << static_cast<int>(byte_count);
  oss << std::setw(4) << static_cast<int>(addr);
  oss << std::setw(2) << static_cast<int>(rectype);
  for (size_t i = 0; i < len; ++i)
  {
    oss << std::setw(2) << static_cast<int>(data[i]);
  }
  uint8_t cks = ihex_checksum(byte_count, addr, rectype, data, len);
  oss << std::setw(2) << static_cast<int>(cks);
  return oss.str();
}

bool iHex::load(const string &filename)
{
  first_addr = UINT_MAX;

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
        m_data.push_back(0xFFU);
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

bool iHex::append(const string &filename, const uint32_t &addr, const std::vector<uint8_t> &data)
{
  uint32_t address = addr;

  std::ifstream infile(filename);
  if (!infile.is_open())
  {
    return false;
  }

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(infile, line))
  {
    lines.push_back(line);
  }
  infile.close();

  /**< Find EOF record position (last line usually) */
  size_t eof_idx = lines.size();
  for (size_t i = 0; i < lines.size(); ++i)
  {
    Line l = lines[i];
    if (l.getType() == static_cast<uint8_t>(RecordType::EndOfFile))
    {
      eof_idx = i;
      break;
    }
  }
  if (eof_idx == lines.size()) return false; // no EOF record

  /**< Prepare new records to insert before EOF */
  std::vector<std::string> insert_lines;
//// If needed, emit ELA (Extended Linear Address) for 32-bit address
//  {
//    std::vector<uint8_t> temp;
//    temp.push_back(static_cast<uint8_t>(address >> 24));
//    temp.push_back(static_cast<uint8_t>(address >> 16));
//    insert_lines.push_back(conv_to_string(0, RecordType::ExtendedLinearAddress, temp));
//    address &= 0xFFFF;
//  }
  const size_t chunkSize = 0x10;
  for (size_t j = 0; j < data.size(); j += chunkSize)
  {
    size_t len = std::min(chunkSize, data.size() - j);
    std::vector<uint8_t> chunk(data.begin() + j, data.begin() + j + len);
    insert_lines.push_back(conv_to_string(address, RecordType::Data, chunk));
    address += static_cast<uint32_t>(len);
    if (((address & 0xFFFF) == 0) && ((j + len) < data.size()))
    {
      /**< Crossed 64 KiB boundary; emit next ELA */
      uint32_t linear = addr + static_cast<uint32_t>(j + len);
      std::vector<uint8_t> ela{ static_cast<uint8_t>(linear >> 24),
                                static_cast<uint8_t>(linear >> 16) };
      insert_lines.push_back(conv_to_string(0, RecordType::ExtendedLinearAddress, ela));
      address = 0;
    }
  }

  /**< Create temp file next to original */
  std::string tmp = filename + ".tmp";
  std::ofstream out(tmp, std::ios::trunc);
  if (!out.is_open())
  {
    return false;
  }

  /**< Write original up to EOF, then inserted records, then EOF and any trailing lines */
  for (size_t i = 0; i < eof_idx; ++i)
  {
    out << lines[i] << '\n';
  }
  for (auto &s : insert_lines)
  {
    out << s << '\n';
  }
  for (size_t i = eof_idx; i < lines.size(); ++i)
  {
    out << lines[i] << '\n';
  }
  out.close();

  /**< Replace original */
  std::error_code ec;
  std::filesystem::rename(tmp, filename, ec);
  if (ec)
  {
    std::filesystem::remove(tmp);
    return false;
  }
  return true;
}

bool iHex::write(const string& filename, const uint32_t &start_addr, const vector<uint8_t> &data)
{
  // Write to a temp file first for safety
  string tmp = filename + ".tmp";
  std::ofstream out(tmp, std::ios::trunc);
  if (!out.is_open()) return false;

  // Work variables
  uint32_t cur = start_addr;
  const size_t chunk = 16;

  auto emit_ela = [&](uint16_t high16)
  {
    uint8_t payload[2] = {
      static_cast<uint8_t>(high16 >> 8),
      static_cast<uint8_t>(high16 & 0xFFU)
    };
    string rec = make_record(2,
                             0x0000,
                             static_cast<uint8_t>(RecordType::ExtendedLinearAddress),
                             payload,
                             2);
    out << rec << '\n';
    return static_cast<bool>(out);
  };

  // Ensure initial ELA reflects the upper 16 bits of start_addr
  uint16_t current_ela = static_cast<uint16_t>(start_addr >> 16);
  if (!emit_ela(current_ela))
  {
    out.close();
    std::remove(tmp.c_str());
    return false;
  }

  size_t offset = 0;
  while (offset < data.size())
  {
    // Check for 64KiB boundary crossing; if crossing, emit new ELA
    uint16_t low16 = static_cast<uint16_t>(cur & 0xFFFFU);
    uint16_t needed_ela = static_cast<uint16_t>(cur >> 16);
    if (needed_ela != current_ela)
    {
      current_ela = needed_ela;
      if (!emit_ela(current_ela))
      {
        out.close();
        std::remove(tmp.c_str());
        return false;
      }
    }

    // Emit a data record limited by remaining bytes and page boundary
    size_t remain = data.size() - offset;
    // bytes until 64KiB boundary
    size_t room_in_page = 0x10000U - low16;
    size_t bc = std::min(chunk, std::min(remain, room_in_page));
    string rec = make_record(static_cast<uint8_t>(bc),
                             low16,
                             static_cast<uint8_t>(RecordType::Data),
                             &data[offset],
                             bc);
    out << rec << '\n';
    if (!out)
    {
      out.close();
      std::remove(tmp.c_str());
      return false;
    }

    cur += static_cast<uint32_t>(bc);
    offset += bc;
  }

  // EOF must be last
  {
    string eof = make_record(0, 0x0000, static_cast<uint8_t>(RecordType::EndOfFile), nullptr, 0);
    out << eof << '\n';
    if (!out)
    {
      out.close();
      std::remove(tmp.c_str());
      return false;
    }
  }

  out.close();
  if (!out)
  {
    std::remove(tmp.c_str());
    return false;
  }

  /**< Replace original */
  std::error_code ec;
  std::filesystem::rename(tmp, filename, ec);
  if (ec)
  {
    std::filesystem::remove(tmp);
    return false;
  }
  return true;
}
