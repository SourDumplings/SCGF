#include "compressor.h"

#include <cstring>
#include "logger.h"
#include "zlib.h"

namespace scgf::util
{
std::pair<std::unique_ptr<char[]>, size_t> CCompressor::compress(const char* data, size_t dataSize)
{
	z_stream zs;
	memset(&zs, 0, sizeof(zs));

	if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK)
	{
		SCGF_LOG_ERROR("Failed to initialize zlib for compression");
		return std::make_pair(nullptr, 0);
	}

	zs.next_in	= (Bytef*)data;
	zs.avail_in = dataSize;

	size_t totalSize = 0;
	int ret;
	std::unique_ptr<char[]> outCompressedData;

	do
	{
		zs.next_out	 = reinterpret_cast<Bytef*>(m_buf);
		zs.avail_out = sizeof(m_buf);

		ret = deflate(&zs, Z_FINISH);

		auto newSize = zs.total_out;
		std::unique_ptr<char[]> newBuffer(new char[newSize]);

		if (!newBuffer)
		{
			deflateEnd(&zs);
			SCGF_LOG_ERROR("Memory allocation failed");
			return std::make_pair(nullptr, 0);
		}

		if (outCompressedData)
		{
			std::copy(
				outCompressedData.get(),
				outCompressedData.get() + totalSize,
				newBuffer.get()
			);
		}
		std::copy(m_buf, m_buf + (newSize - totalSize), newBuffer.get() + totalSize);

		outCompressedData = std::move(newBuffer);
		totalSize		  = newSize;

	} while (ret == Z_OK);

	deflateEnd(&zs);

	if (ret != Z_STREAM_END)
	{
		SCGF_LOG_ERROR("Failed to compress data");
		return std::make_pair(nullptr, 0);
	}

	return std::make_pair(std::move(outCompressedData), totalSize);
}

std::pair<std::unique_ptr<char[]>, size_t>
	CCompressor::decompress(const char* compressedData, size_t compressedDataSize)
{
	z_stream zs;
	memset(&zs, 0, sizeof(zs));

	if (inflateInit(&zs) != Z_OK)
	{
		SCGF_LOG_ERROR("Failed to initialize zlib for decompression");
		return std::make_pair(nullptr, 0);
	}

	zs.next_in	= (Bytef*)compressedData;
	zs.avail_in = compressedDataSize;

	size_t totalSize = 0;
	int ret;
	std::unique_ptr<char[]> outDecompressedData;

	do
	{
		zs.next_out	 = reinterpret_cast<Bytef*>(m_buf);
		zs.avail_out = sizeof(m_buf);

		ret = inflate(&zs, 0);

		auto newSize = zs.total_out;
		std::unique_ptr<char[]> newBuffer(new char[newSize]);

		if (!newBuffer)
		{
			inflateEnd(&zs);
			SCGF_LOG_ERROR("Memory allocation failed");
			return std::make_pair(nullptr, 0);
		}

		if (outDecompressedData)
		{
			std::copy(
				outDecompressedData.get(),
				outDecompressedData.get() + totalSize,
				newBuffer.get()
			);
		}
		std::copy(m_buf, m_buf + (newSize - totalSize), newBuffer.get() + totalSize);

		outDecompressedData = std::move(newBuffer);
		totalSize			= newSize;

	} while (ret == Z_OK);

	inflateEnd(&zs);

	if (ret != Z_STREAM_END)
	{
		SCGF_LOG_ERROR("Failed to decompress data");
		return std::make_pair(nullptr, 0);
	}

	return std::make_pair(std::move(outDecompressedData), totalSize);
}
}  // namespace scgf::util