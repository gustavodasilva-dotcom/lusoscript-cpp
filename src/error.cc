#include <iostream>

#include "lusoscript/error.hh"

ErrorState::ErrorState() : had_error_(false) {}

void ErrorState::error(int line, std::string message) {
	report(line, "", message);
	setHadError();
}

void ErrorState::report(int line, std::string where, std::string message) {
	std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
}

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
