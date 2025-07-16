#pragma once

#include <vector>
#include <ostream>
#include <unordered_map>
#include "syntactic_analyser/SyntacticAnalyser.h"

class SymbolTable {
public:
    void EnterScope();
    void ExitScope();
    bool Declare(const std::string& name);
    bool Exists(const std::string& name) const;

private:
    std::vector<std::unordered_map<std::string,bool>> Scopes;
};

class SemanticAnalyser {
public:
    explicit SemanticAnalyser(std::ostream& errs);
    bool Analyse(const std::vector<Statement>& program);

private:
    std::ostream& Errs;
    SymbolTable Table;

    bool VisitStatement(const Statement& Statement);
    bool VisitExpression(const Expression& expr);

    bool CheckExpressionStatement(const ExpressionStatement& s);
    bool CheckIf(const IfStatement& s);
    bool CheckWhile(const WhileStatement& s);
    bool CheckFor(const ForStatement& s);
    bool CheckReturn(const ReturnStatement& s);
    bool CheckBlock(const BlockStatement& s);

    bool CheckLiteral(const NumberExpression& e);
    bool CheckLiteral(const StringExpression& e);
    bool CheckLiteral(const BoolExpression& e);
    bool CheckLiteral(const NilExpression& e);
    bool CheckVariable(const VariableExpression& e);
    bool CheckUnary(const UnaryExpression& e);
    bool CheckBinary(const BinaryExpression& e);
    bool CheckCall(const CallExpression& e);
    bool CheckList(const ListExpression& e);
};