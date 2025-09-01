#include "lusoscript/interpreter.hh"

#include <assert.h>

#include <any>
#include <iostream>

#include "lusoscript/helper.hh"

Interpreter::Interpreter(error::ErrorState *error_state)
    : error_state_(error_state) {}

void Interpreter::interpret(const ast::Expr &root) {
  try {
    const auto value = evaluate(root);
    std::cout << stringify(value) << std::endl;
  } catch (error::RuntimeError &error) {
    error_state_->runtimeError(error);
  }
}

std::any Interpreter::evaluate(const ast::Expr &expr) {
  struct AnyVisitor {
    Interpreter &interpreter;

    std::any operator()(const ast::Ternary &ternary) {
      const std::any condition = interpreter.evaluate(*ternary.condition);

      return interpreter.isTruthy(condition)
                 ? interpreter.evaluate(*ternary.then_expr)
                 : interpreter.evaluate(*ternary.else_expr);
    }

    std::any operator()(const ast::Binary &binary) {
      const std::any left = interpreter.evaluate(*binary.left);
      const std::any right = interpreter.evaluate(*binary.right);

      switch (binary.opr.type) {
        case token::TokenType::SC_MINUS:
          interpreter.checkNumberOperands(binary.opr, left, right);
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

          throw error::RuntimeError(
              binary.opr, "Operands must be two numbers or two strings");
        case token::TokenType::SC_COMMA:
          return right;
        case token::TokenType::SC_FORWARD_SLASH:
          interpreter.checkNumberOperands(binary.opr, left, right);
          return std::any_cast<float>(left) / std::any_cast<float>(right);
        case token::TokenType::SC_STAR:
          interpreter.checkNumberOperands(binary.opr, left, right);
          return std::any_cast<float>(left) * std::any_cast<float>(right);
        case token::TokenType::MC_GREATER:
          interpreter.checkNumberOperands(binary.opr, left, right);
          return std::any_cast<float>(left) > std::any_cast<float>(right);
        case token::TokenType::MC_GREATER_EQUAL:
          interpreter.checkNumberOperands(binary.opr, left, right);
          return std::any_cast<float>(left) >= std::any_cast<float>(right);
        case token::TokenType::MC_LESS:
          interpreter.checkNumberOperands(binary.opr, left, right);
          return std::any_cast<float>(left) < std::any_cast<float>(right);
        case token::TokenType::MC_LESS_EQUAL:
          interpreter.checkNumberOperands(binary.opr, left, right);
          return std::any_cast<float>(left) <= std::any_cast<float>(right);
        case token::TokenType::MC_EXCL_EQUAL:
          return !interpreter.isEqual(left, right);
        case token::TokenType::MC_EQUAL_EQUAL:
          return interpreter.isEqual(left, right);
      }

      throw error::RuntimeError(
          binary.opr, "Binary operation '" + token::toString(binary.opr.type) +
                          "' not supported.");
    }

    std::any operator()(const ast::Grouping &grouping) {
      return interpreter.evaluate(*grouping.expression);
    }

    std::any operator()(const ast::Literal &literal) { return literal.value; }

    std::any operator()(const ast::Unary &unary) {
      const std::any right = interpreter.evaluate(*unary.right);

      switch (unary.opr.type) {
        case token::TokenType::MC_EXCL:
          return !interpreter.isTruthy(right);
        case token::TokenType::SC_MINUS:
          interpreter.checkNumberOperand(unary.opr, right);
          return -std::any_cast<float>(right);
      }

      throw error::RuntimeError(unary.opr, "Unary operation '" +
                                               token::toString(unary.opr.type) +
                                               "' not supported.");
    }

    std::any operator()(const ast::Error &error) {
      assert(false && "Overload not implemented.");
    }
  };
  AnyVisitor visitor{.interpreter = *this};
  return std::visit(visitor, expr.var);
}

std::string Interpreter::stringify(const std::any &value) {
  if (value.type() == typeid(nullptr)) return token::KW_NULO;

  if (value.type() == typeid(float)) {
    auto text = std::to_string(std::any_cast<float>(value));

    if (helper::endsWith(text, ".000000")) {
      text = text.substr(0, text.length() - 7);
    }

    return text;
  }

  if (value.type() == typeid(bool)) {
    return std::any_cast<bool>(value) ? token::KW_VERDADEIRO : token::KW_FALSO;
  }

  return std::any_cast<std::string>(value);
}

bool Interpreter::isTruthy(std::any value) {
  if (value.type() == typeid(nullptr)) return false;
  if (value.type() == typeid(bool)) return std::any_cast<bool>(value);
  return true;
}

bool Interpreter::isEqual(std::any a, std::any b) {
  // Strict equality comparison.
  if (a.type() == typeid(nullptr) && b.type() == typeid(nullptr)) return true;
  if (a.type() == typeid(std::string) && b.type() == typeid(std::string)) {
    return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
  }
  if (a.type() == typeid(bool) && b.type() == typeid(bool)) {
    return std::any_cast<bool>(a) == std::any_cast<bool>(b);
  }
  if (a.type() == typeid(float) && b.type() == typeid(float)) {
    return std::any_cast<float>(a) == std::any_cast<float>(b);
  }

  // Loose equality comparison (type coercion) is false.
  return false;
}

void Interpreter::checkNumberOperand(token::Token opr, std::any value) {
  if (value.type() == typeid(float)) return;
  throw error::RuntimeError(opr, "Operand must be a number");
}

void Interpreter::checkNumberOperands(token::Token opr, std::any left,
                                      std::any right) {
  if (left.type() == typeid(float) && right.type() == typeid(float)) return;
  throw error::RuntimeError(opr, "Operands must be numbers");
}
