#include "stdafx.h"
#include "iostream"
#include "Linux_Win.h"
#include "time.h"
#include <stdio.h>
#include <ctime>
#include <algorithm>  
#include <functional>
#include <time.h>
#ifdef WIN32
	#include <direct.h>
	#include <windows.h>
	#include <Winsock2.h>

#else
	#include <sys/stat.h>
	#include <sys/time.h>
    #include <sys/ioctl.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <net/if.h>
#endif
#include<vector>
#include "string.h"
#include "errno.h"

/*#ifdef WIN32
     #define HT_LITTLE_ENDIAN
#else
	#include "endian.h"
	#if __BYTE_ORDER==__LITTLE_ENDIAN
		#define HT_LITTLE_ENDIAN
	#endif
#endif*/ //ly mac
using namespace std;

//获取本机IP地址
/*********************
 获取本地IP地址(CString)
 *********************/
#ifdef WIN32
    bool LW_GetHostAddress(string &strIPAddr)
    {
        char    HostName[512];
        cout << "gethostAddress b" << endl;
        gethostname(HostName, sizeof(HostName));// 获得本机主机名.
        
        hostent* hn;
        cout << "gethostAddress-gethostbyname()" << endl;
        hn = gethostbyname(HostName);//根据本机主机名得到本机ip
        cout << hn->h_name << endl;
        strIPAddr = inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]);//把ip换成字符串形式
        cout << "gethostAddress " << strIPAddr << endl;
        return true;
    }

    int LW_GetLocalHostName(string &sHostName) //获得本地计算机名称
    {
        char szHostName[512];
        int nRetCode;
        cout << "gethostname b" << endl;
        nRetCode = gethostname(szHostName, sizeof(szHostName));
        cout << "gethostname e" << endl;
        if (nRetCode != 0)
        {
            //产生错误
            sHostName = "";
            return GetLastError();
        }
        sHostName = szHostName;
        return 0;
    }
    bool  LW_GetLocalMac(string &Mac){
        Mac="";
        return false;
    }
#else
    bool LW_GetHostAddress(string& strIPAddr)
    {
        static vector<string> vNetType;//预先定义了几种可能的网卡类型
    #ifndef __APPLE__
        vNetType.push_back("eth");
    #else
        vNetType.push_back("en0");
    #endif
        for(size_t i=0;i<vNetType.size();i++)
        {
            for(char c='0';c<='9';++c)
            {
                string strDevice = vNetType[i] + c; //根据网卡类型，遍历设备如eth0，eth1
                int fd;
                struct ifreq ifr;
                //使用UDP协议建立无连接的服务
                fd = socket(AF_INET, SOCK_DGRAM, 0);
                strcpy(ifr.ifr_name, strDevice.c_str() );
                //获取IP地址
                if (ioctl(fd, SIOCGIFADDR, &ifr) <  0)
                {
                    ::close(fd);
                    continue;
                }
                
                // 将一个IP转换成一个互联网标准点分格式的字符串
                strIPAddr = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr);
                if(!strIPAddr.empty())
                {
                    ::close(fd);
                    return true;
                }
            }
        }
        return false;
    }
    #if !defined(__ANDROID__) && !defined(TARGET_OS_IPHONE) && !defined(TARGET_IPHONE_SIMULATOR)
        //取得MAC Address
        #ifndef __APPLE__
            const char SCRIPT[]    = "ifconfig eth0 | grep HWaddr|tr -s ' '|cut -d ' ' -f 5 > ./ipmac.txt";
        #else
            const char SCRIPT[]    = "ifconfig en0 | grep ether|tr -s ' '|cut -d ' ' -f 2 > ./ipmac.txt";
        #endif
        bool LW_GetLocalMac(string &Mac)
        {
            char pMac[36];
            const char FILE_NAME[] = "./ipmac.txt";
            FILE*       handle      = NULL;
            char*       delimiter   = NULL;
            
            if (system(SCRIPT) != 0 )
            {
                printf("Error: Execute command '%s'error: %s\n", SCRIPT, strerror(errno));
                goto FAILED;
            }
            
            if ( (handle = fopen(FILE_NAME, "r")) != NULL ){
                fgets(pMac, 18, handle);
                pMac[18]=0;pMac[19]=0;
                //printf("mac:%s",pMac);
            }else
            {
                printf("Error: Open file %s error: %s\n", FILE_NAME, strerror(errno));
                goto FAILED;
            }
            fclose(handle);
            if ( (delimiter = strchr(pMac, '\x0a')) != NULL )
                *delimiter = '\0';
            unlink(FILE_NAME);
            Mac=pMac;
            return 0;
            
        FAILED:
            unlink(FILE_NAME);
            return -1;
        }
    #else
        bool LW_GetLocalMac(string& mac)
        {
            vector<string> vs;//预先定义了几种可能的网卡类型
            vs.push_back("eth");
            vs.push_back("em");
            vs.push_back("oct");

            for(size_t i=0;i<vNetType.size();i++)
            {
                for(char c='0';c<='9';++c)
                {
                    string strDevice = vNetType[i] + c; //根据网卡类型，遍历设备如eth0，eth1
                    int fd;
                    struct ifreq ifr;
                    //使用UDP协议建立无连接的服务
                    fd = socket(AF_INET, SOCK_DGRAM, 0);
                    if (ioctl(fd, SIOCGIFMAC, &ifr) <  0)
                    {
                        ::close(fd);
                        continue;
                    }
                    unsigned char macaddr[6];
                    memcpy(macaddr,ifr.ifr_hwaddr.sa_data,6);
                    for(int k=0;k<6;k++)
                    {
                        sprintf(mac,"%02x",macaddr[k]);
                        if(k<5)
                            sprintf(mac,":");
                    }
                    return true;
                }
            }
            return false;
        }
    #endif
