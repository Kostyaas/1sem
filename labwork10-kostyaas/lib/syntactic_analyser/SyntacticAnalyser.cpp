#include "SyntacticAnalyser.h"
#include <stdexcept>
#include <algorithm>

SyntacticAnalyser::SyntacticAnalyser(std::istream& in)
    : Lex_(in), Cur_(Lex_.Next()) {}

std::vector<Statement> SyntacticAnalyser::Parse() {
  std::vector<Statement> Program;
  while (Cur_.type != TokenType::EndOfFile) {
    Program.push_back(ParseStatement());
  }
  return Program;
}

void SyntacticAnalyser::Update() {
  Cur_ = Lex_.Next();
}

bool SyntacticAnalyser::Match(TokenType t) {
  if (Cur_.type == t) { Update(); return true; }
  return false;
}

void SyntacticAnalyser::Check(TokenType t) {
  if (Cur_.type == t) { Update(); return; }
  throw std::runtime_error("Checked token");
}

Statement SyntacticAnalyser::ParseStatement() {
  if (Cur_.type == TokenType::If)    { Update(); return ParseIf(); }
  if (Cur_.type == TokenType::While) { Update(); return ParseWhile(); }
  if (Cur_.type == TokenType::For)   { Update(); return ParseFor(); }
  if (Cur_.type == TokenType::Return){ Update(); return ParseReturn(); }
  Expression E = ParseExpression();
  return Statement{ExpressionStatement{std::move(E)}};
}

Statement SyntacticAnalyser::ParseIf() {
  Expression Cond = ParseExpression();
  Check(TokenType::Then);

  auto ThenB = ParseBlock({TokenType::Else, TokenType::End});

  std::vector<Statement> ElseB;
  IfStatement* Cur_rent = nullptr;
  IfStatement head{std::move(Cond), std::move(ThenB), {}};
  Cur_rent = &head;

  while (Cur_.type == TokenType::Else) {
    Update();
    if (Cur_.type == TokenType::If) {
      Update();
      Expression eCond = ParseExpression();
      Check(TokenType::Then);
      auto eThen = ParseBlock({TokenType::Else, TokenType::End});
      Cur_rent->ElseBranch.push_back(Statement{ IfStatement{ std::move(eCond), std::move(eThen), {} } });
      auto& nested = std::get<IfStatement>(Cur_rent->ElseBranch.back().Value);
      Cur_rent = &nested;
    } else {
      Cur_rent->ElseBranch = ParseBlock({TokenType::End});
      break;
    }
  }

  Check(TokenType::End);
  Check(TokenType::If);

  return Statement{ std::move(head) };
}


Statement SyntacticAnalyser::ParseWhile() {
  Expression Cond = ParseExpression();
  auto Body = ParseBlock({TokenType::End});
  Check(TokenType::End);
  Check(TokenType::While);
  return Statement{WhileStatement{std::move(Cond), std::move(Body)}};
}

Statement SyntacticAnalyser::ParseFor() {
  if (Cur_.type != TokenType::Identifier)
    throw std::runtime_error("Checked identifier in for");
  std::string Var = Cur_.lexeme;
  Update();
  Check(TokenType::In);
  Expression It = ParseExpression();
  auto Body = ParseBlock({TokenType::End});
  Check(TokenType::End);
  Check(TokenType::For);
  return Statement{ForStatement{std::move(Var), std::move(It), std::move(Body)}};
}

Statement SyntacticAnalyser::ParseReturn() {
  std::unique_ptr<Expression> Val;
  if (Cur_.type != TokenType::End && Cur_.type != TokenType::EndOfFile) {
    Val = std::make_unique<Expression>(ParseExpression());
  }
  return Statement{ReturnStatement{std::move(Val)}};
}

