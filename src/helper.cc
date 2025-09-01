#include "lusoscript/helper.hh"

namespace helper {
bool endsWith(const std::string &str, const std::string &suffix) {
  if (str.length() < suffix.length()) return false;
  return str.substr(str.length() - suffix.length()) == suffix;
}
}  // namespace helper
