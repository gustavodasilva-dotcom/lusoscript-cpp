#include "lusoscript/interpreter.hh"
#include "lusoscript/prompt.hh"

#include <iostream>

void Prompt::run() {
	Interpreter interpreter;

	std::string input;

	std::cout << "> ";

	while (std::getline(std::cin, input)) {
		std::cout << input << std::endl;

		interpreter.process(std::move(input));

		std::cout << "> ";
	}
}
