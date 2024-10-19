#pragma once

#include <memory>
#include <utility>	// For std::pair

#include "Singleton.h"

#define BUFFER_SIZE 0xFFFF

namespace scgf::util
{
class Compressor final : public Singleton<Compressor>
{
public:
	std::pair<std::unique_ptr<char[]>, size_t> compress(const char* data, size_t dataSize);

	std::pair<std::unique_ptr<char[]>, size_t>
		decompress(const char* compressedData, size_t compressedDataSize);

private:
	char m_buf[BUFFER_SIZE];
};

}  // namespace scgf::util