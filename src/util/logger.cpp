#include "logger.h"

#include <iostream>
#include <memory>
#include <vector>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"
#include <iostream>
#include <filesystem>
#include "file_helper.h"

namespace scgf::util
{
CLogger::CLogger()
{
#ifdef DEBUG
	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	consoleSink->set_level(LogLevel::trace);
	consoleSink->set_pattern(LOGIC_LOG_PATTERN);
#endif

	std::filesystem::path cwd = scgf::util::get_executable_dir();

#ifdef PROJECT_NAME
	const std::string LOGIC_LOG_FILE_PATH =
		(cwd / std::filesystem::path("../log/" PROJECT_NAME ".log")).string();
	const std::string STAT_LOG_FILE_PATH =
		(cwd / std::filesystem::path("../log/" PROJECT_NAME ".stat")).string();
#else
	const std::string LOGIC_LOG_FILE_PATH =
		(cwd / std::filesystem::path("../log/scgf.log")).string();
	const std::string STAT_LOG_FILE_PATH =
		(cwd / std::filesystem::path("../log/scgf.stat")).string();
#endif

	auto logicLogFileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
		LOGIC_LOG_FILE_PATH.c_str(),
		DAILY_ROLLING_HOUR,
		DAILY_ROLLING_MINUTE
	);
	logicLogFileSink->set_level(LogLevel::trace);
	logicLogFileSink->set_pattern(LOGIC_LOG_PATTERN);

	auto statLogFileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
		STAT_LOG_FILE_PATH.c_str(),
		DAILY_ROLLING_HOUR,
		DAILY_ROLLING_MINUTE
	);
	statLogFileSink->set_level(LogLevel::trace);
	statLogFileSink->set_pattern("%v");

#ifdef DEBUG
	std::vector<spdlog::sink_ptr> logicLogSinks{consoleSink, logicLogFileSink};
	std::vector<spdlog::sink_ptr> statLogSinks{consoleSink, statLogFileSink};
#else
	std::vector<spdlog::sink_ptr> logicLogSinks{logicLogFileSink};
	std::vector<spdlog::sink_ptr> statLogSinks{statLogFileSink};
#endif

	m_logicLogger =
		std::make_shared<spdlog::logger>("logicLogger", logicLogSinks.begin(), logicLogSinks.end());
	m_statLogger =
		std::make_shared<spdlog::logger>("statLogger", statLogSinks.begin(), statLogSinks.end());

	m_logicLogger->set_level(DEFAULT_LOG_LEVEL);
	m_statLogger->set_level(DEFAULT_LOG_LEVEL);

	spdlog::register_logger(m_logicLogger);
	spdlog::register_logger(m_statLogger);

	spdlog::flush_every(std::chrono::seconds(FLUSH_INTERVAL_SECONDS));

	// flush immediately when getting err or worse level log
	spdlog::flush_on(spdlog::level::err);
}

CLogger::~CLogger() { spdlog::shutdown(); }
}  // namespace scgf::util
