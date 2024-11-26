#include "file_helper.h"

#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include "logger.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <limits.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <limits.h>
#endif

namespace scgf::util
{
std::pair<std::unique_ptr<char[]>, size_t> read_file(const char* filename)
{
	std::ifstream file(filename, std::ios::binary);

	if (!file)
	{
		SCGF_LOG_ERROR("Failed to open file: " + std::string(filename));
		return {nullptr, 0};
	}

	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	auto buffer = std::make_unique<char[]>(fileSize);
	file.read(reinterpret_cast<char*>(buffer.get()), fileSize);

	return std::make_pair(std::move(buffer), static_cast<size_t>(fileSize));
}

const char* get_executable_dir()
{
	static std::string exeDir;

#if defined(_WIN32) || defined(_WIN64)
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	exeDir					   = std::string(buffer).substr(0, pos);
#elif defined(__linux__)
	char buffer[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	if (len != -1)
	{
		buffer[len]				   = '\0';
		std::string::size_type pos = std::string(buffer).find_last_of("\\/");
		exeDir					   = std::string(buffer).substr(0, pos);
	}
#elif defined(__APPLE__)
	char buffer[PATH_MAX];
	uint32_t size = sizeof(buffer);
	if (_NSGetExecutablePath(buffer, &size) == 0)
	{
		char realPath[PATH_MAX];
		realpath(buffer, realPath);
		std::string::size_type pos = std::string(realPath).find_last_of("\\/");
		exeDir					   = std::string(realPath).substr(0, pos);
	}
#endif

	return exeDir.c_str();
}
}  // namespace scgf::util