//============================================================================
// Name        : WizBase.cpp
// Author      : liyang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//===========================================================================
#include <sys/types.h>
#include "stdafx.h"
#include <iostream>
#include "STRING_Helper.h"
//#include "HTTP_Config.h"
#include "stdlib.h"
#include "stdio.h"
#ifdef WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #include <io.h>
    #include <fcntl.h>
    #ifndef S_ISDIR
    #define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
    #endif
#else
	#include <arpa/inet.h>
    #include <sys/stat.h>
    #include <sys/socket.h>
    #include <signal.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <dirent.h>
#endif

#include <event2/dns.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>
#include <event2/buffer.h>
#ifdef _EVENT_HAVE_NETINET_IN_H
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#endif
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "ConnQueue.h"
#include "ConnSSL.h"
#ifdef SENSOR_DBM
#include "XMLService_BerkeleyDB.h"
#endif
#include "ObjectProcess.h"
#include "ParseProcess.h"
#include "ParseBase.h"

using namespace std;
#include "WizBase.h"
#include "AvartarDBManager.h"
#include "ConnSSL.h"

#ifdef THIRD_QQ
    #include "ThirdQQ.h"
#endif
#ifdef __APPLE__
    #include "HDCCallObj.h"
#endif
//vector <event_base *>GB_EventBase;
struct event_base *GB_EventBase[WIZBASE_NUM_THREADS];
ObjectProcess *GB_Gateway; //全局Gateway主要用于短连接HTTP
Http_ParseXML *GB_ParseXml;  //ly obj_new
ConnQueue *GB_ConnQueue;  //主要用于长连接
//TaskQueueCell_T
//PrioQueue *GB_PrioQueue;//主要用于公共任务队列
Session *GB_SessionBuf;
string GB_ParseCfg;
string GB_ThisNodeName;
string GB_HpptsClientCert;
string GB_WorkPath;
string GB_LocalHostName("");
string GB_LocalHostMac("");
string GB_LocalHostAddr("");
AvartarDBManager *GB_AvartarDB;
ConnSSL *GB_SRC_SCTX=NULL; //面向全局的SSL环境。
ConnSSL *GB_CLI_SCTX=NULL; //面向全局的SSL环境。 ly memleak2
struct evdns_base *GB_DNS_BASE; //

//int GB_Ports[WIZBASE_NUM_THREADS];
int GB_Ports[WIZBASE_NUM_PORTS]; //= 8090;  //ly new pthread
int GB_Ports_NUMBER;
#ifdef WIZ_THREAD
    typedef pthread_mutex_t WizBase_pthread_mutex_t;
    WizBase_pthread_mutex_t GB_Lock=PTHREAD_MUTEX_INITIALIZER;
#else
	typedef int WizBase_pthread_mutex_t;
    WizBase_pthread_mutex_t GB_Lock = 0;
#endif
int WizBase_pthread_mutex_lock(WizBase_pthread_mutex_t *mylock) {
#ifdef WIZ_THREAD
    #ifndef WIN32
        return pthread_mutex_lock(mylock);
    #endif
#endif
	return 0;
}

int WizBase_pthread_mutex_unlock(WizBase_pthread_mutex_t *mylock) {
#ifdef WIZ_THREAD
    #ifndef WIN32
        return pthread_mutex_unlock(mylock);
    #endif
#endif
	return 0;
}

#ifdef THIRD_QQ
map<string, ThirdBase*> GB_ThirdMap;
#endif

#define CONFIGXML "./jawsconfig.ini" //redefined to HTTP_Config.h
#define GATEWAY_XML "Gateway.xml"
//#define GATEWAY_XML "Serial_image.xml"
//#define GATEWAY_XML "Serial.xml"
#define Default_Time_Routin 60
#define Default_Conn_Routin 20
#define MAX_TIME_ARRAY 100
#ifdef SENSOR_DBM
#define SENSOR_DB_PATH "sense_db.xml"
#endif

static bool util_get_ipaddr(struct sockaddr *address, int socklen, Conn *conn,
		string &ip_addr, string &ip_port);
static int obj_send_cb2(const char *msg, int msg_len, const char *dest_obj,
		const char *src, int trans_type, bool close_after_send, void *ctx);
static void echo_read_cb(struct bufferevent *bev, void *ctx);
static void echo_write_cb(struct bufferevent *bev, void *ctx);
static void set_time_limit(int *prc_num);
static void set_conn_limit();
static void conn_err_node(struct bufferevent *bev,void *ctx,string &err_str);
static void conn_disconnect(struct bufferevent *bev, void *ctx);
static void accept_conn_cb(struct evconnlistener *listener,evutil_socket_t fd, struct sockaddr *address, int socklen, void *arg);

void WIZBASE_DEBUG(string str){
    WIZBASE_DEBUG(str.c_str());
}

void WIZBASE_DEBUG(const char* str){
#ifdef __APPLE__
    #if TARGET_IPHONE_SIMULATOR
        GHDC_CallObj->CallNSFunc("NSLog",str,strlen(str));
    #elif TARGET_OS_IPHONE
        GHDC_CallObj->CallNSFunc("NSLog",str,strlen(str));
    #else
        printf("%s\n",str);
    #endif
#else
#ifdef __ANDROID__
    LOGD("%s",str);
#else
    printf("%s\n",str);
	fflush(stdout);
#endif
#endif
}
void WIZBASE_ERROR(string str){
    WIZBASE_SHOW_ERROR(str.c_str());
}

void WIZBASE_SHOW_ERROR(const char* str){
#ifdef __APPLE__
    #if TARGET_IPHONE_SIMULATOR
        GHDC_CallObj->CallNSFunc("NSLog",str,strlen(str));
    #elif TARGET_OS_IPHONE
        GHDC_CallObj->CallNSFunc("NSLog",str,strlen(str));
    #else
        printf("%s\n",str);
    #endif
#else
#ifdef __ANDROID__
    LOGD("%s",str);
#else
    printf("%s\n",str);
#endif
#endif
}

//测试大头、小头
int IsLittleEndian() {
	unsigned int usData = 0x12345678;
	unsigned char *pucData = (unsigned char*) &usData;

	if (*pucData == 0x78) {
		return 1;
	} else {
		return 0;
	}

}

void init_do_process_param(Conn *conn) {

}
void after_do_process_param(Conn *conn) {

}

void event_connect_cb(struct bufferevent *bev, short events, void *ctx) {
	Conn *conn = (Conn*) ctx;  //what use?
//#define BEV_EVENT_READING	0x01	//*< error encountered while reading */
//#define BEV_EVENT_WRITING	0x02	//*< error encountered while writing */
//#define BEV_EVENT_EOF		0x10	//*< eof file reached */
//#define BEV_EVENT_ERROR		0x20	//*< unrecoverable error encountered */
//#define BEV_EVENT_TIMEOUT	0x40	//*< user-specified timeout reached */
//#define BEV_EVENT_CONNECTED	0x80	//*< connect operation finished. */
    //cout << "*"; //libevent test
	if (events & BEV_EVENT_CONNECTED) {
		//如果成功连接：打开并运行CONNECT/SKHD_ACT
		//WIZBASE_DEBUG("Connect okay.");
		conn->InitProcessObj(conn->obj_name.c_str());
		conn->is_act = true;
		if (conn->ProcessObj->IsReady()) {
			string act_type = (string) "/XMLParse/Connect/CONNECT[@Name='"
					+ conn->obj_name_orign + "']/SKHD_ACT";
			conn->ProcessObj->SetParam("@_SRC_IP", conn->ip_addr.c_str(),
					Http_ParseObj::STR_COPY_OBJ);  //初始化相关req值。
			conn->ProcessObj->SetParam("@_SRC_PORT", conn->ip_port.c_str(),
					Http_ParseObj::STR_COPY_OBJ);  //初始化相关req值。
			conn->ProcessObj->SetParam("@_THIS_NODE_NAME",
					GB_ThisNodeName.c_str(), Http_ParseObj::STR_COPY_OBJ); //初始化相关req值。
			conn->ProcessObj->SetParam("@_LOCAL_HOST_NAME",
				GB_LocalHostName.c_str(), Http_ParseObj::STR_COPY_OBJ); //初始化相关req值。
            conn->ProcessObj->SetParam("@_LOCAL_HOST_MAC",
                                       GB_LocalHostMac.c_str(), Http_ParseObj::STR_COPY_OBJ); //初始化相关req值。
			conn->ProcessObj->SetParam("@_LOCAL_HOST_ADDR",
				GB_LocalHostAddr.c_str(), Http_ParseObj::STR_COPY_OBJ); //初始化相关req值。
			conn->ProcessObj->SetParam("@_CONN_NAME", conn->obj_name.c_str(),
					Http_ParseObj::STR_COPY_OBJ);  //初始化相关req值。
			conn->SetActive(true);
			conn->ProcessObj->DoProcess(act_type.c_str(), obj_send_cb2,
					(void *) conn); 
			after_do_process_param(conn);
		}
	} else if (events & (BEV_EVENT_ERROR | BEV_EVENT_EOF | BEV_EVENT_TIMEOUT)) {//ly_send_queue BEV_EVENT_TIMEOUT新增
		//如果失败：关闭运行CONNECT,删除对象
        string err_str="";
        if (events & BEV_EVENT_ERROR) {
            err_str="BEV_EVENT_ERROR";
			int err = bufferevent_socket_get_dns_error(bev);
			if (err)
                err_str+=(string)" DNS error:"+evutil_gai_strerror(err);
		}else if(events & BEV_EVENT_EOF){
			err_str="BEV_EVENT_EOF";
		} else if (events & BEV_EVENT_TIMEOUT){
			err_str="BEV_EVENT_TIMEOUT";
		}
        WIZBASE_ERROR((string)"Error("+err_str+"),Closing at connection!");
		conn_err_node(bev,conn,err_str); //ly_send_queue
		conn_disconnect(bev, conn);
	}else{
        WIZBASE_ERROR((string)"Error:event_connect_cb(): unrecgnize event!");
	}
}

