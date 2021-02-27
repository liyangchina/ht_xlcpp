#ifndef PARSE_PROCESS_H
#define PARSE_PROCESS_H
#pragma once
#include <map>
#include <vector>
#include <string>
#include "pugixml.hpp"
//#include "XML_Mini.h"
#include "ParseXml.h"
#include "ParseObj.h"
#include "Linux_Win.h"
#include "ParseBase.h"
#include "FileQueue.h" 

using namespace pugi;
using namespace std;

//#define LW_BIT_STR "bin"
//#define LW_BINARY_STR "char"

//typedef string (*ParseProcess_T)(ParseXml_Node parse_node,PV_AddParas_T &add_paras); //(用于存放para字符串和指针信息。)
//
//Http_ParseObj PP_GB_PARAM;
typedef int (*OBJ_PROCESS_SendCBFunc_T)(const char *msg, int msg_len,const char *dest,const char *src,int trans_type,bool close_after_send,void *trans_handle); //(用于存放para字符串和指针信息。)

#define ParseProcess_XML_ERROR_CODE_PATH "XMLParse/ErrorCode"
class ParseProcess : public ParseBase
{
public:
    //string NULL_STRING= "";
    //UInt64 ProcessStart; //时间测试
    //UInt64 ProcessCount;
    typedef map<string, string> PV_AddParas_T;//对于ParseValue的增补参数（@THIS,@PARENT类）。
    
    /*ParseProcess(void){
     //this->_PubObj = new Http_ParseObj();	//创建对象
     //this->_ActParseXml = new Http_ParseXML();
     };*/
    void setAddress(const string &ip, int port);
    class Http_ParseFunc;
    class Http_ParsePacket;
    ParseXml_Node CfgNode;

    OBJ_PROCESS_SendCBFunc_T SendCBFunc;
    void *SendCBFuncHandle; //bufferevent or tcp handle
    
    //~ParseProcess(void);
    enum {
        SEND_SUCCESS = 0,
        SEND_CFG_ERROR = -1,
        SEND_IP_CFG_ERROR = -8,
        SEND_CONNECT_ERROR = -2,
        SEND_SEND_OUTIME_ERROR = -3,
        SEND_SEND_ERROR = -4,
        SEND_RECEIVE_ERROR_TIMEOUT = -7,
        SEND_RECEIVE_TIMEOUT = 3,
        SEND_RECEIVE_STREAM_CLOSE = 4,
        SEND_RESULT_NULL = 1,
        SEND_RESULT_ERROR = 2,
        
        TRANS_TYPE_NORMAL = 0,
        TRANS_TYPE_HT = 1,
        RCV_TYPE_HTTP = 0,
        RCV_TYPE_BEGIN_END = 1,
        RCV_TYPE_NO_RCV = 2,
        TRACK_START = 0,
        TRACK_END = 1
    };
    //功能：主处理程序，
    //动作：(1)接收请求动作吧相应的req.uri,req.host,req等内容的指针写入_REQ_URI,_REQ_DATE_,REQ_HOST等对象
    //		(2)根据uri:./PARSE/ACTION1打开，Load ActionParseXml(./PARSE/ACTION1.XML)动作配置文件。注：今后需要把改文件放入全局变量，或者通过链表的形式打开一次不再打开。
    //		(3)初始化_PubObj_对象。
    //		(4)定位ActParseXml中第一个ACTIONS节点。
    //		(5)调用ParseAction()解析第一个ACTIONS节点
    //参数：request:输入对象；response:输出对象；
    //virtual int doPost(HTTP_Request &request,HTTP_Response &response);
    
    //功能：-循环parse_node节点的子节点，进行相关动作节点解析。
    //		-CASE ACTIONS: ParseAction递归调用ACTIONS的下级节点解析。
    //		-CASE SET_OBJ: 则取得SET_OBJ的子节点，相关属性发送SET_OBJ函数；
    //		-CASE _CASE:其它函数同SET_OBJ;
    //注：	自身可以被ACTIONs和_CASE_和_FOR_等动作递归调用。
    bool TimeTrack(const char *process_name, BYTE method);
    virtual int ParseAction(ParseXml_Node parse_node,PV_AddParas_T &add_paras);
    
