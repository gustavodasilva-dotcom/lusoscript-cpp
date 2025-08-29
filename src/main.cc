#include <sysexits.h>

#include <iostream>

#include "lusoscript/repl.hh"
#include "lusoscript/source_file.hh"

int main(int argc, char* argv[]) {
  if (argc == 2) {
    SourceFile source_file;
    source_file.run(argv[1]);

    return EXIT_SUCCESS;
  } else if (argc == 1) {
    Repl repl;
    repl.run();

    return EXIT_SUCCESS;
  } else {
    std::cerr << "Usage: luso [script]" << std::endl;
    exit(EX_USAGE);
  }
}