//时间中断
static Conn *do_conntion_obj(string &msg, const char *dest_host,const char *obj_name, const char *obj_name_orign,int ssl_type,ParseXml_Node cfg_node) { //ly ssl
	struct evdns_base *dns_base;
	struct bufferevent *bev=NULL; //,*old_bev=NULL;
	ConnSSL *conn_ssl=NULL;

    WIZBASE_DEBUG((string)"Start connect " +dest_host +" at port_obj-" +obj_name);
	//host parse
	string dest_ip_str(dest_host);
	string ip_addr=dest_ip_str.substr(0,dest_ip_str.find(":"));
	string ip_port=dest_ip_str.substr(dest_ip_str.find(":")+1);
	int ip_port_val = STRING_Helper::StrToi(ip_port);

	//init
	if (GB_DNS_BASE==NULL)
		GB_DNS_BASE=evdns_base_new(GB_EventBase[0], 1); //ly memleak2
	//dns_base = evdns_base_new(GB_EventBase[0], 1);
	dns_base=GB_DNS_BASE;
	switch(ssl_type){
		case WIZBASE_SSL_TYPE_NOSSL:
			bev = bufferevent_socket_new(GB_EventBase[0], -1, BEV_OPT_CLOSE_ON_FREE);
			break;
		case WIZBASE_SSL_TYPE_OPENSSL: //this openssl is not saft;see bufferevent_options() in bufferevent.h
			//bev = GB_CLI_SCTX->bufferevent_ssl_socket_new(GB_EventBase[0], -1,ConnSSL::CONN_SSL_CONNECTING, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS,dest_host);
			//ly memleak2 del
			//conn_ssl = new ConnSSL(ConnSSL::CONN_SSL_CTX_METHOD_CLIENT_HTTPS,ip_addr.c_str(),GB_HpptsClientCert.c_str());
			//if (conn_ssl->ssl==NULL){
			//	WIZBASE_DEBUG((string)"do_conntion_obj->WIZBASE_SSL_TYPE_OPENSSL:ConnSSL() failed\n");
			//	return NULL;
			//}
			//bev = conn_ssl->bufferevent_ssl_socket_new(GB_EventBase[0], -1,ConnSSL::CONN_SSL_CONNECTING, dest_host);
			//ly memleak2 add
			bev = GB_CLI_SCTX->bufferevent_ssl_socket_new(GB_EventBase[0], -1,ConnSSL::CONN_SSL_CONNECTING, dest_host);
			break;
#if !defined(OPEN_WRT) && !defined(__ANDROID__) && !defined(__APPLE__)
		case WIZBASE_SSL_TYPE_CFGSSL: //this openssl is not saft;see bufferevent_options() in bufferevent.h
			//bev = GB_CLI_SCTX->bufferevent_ssl_socket_new(GB_EventBase[0], -1,ConnSSL::CONN_SSL_CONNECTING, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS,dest_host);
			//ly memleak2;
			/*conn_ssl = new ConnSSL(ConnSSL::CONN_SSL_CTX_METHOD_CLIENT,ip_addr.c_str(),cfg_node);
			if (conn_ssl->ssl==NULL){
				printf("do_conntion_obj->WIZBASE_SSL_TYPE_OPENSSL:ConnSSL() failed\n");
				return NULL;
			}
			bev = conn_ssl->bufferevent_ssl_socket_new(GB_EventBase[0], -1,ConnSSL::CONN_SSL_CONNECTING, dest_host);*/
            //ly memleak2 add
			bev = GB_CLI_SCTX->bufferevent_ssl_socket_new(GB_EventBase[0], -1,ConnSSL::CONN_SSL_CONNECTING, dest_host);
			break;
#endif
		case WIZBASE_SSL_TYPE_EVENTSSL:
			bev = bufferevent_socket_new(GB_EventBase[0], -1, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);//filter process
			break;
	}
	if (bev == NULL){
		WIZBASE_DEBUG((string)"bufferevent_openssl_socket_new() failed\n");
		return NULL;
	}
	//active bufferevent connect
	if (bufferevent_socket_connect_hostname(bev, dns_base, AF_UNSPEC,ip_addr.c_str(), ip_port_val)<0){
		WIZBASE_DEBUG((string)"bufferevent_socket_connect_hostname() failed!");
		return NULL;
	}

	//filter type bufferevent new() process!
	if (ssl_type==WIZBASE_SSL_TYPE_EVENTSSL){
		conn_ssl = new ConnSSL(ConnSSL::CONN_SSL_CTX_METHOD_CLIENT_HTTPS,ip_addr.c_str(),GB_HpptsClientCert.c_str());
		if (conn_ssl->ssl==NULL){
			WIZBASE_DEBUG((string)"do_conntion_obj->WIZBASE_SSL_TYPE_EVENTSSL:ConnSSL() failed\n");
			return NULL;
		}
		//struct bufferevent *b_ssl = conn_ssl->bufferevent_ssl_filter_new(GB_EventBase[0],bev, ConnSSL::CONN_SSL_CONNECTING);
		struct bufferevent *old_bev=bev;
		struct bufferevent *bev = conn_ssl->bufferevent_ssl_filter_new(GB_EventBase[0],old_bev, ConnSSL::CONN_SSL_CONNECTING);
		if (!bev) {
			WIZBASE_DEBUG((string)"conn_ssl->bufferevent_openssl_filter_new() failed");
			return NULL;
		}
	}
	//初始化对象
	Conn *conn = GB_ConnQueue->InsertConn(-1, (void *) bev);
	if (conn_ssl!=NULL)
		conn->SetSSLObj(conn_ssl); //later   free in this place!
	conn->InitConnParam(obj_name, dest_host);
	if (obj_name_orign != NULL)
		conn->obj_name_orign = obj_name_orign; //可能obj_name和obj_name_orign不一样;ly thread
#ifdef XML_ALL_LOAD //??move to end??
	conn->ProcessObj->LoadObjectFile(GB_ParseCfg.c_str());	//ly obj_new
#else
	conn->ProcessObj->LoadObject(GB_ParseXml);	//ly obj_new
#endif
	conn->SetActive(false);

	//older version place before bufferevent_socket_connect_hostname();
	bufferevent_setcb(bev, echo_read_cb, echo_write_cb, event_connect_cb, conn);//laster time at connect_hostname;
	bufferevent_enable(bev, EV_READ | EV_WRITE);
	//default send process;??test this??
	if (msg.size() != 0) {
		evbuffer_add_printf(bufferevent_get_output(bev), "%s", msg.c_str());
	}

	//evdns_base_free(dns_base, 0);
	return conn;
}
/*if (ssl_type==1){
		ConnSSL *conn_ssl = GB_CLI_SCTX;
		bev = conn_ssl->bufferevent_ssl_socket_new(GB_EventBase[0], -1,ConnSSL::CONN_SSL_CONNECTING, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS,dest_host);
	}else{
		if (ssl_type==0)
			bev = bufferevent_socket_new(GB_EventBase[0], -1, BEV_OPT_CLOSE_ON_FREE);
		else if (ssl_type==2)
			bev = bufferevent_socket_new(GB_EventBase[0], -1, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);//filter process
		else{
			printf("do_conntion_obj fatal error!")
			return -1;
		}
	}*/

bool obj_test_find_func(Conn *conn, const char *obj_name) {
	if (strstr(conn->ProcessObj->GetParamValue("@_FD_NAME"), obj_name)
			== obj_name)
		return true;
	else
		return false;
}

static int obj_send_cb2(const char *msg, int msg_len, const char *dest_obj,
		const char *src, int trans_type, bool close_after_send, void *ctx) {
	Conn *conn;
	int rt;
    //cout << ".";//libevent test

	if (ctx != NULL && ((strlen(dest_obj) == 0)
					|| (strstr(dest_obj, "@THIS_FD") == dest_obj))) {
		conn = (Conn*) ctx;
		if ((rt = bufferevent_write((bufferevent *) conn->bev, msg, msg_len))
				< 0) {	//应该判断是否还在线
            WIZBASE_DEBUG((string)"obj_send_cb2()->Send Error: return number="+STRING_Helper::iToStr(rt)
					+" packet len:"+STRING_Helper::iToStr(msg_len));
			return -1;
		}
		if (close_after_send) {
			conn->close_after_send = true;
			//cout << "write_flush...." << bufferevent_flush((bufferevent *)conn->bev,EV_WRITE,BEV_FLUSH) << endl;
		}

	} else if ((strlen(dest_obj) == 0)
			|| (strstr(dest_obj, "@ALL_CONN") == dest_obj)) {
		conn = NULL;

		//int time_mergin = 0;
		//if (dest_obj && strlen(dest_obj) > 10) {
			//time_mergin = STRING_Helper::ParseULong((string) dest_obj);
		//}
		size_t i = 0;
		for (conn = GB_ConnQueue->FirstConn(true); conn != NULL; conn =
				GB_ConnQueue->NextConn(conn, true)) {
			if (!conn->is_act)
				continue;
			const char *msg_p = msg;
			size_t msg_p_len = msg_len;
			string msg_str(msg, msg_len);
			string conn_id;
			if (STRING_Helper::PUB_FindStrIs(msg_str, "[~@CONN_ID~]")
					&& (conn->obj_name.size() > conn->obj_name_orign.size())) {	//只是临时
				conn_id = conn->obj_name.substr(
						conn->obj_name_orign.size() + 1);
				STRING_Helper::strReplace(msg_str, "[~@CONN_ID~]", conn_id);
				msg_p = msg_str.data();
				msg_p_len = msg_str.size();
			}
            WIZBASE_DEBUG((string)"SendAllConn:0x"+conn_id);
			if (conn->bev == NULL) { //ly err_check: 测试流畅后，添加其它循环中
				WIZBASE_DEBUG((string)"obj_send_cb2()->Send Error: Conn->dev is NULL! (0x"+conn_id+")");
				continue;
			}
			if ((rt = bufferevent_write((bufferevent *) conn->bev, msg_p,
					msg_p_len)) < 0) { //应该判断是否还在线
				WIZBASE_DEBUG((string)"obj_send_cb2()->Send Error: return number="+STRING_Helper::iToStr(rt)+" packet len:" +STRING_Helper::iToStr(msg_len));
				return -1;
			}

			if (close_after_send) {
				conn->close_after_send = true;
				//cout << "write_flush...." << bufferevent_flush((bufferevent *)conn->bev,EV_WRITE,BEV_FLUSH) << endl;
			}
			//evbuffer *output = bufferevent_get_output((bufferevent *) conn->bev); //其实就是取出bufferevent中的output
			//size_t output_len = evbuffer_get_length(output);
			i++;
		}
		WIZBASE_DEBUG((string)" End");
	} else if ((conn = GB_ConnQueue->FindConnByObjName(dest_obj, 0)) != NULL) {
		if ((rt = bufferevent_write((bufferevent *) conn->bev, msg, msg_len))
				< 0) {
            WIZBASE_DEBUG((string)"obj_send_cb2()->Send Error: return number="+STRING_Helper::iToStr(rt)
					+" packet len:" +STRING_Helper::iToStr(msg_len));
			return -1;
		}
	} else {
        WIZBASE_DEBUG((string)"obj_send_cb2()->Send Error: DestObj->" +dest_obj+" not exited!");
		return -1;
	}

	//struct evbuffer *output;

	//input = bufferevent_get_input((bufferevent *)conn->bev);//其实就是取出bufferevent中的input  
	//if (conn != NULL) {
		//output = bufferevent_get_output((bufferevent *) conn->bev);	//其实就是取出bufferevent中的output
		//size_t output_len = evbuffer_get_length(output);
	//}

	return 0;
}

bool WIZBASE_pub_obj_send_msg(const char *msg, int msg_len,
		const char *dest_obj) {
	Conn *conn;
	if ((conn = GB_ConnQueue->FindConnByObjName(dest_obj, 0)) != NULL) {
		bufferevent_write((bufferevent *) conn->bev, msg, msg_len);
	} else {
		return false;
	}
	return true;
}

#define CUT_LEN_START 3
#define CUT_LEN_LEN 2
//#define CUT_START[2]= {{'0','1'},{'0','2'}};
static int echo_read_cb_check_conn(void *ctx, string &obj_name_str) {
	Conn *conn = (Conn*) ctx;  //what use?
	int pos=0;

	//string obj_name_str("");
	ParseXml_NodeSet nodes = GB_Gateway->GetXmlNodes(
			"/XMLParse/Connect/CONNECT[@Method='Listen']");
	//循环 
	for (ParseXml_NodeSet::const_iterator it = nodes.begin(); it != nodes.end();
			++it) {
		pugi::xpath_node xnode = *it;
		ParseXml_Node actions_node = xnode.node();
		const char *obj_name = conn->ProcessObj->GetXmlNodeAttr(actions_node,
				"Name");
		string test_str = conn->ProcessObj->GetXmlNodeAttr(actions_node,
				"Test");
		string match_str = conn->ProcessObj->GetXmlNodeAttr(actions_node,
				"MatchStr");
		if ((match_str.size() != 0) && (pos = STRING_Helper::PUB_FindStr(conn->buffer, match_str))>= 0) {
			obj_name_str = obj_name;
			return pos;  //Match 方式返回字串起始位置。filter
		} else if (test_str.size() != 0) {
			string err_str("");
			if (conn->ProcessObj->TestConditionStr(test_str, err_str)) {
				if (err_str.size() == 0) {
					obj_name_str = obj_name;
					return 0;  //Test 测试方式返回整个字符串。
				} else {
					cout << "/XMLParse/Connect/CONNECT[@Name='" << obj_name
							<< "'] Error:" << err_str << endl;
					return -1;
				}
			}
		}
	}

	//如果没有则取得缺省路径,test2
	string obj_name_tmp =
			(string) "/XMLParse/Connect/CONNECT[@Type='DEFAULT_CONN']";
	ParseXml_Node node = GB_Gateway->GetXmlNode(obj_name_tmp.c_str());
	if (!node) {
		return -1;
	} else {
		obj_name_str = conn->ProcessObj->GetXmlNodeAttr(node, "Name");
		return 0;
	}
}
static bool echo_read_cb_is_step(void *ctx, const char *act) {
	Conn *conn = (Conn*) ctx;  //what use?
	//如果指定动作
	string obj_path_str = (string) "/XMLParse/Connect/CONNECT[@Name='"
			+ conn->obj_name_orign + "']/" + act;
	ParseXml_Node node = GB_Gateway->GetXmlNode(obj_path_str.c_str());
	if (!node)
		return false;
	else
		return true;
}

