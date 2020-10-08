#ifndef XREGEX_H
#define XREGEX_H
/*
返回utf-8码占据的字节数
如果出错返回0
*/
int utf8ToInt(unsigned char *c, int *result);
/*
函数不会分配内存空间
确保传入的result参数指向有效的地址
*/
int intToUtf8(int number, char *result);
#endif