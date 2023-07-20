#include "Bigint.h"
#define _PUBLIC_
#define _PRIVATE_
using std::string;
const Biginteger Biginteger::SHARED_ZERO = Biginteger("0");
const Biginteger Biginteger::SHARED_ONE = Biginteger("1");
_PRIVATE_
Biginteger Biginteger::subtWith(const Biginteger &small) const {
    int8_t carry = 0;  //退位标志
    Biginteger ret(eff_len);
    for(int i = 0; i < small.eff_len; i++) {
        int now = data[i] - small.data[i] - carry;
        if(now < 0) {
            carry = 1;
            now = BASE + now;
        } else    carry = 0;
        ret.data[ret.eff_len++] = now;
    }
    for(int i = small.eff_len; i < eff_len; i++) {
        int now = data[i] - carry;
        if(now < 0) {
            carry = 1;
            now = BASE + now;
        } else    carry = 0;
        ret.data[ret.eff_len++] = now;
    }
    //删去多余的前导0
    ret.removeZero();
    return ret;
}

Biginteger Biginteger::mulWithInt(int a) {
    if(a == 0||sign == 0)    return SHARED_ZERO;
    int carry = 0;
    Biginteger ret(eff_len + 2);
    if(a > 0)
        ret.sign = sign;
    else {
        ret.sign = -sign;
        a = -a;
    }
    ret.eff_len = eff_len;
    for (int i = 0; i < eff_len; i++) {
        int64_t now = (int64_t)data[i] * a + carry;
        ret.data[i] = now % BASE;
        carry = now / BASE;
    }
    if (carry != 0) {
        ret.data[ret.eff_len++] = carry;
    }
    return ret;
}

Biginteger Biginteger::getBits(int st,int len,bool needZeros) const {
    if(len <= 0)
        return SHARED_ZERO;
    if(st + len > eff_len)
        return *this;
    Biginteger ret;
    ret.data = new int[len];
    ret.eff_len = len;
    ret.sign = sign;
    std::copy(data + st,data + st + len,ret.data);
    if (!needZeros)
        ret.removeZero();
    return ret;
}

Biginteger Biginteger::addZero(unsigned int n) const {
    int len = eff_len + n;
    Biginteger ret;
    ret.data = new int[len];
    ret.eff_len = len;
    ret.sign = sign;
    //[n,eff_len-1+n]
    for(int i = eff_len - 1; i >= 0; i--) {
        ret.data[i + n] = data[i];
    }
    std::fill(ret.data,ret.data+n,0);
    return ret;
}

Biginteger Biginteger::classicMul(const Biginteger &a) const {
    int aLen = a.eff_len;
    Biginteger ret(eff_len + aLen);

    ret.eff_len = eff_len + aLen - 1;
    for(int i = 0; i < eff_len; i++) {  //this
        int carry = 0;
        for(int j = 0; j < aLen; j++) {  //a
            int64_t now = (int64_t)data[i] * a.data[j] + carry + ret.data[i + j];
            carry = (int)(now / BASE);
            ret.data[i + j]=now % BASE;
        }
        if(carry != 0)    ret.data[i + aLen] = carry;
    }
    if(ret.data[eff_len + aLen - 1] != 0)
        ret.eff_len++;
    ret.removeZero();
    return ret;
}

Biginteger Biginteger::karatsuba(const Biginteger &a) const {
    int half = (std::max(eff_len,a.eff_len) + 1) / 2;
    Biginteger xh = getUpper(eff_len - half),xl = getLower(half);  //划分为两部分
    Biginteger yh = a.getUpper(a.eff_len - half),yl = a.getLower(half); //划分为两部分
    //pl=xl * yl  ph=xh * yh
    Biginteger pl = xl.multiply(yl);
    Biginteger ph = xh.multiply(yh);
    //res= pl + ph * BASE^(half * 2) + ((xl + xh)(yl + yh) - pl - ph) * BASE(half)
    return pl + ph.addZero(half * 2) + ((xl + xh).multiply(yl + yh) - pl - ph).addZero(half);
}

