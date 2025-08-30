#include "lusoscript/ast.hh"

#include <iostream>

std::string ast::AstPrinter::print(const Expr &expression) {
  struct ExprVisitor {
    AstPrinter &printer;

    void operator()(const Binary &binary) {
      printer.output_.append("(");

      if (!binary.opr.lexeme.has_value()) {
        std::cerr << "Expected a binary expression operator." << std::endl;
        exit(EXIT_FAILURE);
      }

      printer.output_.append(binary.opr.lexeme.value());
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
      // TODO: for now, just string and floating-point numbers are being
      // printed. Implement validation to check the literal type and print accordingly.
      printer.output_.append(literal.str_num.value_or(token::KW_NULO));
    }

    void operator()(const Unary &unary) {
      printer.output_.append("(");

      if (!unary.opr.lexeme.has_value()) {
        std::cerr << "Expected a unary expression operator." << std::endl;
        exit(EXIT_FAILURE);
      }

      printer.output_.append(unary.opr.lexeme.value());
      printer.output_.append(" ");
      printer.print(*unary.right);
      printer.output_.append(")");
    }
  };
  ExprVisitor visitor{.printer = *this};
  std::visit(visitor, expression.var);

  return output_;
}
