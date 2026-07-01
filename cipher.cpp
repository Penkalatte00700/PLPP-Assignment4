#include "cipher_api.h"
#include <string>
#include <vector>
#include <cstring>
#include <cctype>

class Cipher {
public:
    virtual std::vector<unsigned char> encrypt(const std::vector<unsigned char>& data) = 0;
    virtual std::vector<unsigned char> decrypt(const std::vector<unsigned char>& data) = 0;
    virtual ~Cipher() = default;
};

class CaesarCipher : public Cipher {
    int key_;
public:
    CaesarCipher(int key);
    std::vector<unsigned char> encrypt(const std::vector<unsigned char>& data) override;
    std::vector<unsigned char> decrypt(const std::vector<unsigned char>& data) override;
};

class VigenereCipher : public Cipher {
    std::string key_;
public:
    VigenereCipher(const std::string& key);
    std::vector<unsigned char> encrypt(const std::vector<unsigned char>& data) override;
    std::vector<unsigned char> decrypt(const std::vector<unsigned char>& data) override;
};

CaesarCipher::CaesarCipher(int key) {
    key_ = key;
}

std::vector<unsigned char> CaesarCipher::encrypt(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> r = data;
    int k = key_ % 26;
    if (k < 0) k += 26;
    for (size_t i = 0; i < r.size(); i++) {
        unsigned char c = r[i];
        if (c >= 'a' && c <= 'z')
            r[i] = (unsigned char)('a' + (c - 'a' + k) % 26);
        else if (c >= 'A' && c <= 'Z')
            r[i] = (unsigned char)('A' + (c - 'A' + k) % 26);
    }
    return r;
}

std::vector<unsigned char> CaesarCipher::decrypt(const std::vector<unsigned char>& data) {
    CaesarCipher tmp(-key_);
    return tmp.encrypt(data);
}

VigenereCipher::VigenereCipher(const std::string& key) {
    key_ = key;
}

std::vector<unsigned char> VigenereCipher::encrypt(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> r = data;
    if (key_.empty()) return r;
    size_t ki = 0;
    for (size_t i = 0; i < r.size(); i++) {
        unsigned char c = r[i];
        if (c >= 'a' && c <= 'z') {
            int kv = toupper((unsigned char)key_[ki % key_.size()]) - 'A';
            r[i] = (unsigned char)('a' + (c - 'a' + kv) % 26);
            ki++;
        } else if (c >= 'A' && c <= 'Z') {
            int kv = toupper((unsigned char)key_[ki % key_.size()]) - 'A';
            r[i] = (unsigned char)('A' + (c - 'A' + kv) % 26);
            ki++;
        }
    }
    return r;
}

std::vector<unsigned char> VigenereCipher::decrypt(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> r = data;
    if (key_.empty()) return r;
    size_t ki = 0;
    for (size_t i = 0; i < r.size(); i++) {
        unsigned char c = r[i];
        if (c >= 'a' && c <= 'z') {
            int kv = toupper((unsigned char)key_[ki % key_.size()]) - 'A';
            int s = (26 - kv) % 26;
            r[i] = (unsigned char)('a' + (c - 'a' + s) % 26);
            ki++;
        } else if (c >= 'A' && c <= 'Z') {
            int kv = toupper((unsigned char)key_[ki % key_.size()]) - 'A';
            int s = (26 - kv) % 26;
            r[i] = (unsigned char)('A' + (c - 'A' + s) % 26);
            ki++;
        }
    }
    return r;
}

extern "C" {
    EXPORT cipher_t* cipher_create_caesar(int key) {
        return (cipher_t*) new CaesarCipher(key);
    }

    EXPORT cipher_t* cipher_create_vigenere(const char* key) {
        return (cipher_t*) new VigenereCipher(key ? key : "");
    }

    EXPORT unsigned char* cipher_encrypt(cipher_t* cipher, const unsigned char* data, size_t in_len, size_t* out_len) {
        Cipher* c = (Cipher*)cipher;
        std::vector<unsigned char> input(data, data + in_len);
        std::vector<unsigned char> result = c->encrypt(input);
        unsigned char* out = new unsigned char[result.size()];
        if (!result.empty()) memcpy(out, result.data(), result.size());
        *out_len = result.size();
        return out;
    }

    EXPORT unsigned char* cipher_decrypt(cipher_t* cipher, const unsigned char* data, size_t in_len, size_t* out_len) {
        Cipher* c = (Cipher*)cipher;
        std::vector<unsigned char> input(data, data + in_len);
        std::vector<unsigned char> result = c->decrypt(input);
        unsigned char* out = new unsigned char[result.size()];
        if (!result.empty()) memcpy(out, result.data(), result.size());
        *out_len = result.size();
        return out;
    }
    EXPORT void cipher_destroy(cipher_t* cipher) {
        delete (Cipher*)cipher;
    }
    EXPORT void cipher_free_bytes(unsigned char* data) {
        delete[] data;
    }
}
