#include "cryptor.h"

#include "logger.h"

namespace scgf::util
{
void CCryptor::xor_buf(const BYTE in[], BYTE out[], size_t len) const
{
	for (size_t idx = 0; idx < len; ++idx)
		out[idx] ^= in[idx];
}

int CCryptor::aes_decrypt_cbc(
	const BYTE in[],
	size_t in_len,
	BYTE out[],
	const AES_WORD key[],
	int keysize,
	const BYTE iv[]
) const
{
	BYTE buf_in[SCGF_AES_BLOCK_SIZE], buf_out[SCGF_AES_BLOCK_SIZE], iv_buf[SCGF_AES_BLOCK_SIZE];
	int blocks, idx;

	if (in_len % SCGF_AES_BLOCK_SIZE != 0) return 0;

	blocks = in_len / SCGF_AES_BLOCK_SIZE;

	memcpy(iv_buf, iv, SCGF_AES_BLOCK_SIZE);

	for (idx = 0; idx < blocks; ++idx)
	{
		memcpy(buf_in, &in[idx * SCGF_AES_BLOCK_SIZE], SCGF_AES_BLOCK_SIZE);
		aes_decrypt(buf_in, buf_out, key, keysize);
		xor_buf(iv_buf, buf_out, SCGF_AES_BLOCK_SIZE);
		memcpy(&out[idx * SCGF_AES_BLOCK_SIZE], buf_out, SCGF_AES_BLOCK_SIZE);
		memcpy(iv_buf, buf_in, SCGF_AES_BLOCK_SIZE);
	}

	return 1;
}

std::pair<std::unique_ptr<char[]>, size_t> CCryptor::decrypt_scgf(const char* data, size_t dataSize)
	const
{
	char iv[SCGF_AES_BLOCK_SIZE];
	std::copy(data, data + SCGF_AES_BLOCK_SIZE, iv);

	const size_t keySize = std::strlen(SCGF_AES_KEY);
	auto decryptedData	 = std::make_unique<char[]>(dataSize);
	AES_WORD keySchedule[60];
	const char* encryptedData = data + SCGF_AES_BLOCK_SIZE;
	size_t encryptedDataSize  = dataSize - SCGF_AES_BLOCK_SIZE;

	aes_key_setup(
		reinterpret_cast<const BYTE*>(SCGF_AES_KEY),
		keySchedule,
		keySize * 8	 // convert bytes num into bits num
	);

	// 进行AES CBC解密
	if (!aes_decrypt_cbc(
			reinterpret_cast<const BYTE*>(encryptedData),
			encryptedDataSize,
			reinterpret_cast<BYTE*>(decryptedData.get()),
			keySchedule,
			keySize * 8,
			reinterpret_cast<const BYTE*>(iv)
		))
	{
		SCGF_LOG_ERROR("AES decryption failed");
		return {nullptr, 0};
	}

	return {std::move(decryptedData), dataSize};
}

}  // namespace scgf::util