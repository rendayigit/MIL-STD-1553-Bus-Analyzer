#include "logger.hpp"

#include "common.hpp"

void Logger::info(const std::string &message) { getLogger()->info(message); }

void Logger::error(const std::string &message) { getLogger()->error(message); }

void Logger::warn(const std::string &message) { getLogger()->warn(message); }

void Logger::critical(const std::string &message) { getLogger()->critical(message); }

void Logger::debug(const std::string &message) { getLogger()->debug(message); }

std::shared_ptr<spdlog::logger> Logger::getLogger() {
  static std::shared_ptr<spdlog::logger> logger = [] {
    auto log = spdlog::daily_logger_mt("1553 Bus Log", getExecutableDirectory() + "1553.log", 0, 0);
    log->set_pattern("[%H:%M:%S.%f %z] [%n] [%l] [thread %t] %v");
    log->set_level(spdlog::level::trace);
    log->flush_on(spdlog::level::trace);
    return log;
  }();

  return logger;
}
