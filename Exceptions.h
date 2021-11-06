#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <stdexcept>
using std::exception;  //±ðÍüÁË¼Óusing£¡
/**
 * Throw If the format of any number string constant contains non-digit character(s)
 */

class NumberFormatException : public exception {
public:
    const char *what() const throw() {
        return "Wrong Character.";
    }
};
/**
 * Throw if the divisor is zero.
 */
class DivByZeroException : public exception {
public:
	const char *what() const throw() {
		return "Divided by zero.";
	}
};
#endif // EXCEPTIONS_H
