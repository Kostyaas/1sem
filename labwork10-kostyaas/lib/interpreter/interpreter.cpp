#include "Interpreter.h"
#include "utils/ParseExpression.h"

Value::Value() : data(NilType{}) {}
Value::Value(double v)            : data(v) {}
Value::Value(const std::string& v): data(v) {}
Value::Value(bool v)              : data(v) {}
Value::Value(NilType v)           : data(v) {}
Value::Value(const Array& v)      : data(v) {}
Value::Value(FuncPtr v)           : data(v) {}

Environment::Environment(): parent_(nullptr) {}
Environment::Environment(Environment* parent): parent_(parent) {}

bool Environment::Define(const std::string& name, Value val) {
    if (values_.count(name)) return false;
    values_[name] = std::move(val);
    return true;
}

bool Environment::Assign(const std::string& name, Value val) {
    if (values_.count(name)) {
        values_[name] = std::move(val);
        return true;
    }
    if (parent_) return parent_->Assign(name, std::move(val));
    return false;
}

Value Environment::Get(const std::string& name) const {
    if (values_.count(name)) return values_.at(name);
    if (parent_) return parent_->Get(name);
    throw std::runtime_error("Undefined variable: " + name);
}

ReturnException::ReturnException(Value v) : std::runtime_error("Return"), value(std::move(v)) {}

FunctionObject::FunctionObject(std::vector<std::string> p, const std::vector<Statement>* b, Environment* c) : params(std::move(p)), body(b), closure(c), native(nullptr) {}

FunctionObject::FunctionObject(NativeFn fn) : params(), body(nullptr), closure(nullptr), native(std::move(fn)) {}


bool Interpreter::Interpret(std::istream& in, std::ostream& out) {
    try {
        SyntacticAnalyser parser(in);
        auto program = parser.Parse();

        SemanticAnalyser sem(out);
        if (!sem.Analyse(program)) {
            return false;
        }

        Interpreter interp(out);
        interp.Functions();
        interp.ParseList(program, &interp.globals_);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Interpreter error: " << e.what() << "\n";
        return false;
    } catch (...) {
        std::cerr << "Interpreter error: unknown\n";
        return false;
    }
}

Interpreter::Interpreter(std::ostream& out) : globals_(), output_(out) {}

void Interpreter::Functions() {
    globals_.Define("print",
        Value(std::make_shared<FunctionObject>(
            FunctionObject::NativeFn(
                [this](const std::vector<Value>& args) -> Value {
                    if (!args.empty()) {
                        const auto& v = args[0].data;
                        if (auto pd = std::get_if<double>(&v)) {
                            double d = *pd;
                            if (d == static_cast<int64_t>(d))
                                output_ << static_cast<int64_t>(d);
                            else
                                output_ << d;
                        }
                        else if (auto ps = std::get_if<std::string>(&v)) {
                            const std::string& s = *ps;
                            if (s.find(' ') != std::string::npos)
                                output_ << '"' << s << '"';
                            else
                                output_ << s;
                        }
                        else if (auto pb = std::get_if<bool>(&v)) {
                            output_ << (*pb ? "true" : "false");
                        }
                        else {
                            output_ << "nil";
                        }
                    }
                    return Value(NilType{});
                }
            )
        ))
    );

    globals_.Define("len",
        Value(std::make_shared<FunctionObject>(
            FunctionObject::NativeFn(
                [](const std::vector<Value>& args) -> Value {
                    if (args.empty()) return Value(0.0);
                    const auto& v = args[0].data;
                    if (auto ps = std::get_if<std::string>(&v))
                        return Value(static_cast<double>(ps->size()));
                    if (auto pa = std::get_if<Value::Array>(&v))
                        return Value(static_cast<double>(pa->size()));
                    throw std::runtime_error("len() argument must be string or array");
                }
            )
        ))
    );

    globals_.Define("range",
    Value(std::make_shared<FunctionObject>(
        FunctionObject::NativeFn(
            [](const std::vector<Value>& args)->Value {
                double start = 0, end = 0, step = 1;
                if (args.size() == 1) {
                    end = std::get<double>(args[0].data);
                } else if (args.size() == 2) {
                    start = std::get<double>(args[0].data);
                    end   = std::get<double>(args[1].data);
                } else if (args.size() >= 3) {
                    start = std::get<double>(args[0].data);
                    end   = std::get<double>(args[1].data);
                    step  = std::get<double>(args[2].data);
                } else {
                    throw std::runtime_error("range() expects 1, 2 or 3 args");
                }
                if (step == 0) throw std::runtime_error("range() step cannot be zero");

                Value::Array res;
                if (step > 0) {
                    for (double v = start; v < end; v += step)
                        res.push_back(std::make_shared<Value>(v));
                } else {
                    for (double v = start; v > end; v += step)
                        res.push_back(std::make_shared<Value>(v));
                }
                return Value(res);
            }
        )
    )));

}

