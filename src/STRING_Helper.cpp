#include "stdafx.h"
#include "string.h"
#include <vector>
#include <string>
#include <sstream>
#include "STRING_Helper.h"
#include <stdio.h>
#include <ctime>
#include <ctype.h>
#include "iostream"
//#include <windows.h>
#include <algorithm>
#include <functional>
#include <time.h>
#include "Linux_Win.h"

using namespace std;
#ifdef _LY_CRT
using namespace System;
#endif
using std::vector;

STRING_Helper::STRING_Helper(void)
{
}

STRING_Helper::~STRING_Helper(void)
{
}
//取得字符串中第一个[]对应组合,
//src 取得src 以\0结尾
int STRING_Helper::strVector_GetNext(const char *src, string &dest_str, char* &next, string para) {//ly 910
    char *find, *first_find=0, *start = (char *)src;
    dest_str = "";
    bool begin = true;
    int i = 0;
    
    for (;;) {
        //递进[]查找指针；
        if ((find = STRING_Helper::_strVector_TempStrFind(start, para.data())) != 0) {
            if (*find == para.at(0)) {
                i++;//如果发现[
                if (begin)
                    first_find = find;
            }
            else
                i--;//如果发现]
        }
        else if (begin) {
            //没有发现[]，原版字串内容
            dest_str.append(src);
            next = find;
            return 0;
        }
        else
            return -2;//ly change???
        begin = false;
        //完成[]截取
        if (i == 0) {//完整截取[]
            //发现了子字串。
            dest_str.append(first_find + 1, find - first_find - 1);
            next = find + 1;
            return 1;
        }
        if (i<0) {
            //差错情况
            return -1;
        }
        //递进查询
        start = find + 1;
    };
}

char *STRING_Helper::_strVector_TempStrFind(const char *src, const char *find) {
    size_t len = strlen(find);
    char *dest = 0;
    char *tmp;
    char find_str[2]; find_str[1] = 0;
    
    if (*src == '\0')
        return 0;
    for (size_t i = 0; i<len; i++) {
        find_str[0] = find[i];
        if ((tmp = (char *)strstr(src, (const char *)find_str)) != 0) {
            if (dest == 0 || tmp<dest)
                dest = tmp;
        };
    }
    return dest;
}

bool STRING_Helper::strArrayToBinary(string & dest,const char *src, int byte_width, string split_str){
    vector<string> dest_array;
    stringSplit(src,dest_array,split_str);
    
    dest="";
    BYTE a[32];
    string cell_str;
    for (size_t i=0; i<dest_array.size(); i++){
        cell_str=dest_array[i];
        strTrim(cell_str);//去除两边的空格，还包含OA,OD!!!
        //测试是否为数字
        if (!CheckNum(cell_str)){
            //cout << "CheckNum" << cell_str <<endl;
            return false;
        }
        //数字转换
        uint64 c=ParseInt(cell_str);
        switch(byte_width){
            case 1:
                a[0]=(BYTE)c;
                break;
            case 2:
                STRING_Helper::putUINT16(a,0,c);
                break;
            case 4:
                STRING_Helper::putUINT32(a,0,c);
                break;
            case 8:
                STRING_Helper::putUINT64(a,0,c);
                break;
            default:
                //cout << "1234" <<endl;
                return false;
        }
        dest.append((char *)a,byte_width);
    }
    return true;
}

void STRING_Helper::HTTP_UnEscape(const char * inputsEsc, std::string & strUnEsc) {
#ifdef WIN32
    cout << "unescape...";
    strUnEsc = "";
    int j = 0;
    char ctmp[5];
    char cbuf[128];
    wchar_t wcbuf[128];
    std::string strEsc;
    strEsc.insert(0, inputsEsc, strlen(inputsEsc));
    try {
        for (size_t i = 0; i<strEsc.length();)
        {
            if (strEsc.at(i) == '%')
            {
                i++;
                if (strEsc.at(i) == 'u')
                {
                    i++;
                    ctmp[0] = strEsc.at(i++);
                    ctmp[1] = strEsc.at(i++);
                    ctmp[2] = strEsc.at(i++);
                    ctmp[3] = strEsc.at(i++);
                    ctmp[4] = '\0';
                    wcbuf[0] = strtol(ctmp, NULL, 16);
                    WideCharToMultiByte(CP_ACP, 0, (PWSTR)wcbuf, -1, cbuf, 2, NULL, NULL);
                    cbuf[2] = '\0';
                    strUnEsc.append(cbuf);
                }
                else
                {
                    ctmp[0] = strEsc.at(i++);
                    ctmp[1] = strEsc.at(i++);
                    ctmp[2] = '\0';
                    
                    wcbuf[0] = strtol(ctmp, NULL, 16);
                    WideCharToMultiByte(CP_ACP, 0, (PWSTR)wcbuf, -1, cbuf, 2, NULL, NULL);
                    cbuf[1] = '\0';
                    strUnEsc.append(cbuf);
                }
            }
            else
            {
                strUnEsc.push_back(strEsc.at(i++));
            }
        }
    }
    catch (...)
    {
        cout << "error" << endl;
    }
#else
    strUnEsc = inputsEsc;
#endif
}

char *
STRING_Helper::HTTP_decode_string (char *path)
{
    // replace the percentcodes with the actual character
    int i, j;
    char percentcode[3];
    
    for (i = j = 0; path[i] != '\0'; i++, j++)
    {
        if (path[i] == '%')
        {
            percentcode[0] = path[++i];
            percentcode[1] = path[++i];
            percentcode[2] = '\0';
            path[j] = (char) strtol (percentcode, (char **) 0, 16);
        }
        else
            path[j] = path[i];
    }
    
    path[j] = path[i];
    
    return path;
}

bool STRING_Helper::HTTP_IsUTF8(char* str, UInt64 length)
{
    UInt64 i;
    unsigned long nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节
    unsigned char chr;
    bool bAllAscii = true; //如果全部都是ASCII, 说明不是UTF-8
    for (i = 0; i<length; i++)
    {
        chr = *(str + i);
        if ((chr & 0x80) != 0)  // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
            bAllAscii = false;
        if (nBytes == 0) //如果不是ASCII码,应该是多字节符,计算字节数
        {
            if (chr >= 0x80)
            {
                if (chr >= 0xFC && chr <= 0xFD)
                    nBytes = 6;
                else if (chr >= 0xF8)
                    nBytes = 5;
                else if (chr >= 0xF0)
                    nBytes = 4;
                else if (chr >= 0xE0)
                    nBytes = 3;
                else if (chr >= 0xC0)
                    nBytes = 2;
                else
                {
                    return false;
                }
                nBytes--;
            }
        }
        else //多字节符的非首字节,应为 10xxxxxx
        {
            if ((chr & 0xC0) != 0x80)
            {
                return false;
            }
            nBytes--;
        }
    }
    if (nBytes > 0) //违返规则
    {
        return false;
    }
    if (bAllAscii) //如果全部都是ASCII, 说明不是UTF-8
    {
        return false;
    }
    return true;
}

