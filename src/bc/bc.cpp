#include "bc.hpp"

#include <array>
#include <ios>
#include <iostream>
#include <string>

#include "common.hpp"
#include "logger.hpp"

constexpr int MOD_FLAGS = 0x000F;

constexpr int MSG_BC_TO_RT_ID = 1;
constexpr int MSG_RT_TO_BC_ID = 2;
constexpr int MSG_RT_TO_RT_ID = 3;

constexpr int DATA_BLK_BC_TO_RT_ID = 4;
constexpr int DATA_BLK_RT_TO_BC_ID = 5;
constexpr int DATA_BLK_RT_TO_RT_ID = 6;

constexpr int OP_CODE_1 = 1;
constexpr int OP_CODE_2 = 2;
constexpr int OP_CODE_3 = 3;
constexpr int OP_CODE_4 = 4;

constexpr int MNR_FRAME = 1;

constexpr int MJR_FRAME_1 = 2;
constexpr int MJR_FRAME_2 = 3;
constexpr int MJR_FRAME_3 = 4;

constexpr int MNR_FRAME_TIME = 1000;

BC::BC() : m_messageBuffer() {}

BC::~BC() { stop(); }

S16BIT BC::start(int devNum) {
  S16BIT status = ACE_ERR_SUCCESS;

  m_devNum = devNum;

  Logger::debug("Starting bus controller with device: " + std::to_string(m_devNum));

  status = aceFree(static_cast<S16BIT>(m_devNum));

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  status = aceInitialize(static_cast<S16BIT>(m_devNum), ACE_ACCESS_CARD, ACE_MODE_BC, 0, 0, 0);

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  U16BIT initialBuffer[RT_SA_MAX_COUNT] = {0x0000}; // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays,
                                                    // cppcoreguidelines-avoid-c-arrays)

  // Create BC -> RT data block
  status = aceBCDataBlkCreate(static_cast<S16BIT>(m_devNum), DATA_BLK_BC_TO_RT_ID, RT_SA_MAX_COUNT, initialBuffer,
                              RT_SA_MAX_COUNT);

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Create RT -> BC data block
  status = aceBCDataBlkCreate(static_cast<S16BIT>(m_devNum), DATA_BLK_RT_TO_BC_ID, RT_SA_MAX_COUNT, initialBuffer,
                              RT_SA_MAX_COUNT);

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Create RT -> RT data block
  status = aceBCDataBlkCreate(static_cast<S16BIT>(m_devNum), DATA_BLK_RT_TO_RT_ID, RT_SA_MAX_COUNT, initialBuffer,
                              RT_SA_MAX_COUNT);

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Create BC -> RT message block
  status = aceBCMsgCreateBCtoRT(static_cast<S16BIT>(m_devNum), MSG_BC_TO_RT_ID, DATA_BLK_BC_TO_RT_ID, 0, 0, 0, 0,
                                ACE_BCCTRL_CHL_A);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Create RT -> BC message block
  status = aceBCMsgCreateRTtoBC(static_cast<S16BIT>(m_devNum), MSG_RT_TO_BC_ID, DATA_BLK_RT_TO_BC_ID, 0, 0, 0, 0,
                                ACE_BCCTRL_CHL_A);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Create RT -> RT message block
  status = aceBCMsgCreateRTtoRT(static_cast<S16BIT>(m_devNum), MSG_RT_TO_RT_ID, DATA_BLK_RT_TO_RT_ID, 0, 0, 0, 0, 0, 0,
                                ACE_BCCTRL_CHL_A);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Create XEQ opcode that will use BC -> RT msg block
  status = aceBCOpCodeCreate(static_cast<S16BIT>(m_devNum), OP_CODE_1, ACE_OPCODE_XEQ, ACE_CNDTST_ALWAYS,
                             MSG_BC_TO_RT_ID, 0, 0);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Create XEQ opcode that will use RT -> BC msg block
  status = aceBCOpCodeCreate(static_cast<S16BIT>(m_devNum), OP_CODE_3, ACE_OPCODE_XEQ, ACE_CNDTST_ALWAYS,
                             MSG_RT_TO_BC_ID, 0, 0);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Create XEQ opcode that will use RT -> RT msg block
  status = aceBCOpCodeCreate(static_cast<S16BIT>(m_devNum), OP_CODE_4, ACE_OPCODE_XEQ, ACE_CNDTST_ALWAYS,
                             MSG_RT_TO_RT_ID, 0, 0);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Create CAL opcode that will call BC -> RT mnr frame from major
  status =
      aceBCOpCodeCreate(static_cast<S16BIT>(m_devNum), OP_CODE_2, ACE_OPCODE_CAL, ACE_CNDTST_ALWAYS, MNR_FRAME, 0, 0);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  S16BIT aOpCodes[10] = {0x0000}; // NOLINT(hicpp-avoid-c-arrays, modernize-avoid-c-arrays,
                                  // cppcoreguidelines-avoid-c-arrays)

  // Create BC -> RT Minor Frame
  aOpCodes[0] = OP_CODE_1;
  status = aceBCFrameCreate(static_cast<S16BIT>(m_devNum), MNR_FRAME, ACE_FRAME_MINOR, aOpCodes, 1, 0, 0);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Create BC -> RT Major Frame
  aOpCodes[0] = OP_CODE_2;
  status =
      aceBCFrameCreate(static_cast<S16BIT>(m_devNum), MJR_FRAME_1, ACE_FRAME_MAJOR, aOpCodes, 1, MNR_FRAME_TIME, 0);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Create RT -> BC Major Frame
  aOpCodes[0] = OP_CODE_3;
  status =
      aceBCFrameCreate(static_cast<S16BIT>(m_devNum), MJR_FRAME_2, ACE_FRAME_MAJOR, aOpCodes, 1, MNR_FRAME_TIME, 0);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Create RT -> RT Major Frame
  aOpCodes[0] = OP_CODE_4;
  status =
      aceBCFrameCreate(static_cast<S16BIT>(m_devNum), MJR_FRAME_3, ACE_FRAME_MAJOR, aOpCodes, 1, MNR_FRAME_TIME, 0);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  return status;
}

