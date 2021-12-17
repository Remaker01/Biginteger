#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <stdexcept>
#include <cstdio>
using std::exception;
/**
 * Throws if the string constant contains any non-digit character(s)
 */

class NumberFormatException : public exception {
	char *info;
public:
	NumberFormatException() {
		info = new char[18]();
		sprintf(info,"Wrong Character.");
	}
	NumberFormatException(char ch) {
		info = new char[20]();
		sprintf(info,"Wrong Character: %c",ch);
	}
    const char *what() const throw() {
        return info;
    }
};

/**
 * Throws if the divisor is zero.
 */
class DivByZeroException : public exception {
public:
	const char *what() const throw() {
		return "Divided by zero.";
	}
};

/**
 * Throws if the number is empty(NOT equals 0)
 */
class NullException : public exception {
public:
	const char *what() const throw() {
		return "The number is empty(NOT 0),you must assign a value to it.";
	}
};
#endif // EXCEPTIONS_H
