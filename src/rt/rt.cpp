#include "rt.hpp"

#include "logger.hpp"

RT::RT() : m_messageBuffer() {}

RT::~RT() { stop(); }

S16BIT RT::start(int devNum) {
  S16BIT status = ACE_ERR_SUCCESS;

  Logger::debug("Starting rt emulator with device: " + std::to_string(m_devNum));

  if (status != ACE_ERR_SUCCESS) {
    Logger::error(getStatus(status));
    return status;
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
