#include <iostream>

#include "lusoscript/core.hh"
#include "lusoscript/source_file.hh"
#include "lusoscript/prompt.hh"

int main(int argc, char* argv[]) {
	if (argc == 2) {
		SourceFile source_file;
		source_file.run(argv[1]);

		return EXIT_SUCCESS;
	} else if (argc == 1) {
		Prompt prompt;
		prompt.run();

		return EXIT_SUCCESS;
	} else {
		std::cerr << "Usage: luso [script]" << std::endl;
		exit(EXIT_USAGE);
	}
}
