#include "lusoscript/interpreter.hh"

#include <iostream>

#include "lusoscript/arena.hh"
#include "lusoscript/lexer.hh"
#include "lusoscript/parser.hh"

void Interpreter::process(error::ErrorState *error_state, std::string source) {
  Lexer lexer(std::move(source), error_state);
  std::vector<token::Token> tokens = lexer.scanTokens();

  arena::Arena allocator(1024 * 1024 * 4);

  Parser parser(&allocator, error_state, tokens);
  ast::Expr expression = parser.parse();

  if (error_state->getHadError()) return;

  ast::AstPrinter printer;
  std::cout << printer.print(std::move(expression)) << std::endl;
}