void STRING_Helper::UTF_8ToGB2312(std::string &pOut, const char *pText) {;//UTF-8 转为 GB2312
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
    CodeConvert("utf-8","gb2312",pText,inLen,outBuf,outLen);
    outBuf[outLen] = '\0';
    pOut=outBuf;
    
    //结束
    delete []outBuf;
    
    return;
#endif
}
void STRING_Helper::GB2312ToUTF_8(std::string &pOut, const char *pText) {
#ifdef WIN32
    if (pText[0] == -17)
    {
        pOut = pText;
        return;
    }
    size_t pLen = strlen(pText);
    char buf[4];
    size_t nLength = pLen * 3;
    char* rst = new char[nLength];
    
    memset(buf, 0, 4);
    memset(rst, 0, nLength);
    
    size_t i = 0;
    size_t j = 0;
    while (i < pLen)
    {
        //如果是英文直接复制就可以
        if (*(pText + i) >= 0)
        {
            rst[j++] = pText[i++];
        }
        else
        {
            wchar_t pbuffer;
            Gb2312ToUnicode(&pbuffer, (char *)pText + i);
            
            UnicodeToUTF_8(buf, &pbuffer);
            
            unsigned short int tmp = 0;
            tmp = rst[j] = buf[0];
            tmp = rst[j + 1] = buf[1];
            tmp = rst[j + 2] = buf[2];
            
            j += 3;
            i += 2;
        }
    }
    
    //返回结果
    pOut = rst;
    delete[]rst;
    size_t ipos;
    ipos = pOut.find("encoding=\"gb2312\"");
    if (ipos == -1)
        ipos = pOut.find("encoding=\"GB2312\"");
    if (ipos != -1)
        //pOut.erase(ipos,strlen("encoding=\"gb2312\""));
        pOut.replace(ipos, strlen("encoding=\"gb2312\""), "encoding=\"UTF-8\"");
    return;
#else
    
    int enlarg = 3;
    int inLen = strlen(pText);
    int outLen = inLen;
    char * outBuf = new char[outLen*enlarg + 1];
    
    //调用code_convert
    CodeConvert("utf-8", "gb2312", pText, inLen, outBuf, outLen);
    outBuf[outLen] = '\0';
    pOut = outBuf;
    
    //结束
    delete[]outBuf;
    
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
//for linux using iconv.h 中的转换函数进行转换
#ifdef WIN32
wchar_t *STRING_Helper::StringToLPCWSTR(string change_string)
{
	DWORD buffer_len = MultiByteToWideChar(CP_ACP, 0, change_string.c_str(), -1, NULL, 0);
	//获得所需的宽字符数组空间
	wchar_t *buffer = new wchar_t[buffer_len];//分配响应的数组空间
	if (!buffer)
		delete[]buffer;
	MultiByteToWideChar(CP_ACP, 0, change_string.c_str(), -1, buffer, buffer_len);//转换为ASCI码
	return buffer;
}

void STRING_Helper::UTF_8ToUnicode(wchar_t* pOut, const char *pText)
{
    char* uchar = (char *)pOut;
    
    uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
    uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);
    
    return;
}

void STRING_Helper::UnicodeToUTF_8(char* pOut, wchar_t* pText)
{
    // 注意 WCHAR高低字的顺序,低字节在前，高字节在后
    char* pchar = (char *)pText;
    
    pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
    pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
    pOut[2] = (0x80 | (pchar[0] & 0x3F));
    
    return;
}

void STRING_Helper::UnicodeToGB2312(char* pOut, wchar_t uData)
{
    WideCharToMultiByte(CP_ACP, NULL, &uData, 1, pOut, sizeof(wchar_t), NULL, NULL);
    return;
}

void STRING_Helper::Gb2312ToUnicode(wchar_t* pOut, char *gbBuffer)
{
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, gbBuffer, 2, pOut, 1);
    return;
}

#else
int STRING_Helper::CodeConvert(const char *from_charset, const char *to_charset, const char *inbuf, int inlen, char *outbuf, int outlen) {
//#if !defined(OPEN_WRT) && !defined(__ANDROID__) && !defined(__APPLE__) //ly mac
#if !defined(OPEN_WRT) && !defined(__ANDROID__) //ly table_dir
    iconv_t cd;
    //int rc;
    size_t inlen_tmp = inlen;
    char **pin = (char **)&inbuf;
    
    char **pout = &outbuf;
    
    cd = iconv_open(to_charset, from_charset);
    
    if (cd == 0)
        return -1;
    
    memset(outbuf, 0, outlen);
    
    iconv(cd, pin, &inlen_tmp, pout, (size_t *)&outlen);
    //if (iconv(cd,pin,(unsigned int *)&inlen,pout,(unsigned int *)&outlen)==(unsigned int)-1)
    //return -1;
    
    iconv_close(cd);
#else
    cout << "ERROR:" << "STRING_Helper::CodeConvert() not supported in OPEN_WRT and ANDROID" << endl;
#endif
    return 0;
    
}

#endif

void STRING_Helper::HTTP_UnEscString(const char *EscStr, std::string & strUnEscStr)
{
    unsigned int j, len;
    //char tempstr[4] = { 0 };
    unsigned char tempchar;
    unsigned char tempchar1;
    unsigned char tempchar2;
    
    char * UnEscStr;
    
    len = (size_t)strlen(EscStr);
    
    UnEscStr = new char[len + 1];
    
    j = 0;
    
    for (unsigned int i = 0; i<len; i++)
    {
        tempchar = (unsigned char)EscStr[i];
        if (tempchar != '%') {
            UnEscStr[j++] = EscStr[i];
        }
        else {
            tempchar1 = (unsigned char)EscStr[i + 1];
            tempchar2 = (unsigned char)EscStr[i + 2];
            
            if (tempchar1 >= 0x30 && tempchar1 <= 0x39)
                tempchar1 -= 0x30;
            else if (tempchar1 >= 0x41 && tempchar1 <= 0x46)
                tempchar1 -= 0x37;
            else
                tempchar1 = 0;
            
            if (tempchar2 >= 0x30 && tempchar2 <= 0x39)
                tempchar2 -= 0x30;
            else if (tempchar2 >= 0x41 && tempchar2 <= 0x46)
                tempchar2 -= 0x37;
            else
                tempchar2 = 0;
            
            tempchar = tempchar1 * 16 + tempchar2;
            UnEscStr[j++] = tempchar;
            i += 2;
        }
    }
    UnEscStr[j++] = '\0';
    
    strUnEscStr = UnEscStr;
    delete[] UnEscStr;
    
}

void STRING_Helper::HTTP_EscString(const char *EscStr, std::string & strTo)
{
    //将字母、数字以外的字符转换为%与字符的十六进制ASCII值组成的编码
    unsigned int j, len;
    char tempstr[4] = { 0 };
    unsigned char tempchar;
    
    len = (size_t)strlen(EscStr);
    j = 0;
    
    char * buffer = new char[len * 3 + 1];
    memset(buffer, 0, len * 3 + 1);
    
    for (unsigned int i = 0; i<len; i++)
    {
        tempchar = (unsigned char)EscStr[i];
        if (tempchar == ' ')
        {//将空格转换为+号，空格不转换为+直接用%20也可以，在Notes代理的字符转换函数中已作兼容处理了
            buffer[j++] = ' ';
        }
        else if (tempchar == '"')
        {
            buffer[j++] = '"';
        }
        else if ((tempchar<0x30) ||
                 ((tempchar>0x39) && (tempchar<0x41)) ||
                 ((tempchar>0x5A) && (tempchar<0x61)) ||
                 (tempchar>122))
        {
            //对所有非数字、字母的字符都要进行转换
            sprintf(tempstr, "%%%2x", tempchar);
            strupr(tempstr);
            
            if (tempstr[1] == ' ')
                tempstr[1] = '0';
            
            strcat(buffer, tempstr);
            j += strlen(tempstr);
        }
        else
        {
            buffer[j++] = tempchar;
        }
    }
    buffer[j++] = '\0';
    
    strTo = buffer;
    
    delete[] buffer;
}