static string echo_read_cb_check_step(void *ctx, int param) {
	Conn *conn = (Conn*) ctx;  //what use?
	string obj_path_str;
	if (param == Conn::CHECK_START_STEP) {
		//conn->proc_step=conn->ProcessObj->GetParamValue("@_START_STEP");//如果是指定下一步;
		if (!conn->ProcessObj->GetParamValueStr("@_START_STEP",
				conn->proc_step))
			obj_path_str = (string) "/XMLParse/Connect/CONNECT[@Name='"
					+ conn->obj_name_orign + "']/*[@Type='START_STEP']"; //test1
		else
			return conn->proc_step;
	} else if (param == Conn::CHECK_NEXT_STEP)
		obj_path_str = (string) "/XMLParse/Connect/CONNECT[@Name='"
				+ conn->obj_name_orign + "']/*[contains(@Type,'SON_STEP')]";
	ParseXml_NodeSet nodes = GB_Gateway->GetXmlNodes(obj_path_str.c_str());
	string node_name_str;

	//循环 
	for (ParseXml_NodeSet::const_iterator it = nodes.begin(); it != nodes.end();
			++it) {
		pugi::xpath_node xnode = *it;
		ParseXml_Node actions_node = xnode.node();
		const char *node_name = conn->ProcessObj->GetXmlNodeName(actions_node);
		string test_str = conn->ProcessObj->GetXmlNodeAttr(actions_node,
				"Test");
		//如果Check成功则创建对象	
		if (test_str.size() == 0)
			continue;

		string err_str("");
		if (conn->ProcessObj->TestConditionStr(test_str, err_str)) {
			if (err_str.size() == 0)
				node_name_str = node_name;
			else
				WIZBASE_DEBUG((string)"/XMLParse/Connect/CONNECT[@Name='"+conn->obj_name_orign+"']/"+node_name_str+"Error:"+ err_str);
			break;
		}
	}

	//如果没有则取得缺省路径,test2
	if (node_name_str.size() == 0) {
		obj_path_str = (string) "/XMLParse/Connect/CONNECT[@Name='"
				+ conn->obj_name_orign + "']/*[contains(@Type,'DEFAULT_STEP')]";
		ParseXml_Node node = GB_Gateway->GetXmlNode(obj_path_str.c_str());
		if (!node)
			WIZBASE_DEBUG((string)"/XMLParse/Connect/CONNECT[@Name='" +conn->obj_name_orign+"']/* can't find next step!");
		else {
			node_name_str = conn->ProcessObj->GetXmlNodeName(node);
		}
	}
	return node_name_str;
}

bool WIZBASE_create_routing_packet(string &buffer, int path_type, string &dest,
		string &value, UInt16 dest_point) {
	//初始化buffer
	buffer = "";
	//设置 buffer
	if (path_type == WIZBASE_PATH_TYPE_URL_PATH) {
		//填写header结构
		routing_header_def_str header_str;
		strcpy(header_str.check_str, "@~@");
		STRING_Helper::putUINT16(header_str.packetID, 0, 0);//header_str.packetID=0;
		header_str.type = 1;		//类型01：***.***.**** 类型02:MAC(UInt64)+MAC+MAC
		STRING_Helper::putUINT16(header_str.dest_point, 0, dest_point);	//header.dest_point=dest_point;
		STRING_Helper::putUINT16(header_str.value_point, 0,sizeof(header_str) + dest.size());//header.value_point=sizeof(header)+dest.size();
		STRING_Helper::putUINT32(header_str.length, 0,sizeof(header_str) + dest.size() + value.size());//header.length=sizeof(header)+dest.size()+value.size();
						//header.src_point=sizeof(header)+dest.size()+value.size();

		//添加字串
		buffer.append((const char *) &header_str, sizeof(header_str));
		buffer.append(dest);
		buffer.append(value);
		//buffer.append(src)
		return true;
	} else {
		return false;
	}
}

bool WIZBASE_get_routing_packet(routing_header_def &header, string &src_buf) {
	//初始化
	if (src_buf.size() != sizeof(routing_header_def_str)) {
		cout
				<< "Error:WIZBASE_get_routing_packet(string) input is not routing_header_def_str"
				<< endl;
		return false;
	}
	routing_header_def_str *header_str =
			(routing_header_def_str *) (src_buf.data()); //init data;

	//填写header结构
	strcpy(header.check_str, "@~@");
	header.packetID = STRING_Helper::scanUINT16(header_str->packetID, 0);
	header.type = (byte) header_str->type; //类型01：***->***.**** 类型02:MAC(UInt64)+MAC+MAC
	header.dest_point = STRING_Helper::scanUINT16(header_str->dest_point, 0);
	header.value_point = STRING_Helper::scanUINT16(header_str->value_point, 0);
	header.length = STRING_Helper::scanUINT32(header_str->length, 0);

	return true;
}

string buffer_case_dest_to_src(string &buffer, int point, vector<string> &dest_s) {
	string src_str;
	for (int i = point; i >= 0; i--) {
		if (i != point)
			src_str += "/";
		src_str.append(dest_s[i]);
	};
	return src_str;
}

void buffer_case_init(Conn *conn) {
	conn->case_step = Conn::CASE_START;
	conn->buffer = "";
	conn->rsend_str = "";
	conn->bufstate = 0;
	conn->proc_step = "";
}

void WIZBASE_conn_disconnect(Conn *conn){
    if (conn!=NULL && conn->is_act)
        conn_disconnect((struct bufferevent *)conn->bev,conn);
}

static void conn_disconnect(struct bufferevent *bev, void *ctx) {
	Conn *conn = (Conn*) ctx;

	if (conn->GetSSLObj()!=0){//ly memleak2 del
		ConnSSL *conn_ssl=(ConnSSL *)conn->GetSSLObj();
		cout << "conn_ssl realease!" <<endl;
		conn_ssl->ShutDownSSL(bev);
       /* SSL *ssl = conn_ssl->bufferevent_openssl_get_ssl(bev);
        if (ssl!=NULL){
			SSL_set_session(ssl,NULL);
			//SSL_shutdown (conn_ssl->ssl);
			SSL_set_shutdown(ssl, SSL_RECEIVED_SHUTDOWN);
			SSL_shutdown(conn_ssl->ssl);
        }*/
	}

    //cout << "conn_disconnect step1.." <<endl;
    //release_sock_event(bev);
    bufferevent_disable(bev, EV_READ | EV_WRITE);
    bufferevent_setcb(bev, NULL, NULL, NULL, NULL);
    bufferevent_free(bev);

    //free conn;ly test; 2019-5月for websocket
	//if (conn->GetFd()!=0)
		//evutil_closesocket(conn->GetFd());


	//release pro_obj
    //cout << "conn_disconnect step2.." <<endl;
	/*if (conn->GetSSLObj()!=0){//ly memleak2 del
		ConnSSL *conn_ssl=(ConnSSL *)conn->GetSSLObj();
		delete conn_ssl;
	}*/
    
    //cout << "conn_disconnect step3.." <<endl;
    //time task
    if (conn->time_ev!=NULL)
        WIZBASE_CancleQueueTimeTask(conn);
    
	GB_ConnQueue->DeleteConn(conn);
    //cout << "conn_disconnect step4.." <<endl;
}

static void conn_err_node(struct bufferevent *bev,void *ctx,string &err_str) {
	Conn *conn = (Conn*) ctx;

    //ly_send_queue: 操作差错处理节点
    //cout << "conn_err_node step1.." <<endl;
    const char *err_step=conn->ProcessObj->GetParamValue("@_ERROR_STEP");
    //cout << "conn_err_node step2.." <<endl;
    if(err_step==NULL || strlen(err_step)==0)
        err_step="_CONN_ERR";
    string act_node = (string) "/XMLParse/Connect/CONNECT[@Name='"
                      + conn->obj_name_orign + "']/" + err_step;
    //cout << "conn_err_node step3.." <<endl;
    ParseXml_Node node= conn->ProcessObj->GetXmlNode(act_node.c_str());
    //cout << "conn_err_node step4.." <<endl;
    if (node){
        conn->ProcessObj->SetParam("@_ERR_STR",err_str);
        conn->ProcessObj->GoAction(act_node.c_str());
    }
    //cout << "conn_err_node step5.." <<endl;
}

