#ifndef BIGINT_H
#define BIGINT_H
#include <stdio.h>
#include <cstring>
#include <string>
#include <algorithm>
#include <cstddef>
#include "Exceptions.h"

/**
 * It is a big integer class in C++.<br>
 * Since the C++ standard library has no big integer classes,I wrote this IMMATURE Biginteger class which
 * can complete simple arithmetics including add(+),subtract(-),multiply(*),division(/) and so on.<br>
 * Though this class is far behind Biginteger classes in other high level programming languages such as Java,
 * I still hope to learn something(such as the C++ language,algorithm and <s>English</s>)from it.<br>
 * All comments and suggestions beneficial to this project are welcome.
 * \note <ol><li>This class provides an internal attribute(which is a pointer to <em>int</em>) to store the big integer.
 * To increase the efficiency,the big integer will be divided into groups every eight digits.</li>
 * <li>In *complexity* of all functions:if there are no special notes,<em>n</em> represents the length of *this,and *m* represents
 * the length of another one, and *k=8*, and (a/b)==ceil(a/b)</li>
 * <li>The Biginteger object is not mutable.
 * \version 2.0beta (2.0.230718)
 */
class Biginteger {
  private:
    using string = std::string;
    static constexpr int NUM_GROUP_SIZE = 8,BASE = (int)1e8,KARA_THRESHOLD = 128;
    static const Biginteger SHARED_ZERO,SHARED_ONE;
    int *data;
    //sign == 0:0；-1:负数；1：正数；2：无效，说明此尚未被正确构造，仍是空值
    int eff_len,sign,length = -1;
    //构造有效长度为effLen位的Biginteger
    explicit Biginteger(int effLen):eff_len(0),sign(2) {
        data = new int[effLen]();
    }
    inline static int check(const char *s,int len) {
        if(s[0] == '-'&&len == 1)
            return 0;
        if(s[0] != '-'&& !(s[0] >= '0'&& s[0] <= '9'))
            return 0;
        for(int i = 1; i < len; i++) {
            if(!(s[i] >= '0'&&s[i] <= '9'))
                return i;
        }
        return -1;
    }
    /*删除前导0*/
    inline static int firNoneZero(const char *s,int len) {
        int i;
        for(i = (s[0] == '-') ? 1 : 0; i < len - 1&&s[i] == '0'; i++);
        return i;
    }
    /*转换为int数组，方便乘法与加法运算*/
    inline void convert(const char *s,int len) {
        int k = 0,mini = (s[0] == '-') ? 1 : 0;
        for(int i = len-1; i >= mini; i -= NUM_GROUP_SIZE) {
            int now = 0;
            for(int j = i - (NUM_GROUP_SIZE - 1); j <= i; j++) {
            	if(j < mini)    continue;
                now = now * 10 + (s[j] - '0');
            }
            data[k++] = now;
        }
    }
    /*处理加法剩下的位数。a为位数较多的数*/
    inline void addLefts(const Biginteger &a, int8_t &carry, int minlen) {
        for(int i = minlen; i < a.eff_len; i++) {
            int now = a.data[i] + carry;
            data[eff_len++] = now % BASE;
            carry = (int8_t)(now / BASE);
        }
    }
    inline void removeZero() {
        while(data[eff_len - 1] == 0&&eff_len > 1)
            eff_len--;
    }
    /*处理减法*/
    Biginteger subtWith(const Biginteger &small) const;
    /*大数与int相乘，用于除法*/
    Biginteger mulWithInt(int a);
    //获取数位
    Biginteger getBits(int st,int len,bool needZeros = false) const;
    Biginteger getLower(int len) const {
    	return getBits(0,len);
    }
    Biginteger getUpper(int len) const {
        return getBits(eff_len - len,len);
    }
    Biginteger addZero(unsigned int n) const;
    Biginteger classicMul(const Biginteger &a) const;
    //Karatsuba乘法
    Biginteger karatsuba(const Biginteger &a) const;
    Biginteger divByTwo() const;
  public:
    /**
     * Construct an empty big integer with the length of 0.
     */
    Biginteger():data(nullptr),eff_len(0),sign(2) {}
    /**
     * Construct a big integer with a C-style string constant
     * \param s the string constant to be constructed.
     * \throw if the string constant **s** is not a legal integer,throws NumberFormatException.
     */
    Biginteger(const char *s);
    /**
      * Construct a big integer with a std::string constant.
      * \param str  the string constant to be constructed.
      * \throw if  the string constant **s** is not a legal integer,throws NumberFormatException.
      */
    Biginteger(const string &str) {
        new(this) Biginteger(str.c_str());
    }
    /**
      * Construct a big integer with another big integer(Copy constructor).
      */
      //拷贝构造函数
    Biginteger(const Biginteger &another);
    //移动构造函数
    Biginteger(Biginteger &&another);
    Biginteger &operator=(const Biginteger &another) {
        if (this != &another) {
            if (data != nullptr)    delete [] data;
            new(this) Biginteger(another);
        }
        return *this;
    }