double STRING_Helper::ParseFload(const char *s){
    return atof(s);
}

double STRING_Helper::ParseFload(string str){
    if (str.compare(0, 2, "0x") == 0){
        return STRING_Helper::ParseInt(str);//string temp_s=this->HBCStrToHBCStr(str,"hex","",false)//应该有一个16进制转换
    }else{
        str+='\0';
        return atof(str.c_str());
    }
}

int STRING_Helper::ParseInt(const char *s){
    string s1=s;
    return STRING_Helper::ParseInt(s1);
}


// String 转换位数字
int STRING_Helper::ParseInt(string s){
    //判断是否为争取的 String
    if (!STRING_Helper::CheckNum(s) || s.size()==0)
        return STRING_Helper::NaN;
    
    if (s.compare(0,2,"0x")==0 || s.compare(0,2,"0X")==0){//0x*** String ；
        return STRING_Helper::ParseInt(s.substr(2),16);
    }else if (s.compare(0,2,"0b")==0 || s.compare(0,2,"0B")==0){//0b*** String ；
        return STRING_Helper::ParseInt(s.substr(2),2);//ly change 2014;
    }else
        return STRING_Helper::ParseInt(s,10);//普通 String ；
}

int STRING_Helper::ParseInt(string s, int radix){
    
    if (radix==10){//十进制 String 转换成数值；
        return atoi(s.c_str());
    }else if (radix==2){//而进制 String 转换成数值；
        if (s.compare(0, 2, "0b") == 0 || s.compare(0, 2, "0B") == 0)
            s.erase(0, 2);
        int temp=1,x=0;
        for(int i=(int)s.size()-1;i>=0;i--,temp+=temp){
            if(s[i]=='1')
                x+=temp;
        }
        return x;
        
    }else if (radix==16){//16进制转换成数值；
        if (s.compare(0, 2, "0x") == 0 || s.compare(0, 2, "0X") == 0)
            s.erase(0,2);
        unsigned char num=0;
        int result=0;
        int nsqr=1;
        for(int i=(int)s.size()-1;i>=0;i--){
            if(s[i]>='0'&&s[i]<='9')
                num=s[i]-'0';
            else if(s[i]>='A'&&s[i]<='F')
                num=s[i]-'A'+10;
            else
                num=s[i]-'a'+10;
            result+=num*nsqr;
            nsqr*=16;
        }
        return result;
    }else
        return STRING_Helper::NaN;
}

/*bool STRING_Helper::ParseULong(unsigned long &d, string s) {
	if (!STRING_Helper::CheckNum(s) || s.size() == 0)
 return false;
	char *ptr;
	d = 0;
 
	if (radix == 10 || radix == 16 || radix == 0) {//十进制 String 转换成数值；
 d=strtoul(s.c_str(), &ptr, radix);
	}else if (radix == 2) {//而进制 String 转换成数值；
 if (s.compare(0, 2, "0b") == 0 || s.compare(0, 2, "0B") == 0)
 s.erase(0, 2);
 int temp = 1, x = 0;
 for (int i = s.size() - 1; i >= 0; i--, temp += temp) {
 if (s[i] == '1')
 x += temp;
 }
 d=x;
 
	}else
 return false;
	return true;
 }*/
unsigned long STRING_Helper::ParseULong(string s, int radix) {
    if (!STRING_Helper::CheckNum(s) || s.size() == 0)
        return STRING_Helper::NaN;
    char *ptr;
    
    if (radix == 10 || radix==16 ||radix==0) {//十进制 String 转换成数值；
        return strtoul(s.c_str(), &ptr, radix);
    }
    else if (radix == 2) {//而进制 String 转换成数值；
        if (s.compare(0, 2, "0b") == 0 || s.compare(0, 2, "0B") == 0)
            s.erase(0, 2);
        int temp = 1, x = 0;
        for (int i = (int)s.size() - 1; i >= 0; i--, temp += temp) {
            if (s[i] == '1')
                x += temp;
        }
        return x;
        
    }else
        return STRING_Helper::NaN;
}

//函数:数字转 String ,无0b,0x等标志.
string STRING_Helper::iToStr(int num,int radix){
    char buf[16];
    string s;
    if ((num<0) && (radix==10)){
        ostringstream oss;
        oss << num;
        return oss.str();
    }else
        itoa(num,buf,radix);
    return s=buf;
}

string STRING_Helper::iToStr(int num){
    return STRING_Helper::iToStr(num,10);
}

string STRING_Helper::uToStr(size_t num){
    return STRING_Helper::iToStr((int)num,10);
}

string STRING_Helper::uToHex(unsigned int num,BYTE width){
    char buffer[12];
    switch (width){
        case 1:
            sprintf(buffer,"0x%01x",num);
            return string(buffer,3);
        case 2:
            sprintf(buffer,"0x%02x",num);
            return string(buffer,4);
        case 4:
            sprintf(buffer,"0x%04x",num);
            return string(buffer,6);
        case 8:
            sprintf(buffer,"0x%08x",num);
            return string(buffer,10);
    }
    return "";
}

//函数:数字转 String ,无0b,0x等标志.
int STRING_Helper::StrToi(string str){
    str+='\0';
    return atoi(str.c_str());
}


//函数:数字转 String ,无0b,0x等标志.
double STRING_Helper::StrTof(string str) {
	str += '\0';
	const char*p = str.c_str();
	if (p[0]=='0' && (p[1]=='x' || p[1]=='X')) {
		return STRING_Helper::ParseInt(p);
	}else
		return atof(str.c_str());
}

string STRING_Helper::dToStr(double val)
{
    ostringstream oss;
    
    oss<<val;
    
    return oss.str();
}
string STRING_Helper::fToStr(float iF)
{
    char chFloat[20];
    memset(chFloat,'\0',sizeof(chFloat));
    sprintf(chFloat,"%f",iF);
    
    string tmp=chFloat;
    return tmp;
}

int STRING_Helper::_StrLen(string &s){
    return (int)s.size();
}

string STRING_Helper::SetNumToID(string s,int len, const char*split){
    string str1;
    int i,j;
    
    if (len<=0)
        return (string)"";
    if (!split)
        split="0";
    
    str1 = s;
    
    j = len-STRING_Helper::_StrLen(str1);
    for(i=0;i<j;i++)
        str1=STRING_Helper::_StrAdd(str1,split,false);
    return str1;
}

string STRING_Helper::_StrAdd(string &s,const char *add_str,bool from){
    string s1;
    if (from)
        s+=add_str;
    else
        s.insert(0,add_str);
    s1=s;
    return s1;
}

bool STRING_Helper::CheckNum(const char *s){
    if (s == 0 || strlen(s) == 0)
        return false;

    string num_str=s;
    return STRING_Helper::CheckNum(num_str, "", (int)num_str.size());
}

bool STRING_Helper::CheckNum(string &num_str){
    
    return STRING_Helper::CheckNum(num_str, "", (int)num_str.size());
}

bool STRING_Helper::CheckNum(string &num_str, const char *Radix){
    
    return STRING_Helper::CheckNum(num_str, Radix, (int)num_str.size());
}