static void echo_read_cb(struct bufferevent *bev, void *ctx) {
	char data[4098];
	unsigned long int nbytes;
	string rsp_str = "";
	string rcv_str = "";
	Conn *conn = (Conn*) ctx;
    //cout << "~";//libevent test

	WizBase_pthread_mutex_lock(&GB_Lock);
	//printf("called read_cb!\n");
	//DEBUG
	struct evbuffer *input = bufferevent_get_input(bev);
	//struct evbuffer *output = bufferevent_get_output(bev);
	//Debug
	//printf("input_len: %d\n", (int)evbuffer_get_length(input));
	//printf("output_len: %d\n", (int)evbuffer_get_length(output));

	bool while_break = false;
	string act_node = "";
	string obj_name = "";
	int pos;
	while (!while_break) {
		if (conn->ProcessObj->IsReady()) {
			switch (conn->case_step) {
			case Conn::CASE_START:

				nbytes = bufferevent_read(bev, data,
						(unsigned long int) sizeof(data));
				if (nbytes <= 0) {
					if (conn->buffer.size() == 0) {
						while_break = true;
						break;
					}
				} else
					conn->buffer.append(data, nbytes);

				//cout << "CASE_START:conn->buffer:"<< conn->buffer.substr(0,100) << "..." <<endl;
				conn->packet_len = 0; //!!!buffer
				conn->bufstate = 1; //0:init 1: adding 2:add_end
				conn->ProcessObj->SetParam("@_INPUT_STRING",conn->buffer.data(), conn->buffer.size(),Http_ParseObj::STR_COPY_OBJ); //创建INPUT_STRING字串
				//选择路径：SKHD or NEXT_STEP;
				if (conn->obj_name.size() == 0) {
					//初始未连接对象
					conn->case_step = Conn::CASE_SKHD;			//新创建握手包
				} else if (conn->buffer.size() >= 3
						&& conn->buffer.compare(0, 3, "@~@") == 0) {
					//路由包
					conn->case_step = Conn::CASE_ROUTING_STEP; //新创建握手包!!!buffer!!!如果是非连续递进包则有错,但是如果是数据包头则几率小一些。
				} else {
					//已确定连接对象,普通包
					conn->case_step = Conn::CASE_NORMAL_STEP;   //普通入口包!!!buffer
				}
				;
				break;
			case Conn::CASE_SKHD:
				//cout << "conn->buffer:"<< conn->buffer.substr(0,100) << "..." <<endl;
				if ((pos = echo_read_cb_check_conn(conn, obj_name)) >= 0) {
					if (pos > 0)
						conn->buffer.erase(0, pos - 1); //对于输入字串的截断，过滤前面无用字串。!!!filter test2;
					conn->InitConnParam(obj_name.c_str(), 0);
					conn->proc_step = "SKHD_ACT";
					act_node = (string) "/XMLParse/Connect/CONNECT[@Name='"+ obj_name + "']/" + conn->proc_step;
					//如果需要，取得最小长度，超过则继续，否则再接受
					const char *smin = conn->ProcessObj->GetObjFileNodeAttr(act_node.c_str(), "MinLen");
					if (smin != 0 && strlen(smin) != 0) {
						unsigned long min_len = STRING_Helper::ParseULong(smin);
						if (min_len == STRING_Helper::NaN) {
							WIZBASE_DEBUG((string)"Case CASE_SKHD: Check Min Len error!");
							conn->obj_name = "";
							conn->case_step = Conn::CASE_END;
							break;
						} else if (conn->buffer.size() < (size_t) min_len) {
							conn->obj_name = "";
							while_break = true;
							conn->case_step = Conn::CASE_START;
							break;
						}
					}
					//如果需要，取得确切长度
					const char *slen = conn->ProcessObj->GetObjFileNodeAttr(act_node.c_str(), "Len");
                    if (slen == 0 || strlen(slen) == 0){
						conn->packet_len = 0;
                    }else {
                        //ly_queue_2 add
                        /*const char *begin_hex = conn->ProcessObj->GetObjFileNodeAttr(act_node.c_str(), "BeginHex");
                        if ((begin_hex != 0)&& (strlen(begin_hex) > 0)&& !STRING_Helper::MatchStr2Hex(conn->buffer,begin_hex)) {
                            //如果验证头错误，则递进一步
                            conn->buffer.erase(0, 1);
                            break;
                        }*/
                        
						string parse_len = conn->ProcessObj->Parse(slen);
						conn->packet_len = STRING_Helper::ParseInt(parse_len.c_str());
						if (conn->packet_len == STRING_Helper::NaN) {
							WIZBASE_DEBUG((string)"Case CASE_SKHD: Check Len error!");
							conn->obj_name = "";
							conn->case_step = Conn::CASE_END;
							break;
						}
					}
                    //ly_queue_2 ???
                    const char *end_hex = conn->ProcessObj->GetObjFileNodeAttr(act_node.c_str(), "EndHex");
                    if (conn->packet_len==0 && (end_hex != 0) && (strlen(end_hex) > 0)) {
                        //如果验证头错误，则递进一步
                        conn->packet_end=STRING_Helper::HBCStrToHBCStr(end_hex, LW_HEX_STR, LW_BINARY_STR, false);
                        if (conn->packet_end.size()==0){
                            WIZBASE_DEBUG((string)"Case Load Packet: EndHex error!");
                            conn->case_step = Conn::CASE_END;
                            break;
                        }
                        conn->packet_len=Conn::PACKET_LEN_END;
                    }
					//cout << "CASE_SKHD: checkLen success!=" << conn->packet_len << endl;
					//读取长度
					conn->case_step = Conn::CASE_LOAD_PACKET;
					break;
				} else {
                    WIZBASE_DEBUG((string)"can't find connect string node!("+STRING_Helper::HBCStrToHBCStr(conn->buffer, LW_BINARY_STR, LW_HEX_STR,true)+")");
					conn->case_step = Conn::CASE_START; //继续读取字串;并保留旧的!!!buffer!!!可能会发生死循环test2
					if ((int) evbuffer_get_length(input) <= 0) {
						while_break = true;
						break;
					} else
						continue;
				}
				break;
			case Conn::CASE_ROUTING_STEP:
				//取得下一步
				conn->proc_step = "@_SYS_ROUTING";
				if (conn->buffer.size() >= sizeof(routing_header_def_str)) { //test2
					//路由长度解析操作：
					string header_s = conn->buffer.substr(0,
							sizeof(routing_header_def_str));
					routing_header_def_str *header =(routing_header_def_str *) header_s.data();
					conn->packet_len = STRING_Helper::scanUINT32(header->length,0);					//取得数据报结构routing_header_def
					conn->case_step = Conn::CASE_LOAD_PACKET;
				} else {
					WIZBASE_DEBUG((string)"CASE ROUTING STEP error: data must large then 14! continue...");
					conn->case_step = Conn::CASE_START;	//继续读取字符串;并保留旧的!!!buffer!!!可能会发生死循环
					if ((int) evbuffer_get_length(input) <= 0) {
						while_break = true;
						break;
					} else
						continue;
				}
				;
				break;
			case Conn::CASE_NORMAL_STEP:
				//取得下一步				
				conn->proc_step = echo_read_cb_check_step(conn,Conn::CHECK_START_STEP);//如果是测试下一步。!!!buffer!!!可能会发生包不全的情况
				if (conn->proc_step.size() != 0) {
					act_node = (string) "/XMLParse/Connect/CONNECT[@Name='"+ conn->obj_name_orign + "']/" + conn->proc_step;
					//最小字串长度测试
					const char *min_len_s =conn->ProcessObj->GetObjFileNodeAttr(act_node.c_str(), "MinLen");
					size_t min_len = ((min_len_s != 0 && strlen(min_len_s) > 0) ? STRING_Helper::StrToi(min_len_s) : Conn::MIN_PACKET_LEN);
					if (conn->buffer.size() < min_len) {
						//如果没有达到最小字串长，则继续读取
						WIZBASE_DEBUG((string)"data size("+STRING_Helper::uToStr(conn->buffer.size())+")must large then"+STRING_Helper::uToStr(min_len)+"continue.");
						conn->case_step = Conn::CASE_START; //继续读取字串;并保留旧的!!!buffer!!!可能会发生死循环test2
						if ((int) evbuffer_get_length(input) <= 0) {
							while_break = true;
							break;
						} else
							continue;
					}
					//match str的情况
					const char *begin_hex = conn->ProcessObj->GetObjFileNodeAttr(act_node.c_str(), "BeginHex");
					if ((begin_hex != 0)&& (strlen(begin_hex) > 0)&& !STRING_Helper::MatchStr2Hex(conn->buffer,begin_hex)) {//ly_queue_2 STRING_Helper::MatchStr2Hex
						//如果验证头错误，则递进一步
						conn->buffer.erase(0, 1);
						break;
					}
					conn->ProcessObj->SetParam("@_INPUT_STRING",conn->buffer.data(), conn->buffer.size(),Http_ParseObj::STR_COPY_OBJ); //创建INPUT_STRING字串

					//以下为MatchHeader成功情况	
					if (conn->bufstate != 2) { //如果非完整数据报状态
						//长度判断，今后引为公共。test1
						const char *slen = conn->ProcessObj->GetObjFileNodeAttr(act_node.c_str(), "Len");
                        if (slen == 0 || strlen(slen) == 0){
							conn->packet_len = 0;
                        }else {
							string parse_len = conn->ProcessObj->Parse(slen);
							conn->packet_len = STRING_Helper::ParseInt(parse_len.c_str());
							if (conn->packet_len == STRING_Helper::NaN) {
								WIZBASE_DEBUG((string)"Case Load Packet: Check Len error!");
								conn->case_step = Conn::CASE_END;
								break;
							}
						}
						//cout << "CASE_NORMAL_STEP: checkLen success=" << conn->packet_len << endl;
                        //ly_queue_2
                        const char *end_hex = conn->ProcessObj->GetObjFileNodeAttr(act_node.c_str(), "EndHex");
                        if (conn->packet_len==0 && (end_hex != 0) && (strlen(end_hex) > 0)) {
                            //如果验证头错误，则递进一步
                            conn->packet_end=STRING_Helper::HBCStrToHBCStr(end_hex, LW_HEX_STR, LW_BINARY_STR, false);
                            if (conn->packet_end.size()==0){
                                WIZBASE_DEBUG((string)"Case Load Packet: EndHex error!");
                                conn->case_step = Conn::CASE_END;
                                break;
                            }
                            conn->packet_len=Conn::PACKET_LEN_END;
                        }
						conn->case_step = Conn::CASE_LOAD_PACKET;
					} else {						//完整数据包状态;今后可以作为直接调用完整包时使用
						conn->case_step = Conn::CASE_LOAD_PACKET;
					}
				} else {
					WIZBASE_DEBUG((string)"can't find start step node!");
					conn->case_step = Conn::CASE_END;
				};
				break;
			case Conn::CASE_LOAD_PACKET:
				//cout <<"Recive: conn->packet_len: " <<conn->packet_len << "; conn->buffer.size():" << conn->buffer.size() << endl;
				//cout << ">>=" << conn->buffer.size() << endl;
                //ly_queue_2
                size_t pos;
                if ((conn->packet_len==Conn::PACKET_LEN_END) && conn->packet_end.size()!=0 && (pos=conn->buffer.find(conn->packet_end))!=string::npos){
                    conn->bufstate=2;
                    conn->packet_len = pos+conn->packet_end.size();
                }
				if ((conn->bufstate != 2) && (conn->packet_len != 0) && (conn->buffer.size() < conn->packet_len)) {//如果没有达到包大小则再读取test1
					nbytes = bufferevent_read(bev, data,(unsigned long int) sizeof(data));
					if (nbytes <= 0) {
						while_break = true;				//差错或没有读完
                        //WIZBASE_DEBUG((string)"CASE_LOAD_PACKET:...(old buffer: "+STRING_Helper::HBCStrToHBCStr(conn->buffer,"char","hex",true)+":"+STRING_Helper::iToStr(conn->bufstate)+":"+STRING_Helper::uToStr(conn->packet_len)+":"+STRING_Helper::uToStr(conn->buffer.size())+")");
                        //WIZBASE_DEBUG((string)"CASE_LOAD_PACKET:...(old buffer: "+STRING_Helper::iToStr(conn->bufstate)+":"+STRING_Helper::uToStr(conn->packet_len)+":"+STRING_Helper::uToStr(conn->buffer.size())+")");
						break;
					} else
						conn->buffer.append(data, nbytes);
                    string tmp=conn->buffer.substr(0, 128);
                    //WIZBASE_DEBUG((string) "CASE_LOAD_PACKET:conn->buffer:("+STRING_Helper::uToStr(conn->buffer.size())+"):"+STRING_Helper::HBCStrToHBCStr(tmp,"char","hex",true)+"...");
                }else {
					//已完成读取
					conn->bufstate = 2;
					//cout << "CASE_LOAD_PACKET:successd!" << endl;
					if (conn->proc_step == "@_SYS_ROUTING") {
						if (conn->packet_len == 0)
							conn->rsend_str = conn->buffer;
						else
							conn->rsend_str = conn->buffer.substr(0,conn->packet_len);
						conn->case_step = Conn::CASE_ROUTING_PROCESS;	//test2
					} else {
						if (conn->packet_len == 0)
							conn->ProcessObj->SetParam("@_INPUT_STRING",conn->buffer.data(), conn->buffer.size(),Http_ParseObj::STR_COPY_OBJ);//创建INPUT_STRING字串
						else
							conn->ProcessObj->SetParam("@_INPUT_STRING",conn->buffer.data(), conn->packet_len,Http_ParseObj::STR_COPY_OBJ);//创建INPUT_STRING字串
						conn->case_step = Conn::CASE_PROCESS;
					}
				}
				break;
			case Conn::CASE_PROCESS:
				if (conn->proc_step.size() != 0) {
					//进入处理操作 
					//conn->ProcessObj->TimeTrack(conn->proc_step.c_str(), ParseProcess::SYS_INFO_START_TIME);
					act_node = (string) "/XMLParse/Connect/CONNECT[@Name='"+ conn->obj_name_orign + "']/" + conn->proc_step;
					//cout << "Start: " + act_node << endl;
					//WIZBASE_DEBUG((string)"=>>"+STRING_Helper::uToStr(conn->buffer.size())+":"+conn->proc_step +":");
					conn->ProcessObj->SetParam("@_NEXT_STEP", "",Http_ParseObj::STR_COPY_OBJ);//清空下一步。
					conn->ProcessObj->SetParam("@_CONN_NAME",conn->obj_name.c_str(),Http_ParseObj::STR_COPY_OBJ);//初始化相关req值。
					conn->ProcessObj->DoProcess(act_node.c_str(), obj_send_cb2, (void *) conn);

					//重置obj_name
					const char * new_conn_name = conn->ProcessObj->GetParamValue("@_CONN_NAME");
					if ((new_conn_name != NULL) && (conn->obj_name != new_conn_name))
						conn->obj_name = new_conn_name;
					//如果NEXTSTEP
					conn->proc_step = "";
					conn->ProcessObj->GetParamValueStr("@_NEXT_STEP",conn->proc_step);					//如果是指定下一步;
					//如果强制退出
					string close_after_read = "";
					conn->ProcessObj->GetParamValueStr("@_CLOSE_AFTER_READ",close_after_read);
					if (close_after_read == "true") {
						conn->close_after_read = true;
						conn->case_step = Conn::CASE_END;
					}

					////如果是SKHD_ACT或没有NEXT_STEP
					if ((conn->proc_step == "SKHD_ACT")|| (conn->proc_step.size() == 0))
						conn->case_step = Conn::CASE_END;
					else if (conn->proc_step == "@_SYS_ROUTING") {
						conn->case_step = Conn::CASE_RSEND_STEP;
					} else
						conn->case_step = Conn::CASE_NEXT_STEP;	//如果是连续行NEXT_STEP
				} else {
					WIZBASE_DEBUG((string)"Conn::CASE_PROCESS error: proc_step is null!");
					conn->case_step = Conn::CASE_END;
				}
				;
				//conn->ProcessObj->_TimeSum[0] = LW_GetTickCount()- conn->ProcessObj->_TimeStart;
				//conn->ProcessObj->TimeTrack(conn->proc_step.c_str(), ParseProcess::SYS_INFO_END_TIME);
				//conn->ProcessObj->ProcessCount++;
				break;
			case Conn::CASE_RSEND_STEP:
				//取得下一步
				if (conn->ProcessObj->GetParamValueStr("@_RSEND_STRING",
						conn->rsend_str)) {
					conn->case_step = Conn::CASE_ROUTING_PROCESS;		//test2
				} else {
					WIZBASE_DEBUG((string)"Conn::CASE_PROCESS->@_SYS_ROUTING error: @_RSEND_STRING must be set!"); //test2
					conn->case_step = Conn::CASE_END;
				}
				break;
			case Conn::CASE_NEXT_STEP:
				//取得下一步
				if (conn->proc_step.size() != 0) {
					if (conn->proc_step == "@_SYS_ROUTING") { //转发下一步
						conn->case_step = Conn::CASE_RSEND_STEP; //test2
					} else if (conn->proc_step == "@_SYS_RTEST") { //测试下一步
						conn->proc_step = echo_read_cb_check_step(conn,
								Conn::CHECK_NEXT_STEP); //如果是测试下一步。test2
						if (conn->proc_step.size() != 0) {
							//还沿用原来的@INPUT_STRING;
							conn->case_step = Conn::CASE_PROCESS;
						} else {
							WIZBASE_DEBUG((string)"next step node not exited!");
							conn->case_step = Conn::CASE_END;
						}
					} else if (conn->proc_step == "@_SYS_RELOAD") { //从新把INPUT_STRING(改动后，再加入input_buffer)
						string tmp_str = "";
						conn->ProcessObj->GetParamValueStr("@_RELOAD_STRING",
								tmp_str);
						conn->buffer.insert(conn->packet_len, tmp_str);
						conn->case_step = Conn::CASE_END;
						break;
					} else {
						//还沿用原来的@INPUT_STRING;
						conn->case_step = Conn::CASE_PROCESS; //直接指定下一步
						break;
					}
				} else
					conn->case_step = Conn::CASE_END;
				break;
			case Conn::CASE_ROUTING_PROCESS:
				if ((conn->rsend_str.size() >= sizeof(routing_header_def_str))
						&& conn->rsend_str.size() >= 3
						&& conn->rsend_str.compare(0, 3, "@~@") == 0) {
					//截取、解析路由头
					string header_s = conn->rsend_str.substr(0,
							sizeof(routing_header_def_str));
					routing_header_def header;
					routing_header_def_str header_str;
					memcpy((void*) &header_str, header_s.data(),
							sizeof(routing_header_def_str)); //应该在此处增加str和转移
					WIZBASE_get_routing_packet(header, header_s);		//替代上述。

					//截取目标地址
					string dest_str = conn->rsend_str.substr(
							sizeof(routing_header_def_str),
							header.value_point
									- sizeof(routing_header_def_str));
					WIZBASE_DEBUG((string)"dest_str:"+dest_str +" type:" +STRING_Helper::iToStr(header.type));
					//
					if ((header.type == WIZBASE_PATH_TYPE_URL_PATH)
							&& (dest_str.size() != 0)) {	//如果是***.***.***类型。
						//预解析路径;拆分?项
						string act("");
						vector<string> dest_s;
						STRING_Helper::stringSplit(dest_str, dest_s, "/");
						string end_obj = dest_s[dest_s.size() - 1];
						if (((size_t) (pos = end_obj.find("?")))!= string::npos) {
							act = end_obj.substr((size_t)pos + 1);				//如果有动作
							dest_s[dest_s.size() - 1] = end_obj.substr(0, pos);
						}

						//测试是否存在目标对象
						if ((header.dest_point + 1) < (UInt16) dest_s.size()) {
							//还没有达到目的地
							Conn *conn2;
							if ((conn2 = GB_ConnQueue->FindConnByObjName(dest_s[header.dest_point + 1].c_str(), 0))
									== NULL) {
								//如果不存在目标节点.发送差错字串
								conn->case_param["src_path"] =
										buffer_case_dest_to_src(conn->rsend_str,
												header.dest_point, dest_s);
								conn->case_param["err_str"] =
										"Error(@RSEND): can't find node: "
												+ dest_s[header.dest_point + 1]
												+ " witch dest path is ("
												+ dest_str + ")!";
								conn->case_step = Conn::CASE_ROUTING_ERROR;
								break;
							} else {
								//如果存在目标节点则转发
								//更改dest_num+1
								header.dest_point++;
								//转发送操作
								if ((header.dest_point
										== ((int) dest_s.size() - 1))
										&& echo_read_cb_is_step(conn2,
												"SEND_ACT")) {
									//如果最后一个节点，且设置了SEND_ACT;则内部发送
									string conn2_rsend = conn->rsend_str.substr(
											header.value_point);//去除转发头信息；今后应该改为进入conn->Process循环!!!buffer?
									conn2->ProcessObj->SetParam("@_INPUT_STRING",conn2_rsend.data(),
											conn2_rsend.size(),
											Http_ParseObj::STR_COPY_OBJ);//创建INPUT_STRING字串	
									string send_act_node =
											(string) "/XMLParse/Connect/CONNECT[@Name='"
													+ conn2->obj_name_orign
													+ "']/SEND_ACT";
									conn2->ProcessObj->DoProcess(
											send_act_node.c_str(), obj_send_cb2,
											(void *) conn2);//!!!buffer?可去除conn2->buffer输入
									conn->case_step = Conn::CASE_END;
								} else {
									//否则直接发送操作
									//buffer_case_dest_point_set(conn->rsend_str,(routing_header_def_str *)(header_str.data()),header.dest_point);	//change
									STRING_Helper::putUINT16(
											header_str.dest_point, 0,
											header.dest_point);	//重置header_str.dest_point;
									conn->rsend_str.replace(0,
											sizeof(routing_header_def_str),
											(const char*) &header_str,
											sizeof(routing_header_def_str));//拷贝header_str
									obj_send_cb2(conn->rsend_str.data(),conn->rsend_str.size(),dest_s[header.dest_point].c_str(),0, 0, false, conn); //!!!buffer?
									conn->case_step = Conn::CASE_END;
								}
								break;
							}
						} else if (header.dest_point
								== ((int) dest_s.size() - 1)) {
							//到达目的地; 可能出现num超出的情况
							//设置源地址
							string src_str = buffer_case_dest_to_src(conn->rsend_str, dest_s.size() - 1, dest_s);
							conn->ProcessObj->SetParam("@_SRC_PATH",
									src_str.c_str(), src_str.size(),
									Http_ParseObj::STR_COPY_OBJ);//创建INPUT_STRING字串
							string head_len = STRING_Helper::iToStr(
									(int) header.length);
							conn->ProcessObj->SetParam("@_SRC_DATA_LEN",
									head_len.c_str(), head_len.size(),
									Http_ParseObj::STR_COPY_OBJ);//创建INPUT_STRING字串

							//读取proc_step
							if (act.size() != 0) {					//如果有?动作描述
								if (!echo_read_cb_is_step(conn, act.c_str())) {	//发送差错字串
									conn->case_param["src_path"] = src_str;
									conn->case_param["err_str"] =
											"Error(@RSEND): can't find process step: "
													+ dest_str
													+ " witch dest path is ("
													+ dest_str + ")!";
									conn->case_step = Conn::CASE_ROUTING_ERROR;
									//cout <<"step: dested! act error!" << endl;
									break;
								} else {
									conn->proc_step = act;			//直接取得下一步。
									//cout <<"step: dested! act sended!" << endl;
								}
							} else {
								//cout << "step: dested! act retest" << endl;
								conn->proc_step = echo_read_cb_check_step(conn,
										Conn::CHECK_START_STEP);	//如果是测试下一步。
							}
							//动作处理
							conn->rsend_str.erase(0, header.value_point);//截取内容字串!!!buffer?test2
							//cout << "step: dested! input string:" << conn->rsend_str <<endl;
							conn->ProcessObj->SetParam("@_INPUT_STRING",
									conn->rsend_str.data(),
									conn->rsend_str.size(),
									Http_ParseObj::STR_COPY_OBJ);//创建INPUT_STRING字串
							conn->case_step = Conn::CASE_PROCESS;
							break;
						};
						cout << "CASE ROUTING PROCESS error: dest_str ='"
								<< dest_str << "'   dest_point= "
								<< header.dest_point << "!" << endl;
						conn->case_step = Conn::CASE_END;
						break;
					} else {
						WIZBASE_DEBUG((string)"CASE ROUTING PROCESS error: dest_str ='"+dest_str+"'   dest_point= "+STRING_Helper::iToStr(header.dest_point)+"!");
						conn->case_step = Conn::CASE_END;
						break;
					}
				} else {
					WIZBASE_DEBUG((string)"CASE ROUTING PROCESS error: data must large then 8 or not standerd routing packet!");//!!!buffer -这里可能会把buffer延续包截断。
					conn->case_step = Conn::CASE_END;
				}
				;
				break;
			case Conn::CASE_ROUTING_ERROR:
				//发送差错字串回源地址
				//bool b=WIZBASE_create_routing_packet(conn->rsend_str,WIZBASE_PATH_TYPE_URL_PATH,conn->case_param["src_path"],conn->case_param["err_str"],0);//!!! in_buff;可能删除buffer之后的内容;buffer?
				//if (conn->rsend_str!="")
				if (WIZBASE_create_routing_packet(conn->rsend_str,
						WIZBASE_PATH_TYPE_URL_PATH,
						conn->case_param["src_path"],
						conn->case_param["err_str"], 0))
					conn->case_step = Conn::CASE_ROUTING_PROCESS;
				else
					conn->case_step = Conn::CASE_END;
				break;
			case Conn::CASE_END:
				//buffer_case_init(conn);
				conn->case_step = Conn::CASE_START;
				//!!!buffer;缓存累加处理
				if (conn->packet_len <= 0)
					conn->buffer = "";
				else {
					conn->buffer.erase(0, conn->packet_len);
				}
				conn->bufstate = 0;
				conn->proc_step = "";
				conn->rsend_str = "";
				//
				conn->ProcessObj->SetParam("@_INPUT_STRING", "",
						Http_ParseObj::STR_COPY_OBJ);		//创建INPUT_STRING字串
				conn->ProcessObj->SetParam("@_NEXT_STEP", "",
						Http_ParseObj::STR_COPY_OBJ);				//清空下一步。
				conn->ProcessObj->SetParam("@_SRC_PATH", "",
						Http_ParseObj::STR_COPY_OBJ);		//创建INPUT_STRING字串
				if ((conn->buffer.size() != 0)) {
					conn->case_step = Conn::CASE_START; //再循环处理
				} else
					while_break = true;
				break;
			}
		} else {
			WIZBASE_DEBUG((string)"process_obj is not ready!");
		}
	}

	if (conn->close_after_read) {
		WIZBASE_DEBUG((string)"close socket after read!");
		conn_disconnect(bev, ctx);
	}
	WizBase_pthread_mutex_unlock(&GB_Lock);
	//DEBUG
	//printf("input_len: %d\n", (int)evbuffer_get_length(input));
	//printf("output_len: %d\n", (int)evbuffer_get_length(output));
}

