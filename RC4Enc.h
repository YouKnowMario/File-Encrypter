#pragma once

namespace RC4Enc
{
	const char* encrypt(const char* plainText, const char* password, const int plainTextSize, const int passSize);
	const char* decrypt(const char* cipher, const char* password, const int cipherSize, const int passSize);
	void __swap(unsigned char* S, unsigned char* T, const int i, const int j);
	void __ksa(const char* password, const int passSize, unsigned char* S, unsigned char* T);
	const char* __prga(const char* plainText, const int plainTextSize, unsigned char* S);
}


