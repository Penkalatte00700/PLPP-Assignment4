#include "Text.h"
#include <iostream>
#include <cstring>

const unsigned char TYPE_TEXT = 0;
const unsigned char TYPE_CHECKLIST = 1;
const unsigned char TYPE_CONTACT = 2;

static void write_int(std::vector<unsigned char>& buf, int value) {
    unsigned char bytes[sizeof(int)];
    memcpy(bytes, &value, sizeof(int));
    for (size_t i = 0; i < sizeof(int); i++) {
        buf.push_back(bytes[i]);
    }
}

static void write_string(std::vector<unsigned char>& buf, const std::string& s) {
    write_int(buf, (int)s.size());
    for (size_t i = 0; i < s.size(); i++) {
        buf.push_back((unsigned char)s[i]);
    }
}

static bool read_int(const std::vector<unsigned char>& buf, size_t& offset, int& value) {
    if (offset + sizeof(int) > buf.size()) return false;
    memcpy(&value, buf.data() + offset, sizeof(int));
    offset += sizeof(int);
    return true;
}

static bool read_string(const std::vector<unsigned char>& buf, size_t& offset, std::string& value) {
    int len = 0;
    if (!read_int(buf, offset, len)) return false;
    if (len < 0 || offset + (size_t)len > buf.size()) return false;
    value.clear();
    for (int i = 0; i < len; i++) {
        value.push_back((char)buf[offset + i]);
    }
    offset += len;
    return true;
}

TextLine::TextLine(const std::string& text) : text_(text) {}

void TextLine::print() const {
    std::cout << "Text: " << text_ << "\n";
}

std::vector<unsigned char> TextLine::serialize() const {
    std::vector<unsigned char> buf;
    buf.push_back(TYPE_TEXT);
    write_string(buf, text_);
    return buf;
}

ChecklistLine::ChecklistLine(const std::string& item, bool checked)
    : item_(item), checked_(checked) {}

void ChecklistLine::print() const {
    std::string mark = checked_ ? "x" : " ";
    std::cout << "[ " << mark << " ] " << item_ << "\n";
}

std::vector<unsigned char> ChecklistLine::serialize() const {
    std::vector<unsigned char> buf;
    buf.push_back(TYPE_CHECKLIST);
    buf.push_back(checked_ ? 1 : 0);
    write_string(buf, item_);
    return buf;
}

ContactLine::ContactLine(const std::string& name, const std::string& surname, const std::string& email)
    : name_(name), surname_(surname), email_(email) {}

void ContactLine::print() const {
    std::cout << "Contact - " << name_ << " " << surname_;
    std::cout << ", E-mail: " << email_ << "\n";
}

std::vector<unsigned char> ContactLine::serialize() const {
    std::vector<unsigned char> buf;
    buf.push_back(TYPE_CONTACT);
    write_string(buf, name_);
    write_string(buf, surname_);
    write_string(buf, email_);
    return buf;
}

Line* Line::deserialize(const std::vector<unsigned char>& data, size_t& offset) {
    if (offset >= data.size()) return nullptr;
    unsigned char type = data[offset];
    offset += 1;
    if (type == TYPE_TEXT) {
        std::string text;
        if (!read_string(data, offset, text)) return nullptr;
        return new TextLine(text);
    }

    if (type == TYPE_CHECKLIST) {
        if (offset >= data.size()) return nullptr;
        bool checked = data[offset] != 0;
        offset += 1;
        std::string item;
        if (!read_string(data, offset, item)) return nullptr;
        return new ChecklistLine(item, checked);
    }

    if (type == TYPE_CONTACT) {
        std::string name, surname, email;
        if (!read_string(data, offset, name)) return nullptr;
        if (!read_string(data, offset, surname)) return nullptr;
        if (!read_string(data, offset, email)) return nullptr;
        return new ContactLine(name, surname, email);
    }
    return nullptr;
}

Text::~Text() {
    clear();
}

void Text::addLine(Line* line) {
    lines_.push_back(line);
}

void Text::printAll() const {
    for (size_t i = 0; i < lines_.size(); i++) {
        lines_[i]->print();
    }
}

size_t Text::size() const {
    return lines_.size();
}

void Text::clear() {
    for (size_t i = 0; i < lines_.size(); i++) {
        delete lines_[i];
    }
    lines_.clear();
}

std::vector<unsigned char> Text::serialize() const {
    std::vector<unsigned char> buf;
    write_int(buf, (int)lines_.size());
    for (size_t i = 0; i < lines_.size(); i++) {
        std::vector<unsigned char> lineBytes = lines_[i]->serialize();
        for (size_t j = 0; j < lineBytes.size(); j++) {
            buf.push_back(lineBytes[j]);
        }
    }
    return buf;
}

bool Text::deserialize(const std::vector<unsigned char>& data) {
    size_t offset = 0;
    int count = 0;
    if (!read_int(data, offset, count) || count < 0) return false;

    std::vector<Line*> newLines;
    for (int i = 0; i < count; i++) {
        Line* line = Line::deserialize(data, offset);
        if (!line) {
            for (size_t j = 0; j < newLines.size(); j++) delete newLines[j];
            return false;
        }
        newLines.push_back(line);
    }
    clear();
    lines_ = newLines;
    return true;
}
