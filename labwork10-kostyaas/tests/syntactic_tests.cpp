#include <gtest/gtest.h>
#include <sstream>
#include "syntactic_analyser/SyntacticAnalyser.h"

TEST(Parser, SimpleExpr) {
    std::istringstream in("1+2*3");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(Parser, Parens) {
    std::istringstream in("(1+2)*3");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(Parser, Assignment) {
    std::istringstream in("x=5");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(Parser, IfThen) {
    std::istringstream in("if x then y end if");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(Parser, IfElse) {
    std::istringstream in("if x then y else z end if");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(Parser, While) {
    std::istringstream in("while x end while");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(Parser, For) {
    std::istringstream in("for i in [1,2] end for");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(Parser, Return) {
    std::istringstream in("return x");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(Parser, FunctionLiteral) {
    std::istringstream in("function(a,b) a+b end function");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(Parser, Call) {
    std::istringstream in("f(1,2)");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(Parser, List) {
    std::istringstream in("[1,2,3]");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(Parser, Unary) {
    std::istringstream in("-x");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(Parser, AndOr) {
    std::istringstream in("a and b or c");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(ParserError, MissingEndIf) {
    std::istringstream in("if x then y");
    SyntacticAnalyser p(in);
    EXPECT_THROW(p.Parse(), std::runtime_error);
}
TEST(ParserError, BadFor) {
    std::istringstream in("for in x end for");
    SyntacticAnalyser p(in);
    EXPECT_THROW(p.Parse(), std::runtime_error);
}
TEST(ParserError, BadCall) {
    std::istringstream in("f(,)");
    SyntacticAnalyser p(in);
    EXPECT_THROW(p.Parse(), std::runtime_error);
}
TEST(ParserError, BadSlice) {
    std::istringstream in("a[:]");
    SyntacticAnalyser p(in);
    EXPECT_THROW(p.Parse(), std::runtime_error);
}
TEST(ParserError, BadPrimary) {
    std::istringstream in(")");
    SyntacticAnalyser p(in);
    EXPECT_THROW(p.Parse(), std::runtime_error);
}
TEST(Parser, Complex) {
    std::istringstream in("x = function(a) if a>0 then a*2 else a/2 end if end function");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(Parser, NestedBlocks) {
    std::istringstream in("if x then while y for i in z end for end while else return end if");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(ParserError, UnterminatedFunction) {
    std::istringstream in("function() x");
    SyntacticAnalyser p(in);
    EXPECT_THROW(p.Parse(), std::runtime_error);
}
TEST(Parser, ManyOperators) {
    std::istringstream in("a+b-c*d/e%f^g");
    SyntacticAnalyser p(in);
    EXPECT_NO_THROW(p.Parse());
}
TEST(ParserError, BadAssignment) {
    std::istringstream in("1=2");
    SyntacticAnalyser p(in);
    EXPECT_THROW(p.Parse(), std::runtime_error);
}