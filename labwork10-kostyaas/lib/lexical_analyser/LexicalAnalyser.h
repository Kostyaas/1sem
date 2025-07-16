#pragma once

#include <istream>
#include <array>
#include "Token.h"

class LexicalAnalyser {
public:
    explicit LexicalAnalyser(std::istream& in);
    const Token& Top() const;
    Token Next();

private:
    std::istream& in_;
    std::size_t line_{1}, col_{0};
    int cur_{in_.get()};
    Token lookahead_;

    static constexpr std::array<std::pair<const char*, TokenType>, 10> MultiCharOps{{
        {"==", TokenType::DoubleEqual},
        {"!=", TokenType::NotEqual},
        {"<=", TokenType::LessEqual},
        {">=", TokenType::GreaterEqual},
        {"+=", TokenType::PlusEqual},
        {"-=", TokenType::MinusEqual},
        {"*=", TokenType::AsteriskEqual},
        {"/=", TokenType::SlashEqual},
        {"%=", TokenType::PercentEqual},
        {"^=", TokenType::CaretEqual}
    }};

    static constexpr std::array<std::pair<char, TokenType>, 14> SingleCharOps{{
        {'+', TokenType::Plus},
        {'-', TokenType::Minus},
        {'*', TokenType::Asterisk},
        {'/', TokenType::Slash},
        {'%', TokenType::Percent},
        {'^', TokenType::Caret},
        {'=', TokenType::Assign},
        {'<', TokenType::Less},
        {'>', TokenType::Greater},
        {'(', TokenType::LParen},
        {')', TokenType::RParen},
        {'[', TokenType::LBracket},
        {']', TokenType::RBracket},
        {',', TokenType::Comma}
    }};

    static constexpr std::array<std::pair<const char*, TokenType>, 17> Keywords{{
        {"nil", TokenType::Nil},
        {"true", TokenType::Boolean},
        {"false", TokenType::Boolean},
        {"if", TokenType::If},
        {"then", TokenType::Then},
        {"else", TokenType::Else},
        {"end", TokenType::End},
        {"while", TokenType::While},
        {"for", TokenType::For},
        {"in", TokenType::In},
        {"function", TokenType::Function},
        {"return", TokenType::Return},
        {"and", TokenType::And},
        {"or", TokenType::Or},
        {"not", TokenType::Not},
        {"break", TokenType::Break},
        {"continue", TokenType::Continue}
    }};

    void Update();
    void Skip();
    Token Scan();
    Token ScanIdx();
    Token ScanNumber();
    Token ScanString();
};