Value Interpreter::PerformFunction(
        const Value::FuncPtr& fn,
        const std::vector<Value>& args)
{
    if (fn->native) {
        return fn->native(args);
    }
    Environment local(fn->closure);
    for (size_t i = 0; i < fn->params.size(); ++i) {
        Value v = (i < args.size() ? args[i] : Value(NilType{}));
        local.Define(fn->params[i], v);
    }
    try {
        for (size_t i = 0; i < fn->body->size(); ++i) {
            Perform((*fn->body)[i], &local);
        }
    } catch (const ReturnException& r) {
        return r.value;
    }
    return Value(NilType{});
}

Value Interpreter::ParseNode(const Expression& expr, Environment* env) {
    return std::visit(ParseExpression{this,env}, expr.Value);
}

void Interpreter::Perform(const Statement& stmt, Environment* env) {
    std::visit([&](auto&& s){
        using T = std::decay_t<decltype(s)>;
        if constexpr(std::is_same_v<T, ExpressionStatement>) {
            ParseNode(s.Expression, env);
        }
        else if constexpr(std::is_same_v<T, IfStatement>) {
            if (IsTruthy(ParseNode(s.Condition, env)))
                ParseList(s.ThenBranch, env);
            else if (!s.ElseBranch.empty())
                ParseList(s.ElseBranch, env);
        }
        else if constexpr(std::is_same_v<T, WhileStatement>) {
            while (IsTruthy(ParseNode(s.Condition, env)))
                ParseList(s.Body, env);
        }
        else if constexpr(std::is_same_v<T, ForStatement>) {
            Value iterable = ParseNode(s.Iterable, env);
            if (!std::holds_alternative<Value::Array>(iterable.data))
                throw std::runtime_error("Can only iterate arrays");
            auto& arr = std::get<Value::Array>(iterable.data);
            for (size_t idx = 0; idx < arr.size(); ++idx) {
                Environment loopEnv(env);
                loopEnv.Define(s.Var, *arr[idx]);
                ParseList(s.Body, &loopEnv);
            }
        }

        else if constexpr(std::is_same_v<T, ReturnStatement>) {
            if (s.Value)
                throw ReturnException(ParseNode(*s.Value, env));
            throw ReturnException(Value(NilType{}));
        }
        else if constexpr(std::is_same_v<T, BlockStatement>) {
            ParseList(s.Statements, env);
        }
        else if constexpr (std::is_same_v<AssignExpression, T>) {
          Value val = ParseNode(*s.Rhs, env);
          if (s.Op == TokenType::Assign) {
            if (!env->Assign(s.Name, val))
                env->Define(s.Name, val);
            } else {
              auto old = env->Get(s.Name);
              env->Assign(s.Name, old + val);
            }
          }
    }, stmt.Value);
}

Value Interpreter::ParseList(
        const std::vector<Statement>& stmts,
        Environment* parent)
{
    Environment block(parent);
    for (size_t i = 0; i < stmts.size(); ++i) {
        Perform(stmts[i], &block);
    }
    return Value(NilType{});
}

bool Interpreter::IsTruthy(const Value& v) const {
    if (auto pb = std::get_if<bool>(&v.data)) return *pb;
    if (std::holds_alternative<NilType>(v.data))  return false;
    return true;
}

bool Interpreter::IsEqual(const Value& a, const Value& b) const {
    if (a.data.index() != b.data.index()) return false;
    if (auto ad = std::get_if<double>(&a.data))
        return *ad == std::get<double>(b.data);
    if (auto as = std::get_if<std::string>(&a.data))
        return *as == std::get<std::string>(b.data);
    if (auto ab = std::get_if<bool>(&a.data))
        return *ab == std::get<bool>(b.data);
    if (std::holds_alternative<NilType>(a.data)) return true;
    return false;
}

bool Interpreter(std::istream& in, std::ostream& out) {
    return Interpreter::Interpret(in, out);
}