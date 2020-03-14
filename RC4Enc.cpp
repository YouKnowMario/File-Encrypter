#include "RC4Enc.h"

namespace RC4Enc
{
	const char* encrypt(const char* plainText, const char* password, const int plainTextSize, const int passSize)
	{
		unsigned char S[256];
		unsigned char T[256];
		for (int i = 0; i < 256; i++)
		{
			S[i] = i;
		}
		int i = 0;
		char* cipher = new char[plainTextSize + 1]; // make sure to delete the return value

		__ksa(password, passSize, S, T);
		const char* generatedKey = __prga(plainText, plainTextSize, S);
		for (int i = 0; i < plainTextSize; i++)
		{
			cipher[i] = (generatedKey[i] ^ plainText[i]);
		}
		cipher[plainTextSize] = '\0';
		return cipher;
	}

	const char* decrypt(const char* cipher, const char* password, const int cipherSize, const int passSize)
	{
		unsigned char S[256];
		unsigned char T[256];
		for (int i = 0; i < 256; i++)
		{
			S[i] = i;
		}
		char* plainText = new char[cipherSize + 1];  // make sure to delete the return value
		__ksa(password, passSize, S, T);
		const char* generatedKey = __prga(plainText, cipherSize, S);
		for (int i = 0; i < cipherSize; i++)
		{
			plainText[i] = (generatedKey[i] ^ cipher[i]);
		}
		plainText[cipherSize] = '\0';
		return plainText;
	}

	void __swap(unsigned char* S, unsigned char* T, const int i, const int j)
	{
		unsigned char temp = S[i];
		S[i] = T[j];
		T[j] = temp;
	}

	void __ksa(const char* password, const int passSize, unsigned char* S, unsigned char* T)
	{
		for (int i = 0; i < 256; i++)
		{
			T[i] = password[i % passSize];
		}

		int j = 0;
		for (int i = 0; i < 256; i++)
		{
			j = (j + S[i] + T[i]) % 256;
			__swap(S, T, i, j);
		}
	}

	const char* __prga(const char* plainText, const int plainTextSize, unsigned char* S)
	{
		char* generatedKey = new char[plainTextSize];
		int i = 0, j = 0;
		unsigned char t, k;
		for (int ii = 0; ii < plainTextSize; ii++)
		{
			i = (i + 1) % 256;
			j = (j + S[i]) % 256;
			__swap(S, S, i, j);
			t = (S[i] + S[j]) % 256;
			k = S[t];
			generatedKey[ii] = k;
		}
		return generatedKey;
	}
}


