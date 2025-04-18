#include "rt.hpp"

#include "logger.hpp"
#include <string>

constexpr int DW_H_BUF_SIZE = (512 * ACE_MSGSIZE_RT) * 4 * 3; // TODO: review

RT::RT() {
  S16BIT status = ACE_ERR_SUCCESS;

  // Create Data Block Identifiers
  for (int rt = 0; rt < RT_SA_MAX_COUNT; ++rt) {
    for (int sa = 0; sa < RT_SA_MAX_COUNT; ++sa) {
      S16BIT dataBlockId = rt * 1000 + sa;

      status = aceRTDataBlkCreate(static_cast<S16BIT>(m_devNum), dataBlockId, ACE_RT_DBLK_C_128, nullptr, 0);

      if (status != ACE_ERR_SUCCESS) {
        Logger::error("Cannot create data block id for RT: " + std::to_string(rt) + ", SA: " + std::to_string(sa) +
                      getStatus(status));
      }

      // for (int saNum = 1; saNum <= 30; ++saNum) {
      status = acexMRTDataBlkMapToRTSA(static_cast<S16BIT>(m_devNum), 1, dataBlockId, 1, ACE_RT_MSGTYPE_RX,
                                       ACE_RT_DBLK_EOM_IRQ, 1);

      if (status != ACE_ERR_SUCCESS) {
        Logger::error("Cannot map data block id for RT: " + std::to_string(rt) + ", SA: " + std::to_string(sa) +
                      getStatus(status));
      }
    }
  }
}

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

  // status = aceRTInstallHBuf(static_cast<S16BIT>(m_devNum), DW_H_BUF_SIZE);

  // if (status != ACE_ERR_SUCCESS) {
  //   Logger::error(getStatus(status));
  //   return status;
  // }

  status = acexMRTStart(static_cast<S16BIT>(m_devNum), -1, 0);

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  return status;
}

S16BIT RT::stop() const {
  S16BIT status = ACE_ERR_SUCCESS;

  Logger::debug("Stopping rt emulator with device: " + std::to_string(m_devNum));

  status = acexMRTStop(static_cast<S16BIT>(m_devNum), -1);

  if (status != ACE_ERR_SUCCESS) {
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

S16BIT RT::setRt(int rt, int sa, int wc, U8BIT bus, std::array<std::string, RT_SA_MAX_COUNT> data) {
  S16BIT status = ACE_ERR_SUCCESS;

  // Concatenate data into a single string for logging
  std::string dataString;
  for (auto &d : data) {
    dataString += d + " ";
  }

  Logger::debug("Setting Data for RT: " + std::to_string(rt) + ", SA: " + std::to_string(sa) + " to: " + dataString);

  U16BIT messageBuffer[RT_SA_MAX_COUNT]; // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays,
                                         // cppcoreguidelines-avoid-c-arrays)

  // Convert string array to unsigned short array
  for (int i = 0; i < RT_SA_MAX_COUNT; ++i) {
    messageBuffer[i] = static_cast<unsigned short>( // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        strtoul(data.at(i).c_str(), nullptr, HEX_BYTE));
  }

  S16BIT dataBlockId = rt * 1000 + sa;
  status = aceRTDataBlkWrite(static_cast<S16BIT>(m_devNum), dataBlockId, messageBuffer, RT_SA_MAX_COUNT, 0);

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  return status;
}

S16BIT RT::activateRt(int rt) {
  S16BIT status = ACE_ERR_SUCCESS;

  Logger::debug("Activating RT: " + std::to_string(rt));

  status = acexMRTEnableRT(static_cast<S16BIT>(m_devNum), rt, 0);

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  return status;
}

S16BIT RT::deactivateRt(int rt) {
  S16BIT status = ACE_ERR_SUCCESS;

  Logger::debug("Deactivating RT: " + std::to_string(rt));

  status = acexMRTDisableRT(static_cast<S16BIT>(m_devNum), rt);

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  return status;
}
