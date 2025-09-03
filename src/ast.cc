#include "lusoscript/ast.hh"

#include <iostream>

std::string ast::AstPrinter::print(const Expr &expression) {
  struct ExprVisitor {
    AstPrinter &printer;

    void operator()(const Assign &assign) {
      printer.output_.append("(");

      printer.output_.append("assign");
      printer.output_.append(" ");
      printer.output_.append(assign.name.lexeme.value());
      printer.output_.append("[");
      printer.print(*assign.value);
      printer.output_.append("]");

      printer.output_.append(")");
    }

    void operator()(const Ternary &ternary) {
      printer.output_.append("(");

      printer.output_.append("ternary");
      printer.output_.append(" ");
      printer.print(*ternary.condition);
      printer.output_.append(" ");
      printer.output_.append(token::toString(ternary.then_opr.type));
      printer.output_.append(" ");
      printer.print(*ternary.then_expr);
      printer.output_.append(" ");
      printer.output_.append(token::toString(ternary.else_opr.type));
      printer.output_.append(" ");
      printer.print(*ternary.else_expr);

      printer.output_.append(")");
    }

    void operator()(const Binary &binary) {
      printer.output_.append("(");

      printer.output_.append(token::toString(binary.opr.type));
      printer.output_.append(" ");
      printer.print(*binary.left);
      printer.output_.append(" ");
      printer.print(*binary.right);

      printer.output_.append(")");
    }

    void operator()(const Grouping &grouping) {
      printer.output_.append("(");

      printer.output_.append("group");
      printer.output_.append(" ");
      printer.print(*grouping.expression);

      printer.output_.append(")");
    }

    void operator()(const Literal &literal) {
      switch (literal.token_type) {
        case token::TokenType::LT_NUMBER:
          printer.output_.append(
              std::to_string(std::any_cast<float>(literal.value)));
          break;
        case token::TokenType::LT_STRING:
          printer.output_.append(std::any_cast<std::string>(literal.value));
          break;
        case token::TokenType::KW_VERDADEIRO:
          printer.output_.append(token::KW_VERDADEIRO);
          break;
        case token::TokenType::KW_FALSO:
          printer.output_.append(token::KW_FALSO);
          break;
        default:
          throw std::runtime_error("Literal type cast not implemented.");
      };
    }

    void operator()(const Unary &unary) {
      printer.output_.append("(");

      printer.output_.append(token::toString(unary.opr.type));
      printer.output_.append(" ");
      printer.print(*unary.right);

      printer.output_.append(")");
    }

    void operator()(const Variable &variable) {
      const auto &lexeme = variable.name.lexeme;

      printer.output_.append("(");

      printer.output_.append("var");
      printer.output_.append(" ");
      printer.output_.append(lexeme.value());

      printer.output_.append(")");
    }

    void operator()(const ErrorExpr &error) {
      printer.output_.append("(");

      printer.output_.append("error");

      printer.output_.append(")");
    }
  };
  ExprVisitor visitor{.printer = *this};
  std::visit(visitor, expression.var);

  return output_;
}
