#ifndef BIGINT_H
#define BIGINT_H
#include <cstring>
#include <string>
#include <algorithm>
#include <cmath>
#include "Exceptions.h"
#define NUM_GROUP_SIZE 4
#define MOD_BASE 10000
using std::string;

/**
 * It is a big integer class in C++.<br>
 * Since the C++ standard library has no big integer classes,I wrote this IMMATURE Biginteger class which
 * can complete simple arithmetics including add(+),subtract(-),multiply(*),and division(/)(Not ready).<br>
 * Though this class is far behind Biginteger classes in other High level programming language such as Java,(for instance,
 * it does NOT support negative numbers and bit operations temporarily)I still hope to learn something(such as the C++ language,algorithm and
 * <s>English</s>)from it.<br>
 * All comments and suggestions beneficial to this project are welcome.
 * \note <ol><li>This class provides an internal attribute(which is a pointer to <em>int</em>) to store the big integer.To increase the efficiency,
 * the big integer will be divided into groups every four digits(to ensure that the mid-result of multiply won't exceed the range of <em>int</em>.)</li>
 * <li>In *complexity* of all functions, <em>n</em> represents the length of *this big integer*,and *m* represents the length of another one.
 * \version 1.0 beta1(1.0.211112)
 */
class Biginteger {
  private:
    int *data;
    int eff_len,sign;
    //sign == 0:0；-1:负数；1：正数；2：无效，说明此尚未被正确构造，仍是空值
    inline bool check(const char *s,int len) {
        for(int i = 1; i < len; i++) {
            if(!(s[i] >= '0'&&s[i] <= '9'))    return false;
        }
        //第一个是负号或数字，如果是负号，则长度必须大于一。后面的必须全部是数字。
        if(s[0] == '-')	   return len != 1;
        else	return s[0] >= '0'&&s[0] <= '9';
    }
    /*删除前导0*/
    inline int firNoneZero(const char *s,int len) {
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
    inline void Add_lefts(const Biginteger &a,Biginteger &res,bool &carry,int minlen) {
        for(int i = minlen; i < a.eff_len; i++) {
            int now = a.data[i] + carry;
            res.data[res.eff_len++] = now % MOD_BASE;
            carry = (bool)(now / MOD_BASE);
        }
    }
    /*求长度*/
    inline void make_length(Biginteger &a) {
        int highest = a.data[a.eff_len - 1];
		if(a.eff_len == 1&&highest == 0)    a.length = 1;
        else    a.length = (a.eff_len - 1) * NUM_GROUP_SIZE + (int)log10(highest) + 1;
    }
    /*处理减法。在此处提供统一接口是因为结果可正可负*/
    Biginteger general_subt(const Biginteger &large,const Biginteger &small) {
        bool tw = 0;  //退位
        Biginteger ret(large.length);
        for(int i = 0; i < small.eff_len; i++) {
            int now = large.data[i] - small.data[i] - tw;
            if(now < 0) {
                tw = 1;
                now = MOD_BASE - abs(now);
            }
            else    tw = 0;
            ret.data[ret.eff_len++] = now % MOD_BASE;
        }//处理后面几位
        for(int i = small.eff_len; i < large.eff_len; i++) {
            int now = large.data[i] - tw;
            if(now < 0) {
                tw = 1;
                now = MOD_BASE - abs(now);
            }
            else    tw = 0;
            ret.data[ret.eff_len++] = now % MOD_BASE;
        }
        //前导零的特殊处理。注意a==b的特殊情况
        //注意：由于上面统一用位数较多数的位数初始化，所以此处可能不知要删1个
        while(ret.data[ret.eff_len - 1] == 0&&ret.eff_len > 1)
            ret.eff_len--;
        make_length(ret);
        return ret;
    }
  public:
    ///The length of the big integer.
    int length; //长度
    /**
     * Construct an empty big integer with the length of 0.
     */
    Biginteger():data(NULL),eff_len(0),sign(2),length(0) {}
    /**
     * Construct a big integer with a string constant
     * \param s the string constant to be constructed.
     * \throw if the string constant **s** is not a legal integer,throws NumberFormatException.
     */
    Biginteger(const char *s) {
        if(s == NULL)    throw NullException();
        int len = strlen(s);
        if(!check(s,len))    throw NumberFormatException();
        int loc = firNoneZero(s,len);
        length = len - loc;
        eff_len = ceil(length / (double)NUM_GROUP_SIZE);
        data = new int[eff_len]();
        sign = (s[0] == '-') ? -1 : 1;
        //0的特殊处理
        if(strcmp(s,"0") == 0||strcmp(s,"-0") == 0) {
            data[0] = 0;
            sign = 0;
        }
        //注 长度不是原来的字符串长度了
        else    convert(s + loc,length);
    }
    /**
      * Construct a big integer with a string constant.
      * Note that the parameter *std::string* will be converted to a C-style string constant.
      * \param str the string constant to be constructed.
      * \throw if the string constant **s** is not a legal integer,throws NumberFormatException.
      */
    Biginteger(string str) {
        char *S = new char[str.length() + 1];
        strcpy(S,str.c_str());
        new(this) Biginteger(S);
        if(S != NULL)	   delete S;
    }
    /**
     * Construct an empty big integer with a specific length.
     * \param len length of the integer.
     */
    Biginteger(int len):eff_len(0),sign(2),length(0) {
        data = new int[(int)ceil(len / (double)NUM_GROUP_SIZE)]();
    }
    /**
      * Construct a big integer with another big integer(Copy constructor).
      */
      //拷贝构造函数
    Biginteger(const Biginteger &another) {
        eff_len = another.eff_len;
        length = another.length;
        sign = another.sign;
        data = new int[another.eff_len]();
        for(int i = 0; i < eff_len; i++)    data[i] = another.data[i];
    }
    Biginteger &operator=(const Biginteger &another) {
        if (this != &another) {  //加上判断，防止自己赋给自己
            if (data)    delete data;  //如果原来有数据，则释放数据
            new(this) Biginteger(another);
        }
        return *this;
    }
    /*比较大小。注意从高位向低位比。*/
    /**
     * Compare the big integer to another one.
     * \param a the big integer to compare.
     * \return 1 if this big integer is larger than **a**,0 if equal,and -1 if smaller.
     */
    inline int compare(const Biginteger &a) {
        if(sign == 2||a.sign == 2)    throw NullException();
        int alen = a.eff_len;
        //一个为负，一个不为负
        if(sign == -1&&a.sign != -1)    return -1;
        if(sign == 1&&a.sign != 1)      return 1;
        if(sign == 0&&a.sign != 0)      return (a.sign == 1) ? -1 : 1;
        int ret = 0;
        if(eff_len > a.eff_len)         ret = 1;
        else if(eff_len < a.eff_len)    ret = -1;
        //进入此分支，说明符号相同且位数相同
        else {
            for(int i = alen-1; i >= 0; i--) {
                if(data[i] > a.data[i]) {
                    ret = 1;
                    break;
                }
                else if(data[i] < a.data[i]) {
                    ret = -1;
                    break;
                }
            }
        }
        if(sign == -1)    ret = -ret;
        return ret;
    }
    inline bool operator<(const Biginteger &a)  {return compare(a) == -1;}
    inline bool operator>(const Biginteger &a)  {return compare(a) == 1;}
    inline bool operator>=(const Biginteger &a) {return compare(a) >= 0;}
	inline bool operator<=(const Biginteger &a) {return compare(a) <= 0;}
    inline bool operator==(const Biginteger &a) {return compare(a) == 0;}
    inline bool operator!=(const Biginteger &a) {return compare(a) != 0;}
    /**
     * Find the opposite number of the big integer.
     * \return the result.
     */
    Biginteger Negate() {
        if(sign == 2)    throw NullException();
        Biginteger ret = *this;
        ret.sign = -ret.sign;
        return ret;
    }
    /** Find the absolute value of the big integer.
     * \return the result.
     */
     Biginteger absolute() {
        if(sign == 2)    throw NullException();
        if(sign != -1)   return *this;
        return Negate();
     }
    /**
     * Add the big integer with another one.Using **operator+** has the same effect.<br>
     * **Complexity**:O(max(n,m)/k).
     * \param a another big integer.
     * \return the result.<br>
     */
    Biginteger Add(const Biginteger &a) {
        if(sign == 2||a.sign == 2)    throw NullException();
        //(1,-1)
        if(sign == 1&&a.sign == -1) {
            Biginteger add2 = a;
            add2.sign = 1;
            return Subt(add2);
        }
        //(-1,1)
        if(sign == -1&&a.sign == 1) {
            Biginteger add1 = *this,add2 = a;
            add1.sign = 1;
            return add2.Subt(add1);
        }
		int minlen = std::min(eff_len,a.eff_len);
		bool carry = 0;  //进位标记
		Biginteger ret(std::max(length,a.length) + 1);  //待返回结果
		for(int i = 0; i < minlen; i++) {
            int now = data[i] + a.data[i] + carry;
            ret.data[ret.eff_len++] = now % MOD_BASE;
            carry = (bool)(now / MOD_BASE);
		}
		if(minlen == eff_len)    Add_lefts(a,ret,carry,minlen);
		else    Add_lefts(*this,ret,carry,minlen);
		//处理多余进位
		if(carry != 0)   ret.data[ret.eff_len++] = 1;
        //(0,0)
        if(sign == 0&&a.sign == 0)    ret.sign = 0;
        //(1,0),(1,1),(0,1)
        else if(sign == 1||a.sign == 1)     ret.sign = 1;
        //(-1,0),(-1,-1),(0,-1)
        else if(sign == -1||a.sign == -1)   ret.sign = -1;
        make_length(ret);
		return ret;
    }/*注意：拷贝构造函数的引用必须为const引用，因为返回值（拷贝）的函数的结果不能用于参数为普通引用类型的参数*/
    Biginteger operator+(const Biginteger &a) {return Add(a);}
    /**
     * Subtract another number from the number.Using **operator-** has the same effect.<br>
     * **Complexity**:O(max(n,m)/k).
     * \param a another big integer.
     * \return the result.<br>
     */
    Biginteger Subt(const Biginteger &a) {
        if(sign == 2||a.sign == 2)    throw NullException();
        Biginteger suber = *this,subee = a;
        //(1,0),(-1,0),(0,0)
        if(subee.sign == 0)        return suber;
        //(0,1),(0,-1)
        else if(suber.sign == 0)   return subee.Negate();
        //(1,-1)
        if(suber.sign == 1&&subee.sign == -1) {
            subee.sign = 1;
            return suber.Add(subee);
        }
        //(-1,1)
        if(suber.sign == -1&&subee.sign == 1) {
            suber.sign = 1;
            return suber.Add(subee).Negate();
        }
        //(-1,-1)
        if(subee.sign == -1&&suber.sign == -1) {
            suber.sign = subee.sign = 1;
            std::swap(suber,subee);
        }
        int tmp = suber.compare(subee);
        if(tmp == -1) {
            std::swap(suber,subee);
        }
        Biginteger ret(general_subt(suber,subee));
        ret.sign = tmp;
        return ret;
    }
    Biginteger operator-(const Biginteger &a) {return Subt(a);}
    /**
     * Multiply this large integer by another large integer(may have a little bugs).<br>
     * **Complexity**:O(nm/k).
     * \param a another big integer.
     * \return the result.
     */
    Biginteger Multiply(const Biginteger &a) {
        if(sign == 2||a.sign == 2)    throw NullException();
        int aLen = a.eff_len;
        Biginteger ret(length + a.length);  //存放结果
        for(int i = 0; i < eff_len; i++) {  //本数
            int carry = 0;
            for(int j = 0; j < aLen; j++) {  //a
                int now = data[i] * a.data[j] + carry + ret.data[i + j];
                carry = now / MOD_BASE;
                ret.data[i + j]=now % MOD_BASE;
            }
            if(carry != 0)    ret.data[i + aLen] = carry;
        }
        ret.sign = (sign == a.sign) ? 1 : -1;
        if(sign == 0||a.sign == 0)    ret.sign = 0;
        ret.eff_len = (int)ceil((length + a.length) / (double)NUM_GROUP_SIZE);  //?
        //最高位为0时，有效位数要-1，注意如果本来就是1就不用减
        while(ret.data[ret.eff_len - 1] == 0&&ret.eff_len != 1)
            ret.eff_len--;
		make_length(ret);
        return ret;
    }
    Biginteger operator*(const Biginteger &a) {return Multiply(a);}
    /**
     * Convert the big integer to a string constant.
     * \return the string after converting.
     */
    string toString() {
        if(sign == 2)   throw NullException();
    	if(sign == 0)   return "0";
    	string ret = "";
		int now = 0;
		for(int i = 0; i < eff_len - 1; i++) {
			int now = data[i];
			for(int j = 0; j < NUM_GROUP_SIZE; j++) {
				ret.append(1,(now % 10) + '0');
				now /= 10;
			}
		}
		now = abs(data[eff_len - 1]);
		//处理最高位
		do {
			ret.append(1,(now % 10)+'0');
			now /= 10;
		} while(now > 0);
		//处理减法结果为负数的情况
		if(sign == -1)    ret += "-";
		std::reverse(ret.begin(),ret.end());
		return ret;
    }
    ~Biginteger() {
		if(data != NULL) {
    		delete []data;
    		data = NULL;
		}
	}
};
#endif // BIGINT_H