//js,c++
bool STRING_Helper::CheckNum(string &num_str, const char *Radix, int len){
    int i,test_len;
    string strTemp,type;
    
    if ( STRING_Helper::_StrLen(num_str)== 0 && (STRING_Helper::_IsNull(len) || len==0))
        return true;
    
    if ((Radix && PUB_FindStr(Radix,LW_HEX_STR)>=0) ||(PUB_FindStr(num_str,"0x")==0 || PUB_FindStr(num_str,"0X")==0)){
        strTemp="xX0123456789abcdefABCDEF";
        type=LW_HEX_STR;
    }else if((Radix && PUB_FindStr(Radix,LW_BIT_STR)>=0) ||  (PUB_FindStr(num_str,"0b")==0 || PUB_FindStr(num_str,"0B")==0)){
        strTemp="01bB";
        type=LW_BIT_STR;
    }else if (Radix && PUB_FindStr(Radix,"num")>=0){
        strTemp=".0123456789";
        type="num";
    }else{
        strTemp=".0123456789xbXB";
    }
    
    //测试 String 正确性=
    const char *num_cstr=num_str.c_str();
    for (i=0;i<STRING_Helper::_StrLen(num_str);i++)
    {
        //if (num_cstr[i]=='\0')
            //continue;
        if (strTemp.find(num_cstr[i])==strTemp.npos)
        {//说明有字符不是数字
            return false;
        }
    }
    
    // Length 正确性
    if (type==LW_BIT_STR || type==LW_HEX_STR)
        test_len=STRING_Helper::_StrLen(num_str)-2;
    else
        test_len=STRING_Helper::_StrLen(num_str);
    
    if (test_len>len)
        return false;
    
    //说明是数字
    return true;
}

string STRING_Helper::_SubStr(string &s,int pos,int len){
    return s.substr(pos,len);
}

bool STRING_Helper::_IsNull(const char *obj){
    if (strlen(obj)==0)
        return true;
    else
        return false;
}

bool STRING_Helper::_IsNull(string &obj){
    if (obj=="")
        return true;
    else
        return false;
}
bool STRING_Helper::CheckAllChar(string &obj,char ch){
    for(size_t i=0; i<obj.size(); i++)
        if (obj.at(i)!=ch)
            return false;
    return true;
}

bool STRING_Helper::_IsNull(int len){
    return true;
}

int STRING_Helper::PUB_FindStr(const char *str,const char*s){
    string s1=str,s2=s;
    return STRING_Helper::PUB_FindStr(s1,s2);
    //return (strstr(str,s)-str);
}

int STRING_Helper::PUB_FindStr(string &str,const char*s){
    string s1=s;
    return STRING_Helper::PUB_FindStr(str,s1);
}

int STRING_Helper::PUB_FindStr(string &str,string s){
    size_t i=str.find(s);
    if (i==string::npos)
        return -1;
    else
        return (int)i;
    
}

int STRING_Helper::PUB_FindStr(string &str,string s,bool from_end){
    size_t i;
    if (from_end==true)
        i=str.rfind(s);
    else
        i=str.find(s);
    if (i==string::npos)
        return -1;
    else
        return (int)i;
    
}

bool STRING_Helper::PUB_FindStrIs(string &str,const char*s){
    string s1=s;
    return STRING_Helper::PUB_FindStrIs(str,s1);
}

bool STRING_Helper::PUB_FindStrIs(string &str,string s){
    return str.find(s)!=string::npos;
}

int STRING_Helper::C_FindFirstStr(char *src, char *pos, vector<string> &finds, int len) {
    pos = NULL;
    char *tmp = NULL;
    int k = -1;
    
    for (size_t i=0; i < finds.size(); i++) {
        if ((tmp = STRING_Helper::C_StrStrLen(src, finds[i].c_str(), len)) != NULL)
            if (pos == NULL || tmp < pos) {
                pos = tmp;
                k = i;
            }
        
    }
    return k;
}

int STRING_Helper::FindFirstStr(string src_str, string find_str,const char *split, bool skip_block) {
    unsigned int j=0, start_pos=0;
    //分拆查询字串
    vector <string> finds;
    STRING_Helper::stringSplit(find_str, finds, split);
    
    //跳过空格
    char *tmp_c;
    char *src = (char*)src_str.data();
    if (skip_block) {
        while (j < strlen(src)) {
            tmp_c = (src + start_pos);
            //if (isspace((*tmp_c)) == 0)
            if ((*tmp_c)>32 && (*tmp_c)<127)
                break;
            start_pos++;
            j++;
        }
    }
    //查询字串
    size_t tmp=0;
    int pos=-1;
    for (size_t i = 0; i < finds.size(); i++) {
        if ((tmp = src_str.find(finds[i],start_pos)) != string::npos)
            if (pos == -1 || tmp < (size_t)pos) {
                pos = tmp;
            }
    }
    return pos;
}

int STRING_Helper::C_CmpFirstStr(char *src, int &start_pos, const char *find_str, const char *split, int len, bool skip_block) {
    start_pos = 0;
    char *tmp = NULL;
    unsigned int j = 0;
    //
    vector <string> finds;
    STRING_Helper::stringSplit(find_str, finds, split);
    
    //跳过空格
    if (skip_block) {
        while (j < strlen(src)) {
            tmp = (src + start_pos);
            //if (isspace((*tmp)) == 0)
            if ((*tmp)>32 && (*tmp)<127)
                break;
            start_pos++;
            j++;
        }
    }
    
    for (unsigned int i = 0; i < finds.size(); i++) {
        if ((strncmp(src+start_pos, finds[i].c_str(),finds[i].size())) == 0)
            return i;
    }
    return -1;
}



char *STRING_Helper::C_StrStrLen(const char *s1, const char *s2, size_t len) {
    char *p2s1 = (char*)s1;
    char *p2s2 = (char*)s2;
    
    size_t i = 0, j, k;
    i = k = j = 0;
    while (i<strlen(s1) && j<strlen(s2))
    {
        //补充长度判断
        if (i >= len)
            break;
        
        if (*(p2s1 + i) == *(p2s2 + j))
        {
            i++;
            j++;
        }
        else
        {
            j = 0;
            k++;
            i = k;//可用于记录原来i的位置
        }
    }
    
    if (j >= strlen(s2))
        return (p2s1 + i-1);//判断找到字符串的条件
    else
        return NULL;
}

int STRING_Helper::C_StrCmpLen(char * s1, char * s2, size_t n) {
    if ( !n )//n为无符号整形变量;如果n为0,则返回0
        return(0);
    //在接下来的while函数中
    //第一个循环条件：--n,如果比较到前n个字符则退出循环
    //第二个循环条件：*s1,如果s1指向的字符串末尾退出循
    //第二个循环条件：*s1 == *s2,如果两字符比较不等则退出循环
    while (--n && *s1 && *s1 == *s2)
    {
        s1++;//S1指针自加1,指向下一个字符
        s2++;//S2指针自加1,指向下一个字符
    }
    return( *s1 - *s2 );//返回比较结果
}

//把字符串拆分成数组
void STRING_Helper::stringSplit(const char *content,vector<string> &vecSubContent,string split_str){
    string content1 = content;
    STRING_Helper::stringSplit(content1,vecSubContent,split_str);
};

void STRING_Helper::stringSplit(string &content,vector<string> &vecSubContent,string split_str,bool skip_null_str){
    vecSubContent.clear();
    
    string tempContent;
    size_t bpos,apos=0;
    bpos = content.find(split_str, apos);
    while( bpos!=string::npos)
    {
        tempContent.clear();
        tempContent = content.substr(apos, bpos - apos);
        if (!(skip_null_str && tempContent.size() == 0))
            vecSubContent.push_back(tempContent);
        apos = bpos + split_str.size();
        
        bpos = content.find(split_str, apos);
    }
    
    tempContent = content.substr(apos);
    if(tempContent.size() != 0)
        vecSubContent.push_back(tempContent);
    
}


