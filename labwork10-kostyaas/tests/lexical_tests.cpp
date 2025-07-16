#include <gtest/gtest.h>
#include <sstream>
#include "lexical_analyser/LexicalAnalyser.h"
#include "Token.h"

TEST(Lexer, Identifier) {
    std::istringstream in("foo");
    LexicalAnalyser lex(in);
    Token t = lex.Next();
    EXPECT_EQ(t.type, TokenType::Identifier);
    EXPECT_EQ(t.lexeme, "foo");
}
TEST(Lexer, NumberInteger) {
    std::istringstream in("123");
    LexicalAnalyser lex(in);
    Token t = lex.Next();
    EXPECT_EQ(t.type, TokenType::Number);
    EXPECT_EQ(t.lexeme, "123");
}
TEST(Lexer, NumberFloat) {
    std::istringstream in("3.14");
    LexicalAnalyser lex(in);
    Token t = lex.Next();
    EXPECT_EQ(t.type, TokenType::Number);
    EXPECT_EQ(t.lexeme, "3.14");
}
TEST(Lexer, NumberScientific) {
    std::istringstream in("1e-5");
    LexicalAnalyser lex(in);
    Token t = lex.Next();
    EXPECT_EQ(t.type, TokenType::Number);
    EXPECT_EQ(t.lexeme, "1e-5");
}
TEST(Lexer, StringSimple) {
    std::istringstream in("\"hi\"");
    LexicalAnalyser lex(in);
    Token t = lex.Next();
    EXPECT_EQ(t.type, TokenType::String);
    EXPECT_EQ(t.lexeme, "hi");
}
TEST(Lexer, StringEscape) {
    std::istringstream in("\"a\\nb\"");
    LexicalAnalyser lex(in);
    Token t = lex.Next();
    EXPECT_EQ(t.type, TokenType::String);
    EXPECT_EQ(t.lexeme, "a\nb");
}
TEST(Lexer, BooleanTrue) {
    std::istringstream in("true");
    LexicalAnalyser lex(in);
    Token t = lex.Next();
    EXPECT_EQ(t.type, TokenType::Boolean);
    EXPECT_EQ(t.lexeme, "true");
}
TEST(Lexer, BooleanFalse) {
    std::istringstream in("false");
    LexicalAnalyser lex(in);
    Token t = lex.Next();
    EXPECT_EQ(t.type, TokenType::Boolean);
    EXPECT_EQ(t.lexeme, "false");
}
TEST(Lexer, Nil) {
    std::istringstream in("nil");
    LexicalAnalyser lex(in);
    Token t = lex.Next();
    EXPECT_EQ(t.type, TokenType::Nil);
    EXPECT_EQ(t.lexeme, "nil");
}
TEST(Lexer, KeywordIf) {
    std::istringstream in("if");
    LexicalAnalyser lex(in);
    Token t = lex.Next();
    EXPECT_EQ(t.type, TokenType::If);
}
TEST(Lexer, KeywordThen) {
    std::istringstream in("then");
    LexicalAnalyser lex(in);
    Token t = lex.Next();
    EXPECT_EQ(t.type, TokenType::Then);
}
TEST(Lexer, Plus) {
    std::istringstream in("+");
    LexicalAnalyser lex(in);
    EXPECT_EQ(lex.Next().type, TokenType::Plus);
}
TEST(Lexer, Minus) {
    std::istringstream in("-");
    LexicalAnalyser lex(in);
    EXPECT_EQ(lex.Next().type, TokenType::Minus);
}
TEST(Lexer, AsteriskEqual) {
    std::istringstream in("*=");
    LexicalAnalyser lex(in);
    EXPECT_EQ(lex.Next().type, TokenType::AsteriskEqual);
}
TEST(Lexer, DoubleEqual) {
    std::istringstream in("==");
    LexicalAnalyser lex(in);
    EXPECT_EQ(lex.Next().type, TokenType::DoubleEqual);
}
TEST(Lexer, NotEqual) {
    std::istringstream in("!=");
    LexicalAnalyser lex(in);
    EXPECT_EQ(lex.Next().type, TokenType::NotEqual);
}
TEST(Lexer, LessEqual) {
    std::istringstream in("<=");
    LexicalAnalyser lex(in);
    EXPECT_EQ(lex.Next().type, TokenType::LessEqual);
}
TEST(Lexer, GreaterEqual) {
    std::istringstream in(">=");
    LexicalAnalyser lex(in);
    EXPECT_EQ(lex.Next().type, TokenType::GreaterEqual);
}
TEST(Lexer, LParen) {
    std::istringstream in("(");
    LexicalAnalyser lex(in);
    EXPECT_EQ(lex.Next().type, TokenType::LParen);
}
TEST(Lexer, RParen) {
    std::istringstream in(")");
    LexicalAnalyser lex(in);
    EXPECT_EQ(lex.Next().type, TokenType::RParen);
}
TEST(Lexer, Comma) {
    std::istringstream in(",");
    LexicalAnalyser lex(in);
    EXPECT_EQ(lex.Next().type, TokenType::Comma);
}
TEST(Lexer, SkipWhitespace) {
    std::istringstream in("   \n\tfoo");
    LexicalAnalyser lex(in);
    EXPECT_EQ(lex.Next().lexeme, "foo");
}
TEST(Lexer, SkipComments) {
    std::istringstream in("//hello\nbar");
    LexicalAnalyser lex(in);
    EXPECT_EQ(lex.Next().lexeme, "bar");
}