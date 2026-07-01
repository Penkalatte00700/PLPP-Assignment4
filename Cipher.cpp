#include "Cipher.h"
#include <cstdlib>

Cipher::Cipher() {}

Cipher::~Cipher() {
    destroyCipher();
    unload();
}

bool Cipher::load(const std::string& libPath) {
    if (handle_) unload();

#ifdef _WIN32
    handle_ = LoadLibraryA(libPath.c_str());
    if (!handle_) return false;
    create_caesar_ = (create_caesar_fn)GetProcAddress(handle_, "cipher_create_caesar");
    create_vigenere_ = (create_vigenere_fn)GetProcAddress(handle_, "cipher_create_vigenere");
    encrypt_fn_ = (crypt_fn)GetProcAddress(handle_, "cipher_encrypt");
    decrypt_fn_ = (crypt_fn)GetProcAddress(handle_, "cipher_decrypt");
    destroy_fn_ = (destroy_fn)GetProcAddress(handle_, "cipher_destroy");
    free_bytes_fn_ = (free_bytes_fn)GetProcAddress(handle_, "cipher_free_bytes");
#else
    handle_ = dlopen(libPath.c_str(), RTLD_LAZY);
    if (!handle_) return false;
    create_caesar_ = (create_caesar_fn)dlsym(handle_, "cipher_create_caesar");
    create_vigenere_ = (create_vigenere_fn)dlsym(handle_, "cipher_create_vigenere");
    encrypt_fn_ = (crypt_fn)dlsym(handle_, "cipher_encrypt");
    decrypt_fn_ = (crypt_fn)dlsym(handle_, "cipher_decrypt");
    destroy_fn_ = (destroy_fn)dlsym(handle_, "cipher_destroy");
    free_bytes_fn_ = (free_bytes_fn)dlsym(handle_, "cipher_free_bytes");
#endif

    if (!create_caesar_ || !create_vigenere_ || !encrypt_fn_ || !decrypt_fn_ || !destroy_fn_ || !free_bytes_fn_) {
        unload();
        return false;
    }
    return true;
}

void Cipher::unload() {
    if (handle_) {
#ifdef _WIN32
        FreeLibrary(handle_);
#else
        dlclose(handle_);
#endif
    }
    handle_ = nullptr;
    create_caesar_ = nullptr;
    create_vigenere_ = nullptr;
    encrypt_fn_ = nullptr;
    decrypt_fn_ = nullptr;
    destroy_fn_ = nullptr;
    free_bytes_fn_ = nullptr;
}

bool Cipher::createCipher(int algorithm, const std::string& key) {
    if (!handle_) return false;
    destroyCipher();

    if (algorithm == CAESAR) {
        int k = 0;
        try { k = std::stoi(key); }
        catch (...) { return false; }
        cipher_ = create_caesar_(k);
    } else {
        cipher_ = create_vigenere_(key.c_str());
    }
    return cipher_ != nullptr;
}

void Cipher::destroyCipher() {
    if (cipher_ && destroy_fn_) {
        destroy_fn_(cipher_);
    }
    cipher_ = nullptr;
}

std::vector<unsigned char> Cipher::encrypt(const std::vector<unsigned char>& data) {
    if (!cipher_) return std::vector<unsigned char>();
    size_t outLen = 0;
    unsigned char* out = encrypt_fn_(cipher_, data.data(), data.size(), &outLen);
    std::vector<unsigned char> result(out, out + outLen);
    free_bytes_fn_(out);
    return result;
}

std::vector<unsigned char> Cipher::decrypt(const std::vector<unsigned char>& data) {
    if (!cipher_) return std::vector<unsigned char>();
    size_t outLen = 0;
    unsigned char* out = decrypt_fn_(cipher_, data.data(), data.size(), &outLen);
    std::vector<unsigned char> result(out, out + outLen);
    free_bytes_fn_(out);
    return result;
}