std::vector<Statement> SyntacticAnalyser::ParseBlock(std::initializer_list<TokenType> endTypes) {
  std::vector<Statement> Statements;
  while (Cur_.type != TokenType::EndOfFile &&
         std::find(endTypes.begin(), endTypes.end(), Cur_.type) == endTypes.end()) {
    Statements.push_back(ParseStatement());
  }
  return Statements;
}

Expression SyntacticAnalyser::ParseExpression() {
  return ParseAssignment();
}

Expression SyntacticAnalyser::ParseAssignment() {
  Expression E = ParseOr();
  if (Cur_.type == TokenType::Assign || Cur_.type == TokenType::PlusEqual ||
      Cur_.type == TokenType::MinusEqual || Cur_.type == TokenType::AsteriskEqual ||
      Cur_.type == TokenType::SlashEqual || Cur_.type == TokenType::PercentEqual ||
      Cur_.type == TokenType::CaretEqual) {
    TokenType Op = Cur_.type;
    Update();
    auto rhs = std::make_unique<Expression>(ParseAssignment());
    if (!std::holds_alternative<VariableExpression>(E.Value))
      throw std::runtime_error("Invalid assignment target");
    auto var = std::get<VariableExpression>(std::move(E.Value)).Name;
    return Expression{AssignExpression{std::move(var), Op, std::move(rhs)}};
  }
  return E;
}

Expression SyntacticAnalyser::ParseOr() {
  Expression E = ParseAnd();
  while (Cur_.type == TokenType::Or) {
    TokenType Op = Cur_.type;
    Update();
    Expression R = ParseAnd();
    E = Expression{BinaryExpression{std::make_unique<Expression>(std::move(E)), Op, std::make_unique<Expression>(std::move(R))}};
  }
  return E;
}

Expression SyntacticAnalyser::ParseAnd() {
  Expression E = ParseEquality();
  while (Cur_.type == TokenType::And) {
    TokenType Op = Cur_.type;
    Update();
    Expression R = ParseEquality();
    E = Expression{BinaryExpression{std::make_unique<Expression>(std::move(E)), Op, std::make_unique<Expression>(std::move(R))}};
  }
  return E;
}

Expression SyntacticAnalyser::ParseEquality() {
  Expression E = ParseComparison();
  while (Cur_.type == TokenType::DoubleEqual || Cur_.type == TokenType::NotEqual) {
    TokenType Op = Cur_.type;
    Update();
    Expression R = ParseComparison();
    E = Expression{BinaryExpression{std::make_unique<Expression>(std::move(E)), Op, std::make_unique<Expression>(std::move(R))}};
  }
  return E;
}

Expression SyntacticAnalyser::ParseComparison() {
  Expression E = ParseTerm();
  while (Cur_.type == TokenType::Less || Cur_.type == TokenType::LessEqual ||
         Cur_.type == TokenType::Greater || Cur_.type == TokenType::GreaterEqual) {
    TokenType Op = Cur_.type;
    Update();
    Expression R = ParseTerm();
    E = Expression{BinaryExpression{std::make_unique<Expression>(std::move(E)), Op, std::make_unique<Expression>(std::move(R))}};
  }
  return E;
}

Expression SyntacticAnalyser::ParseTerm() {
  Expression E = ParseFactor();
  while (Cur_.type == TokenType::Plus || Cur_.type == TokenType::Minus) {
    TokenType Op = Cur_.type;
    Update();
    Expression R = ParseFactor();
    E = Expression{BinaryExpression{std::make_unique<Expression>(std::move(E)), Op, std::make_unique<Expression>(std::move(R))}};
  }
  return E;
}

Expression SyntacticAnalyser::ParseFactor() {
  Expression E = ParseUnary();
  while (Cur_.type == TokenType::Asterisk || Cur_.type == TokenType::Slash ||
         Cur_.type == TokenType::Percent  || Cur_.type == TokenType::Caret) {
    TokenType Op = Cur_.type;
    Update();
    Expression R = ParseUnary();
    E = Expression{BinaryExpression{std::make_unique<Expression>(std::move(E)), Op, std::make_unique<Expression>(std::move(R))}};
  }
  return E;
}