    int ACTION(ParseXml_Node parse_node,PV_AddParas_T &add_paras);
    int PARSE(ParseXml_Node parse_node,PV_AddParas_T &add_paras);
    //bool _PARSE_SetObjValue(string &name,string &value);
    
    //功能：针对ActParseXml 中SET_OBJ节动作；主要是吧来源于node.value的字串，进行解析后，设置入to_obj对象节点中。
    //参数：parse_node_xpath:当前ActParseXml所处配置节点;to_obj为设置的目标节点;parse_type解析方式.
    //动作：(1)CASE STR2OBJ:把字符串解析后写入pub_obj（先）
    //		(2)CASE XML2OBJ:吧xml节点解析后对应生成pub_obj的节点写入pub_obj
    //		(3)CASE OBJ2XML:吧obj节点解析后对应生成xml字符串。
    //返回：ACT_SUCCESS成功;否则失败。
    int SET_OBJ(ParseXml_Node parse_node,PV_AddParas_T &add_paras);//通过节点定位的节点。
    int PRINT(ParseXml_Node parse_node,PV_AddParas_T &add_paras);//通过节点定位的节点。
    int DEL_OBJ(ParseXml_Node parse_node,PV_AddParas_T &add_paras);//通过节点定位的节点。
#ifdef SENSOR_DBM
    int DB_WRITE_BKDB(ParseXml_Node parse_node,PV_AddParas_T &add_paras);//旧的读取BerkleyDB数据库。
    int DB_SEARCH(ParseXml_Node parse_node,PV_AddParas_T &add_paras);  //通用数据库读取接口。
#endif
    int DB_WRITE(ParseXml_Node parse_node,PV_AddParas_T &add_paras);  //通用数据库读取接口。
    int _DB_WRITE_FUNCTION(ParseXml_Node parse_node, PV_AddParas_T &add_paras);//取得ParseDB定义的数据库CONTENT结果。
    int FUNCTION(ParseXml_Node parse_node, PV_AddParas_T &add_paras);

    int CALL(ParseXml_Node parse_node,PV_AddParas_T &add_paras);//调用函数节点
    int SEND(ParseXml_Node parse_node,PV_AddParas_T &add_paras);//调用函数节点
    int SET_QUEUE(ParseXml_Node parse_node, PV_AddParas_T &add_paras);//调用函数节点
    int DB_QUERY(ParseXml_Node parse_node, PV_AddParas_T &add_paras);//调用函数节点
    int DB_QUERY_EXT(ParseXml_Node parse_node, PV_AddParas_T &add_paras);//调用函数节点
    //int DO_QUEUE(ParseXml_Node parse_node, PV_AddParas_T &add_paras);//调用函数节点
    
    //功能：路线判断函数;
    //动作：
    //`	- 循环所有condition节点(为and关系;)
    //		//循环condition所有子节点。
    //		(1)CASE @tag=”include”;判断是否所指定的节点包含。
    //      (2)CASE @tag=”!include"
    //	- 组合上述判断条件(****or*****)and(****,****)
    //  - 如果false则什么也不做，如果true则调用parse_node下其它的节点递归ParseAction
    int SWITCH(ParseXml_Node parse_node,PV_AddParas_T &add_paras);
    int SWITCH_CASE(ParseXml_Node parse_node,bool is_default,PV_AddParas_T &add_paras);
    int SWITCH_CASE_SIMPLE(ParseXml_Node parse_node, bool is_default, PV_AddParas_T &add_paras);
    int SWITCH_CASE_TEST(ParseXml_Node parse_node,PV_AddParas_T &add_paras);
    int SWITCH_CASE_TEST_CONDITION(ParseXml_Node parse_node,PV_AddParas_T &add_paras);
    
