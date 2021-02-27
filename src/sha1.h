//sha1.h：对字符串进行sha1加密
#ifndef _SHA1_H_
#define _SHA1_H_
#include "string.h"
#include <stdio.h>
#include <iostream>
using namespace std;

extern unsigned char *sha1(const unsigned char *msg,size_t size, unsigned char *result);

#endif
