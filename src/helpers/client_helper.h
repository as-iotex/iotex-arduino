#ifndef CLIENT_HELPERS_H
#define CLIENT_HELPERS_H

#include "extern/cpplogger/cpplogger.h"
#include "IoTeXResultCodes.h"
#include <cstring>
#include <string>
#include <vector>

// clang-format off

/**************************************************************************/
/* Set default log level */
/**************************************************************************/

#define IOTEX_DEFAULT_LOG_LEVEL cpplogger::LogLevel::TRACE
#ifndef IOTEX_LOG_LEVEL
	#define IOTEX_LOG_LEVEL IOTEX_DEFAULT_LOG_LEVEL
#endif

/**************************************************************************/
/* Conditinally include board specific files and add board specific flags */
/**************************************************************************/

#if defined(ARDUINO) || defined(ESP8266) || defined(ESP32) || defined(__SAMD21G18A__)
	#include <Arduino.h>
	#define IotexString String	// Use Arduino String intead of std::string in Arduino
#else
	#define IotexString std::string
	#ifndef OS
		#define OS
	#endif
#endif

/**************************************************************************/
/* Helper macros for logging a message with different levels */
/**************************************************************************/
#define IOTEX_DEBUG(module, ...) IotexHelpers.logger.LOG_MSG(cpplogger::LogLevel::DEBUG, module, __VA_ARGS__)
#define IOTEX_INFO(module, ...) IotexHelpers.logger.LOG_MSG(cpplogger::LogLevel::INFO, module, __VA_ARGS__)
#define IOTEX_WARNING(module, ...) IotexHelpers.logger.LOG_MSG(cpplogger::LogLevel::WARNING, module, __VA_ARGS__)
#define IOTEX_TRACE(module, ...) IotexHelpers.logger.LOG_MSG(cpplogger::LogLevel::TRACE, module, __VA_ARGS__)
#define IOTEX_ERROR(module, ...) IotexHelpers.logger.LOG_MSG(cpplogger::LogLevel::ERROR, module, __VA_ARGS__)

/**************************************************************************/
/* Helper macros for logging a hex dump of a buffer with different levels */
/**************************************************************************/

#define IOTEX_DEBUG_BUF(module, buf, size) IotexHelpers.logger.LOG_BUF(cpplogger::LogLevel::DEBUG, module, buf, size)
#define IOTEX_INFO_BUF(module, buf, size) IotexHelpers.logger.LOG_BUF(cpplogger::LogLevel::INFO, module, buf, size)
#define IOTEX_WARNING_BUF(module, buf, size) IotexHelpers.logger.LOG_BUF(cpplogger::LogLevel::WARNING, module, buf, size)
#define IOTEX_TRACE_BUF(module, buf, size) IotexHelpers.logger.LOG_BUF(cpplogger::LogLevel::TRACE, module, buf, size)
#define IOTEX_ERROR_BUF(module, buf, size) IotexHelpers.logger.LOG_BUF(cpplogger::LogLevel::ERROR, module, buf, size)

/**************************************************************************/
/* Helper macros for logging a flash string with different levels (for ESP) */
/**************************************************************************/

#if !defined(ESP8266) && !defined(ESP32)
	#define IOTEX_FLASH_STRING_SUPPORT false
#else
	#define IOTEX_FLASH_STRING_SUPPORT true
#endif

#if !IOTEX_FLASH_STRING_SUPPORT
	#define IOTEX_DEBUG_F(module, ...) IOTEX_DEBUG(module, __VA_ARGS__)
	#define IOTEX_INFO_F(module, ...) IOTEX_INFO(module, __VA_ARGS__)
	#define IOTEX_WARNING_F(module, ...) IOTEX_WARNING(module, __VA_ARGS__)
	#define IOTEX_TRACE_F(module, ...) IOTEX_TRACE(module, __VA_ARGS__)
	#define IOTEX_ERROR_F(module, ...) IOTEX_ERROR(module, __VA_ARGS__)
#else
	#define IOTEX_DEBUG_F(module, msg) IotexHelpers.logger.LOG_PROGMEM_STRING(cpplogger::LogLevel::DEBUG, module, msg)
	#define IOTEX_INFO_F(module, msg) IotexHelpers.logger.LOG_PROGMEM_STRING(cpplogger::LogLevel::DEBUG, module, msg)
	#define IOTEX_WARNING_F(module, msg) IotexHelpers.logger.LOG_PROGMEM_STRING(cpplogger::LogLevel::DEBUG, module, msg)
	#define IOTEX_TRACE_F(module, msg) IotexHelpers.logger.LOG_PROGMEM_STRING(cpplogger::LogLevel::DEBUG, module, msg)
	#define IOTEX_ERROR_F(module, msg) IotexHelpers.logger.LOG_PROGMEM_STRING(cpplogger::LogLevel::DEBUG, module, msg)
#endif
// clang-format on

namespace iotex
{
/**
 * @brief Enum that contains the different logging modules of the library
 *
 */
enum LogModules
{
	GENERAL,
	HTTP,
	CONTRACT,
	VALES_COUNT
};

/**
 * @brief General log module name exposed to the user, so the logger can be used by user code if
 * needed
 *
 */
extern const std::string& generalLogModule;

/**
 * @brief Lookup table for log module names. This should be only used by the internal library code
 *
 */
extern const std::string logModuleNamesLookupTable[(int)LogModules::VALES_COUNT];

/**
 * @brief Enum representing all the log levels
 *
 */
enum IotexLogLevel
{
	NONE,
	ERROR,
	WARNING,
	INFO,
	DEBUG,
	TRACE
};

/**
 * @brief Class that contains helper methods
 *
 */
class Helpers
{
  public:
	Helpers();
	cpplogger::Logger logger;

	/**
	 * @brief Gets the hex string representation of a vector of bytes
	 *
	 * @param data
	 * @param[out] out
	 */
	void vectorToHexString(std::vector<uint8_t>& data, IotexString& out);

	/**
	 * @brief Gets the result strinf value of a ResultCode
	 *
	 * @param code
	 * @return[out] const char*
	 */
	const char* GetResultString(ResultCode code);

	/**
	 * @brief Sets the log level globally. Overrides log level modules if they are of a higher level
	 *
	 * @param level
	 */
	void setGlobalLogLevel(IotexLogLevel level);

	/**
	 * @brief Set the Mlog level for a module. Note that the global lov level can override a module
	 * log level
	 *
	 * @param module
	 * @param level
	 */
	void setModuleLogLevel(const std::string& module, IotexLogLevel level);
};
} // namespace iotex

/**
 * @brief IotexHelpers global object
 *
 */
extern iotex::Helpers IotexHelpers;

#endif
