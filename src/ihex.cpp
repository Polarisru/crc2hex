#include "ihex.hpp"

bool iHex::load(const string &filename)
{
  unsigned int first_addr = UINT_MAX;

  m_data.clear();

  ifstream infile(filename);
  if (!infile.is_open())
  {
    return false;
  }

  Line line;

  while (getline(infile, line))
  {
    unsigned int type = line.getType();
    if (type == 0)
    {
      unsigned int offset = line.getOffset();
      /**< Hex file can have offset and must be aligned to address 0 */
      if (offset < first_addr)
      {
        first_addr = offset;
      }
      offset -= first_addr;
      unsigned int size = line.getLen();
      while (m_data.size() < offset + size)
      {
        m_data.push_back(0xff);
      }
      for (unsigned int i = 0; i < size; i++)
      {
        m_data[i + offset] = line.getData(i);
      }
    }
  }
  /**< Padding with 0xff to the end of the current page */
  while (m_data.size() % 256)
  {
    m_data.push_back(0xff);
  }

  return true;
}