Biginteger Biginteger::divByTwo() const {
    Biginteger ret(*this);
    bool carry = 0; //进位标志
    for(int i = eff_len - 1; i >= 0; i--) {
        int now = data[i] >> 1;
        if(carry) {
            now = (now + (BASE / 2)) % BASE;
        }
        ret.data[i] = now;
        carry = data[i] & 1;
    }
    ret.removeZero();
    return ret;
}
_PUBLIC_
Biginteger::Biginteger(const char *s) {
    if(s == nullptr)
        throw NullException();
    int len = strlen(s),chkres=check(s,len);
    if(len == 0)
        throw NullException();
    if(chkres >= 0)
        throw NumberFormatException(s[chkres]);
    int loc = firNoneZero(s,len);
    eff_len = (len - loc + NUM_GROUP_SIZE -1) / NUM_GROUP_SIZE;
    data = new int[eff_len];
    sign = (s[0] == '-') ? -1 : 1;
    //0的特殊处理
    if(strcmp(s + loc,"0") == 0||strcmp(s + loc,"-0") == 0) {
        data[0] = 0;
        sign = 0;
    }
    //注 长度不是原来的字符串长度了
    else
        convert(s + loc,len - loc);
}

Biginteger::Biginteger(const Biginteger &another) {
    eff_len = another.eff_len;
    sign = another.sign;
    data = new int[another.eff_len];
    std::copy(another.data,another.data + another.eff_len,data);
}

Biginteger::Biginteger(Biginteger &&another)  {
    eff_len = another.eff_len;
    sign = another.sign;
    data = another.data;
    another.data = nullptr;
}

int Biginteger::compareTo(const Biginteger &a) const {
    if(sign == 2||a.sign == 2)
        throw NullException();
    int alen = a.eff_len;
    //一个为负，一个不为负
    if(sign < 0&&a.sign >= 0)
        return -1;
    if(sign > 0&&a.sign <= 0)
        return 1;
    if(sign == 0&&a.sign != 0)
        return -a.sign;
    int ret = 0;
    if(eff_len > a.eff_len)
        ret = 1;
    else if(eff_len < a.eff_len)
        ret = -1;
    //进入此分支，说明符号相同且位数相同
    else {
        for(int i = alen-1; i >= 0; i--) {
            if(data[i] > a.data[i]) {
                ret = 1;
                break;
            } else if(data[i] < a.data[i]) {
                ret = -1;
                break;
            }
        }
    }
    if(sign < 0)
        ret = -ret;
    return ret;
}

int Biginteger::getLength() {
    if(length >= 0)
        return length;
    int highest = data[eff_len - 1];
    if(sign == 0)
        return 1;
    int lg = 0;
    while (highest > 0) {
        highest /= 10;
        lg++;
    }
    return length = ((eff_len - 1) * NUM_GROUP_SIZE + lg);
}

Biginteger Biginteger::add(const Biginteger &a) const {
    if(sign == 2||a.sign == 2)
        throw NullException();
    //(1,-1)
    if(sign > 0&&a.sign < 0) {
        Biginteger add2 = a;
        add2.sign = 1;
        return subt(add2);
    }
    //(-1,1)
    if(sign < 0&&a.sign > 0) {
        Biginteger add1 = *this;
        add1.sign = 1;
        return a.subt(add1);
    }
    int minlen = std::min(eff_len,a.eff_len);
    int8_t carry = 0;  //进位
    Biginteger ret(std::max(eff_len,a.eff_len) + 1);  //长度
    for(int i = 0; i < minlen; i++) {
        int now = data[i] + a.data[i] + carry;
        ret.data[ret.eff_len++] = now % BASE;
        carry = (int8_t)(now / BASE);
    }
    if(minlen == eff_len)
        ret.addLefts(a,carry,minlen);
    else
        ret.addLefts(*this,carry,minlen);
    //注意最高位可能有进位
    if(carry != 0)
        ret.data[ret.eff_len++] = 1;
    //(0,0)
    if(sign == 0&&a.sign == 0)
        ret.sign = 0;
    //(1,0),(1,1),(0,1)
    else if(sign > 0||a.sign > 0)
        ret.sign = 1;
    //(-1,0),(-1,-1),(0,-1)
    else if(sign < 0||a.sign < 0)
        ret.sign = -1;
    return ret;
}

Biginteger Biginteger::subt(Biginteger subee) const {
    if(sign == 2||subee.sign == 2)
        throw NullException();
    Biginteger suber = *this;
    //(1,0),(-1,0),(0,0)
    if(subee.sign == 0)
        return suber;
    //(0,1),(0,-1)
    else if(suber.sign == 0)
        return subee.negate();
    //(1,-1)
    if(suber.sign > 0&&subee.sign < 0) {
        subee.sign = 1;
        return suber.add(subee);
    }
    //(-1,1)
    if(suber.sign < 0&&subee.sign > 0) {
        suber.sign = 1;
        return suber.add(subee).negate();
    }
    //(-1,-1)
    if(subee.sign < 0&&suber.sign < 0) {
        suber.sign = subee.sign = 1;
        std::swap(suber,subee);
    }
    int tmp = suber.compareTo(subee);
    Biginteger ret = (tmp < 0) ? subee.subtWith(suber) : suber.subtWith(subee);
    ret.sign = tmp;
    return ret;
}

