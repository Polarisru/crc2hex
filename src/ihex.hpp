#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <climits>

using namespace std;

class iHex
{
public:

    /// @brief loads the data from a hexfile
    /// @param filename The name of the file to read
    /// @return true, if the lod was successful
    bool load(const string& filename);

    /// @brief the number of databytes
    size_t size() const {return m_data.size();}

    /// @brief accessor
    uint8_t operator [](unsigned int index) {return m_data[index];}

    vector <uint8_t>::iterator begin() {return m_data.begin();}

    vector <uint8_t>::iterator end() {return m_data.end();}

    /// @brief set the internal iterator to the next address which is not 0xff
    void next() {
      do
        m_iterator++;
      while ((m_iterator < m_data.size()) && (m_data[m_iterator] == 0xff));
    }

    /// @brief accessor
    unsigned int getCurrentAddress() const {return m_iterator;}

    /// @brief accessor
    bool getData(uint8_t& data) {
      if (m_iterator >= m_data.size())
        return false;
      data = m_data[m_iterator];
      return true;
    }

private:

    class Line : public string {
    public:
        /// @brief reads the hex value inside the string.
        /// If the index is outside the string, 0 is returned.
        uint8_t getHex(unsigned int index) const {return strtol(substr(1 + 2 * index, 2).c_str(), NULL, 16);}

        /// @brief reads the len byte
        uint8_t getLen() const {return getHex(0);}

        /// @brief reads the len byte
        unsigned int getOffset() const {return 256 * getHex(1) + getHex(2);}

        /// @brief reads the record type
        uint8_t getType() const {return getHex(3);}

        /// @brief reads the record data
        uint8_t getData(unsigned int index) const {return getHex(4 + index);}

    };

    /// @brief holds the loaded data (if load was successful)
    vector<uint8_t> m_data;

    /// @brief points to an address in the data area
    unsigned int m_iterator;

};
