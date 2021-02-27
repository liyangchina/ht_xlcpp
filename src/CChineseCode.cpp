#include "stdafx.h"
#include <vector>
#include <string>
#include "string.h"
#include <sstream>
#include "CChineseCode.h"
#include <stdio.h>
#include <ctime>
#ifdef WIN32
	#include <windows.h>
#else
	#include <iconv.h>
#endif
#include <algorithm>  
#include <functional>
#include <time.h>
#include<locale> 
#include<cstdlib> 
 
using namespace std;
#ifdef _LY_CRT
using namespace System;
#endif
using std::vector;

//------------------------------
void CChineseCode::GB2312ToUTF_8(std::string& pOut,const char *pText)
{
	CChineseCode::GB2312ToUTF_8(pOut,(char *)pText);
}

void CChineseCode::GB2312ToUTF_8(std::string& pOut,char *pText)
{
#ifdef WIN32   
	if(pText[0]==-17)
	{
		pOut=pText;
		return;
	}
	size_t pLen=strlen(pText);
    char buf[4];
    size_t nLength = pLen* 3;
    char* rst = new char[nLength];
    
    memset(buf,0,4);
    memset(rst,0,nLength);
    
    size_t i = 0;
    size_t j = 0;      
    while(i < pLen)
    {
            //如果是英文直接复制就可以
            if( *(pText + i) >= 0)
            {
                    rst[j++] = pText[i++];
            }
            else
            {
                    wchar_t pbuffer;
                    Gb2312ToUnicode(&pbuffer,pText+i);
                    
                    UnicodeToUTF_8(buf,&pbuffer);
                    
                    unsigned short int tmp = 0;
                    tmp = rst[j] = buf[0];
                    tmp = rst[j+1] = buf[1];
                    tmp = rst[j+2] = buf[2];    
                    
                    j += 3;
                    i += 2;
            }
    }
 
    //返回结果
    pOut = rst;             
    delete []rst;   
	size_t ipos;
	ipos=pOut.find("encoding=\"gb2312\"");
	if(ipos==-1)
		ipos=pOut.find("encoding=\"GB2312\"");
	if(ipos!=-1)
		//pOut.erase(ipos,strlen("encoding=\"gb2312\""));
		pOut.replace(ipos,strlen("encoding=\"gb2312\""),"encoding=\"UTF-8\"");
    return;
#else
	
	int enlarg = 3;	
	int inLen = strlen(pText);
	int outLen = inLen;
    char * outBuf = new char[outLen*enlarg + 1];

	//调用code_convert
	CodeConvert("utf-8","gb2312",pText,inLen,outBuf,outLen);
	outBuf[outLen] = '\0';
	pOut=outBuf;

	//结束
    delete []outBuf;

	//补充,针对xml头,应该转入xml的专用转换
	/*size_t ipos;
	ipos=pOut.find("encoding=\"gb2312\"");
	if(ipos==-1)
		ipos=pOut.find("encoding=\"GB2312\"");
	if(ipos!=-1 && ipos <20)
		pOut.replace(ipos,strlen("encoding=\"gb2312\""),"encoding=\"UTF-8\"");
    return;*/

	return;
#endif
	
}

void CChineseCode::UTF_8ToGB2312(std::string &pOut, const char *pText)
{
#ifdef WIN32   

	size_t pLen = strlen(pText);
    char * newBuf = new char[pLen + 1];
    char Ctemp[4];
    memset(Ctemp,0,4);

    size_t i =0;
    size_t j = 0;
    
    while(i < pLen)
    {
        if(pText[i] > 0)
        {
                newBuf[j++] = pText[i++];                       
        }
        else                 
        {
                WCHAR Wtemp;
                UTF_8ToUnicode(&Wtemp,pText + i);
        
                UnicodeToGB2312(Ctemp,Wtemp);

				if(Ctemp[0]!=0x3F){//如果Unicode内容GB2312无法识别,则为0x3F-?
					newBuf[j] = Ctemp[0];
					newBuf[j + 1] = Ctemp[1];
				}else{
					newBuf[j] = '?';
					newBuf[j + 1] = '?';
				}

                i += 3;    
                j += 2;   
        }
    }
    newBuf[j] = '\0';
    
    pOut = newBuf;
    delete []newBuf;
    
    return; 
#else
	int enlarg = 1;	
	int inLen = strlen(pText);	
	int outLen = inLen;
    char * outBuf = new char[outLen*enlarg + 1];

	//调用code_convert
	CodeConvert("utf-8","gb2312",(char *)pText,inLen,outBuf,outLen);
	outBuf[outLen] = '\0';
	pOut=outBuf;

	//结束
	delete []outBuf;

	return;
#endif

}


