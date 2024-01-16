#include "app.hpp"
#include "crc16.hpp"
#include "progress.hpp"
#include "volz.hpp"
#include "xtea.hpp"
#include "CErrorNumber.hpp"

const int pageSize       = 256;
const int syncTimeout    = 10;
const int blTimeout      = 70;
const int blTimeoutWrite = 100;
const int blTimeoutSync  = 40;
const int maxErrValue    = 5;

/** \brief Do synchronization with connected device
 *
 * \return void
 *
 */
void App::do_sync()
{
  CommPacket packet, packet_rx;

  packet.can_id = get_config_id();

  time_t last = 0;
  int countdown = syncTimeout;
  while (countdown >= 0)
  {
    if (time(nullptr) > last)
    {
      cout << "\rSyncing: " << countdown-- << " sec... " << flush;
      last = time(0);
    }
    packet.data = Volz::sync(VolzProtocol::PROTOCOL_NEW);
    cPcan.send(packet);
    //if ((cPcan.receive(packet_rx, filter, blTimeoutSync) == true) &&
    //    (Volz::check_response(packet_rx.data) == true))
    while (cPcan.receive(packet_rx, filter, blTimeoutSync) == true)
    {
      if (Volz::check_response(packet_rx.data) == true)
      {
        cout << "\tOK" << endl;
        return;
      }
    }
    packet.data = Volz::sync(VolzProtocol::PROTOCOL_OLD);
    cPcan.send(packet);
    //if ((cPcan.receive(packet_rx, filter, blTimeoutSync) == true) &&
    //    (Volz::check_response(packet_rx.data) == true))
    while (cPcan.receive(packet_rx, filter, blTimeoutSync) == true)
    {
      if (Volz::check_response(packet_rx.data) == true)
      {
        cout << "\tOK" << endl;
        return;
      }
    }
  }
  cout << "\tFAILED!" << endl;
  throw CErrorNumber::number::bootloaderNoSync;
}

void App::write(bool secured)
{
  CommPacket packet, packet_rx;
  bool repeat_tx = true;
  vector<uint8_t> data;
  union {
    uint8_t  u8_arr[sizeof(uint32_t)];
    uint32_t u32_val;
  } u_trans;

  while (cPcan.receive(packet_rx, filter, blTimeout) == true);

  packet.can_id = Volz::get_config_id();

  Progress pr("Writing firmware", hexfile.size() / pageSize);
  int errors = 0;
  uint16_t page = 0;
  auto it = hexfile.begin();
  while (it != hexfile.end())
  {
    if (secured == true)
    {
      Xtea x;
      data = x.encrypt(vector<uint8_t>(it, it + pageSize), pageSize, page);
    } else
    {
      data = vector<uint8_t>(it, it + pageSize);
    }
    int counter = 0;
    int errors2 = 0;
    while (counter < pageSize)
    {
      copy(data.begin() + counter, data.begin() + counter + sizeof(uint32_t),
           u_trans.u8_arr);
      packet.data = Volz::write_data(counter, u_trans.u32_val);
      if (repeat_tx == true)
      {
        cPcan.send(packet);
      }
      repeat_tx = true;
      if ((cPcan.receive(packet_rx, filter, blTimeout) == false) ||
          (Volz::check_response(packet_rx.data) == false))
      {
        /**< No response or error reported */
        if (errors2++ >= maxErrValue)
        {
          cout << "\tERROR!";
          throw CErrorNumber::number::bootloaderWriteFails;
        }
        continue;
      }
      if (Volz::check_packet(packet_rx.data) == false)
      {
        /**< Wrong response, packet mismatch probably */
        repeat_tx = false;
        continue;
      }
      errors2 = 0;
      counter += sizeof(uint32_t);
    }
    if (secured == true)
    {
      packet.data = write_page(page);
    } else
    {
      packet.data = write_page_raw(page);
    }
    if (repeat_tx == true)
    {
      cPcan.send(packet);
    }
    repeat_tx = true;
    if ((cPcan.receive(packet_rx, filter, blTimeoutWrite) == false) ||
        (Volz::check_response(packet_rx.data) == false))
    {
      /**< No response or error reported */
      if (errors++ >= maxErrValue)
      {
        cout << "\tERROR!";
        throw CErrorNumber::number::bootloaderWriteFails;
      }
      continue;
    }
    if (Volz::check_packet(packet_rx.data) == false)
    {
      /**< Wrong response, packet mismatch probably */
      repeat_tx = false;
      continue;
    }
    errors = 0;
    page++;
    it += pageSize;
    pr.inc();
  }
  cout << "\tOK";
}

/** \brief Compare firmware with firmware from device
 *
 * \return void
 *
 */
