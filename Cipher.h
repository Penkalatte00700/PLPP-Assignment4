#pragma once
#include <vector>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

const int CAESAR = 1;
const int VIGENERE = 2;

class Cipher {
public:
    Cipher();
    ~Cipher();

    bool load(const std::string& libPath);
    void unload();
    bool createCipher(int algorithm, const std::string& key);
    void destroyCipher();
    std::vector<unsigned char> encrypt(const std::vector<unsigned char>& data);
    std::vector<unsigned char> decrypt(const std::vector<unsigned char>& data);

private:
#ifdef _WIN32
    HMODULE handle_ = nullptr;
#else
    void* handle_ = nullptr;
#endif
    void* cipher_ = nullptr;

    typedef void* (*create_caesar_fn)(int);
    typedef void* (*create_vigenere_fn)(const char*);
    typedef unsigned char* (*crypt_fn)(void*, const unsigned char*, size_t, size_t*);
    typedef void (*destroy_fn)(void*);
    typedef void (*free_bytes_fn)(unsigned char*);

    create_caesar_fn create_caesar_ = nullptr;
    create_vigenere_fn create_vigenere_ = nullptr;
    crypt_fn encrypt_fn_ = nullptr;
    crypt_fn decrypt_fn_ = nullptr;
    destroy_fn destroy_fn_ = nullptr;
    free_bytes_fn free_bytes_fn_ = nullptr;
};
