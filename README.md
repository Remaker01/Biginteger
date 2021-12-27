## Biginteger<br>
<p>一个用C++写的用于学习的Biginteger类。</p>
<p>本人是中国江苏某三流大学计算机专业的学生，受到Java中BigInteger类的启发，自己用C++ 写了一个大整数类，目前功能还很不完善，只支持加、减、乘、比较大小而且暂不支持负数。</p>
<p>关于具体使用方法，请参见docs/html，这是用doxygen自动生成的帮助文档。</p>
<p>欢迎提出任何有益的意见和建议。</p>
<small>~~为了锻炼英语能力，当初写文档时都是用英文写的，现在早就懒得改了，希望见谅哈~~</small>
<p>-------------------------------------------------------------------------------------------------------------------------------</p>
<p>使用方法举例：</p>
```cpp
#include <iostream>
#include <string>
using std::cin;
using std::cout;
int main() {
    string s1,s2;
    cin >> s1 >> s2;
    Biginteger b1(s1),b2(s2.c_str());
    try {
        cout << (b1 + b2).toString() << '\n';
        cout << (b1 - b2).toString() << '\n';
        cout << (b1 * b2).toString() << '\n';
        cout << (b1 / b2).toString() << '\n';
        cout << (b1.absolute() % b2.absolute()).toString() << '\n';
    } catch(NumberFormatException &e) {
        cout << e.what();
    } catch(DivByZeroException &e) {
        cout << e.what();
    }
    return 0;
}
```
<p>开发环境：</p>
<p>IDE:CodeBlocks 20.03 </p>
<p>编译器:G++(MinGW 8.1.0) </p>
<p>已测试平台:Windows 10 x64,Intel CPU</p>
<p>[临时个人博客地址](https://blog.csdn.net/m0_51303687?spm=1011.2124.3001.5343)</p>

<p>--------------------------------------------------------------------------------------------------------------------------------------------------------</p>
A BigInteger class written in C + + for learning.<br>
I am a computer major student of an ordinary university in Jiangsu, China. Inspired by the BigInteger class in Java, I have written this Biginteger class in C + +.<br>
At present, It is not perfect and only support addition, subtraction, multiplication and comparison.Negative numbers is NOT supported temporarily.<br>
For specific usage, please refer to docs / html, which is a help document automatically generated by Doxygen.<br>
<em>All useful comments and suggestions are welcome.</em>
<p>-------------------------------------------------------------------------------------------------------------------------------</p>
<p>Usage examples: see codes above.</p>

<p>Development Environment:</p>
<p>IDE: CodeBlocks 20.03 </p>
<p>Compiler: G + + (MinGW 8.1.0)</p>
<p>Tested platforms: Windows 10 x64, Intel CPU</p>
<p>[Temporary Personal Blog Address](https://blog.csdn.net/m0_51303687?spm=1011.2124.3001.5343)</p>
