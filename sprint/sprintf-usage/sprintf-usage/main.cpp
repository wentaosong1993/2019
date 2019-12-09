#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
/*<
sprintf()
方法的作用，定义字符串输入格式
>*/
int main()
{
	char buffer[50];
	int n, a = 5, b = 3;
	n = sprintf(buffer, "%d plus %d is %d", a, b, a + b);
	printf("[%s] is a string %d chars long\n", buffer, n);
	return 0;
}