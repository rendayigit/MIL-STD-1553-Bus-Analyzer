#include "bm.hpp"

#include <chrono>
#include <exception>
#include <iostream>
#include <string>

BM::BM()
    : m_devNum(Json(CONFIG_PATH).getNode("DEFAULT_DEVICE_NUMBER").getValue<int>()),
      m_monitorPollThreadSleepMs(Json(CONFIG_PATH).getNode("MONITOR_POLL_THREAD_SLEEP_MS").getValue<int>()),
      m_loop(false) {}

BM::~BM() { stopBm(); }

S16BIT BM::startBm(int devNum) {
  S16BIT err = 0;

  m_devNum = devNum;

  m_logger.log(LOG_INFO, "start bm with dev: " + std::to_string(m_devNum));

  m_loop = false;

  if (m_monitorThread.joinable()) {
    m_monitorThread.join();
  }

  err = aceFree(static_cast<S16BIT>(m_devNum));

  if (err != 0) {
    return err;
  }

  // err = aceInitialize(m_devNum, ACE_ACCESS_CARD, ACE_MODE_MT, 0, 0, 0);

  if (err != 0) {
    return err;
  }

  // err = aceMTStart(m_devNum);

  if (err != 0) {
    return err;
  }

  try {
    m_loop = true;
    m_monitorThread = std::thread([&] { monitor(); });
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}

S16BIT BM::stopBm() {
  S16BIT err = 0;

  m_logger.log(LOG_INFO, "stop bm with dev: " + std::to_string(m_devNum));

  m_loop = false;

  if (m_monitorThread.joinable()) {
    m_monitorThread.join();
  }

  err = aceMTStop(static_cast<S16BIT>(m_devNum));

  if (err != 0) {
    return err;
  }

  err = aceFree(static_cast<S16BIT>(m_devNum));

  if (err != 0) {
    return err;
  }

  return 0;
}

Message BM::getMessage(MSGSTRUCT *msg) {
  static U64BIT messageNumber = 0;
  U16BIT rt = 0;
  U16BIT wTR1 = 0;
  U16BIT wTR2 = 0;
  U16BIT sa = 0;
  U16BIT wc = 0;

  U16BIT aDataWrds[32];
  std::fill(std::begin(aDataWrds), std::end(aDataWrds), 0);
  const char *type = "BC to RT";

  return Message(rt, sa, -1, -1, wc, 'A', type, "00000000:00000000:00000000us", ++messageNumber, aDataWrds);

  aceCmdWordParse(msg->wCmdWrd1, &rt, &wTR1, &sa, &wc);

  std::ostringstream time;

  time << std::setw(8) << std::setfill('0') << (msg->wTimeTag3 * 2) << ":" << std::setw(8) << std::setfill('0')
       << (msg->wTimeTag2 * 2) << ":" << std::setw(8) << std::setfill('0') << (msg->wTimeTag * 2) << "us";

  Message message(rt, sa, -1, -1, wc, (msg->wBlkSts & ACE_MT_BSW_CHNL) != 0 ? 'B' : 'A',
                  aceGetMsgTypeString(msg->wType), time.str(), ++messageNumber, msg->aDataWrds);

  return message;
}

void BM::monitor() {
  S16BIT err = 0;
  MSGSTRUCT sMsg;
  std::string messageBuffer;

  // Poll Messages
  while (m_loop) {
    // err = aceMTGetStkMsgDecoded(m_devNum, &sMsg, ACE_MT_MSGLOC_NEXT_PURGE,
    // ACE_MT_STKLOC_ACTIVE);

    // if (err == 1) {
    Message message = getMessage(&sMsg);

    std::string messageString = "message#: " + std::to_string(message.getNumber()) + " time: " + message.getTime() +
                                " bus: " + message.getBus() + " type: " + message.getType() +
                                " rt: " + std::to_string(message.getRt()) + " sa: " + std::to_string(message.getSa()) +
                                " wc: " + std::to_string(message.wc()) + " data: ";

    std::vector<std::string> data = message.getData();

    for (auto &d : data) {
      messageString += d + " ";
    }

    m_logger.log(LOG_INFO, "Bus Activity: \n " + messageString);

    m_updateSaState(message.getBus(), message.getRt(), message.getSa(), true);
    // TODO(renda): implement false state

    m_updateMessages(messageString + "\n");
    // }

    std::this_thread::sleep_for(std::chrono::milliseconds(m_monitorPollThreadSleepMs));

    // TODO(renda): implement filter
  }
}