//取得字符串以head为起点，以end中的任意一个字符为终点，之间的字符串
int STRING_Helper::strStringCut(std::string src_str,std::string &result, std::string head, std::string end){
    size_t ipos1,ipos2;
    string src_str1;
    
    ipos1 = src_str.find(head);
    if( string::npos != ipos1)
    {
        src_str1 = src_str.substr(ipos1+head.size());
        if (end!="")
            ipos2=src_str1.find_first_of(end);
        else
            ipos2=string::npos;
        if( string::npos != ipos2){
            result = src_str1.substr(0, ipos2);
            return 1;
        }else{
            result = src_str1;
            return 1;
        }
    }
    return 0;
}

//取得字符串以head为起点，以end为终点，之间的字符串;
//???error in end,return false;
int STRING_Helper::strStringCutField(std::string src_str,std::string &result, std::string head, std::string end){
    size_t ipos1,ipos2;
    string src_str1;
    
    ipos1 = src_str.find(head);
    if( string::npos != ipos1)
    {
        src_str1 = src_str.substr(ipos1+head.size());
        ipos2=src_str1.find(end);
        if( string::npos != ipos2){
            result = src_str1.substr(0, ipos2);
            return 1;
        }else{
            result = src_str1;
            return 1;
        }
    }
    return 0;
}

//取得字符串以head为起点，以end为终点，之间的字符串
int STRING_Helper::strStringCutFrom(std::string src_str,std::string &result, std::string head, std::string end, size_t start){
    size_t ipos1,ipos2;
    string src_str1;
    
    ipos1 = src_str.find(head,start);
    if( string::npos != ipos1)
    {
        src_str1 = src_str.substr(ipos1+head.size());
        ipos2=src_str1.find(end);
        if( string::npos != ipos2){
            result = src_str1.substr(0, ipos2);
            return 1;
        }
    }
    return 0;
}

//取得字符串以head为起点，以end为终点，之间的字符串
bool STRING_Helper::strFastCut(std::string &src_str, std::string head_str, std::string end_str, size_t start){
    size_t ipos1,ipos2;
    size_t pstart_len,pend_len;
    
    ipos1 = src_str.find(head_str,start);
    if( string::npos != ipos1){
        //find head str
        pstart_len = ipos1+head_str.size();
        src_str.erase(0,pstart_len);// delete head str
        ipos2=src_str.rfind(end_str);
        if( string::npos != ipos2){
            //find end str
            pend_len = src_str.size()-ipos2;
            src_str.erase(ipos2,pend_len); //delete end str;
            return true;
        }
    }
    return false;
}

//取得字符串以head为起点，以end为终点，之间的字符串
//url字符串
//is_all_name:是否回传整个文件名。
//is_add_part:是否只回传扩展名
bool STRING_Helper::strStringCutUrlFileName(std::string url,std::string &result){
    return STRING_Helper::strStringCutUrlFileName(url,result,"");
}

bool STRING_Helper::strStringCutUrlFileName(string url,std::string &result,const char *part){
    
    //取得字段名称
    if(strlen(part)>0 && part[0]=='&'){//取得字段名称
        char *uri=(char *)url.c_str();
        if (strchr(uri,'?')!=0)
            uri=strchr(uri,'?');
        part++;
        if(strstr(uri,part) !=0 ){
            STRING_Helper::strStringCut(uri,result,(string)part+"="," &\r\n");
            return true;
        }else
            return false;
        
    }else{//取得文件名称
        std::string temp1;
        std::string temp2;
        //去除&参数
        if(url.find_first_of("?&\r\n")!=string::npos){
            temp2=url.substr(0,url.find_first_of(" ?&\r\n"));
        }else
            temp2=url;
        //去除最后一个/
        if(temp2.find_last_of("/\\")!=string::npos){
            temp1 = temp2.substr(temp2.find_last_of("/\\")+1);
        }else
            temp1 = temp2;
        
        
        //取得文件名
        if (strcmp(part,"SECOND_PART")==0){//返回第二部分
            if(temp1.find_last_of(".")!=string::npos)
                result=temp1.substr(temp1.find_last_of(".")+1);
            else
                return false;
        }else if(strcmp(part,"FIRST_PART")==0){//返回第一部分
            if(temp1.find_last_of(".")!=string::npos)
                result=temp1.substr(0,temp1.find_last_of("."));
            else
                result=temp1;
        }else
            result=temp1;//返回全部文件
        return true;
    }
}

void STRING_Helper::strReplace(string &src_str,char old_char, char new_char){
    unsigned int i;
    if (old_char!=0 && new_char!=0) //LY 2013-1-28
        for(i=0; i<src_str.size(); i++){
            if (src_str[i]==old_char)
                src_str[i]=new_char;
        }
}

void STRING_Helper::strReplace(string &src_str,const char *old_str, const char *new_str){
    string olds(old_str);
    string news(new_str);
    STRING_Helper::strReplace(src_str,olds,news);
}


void STRING_Helper::strReplace(string &src_str,string old_str, string new_str){
    size_t ipos;
    while(1){
        if((ipos= src_str.find(old_str)) != string::npos ){
            //if (new_str==""){
            //src_str.erase(ipos,old_str.size());//ly 2013-5-21 //替换空字符串时出现问题
            //}else
            src_str.replace(ipos,old_str.size(),new_str);
            break;
        }else
            break;
    }
}

void STRING_Helper::strReplace(string &src_str,string old_str, const char *new_str, UInt64 new_str_size){
    size_t ipos;
    while(1){
        if((ipos= src_str.find(old_str)) != string::npos ){
            src_str.replace(ipos,old_str.size(),new_str,new_str_size);
            break;
        }else
        break;
    }
}

void STRING_Helper::strReplaceAll(string &src_str,string old_str, string new_str){
    size_t ipos=0;
    while(1){
        if((ipos= src_str.find(old_str,ipos)) != string::npos ){
            src_str.replace(ipos,old_str.size(),new_str);
            ipos+=new_str.size();
        }else
            break;
    }
}

void STRING_Helper::GetNowDate(std::string &ret_str){
    STRING_Helper::GetNowDate(ret_str,"-");
}

void STRING_Helper::GetNowDate(std::string &ret_str,const char *split_str)
{
    time_t t;
    time(&t);
    tm * lt = localtime(&t);
    
    stringstream out;
    out <<lt->tm_year + 1900;
    
    if (lt->tm_mon+1<10)
        out<<split_str<<0<<lt->tm_mon+1;
    else
        out<<split_str<<lt->tm_mon+1;//01月补齐
    
    if (lt->tm_mday<10)
        out<<split_str<<0<<lt->tm_mday;
    else
        out<<split_str<<lt->tm_mday;//01日补齐
    
    ret_str =out.str();
}

UInt64 STRING_Helper::GetNowTimeSec(){
    return time(NULL);
}

void STRING_Helper::GetNowTime(std::string &ret_str){
    STRING_Helper::GetNowTime(ret_str,":");
}

