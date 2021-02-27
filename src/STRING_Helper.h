#include "stdafx.h"
#pragma once
#include <string>
#include <vector>
#include "Linux_Win.h"
using namespace std; 
//#define INGNaN 2294967295UL

class STRING_Helper
{
public:
    enum {//LY 2013-1-28
        NOT_FIND = 0,
        FINDED = 1,
        NaN = 2294967295UL,
        SKIP_BLOCK = 1
    };
    STRING_Helper(void);
    ~STRING_Helper(void);
    
    //New Add String
    static int strVector_GetNext(const char *src, string &dest_str, char * &next_point, string para);
    static char *_strVector_TempStrFind(const char *src, const char *find);
    static bool strArrayToBinary(string & dest,const char *src, int byte_width, string split_char);
    static char *HTTP_decode_string (char *path);
    static int _StrLen(string &s);
    static bool _IsNull(string &obj);
    static bool CheckAllChar(string &obj,char ch);
    static bool _IsNull(const char *obj);
    static bool _IsNull(int len);
    static char _CharAt(string &s,int pos);
    static string _SubStr(string &s,int pos,int len);
    static int PUB_FindStr(const char *str,const char*s);
    static int PUB_FindStr(string &str,string s);
    static int PUB_FindStr(string &str,const char*s);
    static int PUB_FindStr(string &str,string s,bool from_end);
    static bool PUB_FindStrIs(string &str,const char*s);
    static bool PUB_FindStrIs(string &str,string s);
    static int C_FindFirstStr(char *src, char *pos, vector<string> &find, int len = 0);
    static int FindFirstStr(string src_str, string find_str, const char *split, bool skip_block=true);
    static int C_CmpFirstStr(char *src, int &start_pos, const char *find_str, const char *split,int len, bool skip_bool);
    static char *C_StrStrLen(const char *src, const char *find, size_t len=0);
    static int C_StrCmpLen(char * s1, char * s2, size_t n=0);
    static string _StrAdd(string &s,const char *add_str,bool from);
    static string SetNumToID(string s,int len, const char*split=NULL);
    static bool CheckNum(string &num_str);
    static bool CheckNum(string &num_str, const char *Radix);
    static bool CheckNum(string &num_str, const char *Radix, int len);
    static bool CheckNum(const char *s);
    static int ParseInt(const char *s);
    static double ParseFload(const char *s);
    static double ParseFload(string s);
    static int ParseInt(string s);
    static int ParseInt(string s, int radix);
    //static bool ParseULong(unsigned long &d,string s);
    static unsigned long ParseULong(string s, int radix=0);
    static string iToStr(int num,int radix);
    static string iToStr(int num);
    static string uToStr(size_t num);
    static string uToHex(unsigned int num,BYTE width);
    static int StrToi(string str);
    static double StrTof(string str);
    //template <typename T> string ToStr(T val);
    static string dToStr(double num);
    static string fToStr(float num);
    static string iToChar(int num);
    static string HBCStrToHBCStr(const char *temp, const char *src_hbc, const char *dest_hbc, bool add_head);
    static string HBCStrToHBCStr(string &temp, string &src_hbc, const char *dest_hbc, bool add_head);
    static string HBCStrToHBCStr(string &temp, const char* src_hbc, string &dest_hbc, bool add_head);
    static string HBCStrToHBCStr(string &temp, const char* src_hbc, const char *dest_hbc, bool add_head);
    static string HBCStrToHBCStr(string &temp, string &src_hbc, string &dest_hbc, bool add_head);
    static string BinaryToHBCStr(const char* data,int data_len, const char *dest_hbc, bool add_head);
    static string BinaryToHBCStr(const char* data,int data_len, string &dest_hbc, bool add_head);
    //static string BinaryToIPStr(string &ip);
    //static string IPStrToBinary(string &)
    static std::string strRandTime(void);
    static void strRandTime(std::string &ret_str);
    static string& strTrim(std::string &s);
    static string& strTrim(std::string &s,const char *);
    //char*  STRING_Helper::strTrim(char* s);
    static string&  LeftTrim(string   &str);
    static string&  LowerCase(string   &str);
    static string&  UpperCase(string   &str);
    static string&  RightTrim(string   &str);
    static string&  Trim(string   &str);
    static string UInt8ToBinaryStr(UInt8 u);
    static string UInt16ToBinaryStr(UInt16 u);
    static string UInt32ToBinaryStr(UInt32 u);
    static string UInt64ToBinaryStr(UInt64 u);
    static UInt32 BinaryStrToUInt32(string &in_str);
    static UInt64 GetNowTimeSec();
    static void GetNowTime(std::string &ret_str);
    static void GetNowTime(std::string &ret_str,const char * split_str);
    static void GetNowDate(std::string &ret_str);
    static void GetNowDate(std::string &ret_str,const char * split_str);
    static void stringSplit(std::string &content,vector<std::string> &vecSubContent,std::string split_str, bool skip_null_str=true);
    static void stringSplit(const char *content,vector<std::string> &vecSubContent,std::string split_str);
    static int strStringCut(std::string src_str,std::string &result, std::string head, std::string end);
    static int strStringCutFrom(std::string src_str,std::string &result, std::string head, std::string end, size_t start);
    static bool strFastCut(std::string &src_str, std::string head, std::string end, size_t start);
    static int strStringCutField(std::string src_str,std::string &result, std::string head, std::string end);
    static bool strStringCutUrlFileName(std::string url,std::string &result,const char *part);
    static bool strStringCutUrlFileName(std::string url,std::string &result);
    static void strReplace(string &src_str,char old_char, char new_char);
    static void strReplace(string &src_str,const char *old_str, const char *new_str);
    static void strReplace(string &src_str,string old_str, string new_str);
    static void strReplace(string &src_str,string old_str, const char *new_str,UInt64 new_str_size);
    static void strReplaceAll(string &src_str,string old_str, string new_str);
    static void HashNumStrToID(const char* s,string &out_s);
    static void strRandTime_Us(std::string &ret_str);
    static bool MatchStr2Hex(string &src, string match);
    static void HTTP_UnEscape(const char * inputsEsc, std::string & strUnEsc);
    static void HTTP_EscString(const char *EscStr, std::string & strTo);
    static void HTTP_UnEscString(const char *EscStr, std::string & strUnEscStr);
    static bool HTTP_IsUTF8(char* str, UInt64 length);
    static void UTF_8ToGB2312(std::string &pOut, const char *pText);//UTF-8 转为 GB2312
    static void GB2312ToUTF_8(std::string &pOut, const char *pText);//UTF-8 转为 GB2312
#ifdef WIN32
	static wchar_t *StringToLPCWSTR(string change_string);
    static void UTF_8ToUnicode(wchar_t* pOut, const char *pText);  // 把UTF-8转换成Unicode
    static void UnicodeToUTF_8(char* pOut, wchar_t* pText);  //Unicode 转换成UTF-8
    static void UnicodeToGB2312(char* pOut, wchar_t uData);  // 把Unicode 转换成 GB2312
    static void Gb2312ToUnicode(wchar_t* pOut, char *gbBuffer);// GB2312 转换成　Unicode
#else
    static int CodeConvert(const char *from_charset, const char *to_charset, const char *inbuf, int inlen, char *outbuf, int outlen);// GB2312 转换成　Unicode
#endif
    
