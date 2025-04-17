#include "rt.hpp"

#include "logger.hpp"

RT::RT() : m_messageBuffer() {}

RT::~RT() { stop(); }

S16BIT RT::start(int devNum) {
  S16BIT status = ACE_ERR_SUCCESS;

  Logger::debug("Starting rt emulator with device: " + std::to_string(m_devNum));

  status = aceFree(static_cast<S16BIT>(m_devNum));

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  status = aceInitialize(static_cast<S16BIT>(m_devNum), ACE_ACCESS_CARD, ACE_MODE_MRT, 0, 0, 0);

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  U16BIT u16GblDStkID = 2; // TODO: what is this?
  status = acexMRTConfigure(static_cast<S16BIT>(m_devNum), ACE_RT_CMDSTK_2K, ACE_RT_DBLK_GBL_C_128, u16GblDStkID);

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  int id = 3; // TODO: what is this?
  status = aceRTDataBlkCreate(static_cast<S16BIT>(m_devNum), id, ACE_RT_DBLK_C_128, NULL, 0);

  std::array<std::string, RT_SA_MAX_COUNT> data = {"AAAA", "BBBB", "CCCC", "DDDD", "1234", "0000", "0000", "0000",
                                                   "0000", "0000", "0000", "0000", "0000", "0000", "0000", "0000",
                                                   "0000", "0000", "0000", "0000", "0000", "0000", "0000", "0000",
                                                   "0000", "0000", "0000", "0000", "0000", "0000", "0000", "0000"};

  // Convert string array to unsigned short array
  for (int i = 0; i < RT_SA_MAX_COUNT; ++i) {
    m_messageBuffer[i] = static_cast<unsigned short>( // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        strtoul(data.at(i).c_str(), nullptr, HEX_BYTE));
  }

  status = aceRTDataBlkWrite(static_cast<S16BIT>(m_devNum), id, m_messageBuffer, RT_SA_MAX_COUNT, 0);

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  for (int rtNum = 0; rtNum < RT_SA_MAX_COUNT; ++rtNum) {
    // for (int saNum = 0; saNum < RT_SA_MAX_COUNT; ++saNum) {
    for (int saNum = 1; saNum <= 30; ++saNum) { // TODO: Try above code
      status = acexMRTDataBlkMapToRTSA(static_cast<S16BIT>(m_devNum), rtNum, id, saNum, ACE_RT_MSGTYPE_RX,
                                       ACE_RT_DBLK_EOM_IRQ, 1);

      if (status != ACE_ERR_SUCCESS) {
        Logger::error(getStatus(status));
        return status;
      }
    }
  }

  return status;
}

S16BIT RT::stop() const {
  S16BIT status = ACE_ERR_SUCCESS;

  Logger::debug("Stopping rt emulator with device: " + std::to_string(m_devNum));

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  return status;
}

S16BIT RT::setRt(int rt, int sa, int wc, U8BIT bus, std::array<std::string, RT_SA_MAX_COUNT> data) {
  S16BIT status = ACE_ERR_SUCCESS;

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  return status;
}

S16BIT RT::activateRt(int rt, int sa) {
  S16BIT status = ACE_ERR_SUCCESS;

  Logger::debug("Activating RT:SA: " + std::to_string(rt) + ":" + std::to_string(sa));

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  status = acexMRTEnableRT(static_cast<S16BIT>(m_devNum), rt, 0);

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  return status;
}

S16BIT RT::deactivateRt(int rt, int sa) {
  S16BIT status = ACE_ERR_SUCCESS;

  Logger::debug("Deactivating RT:SA: " + std::to_string(rt) + ":" + std::to_string(sa));

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  return status;
}