void STRING_Helper::GetNowTime(std::string &ret_str,const char * split_str)
{
    //time string:
    
    time_t t;
    time(&t);
    tm * lt = localtime(&t);
    
    if (strstr(split_str, "GEM") != split_str) {
        stringstream out;
        //out <<lt->tm_hour<<":"<<lt->tm_min<<":"<<lt->tm_sec;
        if (lt->tm_hour < 10)
            out << 0 << lt->tm_hour;
        else
            out << lt->tm_hour;//01补齐
        
        if (lt->tm_min < 10)
            out << split_str << 0 << lt->tm_min;
        else
            out << split_str << lt->tm_min;//01补齐
        
        if (lt->tm_sec < 10)
            out << split_str << 0 << lt->tm_sec;
        else
            out << split_str << lt->tm_sec;//01补齐
        ret_str =out.str();
    }
    else {
        string lasttime = ctime(&t);
        
        string ret_str = lasttime.substr(0, 3).c_str();
        ret_str.append(", ");
        ret_str.append(lasttime.substr(8, 2).c_str());
        ret_str.append(lasttime.substr(3, 4).c_str());
        ret_str.append(lasttime.substr(19, 5).c_str());
        ret_str.append(lasttime.substr(10, 9).c_str());
        ret_str.append(" GMT");
        
        
    }
    //time GEM
    
    
}

std::string STRING_Helper::strRandTime(void)
{
    std::string ret_str;
    
    char time_str[30];
#ifdef WIN32
    sprintf(time_str, "%d", time(NULL));
#else
    sprintf(time_str, "%lu", time(NULL));
#endif
    ret_str = time_str;
    
    return ret_str;
}

bool STRING_Helper::MatchStr2Hex(string &src,string match)
{
    //测试头匹配情况
    vector <string> tmp,tmp2;
    bool finded=false;
    STRING_Helper::stringSplit(match,tmp,"|");
    for(size_t i=0; i<tmp.size(); i++){
        //区段对比
        if (tmp[i].find("~")!=string::npos){
            //"21~71"
            tmp2.clear();
            STRING_Helper::stringSplit(tmp[i],tmp2,"~");
            int begin=STRING_Helper::ParseInt(tmp2[0],16);
            int end=STRING_Helper::ParseInt(tmp2[1],16);
            if (begin!=STRING_Helper::NaN && end!=STRING_Helper::NaN){
                if (begin<=src[0] && src[0]<=end){
                    finded=true;
                    break;
                }
            }
        }
        //普通一一对比
        //cout << HBCStrToHBCStr(tmp[i], "hex", "char", false) << " "<< tmp[i].size()/2 <<" " <<src.compare(0, tmp[i].size() / 2, HBCStrToHBCStr(tmp[i], "hex", "char", false)) << endl;
        if (src.compare(0, tmp[i].size() / 2, HBCStrToHBCStr(tmp[i], "hex", "char", false)) == 0) {
            finded=true;
            break;
        }

    }
    return finded;
}

string STRING_Helper::UInt8ToBinaryStr(UInt8 u)
{
    unsigned char len_s[5];
    UInt8 *l_len;
    l_len=(UInt8 *)len_s;
    *l_len = u;
    
    std::string r_str;
    r_str.append((const char *)len_s,sizeof(UInt8));
    return r_str;
}

string STRING_Helper::UInt16ToBinaryStr(UInt16 u)
{
    unsigned char len_s[10];
    UInt16 *l_len;
    l_len=(UInt16 *)len_s;
    *l_len = u;
    //len_s[2]='\0';
    
    std::string r_str;
    r_str.append((const char *)len_s,sizeof(UInt16));
    return r_str;
}

string STRING_Helper::UInt32ToBinaryStr(UInt32 u)
{
    std::string r_str;
    unsigned char len_s[20];
    
    UInt32 *l_len;
    l_len=(UInt32 *)len_s;
    *l_len = u;
    
    r_str.append((const char *)len_s,sizeof(UInt32));
    return r_str;
}

string STRING_Helper::UInt64ToBinaryStr(UInt64 u)
{
    unsigned char len_s[40];
    UInt64 *l_len;
    l_len=(UInt64 *)len_s;
    *l_len = u;
    
    std::string r_str;
    r_str.append((const char *)len_s,sizeof(UInt64));
    return r_str;
}

UInt32 STRING_Helper::BinaryStrToUInt32(string &in_str)
{
    UInt32 *l_len;
    l_len=(UInt32 *)in_str.data();
    return (*l_len);
}

void STRING_Helper::strRandTime(std::string &ret_str)
{
    //char time_str[30];
    //sprintf(time_str, "%d", time(NULL));
    //STRING_Helper::strRandTime();
    ret_str = STRING_Helper::strRandTime();
}

string  &STRING_Helper::strTrim(std::string &s,const char* val){
    s.erase(0,s.find_first_not_of(val));
    s.erase(s.find_last_not_of(val)+1);
    return s;
}

template <typename T> string STRING_Helper::ToStr(T val)
{
    ostringstream oss;
    
    oss<<val;
    
    return oss.str();
}

string  &STRING_Helper::strTrim(std::string &s){
    return STRING_Helper::strTrim(s,"\r\t\n ");
}

string&  STRING_Helper::LeftTrim(string   &str)
{
    //跳过空格
    char *tmp_c=(char*)str.data();
    size_t j;
    for (j = 0; j < str.size(); j++,tmp_c++) {
        //if (isspace((*tmp_c))==0)
        if ((*tmp_c)>32 && (*tmp_c)<127)
            break;
    }
    str.erase(0,j);
    //string::iterator iter=find_if(str.begin(),str.end(),not1(ptr_fun<int>(::isspace)));
    //str.erase(str.begin(),iter);
    return  str;
}

string&  STRING_Helper::LowerCase(string   &str)
{
#ifdef WIN32
    char * tmp=_strdup(str.c_str());
#else
    char * tmp = strdup(str.c_str());
#endif
    //str = strlwr(tmp);
    int i=0;
    while(tmp[i])
    {
        if(tmp[i]>='A' && tmp[i]<='Z')
        {
            tmp[i] += 32;
        }
        i++;
    }
    //
    str=tmp;
    free(tmp);
    
    return  str;
}


string&  STRING_Helper::UpperCase(string   &str)
{
#ifdef WIN32
    char * tmp=_strdup(str.c_str());
#else
    char * tmp = strdup(str.c_str());
#endif
    //str = strupr(tmp);
    //
    int i=0;
    while(tmp[i])
    {
        if(tmp[i]>='a' && tmp[i]<='z')
        {
            tmp[i] -= 32;
        }
        i++;
    }
    str=tmp;
    //
    free(tmp);
    return  str;
}

string&  STRING_Helper::RightTrim(string   &str)
{
    string::reverse_iterator rev_iter=find_if(str.rbegin(),str.rend(),not1(ptr_fun<int>(::isspace)));
    str.erase(rev_iter.base(),str.end());
    return   str;
}

string& STRING_Helper::Trim(string   &st)
{
    return STRING_Helper::LeftTrim(STRING_Helper::RightTrim(st));
}


void STRING_Helper::HashNumStrToID(const char* s,string &out_s) {
    long h,hash;
    h=hash=0;
    
    if (h == 0) {
        unsigned int  i;
        for (i = 0; i < strlen(s); i++) {
            //h = 31*h + parseInt(s.substr(i,1),10);
            h = 12*h + (s[i]-48);
        }
        hash = h;
    }
    
    stringstream out;
    out<<h;
    out_s = out.str();
    return ;
}

//以下暂时Linux 不兼容
void STRING_Helper::strRandTime_Us(std::string &ret_str)
{
    char time_str[60];
    
    uint64_t istart=LW_GetTickCount();
    
    sprintf(time_str, "%lu_%llu", (unsigned long)time(NULL),(unsigned long long)istart);
    ret_str = time_str;
}


