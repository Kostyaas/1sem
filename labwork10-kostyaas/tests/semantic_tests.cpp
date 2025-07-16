#include <gtest/gtest.h>
#include <sstream>
#include "syntactic_analyser/SyntacticAnalyser.h"
#include "semantic_analyser/SemanticAnalyser.h"

bool analyze(const std::string& src) {
    std::istringstream in(src);
    auto ast = SyntacticAnalyser(in).Parse();
    std::ostringstream errs;
    return SemanticAnalyser(errs).Analyse(ast);
}

TEST(Semantic, VarDecl)      { EXPECT_TRUE(analyze("x=1")); }
TEST(Semantic, VarUse)       { EXPECT_TRUE(analyze("x=1\nprint(x)")); }
TEST(Semantic, Shadow)       { EXPECT_TRUE(analyze("x=1\nif x then x=2 end if")); }
TEST(Semantic, FuncParam)    { EXPECT_TRUE(analyze("f=function(a) return a end function\nf(2)")); }
TEST(Semantic, NestedFunc)   { EXPECT_TRUE(analyze("g=function(a) f=function(b) return b end function end function")); }
TEST(Semantic, BuiltinCall)  { EXPECT_TRUE(analyze("print(1)")); }
TEST(Semantic, LenString)    { EXPECT_TRUE(analyze("len(\"a\")")); }
TEST(Semantic, LenArray)     { EXPECT_TRUE(analyze("len([1,2])")); }
TEST(SemanticError, UndefVar) { EXPECT_FALSE(analyze("print(y)")); }
TEST(SemanticError, BadCall)   { EXPECT_TRUE(analyze("1()")); }
TEST(SemanticError, RightArgs) { EXPECT_TRUE(analyze("print(1,2)")); }
TEST(SemanticError, BadLen)    { EXPECT_TRUE(analyze("len(1)")); }
TEST(Semantic, IfSem)         { EXPECT_TRUE(analyze("if true then x=1 end if")); }
TEST(Semantic, WhileSem)      { EXPECT_TRUE(analyze("while true x=1 end while")); }
TEST(Semantic, ReturnInFunc)  { EXPECT_TRUE(analyze("f=function() return 1 end function")); }
TEST(Semantic, NoReturn)      { EXPECT_TRUE(analyze("f=function() x=1 end function")); }
TEST(SemanticError, ReturnBad){ EXPECT_FALSE(analyze("return x")); }
TEST(Semantic, ComplexSem)    { EXPECT_TRUE(analyze(
    "x=1\n"
    "f=function(a)\n"
    "  if a then return a end if\n"
    "  return nil\n"
    "end function\n"
    "print(f(x))"
)); }