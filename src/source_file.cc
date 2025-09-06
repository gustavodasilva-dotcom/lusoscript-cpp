#include "lusoscript/source_file.hh"

#include <sysexits.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "lusoscript/driver.hh"
#include "lusoscript/state.hh"

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

  state::AppState app_state{.mode = state::RunningMode::SourceFile,
                            .source = std::move(file_content),
                            .error = error::ErrorState{}};

  Driver driver;
  driver.process(&app_state);

  if (app_state.error.getHadError()) {
    app_state.error.summary();
    exit(EX_DATAERR);
  }

  if (app_state.error.getHadRuntimeError()) {
    exit(EX_SOFTWARE);
  }
}