static void echo_event_cb(struct bufferevent *bev, short events, void *ctx) {
	Conn *conn = (Conn*) ctx;
    string err_str("");
    //cout << "_"; //libevent test
	//disconnect
	//if (events & BEV_EVENT_ERROR)
	//perror("echo_event_cb()->Error from bufferevent");

	if (events & (BEV_EVENT_EOF|BEV_EVENT_TIMEOUT|BEV_EVENT_ERROR)){
		//ly_send_queue
		if (events & (BEV_EVENT_EOF)) {
			//bufferevent_free(bev);
			err_str=(string)"Connect object["+conn->obj_name+"] closed!";
		} else if (events & (BEV_EVENT_TIMEOUT)) {
            err_str=(string)(string)"echo_event_cb()->Connect object[" +conn->obj_name+"] time out!";
		} else if (events & (BEV_EVENT_ERROR)) {
			if (conn->obj_name.size() < 1024)
                err_str=(string)"echo_event_cb()->Connect object["+conn->obj_name+"] some other error!";
			else {
                err_str=(string)(string)"echo_event_cb()->Connect object[???] some other error!";
			}
		}
        WIZBASE_ERROR((string)"echo_event_cb()"+err_str);
		conn_err_node(bev,conn,err_str); //补充节点差错处理。
	}
	else if (events & (BEV_EVENT_CONNECTED)) {
		//WIZBASE_DEBUG((string)"event_connected.....");
		return;
	} else if (events & (BEV_EVENT_READING)) {
		WIZBASE_DEBUG((string)"echo_event_cb()->Connect object["+conn->obj_name+"] error in reading!");
	} else if (events & (BEV_EVENT_WRITING)) {
		WIZBASE_DEBUG((string)"echo_event_cb()->Connect object["+conn->obj_name+"] error in writing!");
	} else
        WIZBASE_DEBUG((string)"echo_event_cb()->Connect object["+conn->obj_name+"] error in not defined!");
	conn_disconnect(bev, conn);

	//free conn
	//GB_ConnQueue->DeleteConn(conn);
}

