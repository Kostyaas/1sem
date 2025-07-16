#include <gtest/gtest.h>
#include <sstream>
#include "Interpreter.h"

bool run(const std::string& src, std::string& out) {
    std::istringstream in(src);
    std::ostringstream os;
    bool ok = Interpreter(in, os);
    out = os.str();
    return ok;
}

TEST(Interpreter, PrintNumber) {
    std::string o;
    EXPECT_TRUE(run("print(5)", o));
    EXPECT_EQ(o, "5");
}
TEST(Interpreter, PrintString) {
    std::string o;
    EXPECT_TRUE(run("print(\"hi\")", o));
    EXPECT_EQ(o, "hi");
}
TEST(Interpreter, Arithmetic) {
    std::string o;
    EXPECT_TRUE(run("print(2+3*4)", o));
    EXPECT_EQ(o, "14");
}
TEST(Interpreter, IfTrue) {
    std::string o;
    EXPECT_TRUE(run("if true then print(1) end if", o));
    EXPECT_EQ(o, "1");
}
TEST(Interpreter, IfFalse) {
    std::string o;
    EXPECT_TRUE(run("if false then print(1) else print(2) end if", o));
    EXPECT_EQ(o, "2");
}
TEST(Interpreter, While) {
    std::string o;
    EXPECT_TRUE(run("i=0\nwhile i<3\nprint(i)\ni=i+1\nend while", o));
    EXPECT_EQ(o, "012");
}
TEST(Interpreter, For) {
    std::string o;
    EXPECT_TRUE(run("for i in range(0,3,1)\nprint(i)\nend for", o));
    EXPECT_EQ(o, "012");
}
TEST(Interpreter, ListIndex) {
    std::string o;
    EXPECT_TRUE(run("a=[10,20,30]\nprint(a[1])", o));
    EXPECT_EQ(o, "20");
}
TEST(Interpreter, Slice) {
    std::string o;
    EXPECT_FALSE(run("s=\"abcde\"\nprint(s[1:4])", o));
}
TEST(Interpreter, Function) {
    std::string o;
    EXPECT_TRUE(run(
        "incr=function(x) return x+1 end function\n"
        "print(incr(5))", o));
    EXPECT_EQ(o, "6");
}
TEST(Interpreter, NestedFunction) {
    std::string o;
    EXPECT_TRUE(run(
        "outer=function(x)\n"
        "  inner=function(y) return y*2 end function\n"
        "  print(inner(x))\n"
        "end function\n"
        "outer(3)", o));
    EXPECT_EQ(o, "6");
}
TEST(Interpreter, BooleanLogic) {
    std::string o;
    EXPECT_TRUE(run("print(true and false or true)", o));
    EXPECT_EQ(o, "true");
}
TEST(Interpreter, StringOps) {
    std::string o;
    EXPECT_TRUE(run("print(\"ab\"*3)", o));
    EXPECT_EQ(o, "ababab");
}
TEST(Interpreter, Mixed) {
    std::string o;
    EXPECT_TRUE(run(
        "f=function(a)\n"
        "  if a>2 then return a*a else return a end if\n"
        "end function\n"
        "print(f(3))\nprint(f(2))", o));
    EXPECT_EQ(o, "92");
}
TEST(InterpreterError, TypeMix) {
    std::string o;
    EXPECT_FALSE(run("print(1+\"a\")", o));
}
TEST(InterpreterError, ArgCount) {
    std::string o;
    EXPECT_TRUE(run("print()", o));
}
TEST(InterpreterError, UndefinedVar) {
    std::string o;
    EXPECT_FALSE(run("print(x)", o));
}
TEST(Interpreter, ComplexScript) {
    std::string o;
    EXPECT_TRUE(run(
        "fib=function(n)\n"
        " if n<2 then return n end if\n"
        " return fib(n-1)+fib(n-2)\n"
        "end function\n"
        "print(fib(5))", o));
    EXPECT_EQ(o, "5");
}