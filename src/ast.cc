#include "lusoscript/ast.hh"

#include <iostream>

void throwIfBooleanHasNoValue(std::optional<bool> opt_bool) {
  if (!opt_bool.has_value()) {
    throw std::runtime_error("Expected boolean value.");
  }
}

void throwIfLiteralHasNoValue(std::optional<std::string> opt_lit) {
  if (!opt_lit.has_value()) {
    throw std::runtime_error("Expected a literal value.");
  }
}

std::string ast::AstPrinter::print(const Expr &expression) {
  struct ExprVisitor {
    AstPrinter &printer;

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
      std::string str;

      if (literal.type == LiteralType::BOOLEAN) {
        throwIfBooleanHasNoValue(literal.boolean);

        str.append(literal.boolean.value() == true ? "true" : "false");
      } else if (literal.type == LiteralType::NULO) {
        str.append(token::KW_NULO);
      } else {
        throwIfLiteralHasNoValue(literal.str_num);

        str.append(literal.str_num.value());
      }

      printer.output_.append(str);
    }

    void operator()(const Unary &unary) {
      printer.output_.append("(");

      printer.output_.append(token::toString(unary.opr.type));
      printer.output_.append(" ");
      printer.print(*unary.right);

      printer.output_.append(")");
    }
  };
  ExprVisitor visitor{.printer = *this};
  std::visit(visitor, expression.var);

  return output_;
}