    int FOR(ParseXml_Node parse_node, PV_AddParas_T &add_paras);
    
    //从ParseAction开始可采用可递进式的DEBUGY,显示XML字串，成功则输出-SUCCESS,如果ERROR则在下一层次输出错误信息。
    Http_ParseFunc *_PubFunc;   //公共函数对象;
    Http_ParsePacket *_PubPacket;   //数据包解析对象;
    //int ParseDeep;   //用于跟踪层次显示。
private:
    string _GetNodeAttrStr(ParseXml_Node parse_node, const char* attr, PV_AddParas_T &add_paras);
protected:
    virtual void getHttpHeader(const char * header_name,string &header_content);
    string& ParseValueFunc(string &func_str, string &func_return, string &r_str,PV_AddParas_T &add_paras);//函数解析{@STR_LEN({对象名称})}
    
    PV_AddParas_T AddParas;//为提速，对于空增补参数的预备。
    
    //int DebugType;
    
    //url 初始化。
    string ip_;
    int port_;
    string host_;
    
};

const unsigned char Http_ParseProcess_CRC_TABLE[256]= {
    0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
    35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98 ,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
    70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7 ,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
    17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
    50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
    87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53,
};


typedef string (*ParseFunc_T)(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras); //(用于存放para字符串和指针信息。)
typedef map<string, ParseFunc_T> ParseFuncMap_T;

class ParseProcess::Http_ParseFunc:ParseBase::FuncBase{
public:
    enum{
        FIELD_PARAS_SUCCESS=0,
        FIELD_PARAS_ERROR=-1,
        DIR_INCLUDE_DIR=1,
        DIR_INCLUDE_FILE=2,
        DIR_ADD_PATH=4,
        DIR_DEEP=8,
        FILE_DIR_JSON_OBJ=0,
        FILE_DIR_JSON_ARR=1,
        FILE_DIR_JSON_BB_OBJ=2,
        FILE_DIR_MAX_NUM=10000,
        PARAS_BITMAP_0X_2_BIN=0
    };
    
