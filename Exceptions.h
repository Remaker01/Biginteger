#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <stdexcept>
using std::exception;  //±ðÍüÁË¼Óusing£¡
/**
 * Throws if the string constant contains any non-digit character(s)
 */

class NumberFormatException : public exception {
public:
    const char *what() const throw() {
        return "Wrong Character.";
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