    Biginteger &operator=(Biginteger &&another) noexcept {
        if (this != &another) {
            if(data != nullptr)    delete [] data;
            eff_len = another.eff_len;
            sign = another.sign;
            data = another.data;
            another.data = nullptr;
        }
        return *this;
    }
    /*比较大小。注意从高位向低位比。*/
    /**
     * Compare the big integer to another one.
     * \param a the big integer to compare.
     * \return 1 if this big integer is larger than **a**,0 if equal,and -1 if smaller.
     */
    int compareTo(const Biginteger &a) const;
    inline bool operator< (const Biginteger &a) const  {return compareTo(a) < 0;}
    inline bool operator> (const Biginteger &a) const  {return compareTo(a) > 0;}
    inline bool operator>= (const Biginteger &a) const {return compareTo(a) >= 0;}
	inline bool operator<= (const Biginteger &a) const {return compareTo(a) <= 0;}
    inline bool operator== (const Biginteger &a) const {return compareTo(a) == 0;}
    inline bool operator!= (const Biginteger &a) const {return compareTo(a) != 0;}
    /**
     * Get the length of the big integer.
     * \return the length of the big integer.
     */
    int getLength();
    /**
     * Find the opposite number of the big integer.
     * \return the result(-(*this)).
     */
    Biginteger negate() const{
        if(sign == 2)    throw NullException();
        Biginteger ret = *this;
        ret.sign = -ret.sign;
        return ret;
    }
    /** Find the absolute value of the big integer.
     * \return the result(|*this|).
     */
     Biginteger absolute() const{
        if(sign == 2)    throw NullException();
        if(sign >= 0)   return *this;
        Biginteger ret = *this;
        ret.sign = 1;
        return ret;
     }
    /**
     * Add the integer with another one.Using **operator+** has the same effect.<br>
     * **Complexity**:O(max(n,m) / k).
     * \param a another big integer.
     * \return the result((*this) + a).<br>
     */
    Biginteger add(const Biginteger &a) const;
    Biginteger operator+ (const Biginteger &a) const {return add(a);}
    Biginteger &operator+= (const Biginteger &a) {return (*this) = add(a);}
    /**
     * Subtract another integer from the integer.Using **operator-** has the same effect.<br>
     * **Complexity**:O(max(n,m) / k).
     * \param a another big integer.
     * \return the result((*this) - a).<br>
     */
    Biginteger subt(Biginteger subee) const;
    Biginteger operator- (const Biginteger &a) const {return subt(a);}
    Biginteger &operator-= (const Biginteger &a) {return (*this) = subt(a);}
    /**
     * Multiply this integer by another one.<br>
     * **Complexity**:
     * <p>The multiplication uses two algorithms</p>
     * <ol><li>If the length of the arrays storing both factors are greater than KARA_THRESHOLD, the Karatsuba multiplication is used.
     * In this condition,the complexity is around O(n^(1.58)),with <em>n</em>represents the length of the bigger factor.</li>
     * <li>Otherwise,the classic multiplication which behaves alike vertical multiplication you learned in the elementary school is used.
     * In this condition,the complexity is around O(nm / k)
     * \param a another big integer.
     * \return the result((*this) * a).
     */
    Biginteger multiply(const Biginteger &a) const;
    Biginteger operator* (const Biginteger &a) const {return multiply(a);}
    Biginteger &operator*= (const Biginteger &a) {return (*this) = multiply(a);}
    /**
     * Divide the integer by another one.Using <strong>operator/ </strong>has the same effect.<br>
     * **Complexity**:Up to O(m(n - m) / k)
     * \param a the divisor.
     * \return the result((*this) / a).
     * \throws <em>DivByZeroException</em> if the divisor is zero.
     */
    Biginteger divide(const Biginteger &a) const;
    Biginteger operator/ (const Biginteger &a) const {return divide(a);}
    Biginteger &operator/= (const Biginteger &a) {return (*this) = divide(a);}
    /**
     * Find the remainder of this integer to another one.Using **operator%** has the same effect.
     * \param a the divisor.
     * \return the result((*this) mod a).
     * \throws <em>DivByZeroException</em> if the divisor is zero.
     * \note <em>Experimental</em>.
     * <br>If dividend is negative,the result will be negative.
     * <br>In other cases,the result will be positive.
     */
    Biginteger mod(const Biginteger &a) const {
        Biginteger quot = divide(a);
        return subt(quot.multiply(a));
    }
    Biginteger operator% (const Biginteger &a) const {return mod(a);}
    Biginteger &operator%= (const Biginteger &a) {return (*this) = mod(a);}
    /**
     * Convert the big integer to an std::string object.
     * \return the string after converting.
     */
    string toString() const;
    ~Biginteger() {
        delete []data;
        data = nullptr;
	}
};
#endif // BIGINT_H
