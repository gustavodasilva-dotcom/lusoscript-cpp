#include "lusoscript/interpreter.hh"
#include "lusoscript/scanner.hh"

void Interpreter::process(ErrorState *error_state, std::string source) {
	Scanner scanner(std::move(source), error_state);
	scanner.scanTokens();
}