static void echo_write_cb(struct bufferevent *bev, void *ctx) {
	//free conn
    //cout << "-"; //libevent test
	Conn *conn = (Conn*) ctx;
	if (conn->close_after_send) {
		WIZBASE_DEBUG((string)"close socket after write!");
		conn_disconnect(bev, ctx);
	}
}

static void accept_conn_cb(struct evconnlistener *listener,
evutil_socket_t fd, struct sockaddr *address, int socklen, void *arg) {
	GB_Gateway->TimeTrack("ACCEPT", ParseProcess::TRACK_START);
	ConnSSL *conn_ssl = GB_SRC_SCTX; 	//(ConnSSL *)arg;

	/* We got a new connection! Set up a bufferevent for it. */
	struct event_base *base = evconnlistener_get_base(listener);
	struct bufferevent *bev;

	Conn *conn;
	int port = *((int *) arg);
	if ((port) == 443) {
		bev = conn_ssl->bufferevent_ssl_socket_new(base, fd,ConnSSL::CONN_SSL_ACCEPTING);
		cout << "port(443):ssl connection...." <<endl;
	} else {
		bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	}
	//SSL *tmp_ssl = bufferevent_openssl_get_ssl(bev);
	conn = GB_ConnQueue->InsertConn(fd, (void *) bev);

	//get connect info
	string ip_addr, ip_port, host;
	util_get_ipaddr(address, socklen, conn, ip_addr, ip_port);
	host = ip_addr + ":" + ip_port;

	WIZBASE_DEBUG((string)"accepted connection from "+ip_addr+":"+ip_port);
	conn->InitConnParam("", host.c_str());	//设置ip，但是暂时不设obj_name,等read判断后在设置
	//conn->ProcessObj->LoadObjectFile(GB_ParseCfg.c_str());
	//conn->ProcessObj->LoadObject(GB_ParseXml);
#ifdef XML_ALL_LOAD
	conn->ProcessObj->LoadObjectFile(GB_ParseCfg.c_str());
#else
	conn->ProcessObj->LoadObject(GB_ParseXml);		//ly obj_new
#endif
	conn->InitProcessObj("");
	conn->ProcessObj->SetParam("@_SRC_IP", conn->ip_addr.c_str(),
			Http_ParseObj::STR_COPY_OBJ);		//初始化相关req值。
	conn->ProcessObj->SetParam("@_SRC_PORT", conn->ip_port.c_str(),
			Http_ParseObj::STR_COPY_OBJ);		//初始化相关req值。
	conn->ProcessObj->SetParam("@_THIS_NODE_NAME", GB_ThisNodeName.c_str(),
			Http_ParseObj::STR_COPY_OBJ);		//初始化相关req值。
	conn->SetActive(true);
	//receive 初始化
	buffer_case_init(conn);

	//bufferevent_setfd(bev,fd);
	bufferevent_setcb(bev, echo_read_cb, echo_write_cb, echo_event_cb, conn);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
	GB_Gateway->TimeTrack("ACCEPT", ParseProcess::TRACK_END);
	//bufferevent_enable(bev, EV_READ);
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx) {
    //cout << "_"; //libevent test
	struct event_base *base = evconnlistener_get_base(listener);
	int err = EVUTIL_SOCKET_ERROR();
	//fprintf(stderr, "Got an error %d (%s) on the listener.Shutting down.\n", err, evutil_socket_error_to_string(err));
	WIZBASE_DEBUG((string)"Error:"+"Got an error "+STRING_Helper::iToStr(err)+"("+evutil_socket_error_to_string(err)+") on the listener.Shutting down.");

	event_base_loopexit(base, NULL);
}

//以下为时间循环
struct event* g_time_limit_event[MAX_TIME_ARRAY];
int TimePrcArray[MAX_TIME_ARRAY];
int TimePrcCounter[MAX_TIME_ARRAY]; //ly counter
void TimeLimitCallback(evutil_socket_t fd, short what, void* arg) {
	WizBase_pthread_mutex_lock(&GB_Lock);
	int *proc_num = (int *) arg;
	string node_path = (string) "/XMLParse/Task/__TimeProcess["
			+ STRING_Helper::iToStr((*proc_num) + 1) + "]";

	//调用时间循环，如果有的话
	GB_Gateway->SendCBFunc = obj_send_cb2;
	GB_Gateway->SendCBFuncHandle = NULL;

	//时间计数处理 ly counter
	//取得CountMax
	int cnt_max = 0;
	const char *count_max = GB_Gateway->GetObjFileNodeAttr(node_path.c_str(),
			"CountMax");
	if (count_max != 0 && strlen(count_max) != 0)
		cnt_max = STRING_Helper::ParseInt(count_max);		//取得CountMax
	//累加PROCESS_COUNTER
	if (cnt_max != 0 && ((TimePrcCounter[*proc_num]) <= cnt_max))
		(TimePrcCounter[*proc_num])++;   //计数累加
	else
		(TimePrcCounter[*proc_num]) = 0;
	//传递当前count
	GB_Gateway->SetParam("@_PROCESS_COUNTER", TimePrcCounter[*proc_num]);

	//时间循环处理
	//测试是否执行,ly counter
	const char *act_test = GB_Gateway->GetObjFileNodeAttr(node_path.c_str(),
			"Test");
	string err_str(""), act_test_str(act_test);
	if (strlen(act_test) != 0 && strlen(act_test) != 0
			&& !GB_Gateway->TestConditionStr(act_test_str, err_str)) {
		if (err_str.size() != 0)
			cout << err_str << endl;
//        else
//            cout << "."; //libevent test
	} else {		//执行循环操作
		//cout << endl;
		GB_Gateway->GoAction(node_path.c_str());
	}

	//结束时间循环
	if (fd != 0) { //???
		event_del(g_time_limit_event[*proc_num]);
		set_time_limit(proc_num);
	}
	WizBase_pthread_mutex_unlock(&GB_Lock);
	//(string)"/XMLParse/Task/__TimeProcess["+STRING_Helper::iToStr(i+1)+"]"
}

static void set_time_limit_array() {
	//初始化
	for (int i = 0; i < MAX_TIME_ARRAY; i++) {
		g_time_limit_event[i] = 0;
	}

	//循环取得时间节点
	ParseXml_NodeSet tmps = GB_Gateway->GetXmlNodes(
			"/XMLParse/Task/__TimeProcess");
	for (size_t i = 0; i < tmps.size() && i < MAX_TIME_ARRAY; i++) {
		//保存指针
		string node_path = (string) "/XMLParse/Task/__TimeProcess["
				+ STRING_Helper::iToStr(i + 1) + "]";
		const char *p2 = GB_Gateway->GetObjFileNodeAttr(node_path.c_str(),
				"RunStart");
		TimePrcArray[i] = i;
		if (strstr(p2, "true") == p2) {
			TimeLimitCallback(0, 0, &(TimePrcArray[i]));
		}
		//设定回调
		set_time_limit(&(TimePrcArray[i]));
		(TimePrcCounter[i]) = 0; //ly counter
	}
}

static void set_time_limit(int *proc_num) { //循环取得所有TimeProcess节点，并驱动连接。

	string node_path = (string) "/XMLParse/Task/__TimeProcess["
			+ STRING_Helper::iToStr((*proc_num) + 1) + "]";
	//取得时间
	const char *p = GB_Gateway->GetObjFileNodeAttr(node_path.c_str(),
			"TimeValue");
	string time_val_str = (p == 0 ? "" : p);
	int i_time =
			time_val_str != "" ?
					STRING_Helper::StrToi(time_val_str) : Default_Time_Routin;
	struct timeval tv = { i_time, 0 };

	//设定循环
	g_time_limit_event[*proc_num] = evtimer_new(GB_EventBase[0],
			&TimeLimitCallback, (void* )proc_num); //only for lpread 0 set time, we coan set other spread later...
	evtimer_add(g_time_limit_event[*proc_num], &tv);
}

