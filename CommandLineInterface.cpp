#include "CommandLineInterface.h"
#include <iostream>
#include <fstream>

#ifdef _WIN32
static const char* DEFAULT_LIB = "cipher.dll";
#elif __APPLE__
static const char* DEFAULT_LIB = "./libcipher.dylib";
#else
static const char* DEFAULT_LIB = "./libcipher.so";
#endif

static std::string promptLine(const std::string& label) {
    std::cout << label;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

static bool readFileBytes(const std::string& path, std::vector<unsigned char>& out) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;
    out.clear();
    char byte;
    while (f.get(byte)) {
        out.push_back((unsigned char)byte);
    }
    return true;
}

static bool writeFileBytes(const std::string& path, const std::vector<unsigned char>& data) {
    std::ofstream f(path, std::ios::binary);
    if (!f) return false;
    const char* bytes = (const char*)data.data();
    std::streamsize size = (std::streamsize)data.size();
    f.write(bytes, size);
    return f.good();
}

void CommandLineInterface::showMenu() const {
    std::cout << "\n=== Text Editor ===\n"
              << "1) Add text line\n"
              << "2) Add checklist item\n"
              << "3) Add contact\n"
              << "4) Print document\n"
              << "5) Encrypt and save to file\n"
              << "6) Load and decrypt from file\n"
              << "0) Exit\n> ";
}

void CommandLineInterface::addTextLine() {
    std::string t = promptLine("Text: ");
    text_.addLine(new TextLine(t));
}

void CommandLineInterface::addChecklistLine() {
    std::string item = promptLine("Item: ");
    std::string done = promptLine("Checked? (y/n): ");
    bool checked = !done.empty() && (done[0] == 'y' || done[0] == 'Y');
    text_.addLine(new ChecklistLine(item, checked));
}

void CommandLineInterface::addContactLine() {
    std::string name = promptLine("Name: ");
    std::string surname = promptLine("Surname: ");
    std::string email = promptLine("E-mail: ");
    text_.addLine(new ContactLine(name, surname, email));
}

void CommandLineInterface::printText() const {
    if (text_.size() == 0) {
        std::cout << "(document is empty)\n";
        return;
    }
    text_.printAll();
}

void CommandLineInterface::encryptAndSave() {
    if (text_.size() == 0) {
        std::cout << "Nothing to encrypt(document is empty).\n";
        return;
    }

    std::string libPath = promptLine(std::string("DLL path [") + DEFAULT_LIB + "]: ");
    if (libPath.empty()) libPath = DEFAULT_LIB;

    std::string algoChoice = promptLine("Cipher: 1) Caesar  2) Vigenere\n> ");
    int algorithm;
    std::string key;
    if (algoChoice == "2") {
        algorithm = VIGENERE;
        key = promptLine("Key (word): ");
    } else {
        algorithm = CAESAR;
        key = promptLine("Key (integer): ");
    }

    Cipher lib;
    if (!lib.load(libPath)) {
        std::cout << "Could not load cipher library: " << libPath << "\n";
        return;
    }
    if (!lib.createCipher(algorithm, key)) {
        std::cout << "Could not create cipher\n";
        return;
    }

    std::vector<unsigned char> plain = text_.serialize();
    std::vector<unsigned char> cipherBytes = lib.encrypt(plain);
    lib.destroyCipher();
    lib.unload();

    std::string outPath = promptLine("Output file path: ");
    if (!writeFileBytes(outPath, cipherBytes)) {
        std::cout << "Failed to write output file.\n";
        return;
    }
    std::cout << "Encrypted document saved to " << outPath << "\n";
}

void CommandLineInterface::loadAndDecrypt() {
    std::string inPath = promptLine("Input file path: ");
    std::vector<unsigned char> cipherBytes;
    if (!readFileBytes(inPath, cipherBytes)) {
        std::cout << "Failed to read input file.\n";
        return;
    }

    std::string libPath = promptLine(std::string("DLL path [") + DEFAULT_LIB + "]: ");
    if (libPath.empty()) libPath = DEFAULT_LIB;

    std::string algoChoice = promptLine("Cipher: 1) Caesar  2) Vigenere\n> ");
    int algorithm;
    std::string key;
    if (algoChoice == "2") {
        algorithm = VIGENERE;
        key = promptLine("Key (word): ");
    } else {
        algorithm = CAESAR;
        key = promptLine("Key (integer): ");
    }

    Cipher lib;
    if (!lib.load(libPath)) {
        std::cout << "Could not load cipher library: " << libPath << "\n";
        return;
    }
    if (!lib.createCipher(algorithm, key)) {
        std::cout << "Could not create cipher\n";
        return;
    }

    std::vector<unsigned char> plain = lib.decrypt(cipherBytes);
    lib.destroyCipher();
    lib.unload();

    if (!text_.deserialize(plain)) {
        std::cout << "Could not parse decrypted data.\n";
        return;
    }
    std::cout << "Document loaded and decrypted (" << text_.size() << " lines).\n";
    printText();
}

void CommandLineInterface::run() {
    while (true) {
        showMenu();
        std::string choice = promptLine("");
        if (choice == "0") break;
        else if (choice == "1") addTextLine();
        else if (choice == "2") addChecklistLine();
        else if (choice == "3") addContactLine();
        else if (choice == "4") printText();
        else if (choice == "5") encryptAndSave();
        else if (choice == "6") loadAndDecrypt();
        else std::cout << "Unknown choice.\n";
    }
    std::cout << "Bye.\n";
}
