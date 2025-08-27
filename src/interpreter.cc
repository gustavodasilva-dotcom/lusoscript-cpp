#include "lusoscript/interpreter.hh"
#include "lusoscript/scanner.hh"

void Interpreter::process(std::string source) {
	Scanner scanner;
	scanner.scanTokens();
}
