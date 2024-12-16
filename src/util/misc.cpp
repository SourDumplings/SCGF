#include "misc.h"

#include <sstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <filesystem>

#include "util/logger.h"
#include "SCGF.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "util/file_helper.h"

namespace scgf::util
{
void print_envir_info()
{
	std::ostringstream oss;
	oss << std::endl;
	oss << "-------------- " PROJECT_NAME " envir info BEGIN -------------" << std::endl;
	oss << "Version: " SCGF_VERSION << std::endl;

	std::filesystem::path cwd = scgf::util::get_executable_dir();
	oss << "Current working directoy: " << cwd << std::endl;

	// Context
#ifdef _WIN32
	DWORD processID = GetCurrentProcessId();
#else
	pid_t processID = getpid();
#endif

	oss << "Current process ID: " << processID << std::endl;

	std::thread::id threadID = std::this_thread::get_id();
	oss << "Current thread ID: " << threadID << std::endl;

	// Time
	auto		now		 = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);
	std::tm		local_tm = *std::localtime(&now_time);
	oss << "Current time: " << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S ");
	char buf[16];
	strftime(buf, sizeof(buf), "%z", &local_tm);
	oss << "Time zone: " << buf << std::endl;

	// OS
#ifdef _WIN32
	oss << "Operating System: Windows" << std::endl;
#elif __linux__
	oss << "Operating System: Linux" << std::endl;
#elif __APPLE__
	oss << "Operating System: macOS" << std::endl;
#else
	oss << "Operating System: Unknown" << std::endl;
#endif

	// CPU
	oss << "CPU Architecture: ";
#if defined(__x86_64__) || defined(_M_X64)
	oss << "x86-64";
#elif defined(__i386) || defined(_M_IX86)
	oss << "x86";
#elif defined(__arm__) || defined(_M_ARM)
	oss << "ARM";
#elif defined(__aarch64__) || defined(_M_ARM64)
	oss << "ARM64";
#else
	oss << "Unknown";
#endif

	oss << ", " << sizeof(void *) * 8 << "-bit" << std::endl;

	// Compiler
	oss << "Compiler: ";

#ifdef __clang__
	oss << "Clang";
#elif defined(__GNUC__)
	oss << "GCC";
#elif defined(_MSC_VER)
	oss << "MSVC";
#else
	oss << "Unknown";
#endif

	oss << ", Version: ";

#if defined(__clang__)
	oss << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__;
#elif defined(__GNUC__)
	oss << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__;
#elif defined(_MSC_VER)
	oss << _MSC_VER;
#else
	oss << "Unknown";
#endif

	oss << std::endl;

	// Cpp
	oss << "Cpp version: " << __cplusplus << std::endl;

	// Build
	oss << "Build type:  " << CMAKE_BUILD_TYPE << std::endl;
	
	if (DEPLOY_TARGET_ENUM == eDEPLOY_TARGET_GAME_CLIENT)
	{
		oss << "Deploy target: Game Client" << std::endl;
	}
	else if (DEPLOY_TARGET_ENUM == eDEPLOY_TARGET_GAME_SERVER)
	{
		oss << "Deploy target: Game Server" << std::endl;
	}

	oss << "-------------- " PROJECT_NAME " envir info END -------------" << std::endl;
	SCGF_LOG_INFO(oss.str());
}
}  // namespace scgf::util
