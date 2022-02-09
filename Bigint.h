#ifndef BIGINT_H
#define BIGINT_H
#include <cstring>
#include <string>
#include <algorithm>
#include <cstddef>
#include "Exceptions.h"
using std::string;

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
 * \version 1.23 (1.23.220129)
 */
class Biginteger {
  private:
    static constexpr int NUM_GROUP_SIZE = 8,MOD_BASE = (int)1e8,KARA_THRESHOLD = 128;
    int *data;
    //sign == 0:0��-1:������1��������2����Ч��˵������δ����ȷ���죬���ǿ�ֵ
    int eff_len,sign;
    //������Ч����ΪeffLenλ��Biginteger
    explicit Biginteger(int effLen):eff_len(0),sign(2) {
        data = new int[effLen]();
    }
    inline static int check(const char *s,int len) {
        if(s[0] == '-'&&len == 1)
            return 0;
        if(s[0] != '-'&& !(s[0] >= '0'&& s[0] <= '9'))
            return 0;
        for(int i = 1; i < len; i++) {
            if(!(s[i] >= '0'&&s[i] <= '9'))    return i;
        }
        return -1;
    }
    /*ɾ��ǰ��0*/
    inline static int firNoneZero(const char *s,int len) {
        int i;
        for(i = (s[0] == '-') ? 1 : 0; i < len - 1&&s[i] == '0'; i++);
        return i;
    }
    /*ת��Ϊint���飬����˷���ӷ�����*/
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
    /*����ӷ�ʣ�µ�λ����aΪλ���϶����*/
    inline void addLefts(const Biginteger &a, int8_t carry, int minlen) {
        for(int i = minlen; i < a.eff_len; i++) {
            int now = a.data[i] + carry;
            data[eff_len++] = now % MOD_BASE;
            carry = (int8_t)(now / MOD_BASE);
        }
    }
    inline void removeZero() {
        while(data[eff_len - 1] == 0&&eff_len > 1)
            eff_len--;
    }
    /*����������ڴ˴��ṩͳһ�ӿ�����Ϊ��������ɸ�*/
    Biginteger subtWith(const Biginteger &small) const {
        int8_t carry = 0;  //��λ
        Biginteger ret(eff_len);
        for(int i = 0; i < small.eff_len; i++) {
            int now = data[i] - small.data[i] - carry;
            if(now < 0) {
                carry = 1;
                now = MOD_BASE + now;
            }
            else    carry = 0;
            ret.data[ret.eff_len++] = now;
        }//������漸λ
        for(int i = small.eff_len; i < eff_len; i++) {
            int now = data[i] - carry;
            if(now < 0) {
                carry = 1;
                now = MOD_BASE + now;
            }
            else    carry = 0;
            ret.data[ret.eff_len++] = now;
        }
        //ǰ��������⴦��ע��a==b���������
        //ע�⣺��������ͳһ��λ���϶�����λ����ʼ�������Դ˴����ܲ�֪Ҫɾ1��
        ret.removeZero();
        return ret;
    }
    /*������int��ˣ����ڳ���*/
    Biginteger mulWithInt(int a) {
        if(a == 0||sign == 0)    return Biginteger("0");
        int carry = 0;
        Biginteger ret(eff_len + 1);
        if(a >= 0)    ret.sign = sign;
		else {
			ret.sign = -sign;
			a = -a;
		}
		ret.eff_len = eff_len;
        for (int i = 0; i < eff_len; i++) {
            int64_t now = (int64_t)data[i] * a + carry;
            ret.data[i] = now % MOD_BASE;
            carry = now / MOD_BASE;
        }
        if (carry != 0) {
            ret.data[ret.eff_len++] = carry;
        }
        return ret;
    }
    //��ȡ��λ
    Biginteger getBits(int st,int len,bool needZeros = false) const {
        if(len <= 0)	return Biginteger("0");
        if(st + len > eff_len)    return *this;
        Biginteger ret(len);
        ret.eff_len = len;
        ret.sign = sign;
        for (int i = 0; i < len; i++) {
            ret.data[i] = data[i + st];
        }
        if (!needZeros)    ret.removeZero();
        return ret;
    }
    //ȡ�õ�λ
    Biginteger getLower(int len) const {
    	return getBits(0,len);
    }
	//ȡ�ø�λ���Զ���0
    Biginteger getUpper(int len) const {
        return getBits(eff_len - len,len);
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
        Biginteger ret(eff_len + aLen);  //��Ž��
        
		ret.eff_len = eff_len + aLen - 1;
        for(int i = 0; i < eff_len; i++) {  //this
            int carry = 0;
            for(int j = 0; j < aLen; j++) {  //a
                int64_t now = (int64_t)data[i] * a.data[j] + carry + ret.data[i + j];
                carry = (int)(now / MOD_BASE);
                ret.data[i + j]=now % MOD_BASE;
            }
            if(carry != 0)    ret.data[i + aLen] = carry;
        }
        if(ret.data[eff_len + aLen - 1] != 0)
        	ret.eff_len++;
        ret.removeZero();
        return ret;
    }
    //Karatsuba�˷�
    Biginteger Karatsuba(const Biginteger &a) const {
        int half = (std::max(eff_len,a.eff_len) + 1) / 2;
        Biginteger xh = getUpper(eff_len - half),xl = getLower(half);  //��һ������ 
        Biginteger yh = a.getUpper(a.eff_len - half),yl = a.getLower(half); //�ڶ�������
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
    Biginteger():data(nullptr),eff_len(0),sign(2) {}
    /**
     * Construct a big integer with a string constant
     * \param s the string constant to be constructed.
     * \throw if the string constant **s** is not a legal integer,throws NumberFormatException.
     */
    Biginteger(const char *s) {
        if(s == nullptr)    throw NullException();
        int len = strlen(s),chkres=check(s,len);
        if(len == 0)    throw NullException();
        if(chkres != -1)    throw NumberFormatException(s[chkres]);
        int loc = firNoneZero(s,len);
        eff_len = (len - loc + NUM_GROUP_SIZE -1) / NUM_GROUP_SIZE;
        data = new int[eff_len]();
        sign = (s[0] == '-') ? -1 : 1;
        //0�����⴦��
        if(strcmp(s + loc,"0") == 0||strcmp(s + loc,"-0") == 0) {
            data[0] = 0;
            sign = 0;
        }
        //ע ���Ȳ���ԭ�����ַ���������
        else    convert(s + loc,len - loc);
    }
    /**
      * Construct a big integer with a string constant.
      * Note that the parameter *std::string* will be converted to a C-style string constant.
      * \param str the string constant to be constructed.
      * \throw if the string constant **s** is not a legal integer,throws NumberFormatException.
      */
    Biginteger(const string &str) {
        new(this) Biginteger(str.c_str());
    }
    /**
      * Construct a big integer with another big integer(Copy constructor).
      */
      //�������캯��
    Biginteger(const Biginteger &another) {
        eff_len = another.eff_len;
        sign = another.sign;
        data = new int[another.eff_len]();
        std::copy(another.data,another.data + another.eff_len,data);
    }
    //�ƶ����캯��
    Biginteger(Biginteger &&another) {
        eff_len = another.eff_len;
        sign = another.sign;
        data = another.data;
        another.data = nullptr;  //Դָ������ÿ�
    }
    Biginteger &operator=(const Biginteger &another) {
        if (this != &another) {  //�����жϣ���ֹ�Լ������Լ�
            if (data != nullptr)    delete [] data;  //���ԭ�������ݣ����ͷ�����
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
    /*�Ƚϴ�С��ע��Ӹ�λ���λ�ȡ�*/
    /**
     * Compare the big integer to another one.
     * \param a the big integer to compare.
     * \return 1 if this big integer is larger than **a**,0 if equal,and -1 if smaller.
     */
    inline int compare(const Biginteger &a) const {
        if(sign == 2||a.sign == 2)    throw NullException();
        int alen = a.eff_len;
        //һ��Ϊ����һ����Ϊ��
        if(sign < 0&&a.sign >= 0)    return -1;
        if(sign > 0&&a.sign <= 0)      return 1;
        if(sign == 0&&a.sign != 0)      return -a.sign;
        int ret = 0;
        if(eff_len > a.eff_len)         ret = 1;
        else if(eff_len < a.eff_len)    ret = -1;
        //����˷�֧��˵��������ͬ��λ����ͬ
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
        if(sign < 0)    ret = -ret;
        return ret;
    }
    inline bool operator< (const Biginteger &a) const  {return compare(a) < 0;}
    inline bool operator> (const Biginteger &a) const  {return compare(a) > 0;}
    inline bool operator>= (const Biginteger &a) const {return compare(a) >= 0;}
	inline bool operator<= (const Biginteger &a) const {return compare(a) <= 0;}
    inline bool operator== (const Biginteger &a) const {return compare(a) == 0;}
    inline bool operator!= (const Biginteger &a) const {return compare(a) != 0;}
    /**
     * Get the length of the big integer.
     * \return the length of the big integer.
     */
    inline int getLength() const {
        int highest = data[eff_len - 1];
		if(sign == 0)    return 1;
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
        if(sign >= 0)   return *this;
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
        if(sign > 0&&a.sign < 0) {
            Biginteger add2 = a;
            add2.sign = 1;
            return Subt(add2);
        }
        //(-1,1)
        if(sign < 0&&a.sign > 0) {
            Biginteger add1 = *this;
            add1.sign = 1;
            return a.Subt(add1);
        }
		int minlen = std::min(eff_len,a.eff_len);
		int8_t carry = 0;  //��λ���
		Biginteger ret(std::max(eff_len,a.eff_len) + 1);  //�����ؽ��
		for(int i = 0; i < minlen; i++) {
            int now = data[i] + a.data[i] + carry;
            ret.data[ret.eff_len++] = now % MOD_BASE;
            carry = (int8_t)(now / MOD_BASE);
		}
		if(minlen == eff_len)    ret.addLefts(a,carry,minlen);
		else    ret.addLefts(*this,carry,minlen);
		//��������λ
		if(carry != 0)   ret.data[ret.eff_len++] = 1;
        //(0,0)
        if(sign == 0&&a.sign == 0)    ret.sign = 0;
        //(1,0),(1,1),(0,1)
        else if(sign > 0||a.sign > 0)     ret.sign = 1;
        //(-1,0),(-1,-1),(0,-1)
        else if(sign < 0||a.sign < 0)   ret.sign = -1;
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
        if(suber.sign > 0&&subee.sign < 0) {
            subee.sign = 1;
            return suber.Add(subee);
        }
        //(-1,1)
        if(suber.sign < 0&&subee.sign > 0) {
            suber.sign = 1;
            return suber.Add(subee).Negate();
        }
        //(-1,-1)
        if(subee.sign < 0&&suber.sign < 0) {
            suber.sign = subee.sign = 1;
            std::swap(suber,subee);
        }
        int tmp = suber.compare(subee);
        Biginteger ret = (tmp < 0) ? subee.subtWith(suber) : suber.subtWith(subee);
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
     * Divide the integer by another one.Using <strong>operator/ </strong>has the same effect.<br>
     * **Complexity**:Up to O(m(n - m) / k)
     * \param a the divisor.
     * \return the result((*this) / a).
     * \throws <em>DivByZeroException</em> if the divisor is zero.
     */
    Biginteger Divide(const Biginteger &a) const {
    	if(sign == 2||a.sign == 2)	   throw NullException();
        //����Ϊ0
        if(a.sign == 0)    throw DivByZeroException();
        Biginteger divided(absolute()),divisor(a.absolute());
        //��������С
        if(divisor > divided)      return Biginteger("0");
        int highest = divisor.data[divisor.eff_len - 1];
        int d = MOD_BASE / (highest + 1);
        //��������������ʹ��Knuth����
        divided = divided.mulWithInt(d);
        divisor = divisor.mulWithInt(d);
        //ע�ⳤ�ȿ��ܸı䡣����һ�о��Ե�����Ϊ׼
        int aLen = divisor.eff_len,len = divided.eff_len;
        highest = divisor.data[aLen - 1];
        Biginteger ret(len - aLen + 1);
        if(len == aLen) {
            ret.eff_len = 1;
            ret.data[0] = 1;
        }
        for(int i = len - aLen - 1; i >= 0; i--) {
            //�ӵ�iλ��ʼ���λȡaLen+1λ��ѭ��ÿִ��һ�Σ��൱�ڽ���һ�����̣������ǹ��̣�
            Biginteger part = divided.getBits(i,1 + aLen,true);
            //���Ƶ��̣�һ��ʼ��Ϊ�Ͻ�
            int q = ((int64_t)part.data[aLen] * MOD_BASE + part.data[aLen - 1]) / highest;
            //ע��ȡ�����ȥǰ���㣬�����������Ԥ���ĺ��
            part.removeZero();
            //��ǰ����
            Biginteger rmder = part.Subt(divisor.mulWithInt(q));
            //����������Knuth�������ǽ�����ĳɶ��ֲ���
            while(rmder.sign < 0) {
                rmder = rmder.Add(divisor);
                q--;
            }
            int j,k;
            //�������飬ע����0
            for (j = i,k = 0; k < rmder.eff_len; j++, k++)
                divided.data[j] = rmder.data[k];
            for(; j < i + aLen; j++)
                divided.data[j] = 0;
            ret.data[i] = q % MOD_BASE;
            ret.data[i+1] += q / MOD_BASE;    //�����Ĵ���
        }
        ret.eff_len = len - aLen;
        if(ret.data[len-aLen] > 0)    ret.eff_len++;
        ret.sign = (sign == a.sign) ? 1 : -1;
        return ret;
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
     * Convert the big integer to an std::string object.
     * \return the string after converting.
     */
    string toString() const {
        if(sign == 2)   throw NullException();
    	if(sign == 0)   return "0";
    	string ret;
        ret.reserve((eff_len + 1) * NUM_GROUP_SIZE);
		int now;

		for(int i = 0; i < eff_len - 1; i++) {
			now = data[i];
			for(int j = 0; j < NUM_GROUP_SIZE; j++) {
				ret.append(1,(now % 10) + '0');
				now /= 10;
			}
		}
		now = data[eff_len - 1];
		//�������λ
		do {
			ret.append(1,(now % 10)+'0');
			now /= 10;
		} while(now > 0);
		if(sign < 0)    ret += "-";
		std::reverse(ret.begin(),ret.end());
		return ret;
    }
    ~Biginteger() {
		if(data != nullptr) {
    		delete []data;
    		data = nullptr;
		}
	}
};
#endif // BIGINT_H
