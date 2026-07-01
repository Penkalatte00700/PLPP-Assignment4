#ifndef CIPHER_API_H
#define CIPHER_API_H

#include <cstddef>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void* cipher_t;

EXPORT cipher_t* cipher_create_caesar(int key);
EXPORT cipher_t* cipher_create_vigenere(const char* key);

EXPORT unsigned char* cipher_encrypt(cipher_t* cipher, const unsigned char* data, size_t in_len, size_t* out_len);
EXPORT unsigned char* cipher_decrypt(cipher_t* cipher, const unsigned char* data, size_t in_len, size_t* out_len);

EXPORT void cipher_destroy(cipher_t* cipher);
EXPORT void cipher_free_bytes(unsigned char* data);

#ifdef __cplusplus
}
#endif
#endif