    //公共函数
    Http_ParseFunc();
    ~Http_ParseFunc();
    virtual string Do(const char *func_name,string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string STR_LEN(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string ARRAY_LEN(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string GET(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string PUT(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string BARRAY_LEN(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string BARRAY_UPDATE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string BARRAY_INSERT(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string BARRAY_FIND(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string BARRAY_EMPTY(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string BARRAY_DEL(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string OBJ_VALID(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string CONN_VALID(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string OBJ_SON_NUM(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
   static string OBJ_SON_CLEAR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    //static string NUM_LEN(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string XML_TO_STR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string UTF8_TO_GB2312(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string GB2312_TO_UTF8(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string SAVE_FILE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string FILE_SAVE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string FILE_EXITS(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string FILE_DEL(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string FILE_RENAME(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string FILE_MKDIR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string FILE_READ(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
	//static string READ_XML_VALUE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string FILE_READ_INFO(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string FILE_DIR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
	static string FILE_DIR_EXT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string FILE_DIR_ALL(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string _FILE_DIR_ALL_TMP(string &func_return, string &search_di,BB_SIZE_T num,string &filter,int r_type,vector<string> & r_model);
    static string CHKSUM(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string FILE_READ_EXT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string FILE_SAVE_EXT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string PACKET_PARSE_XML(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string XML_PARSE_PACKET(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string GET_PARSE_TYPE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string BINARY_TO_HBC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string SUB_STR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string SET_NUM_TO_ID(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string CUT_STR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string POINT_TO_STR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string PARSE_LEN(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string PARSE_ENCODE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string PARSE_FUNC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string EVAL(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string SYS_GET_MAC(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
#if !defined(__ANDROID__) && !defined(TARGET_OS_IPHONE) && !defined(TARGET_IPHONE_SIMULATOR)
    static string SYS_CMD(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string SYS_EXIT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
	static string EXEC_CMD(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string POPEN(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string POPEN_READ(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string POPEN_CLOSE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
#endif
#if !defined(__ANDROID__) && !defined(__APPLE__)
    static string WEBSOCKET_ACCEPT_KEY(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string WEBSOCKET_FRAME_TYPE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string WEBSOCKET_FRAME_LEN(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string WEBSOCKET_FRAME_PAYLOAD(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string WEBSOCKET_ENCODE_FRAME(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
#endif
    static string ROUTING_CREAT_PACKET(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string CONN_OBJ_CREATE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string CONN_OBJ_STATE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string DO_QUEUE_TABLE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string DO_QUEUE_SEND(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string DO_QUEUE_RACT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string DO_QUEUE_SIZE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string CHK_QUEUE_TIMEOUT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string DO_QUEUE_ERR(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    //void _WEBSOCKET_ACCEPT_KEY(string &key,string &out_s);
    static string SUB_STR_NUM(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string STR_REPLACE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string STR_BIN_TO_CHAR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string STR_SPLIT_ARRAY(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string SUB_STR_SPLIT(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string STR_SPLIT_FROM_TO(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string MASK(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string EQUALS(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string PLUS(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string MINUS(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string DIVI(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string MOD(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string NUMBER_TO_FLOAT(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string MULT(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string ERR_CODE_STR(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string ERR_GET_LAST_ERROR(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string CHECK_ACTION_QUERY(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string COUNTER(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string HEX2IP(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string IP2HEX(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string POW(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string ROUND_TIME_STR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string GET_NOW_DATE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string GET_NOW_TIME(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string HBC_TO_HBC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    //static string CHECK_CRC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string HBC_TO_CRC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    /*
     static string BITDB_LEN_CRC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     static string BITDB_BUF_WRITE_TO(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     static string BITDB_BUF_READ_FROM(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     */
    static string AVTDB_BUF_READ_FROM(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string AVTDB_BUF_WRITE_TO(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string AVTDB_BUF_WRITE_TO_JENNIC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string AVTDB_FUNC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static bool _AVTDB_FUNC_BB_ARRAY_TO_OBJ(ParseProcess *parent, string src_array, string dest_obj, const char* hex_para);
    static string SESSION_FUNC(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string THIRD_RCV_MSG(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string THIRD_SEND_MSG(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string THIRD_FUNC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string CALL_FUNC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string GB_PARAM_VALUE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string GET_CONN_PARAM(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    
    static string CNT_CONN_PARAM(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string GB_PARAM_SET(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string CHECK_CONN(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string IS_NUM(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string IS_TEXT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string TO_TEXT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static int _TO_TEXT_UTF8_LEN(unsigned char chr);
    static string HTTP_UNESCAPE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string HTTP_ESCAPE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string HTTP_BASE64_ENCODE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string HTTP_BASE64_DECODE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string HTTP_GB2312ToUTF8(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string HTTP_UTF8ToGB2312(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
	static string IS_UTF8(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras); //ly shop
    
#ifdef JAWS_CDBM
    static string GET_XML_DB_DOC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
#endif
#ifdef SENSOR_DBM
    static string GET_SENSOR_DB_DOC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
#endif
    static string IIF(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string GET_HTTP_HEADER(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string DEBUGY(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string SET_OBJ_VALUE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string GET_OBJ_VALUE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string GET_NODE_PATH(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string TEST_CONDITION(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string FIND(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string SEEK(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    static string IS_TIME_OUT(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
    
    ParseFuncMap_T FuncMap;
    static int FieldParseCheckQuot(string &func_field, vector<string>  &paras, int paras_num,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras,UInt16 bitmap=255);

private:
    //内部参数
    static int FieldParse(string &func_field, vector<string>  &paras);
    static bool CheckParaQuot(string &paras,string &temp_s);
    };

//typedef bool (*ParsePacketSetFunc_T)(string &field_name, string &field_value);//解析后的程序的回写函数
class ParseProcess::Http_ParsePacket{
public:
    //内部中转参数对象。
    typedef struct {
        string name;
        string test;
        string len;
        string start;
        string end;
        string bit;
        string type;
        string from_end;
        string radix;
        string value;
        string result_eval;
        string src_eval;
        string remark;
        string unit;
    }ParseObjT;
    
    typedef struct {
        int end;
        int start;
        string value;
        int len;
    }DestObjT;
    
    enum{
        NaN=2294967295UL,
        PACKET_PARAS_SUCCESS=0,
        PACKET_PARAS_ERROR=-1
    };
    ParseXml_Node PARSE_PACKET_NODE_NULL;
    string OriginStr;//!!今后应该为参数传递模式。
    
    //公共函数
    Http_ParsePacket(Http_ParseXML *parse_xml,ParseProcess *parentProcess);
    ~Http_ParsePacket();
    
    //取得ParsePacket解析节点
    const char *GetParseType(const char *xpath, ParseXml_Node from_node);//通过条件xml查询相关的解析节点，查询不到返回NULLs。
    ParseXml_Node GetParseNode(const char *parse_path, const char *parse_type);//通过parse_node路径取得具体的配置节点,parse_type补充二级分支
    ParseXml_Node _GetParseNode(ParseXml_Node parse_node, const char *parse_type);//通过parse_node路径取得具体的配置节点,parse_type补充二级分支
    
    
    //解析主函数
    //正向解析：从字符流节点中取得系列字段值对象。
    //parse_node:解析配置xml节点或者路径；origin_str:原始字符串；dest_node生成目标xml的根节点；其它为内部中转变量。
    bool GetFieldsFormOriginStr(const char *parse_path, string &origin_str, ParseXml_Node dest_node,string this_path);
    bool GetFieldsFormOriginStr(ParseXml_Node parse_node, string &origin_str, ParseXml_Node dest_node,string this_path);
    bool _GetFieldsFormOriginStr(ParseXml_Node ps, string &origin_str, ParseXml_Node dest_node);
    bool _GetFieldsFormOriginStr_XML_Set(ParseXml_Node dest_node,DestObjT &d,ParseObjT &p);
    
    //流对象解析法
    bool GetFieldsFormOriginStr(const char *parse_path, string &origin_str, Http_ParseObj *dest_node, string &obj_name);
    bool _GetFieldsFormOriginStr(ParseXml_Node ps, string &origin_str, Http_ParseObj *dest_node, string &obj_name);
    
    //反向解析：从字符流节点中取得系列字段值对象。
    //parse_node:解析配置xml节点或者路径；from_node,xml字段节点的根节点；其它为内部中转变量。
    
    string GetOriginStrFromFields(const char *parse_path, ParseXml_Node from_node,string this_path,const char *radix);
    string _GetOriginStrFromFields(ParseXml_Node dest_node, ParseXml_Node from_node,const char *radix);
    
    //函数：对单一字段节点的长度解析，取得stat,end,len;
    //参数：p_*为解析节点设置，d_*为上一解析结果（为递进查询使用；
    bool GetFieldBitPosition(ParseObjT &p_obj, DestObjT &d,int origin_len);
    
    //函数：取得字段解析节点相关参数。 
    void _GetParseFieldObj(ParseXml_Node field_node, ParseObjT & p_obj); //取得字段解析节点的具体参数。
    
    //函数：字符串进制转换。把temp从src_hbc转成dest_hbc进制数;并判断是否增加add_head（0x,0b）
    string HBCStrToHBCStr(const char *temp, const char *src_hbc, const char *dest_hbc, bool add_head);
    string HBCStrToHBCStr(string &temp, string &src_hbc, const char *dest_hbc, bool add_head);
    string HBCStrToHBCStr(string &temp, const char* src_hbc, string &dest_hbc, bool add_head);
    string HBCStrToHBCStr(string &temp, const char* src_hbc, const char *dest_hbc, bool add_head);
    string HBCStrToHBCStr(string &temp, string &src_hbc, string &dest_hbc, bool add_head);
    string HBCStrToCRCStr(string &src_hbc, int start_hex, int end_hex);
    string BinaryStrToCRCStr(string &src_hbc, int start_hex, int end_hex);
    string BinaryToHBCStr(const char* data,size_t data_len, const char *dest_hbc, bool add_head);
    string BinaryToHBCStr(const char* data,size_t data_len, string &dest_hbc, bool add_head);
    
    //string HBCStrToBinary(string &temp, string &src_hbc, char * dest, int &len);
    //函数：补齐缺失长度的字段为split所指定值
    string SetNumToID(string s,int len, const char*split);
    
    //函数：判断数字字符串合法性，Radix：进制，len：要求长度；
    bool CheckNum(string &num_str);
    bool CheckNum(string &num_str, const char *Radix);
    bool CheckNum(string &num_str, const char *Radix, int len);
    
    //函数：交叉替换字串的处理。
    string ReplaceBit(string &origin_str,int d_start, string &from_bit);
    
    //各类js同步过滤函数。
    bool _TypeOf(ParseXml_Node &obj,const char *type);
    bool _TypeOf(string &obj,const char *type);
    bool _TypeOf(const char *obj,const char *type);
    bool _IsNull(ParseXml_Node &obj,const char *type);
    bool _IsNull(string &obj);
    bool _IsNull(const char *obj);
    bool _IsNull(int len);
    
    ParseXml_Node _GetNode(ParseXml_Node n, const char *xpath);
    ParseXml_Node _GetNode(ParseXml_Node n, string xpath);
    ParseXml_Node _AddNode(ParseXml_Node n, string node_name, bool is_clear);
    bool _SetNodeValue(ParseXml_Node n, string &value);
    bool _SetNodeAttr(ParseXml_Node n, const char *att_name, int value);
    bool _SetNodeAttr(ParseXml_Node n, const char *att_name,  const char *value);
    const char*_NodeName(ParseXml_Node this_node);
    const char*_NodeValue(ParseXml_Node this_node);
    const char*_NodeAttr(ParseXml_Node this_node,const char*attr_name);
    int _StrLen(string &s);
    //int _ParseInt(const char *s);
    //int _ParseInt(string s);
    //int _ParseInt(string s, int radix);
    string _iToStr(int num,int radix);
    int _StrToi(string str);
    double _StrTof(string str);
    //template <typename T> string ToStr(T val);
    string _dToStr(double num);
    string _iToChar(int num);
    string _SubStr(string &s,int pos,int len);
    char _CharAt(string &s,int pos);
    string _StrAdd(const char*s,string &add_str,bool from);
    string _StrAdd(string &s,const char *add_str,bool from);
    string _StrAdd(string &s,string &add_str);
    string _StrAdd(string &s,string &add_str,bool from);
    string _ParseValue(const char*str,string now_path);
    string _ParseValue(string &str,string now_path);
    int PUB_FindStr(const char *str,const char*s);
    int PUB_FindStr(string &str,string s);
    int PUB_FindStr(string &str,const char*s);
    bool PUB_FindStrIs(string &str,const char*s);
    bool PUB_FindStrIs(string &str,string s);
    void PUB_Error(string str);
    void SetDebug();
    void CloseDebug();
    void DEBUGY(string str);
    void DEBUGY(const char *str);
    void DEBUGY(ParseXml_Node n);
    
    DestObjT destObj;
    ParseObjT parseObj;
    ParseProcess *parentProcess;
    
    Http_ParseXML *objXmlDoc;//动作配置对象。
    ParseXml_Node objXml;
    Http_ParseXML  *destXmlDoc;//动作配置对象。
    ParseXml_Node destXml;
    string nowParsePath;//临时用于记录xpath的地址。ParseValue函数可以随时取得当前节点
    
    string rootNode;
    string ErrorStr;
    
private:
    string S1;
    bool doDebug;
};

#endif
