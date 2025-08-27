#include <iostream>

#include "lusoscript/error.hh"

bool ErrorState::getHadError() {
	return had_error_;
}

void ErrorState::setHadError() {
	if (!had_error_)
		had_error_ = true;
}

void ErrorState::resetHadError() {
	had_error_ = false;
}

void report(int line, std::string where, std::string message) {
	std::cerr << "[line" << line << "] Error" << where << ": " << message << std::endl;
}

void error(ErrorState *state, int line, std::string message) {
	report(line, "", message);
	state->setHadError();
}
