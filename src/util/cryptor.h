#pragma once

extern "C"
{
#include "crypto_algorithms/aes.h"	// From B-Con/crypto-algorithms
}

#include <memory>
#include <utility>	// For std::pair
#include "cryptor.h"
#include "singleton.h"

#define SCGF_AES_BLOCK_SIZE AES_BLOCK_SIZE

namespace scgf::util
{
class CCryptor final : public CSingleton<CCryptor>
{
public:
	std::pair<std::unique_ptr<char[]>, size_t> decrypt_scgf(const char* data, size_t dataSize)
		const;

private:
	int aes_decrypt_cbc(
		const BYTE in[],
		size_t in_len,
		BYTE out[],
		const AES_WORD key[],
		int keysize,
		const BYTE iv[]
	) const;

	// XORs the in and out buffers, storing the result in out. Length is in bytes.
	void xor_buf(const BYTE in[], BYTE out[], size_t len) const;

	int load_key();
};

}  // namespace scgf::util