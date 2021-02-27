#include "stdafx.h"

#ifndef CCHINESECODE_H
#define CCHINESECODE_H
#include <string>
#include <vector>
using namespace std;

class CChineseCode
{
   public:
	   static void GB2312ToUTF_8(std::string& pOut,char *pText);//GB2312 转为 UTF-8
	   static void GB2312ToUTF_8(std::string& pOut,const char *pText);//GB2312 转为 UTF-8
	   static void UTF_8ToGB2312(std::string &pOut, const char *pText);//UTF-8 转为 GB2312
#ifdef WIN32
	   static bool CChineseCode::IsGB2312(const char *pText);	//判断是否为gb2312,尚不完善,请不要使用
	   static int CChineseCode::IsTextUTF8(char* str,ULONGLONG length);      //判断是否为UTF8字符串.
#endif
private:
	#ifdef WIN32
	   static void UTF_8ToUnicode(wchar_t* pOut,const char *pText);  // 把UTF-8转换成Unicode
       static void UnicodeToUTF_8(char* pOut,wchar_t* pText);  //Unicode 转换成UTF-8
       static void UnicodeToGB2312(char* pOut,wchar_t uData);  // 把Unicode 转换成 GB2312 
       static void Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer);// GB2312 转换成　Unicode
	#else
	   static int CodeConvert(const char *from_charset,const char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen);// GB2312 转换成　Unicode
	#endif
};

#endif
