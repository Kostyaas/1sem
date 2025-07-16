#pragma once

#include "interpreter/interpreter.h"

struct ParseExpression {
    class Interpreter* I;
    Environment* env;
    Value operator()(const NumberExpression& e) const { return Value(e.Value); }
    Value operator()(const StringExpression& e) const { return Value(e.Value); }
    Value operator()(const BoolExpression&   e) const { return Value(e.Value); }
    Value operator()(const NilExpression&    ) const { return Value(NilType{}); }
    Value operator()(const VariableExpression& e) const {
        return env->Get(e.Name);
    }
    Value operator()(const UnaryExpression& e) const {
        Value r = I->ParseNode(*e.Rhs, env);
        if (e.Op==TokenType::Minus) {
            return Value(-std::get<double>(r.data));
        }
        return Value(!I->IsTruthy(r));
    }
    Value operator()(const BinaryExpression& e) const {
        if (e.Op == TokenType::And) {
            Value L = I->ParseNode(*e.Lhs, env);
            if (!I->IsTruthy(L)) return L;
            return I->ParseNode(*e.Rhs, env);
        }
        if (e.Op == TokenType::Or) {
            Value L = I->ParseNode(*e.Lhs, env);
            if (I->IsTruthy(L)) return L;
            return I->ParseNode(*e.Rhs, env);
        }

        Value L = I->ParseNode(*e.Lhs, env);
        Value R = I->ParseNode(*e.Rhs, env);
        auto& a = L.data;
        auto& b = R.data;

        auto to_num = [&](const Value& v)->double {
            if (auto pd = std::get_if<double>(&v.data)) return *pd;
            if (auto pb = std::get_if<bool>(&v.data))   return *pb ? 1.0 : 0.0;
            throw std::runtime_error("Operand is not a number or bool");
        };

        switch (e.Op) {
          case TokenType::Plus:
            if (auto pd = std::get_if<double>(&a))
              return Value(*pd + to_num(R));
            if (auto ps = std::get_if<std::string>(&a))
              return Value(*ps + std::get<std::string>(b));
            break;

          case TokenType::Minus:
            if (auto pd = std::get_if<double>(&a))
              return Value(*pd - to_num(R));
            if (auto ps = std::get_if<std::string>(&a)) {
              std::string s = *ps, t = std::get<std::string>(b);
              if (s.size() >= t.size() &&
                  s.compare(s.size() - t.size(), t.size(), t) == 0)
                s.erase(s.size() - t.size());
              return Value(s);
            }
            break;

          case TokenType::Asterisk:
            if (auto pd = std::get_if<double>(&a))
              return Value(*pd * to_num(R));
            if (auto ps = std::get_if<std::string>(&a)) {
              std::string s = *ps;
              int times = static_cast<int>(std::floor(to_num(R)));
              std::string out;
              for (int i = 0; i < times; ++i) out += s;
              return Value(out);
            }
            break;

          case TokenType::Slash:
            return Value(to_num(L) / to_num(R));

          case TokenType::Percent:
            return Value(std::fmod(to_num(L), to_num(R)));

          case TokenType::Caret:
            return Value(std::pow(to_num(L), to_num(R)));

          case TokenType::DoubleEqual:
            return Value(I->IsEqual(L, R));
          case TokenType::NotEqual:
            return Value(!I->IsEqual(L, R));

          case TokenType::Less:
            if (std::holds_alternative<double>(L.data) || std::holds_alternative<bool>(L.data))
              return Value(to_num(L) < to_num(R));
            if (auto ps = std::get_if<std::string>(&a))
              return Value(*ps < std::get<std::string>(b));
            break;

          case TokenType::LessEqual:
            if (std::holds_alternative<double>(L.data) || std::holds_alternative<bool>(L.data))
              return Value(to_num(L) <= to_num(R));
            if (auto ps = std::get_if<std::string>(&a))
              return Value(*ps <= std::get<std::string>(b));
            break;

          case TokenType::Greater:
            if (std::holds_alternative<double>(L.data) || std::holds_alternative<bool>(L.data))
              return Value(to_num(L) > to_num(R));
            if (auto ps = std::get_if<std::string>(&a))
              return Value(*ps > std::get<std::string>(b));
            break;

          case TokenType::GreaterEqual:
            if (std::holds_alternative<double>(L.data) || std::holds_alternative<bool>(L.data))
              return Value(to_num(L) >= to_num(R));
            if (auto ps = std::get_if<std::string>(&a))
              return Value(*ps >= std::get<std::string>(b));
            break;

          default:
            break;
        }

        throw std::runtime_error("Bad operands for binary operation");
    }

