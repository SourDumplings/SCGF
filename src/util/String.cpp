#pragma once

#include <regex>
#include <string>

namespace scgf::util
{
void replace(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty()) return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();  // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

void clean_invalid_content(std::string& str)
{
	std::regex illegalChars("[^\\x00-\\x7F]+");
	str = std::regex_replace(str, illegalChars, "");
}
}  // namespace scgf::util