Biginteger Biginteger::multiply(const Biginteger &a) const {
    if(sign == 2||a.sign == 2)
        throw NullException();
    if(sign == 0||a.sign == 0)
        return SHARED_ZERO;
    Biginteger ret;
    if(eff_len > KARA_THRESHOLD&&a.eff_len > KARA_THRESHOLD)
        ret = karatsuba(a);
    else
        ret = classicMul(a);
    ret.sign = (sign == a.sign) ? 1 : -1;
    return ret;
}

Biginteger Biginteger::divide(const Biginteger &a) const {
    if(sign == 2||a.sign == 2)
        throw NullException();
    //除数为0
    if(a.sign == 0)
        throw DivByZeroException();
    Biginteger divided(absolute()),divisor(a.absolute());
    //被除数更小
    if(divisor > divided)
        return SHARED_ZERO;
    if(divisor == SHARED_ONE)
        return (sign == a.sign) ? *this : negate();
    if(divisor == "2") {
        Biginteger ret = divided.divByTwo();
        return (sign == a.sign) ? ret : ret.negate();
    }
    int highest = divisor.data[divisor.eff_len - 1];
    int d = BASE / (highest + 1);
    //调整除数，下面使用Knuth除法
    divided = divided.mulWithInt(d);
    divisor = divisor.mulWithInt(d);
    //从第i位开始向高位取aLen+1位。循环每执行一次，相当于进行一次试商（这里是估商）
    int aLen = divisor.eff_len,len = divided.eff_len;
    highest = divisor.data[aLen - 1];
    Biginteger ret(len - aLen + 1);
    if(len == aLen) {
        ret.eff_len = 1;
        ret.data[0] = 1;
    }
    for(int i = len - aLen - 1; i >= 0; i--) {
        //从第i位开始向高位取aLen+1位。循环每执行一次，相当于进行一次试商（这里是估商）
        Biginteger part = divided.getBits(i,1 + aLen,true);
        //估计的商，一开始估为上界
        int q = (int)(((int64_t)part.data[aLen] * BASE + part.data[aLen - 1]) / highest);
        //注意取完后再去前导零，否则产生不可预见的后果
        part.removeZero();
        //当前余数
        Biginteger rmder = part.subt(divisor.mulWithInt(q));
        //调整。不用Knuth除法就是将下面改成二分查找
        while(rmder.sign < 0) {
            rmder = std::move(rmder.add(divisor));
            q--;
        }
        int j,k;
        //拷贝数组，注意清0
        for (j = i,k = 0; k < rmder.eff_len; j++, k++)
            divided.data[j] = rmder.data[k];
        for(; j < i + aLen; j++)
            divided.data[j] = 0;
        ret.data[i] = q;
        //printf("%d ",q / BASE);
        if(i == len - aLen - 1) { //只有最高位可能出现商>=基数的情况
            ret.data[i] %= BASE;
            ret.data[i+1] += q / BASE;    //处理最高位的特殊情况，这里必须用q不能用ret.date[i]
        }
    }
    ret.eff_len = len - aLen;
    if(ret.data[len-aLen] > 0)
        ret.eff_len++;
    ret.sign = (sign == a.sign) ? 1 : -1;
    return ret;
}

string Biginteger::toString() const {
    if(sign == 2)
        throw NullException();
    if(sign == 0)
        return "0";
    string ret;
    ret.reserve(eff_len * NUM_GROUP_SIZE);
    int now;

    for(int i = 0; i < eff_len - 1; i++) {
        now = data[i];
        for(int j = 0; j < NUM_GROUP_SIZE; j++) {
            ret += (char)((now % 10) + '0');
            now /= 10;
        }
    }
    now = data[eff_len - 1];
    //处理最高位
    do {
        ret += (char)((now % 10) + '0');
//        ret.append(1,(now % 10)+'0');
        now /= 10;
    } while(now > 0);
    if(sign < 0)
        ret += '-';
    std::reverse(ret.begin(),ret.end());
    return ret;
}
#undef _PUBLIC_
#undef _PRIVATE_
