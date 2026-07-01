#pragma once
#include "Text.h"
#include "Cipher.h"

class CommandLineInterface {
    Text text_;
public:
    void run();

private:
    void showMenu() const;
    void addTextLine();
    void addChecklistLine();
    void addContactLine();
    void printText() const;
    void encryptAndSave();
    void loadAndDecrypt();
};
