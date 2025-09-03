#include "lusoscript/interpreter.hh"

#include <assert.h>

#include <iostream>

#include "lusoscript/helper.hh"

Interpreter::Interpreter(error::ErrorState *error_state)
    : error_state_(error_state), current_env_({}) {}

void Interpreter::interpret(const std::vector<ast::Stmt> &stmts) {
  try {
    for (const ast::Stmt &stmt : stmts) {
      execute(stmt);
    }
  } catch (error::RuntimeError &error) {
    error_state_->runtimeError(error);
  }
}

void Interpreter::execute(const ast::Stmt &stmt) {
  struct VoidVisitor {
    Interpreter &interpreter;

    void operator()(const ast::Block &block) {
      interpreter.executeBlock(block, Environment{interpreter.current_env_});
    };

    void operator()(const ast::Expression &expression) {
      interpreter.evaluate(*expression.expression);
    };

    void operator()(const ast::Imprima &imprima) {
      const std::any value = interpreter.evaluate(*imprima.expression);
      std::cout << interpreter.stringify(value) << std::endl;
    }

    void operator()(const ast::Var &variable) {
      std::any value = nullptr;

      const auto &identifier = variable.name.lexeme;
      const auto &initializer = variable.initializer;

      if (initializer.has_value()) {
        value = interpreter.evaluate(*initializer.value());
      }

      interpreter.current_env_.define(identifier.value(), value);
    }

    void operator()(const ast::ErrorStmt &error) {
      assert(false && "Overload not implemented.");
    }
  };
  VoidVisitor visitor{.interpreter = *this};
  std::visit(visitor, stmt.var);
}

void Interpreter::executeBlock(const ast::Block &block,
                               const Environment &env) {
  Environment &prev = current_env_;

  current_env_ = env;

  for (auto &stmt : block.stmts) {
    execute(*stmt);
  }

  current_env_ = prev;
}

