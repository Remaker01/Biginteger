#ifndef BIGINT_H
#define BIGINT_H
#include <cstring>
#include <string>
#include <algorithm>
#include <cmath>
#include "Exceptions.h"
using std::string;
static const int NUM_GROUP_SIZE = 8,MOD_BASE = (int)1e8,KARA_THRESHOLD = 128;
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
 * the length of another one, and *k=8*.
 * \version 1.10 (1.10.211226)
 */
class Biginteger {
  private:
    int *data;
    //sign == 0:0；-1:负数；1：正数；2：无效，说明此尚未被正确构造，仍是空值
    int eff_len,sign;
    //构造有效长度为effLen位的Biginteger
    Biginteger(int effLen):eff_len(0),sign(2) {
    	data = new int[effLen]();
	}
    inline int check(const char *s,int len) {
        for(int i = 1; i < len; i++) {
            if(!(s[i] >= '0'&&s[i] <= '9'))    return i;
        }
        //第一个是负号或数字，如果是负号，则长度必须大于一。后面的必须全部是数字。
        if(s[0] == '-')	   return (len != 1) ? -1 : 0;
        else	return (s[0] >= '0'&&s[0] <= '9') ? -1 : 0;
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
    inline void addLefts(const Biginteger &a,bool &carry,int minlen) {
        for(int i = minlen; i < a.eff_len; i++) {
            int now = a.data[i] + carry;
            data[eff_len++] = now % MOD_BASE;
            carry = (bool)(now / MOD_BASE);
        }
    }
    inline void removeZero() {
        while(data[eff_len - 1] == 0&&eff_len > 1)
            eff_len--;
    }
    /*处理减法。在此处提供统一接口是因为结果可正可负*/
    Biginteger subt(const Biginteger &large,const Biginteger &small) const {
        bool carry = 0;  //退位
        Biginteger ret(large.eff_len);
        for(int i = 0; i < small.eff_len; i++) {
            int now = large.data[i] - small.data[i] - carry;
            if(now < 0) {
                carry = 1;
                now = MOD_BASE + now;
            }
            else    carry = 0;
            ret.data[ret.eff_len++] = now;
        }//处理后面几位
        for(int i = small.eff_len; i < large.eff_len; i++) {
            int now = large.data[i] - carry;
            if(now < 0) {
                carry = 1;
                now = MOD_BASE + now;
            }
            else    carry = 0;
            ret.data[ret.eff_len++] = now;
        }
        //前导零的特殊处理。注意a==b的特殊情况
        //注意：由于上面统一用位数较多数的位数初始化，所以此处可能不知要删1个
        ret.removeZero();      
        return ret;
    }
    //除以2，方便除法时使用二分。
    Biginteger divByTwo() const {
        Biginteger ret(*this);
        bool carry = 0; //进位指示器
        for(int i = eff_len - 1; i >= 0; i--) {
            int now = data[i] >> 1;
            if(carry) {
                now = (now + (MOD_BASE >> 1)) % MOD_BASE;
            }
            ret.data[i] = now;
            carry = data[i] & 1;
        }
        ret.removeZero();
        return ret;
    }
      //取得低位 
    Biginteger getLower(int len) const {
    	if(len <= 0)	return Biginteger("0");
        if(len >= eff_len)    return *this;
        Biginteger ret(len);
        ret.eff_len = len;
        ret.sign = 1;
        for(int i = 0; i < len; i++) {
            ret.data[i] = data[i];
        }
        return ret; 
    }
	//取得高位，自动补0
    Biginteger getUpper(int len) const {
    	if(len <= 0)	return Biginteger("0");
        if(len >= eff_len)    return *this;
        Biginteger ret(len);
        ret.eff_len = len;
        ret.sign = 1;
        int dif = eff_len - len;
        for(int i = 0; i < len; i++) {
            ret.data[i] = data[i + dif];
        }
        ret.removeZero();
        return ret;
    }
    Biginteger addZero(unsigned int n) const {
        int len = eff_len + n;
        Biginteger ret(len);
        ret.eff_len = len;
        ret.sign = sign;
        for(int i = eff_len - 1; i >= 0; i--) {
            ret.data[i + n] = data[i];
        }
        return ret;
    }
    Biginteger classicMul(const Biginteger &a) const {
    	int aLen = a.eff_len;
        Biginteger ret(eff_len + aLen);  //存放结果
        
		ret.eff_len = eff_len + aLen - 1;
        for(int i = 0; i < eff_len; i++) {  //this
            int carry = 0;
            for(int j = 0; j < aLen; j++) {  //a
                long long now = (long long)data[i] * a.data[j] + carry + ret.data[i + j];
                carry = now / MOD_BASE;
                ret.data[i + j]=now % MOD_BASE;
            }
            if(carry != 0)    ret.data[i + aLen] = carry;
        }
        if(ret.data[eff_len + aLen - 1] != 0)
        	ret.eff_len++;
        ret.removeZero();
        return ret;
    }
    //Karatsuba乘法
    Biginteger Karatsuba(const Biginteger &a) const {
        int half = (std::max(eff_len,a.eff_len) + 1) / 2;
        Biginteger xh = getUpper(eff_len - half),xl = getLower(half);  //第一个因数 
        Biginteger yh = a.getUpper(a.eff_len - half),yl = a.getLower(half); //第二个因数
        //pl=xl * yl  ph=xh * yh
        Biginteger pl = xl.Multiply(yl);
		Biginteger ph = xh.Multiply(yh);
		//res= pl + ph * MOD_BASE^(half * 2) + ((xl + xh)(yl + yh) - pl - ph) * MOD_BASE(half)
        return pl + ph.addZero(half * 2) + ((xl + xh).Multiply(yl + yh) - pl - ph).addZero(half);
    }
  public:
    /**
     * Construct an empty big integer with the length of 0.
     */
    Biginteger():data(NULL),eff_len(0),sign(2) {}
    /**
     * Construct a big integer with a string constant
     * \param s the string constant to be constructed.
     * \throw if the string constant **s** is not a legal integer,throws NumberFormatException.
     */
    Biginteger(const char *s) {
        if(s == NULL)    throw NullException();
        int len = strlen(s),chkres=check(s,len);
        if(chkres != -1)    throw NumberFormatException(s[chkres]);
        int loc = firNoneZero(s,len);
        eff_len = (len - loc + NUM_GROUP_SIZE -1) / NUM_GROUP_SIZE;
        data = new int[eff_len]();
        sign = (s[0] == '-') ? -1 : 1;
        //0的特殊处理
        if(strcmp(s + loc,"0") == 0||strcmp(s + loc,"-0") == 0) {
            data[0] = 0;
            sign = 0;
        }
        //注 长度不是原来的字符串长度了
        else    convert(s + loc,len - loc);
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
      * Construct a big integer with another big integer(Copy constructor).
      */
      //拷贝构造函数
    Biginteger(const Biginteger &another) {
        eff_len = another.eff_len;
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
    inline int compare(const Biginteger &a) const {
        if(sign == 2||a.sign == 2)    throw NullException();
        int alen = a.eff_len;
        //一个为负，一个不为负
        if(sign == -1&&a.sign != -1)    return -1;
        if(sign == 1&&a.sign != 1)      return 1;
        if(sign == 0&&a.sign != 0)      return -a.sign;
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
    inline bool operator< (const Biginteger &a) const  {return compare(a) == -1;}
    inline bool operator> (const Biginteger &a) const  {return compare(a) == 1;}
    inline bool operator>= (const Biginteger &a) const {return compare(a) >= 0;}
	inline bool operator<= (const Biginteger &a) const {return compare(a) <= 0;}
    inline bool operator== (const Biginteger &a) const {return compare(a) == 0;}
    inline bool operator!= (const Biginteger &a) const {return compare(a) != 0;}
    /*求长度*/
    inline int getLength() const {
        int highest = data[eff_len - 1];
		if(eff_len == 1&&highest == 0)    return 1;
        int lg = 0;
        while (highest > 0) {
            highest /= 10;
            lg++;
        }
        return (eff_len - 1) * NUM_GROUP_SIZE + lg;
    }
    /**
     * Find the opposite number of the big integer.
     * \return the result(-(*this)).
     */
    Biginteger Negate() const{
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
        if(sign != -1)   return *this;
        return Negate();
     }
    /**
     * Add the integer with another one.Using **operator+** has the same effect.<br>
     * **Complexity**:O(max(n,m) / k).
     * \param a another big integer.
     * \return the result((*this) + a).<br>
     */
    Biginteger Add(const Biginteger &a) const {
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
		Biginteger ret(std::max(eff_len,a.eff_len) + 1);  //待返回结果
		for(int i = 0; i < minlen; i++) {
            int now = data[i] + a.data[i] + carry;
            ret.data[ret.eff_len++] = now % MOD_BASE;
            carry = (bool)(now / MOD_BASE);
		}
		if(minlen == eff_len)    ret.addLefts(a,carry,minlen);
		else    ret.addLefts(*this,carry,minlen);
		//处理多余进位
		if(carry != 0)   ret.data[ret.eff_len++] = 1;
        //(0,0)
        if(sign == 0&&a.sign == 0)    ret.sign = 0;
        //(1,0),(1,1),(0,1)
        else if(sign == 1||a.sign == 1)     ret.sign = 1;
        //(-1,0),(-1,-1),(0,-1)
        else if(sign == -1||a.sign == -1)   ret.sign = -1;
		return ret;
    }
    Biginteger operator+ (const Biginteger &a) const {return Add(a);}
    /**
     * Subtract another integer from the integer.Using **operator-** has the same effect.<br>
     * **Complexity**:O(max(n,m) / k).
     * \param a another big integer.
     * \return the result((*this) - a).<br>
     */
    Biginteger Subt(const Biginteger &a) const {
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
        Biginteger ret(subt(suber,subee));
        ret.sign = tmp;
        return ret;
    }
    Biginteger operator- (const Biginteger &a) const {return Subt(a);}
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
    Biginteger Multiply(const Biginteger &a) const {
        if(sign == 2||a.sign == 2)    throw NullException();
        if(sign == 0||a.sign == 0)    return Biginteger("0");
        Biginteger ret;
        if(eff_len > KARA_THRESHOLD&&a.eff_len > KARA_THRESHOLD)
            ret = Karatsuba(a);
        else    ret = classicMul(a);
        ret.sign = (sign == a.sign) ? 1 : -1;
        return ret;
    }
    Biginteger operator* (const Biginteger &a) const {return Multiply(a);}
    /**
     * Divide the integer by another one.Using <strong>operator/ </strong>has the same effect.
     * \param a the divisor.
     * \return the result((*this) / a).
     * \throws <em>DivByZeroException</em> if the divisor is zero.
     * \note <em>Experimental</em>.
     */
    Biginteger Divide(const Biginteger &a) const {
    	if(sign == 2||a.sign == 2)	   throw NullException();
        //除数为0
        if(a.sign == 0)    throw DivByZeroException();
		const Biginteger ONE("1"),Divided(absolute()),Divisor(a.absolute());
		Biginteger l(ONE),r(Divided.divByTwo()),mid;
        //被除数更小
        if(Divisor > Divided)      return Biginteger("0");
        if(Divisor == ONE)    l = Divided;
        else if(Divisor == ONE.Add(ONE))    l = r;
        else {
            while (l < r) {
                mid = (l.Add(r)).divByTwo();
                if(mid.Multiply(Divisor) > Divided)    r = mid;
                else	l = mid.Add(ONE);
            }
            l = l.Subt(ONE);
        }
        l.sign = (sign == a.sign) ? 1 : -1;
//        removeZero(l);
        return l;
    }
    Biginteger operator/ (const Biginteger &a) const {return Divide(a);}
    /**
     * Find the remainder of this integer to another one.Using **operator%** has the same effect.
     * \param a the divisor.
     * \return the result((*this) mod a).
     * \throws <em>DivByZeroException</em> if the divisor is zero.
     * \note <em>Experimental</em>.
     * <br>If dividend is negative,the result will be negative.
     * <br>In other cases,the result will be positive.
     */
    Biginteger Mod(const Biginteger &a) const {
        Biginteger quot = Divide(a);
        return Subt(quot.Multiply(a));
    }
    Biginteger operator% (const Biginteger &a) const {return Mod(a);}
    /**
     * Convert the big integer to a string constant.
     * \return the string after converting.
     */
    string toString() const {
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
		now = data[eff_len - 1];
		//处理最高位
		do {
			ret.append(1,(now % 10)+'0');
			now /= 10;
		} while(now > 0);
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