//以下为连接通用时间循环
struct event* g_conn_limit_event = 0;
//ly_send_queue
string WIZBASE_create_connect_obj(const char *obj_path_name, string &conn_str1, map<string, string> *tmp_add_paras,const char *dest_host_add,const char *new_obj_name) {
	ParseXml_NodeSet nodes = GB_Gateway->GetXmlNodes(obj_path_name);
	string err_str;
	size_t conn_thread_n = 1;

	if (nodes.size() == 0)
		err_str ="Error(createWIZBASE_create_connect_obj):can't find connect obj at '"
						+ (string) obj_path_name + "'";
	for (ParseXml_NodeSet::const_iterator it = nodes.begin(); it != nodes.end();
			++it) {
		bool err = false;
		int ssl_type=WIZBASE_SSL_TYPE_NOSSL; //no ssl
		pugi::xpath_node xnode = *it;
		ParseXml_Node actions_node = xnode.node();

		const char *obj_name = GB_Gateway->GetXmlNodeAttr(actions_node, "Name");
		const char *on_ssl = GB_Gateway->GetXmlNodeAttr(actions_node, "SSL");

        if (on_ssl!=NULL && strlen(on_ssl)!=0) {
            if (strstr(on_ssl, "OpenSSL") == on_ssl )
                ssl_type = WIZBASE_SSL_TYPE_OPENSSL;   //ly ssl on OpenSSL
            else
                ssl_type = WIZBASE_SSL_TYPE_EVENTSSL;    //EventFilter;
        }
		ParseXml_Node cfg_node=actions_node.child("_SSL_CFG");
		if (cfg_node){
			ssl_type=WIZBASE_SSL_TYPE_CFGSSL;   //ly ssl on OpenSSL; chang later cfg with OPENSSL and EVENTSSL
		};
        //ly_send_queue
        const char *dest_host;
        if (dest_host_add!=NULL && dest_host_add[0]!=0)
            dest_host=dest_host_add;
        else
            dest_host = GB_Gateway->GetXmlNodeAttr(actions_node,"DestHost");

		if (strlen(dest_host) == 0 || strlen(obj_name) == 0) {
			WIZBASE_DEBUG((string)"//XMLParse/Connect/CONNECT[@Name=***]'s attributes DestHost & Name should not be empty! ");
			err = true;
		}
		string conn_str = GB_Gateway->GetXmlNodeAttr(actions_node, "ConnStr");
		const char *thread_conn_p = GB_Gateway->GetXmlNodeAttr(actions_node,"ThreadCount");

		if (thread_conn_p != 0 && strlen(thread_conn_p) != 0) { //ly thread
            //if ((conn_thread_n = STRING_Helper::ParseULong(thread_conn_p)) >= 0&& conn_thread_n > WIZBASE_MAX_CONN_THREAD) {
            if (!STRING_Helper::CheckNum(thread_conn_p) ||((conn_thread_n = STRING_Helper::ParseULong(thread_conn_p)) && conn_thread_n > WIZBASE_MAX_CONN_THREAD) ) {
                WIZBASE_DEBUG((string)"//XMLParse/Connect/CONNECT[@Name=***]'s attributes ThreadCount error! ");
                err = true;
            }
			for (size_t i = 1; i < conn_thread_n + 1; i++) {
				string tmp_obj_name = (string) obj_name + "."
						+ STRING_Helper::SetNumToID(
								STRING_Helper::iToStr(i, 16),
								WIZBASE_MAX_CONN_THREAD_WIDTH, "0");
				Conn *tmp_conn;
				if ((tmp_conn = GB_ConnQueue->FindConnByObjName(
						tmp_obj_name.c_str(), ConnQueue::TestObjName, true))
						== NULL && !err) {
					//如果未发现CONNECT 对应的对象则启动连接，创建对象
					Conn *conn=do_conntion_obj( conn_str1.size()!=0 ? conn_str1: conn_str, dest_host, tmp_obj_name.c_str(), obj_name, ssl_type,cfg_node); //obj_name_orign(obj_name)用于xpath检索
					if (conn==NULL){
                        WIZBASE_DEBUG((string)"//XMLParse/Connect/CONNECT[@Name='"+obj_name +"']'s connect error!");
						err=true;
					}else if (tmp_add_paras!=NULL && tmp_add_paras->size()!=0)
						conn->LaterProcessObjAddParams(tmp_add_paras);
				}
			}
		} else {
			const char *tmp_obj_name=(new_obj_name==NULL||new_obj_name[0]==0 ? obj_name: new_obj_name);
			if (!GB_ConnQueue->FindConnByObjName(tmp_obj_name,
					ConnQueue::TestOrignName) && !err) {
				//如果未发现CONNECT 对应的对象则启动连接，创建对象
				Conn *conn=do_conntion_obj(conn_str1.size()!=0 ? conn_str1 : conn_str, dest_host, tmp_obj_name, obj_name,ssl_type,cfg_node);
				if (conn==NULL){
					WIZBASE_DEBUG((string)"//XMLParse/Connect/CONNECT[@Name='" +obj_name +"'] connect error!");
					err=true;
				}else if (tmp_add_paras!=NULL && tmp_add_paras->size()!=0)
					conn->LaterProcessObjAddParams(tmp_add_paras);
			}
		}

	}
	return err_str;
}

void ConnLimitCallback(evutil_socket_t fd, short what, void* arg) {
	WizBase_pthread_mutex_lock(&GB_Lock);
	string err_str, act_path("Connect");

	//调用连接循环
	string conn_str("");
	WIZBASE_create_connect_obj("//XMLParse/Connect/CONNECT[@Method='Connect']",conn_str);

	//结束时间循环
	event_del(g_conn_limit_event);
	set_conn_limit();
	WizBase_pthread_mutex_unlock(&GB_Lock);
}

static void set_conn_limit() {
	g_conn_limit_event = evtimer_new(GB_EventBase[0], &ConnLimitCallback, NULL); //only for spread 0; for other spread later; when  ssl port connect
	//循环取得所有TimeProcess节点，并驱动连接。
	ParseXml_NodeSet nodes = GB_Gateway->GetXmlNodes(
			"/XMLParse/Task/__ConnProcess");
	string conn_val_str("");
	if (nodes.size() != 0) {
		const char *p = GB_Gateway->GetObjFileNodeAttr(
				"/XMLParse/Task/__ConnProcess", "TimeValue");
		conn_val_str = (p == 0 ? "" : p);
	}

	// int i_time =conn_val_str != "" ? STRING_Helper::StrToi(conn_val_str) : Default_Conn_Routin;
    int i_time=0,u_time=0;;
    if ((conn_val_str.size()!=0) && STRING_Helper::CheckNum(conn_val_str,"num")){
        //如果是小数
        if (STRING_Helper::PUB_FindStrIs(conn_val_str, ".")){
            vector<string> time_vs;
            STRING_Helper::stringSplit(conn_val_str, time_vs, ".");
            i_time=STRING_Helper::StrToi(time_vs[0]);
            u_time=(STRING_Helper::StrTof(conn_val_str)-i_time)*1000000;
        }else
            i_time=STRING_Helper::StrToi(conn_val_str);
    }else
        i_time=Default_Conn_Routin;
    struct timeval tv = { i_time, u_time};
    
	//struct timeval tv = { i_time, 0 };
	evtimer_add(g_conn_limit_event, &tv);
}


//time_task
void WIZBASE_CancleQueueTimeTask(Conn *ctx){
    evtimer_del(ctx->time_ev);
    ctx->time_ev=NULL;
}

//time_task
void ConnQueueCallback(evutil_socket_t fd, short what, void* arg) {
    Conn *ctx=(Conn *)arg;
    string send_act_node =(string)"/XMLParse/Connect/CONNECT[@Name='"+ ctx->obj_name_orign + "']/_TIME_TASK";
//    if (GB_ConnQueue->FindConnByObjName(ctx->obj_name_orign.c_str(), ConnQueue::TestOrignName) == NULL)
//        return; //面向连接的时间循环，有可能连接中断的情况，必须排除

    //测试是否执行 
    ParseXml_Node node = GB_Gateway->GetXmlNode(send_act_node.c_str());
    if (!node)
        return;
    const char *act_test = GB_Gateway->GetXmlNodeAttr(node,"Test");
    string err_str(""), act_test_str(act_test);
    if (strlen(act_test) != 0 && strlen(act_test) != 0 && !ctx->ProcessObj->TestConditionStr(act_test_str, err_str)) {
        if (err_str.size() != 0)
            cout << err_str << endl;
        else
            cout << "."; //libevent test
        return;
    }
    
    //执行
    ctx->ProcessObj->DoProcess(send_act_node.c_str(), obj_send_cb2, (void *) ctx);//!!!buffer?可去除conn2->buffer输入
}

//time_task
int WIZBASE_StartQueueTimeTask(Conn *ctx){
    //查询TimeTask节点
    string act_node=(string)"/XMLParse/Connect/CONNECT[@Name='"+ ctx->obj_name_orign + "']/_TIME_TASK";
    ParseXml_Node node = GB_Gateway->GetXmlNode(act_node.c_str());
    if (!node)
        return -1;
    
    //ctx->time_ev = evtimer_new(GB_EventBase[0], &ConnQueueCallback, ctx); //only for spread 0; for other spread later; when  ssl port connect
    ctx->time_ev = event_new(GB_EventBase[0],  -1, EV_PERSIST, &ConnQueueCallback, ctx);
    
    //取得时间属性
    string conn_val_str("");
    const char *p = GB_Gateway->GetXmlNodeAttr(node, "TimeValue");
    conn_val_str = (p == 0 ? "" : p);
    
    //取得时间值
    int i_time=0,u_time=0;;
    if ((conn_val_str.size()!=0) && STRING_Helper::CheckNum(conn_val_str,"num")){
        //如果是小数
        if (STRING_Helper::PUB_FindStrIs(conn_val_str, ".")){
            vector<string> time_vs;
            STRING_Helper::stringSplit(conn_val_str, time_vs, ".");
            i_time=STRING_Helper::StrToi(time_vs[0]);
            u_time=(STRING_Helper::StrTof(conn_val_str)-i_time)*1000000;
        }else
            i_time=STRING_Helper::StrToi(conn_val_str);
    }else
        i_time=Default_Conn_Routin;
        
    //i_time = ((conn_val_str != "") && STRING_Helper::CheckNum(conn_val_str) ? STRING_Helper::StrToi(conn_val_str) : Default_Conn_Routin ); //struct timeval tv = { i_time, 0 };
    //设置时间循环
    struct timeval tv = { i_time, u_time};
    evtimer_add(ctx->time_ev, &tv);
    return 0;
}

static bool util_get_ipaddr(struct sockaddr *address, int socklen, Conn *conn,
		string &ip_addr, string &ip_port) {
	/* Extract and display the address we're listening on. */
	char addrbuf[128];
	void *inaddr;
	const char *addr;
	int got_port = -1;
	//char uri_root[512];

	if (address->sa_family == AF_INET) {
		got_port = ntohs(((struct sockaddr_in*) address)->sin_port);
		inaddr = &((struct sockaddr_in*) address)->sin_addr;
	} else if (address->sa_family == AF_INET6) {
		got_port = ntohs(((struct sockaddr_in6*) address)->sin6_port);
		inaddr = &((struct sockaddr_in6*) address)->sin6_addr;
	} else {
		fprintf(stderr, "Weird address family %d\n", address->sa_family);
		return false;
	}
	addr = evutil_inet_ntop(address->sa_family, inaddr, addrbuf,
			sizeof(addrbuf));
	if (addr) {
		ip_addr = addr;
		ip_port = STRING_Helper::iToStr(got_port);
		//ip_port_val = got_port;
	} else {
		fprintf(stderr, "evutil_inet_ntop failed\n");
		return false;
	}
	return true;
}

