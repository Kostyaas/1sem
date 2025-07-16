#include <gtest/gtest.h>
#include <chrono>
#include <string>
#include <sstream>
#include "lexical_analyser/LexicalAnalyser.h"
#include "syntactic_analyser/SyntacticAnalyser.h"
#include "semantic_analyser/SemanticAnalyser.h"
#include "Interpreter.h"

using namespace std::chrono;

#define PERF_TEST(NAME, BODY)                                              \
TEST(Performance, NAME) {                                                  \
    auto t0 = steady_clock::now();                                         \
    BODY                                                                   \
    auto t1 = steady_clock::now();                                         \
    auto dt = duration_cast<milliseconds>(t1 - t0).count();                \
    std::cout << #NAME << ": " << dt << " ms\n";                           \
}

PERF_TEST(InterpretEmptyLoopMillion, {
    std::string script = "i=0\nwhile i < 1000000 then\ni=i+1\nend while";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})

PERF_TEST(InterpretForRangeMillion, {
    std::string script = "for i in range(0,1000000) do\nend for";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})

PERF_TEST(RecursiveFib30, {
    std::string script =
        "fib=function(n)\n"
        "  if n <= 1 then return n end if\n"
        "  return fib(n-1)+fib(n-2)\n"
        "end function\n"
        "print(fib(30))";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})

PERF_TEST(StringConcat100k, {
    std::string script = "s=\"\"\n";
    script += "for i in range(0,100000) do\n s=s+\"x\"\nend for\nprint(len(s))";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})

PERF_TEST(ListAppend100k, {
    std::string script = "a=[]\n";
    script += "for i in range(0,100000) do\n a=a+[i]\nend for\nprint(len(a))";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})

PERF_TEST(RangeCreationLarge, {
    std::string script = "r=range(0,1000000)\nprint(len(r))";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})

PERF_TEST(SliceLargeString, {
    std::string script = "s=\"\"";
    script.insert(script.end(), 1000000, 'a');
    script += "\nprint(len(s[100:999900]))";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})

PERF_TEST(SliceLargeArray, {
    std::string script = "a=range(0,1000000)\nprint(len(a[100:999900]))";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})

PERF_TEST(BooleanChain100k, {
    std::string script = "b=true\n";
    script += "for i in range(0,100000) do\n b=b and true\nend for\nprint(b)";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})

PERF_TEST(ArithmeticChain100k, {
    std::string script = "x=0\n";
    script += "for i in range(0,100000) do\n x=x+1\nend for\nprint(x)";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})

PERF_TEST(IndexingLargeArray100k, {
    std::string script = "a=range(0,1000000)\n";
    script += "for i in range(0,100000) do print(a[i*10]) end for";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})

PERF_TEST(BuiltinLenLarge, {
    std::string script = "s=\"\"; a=range(0,10000)\n";
    script += "for i in range(0,100000) do print(len(s)); print(len(a)) end for";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})

PERF_TEST(ComplexScript100kLines, {
    std::string script;
    for (int i = 0; i < 100000; ++i) script += "x=x+1\n";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})

PERF_TEST(CombinedStress, {
    std::string script =
        "fib=function(n)\n"
        "  if n<=1 then return n end if\n"
        "  return fib(n-1)+fib(n-2)\n"
        "end function\n"
        "s=\"\"\n"
        "for i in range(0,50000) do s=s+\"x\" end for\n"
        "a=range(0,50000)\n"
        "print(len(s), len(a), fib(20))";
    std::istringstream in(script);
    std::ostringstream out;
    Interpreter::Interpret(in, out);
})