/*
 * Copyright (c) 2010 Putilov Andrey
 * Copyright (c) 2012 Felipe Cruz
 *
 * Permission is hereby granted, free of uint8_tge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef WizBase_H
#define WizBase_H
#include "ObjectProcess.h"
#include "AvartarDBManager.h"
#include "ConnQueue.h"

#ifdef __ANDROID__
    #include <android/log.h>
    #define TAG "ly_test"
    #define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
    #define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
    #define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
    #define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
    #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#endif

//#ifdef  __cplusplus
//extern "C" {
//#endif

//路由表头结构
#define WIZBASE_PATH_TYPE_URL_PATH 1
#define WIZBASE_PATH_TYPE_MAC_PATH 2
#define WIZBASE_NUM_THREADS    2
#define WIZBASE_NUM_PORTS    2
#define WIZBASE_MAX_CONN_THREAD 5000
#define WIZBASE_MAX_CONN_THREAD_WIDTH 4
#define WIZBASE_SSL_TYPE_NOSSL	0
#define WIZBASE_SSL_TYPE_OPENSSL 1
#define WIZBASE_SSL_TYPE_EVENTSSL 2
#define WIZBASE_SSL_TYPE_CFGSSL 3

//#define WIZBASE_PATH_TYPE_URL_PATH
struct routing_header_def{
	char check_str[3];//@~@
	UInt32 length;//整个数据区长度
	UInt16 packetID; //数据包标识,用于区别连续的数据包
	byte type;//类型01：***.***.**** 类型02:MAC(UInt64)+MAC+MAC
	UInt16 dest_point;//dest 起点：路径区第几(或)指针
	//UInt16 src_point;
	UInt16 value_point; //内容长度指针
	//UInt16 act_point; //动作长度指针
};
/*typedef struct{
	evutil_socket_t fd;
	struct buffevent *bev;
	ConnSSL *conn_ssl;
}protcal_obj;*/

struct routing_header_def_str{
	char check_str[3];//@~@
	BYTE length[4];//整个数据区长度
	BYTE packetID[2]; //数据包标识,用于区别连续的数据包
	byte type;//类型01：***.***.**** 类型02:MAC(UInt64)+MAC+MAC
	BYTE dest_point[2];//dest 起点：路径区第几(或)指针
	//UInt16 src_point;
	BYTE value_point[2]; //内容长度指针
	//BYTE act_point[2]; //动作长度指针
};

bool WIZBASE_create_routing_packet(string &buffer,int path_type,string &dest,string &value,UInt16 dest_point);
//ly_send_queue
extern void WIZBASE_conn_disconnect(Conn *conn);
extern string WIZBASE_create_connect_obj(const char *obj_path_name,  string &conn_str1, map<string, string> *tmp_add_paras=NULL,const char *dest_host=NULL,const char *new_obj_name=NULL);
extern void *WIZBASE_RunLibEvent(void *arg);
extern void *WIZBASE_RunUDP(void *arg);
extern int WIZBASE_SysInitGBParams(const char *c_ports, const char*data_path, const char *work_path, const char* dbconfig_xml, const char *gateway_xml, const char *node_name, const char  *sys_cert,  const char *sensor_dbm);
extern int WIZBASE_RunGBParams(int argc, char **argv);
extern void WIZBASE_DEBUG(const char*);
extern void WIZBASE_ERROR(string str);
extern void WIZBASE_SHOW_ERROR(const char*);
extern int WIZBASE_StartQueueTimeTask(Conn *conn);
extern void WIZBASE_CancleQueueTimeTask(Conn *conn);

//#ifdef  __cplusplus
//}
//#endif
//#ifdef  __cplusplus
//extern "C" {
//#endif

extern int GB_Ports[WIZBASE_NUM_PORTS]; //ly new pthread
extern int GB_Ports_NUMBER;
extern ObjectProcess *GB_Gateway;
extern AvartarDBManager *GB_AvartarDB;
extern Session *GB_SessionBuf; //临时存放在ConnGueue.h中，今后应移开
extern Http_ParseXML *GB_ParseXml;
extern string GB_WorkPath;
extern ConnQueue *GB_ConnQueue;  //主要用于长连接
//extern PrioQueue *GB_PrioQueue;//主要用于公共任务队列

//#ifdef  __cplusplus
//}
//#endif

/*typedef int WizBase_pthread_mutex_t;
WizBase_pthread_mutex_t GB_Lock = 0;
extern WizBase_pthread_mutex_lock(WizBase_pthread_mutex_t *mylock);
extern WizBase_pthread_mutex_unlock(WizBase_pthread_mutex_t *mylock);*/

#endif  /* CWS_H */
