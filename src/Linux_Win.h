//这是兼容linux windows二者的各种特殊性的描述场所
#ifndef LINUX_WIN_H
#define LINUX_WIN_H
#include "stdlib.h"
#include <string>
using namespace std;

#define NO_CCODE
//#define __APPLE__ //ly mac
//#define OPEN_WRT
//#define SENSOR_DBM
//#pragma GCC diagnostic ignored "-Wxxxx"
#ifdef WIN32
#else
//#pragma GCC diagnostic ignored "-Wuninitialized"
//#pragma GCC diagnostic ignored "-Wunused-but-set-variable"  //ly mac
//#pragma GCC diagnostic ignored "-Wunused-variable" //ly mac
#endif

#ifdef WIN32
    #define HT_LITTLE_ENDIAN
#else
    #include "unistd.h" //当前工作目录操作等
    #ifdef __APPLE__  //ly mac
        #ifdef TARGET_OS_IPHONE
            // iOS
        #elif TARGET_IPHONE_SIMULATOR
            // iOS Simulator
        #elif TARGET_OS_MAC
            // Other kinds of Mac OS
            #define HT_BIG_ENDIAN
        #else
            // Unsupported platform
        #endif
    #else
        #include "endian.h"
        #if __BYTE_ORDER==__LITTLE_ENDIAN
            #define HT_LITTLE_ENDIAN
        #endif
    #endif
#endif

#define LW_BIT_STR "bit"
#define LW_BINARY_STR "char"
#define LW_HEX_STR "hex"

//long int defined
typedef unsigned char   byte;
typedef unsigned char   UInt8;
typedef unsigned int	UInt32;
typedef unsigned short int	UInt16;

typedef signed char     int8_t;
typedef short int       int16_t;
typedef int             int32_t;
typedef unsigned char   BYTE;

# if __WORDSIZE == 64
#if !defined(__APPLE__)
typedef long int int64_t;
typedef unsigned long int uint64_t;

#endif
typedef unsigned long long int UInt64;
typedef unsigned int ULONG;
typedef	UInt16 UINT;
typedef	int BOOL;
# else
#ifdef WIN32
typedef long long int   int64_t;
#else
__extension__
typedef long long int   int64_t;
typedef	UInt32			ULONG;
typedef	UInt16			UINT;
typedef	int			BOOL;
#endif
typedef unsigned long long int uint64_t;
typedef unsigned long long int UInt64;
#endif
typedef UInt64 uint64;

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

//typedef	UInt8			BYTE;


#define itoa _itoa
#ifdef VS2010
#define strdup _strdup
#endif
#ifdef ACE
void SAFE_FREE(void * a);
#else
void SAFE_FREE(void * a);
#endif

#ifdef WIN32
#ifdef _LY_CLR
#include <\Program Files\Microsoft Visual Studio 8\VC\include\io.h> // for _access() function only
#else
#include <io.h> // for _access() function only
#endif
#include <direct.h>
#include <windows.h>
#define LW_access(a,b) _access(a,b)
#define LW_NULL NULL;
//#define LW_mkdir(a,b) _mkdir(a,b)

#else
//#if !defined(OPEN_WRT) && !defined(__ANDROID__) && !defined(__APPLE__)
#if !defined(OPEN_WRT) && !defined(__ANDROID__)
    #include <iconv.h>  //ly table_dir
#endif
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#define LW_access(a,b) access(a,b)
//#define LW_mkdir(a,b) mkdir(a,b)
#define LW_NULL 0;
extern char *strupr(char *str);
#endif

//针对Windows: ACE_OS::free方便调试的处理。
/*#ifdef ACE
 #
	#else
 #define SAFE_FREE(a) free(a)
	#endif*/
extern void itoa(unsigned long val, char *buf, unsigned radix);
extern int LW_mkdir(const char *filename, unsigned int mode);
extern int LW_del(const char *filename);
extern int LW_rename(const char *src_name,const char *dest_name);
extern int LW_access(const char *filename, unsigned int mode);
extern uint64_t LW_GetTickCount();
extern int LW_mkdir(const char *path);
bool LW_GetHostAddress(string &strIPAddr);
int LW_GetLocalHostName(string &sHostName);
bool  LW_GetLocalMac(string &Mac);
#endif