Expression SyntacticAnalyser::ParseUnary() {
  if (Cur_.type == TokenType::Not || Cur_.type == TokenType::Minus || Cur_.type == TokenType::Plus) {
    TokenType Op = Cur_.type;
    Update();
    Expression R = ParseUnary();
    return Expression{UnaryExpression{Op, std::make_unique<Expression>(std::move(R))}};
  }
  return ParseCall();
}

Expression SyntacticAnalyser::ParseCall() {
  Expression E = ParsePrimary();
  while (true) {
    if (Cur_.type == TokenType::LParen) {
      Update();
      std::vector<std::unique_ptr<Expression>> args;
      if (Cur_.type != TokenType::RParen) {
        do {
          args.push_back(std::make_unique<Expression>(ParseExpression()));
        } while (Match(TokenType::Comma));
      }
      Check(TokenType::RParen);
      E = Expression{ CallExpression{ std::make_unique<Expression>(std::move(E)), std::move(args) } };
    }
    else if (Cur_.type == TokenType::LBracket) {
      Update();
      Expression from = ParseExpression();
      if (Match(TokenType::Colon)) {
        std::unique_ptr<Expression> to;
        if (Cur_.type != TokenType::RBracket)
          to = std::make_unique<Expression>(ParseExpression());
        Check(TokenType::RBracket);
        E = Expression{ SliceExpression{
          std::make_unique<Expression>(std::move(E)),
          std::make_unique<Expression>(std::move(from)),
          std::move(to)
      } };
      } else {
        Check(TokenType::RBracket);
        E = Expression{ IndexExpression{
          std::make_unique<Expression>(std::move(E)),
          std::make_unique<Expression>(std::move(from))
      } };
      }
    }
    else {
      break;
    }
  }
  return E;
}

Expression SyntacticAnalyser::ParsePrimary() {
  if (Cur_.type == TokenType::Number) {
    Token tok = Cur_; Update();
    return Expression{NumberExpression{std::stod(tok.lexeme)}};
  }
  if (Cur_.type == TokenType::String) {
    Token tok = Cur_; Update();
    return Expression{StringExpression{tok.lexeme}};
  }
  if (Cur_.type == TokenType::Boolean) {
    Token tok = Cur_; Update();
    return Expression{BoolExpression{tok.lexeme == "true"}};
  }
  if (Cur_.type == TokenType::Nil) {
    Update();
    return Expression{NilExpression{}};
  }
  if (Cur_.type == TokenType::Identifier) {
    Token tok = Cur_; Update();
    return Expression{VariableExpression{tok.lexeme}};
  }
  if (Cur_.type == TokenType::LBracket) {
    Update();
    std::vector<std::unique_ptr<Expression>> Elements;
    while (Cur_.type != TokenType::RBracket) {
      Elements.push_back(std::make_unique<Expression>(ParseExpression()));
      if (!Match(TokenType::Comma)) break;
    }
    Check(TokenType::RBracket);
    return Expression{ListExpression{std::move(Elements)}};
  }
  if (Cur_.type == TokenType::Function) {
    Update();
    Check(TokenType::LParen);
    std::vector<std::string> Params;
    if (Cur_.type != TokenType::RParen) {
      do {
        if (Cur_.type != TokenType::Identifier)
          throw std::runtime_error("Checked parameter name");
        Params.push_back(Cur_.lexeme);
        Update();
      } while (Match(TokenType::Comma));
    }
    Check(TokenType::RParen);
    auto Body = ParseBlock({TokenType::End});
    Check(TokenType::End);
    Check(TokenType::Function);
    return Expression{FunctionExpression{std::move(Params), std::move(Body)}};
  }
  if (Cur_.type == TokenType::LParen) {
    Update();
    Expression E = ParseExpression();
    Check(TokenType::RParen);
    return E;
  }
  throw std::runtime_error("Checked Expressionession");
}
