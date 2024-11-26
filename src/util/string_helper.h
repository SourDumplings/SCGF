#pragma once

#include <string>

namespace scgf::util
{
void replace(std::string& str, const std::string& from, const std::string& to);

void clean_invalid_content(std::string& str);
}