#include "SemanticAnalyser.h"

void SymbolTable::EnterScope() {
    Scopes.emplace_back();
}

void SymbolTable::ExitScope() {
    if (!Scopes.empty()) Scopes.pop_back();
}

bool SymbolTable::Declare(const std::string& name) {
    if (Scopes.empty()) EnterScope();
    auto& top = Scopes.back();
    if (top.count(name)) return false;
    top[name] = true;
    return true;
}

bool SymbolTable::Exists(const std::string& name) const {
    for (auto it = Scopes.rbegin(); it != Scopes.rend(); ++it) {
        if (it->count(name)) return true;
    }
    return false;
}

SemanticAnalyser::SemanticAnalyser(std::ostream& errs)
    : Errs(errs) {}

bool SemanticAnalyser::Analyse(const std::vector<Statement>& program) {
    Table.EnterScope();

    const char* builtins[] = {
        "print", "println", "read", "stacktrace",
        "abs", "ceil", "floor", "round", "sqrt", "rnd",
        "parse_num", "to_string",
        "len", "lower", "upper", "split", "join", "replace",
        "range",
        "push", "pop", "insert", "remove", "sort"
    };
    for (auto name : builtins) {
        Table.Declare(name);
    }

    bool ok = true;
    for (auto& st : program) ok &= VisitStatement(st);
    Table.ExitScope();
    return ok;
}

bool SemanticAnalyser::VisitStatement(const Statement& Statement) {
    return std::visit([this](auto&& s) -> bool {
        using T = std::decay_t<decltype(s)>;
        if constexpr (std::is_same_v<T, ExpressionStatement>)   return CheckExpressionStatement(s);
        if constexpr (std::is_same_v<T, IfStatement>)     return CheckIf(s);
        if constexpr (std::is_same_v<T, WhileStatement>)  return CheckWhile(s);
        if constexpr (std::is_same_v<T, ForStatement>)    return CheckFor(s);
        if constexpr (std::is_same_v<T, ReturnStatement>) return CheckReturn(s);
        if constexpr (std::is_same_v<T, BlockStatement>)  return CheckBlock(s);
        return true;
    }, Statement.Value);
}

bool SemanticAnalyser::VisitExpression(const Expression& expr) {
    return std::visit([this](auto&& e) -> bool {
        using T = std::decay_t<decltype(e)>;
        if constexpr (std::is_same_v<T, NumberExpression> ||
                      std::is_same_v<T, StringExpression> ||
                      std::is_same_v<T, BoolExpression>   ||
                      std::is_same_v<T, NilExpression>)    return true;
        if constexpr (std::is_same_v<T, VariableExpression>) return CheckVariable(e);
        if constexpr (std::is_same_v<T, UnaryExpression>)    return CheckUnary(e);
        if constexpr (std::is_same_v<T, BinaryExpression>)   return CheckBinary(e);
        if constexpr (std::is_same_v<T, CallExpression>)     return CheckCall(e);
        if constexpr (std::is_same_v<T, ListExpression>)     return CheckList(e);
        if constexpr (std::is_same_v<T, FunctionExpression>) {
            Table.EnterScope();
            for (auto& p : e.Params) Table.Declare(p);
            bool ok = true;
            for (auto& st : e.Body) ok &= VisitStatement(st);
            Table.ExitScope();
            return ok;
        }
        if constexpr (std::is_same_v<T, AssignExpression>) {
            if (!Table.Exists(e.Name)) {
                Table.Declare(e.Name);
            }
            return VisitExpression(*e.Rhs);
        }
        return true;
    }, expr.Value);
}

bool SemanticAnalyser::CheckExpressionStatement(const ExpressionStatement& s) {
    return VisitExpression(s.Expression);
}

bool SemanticAnalyser::CheckIf(const IfStatement& s) {
    bool ok = VisitExpression(s.Condition);
    Table.EnterScope();
    for (auto& st : s.ThenBranch) ok &= VisitStatement(st);
    Table.ExitScope();
    if (!s.ElseBranch.empty()) {
        Table.EnterScope();
        for (auto& st : s.ElseBranch) ok &= VisitStatement(st);
        Table.ExitScope();
    }
    return ok;
}

bool SemanticAnalyser::CheckWhile(const WhileStatement& s) {
    bool ok = VisitExpression(s.Condition);
    Table.EnterScope();
    for (auto& st : s.Body) ok &= VisitStatement(st);
    Table.ExitScope();
    return ok;
}

bool SemanticAnalyser::CheckFor(const ForStatement& s) {
    bool ok = VisitExpression(s.Iterable);
    Table.EnterScope();
    ok &= Table.Declare(s.Var);
    for (auto& st : s.Body) ok &= VisitStatement(st);
    Table.ExitScope();
    return ok;
}

bool SemanticAnalyser::CheckReturn(const ReturnStatement& s) {
    return s.Value ? VisitExpression(*s.Value) : true;
}

bool SemanticAnalyser::CheckBlock(const BlockStatement& s) {
    Table.EnterScope();
    bool ok = true;
    for (auto& st : s.Statements) ok &= VisitStatement(st);
    Table.ExitScope();
    return ok;
}

bool SemanticAnalyser::CheckLiteral(const NumberExpression&) {
    return true;
}

bool SemanticAnalyser::CheckLiteral(const StringExpression&) {
    return true;
}

bool SemanticAnalyser::CheckLiteral(const BoolExpression&) {
    return true;
}

bool SemanticAnalyser::CheckLiteral(const NilExpression&) {
    return true;
}

bool SemanticAnalyser::CheckVariable(const VariableExpression& e) {
    if (!Table.Exists(e.Name)) {
        Errs << "Undefined variable: " << e.Name << "\n";
        return false;
    }
    return true;
}

bool SemanticAnalyser::CheckUnary(const UnaryExpression& e) {
    return VisitExpression(*e.Rhs);
}

bool SemanticAnalyser::CheckBinary(const BinaryExpression& e) {
    bool ok = VisitExpression(*e.Lhs);
    ok &= VisitExpression(*e.Rhs);
    return ok;
}

bool SemanticAnalyser::CheckCall(const CallExpression& e) {
    bool ok = VisitExpression(*e.Callee);
    for (auto& arg : e.Args) ok &= VisitExpression(*arg);
    return ok;
}

bool SemanticAnalyser::CheckList(const ListExpression& e) {
    bool ok = true;
    for (auto& el : e.Elements) ok &= VisitExpression(*el);
    return ok;
}
