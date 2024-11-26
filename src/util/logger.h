#pragma once

#include <memory>
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include <cstdlib>
#include "singleton.h"

// Log
#define SCGF_LOG_TRACE(...) \
	SPDLOG_LOGGER_TRACE(scgf::util::CLogger::get_instance().get_logic_logger(), __VA_ARGS__)
#define SCGF_LOG_DEBUG(...) \
	SPDLOG_LOGGER_DEBUG(scgf::util::CLogger::get_instance().get_logic_logger(), __VA_ARGS__)
#define SCGF_LOG_INFO(...) \
	SPDLOG_LOGGER_INFO(scgf::util::CLogger::get_instance().get_logic_logger(), __VA_ARGS__)
#define SCGF_LOG_WARN(...) \
	SPDLOG_LOGGER_WARN(scgf::util::CLogger::get_instance().get_logic_logger(), __VA_ARGS__)

// Error log will abort the program after logging in DEBUG mode!
#ifdef DEBUG
#define SCGF_LOG_ERROR(...)                                                                  \
	SPDLOG_LOGGER_ERROR(scgf::util::CLogger::get_instance().get_logic_logger(), __VA_ARGS__); \
	std::exit(EXIT_FAILURE)
#else
#define SCGF_LOG_ERROR(...) \
	SPDLOG_LOGGER_ERROR(scgf::util::CLogger::get_instance().get_logic_logger(), __VA_ARGS__)
#endif

// Critical log will abort the program after logging!
#define SCGF_LOG_CRITICAL(...)                                                                  \
	SPDLOG_LOGGER_CRITICAL(scgf::util::CLogger::get_instance().get_logic_logger(), __VA_ARGS__); \
	std::exit(EXIT_FAILURE)

// TODO use proto generate stat log msg
#define SCGF_STAT(logMsg) \
	SPDLOG_LOGGER_INFO(scgf::util::CLogger::get_instance().get_stat_logger(), logMsg)

namespace scgf::util
{
class CLogger final : public CSingleton<CLogger>
{
public:
	using LogLevel = spdlog::level::level_enum;

	CLogger();
	virtual ~CLogger() override;

	std::shared_ptr<spdlog::logger>& get_logic_logger() { return m_logicLogger; }
	std::shared_ptr<spdlog::logger>& get_stat_logger() { return m_statLogger; }

	spdlog::level::level_enum get_logic_logger_level() { return m_logicLogger->level(); }
	void set_logic_logger_level(LogLevel logLevel) { m_logicLogger->set_level(logLevel); }

private:
	const char* LOGIC_LOG_PATTERN = "[%Y-%m-%d %H:%M:%S.%e][%l][%t][%@][%!] %v";

	// 00:00 per day rolling
	const uint8_t DAILY_ROLLING_HOUR   = 0;
	const uint8_t DAILY_ROLLING_MINUTE = 0;

	const uint8_t FLUSH_INTERVAL_SECONDS = 30;

#ifdef DEBUG
	const spdlog::level::level_enum DEFAULT_LOG_LEVEL =
		static_cast<spdlog::level::level_enum>(SPDLOG_LEVEL_DEBUG);
#else
	const spdlog::level::level_enum DEFAULT_LOG_LEVEL =
		static_cast<spdlog::level::level_enum>(SPDLOG_LEVEL_INFO);
#endif

	std::shared_ptr<spdlog::logger> m_logicLogger;
	std::shared_ptr<spdlog::logger> m_statLogger;
};
}  // namespace scgf::util
