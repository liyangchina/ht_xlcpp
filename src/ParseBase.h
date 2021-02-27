#ifndef PARSE_BASE_H
#define PARSE_BASE_H
#pragma once
#include <map>
#include <vector>
#include <string>
#include "pugixml.hpp"
//#include "XML_Mini.h"
#include "ParseXml.h"
#include "ParseObj.h"
#include "Linux_Win.h"
using namespace pugi;
using namespace std;

class ParseBase
{
public:
    typedef map<string, string> PV_AddParas_T;//对于ParseValue的增补参数（@THIS,@PARENT类）。
    
    ParseBase(void){
        //this->_PubObj = new Http_ParseObj();	//创建对象
        //this->_ActParseXml = new Http_ParseXML();
    };
    virtual ~ParseBase(void);
    
    class FuncBase;
    
    //~ParseBase(void);
    enum{
        ACT_SUCCESS=0,
        ACT_ERROR=-999,
        ACT_BREAK =-998,
        SWITCH_CASE_BREAK=0,
        SWITCH_CASE_NEXT=-1,
        SWITCH_CASE_TEST_TRUE=0,
        SWITCH_CASE_TEST_FALSE=-1,
        SWITCH_CASE_TEST_CONDITION_TRUE=0,
        SWITCH_CASE_TEST_CONDITION_FALSE=-1,
        DEBUG_NO=0,
        DEBUG_NORMAL=1,
        DEBUG_DEEP=2,
    };
    
    Http_ParseXML *_ActParseXml;//动作配置对象。
    
    Http_ParseObj *_PubObj;     //公共对象;
    int TestCondition(string &condition, string &left, string &right);
    bool TestConditionStr(string &act_test,string &err_str);
    bool TestConditionStr(string &act_test,string &err_str,PV_AddParas_T &add_paras);
    bool TestConditionStr(const char *act_test,PV_AddParas_T &add_paras);
    int GetConditionPart(string &src,string &condition,string &left,string &right);
    
    //功能：从属于_PARSE_NODE对节点中的内容进行解析，简单就是{****}对象替换为相应的对象值
    //动作：（1）CASE:普通的{****}替换操作。先
    //		（2）CASE:{@STR_LEN([_*****_])}数据长度函数。
    //		（3）CASE:其它函数。
    string ParseValue(ParseXml_Node parse_node,string &dest_buff,PV_AddParas_T &add_paras);
    string ParseValue(string src,string &dest,PV_AddParas_T &add_paras);
    string CallParseValue(string src);
    
    //功能：针对[[***],[***]]数组的处理
    string ParseVector(const char *src,string &dest,PV_AddParas_T &add_paras);
    int _ParseVector_GetNext(const char *src, string &dest_str, char * &next_point, string para);
    //int _ParseVector_GetNextPos(const char *src, char* &next, size_t &begin, size_t &end, string para);
    char *_ParseVector_TempStrFind(const char *src, const char *find);
    //char *_ParseVector_StrStr(const char *src, const char *find, size_t len, bool skip_block);//跳过空白发现字串,控制长度的strstr
    virtual string& ParseValueFunc(string &func_str, string &func_return, string &r_str,PV_AddParas_T &add_paras);//函数解析{@STR_LEN({对象名称})}
    virtual int FUNCTION(ParseXml_Node parse_node, PV_AddParas_T &add_paras)=0;

    void SHOW_ERROR(string act_stat, bool set_to_last_error = false);
    void SHOW_ERROR(const char *act_stat, bool set_to_last_error=false);
    string GET_LAST_ERROR();
    bool GetLastError(string &err_str);
    void CLEAR_LAST_ERROR();
    void SET_LAST_ERROR_STR(string str);
    void DEBUGY(const char *act_stat);
    void DEBUGY(string act_stat);
    void DEBUGY(ParseXml_Node parse_node);
    void DEBUGY(ParseXml_Node parse_node,const char *act_stat);
    void DEBUGY(const char *node_value,string act_stat);
    void DEBUGY(ParseXml_Node parse_node,string act_stat);
    void DEBUGY();
    
    //int TransmitRequest(ACE_SOCK_Stream &xserver_peer,const char* ip, const char * port, void * request, int requestLen,string &result,int &result_len,int trans_type, bool keep_cnt);
    int savefiles(const char * path_file,const char * file_data, size_t file_len, bool is_append=false,bool is_binary=false);
    int savefiles(const char * path_file,string &file_data,bool is_append=false);
    int CreatDirExt(const char *,string);
    
    int ParseDeep;   //用于跟踪层次显示。

    //FuncBase *_PubFunc;   //公共函数对象;
    string ErrorStr;
    unsigned char DebugType;
#define JSON_CHG_MARK_BEGNG "!-__-!"
#define JSON_CHG_MARK_END "!_--_!"
protected:

};

class ParseBase::FuncBase{
public:

private:
};

#endif