    Value operator()(const CallExpression& e) const {
        Value c = I->ParseNode(*e.Callee, env);
        if (!std::holds_alternative<Value::FuncPtr>(c.data))
            throw std::runtime_error("Call of non-function");
        auto fn = std::get<Value::FuncPtr>(c.data);
        std::vector<Value> args2;
        for (auto& a : e.Args)
            args2.push_back(I->ParseNode(*a, env));
        return I->PerformFunction(fn, args2);
    }
    Value operator()(const ListExpression& e) const {
        Value::Array a;
        for (auto& el : e.Elements)
            a.push_back(std::make_shared<Value>(I->ParseNode(*el, env)));
        return Value(a);
    }
    Value operator()(const FunctionExpression& e) const {
        auto fnobj = std::make_shared<FunctionObject>(
            e.Params, &e.Body, env
        );
        return Value(fnobj);
    }
    Value operator()(const AssignExpression& e) const {
        Value val = I->ParseNode(*e.Rhs, env);
        if (!env->Assign(e.Name, val))
            env->Define(e.Name, val);
        return val;
    }

    Value operator()(const IndexExpression& e) const {
        Value obj = I->ParseNode(*e.Obj, env);
        Value idxv = I->ParseNode(*e.Index, env);
        int idx = static_cast<int>(std::get<double>(idxv.data));
        if (auto ps = std::get_if<std::string>(&obj.data)) {
            int n = (int)ps->size();
            if (idx < 0) idx += n;
            if (idx < 0 || idx >= n) throw std::runtime_error("String index out of range");
            return Value(std::string(1, (*ps)[idx]));
        }
        if (auto pa = std::get_if<Value::Array>(&obj.data)) {
            int n = (int)pa->size();
            if (idx < 0) idx += n;
            if (idx < 0 || idx >= n) throw std::runtime_error("Array index out of range");
            return *(*pa)[idx];
        }
        throw std::runtime_error("Indexing non-indexable type");
    }

    Value operator()(const SliceExpression& e) const {
        Value obj = I->ParseNode(*e.Obj, env);
        int from = static_cast<int>(std::get<double>(I->ParseNode(*e.From, env).data));
        int to;
        if (e.To) {
            to = static_cast<int>(std::get<double>(I->ParseNode(*e.To, env).data));
        } else {
            if (auto ps = std::get_if<std::string>(&obj.data))
                to = (int)ps->size();
            else if (auto pa = std::get_if<Value::Array>(&obj.data))
                to = (int)pa->size();
            else
                throw std::runtime_error("Slicing non-sliceable type");
        }
        if (auto ps = std::get_if<std::string>(&obj.data)) {
            int n = (int)ps->size();
            if (from < 0) from += n;
            if (to   < 0) to   += n;
            from = std::clamp(from, 0, n);
            to   = std::clamp(to,   0, n);
            return Value(ps->substr(from, to - from));
        }
        if (auto pa = std::get_if<Value::Array>(&obj.data)) {
            int n = (int)pa->size();
            if (from < 0) from += n;
            if (to   < 0) to   += n;
            from = std::clamp(from, 0, n);
            to   = std::clamp(to,   0, n);
            Value::Array res;
            for (int i = from; i < to; ++i)
                res.push_back(std::make_shared<Value>(*(*pa)[i]));
            return Value(res);
        }
        throw std::runtime_error("Slicing non-sliceable type");
    }
};