S16BIT BC::stop() const {
  S16BIT status = ACE_ERR_SUCCESS;

  Logger::debug("Stopping bus controller with device: " + std::to_string(m_devNum));

  status = aceBCStop(static_cast<S16BIT>(m_devNum));

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

S16BIT BC::bcToRt(int rt, int sa, int wc, U8BIT bus, std::array<std::string, RT_SA_MAX_COUNT> data) {
  S16BIT status = ACE_ERR_SUCCESS;

  std::string log = "Sending Frame, Mode: BC->RT, RT: " + std::to_string(rt) + ", SA: " + std::to_string(sa) +
                    ", WC: " + std::to_string(wc) + ", BUS: " + (bus == ACE_BCCTRL_CHL_A ? "A" : "B") + ", DATA:";

  for (int i = 0; i < data.size(); ++i) {
    if (i % DATA_OCTET == 0) {
      log += "\n\t " + data.at(i);
    } else {
      log += "   " + data.at(i);
    }
  }

  Logger::debug(log);

  status = aceBCMsgModifyBCtoRT(static_cast<S16BIT>(m_devNum), MSG_BC_TO_RT_ID, DATA_BLK_BC_TO_RT_ID, rt, sa, wc, 0,
                                bus, MOD_FLAGS);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Convert string array to unsigned short array
  for (int i = 0; i < RT_SA_MAX_COUNT; ++i) {
    m_messageBuffer[i] = static_cast<unsigned short>( // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        strtoul(data.at(i).c_str(), nullptr, HEX_BYTE));
  }

  status = aceBCDataBlkWrite(static_cast<S16BIT>(m_devNum), DATA_BLK_BC_TO_RT_ID, m_messageBuffer, RT_SA_MAX_COUNT, 0);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Start BC
  status = aceBCStart(static_cast<S16BIT>(m_devNum), MJR_FRAME_1, 1);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  return ACE_ERR_SUCCESS;
}

S16BIT BC::rtToBc(int rt, int sa, int wc, U8BIT bus, std::array<std::string, RT_SA_MAX_COUNT> *data) {
  S16BIT status = ACE_ERR_SUCCESS;

  std::string log = "Sending Frame, Mode: RT->BC, RT: " + std::to_string(rt) + ", SA: " + std::to_string(sa) +
                    ", WC: " + std::to_string(wc) + ", BUS: " + (bus == ACE_BCCTRL_CHL_A ? "A" : "B");

  Logger::debug(log);

  status = aceBCMsgModifyRTtoBC(static_cast<S16BIT>(m_devNum), MSG_RT_TO_BC_ID, DATA_BLK_RT_TO_BC_ID, rt, sa, wc, 0,
                                bus, MOD_FLAGS);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Start BC
  status = aceBCStart(static_cast<S16BIT>(m_devNum), MJR_FRAME_2, 1);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  S16BIT readCount =
      aceBCDataBlkRead(static_cast<S16BIT>(m_devNum), DATA_BLK_RT_TO_BC_ID, m_messageBuffer, RT_SA_MAX_COUNT, 0);
  if (readCount < 0) {
    Logger::error(getStatus(readCount));
    return readCount;
  }

  for (int i = 0; i < RT_SA_MAX_COUNT; ++i) {
    std::stringstream ss;
    ss << std::hex << std::setw(4) << std::uppercase << std::setfill('0')
       << m_messageBuffer[i]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)

    data->at(i) = ss.str();
  }

  return ACE_ERR_SUCCESS;
}

S16BIT BC::rtToRt(int rtTx, int saTx, int rtRx, int saRx, int wc, U8BIT bus,
                  std::array<std::string, RT_SA_MAX_COUNT> *data) {
  S16BIT status = ACE_ERR_SUCCESS;

  std::string log = "Sending Frame, Mode: RT->RT, RT TX: " + std::to_string(rtTx) + ", RT RX: " + std::to_string(rtRx) +
                    ", SA TX: " + std::to_string(saTx) + ", SA RX: " + std::to_string(saRx) +
                    ", WC: " + std::to_string(wc) + ", BUS: " + (bus == ACE_BCCTRL_CHL_A ? "A" : "B");

  Logger::debug(log);

  status = aceBCMsgModifyRTtoRT(static_cast<S16BIT>(m_devNum), MSG_RT_TO_RT_ID, DATA_BLK_RT_TO_RT_ID, rtRx, saRx, wc,
                                rtTx, saTx, 0, bus, MOD_FLAGS);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  // Start BC
  status = aceBCStart(static_cast<S16BIT>(m_devNum), MJR_FRAME_3, 1);
  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
  }

  S16BIT readCount =
      aceBCDataBlkRead(static_cast<S16BIT>(m_devNum), DATA_BLK_RT_TO_RT_ID, m_messageBuffer, RT_SA_MAX_COUNT, 0);
  if (readCount < 0) {
    Logger::error(getStatus(readCount));
    return readCount;
  }

  for (int i = 0; i < RT_SA_MAX_COUNT; ++i) {
    std::stringstream ss;
    ss << std::hex << std::setw(4) << std::uppercase << std::setfill('0')
       << m_messageBuffer[i]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)

    data->at(i) = ss.str();
  }

  return ACE_ERR_SUCCESS;
}