    /*=============================================================================
     ; Function:	putUINT32(pBuf, offset, u32value)
     ; Abstract:	insert 32-bit unsigned integet into the indicated buffer @ offset
     ; Returns:	Number of bytes copied
     ; Inputs:	pBuf:		-> destination buffer
     ;			offset:		buffer offset
     ;			u32value:	value to be written (always MSByte first)
     ==============================================================================*/
    static void putUINTToStr(string &p_buf,uint64 u64value);
    static string putUINTToStr(uint64 u64value,BYTE bit_num);
    static BYTE putUINT16(BYTE *pBuf, size_t offset, ULONG u16value);
    static BYTE putUINT32(BYTE *pBuf, size_t offset, ULONG u32value);
    static BYTE putUINT64(BYTE *pBuf, size_t offset, uint64 u64value);
    static UInt16 scanUINT16(BYTE *pBuf, size_t offset);
    static UInt32 scanUINT32(BYTE *pBuf, size_t offset);
    static UInt64 scanUINT64(BYTE *pBuf, size_t offset);
    static UInt64 scanUINTStr(string buf);
    static UInt64 scanUINTIDStr(string &buf,BYTE cell_width);
    static void  *pMemStrPos(char *buf, char * src, unsigned int count);
    //static UInt64 scanUINTStr(string buf);
    
    //not compact Linux
    //static string UTF_8ToGB2312(const char *pText);
    /*static wchar_t* AnsiToUnicode( const char* szStr );
     static char* UnicodeToAnsi( const wchar_t* szStr );
     static void Ascii2WideString( const std::string& szStr, std::wstring& wszStr );*/
    template <typename T> static string ToStr(T val);
    //static string S1;
    
#ifdef _LY_CLR
    static std::string ConvertToStdString(System::String^ str);
    static System::String^ ConvertToSysString(std::string& s);
#endif
private:
};

