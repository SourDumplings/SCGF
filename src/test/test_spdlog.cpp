#include "test_spdlog.h"

#include <iostream>
#include <thread>
#include "util/logger.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"

int test_scgf_logger()
{
	while (true)
	{
		SCGF_LOG_TRACE("trace message");
		SCGF_LOG_DEBUG("debug message");
		SCGF_LOG_INFO("info message");
		SCGF_LOG_WARN("warn message");

		SCGF_STAT("stat message");

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}

void test_error_log()
{
	SCGF_LOG_TRACE("trace message");
	SCGF_LOG_DEBUG("debug message");
	SCGF_LOG_INFO("info message before");
	SCGF_LOG_ERROR("error message");
	SCGF_LOG_INFO("info message after");
}

void test_critical_log()
{
	SCGF_LOG_TRACE("trace message");
	SCGF_LOG_DEBUG("debug message");
	SCGF_LOG_INFO("info message before");
	SCGF_LOG_CRITICAL("critical message");
	SCGF_LOG_INFO("info message after");
}

void test_set_log_level()
{
	std::cout << "Current log level: "
			  << scgf::util::CLogger::get_instance().get_logic_logger_level() << std::endl;
	SCGF_LOG_TRACE("trace message1");
	SCGF_LOG_DEBUG("debug message1");
	scgf::util::CLogger::get_instance().set_logic_logger_level(scgf::util::CLogger::LogLevel::trace);
	std::cout << "Current log level: "
			  << scgf::util::CLogger::get_instance().get_logic_logger_level() << std::endl;
	SCGF_LOG_TRACE("trace message2");
	SCGF_LOG_DEBUG("debug message2");
	SCGF_STAT("stat message in trace");

	scgf::util::CLogger::get_instance().set_logic_logger_level(scgf::util::CLogger::LogLevel::debug);
	std::cout << "Current log level: "
			  << scgf::util::CLogger::get_instance().get_logic_logger_level() << std::endl;
	SCGF_LOG_TRACE("trace message3");
	SCGF_LOG_DEBUG("debug message3");

	SCGF_STAT("stat message in debug");
}

int test_spdlog()
{
	// test_scgf_logger();
	// test_error_log();
	// test_critical_log();
	test_set_log_level();
	return 0;
}