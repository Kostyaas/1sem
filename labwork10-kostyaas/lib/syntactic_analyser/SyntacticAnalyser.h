#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include "lexical_analyser/LexicalAnalyser.h"

struct NumberExpression {
  double Value;
};

struct StringExpression {
  std::string Value;
};

struct BoolExpression {
  bool Value;
};

struct NilExpression {};

struct VariableExpression {
  std::string Name;
};

struct UnaryExpression {
  TokenType Op;
  std::unique_ptr<struct Expression> Rhs;
};

struct BinaryExpression {
  std::unique_ptr<struct Expression> Lhs;
  TokenType Op;
  std::unique_ptr<struct Expression> Rhs;
};

struct CallExpression {
  std::unique_ptr<struct Expression> Callee;
  std::vector<std::unique_ptr<struct Expression>> Args;
};

struct ListExpression {
  std::vector<std::unique_ptr<struct Expression>> Elements;
};

struct FunctionExpression {
  std::vector<std::string> Params;
  std::vector<struct Statement> Body;
};

struct AssignExpression {
  std::string Name;
  TokenType Op;
  std::unique_ptr<Expression> Rhs;
};

struct IndexExpression {
  std::unique_ptr<Expression> Obj, Index;
};

struct SliceExpression {
  std::unique_ptr<Expression> Obj;
  std::unique_ptr<Expression> From, To;
};

using ExpressionVariant = std::variant<
  NumberExpression,
  StringExpression,
  BoolExpression,
  NilExpression,
  VariableExpression,
  UnaryExpression,
  BinaryExpression,
  CallExpression,
  ListExpression,
  FunctionExpression,
  AssignExpression,
  IndexExpression,
  SliceExpression
>;

struct Expression {
  ExpressionVariant Value;
  template<typename T>
  Expression(T&& v): Value(std::forward<T>(v)) {}
};

struct ExpressionStatement {
  Expression Expression;
};

struct IfStatement {
  Expression Condition;
  std::vector<struct Statement> ThenBranch;
  std::vector<struct Statement> ElseBranch;
};

struct WhileStatement {
  Expression Condition;
  std::vector<struct Statement> Body;
};

struct ForStatement {
  std::string Var;
  Expression Iterable;
  std::vector<struct Statement> Body;
};

struct ReturnStatement {
  std::unique_ptr<Expression> Value;
};

struct BlockStatement {
  std::vector<struct Statement> Statements;
};

using StatementVariant = std::variant<
  ExpressionStatement,
  IfStatement,
  WhileStatement,
  ForStatement,
  ReturnStatement,
  BlockStatement
>;

struct Statement {
  StatementVariant Value;
  template<typename T>
  Statement(T&& v): Value(std::forward<T>(v)) {}
};

class SyntacticAnalyser {
public:
  explicit SyntacticAnalyser(std::istream& in);
  std::vector<Statement> Parse();

private:
  LexicalAnalyser Lex_;
  Token Cur_;

  void Update();
  bool Match(TokenType t);
  void Check(TokenType t);

  Statement ParseStatement();
  Statement ParseIf();
  Statement ParseWhile();
  Statement ParseFor();
  Statement ParseReturn();
  std::vector<Statement> ParseBlock(std::initializer_list<TokenType> endTypes);

  Expression ParseExpression();
  Expression ParseAssignment();
  Expression ParseOr();
  Expression ParseAnd();
  Expression ParseEquality();
  Expression ParseComparison();
  Expression ParseTerm();
  Expression ParseFactor();
  Expression ParseUnary();
  Expression ParseCall();
  Expression ParsePrimary();
};