string STRING_Helper::iToChar(int num){
    char a;
    a=(char )num;
    if (num!=0)
        return (string)""+a;
    else{
        string s(1,0);
        return s;
    }
}

char STRING_Helper::_CharAt(string &s,int pos){
    const char* c=s.c_str();
    return *(c+pos);
}

//函数: String 进制转换.把temp从src_hbc转成dest_hbc进制数;并判断是否增加add_head（0x,0b）
//函数: String 进制转换.把temp从src_hbc转成dest_hbc进制数;并判断是否增加add_head（0x,0b）
string STRING_Helper::HBCStrToHBCStr(const char *temp, const char *src_hbc, const char *dest_hbc, bool add_head){
    string temp_s=temp,src_hbc_s=src_hbc,dest_hbc_s=dest_hbc;
    return STRING_Helper::HBCStrToHBCStr(temp_s,src_hbc_s,dest_hbc_s,add_head);
}

string STRING_Helper::HBCStrToHBCStr(string &temp, const char *src_hbc, const char *dest_hbc, bool add_head){
    string src_hbc_s=src_hbc,dest_hbc_s=dest_hbc;
    return STRING_Helper::HBCStrToHBCStr(temp,src_hbc_s,dest_hbc_s,add_head);
}

string STRING_Helper::HBCStrToHBCStr(string &temp, string &src_hbc, const char *dest_hbc, bool add_head){
    string dest_hbc_s=dest_hbc;
    
    return STRING_Helper::HBCStrToHBCStr(temp,src_hbc,dest_hbc_s,add_head);
}

string STRING_Helper::HBCStrToHBCStr(string &temp, const char* src_hbc, string &dest_hbc, bool add_head){
    string src_hbc_s=src_hbc;
    
    return STRING_Helper::HBCStrToHBCStr(temp,src_hbc_s,dest_hbc,add_head);
}
/*
 string ParseProcess::Http_ParsePacket::HBCStrToBinary(string &temp, string &src_hbc, char * dest, int &len){
	//取得二进制字符流.
	string bin_str;
	bin_str = this->HBCStrToHBCStr(temp,src_hbc,LW_BINARY_STR,false);
	
	//把二进制字符流转换为字符（包括\0）.
	memcpy((void *)dest,(void *)bin_str.data(),bin_str.size()+1);
	len=bin_str.size()
 }*/

//各种进制 String 之间的转换-》已经具有了字符流 String 的转换能力
string STRING_Helper::BinaryToHBCStr(const char* data, int data_len, const char *dest_hbc, bool add_head){
    string temp(dest_hbc);
    return STRING_Helper::BinaryToHBCStr(data,data_len,temp,add_head);
}

string STRING_Helper::BinaryToHBCStr(const char* data, int data_len, string &dest_hbc, bool add_head){
    string ns="",s,d_bit,ds,ds1,add_head_str;
    int i,dest_num;
    unsigned short int num;
    
    s="";
    //data全部生成二进制 String .
    for (i=0;i<data_len; i++){
        //取得 Source 字符数据.
        num = data[i];//获取ascii
        if (num >= 0xff00)
            num =num-0xff00; //!!!这个部分不是很确定,为什么会有ff之类,也学只能适用于32位c++
        d_bit=STRING_Helper::SetNumToID(STRING_Helper::iToStr(num,2),8,"0"); //转换为二进制,并填补位数.
        s+=d_bit;
    }
    
    //生成目标 String
    ds="";
    for(i=0; i<STRING_Helper::_StrLen(s);){
        ds1=ds;
        if (dest_hbc==LW_HEX_STR){
            dest_num = STRING_Helper::ParseInt(s.substr(i,4),2);
            ds+=STRING_Helper::iToStr(dest_num,16);
            i+=4;
        }else if (dest_hbc==LW_BINARY_STR){
            dest_num=STRING_Helper::ParseInt(s.substr(i,8),2);
            //if (dest_num!=0) //only_c++
            ds+=STRING_Helper::iToChar(dest_num);
            i+=8;
        }else {//如果是二进制,则原样照搬
            ds=s;
            break;
        }
    }
    
    //添加头描述字段
    if (add_head){
        add_head_str=(dest_hbc==LW_HEX_STR ? "0x" : (dest_hbc==LW_BIT_STR ? "0b" : ""));
        return add_head_str+ds;
    }else {
        return ds;
    }
}

//各种进制 String 之间的转换-》已经具有了字符流 String 的转换能力
string STRING_Helper::HBCStrToHBCStr(string &temp, string &src_hbc, string &dest_hbc, bool add_head){
    if (src_hbc=="char"){
        return STRING_Helper::BinaryToHBCStr(temp.c_str(),temp.size(),dest_hbc,add_head);
    };
    
    string ns="",s,d_bit,ds,ds1,add_head_str,temp1;
    int i,k,num,dest_num;
    
    //判断缺省数字类型；
    if (STRING_Helper::_IsNull(src_hbc)){
        if (STRING_Helper::PUB_FindStr(temp,"0x")==0 && STRING_Helper::CheckNum(temp,LW_HEX_STR))
            src_hbc=LW_HEX_STR;
        else if (STRING_Helper::PUB_FindStr(temp,"0b")==0 && STRING_Helper::CheckNum(temp,LW_BIT_STR))
            src_hbc=LW_BIT_STR;
        else
            src_hbc=LW_BINARY_STR;
    }
    if (src_hbc==LW_HEX_STR && STRING_Helper::PUB_FindStr(temp,"0x")==0)
        temp1=temp.substr(2);
    else if (src_hbc==LW_BIT_STR && STRING_Helper::PUB_FindStr(temp,"0b")==0)
        temp1=temp.substr(2);
    else
        temp1=temp;
    
    if (src_hbc==dest_hbc){//如果同类型转换则直接赋值
        ds=temp1;
    }else{
        s="";
        //temp全部生成二进制 String .
        k=STRING_Helper::_StrLen(temp1);
        for (i=0;i<k;){
            //取得 Source 字符数据.
            if (src_hbc==LW_BINARY_STR){//八位
                num = STRING_Helper::_CharAt(temp1,i);//获取ascii
                d_bit=STRING_Helper::SetNumToID(STRING_Helper::iToStr(num,2),8,"0"); //转换为二进制,并填补位数.
                i+=1;
            }else if (src_hbc==LW_HEX_STR){//如果Hex16进制.
                num = STRING_Helper::ParseInt(temp1.substr(i,1),16);
                d_bit=STRING_Helper::SetNumToID(STRING_Helper::iToStr(num,2),4,"0"); //转换为二进制,并填补位数.
                i+=1;
            }else{//如果是二进制,则原样照搬
                s=temp1;
                break;
            }
            s+=d_bit;
        }
        
        //生成目标 String
        ds="";
        //cout << ds.capacity() <<endl;
        //ds.reserve(this->_StrLen(s));
        //cout << ds.capacity() <<endl;
        for(i=0; i<STRING_Helper::_StrLen(s);){
            ds1=ds;
            if (dest_hbc==LW_HEX_STR){
                dest_num = STRING_Helper::ParseInt(s.substr(i,4),2);
                ds+=STRING_Helper::iToStr(dest_num,16);
                i+=4;
            }else if (dest_hbc==LW_BINARY_STR){
                dest_num=STRING_Helper::ParseInt(s.substr(i,8),2);
                //if (dest_num!=0) //only_c++
                ds+=STRING_Helper::iToChar(dest_num);
                i+=8;
            }else {//如果是二进制,则原样照搬
                ds=s;
                break;
            }
        }
    }
    
    //添加头描述字段
    if (add_head){
        add_head_str=(dest_hbc==LW_HEX_STR ? "0x" : (dest_hbc==LW_BIT_STR ? "0b" : ""));
        return add_head_str+ds;
    }else {
        return ds;
    }         
}


