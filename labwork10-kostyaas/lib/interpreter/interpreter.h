#pragma once

#include <variant>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <cmath>
#include <iostream>

#include "syntactic_analyser/SyntacticAnalyser.h"
#include "semantic_analyser/SemanticAnalyser.h"

struct NilType {};
struct FunctionObject;

struct Value {
    using Array   = std::vector<std::shared_ptr<Value>>;
    using FuncPtr = std::shared_ptr<FunctionObject>;

    std::variant<double, std::string, bool, NilType, Array, FuncPtr> data;

    Value();
    explicit Value(double v);
    explicit Value(const std::string& v);
    explicit Value(bool v);
    explicit Value(NilType v);
    explicit Value(const Array& v);
    explicit Value(FuncPtr v);
};

class Environment {
public:
    Environment();
    explicit Environment(Environment* parent);

    bool Define(const std::string& name, Value val);
    bool Assign(const std::string& name, Value val);
    Value Get(const std::string& name) const;

private:
    Environment* parent_;
    std::unordered_map<std::string, Value> values_;
};

class ReturnException : public std::runtime_error {
public:
    Value value;
    explicit ReturnException(Value v);
};

struct FunctionObject {
    using NativeFn = std::function<Value(const std::vector<Value>&)>;

    std::vector<std::string> params;
    const std::vector<Statement>* body;
    Environment* closure;
    NativeFn native;
    FunctionObject(std::vector<std::string> p, const std::vector<Statement>* b, Environment* c);
    explicit FunctionObject(NativeFn fn);
};

class Interpreter {
public:
    static bool Interpret(std::istream& in, std::ostream& out);

private:
    Environment globals_;
    std::ostream& output_;

    explicit Interpreter(std::ostream& out);

    void Functions();

    Value ParseNode(const Expression& expr, Environment* env);
    void Perform(const Statement& stmt, Environment* env);
    Value ParseList(const std::vector<Statement>& stmts, Environment* env);
    Value PerformFunction(const Value::FuncPtr& fn, const std::vector<Value>& args);

    bool IsTruthy(const Value& v) const;
    bool IsEqual(const Value& a, const Value& b) const;

    friend struct ParseExpression;
};

bool Interpreter(std::istream& in, std::ostream& out);