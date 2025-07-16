#pragma once

#include <string>
#include <cstddef>

enum class TokenType {
    EndOfFile,
    Identifier,
    Number,
    String,
    Boolean,
    Nil,
    If,
    Then,
    Else,
    End,
    While,
    For,
    In,
    Function,
    Return,
    And,
    Or,
    Not,
    Break,
    Continue,
    Plus,
    Minus,
    Asterisk,
    Slash,
    Percent,
    Caret,
    Assign,
    PlusEqual,
    MinusEqual,
    AsteriskEqual,
    SlashEqual,
    PercentEqual,
    CaretEqual,
    DoubleEqual,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    LParen,
    RParen,
    LBracket,
    RBracket,
    Comma,
    Colon
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::size_t line;
    std::size_t column;
    Token(TokenType t = TokenType::EndOfFile, std::string l = "", std::size_t ln = 0, std::size_t col = 0) : type(t), lexeme(std::move(l)), line(ln), column(col) {}
};
