#include "LexicalAnalyser.h"
#include <cctype>
#include <stdexcept>

LexicalAnalyser::LexicalAnalyser(std::istream& in)
    : in_(in) {
    lookahead_ = Scan();
}

const Token& LexicalAnalyser::Top() const {
    return lookahead_;
}

Token LexicalAnalyser::Next() {
    Token cur = lookahead_;
    lookahead_ = Scan();
    return cur;
}

void LexicalAnalyser::Update() {
    if (cur_ == '\n') {
        ++line_;
        col_ = 0;
    } else {
        ++col_;
    }
    cur_ = in_.get();
}

void LexicalAnalyser::Skip() {
    while (std::isspace(cur_) || (cur_ == '/' && in_.peek() == '/')) {
        if (cur_ == '/') {
            while (cur_ != EOF && cur_ != '\n') {
                Update();
            }
        } else {
            Update();
        }
    }
}

Token LexicalAnalyser::Scan() {
    Skip();
    if (cur_ == EOF) {
        return Token{TokenType::EndOfFile, "", line_, col_};
    }
    char f = static_cast<char>(cur_);
    char n = static_cast<char>(in_.peek());
    std::string two{f, n};
    for (auto& [op, tp] : MultiCharOps) {
        if (two == op) {
            Update();
            Update();
            return Token{tp, op, line_, col_ - 2};
        }
    }
    for (auto& [ch, tp] : SingleCharOps) {
        if (cur_ == ch) {
            char c = static_cast<char>(cur_);
            Update();
            return Token{tp, std::string(1, c), line_, col_ - 1};
        }
    }
    if (std::isalpha(cur_) || cur_ == '_') {
        return ScanIdx();
    }
    if (std::isdigit(cur_) || cur_ == '.') {
        return ScanNumber();
    }
    if (cur_ == '"') {
        return ScanString();
    }
    throw std::runtime_error("Unexpected character at " + std::to_string(line_) + ":" + std::to_string(col_));
}

Token LexicalAnalyser::ScanIdx() {
    size_t L = line_;
    size_t C = col_;
    std::string buf;
    while (std::isalnum(cur_) || cur_ == '_') {
        buf.push_back(static_cast<char>(cur_));
        Update();
    }
    for (auto& [kw, tp] : Keywords) {
        if (buf == kw) {
            return Token{tp, buf, L, C};
        }
    }
    return Token{TokenType::Identifier, buf, L, C};
}

Token LexicalAnalyser::ScanNumber() {
    size_t L = line_;
    size_t C = col_;
    std::string buf;
    bool dot = false;
    while (std::isdigit(cur_) || (!dot && cur_ == '.')) {
        if (cur_ == '.') dot = true;
        buf.push_back(static_cast<char>(cur_));
        Update();
    }
    if (cur_ == 'e' || cur_ == 'E') {
        buf.push_back(static_cast<char>(cur_));
        Update();
        if (cur_ == '+' || cur_ == '-') {
            buf.push_back(static_cast<char>(cur_));
            Update();
        }
        while (std::isdigit(cur_)) {
            buf.push_back(static_cast<char>(cur_));
            Update();
        }
    }
    return Token{TokenType::Number, buf, L, C};
}

Token LexicalAnalyser::ScanString() {
    size_t L = line_;
    size_t C = col_;
    Update();
    std::string buf;
    while (cur_ != '"' && cur_ != EOF) {
        if (cur_ == '\\') {
            Update();
            char e = static_cast<char>(cur_);
            buf += (e == 'n' ? '\n' : e == 't' ? '\t' : e);
        } else {
            buf.push_back(static_cast<char>(cur_));
        }
        Update();
    }
    if (cur_ != '"') throw std::runtime_error("Unterminated string");
    Update();
    return Token{TokenType::String, buf, L, C};
}