#endif

//for win32->_mkdir; linux->mkdir的兼容
int LW_mkdir (const char *filename, unsigned int mode){
	#ifdef WIN32
		return _mkdir(filename);
	#else
		return mkdir(filename,(mode_t)mode);
	#endif
}

int LW_del (const char *filename){//ly !!!test
#ifdef WIN32
	//获得所需的宽字符数组空间
	DWORD buffer_len = MultiByteToWideChar(CP_ACP, 0, filename, -1, NULL, 0);
	wchar_t *buffer = new wchar_t[buffer_len];//分配响应的数组空间码
	if (!buffer)
		delete[]buffer;
	if (buffer) {
		MultiByteToWideChar(CP_ACP, 0, filename, -1, buffer, buffer_len);//转换为ASCI码
		BOOL del = false;
		DWORD dwAttr = GetFileAttributes(buffer);
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) {
			del=RemoveDirectory(buffer);
		}else
			del=DeleteFile(buffer);//删除文件
		delete[]buffer;
		if (!del) {
			DWORD error_rus = GetLastError();
			return error_rus;
		}
	}
    return 0;
#else
    return remove(filename);
#endif
}

int LW_rename(const char *src_name,const char *dest_name){
   return rename(src_name,dest_name);
}
/*
int LW_access (const char *filename, unsigned int mode){
	#ifdef WIN32
		return _access(filename,mode);
	#else
		return access(filename,(mode_t)mode);
	#endif
}*/

//对原ACE::OS函数的处理
#ifdef ACE
	void SAFE_FREE(void * a){
		ACE::free(a);
	}
#else
	void SAFE_FREE(void * a){
		free(a);
	}
#endif

//Win32 中的::GetTickCount()函数；
#ifdef	WIN32
	//#define LW_GetTickCount() ::GetTickCount()

	uint64_t LW_GetTickCount(){
		//原来是：::GetTickCount();
		return ::GetTickCount();
	}
	int LW_mkdir(const char *path){
		return _mkdir(path);
	}
#else
	uint64_t LW_GetTickCount(){
		struct timeval tv;  
		gettimeofday(&tv,NULL);
		return tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}
	int LW_mkdir(const char *path){
		return mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
	char *strupr(char *str) {
		char *orign = str;
		for (; *str != '\0'; str++)
			*str = toupper(*str);
		return orign;
	}

#endif

#ifdef WIN32
	//sprintf(time_str, "%d", time(NULL));
	#define LY_ACCESS(a,b) _access(a,b)
#else
	#define LY_ACCESS(a,b) access(a,b)
	//itoa is not availed in win32 vc++
	/*void _itoa ( unsigned long val, char *buf, unsigned radix ){
		itoa(val,buf,radix);
	}*/
	void itoa ( unsigned long val, char *buf, unsigned radix )
	{
	        char *p;                /* pointer to traverse string */
	        char *firstdig;         /* pointer to first digit */
	        char temp;              /* temp char */
	        unsigned digval;        /* value of digit */

	        p = buf;
	        firstdig = p;           /* save pointer to first digit */

	        do {
	            digval = (unsigned) (val % radix);
	            val /= radix;       /* get next digit */

	            /* convert to ascii and store */
	            if (digval > 9)
	                *p++ = (char ) (digval - 10 + 'a');  /* a letter */
	            else
	                *p++ = (char ) (digval + '0');       /* a digit */
	        } while (val > 0);

	        /* We now have the digit of the number in the buffer, but in reverse
	           order.  Thus we reverse them now. */

	        *p-- = '\0';            /* terminate string; p points to last digit */

	        do {
	            temp = *p;
	            *p = *firstdig;
	            *firstdig = temp;   /* swap *p and *firstdig */
	            --p;
	            ++firstdig;         /* advance to next two digits */
	        } while (firstdig < p); /* repeat until halfway */
	}

	bool IsBig_Endian()
	//如果字节序为big-endian，返回true;
	//反之为little-endian，返回false
	{
	     unsigned short test = 0x1122;
	    if(*( (unsigned char*) &test ) == 0x11)
	        return TRUE;
	    else
	        return FALSE;

	}//IsBig_Endian()

#endif

