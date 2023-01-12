#include <cstdio>
#include <ctime>
#include "Bigint.h"
const int M=2e5+1;
using namespace std;
char s1[M],s2[M];
int main() {
	FILE *in=fopen("E:/in.txt","r"),*out=fopen("E:/out1.txt","w");
	fscanf(in,"%s",s1);
	fgetc(in);
	fscanf(in,"%s",s2);
	long st=clock();
    Biginteger b1(s1),b2(s2);
    Biginteger res(b1/b2);
    printf("%ld",clock()-st);
    fputs(res.toString().c_str(),out);
    fclose(in);
    fclose(out);
	return 0;
}