void App::check()
{
  CommPacket packet, packet_rx;
  bool repeat_tx = true;

  while (cPcan.receive(packet_rx, filter, blTimeout) == true);

  packet.can_id = get_config_id();

  Progress pr("Checking firmware", hexfile.size() / pageSize);
  int errors = 0;
  uint16_t page = 0;
  while (page * pageSize < hexfile.size())
  {
    CRC16 sCRC;
    vector<uint8_t> data(hexfile.begin() + page * pageSize,
                         hexfile.begin() + (page + 1) * pageSize);
    uint16_t crc = sCRC.calc(data, pageSize);
    packet.data = Volz::check_data(page, crc);
    errors = 0;
    while (errors++ < maxErrValue)
    {
      if (repeat_tx == true)
      {
        cPcan.send(packet);
      }
      repeat_tx = true;
      if ((cPcan.receive(packet_rx, filter, blTimeout) == true) &&
          (Volz::check_response(packet_rx.data) == true) &&
          (Volz::check_packet(packet_rx.data) == true))
      {
        /**< Everything is Ok */
        break;
      }
      if (Volz::check_packet(packet_rx.data) == false)
      {
        /**< Wrong response, packet mismatch probably */
        repeat_tx = false;
        continue;
      }
      errors++;
    }
    if (errors >= maxErrValue)
    {
      cout << "\tERROR!";
      throw CErrorNumber::number::bootloaderVerifyFails;
    }
    page++;
    pr.inc();
  }
  cout << "\tOK";
}

/** \brief Compare firmware with firmware from device
 *
 * \return void
 *
 */
void App::save_settings()
{
  std::pair<std::string, int> data;
  CommPacket packet, packet_rx;

  bool repeat_tx = true;

  while (cPcan.receive(packet_rx, filter, blTimeout) == true);

  packet.can_id = get_config_id();

  Progress pr("Writing settings", mapfile.size());
  int errors = 0;

  for (uint32_t i = 0; i < mapfile.size(); i++)
  {
    std::stringstream ss;
    int x;
    data = mapfile.getNextData();
    ss << std::hex << data.first;
    ss >> x;
    packet.data = Volz::write_ee(static_cast<uint16_t>(x), static_cast<uint8_t>(data.second));
    errors = 0;
    while (errors++ < maxErrValue)
    {
      if (repeat_tx == true)
      {
        cPcan.send(packet);
      }
      repeat_tx = true;
      if ((cPcan.receive(packet_rx, filter, blTimeout) == true) &&
          (Volz::check_response(packet_rx.data) == true) &&
          (Volz::check_packet(packet_rx.data) == true))
      {
        /**< Everything is Ok */
        break;
      }
      if (Volz::check_packet(packet_rx.data) == false)
      {
        /**< Wrong response, packet mismatch probably */
        repeat_tx = false;
        continue;
      }
      errors++;
    }
    if (errors >= maxErrValue)
    {
      cout << "\tERROR!";
      throw CErrorNumber::number::bootloaderSettFails;
    }
    pr.inc();
  }
  cout << "\tOK";
}

/** \brief Do reset (jump to main application)
 *
 * \return void
 *
 */
void App::do_reset()
{
  CommPacket packet;
  packet.can_id = Volz::get_config_id();
  packet.data = Volz::reset();
  for (int i = 0; i < 3; i++)
  {
    cPcan.send(packet);
  }
}

/** \brief Main application process, perform actions from command line
 *
 * \return int
 *
 */
void App::process()
{
  if (hexfile.load(parameters.f_name) == false)
  {
    cout << "Can't open file: " << parameters.f_name << endl;
    throw CErrorNumber::number::hexfileError;
  }

  if (parameters.ee_name.empty() == false)
  {
    if (mapfile.load(parameters.ee_name) == false)
    {
      cout << "Can't open file: " << parameters.ee_name << endl;
      throw CErrorNumber::number::mapfileError;
    }
  }

  cout << "File: " << parameters.f_name << "\tSize: " << hexfile.size() << " bytes" << endl;
  cout << "CAN ID: 0x" << std::hex << parameters.can_id << std::dec << "\tDevice ID: " <<
          parameters.id << "\tPCB ID: " << parameters.pcb << endl;

  if (parameters.id == 0)
  {
    /**< Broadcasting */
    filter.mask = 0x7F0;
  } else
  {
    filter.mask = 0x7FF;
  }
  filter.id = Volz::get_reply_id();

  cPcan.reset();

  do_sync();

  if (parameters.write)
  {
    write(!parameters.unsecured);
  }

  if (parameters.check)
  {
    check();
  }

  // Write settings
  if (parameters.ee_name.empty() == false)
  {
    cout << "File: " << parameters.ee_name << "\tSize: " << mapfile.size() << " records" << endl;
    save_settings();
  }

  if (parameters.reset)
  {
    do_reset();
  }
}
