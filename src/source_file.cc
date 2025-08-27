#include "lusoscript/interpreter.hh"
#include "lusoscript/source_file.hh"

#include <filesystem>
#include <iostream>
#include <fstream>

void SourceFile::run(std::string file_path) {
	std::filesystem::path path = file_path;

	if (!std::filesystem::exists(path)) {
		std::cerr << "File not found " << path << "." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!path.has_extension()) {
		std::cerr << "Invalid file." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (path.extension() != ".luso") {
		std::cerr << "Invalid LusoScript file." << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string file_content;
	{
		std::stringstream contents_stream;
		std::fstream file_stream(path, std::ios::in);
		contents_stream << file_stream.rdbuf();
		file_content = contents_stream.str();
	}

	Interpreter interpreter;
	interpreter.process(std::move(file_content));
}