int WIZBASE_SysInitGBParams(const char *c_ports,const char*data_path, const char *work_path,const char* dbconfig_xml, const char *gateway_xml, const char *node_name, const char  *sys_cert,  const char *sensor_dbm){
	//1: Init GB_Port Num
    vector<string> port_s;
    if (c_ports!=NULL) {
        string str_ports = c_ports;
        STRING_Helper::stringSplit(str_ports, port_s, ".");
        for (size_t i = 0; i < port_s.size(); i++) {
            GB_Ports[i] = atoi(port_s[i].c_str());
            if (GB_Ports[i] <= 0 || GB_Ports[i] > 65535) {
                WIZBASE_DEBUG((string)"Invalid port num!"+STRING_Helper::iToStr(GB_Ports[i]));
                return -1;
            }
        }
        GB_Ports_NUMBER = (int)port_s.size();
    } else {
        GB_Ports[0] = 8090;
        GB_Ports_NUMBER = 1;
    }

    //设置缺省路径
    if (work_path!=NULL){
        GB_WorkPath=work_path;
#ifdef WIN32
        _chdir(work_path);
#else
        if (chdir(work_path)==-1){
            WIZBASE_DEBUG((string)"change work path error!");
            exit(2);
        }
#endif
    }
    char xxx[1024];//获取当前目录并打印
#ifdef WIN32
    _getcwd(xxx,1024);
#else
    getcwd(xxx,1024);
#endif
    WIZBASE_DEBUG((string)"working path:"+xxx);
    
    //2: Init Gateway.xml
    if (gateway_xml!=NULL) {
        GB_ParseCfg = gateway_xml;
    } else {
        GB_ParseCfg = GATEWAY_XML;
    }
    GB_ParseXml = new Http_ParseXML(GB_ParseCfg.c_str());    //ly obj_new
    if (!(GB_ParseXml->LoadSuccess())) {
        WIZBASE_DEBUG((string)"---Init Gateway Error: can't load "+GB_ParseCfg+"!");
    }
    GB_Gateway = new ObjectProcess();
    cout << (string) "Loading:" + GB_ParseCfg;
#ifdef XML_ALL_LOAD
    if (GB_Gateway->LoadObjectFile(GB_ParseCfg.c_str())==false) {
#else
	if (GB_Gateway->LoadObject(GB_ParseXml) == false) { //ly obj_new
#endif
        WIZBASE_DEBUG((string)"---Init Gateway Error:"+GB_Gateway->ErrorStr);
        return -2;
	} else {
		WIZBASE_DEBUG((string)"---Init Gateway Success ! ");
	}
	GB_Gateway->_PubObj->SetNodeValue("_THIS_CFG_XML_",
									  (const char *) GB_Gateway->_ActParseXml->XmlToStr(
																						GB_Gateway->_ActParseXml->ObjXml).c_str(),
									  Http_ParseObj::XML_POINT_OBJ);

    //3: GetNodeName
    if (node_name!=NULL) {
            GB_ThisNodeName = node_name;
        } else {
            while (1) {
                if (GB_ParseCfg.rfind(".") != std::string::npos) {
                    string tmp_str = GB_ParseCfg.substr(0, GB_ParseCfg.rfind("."));
                    if (tmp_str.rfind("/") != std::string::npos) {
                        GB_ThisNodeName = tmp_str.substr( GB_ParseCfg.rfind("/") + 1);
                        break;
                    } else if (tmp_str.rfind("\\") != std::string::npos) {
                        GB_ThisNodeName = tmp_str.substr( GB_ParseCfg.rfind("\\") + 1);
                        break;
                    } else {
                        GB_ThisNodeName = tmp_str;
                    }
                } else
                    GB_ThisNodeName = GB_ParseCfg;
                break; 
            }
        }
	WIZBASE_DEBUG((string)"Set Gateway Name:"+"["+GB_ThisNodeName+"]");

    //4: Init httpClientCert;
    if (sys_cert!=NULL) {
            GB_HpptsClientCert = sys_cert;
        }else
            GB_HpptsClientCert = "/etc/ssl/certs/ca-certificates.crt";

    //5: SENSOR_DBM
#ifdef SENSOR_DBM
    if (sensor_dbm!=NULL) {
        HTTP_Config::SensorDB=new SensorDBManager(sensor_dbm);
    } else {
        HTTP_Config::SensorDB=new SensorDBManager(sensor_dbm);
    }
#endif

    //6: Init third party system
#ifdef THIRD_QQ
        ThirdQQ qq;
        GB_ThirdMap.insert(pair<string, ThirdBase*>((string )"QQ",&qq));
#endif

        //7: Init AvartarDB
        string path=data_path==NULL? "": data_path;
        if (dbconfig_xml!=NULL) {
            GB_AvartarDB = new AvartarDBManager((char *) dbconfig_xml, 0, path);
        }else {
            const char *cfg="./DBConfig.xml";
            GB_AvartarDB = new AvartarDBManager((char *)cfg, 0, path);
        }
        if (GB_AvartarDB->ErrorStr!=""){
            WIZBASE_DEBUG((string)"---Init GB_AvartarDB Error:" +GB_AvartarDB->ErrorStr);
        }

    //8:orther Object Init
        ConnSSL::CONN_PROTOCOL_ENV_INIT(); //初始化SSL环境
        GB_SRC_SCTX = new ConnSSL(ConnSSL::CONN_SSL_CTX_METHOD_SERVER);
        GB_CLI_SCTX = new ConnSSL(ConnSSL::CONN_SSL_CTX_METHOD_CLIENT_HTTPS,NULL,GB_HpptsClientCert.c_str());//ly memleak2
        //GB_DNS_BASE =evdns_base_new(GB_EventBase[0], 1);
        GB_ConnQueue = new ConnQueue();
        GB_SessionBuf = new Session();
    return 0;
        
}

#define SVR_IP                          "0.0.0.0"
#define SVR_PORT                        10000
#define BUF_SIZE                        1024
 /*
void read_cb(int fd, short event, void *arg) {
    char                buf[BUF_SIZE];
    int                 len;
    int                 size = sizeof(struct sockaddr);
    struct sockaddr_in  client_addr;
    memset(buf, 0, sizeof(buf));
    len = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &size);
    if (len == -1) {
        perror("recvfrom()");
    } else if (len == 0) {
        printf("Connection Closed\n");
    } else {
        printf("Read: len [%d] - content [%s]\n", len, buf);
        
        
        sendto(fd, buf, len, 0, (struct sockaddr *)&client_addr, size);
    }
}
    
int UDP_RunLibEvent(struct event *ev) {
    
    int                 sock_fd;
    int                 flag = 1;
    struct sockaddr_in  sin;
    
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }
    
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0) {
        perror("setsockopt()");
        return 1;
    }
    
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(SVR_IP);
    sin.sin_port = htons(SVR_PORT);
    
    if (bind(sock_fd, (struct sockaddr *)&sin, sizeof(struct sockaddr)) < 0) {
        perror("bind()");
        return -1;
    } else {
        printf("bind() success - [%s] [%u]\n", SVR_IP, SVR_PORT);
    }
    
    event_set(ev, sock_fd, EV_READ | EV_PERSIST, &read_cb, NULL);
    if (event_add(ev, NULL) == -1) {
        printf("event_add() failed\n");
    }
    return 0;
}*/
    
void NormalUDPClient(){
    int connFd = -1;
    struct sockaddr_in servAddr;
    struct sockaddr_in peer;
    
#define UDP_BUF_LEN 2048
    char buffRecv[UDP_BUF_LEN];
    
    connFd = socket(AF_INET, SOCK_DGRAM, 0);
    
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(0);
    //servAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    cout << "new thread: waiting for udp......" << endl;
    for (;;) {
        //sendto(connFd, buff, sizeof(buff), 0, (struct sockaddr *)&servAddr, sizeof(servAddr));
        socklen_t add_len=sizeof(peer);
        long recvLen = recvfrom(connFd, buffRecv, UDP_BUF_LEN-1, 0, (struct sockaddr *)&peer, &add_len);
        buffRecv[recvLen] = '\0';
        cout <<"!!!udp received: " <<buffRecv << endl;
        std::ofstream file("./udp_buff.txt", ios::binary | ios::out | ios::app);
            file.write((const char *)buffRecv, sizeof(buffRecv));
        file.close();
    }
    close(connFd);
}
    
void *WIZBASE_RunUDP(void *arg) {
//初始化 WINsocket
#ifdef WIN32
    WSADATA WSAData;
    WSAStartup(0x101, &WSAData);
#endif

    //再次添加处理程序
    NormalUDPClient();
    
#ifdef _WIN32
    WSACleanup();
#endif
    return NULL;
}
    
void *WIZBASE_RunOther(void *arg) {//ly new thread
        //初始化 WINsocket
#ifdef WIN32
        WSADATA WSAData;
        WSAStartup(0x101, &WSAData);
#endif
        //初始化Libevent
        int num = *((int *) arg);
        GB_EventBase[num] = event_base_new();  //one event_base one thread;
        if (!GB_EventBase[num]) {
            WIZBASE_DEBUG((string)"Couldn't open Gable event base!");
            return NULL;
        }
        //在此添加处理程序
        
        //关闭libevent
        event_base_dispatch(GB_EventBase[num]);
        //关闭WinSocket
#ifdef _WIN32
        WSACleanup();
#endif
        return NULL;
}

#define ERROR_NETWORK_PKG 1
#define ERROR_TYPE_NORMAL 0
static void ERROR_LOG(const char *s,unsigned char type,unsigned char deep){
    cout << s << "type(" <<type <<  ") deep (" << deep << endl;
}
    
static void pipesig_handler(int s){
    string str=(string)"SIGPIPE ignore(" +STRING_Helper::iToStr(s)+")!!!!";
    ERROR_LOG(str.c_str(),ERROR_NETWORK_PKG,ERROR_TYPE_NORMAL);
}

void *WIZBASE_RunLibEvent(void *arg) {

#ifdef WIN32
	WSADATA WSAData;
	WSAStartup(0x101, &WSAData);
#else
	//if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	//return (1);
    struct sigaction act;
    act.sa_handler = pipesig_handler;
    if (sigaction(SIGPIPE, &act, NULL) == 0) {
        cout << "SIGPIPE ignore set for tmp socket close!" << endl;
    }
    signal(SIGPIPE, pipesig_handler);
#endif
    //phread num;
	int num = *((int *) arg);
	GB_EventBase[num] = event_base_new();  //one event_base one thread;
	if (!GB_EventBase[num]) {
		WIZBASE_DEBUG((string)"Couldn't open Gable event base!");
		return NULL;
	}

	//本机IP
#ifdef WIN32
	LW_GetLocalHostName(GB_LocalHostName);
	if (GB_LocalHostName.size() != 0)
		LW_GetHostAddress(GB_LocalHostAddr);
	cout << "This machine name:" << GB_LocalHostName << " addr:" << GB_LocalHostAddr << endl;
#else
    LW_GetLocalMac(GB_LocalHostMac);
    LW_GetHostAddress(GB_LocalHostAddr);
    cout << "This machine mac:" << GB_LocalHostMac << " addr:" << GB_LocalHostAddr << endl;
#endif
    //struct evconnlistener *Listener[WIZBASE_NUM_THREADS]
	struct evconnlistener *Listener[WIZBASE_NUM_PORTS]; //ly new pthread
	struct sockaddr_in sin;

	//for (int i = 0; i < GB_Ports_NUMBER && i < WIZBASE_NUM_THREADS; i++) {
    for (int i = 0; i < GB_Ports_NUMBER && i < WIZBASE_NUM_PORTS; i++) { //ly new pthread
		//2.设定ip被动接受状态
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = htonl(0);
		sin.sin_port = htons(GB_Ports[i]);
		int *p_port = &(GB_Ports[i]);
		//3.set listen
		Listener[i] = evconnlistener_new_bind((event_base *) GB_EventBase[0],
				accept_conn_cb, (void *) p_port,
				LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10,
                                              (struct sockaddr*) &sin, sizeof(sin));//ly crash 1024->10;ly new pthread
        if (!Listener[i]) {
            //cout << "Couldn't create listener at port" << GB_Ports[i] << "..."<< endl;
            WIZBASE_DEBUG((string)"---Couldn't create listener at port:"+STRING_Helper::iToStr(GB_Ports[i]));
            ConnSSL::CONN_PROTOCOL_ENV_CLOSE(); //初始化SSL环境
            free(GB_SRC_SCTX);
            exit(-1);
            //return NULL;
        } else {
            //cout << "Start listen at port:" << GB_Ports[i] << "..." << endl;
            WIZBASE_DEBUG((string)"---Start listener at port:"+STRING_Helper::iToStr(GB_Ports[i]));
        }
		evconnlistener_set_error_cb(Listener[i], accept_error_cb);
	}

	//3.设定时间回调函数
	if (num == 0) {	//only for main pthread!
		set_time_limit_array();
		set_conn_limit();
	}
	//cout << "run system..." << endl;
	event_base_dispatch(GB_EventBase[num]);

	//clear listener and ssl_obj
    //for (int i = 0; i < GB_Ports_NUMBER && i < WIZBASE_NUM_THREADS; i++)
	for (int i = 0; i < GB_Ports_NUMBER && i < WIZBASE_NUM_PORTS; i++)
		evconnlistener_free(Listener[i]);//ly new pthread

    event_base_free(GB_EventBase[num]); //ly new pthread update
	ConnSSL::CONN_PROTOCOL_ENV_CLOSE(); //初始化SSL环境
	free(GB_SRC_SCTX);

#ifdef _WIN32
	WSACleanup();
#endif
    return NULL;
}
    
int WIZBASE_RunGBParams(int argc, char **argv) {
    //string work_path="";
        //测试大头和小头
        if (IsLittleEndian()) {
            printf("is little endian!\n");
        } else {
            printf("is big endian!\n");
        }
        const char *c_ports=NULL, *gateway_xml=NULL, *node_name=NULL, *sys_cert=NULL,*sensor_dbm=NULL,*work_path=NULL;
        //
        if (argc>1)
            c_ports=argv[1];
        if (argc>2)
            gateway_xml=argv[2];
        if (argc > 3 && strlen(argv[3]) > 1)
            node_name=argv[3];
        if (argc > 4 && strlen(argv[4]) > 1)
            sys_cert=argv[4];
        if (argc > 5 && strlen(argv[5]) > 1)
            work_path=argv[5];
#ifdef SENSOR_DBM
        if (argc > 6)
            sensor_dbm=argv[6];
#endif
    
        return WIZBASE_SysInitGBParams(c_ports,NULL,work_path,NULL,gateway_xml, node_name, sys_cert,  sensor_dbm);
}
    
    
    
/*
    bool GetMacAddr(const vector<string>& vNetType,string& strip)
    {
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
                    printf("%02x",macaddr[k]);
                    if(k<5)
                        printf(":");
                }
            }
        }
        return false;
    }
 */
 
