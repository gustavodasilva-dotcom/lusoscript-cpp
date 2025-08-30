#include "lusoscript/source_file.hh"

#include <sysexits.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "lusoscript/error.hh"
#include "lusoscript/interpreter.hh"

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

  error::ErrorState error_state;

  Interpreter interpreter;
  interpreter.process(&error_state, std::move(file_content));

  if (error_state.getHadError()) {
    error_state.summary();
    exit(EX_DATAERR);
  }
}
