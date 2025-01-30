#include "bm.hpp"

#include <exception>

#include "common.hpp"
#include "logger.hpp"

constexpr int US_TIME_LENGTH = 8;

BM::BM()
    : m_devNum(getDefaultDeviceNumber()), m_isMonitoring(false), m_filter(false), m_filteredBus('A'), m_filteredRt(-1),
      m_filteredSa(-1) {}

BM::~BM() { stop(); }

S16BIT BM::start(int devNum) {
  S16BIT status = ACE_ERR_SUCCESS;

  m_devNum = devNum;

  Logger::debug("Starting bus monitor with device: " + std::to_string(m_devNum));

  m_isMonitoring = false;

  if (m_monitorThread.joinable()) {
    m_monitorThread.join();
  }

  status = aceFree(static_cast<S16BIT>(m_devNum));

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  status = aceInitialize(static_cast<S16BIT>(m_devNum), ACE_ACCESS_CARD, ACE_MODE_MT, 0, 0, 0);

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  status = aceMTStart(static_cast<S16BIT>(m_devNum));

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  try {
    m_isMonitoring = true;
    m_monitorThread = std::thread([&] { monitor(); });
  } catch (std::exception &e) {
    Logger::error("Failed to start monitoring thread, " + std::string(e.what()));
  }

  return status;
}

S16BIT BM::stop() {
  S16BIT status = ACE_ERR_SUCCESS;

  Logger::debug("Stopping bus monitor with device: " + std::to_string(m_devNum));

  m_isMonitoring = false;

  if (m_monitorThread.joinable()) {
    m_monitorThread.join();
  }

  status = aceMTStop(static_cast<S16BIT>(m_devNum));

  if (status != ACE_ERR_SUCCESS and status != ACE_ERR_INVALID_STATE) {
    Logger::error(getStatus(status));
    return status;
  }

  status = aceFree(static_cast<S16BIT>(m_devNum));

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  return status;
}

Message BM::getMessage(MSGSTRUCT *msg) {
  U16BIT rt = 0;
  U16BIT sa = 0;
  U16BIT rtRx = -1;
  U16BIT saRx = -1;
  U16BIT wc = 0;
  U16BIT wTR1 = 0;
  U16BIT wTR2 = 0;
  bool noRes = true;

  aceCmdWordParse(msg->wCmdWrd1, &rt, &wTR1, &sa, &wc);

  if (msg->wCmdWrd2Flg != 0U) {
    aceCmdWordParse(msg->wCmdWrd2, &rtRx, &wTR2, &saRx, &wc);
  }

  std::ostringstream time;

  time << std::setw(US_TIME_LENGTH) << std::setfill('0') << (msg->wTimeTag3 * 2) << ":" << std::setw(US_TIME_LENGTH)
       << std::setfill('0') << (msg->wTimeTag2 * 2) << ":" << std::setw(US_TIME_LENGTH) << std::setfill('0')
       << (msg->wTimeTag * 2) << "us";

  if ((msg->wBlkSts & ACE_MT_BSW_ERRFLG) != 0) { // NOLINT(hicpp-signed-bitwise)
    if (std::strcmp(aceGetBSWErrString(ACE_MODE_MT, msg->wBlkSts), "NORES") != 0) {
      noRes = true;
    }
  }

  return Message(rt, sa, rtRx, saRx, wc,
                 (msg->wBlkSts & ACE_MT_BSW_CHNL) != 0 ? 'B' : 'A', // NOLINT(hicpp-signed-bitwise)
                 aceGetMsgTypeString(msg->wType), time.str(), msg->aDataWrds, msg->wCmdWrd2Flg != 0U, noRes);
}

void BM::monitor() {
  S16BIT status = 0;
  MSGSTRUCT sMsg;
  std::string messageBuffer;

  // Poll Messages
  while (m_isMonitoring) {
    status =
        aceMTGetStkMsgDecoded(static_cast<S16BIT>(m_devNum), &sMsg, ACE_MT_MSGLOC_NEXT_PURGE, ACE_MT_STKLOC_ACTIVE);

    if (status == ACE_ERR_MTI_EOB) {
      Message message = getMessage(&sMsg);

      std::string messageString =
          "Time: " + message.getTime() + "\t Bus: " + message.getBus() + "\t Type: " + message.getType() +
          "\t RT: " + std::to_string(message.getRt()) + "\t SA: " + std::to_string(message.getSa()) +
          (message.isCmdWord2()
               ? "\t RT2: " + std::to_string(message.getRtRx()) + "\t SA2: " + std::to_string(message.getSaRx())
               : "") +
          "\t WC: " + std::to_string(message.wc()) + (message.isResponded() ? "" : "\t (No Response)");

      messageString += "\nData: ";

      std::vector<std::string> data = message.getData();

      for (auto &d : data) {
        messageString += d + " ";
      }

      Logger::info("Bus Activity: \n " + messageString);

      m_updateSaState(message.getBus(), message.getRt(), message.getSa(), message.isResponded());

      if (m_filter and
          (m_filteredBus != message.getBus() or m_filteredRt != message.getRt() or m_filteredSa != message.getSa())) {
        continue;
      }

      m_updateMessages(messageString + "\n\n");
    }
  }
}
