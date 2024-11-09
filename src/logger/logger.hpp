#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <map>
#include <mutex>
#include <string>

#define LOG_BUFFER__DEFAULT_LIMIT 50 // Limit for the log buffer before flushing

// Enum to represent different levels of logging
enum LogLevel {
  LOG_INFO,    // Informational messages
  LOG_WARNING, // Warning messages
  LOG_ERROR,   // Error messages
};

/**
 * Logger class
 */
class Logger {
public:
  /**
   * @brief Construct a new Logger object
   *
   * @param logFilePath Full path of the log file
   * @param logBufferLimit Buffer limit to flush logs
   */
  explicit Logger(std::string logFilePath = "", int logBufferLimit = LOG_BUFFER__DEFAULT_LIMIT);

  /**
   * @brief Destroy the Logger object
   *
   */
  ~Logger();

  /**
   * @brief Log a message
   *
   * @param level The level of the message
   * @param message The message to log
   */
  void log(LogLevel level, const std::string &message);

  /**
   * @brief Flush the log buffer
   *
   */
  void flush();

private:
  std::mutex m_mutex;         // Mutex to ensure thread-safe access to the logger
  std::ofstream m_logFile;    // File stream to write log messages
  std::string m_logBuffer;    // Buffer to hold log messages before they are written to the file
  std::string m_logFilePath;  // Full path of the log file
  std::string m_logFileName;  // Name of the log file
  std::string m_logDirectory; // Directory of the log file
  int m_logBufferLimit;       // Buffer limit to flush logs
};

#endif // LOGGER_HPP