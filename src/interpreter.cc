#include "lusoscript/interpreter.hh"

#include <assert.h>

#include <any>

void Interpreter::interpret(const ast::Expr &root) { evaluate(root); }

std::any Interpreter::evaluate(const ast::Expr &expr) {
  struct AnyVisitor {
    Interpreter &interpreter;

    std::any operator()(const ast::Ternary &ternary) {
      assert(false && "Overload not implemented.");
    }

    std::any operator()(const ast::Binary &binary) {
      const std::any left = interpreter.evaluate(*binary.left);
      const std::any right = interpreter.evaluate(*binary.right);

      switch (binary.opr.type) {
        case token::TokenType::SC_MINUS:
          return std::any_cast<float>(left) - std::any_cast<float>(right);
        case token::TokenType::SC_PLUS:
          if (left.type() == typeid(float) && right.type() == typeid(float)) {
            return std::any_cast<float>(left) + std::any_cast<float>(right);
          }

          if (left.type() == typeid(std::string) &&
              right.type() == typeid(std::string)) {
            return std::any_cast<std::string>(left) +
                   std::any_cast<std::string>(right);
          }
        case token::TokenType::SC_FORWARD_SLASH:
          return std::any_cast<float>(left) / std::any_cast<float>(right);
        case token::TokenType::SC_STAR:
          return std::any_cast<float>(left) / std::any_cast<float>(right);
        case token::TokenType::MC_GREATER:
          return std::any_cast<float>(left) > std::any_cast<float>(right);
        case token::TokenType::MC_GREATER_EQUAL:
          return std::any_cast<float>(left) >= std::any_cast<float>(right);
        case token::TokenType::MC_LESS:
          return std::any_cast<float>(left) < std::any_cast<float>(right);
        case token::TokenType::MC_LESS_EQUAL:
          return std::any_cast<float>(left) <= std::any_cast<float>(right);
        case token::TokenType::MC_EXCL_EQUAL:
          return !interpreter.isEqual(left, right);
        case token::TokenType::MC_EQUAL_EQUAL:
          return interpreter.isEqual(left, right);
      }

      // TODO: Unreachable.
      return nullptr;
    }

    std::any operator()(const ast::Grouping &grouping) {
      return interpreter.evaluate(*grouping.expression);
    }

    std::any operator()(const ast::Literal &literal) { return literal.value; }

    std::any operator()(const ast::Unary &unary) {
      const std::any right = interpreter.evaluate(*unary.right);

      switch (unary.opr.type) {
        case token::TokenType::MC_EXCL:
          return interpreter.isTruthy(right);
        case token::TokenType::SC_MINUS:
          return -std::any_cast<float>(right);
      }

      // TODO: Unreachable.
      return nullptr;
    }

    std::any operator()(const ast::Error &error) {
      assert(false && "Overload not implemented.");
    }
  };
  AnyVisitor visitor{.interpreter = *this};
  std::visit(visitor, expr.var);
}

bool Interpreter::isTruthy(std::any value) {
  if (value.type() == typeid(nullptr)) return false;
  if (value.type() == typeid(bool)) return std::any_cast<bool>(value);
  return true;
}

bool Interpreter::isEqual(std::any a, std::any b) {
  if (a.type() == typeid(nullptr) && b.type() == typeid(nullptr)) return true;
  if (a.type() == typeid(nullptr)) return false;

  // TODO: implement comparison.
  assert(false && "Comparison not implemented.");
}
