#pragma once
#include <vector>
#include <string>

class Line {
public:
    virtual void print() const = 0;
    virtual std::vector<unsigned char> serialize() const = 0;
    virtual ~Line() {}
    static Line* deserialize(const std::vector<unsigned char>& data, size_t& offset);
};

class TextLine : public Line {
    std::string text_;
public:
    TextLine(const std::string& text);
    void print() const override;
    std::vector<unsigned char> serialize() const override;
};

class ChecklistLine : public Line {
    std::string item_;
    bool checked_;
public:
    ChecklistLine(const std::string& item, bool checked);
    void print() const override;
    std::vector<unsigned char> serialize() const override;
};

class ContactLine : public Line {
    std::string name_;
    std::string surname_;
    std::string email_;
public:
    ContactLine(const std::string& name, const std::string& surname, const std::string& email);
    void print() const override;
    std::vector<unsigned char> serialize() const override;
};

class Text {
    std::vector<Line*> lines_;
public:
    ~Text();

    void addLine(Line* line);
    void printAll() const;
    size_t size() const;
    void clear();

    std::vector<unsigned char> serialize() const;
    bool deserialize(const std::vector<unsigned char>& data);
};
