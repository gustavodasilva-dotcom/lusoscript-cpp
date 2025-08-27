#ifndef LUSOSCRIPT_ERROR_H
#define LUSOSCRIPT_ERROR_H

#include <string>

class ErrorState {
public:
	explicit ErrorState();

	void error(int line, std::string message);
	bool getHadError();
	void resetHadError();
private:
	bool had_error_;

	void report(int line, std::string where, std::string message);
	void setHadError();
};

#endif