//for linux using iconv.h 中的转换函数进行转换
#ifdef WIN32
void CChineseCode::UTF_8ToUnicode(wchar_t* pOut,const char *pText)
{
    char* uchar = (char *)pOut;

    uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
    uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

    return;
}

void CChineseCode::UnicodeToUTF_8(char* pOut,wchar_t* pText)
{
    // 注意 WCHAR高低字的顺序,低字节在前，高字节在后
    char* pchar = (char *)pText;

    pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
    pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
    pOut[2] = (0x80 | (pchar[0] & 0x3F));

    return;
}

void CChineseCode::UnicodeToGB2312(char* pOut,wchar_t uData)
{
    WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(wchar_t),NULL,NULL);
    return;
}     

void CChineseCode::Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer)
{
    MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
    return ;
}
  
#else
int CChineseCode::CodeConvert(const char *from_charset,const char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen) {
	#ifndef OPEN_WRT
	   iconv_t cd; 
	   //int rc;
	   size_t inlen_tmp=inlen;
	   char **pin = &inbuf; 

	   char **pout = &outbuf; 

	   cd = iconv_open(to_charset,from_charset); 

	   if (cd==0) 
		   return -1; 

	   memset(outbuf,0,outlen); 

	   iconv(cd,pin,&inlen_tmp,pout,(size_t *)&outlen);
	   //if (iconv(cd,pin,(unsigned int *)&inlen,pout,(unsigned int *)&outlen)==(unsigned int)-1)
		   //return -1;

	   iconv_close(cd); 
	#endif
   return 0; 

} 

#endif
#ifdef WIN32
int CChineseCode::IsTextUTF8(char* str,ULONGLONG length)
{
	int i;
	DWORD nBytes=0;//UFT8可用1-6个字节编码,ASCII用一个字节
	UCHAR chr;
	BOOL bAllAscii=TRUE; //如果全部都是ASCII, 说明不是UTF-8
	for(i=0;i<length;i++)
	{
	  chr= *(str+i);
	  if( (chr&0x80) != 0 )  // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
		  bAllAscii= FALSE;
	  if(nBytes==0) //如果不是ASCII码,应该是多字节符,计算字节数
	  {
		  if(chr>=0x80)
		  {
			  if(chr>=0xFC&&chr<=0xFD)
				  nBytes=6;
			  else if(chr>=0xF8)
				  nBytes=5;
			  else if(chr>=0xF0)
				  nBytes=4;
			  else if(chr>=0xE0)
				  nBytes=3;
			  else if(chr>=0xC0)
				  nBytes=2;
			  else
			  {
				  return FALSE;
			  }
			  nBytes--;
		  }
	  }
	  else //多字节符的非首字节,应为 10xxxxxx
	  {
		  if( (chr&0xC0) != 0x80 )
		  {
			  return FALSE;
		  }
		  nBytes--;
	  }
  }
  if( nBytes > 0 ) //违返规则
  {
	  return FALSE;
  }
  if( bAllAscii ) //如果全部都是ASCII, 说明不是UTF-8
  {
	  return FALSE;
  }
  return TRUE;
} 
#endif


////尚未测试
//char*  STRING_Helper::strTrim(char* s){
//	char * beg=s;
//	char * end=s+strlen(s);
//	for(;;){
//		beg ++;
//		if ((beg)!=' ') 
//			break;
//	}
//	for(;;){
//		if ((end --)!=' ') 
//			break;
//	}
//	end++;
//	end=0;
//	return beg;
//}