std::any Interpreter::evaluate(const ast::Expr &expr) {
  struct AnyVisitor {
    Interpreter &interpreter;

    std::any operator()(const ast::Assign &assign) {
      const std::any &value = interpreter.evaluate(*assign.value);
      interpreter.current_env_.assign(assign.name, value);
      return value;
    }

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
          if (left.type() == right.type()) {
            return interpreter.combineStrict(binary.opr, left, right);
          } else {
            return interpreter.combineLoose(binary.opr, left, right);
          }
        case token::TokenType::SC_COMMA:
          return right;
        case token::TokenType::SC_FORWARD_SLASH: {
          interpreter.checkNumberOperands(binary.opr, left, right);
          const float divisor = std::any_cast<float>(right);
          if (divisor == 0.f) {
            throw error::RuntimeError(binary.opr,
                                      "Attempted to divide by zero");
          }
          return std::any_cast<float>(left) / divisor;
        }
        case token::TokenType::SC_STAR:
          interpreter.checkNumberOperands(binary.opr, left, right);
          return std::any_cast<float>(left) * std::any_cast<float>(right);
        case token::TokenType::MC_GREATER:
          if (left.type() == typeid(float) && right.type() == typeid(float)) {
            return std::any_cast<float>(left) > std::any_cast<float>(right);
          }

          if (left.type() == typeid(std::string) &&
              right.type() == typeid(std::string)) {
            return std::any_cast<std::string>(left) >
                   std::any_cast<std::string>(right);
          }

          throw error::RuntimeError(
              binary.opr, "Operands must be two numbers or two strings");
        case token::TokenType::MC_GREATER_EQUAL:
          if (left.type() == typeid(float) && right.type() == typeid(float)) {
            return std::any_cast<float>(left) >= std::any_cast<float>(right);
          }

          if (left.type() == typeid(std::string) &&
              right.type() == typeid(std::string)) {
            return std::any_cast<std::string>(left) >=
                   std::any_cast<std::string>(right);
          }

          throw error::RuntimeError(
              binary.opr, "Operands must be two numbers or two strings");
        case token::TokenType::MC_LESS:
          if (left.type() == typeid(float) && right.type() == typeid(float)) {
            return std::any_cast<float>(left) < std::any_cast<float>(right);
          }

          if (left.type() == typeid(std::string) &&
              right.type() == typeid(std::string)) {
            return std::any_cast<std::string>(left) <
                   std::any_cast<std::string>(right);
          }

          throw error::RuntimeError(
              binary.opr, "Operands must be two numbers or two strings");
        case token::TokenType::MC_LESS_EQUAL:
          if (left.type() == typeid(float) && right.type() == typeid(float)) {
            return std::any_cast<float>(left) <= std::any_cast<float>(right);
          }

          if (left.type() == typeid(std::string) &&
              right.type() == typeid(std::string)) {
            return std::any_cast<std::string>(left) <=
                   std::any_cast<std::string>(right);
          }

          throw error::RuntimeError(
              binary.opr, "Operands must be two numbers or two strings");
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

    std::any operator()(const ast::Variable &variable) {
      return interpreter.current_env_.get(variable.name);
    }

    std::any operator()(const ast::ErrorExpr &error) {
      assert(false && "Overload not implemented.");
    }
  };
  AnyVisitor visitor{.interpreter = *this};
  return std::visit(visitor, expr.var);
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

std::any Interpreter::combineStrict(const token::Token &opr,
                                    const std::any &left,
                                    const std::any &right) {
  if (left.type() == typeid(float) && right.type() == typeid(float)) {
    return std::any_cast<float>(left) + std::any_cast<float>(right);
  }

  if (left.type() == typeid(std::string) &&
      right.type() == typeid(std::string)) {
    return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
  }

  throw error::RuntimeError(
      opr,
      "Operands must be two numbers or two strings for strict combination");
}

std::any Interpreter::combineLoose(const token::Token &opr,
                                   const std::any &left,
                                   const std::any &right) {
  // Loose binary operations where the left operand is a string.
  if (left.type() == typeid(std::string)) {
    const auto left_str = std::any_cast<std::string>(left);

    if (right.type() == typeid(float)) {
      return left_str + stringify(right);
    }

    if (right.type() == typeid(bool)) {
      return left_str + stringify(right);
    }

    if (right.type() == typeid(nullptr)) {
      return left_str + stringify(right);
    }

    throw error::RuntimeError(opr, "Invalid right-hand side operand type");
  }

  // Loose binary operations where the left operand is a float.
  if (left.type() == typeid(float)) {
    auto left_str = stringify(left);

    if (right.type() == typeid(std::string)) {
      return left_str + std::any_cast<std::string>(right);
    }

    if (right.type() == typeid(bool) || right.type() == typeid(nullptr)) {
      return left_str;
    }

    throw error::RuntimeError(opr, "Invalid right-hand side operand type");
  }

  // Loose binary operations where the left operand is a boolean.
  if (left.type() == typeid(bool)) {
    if (right.type() == typeid(std::string)) {
      return stringify(left) + std::any_cast<std::string>(right);
    }

    if (right.type() == typeid(float)) {
      return stringify(right);
    }

    throw error::RuntimeError(opr, "Invalid right-hand side operand type");
  }

  throw error::RuntimeError(opr, "Unsupported loose combination operands");
}

std::string Interpreter::stringify(const std::any &value) {
  if (value.type() == typeid(nullptr)) return token::KW_NULO;

  if (value.type() == typeid(float)) {
    auto str = std::to_string(std::any_cast<float>(value));

    return helper::endsWith(str, ".000000") ? str.substr(0, str.length() - 7)
                                            : str;
  }

  if (value.type() == typeid(bool)) {
    return std::any_cast<bool>(value) ? token::KW_VERDADEIRO : token::KW_FALSO;
  }

  return std::any_cast<std::string>(value);
}