BYTE STRING_Helper::putUINT16(BYTE *pBuf, size_t offset, ULONG u16value)
{
    pBuf[offset++] = (BYTE)(u16value >> 8);
    pBuf[offset] = (BYTE)(u16value & 0xff);
    return(2);
}


BYTE STRING_Helper::putUINT32(BYTE *pBuf, size_t offset, ULONG u32value)
{
    BYTE i;
    
    pBuf += (offset + 4);
    
    for (i = 0; i < 4; i++)
    {
        *(--pBuf) = (BYTE)(u32value & 0xff);
        u32value >>= 8;
    }
    
    return(4);
}


BYTE STRING_Helper::putUINT64(BYTE *pBuf, size_t offset, uint64 u64value)
{
    BYTE i;
    
    pBuf += (offset + 8);
    
    for (i = 0; i < 8; i++)
    {
        *(--pBuf) = (BYTE)(u64value & 0xff);
        u64value >>= 8;
    }
    
    return i;
}

void STRING_Helper::putUINTToStr(string &p_buf,uint64 u64value){
    BYTE buff[16];
    p_buf.clear();
    if (u64value <= 0xFF){
        buff[0] = (BYTE)u64value;
        p_buf.append((const char *)buff,1);
    }else if (u64value <= 0xFFFF){
        putUINT16(buff, 0, (UINT)u64value);
        p_buf.append((const char *)buff,2);
    }else if (u64value <= 0xFFFFFFFF){
        putUINT32(buff, 0, (ULONG)u64value);
        p_buf.append((const char *)buff,4);
    }else{
        putUINT64(buff, 0, u64value);
        p_buf.append((const char *)buff,8);
    }
}

string STRING_Helper::putUINTToStr(uint64 u64value,BYTE bit_num){
    BYTE buff[16];
    string p_buf;
    if (bit_num == 8){
        buff[0] = (BYTE)u64value;
        p_buf.append((const char *)buff,1);
    }else if (bit_num == 16){
        putUINT16(buff, 0, (UINT)u64value);
        p_buf.append((const char *)buff,2);
    }else if (bit_num == 32){
        putUINT32(buff, 0, (ULONG)u64value);
        p_buf.append((const char *)buff,4);
    }else{
        putUINT64(buff, 0, u64value);
        p_buf.append((const char *)buff,8);
    }
    return p_buf;
}

UInt16 STRING_Helper::scanUINT16(BYTE *pBuf, size_t offset)
{
    return((((UINT)pBuf[offset]) << 8) + pBuf[offset+1]);
}
UInt32 STRING_Helper::scanUINT32(BYTE *pBuf, size_t offset)
{
    pBuf += offset;
    
    return (((ULONG)pBuf[0]) << 24)+(((ULONG)pBuf[1] ) << 16)+(((ULONG)pBuf[2] ) << 8) + pBuf[3];
}
UInt64 STRING_Helper::scanUINT64(BYTE *pBuf, size_t offset)
{
    pBuf += offset;
    
    return (((uint64)pBuf[0]) << 56)+(((uint64)pBuf[1]) << 48)+(((uint64)pBuf[2]) << 40)+(((uint64)pBuf[3]) << 32)+
    (((uint64)pBuf[4]) << 24)+(((uint64)pBuf[5]) << 16)+(((uint64)pBuf[6]) << 8) + pBuf[7];
    
}
UInt64 STRING_Helper::scanUINTStr(string buf){
    BYTE *pBuf=(BYTE *)buf.c_str();
    BYTE cell_width=buf.size();
    
    switch (cell_width){
        case 1:
            return (*pBuf);
        case 2:
            return scanUINT16(pBuf,0);
        case 4:
            return scanUINT32(pBuf,0);
        case 8:
            return scanUINT64(pBuf,0);
        default:
            return 0;
    }
}

UInt64 STRING_Helper::scanUINTIDStr(string &buf, BYTE bit_num) {
    UInt64 rt = 0;
    UInt64 pos_on = 1;
    for (size_t i = 0; (i < buf.size()) && (i < bit_num); i++) {
        if (buf[i] >= bit_num)
            continue;
        //string tmp=putUINTToStr(1 << (bit_num - buf[i] - 1), bit_num);
        if (rt == 0)
            rt = pos_on << (bit_num - buf[i] - 1);
        else
            rt |= pos_on << (bit_num - buf[i] - 1);
    }
    return rt;
}

//把字符串拆分成数组
void *STRING_Helper::pMemStrPos(char *buf, char * src, unsigned int count) {
    char a = *src;
    char * p;
    char * p_buf;
    char * buf_end = buf + count;
    unsigned cnt = count;
    
    for (p_buf = buf; p_buf <= buf_end;) {
        //查找第一个字符
        p = (char *)memchr((void *)p_buf, a, cnt);
        if (p == 0)
            return 0; //没查到返回0;
        
        //如果查到，则比较整个字符串
        if (memcmp((void *)p, (void *)src, (size_t)strlen(src)) == 0)
            return p;  //如果找到，则返回内存地址。
        
					   //否则，指针后移查找下一个。
        p_buf = p + 1;
        cnt = count - (p_buf - buf);
    }
    return 0;
}


/*string STRING_Helper::UTF_8ToGB2312(const char *pText){
	string pOut;
	CChineseCode::UTF_8ToGB2312(pOut,pText);
	
	return pOut;
 }*/

/*wchar_t* STRING_Helper::AnsiToUnicode( const char* szStr )
 {
	int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0 );
	if (nLen == 0)
	{
 return NULL;
	}
	wchar_t* pResult = new wchar_t[nLen];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen );
	return pResult;
 }
 
 //———————————————————————————-
 // 将 宽字节wchar_t* 转换 单字节char*
 char* STRING_Helper::UnicodeToAnsi( const wchar_t* szStr )
 {
	int nLen = WideCharToMultiByte( CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL );
	if (nLen == 0)
	{
 return NULL;
	}
	char* pResult = new char[nLen];
	WideCharToMultiByte( CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL );
	return pResult;
 }
 
 //———————————————————————————-
 // 将单字符 string 转换为宽字符 wstring
 void STRING_Helper::Ascii2WideString( const std::string& szStr, std::wstring& wszStr )
 {
	int nLength = MultiByteToWideChar( CP_ACP, 0, szStr.c_str(), -1, NULL, NULL );
	wszStr.resize(nLength);
	LPWSTR lpwszStr = new wchar_t[nLength];
	MultiByteToWideChar( CP_ACP, 0, szStr.c_str(), -1, lpwszStr, nLength );
	wszStr = lpwszStr;
	delete [] lpwszStr;
 }*/
#ifdef _LY_CLR
std::string STRING_Helper::ConvertToStdString(System::String^ str)
{
    int q=(int)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(str);
    char* p=(char*)q;
    return std::string(p);
}

System::String^
STRING_Helper::ConvertToSysString(std::string& s)
{
    System::String ^temp;
    temp =  gcnew System::String(s.c_str());
    return temp;
}
#endif

//int CodeConvert(const char *from_charset, const char *to_charset, char *inbuf, int inlen, char *outbuf, int outlen);
