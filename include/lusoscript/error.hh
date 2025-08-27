#ifndef LUSOSCRIPT_ERROR_H
#define LUSOSCRIPT_ERROR_H

#include <string>

class ErrorState {
public:
	bool getHadError();
	void setHadError();
	void resetHadError();
private:
	bool had_error_ = false;
};

void error(ErrorState *state, int line, std::string message);

#endif
