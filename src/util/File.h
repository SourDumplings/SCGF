#pragma once

#include <memory>
#include <utility>

namespace scgf::util
{
std::pair<std::unique_ptr<char[]>, size_t> read_file(const char* filename);

const char* get_executable_dir();
}  // namespace scgf::util