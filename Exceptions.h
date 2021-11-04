#ifndef NFEXCEPTION_H
#define NFEXCEPTION_H
#include <stdexcept>
using std::exception;  //±ðÍüÁË¼Óusing£¡
/**
 * Class that defines Exceptions.<br>
 * If the format of a number string constant contains non-digit character(s),the exception will throw.
 *
 */

class NumberFormatException : public exception {
public:
    const char *what() const throw() {
        return "Wrong Character.";
    }
};
class DivByZeroException : public exception {
	const char *what() const throw() {
		return "Divided by zero.";
	}
};
#endif // NFEXCEPTION_H
