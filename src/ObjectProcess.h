//------------------------------------------------------
//功能：HTTP_Process声明
//作者syh
//时间06-5-10
//------------------------------------------------------
#ifndef ObjectProcess_H
#define ObjectProcess_H

#include "ParseProcess.h"

#include <vector>
#include <stdlib.h>
#include <string>
#include <string.h>
//#include "pugixml.hpp"
//#include "XML_Mini.h"
#include "ParseXml.h"
#include "ParseObj.h"
#include "Linux_Win.h"
#include "ParseBase.h"
//#include "FileQueue.h"
using namespace pugi;
using namespace std;

//new jaws add;
#define ObjectProcess_XML_USER_ADD_PARAS "XMLParse/Configs/UserParas/Base/*"
extern int PUB_ObjectProcessNum;
class ObjectProcess : public ParseProcess
{
public:
   //ly_send_queue
    SendNodeQueue *sendQueue;
    SendNodeQueueCell_T doCellObj;
#define Default_SendQueue_TimeVal 60
#define Default_SendQueue_ConnVal 55
#define sendQueue_TimeOutErrPath "_TIMEOUT_ERR"

    string& ParseValueFunc(string &func_str, string &func_return, string &r_str,PV_AddParas_T &add_paras);//函数解析{@STR_LEN({对象名称})}
    ObjectProcess(void);
    virtual ~ObjectProcess(void);

    class ObjFunc; //ly_obj_func

    //int ProcessMsg(string &msg, const char *msg_type,const char *src,const char *dest,OBJ_PROCESS_SendCBFunc_T send_cb_func,void *send_cb_handle);
    int InitProcess(const char *src,const char *dest);
    int DoProcess(const char *act_type,OBJ_PROCESS_SendCBFunc_T send_cb_func,void *send_cb_handle);
    int GoAction(const char *action_path);
    int SEND_MSG(ParseXml_Node parse_node,PV_AddParas_T &add_paras);
    string DoQueueSendDest();
    string DoQueueRcvAction(SendNodeQueueCell_T *cell,string &pkg,string &state_value, ParseProcess::PV_AddParas_T &add_paras);

    string Parse(string src);
    ParseXml_NodeSet GetXmlNodes(const char *jpath);
    ParseXml_Node GetXmlNode(const char *jpath);
    //ParseXml_Node GetConnSingleNode(const char *jpath);
    const char *GetXmlNodeAttr(ParseXml_Node node,const char *attr);
    const char *GetXmlNodeName(ParseXml_Node node);
    bool LoadObjectFile(const char *parse_cfg);
    bool LoadObject(Http_ParseXML *xml_obj);
    void InitObjectParam(void);
    void UserAddParas(PV_AddParas_T &add_paras,const char *add_paras_path);
    void UserDelParas(PV_AddParas_T &add_paras,const char *add_paras_path);
    int SetParam(const char *jpath, const char *value,int set_type);
    int SetParam(const char *jpath, int value);
    int SetParam(const char *jpath, const char *data,size_t len,int set_type);
    int SetParam(const char *jpath, string  &src_str,int set_type=Http_ParseObj::STR_COPY_OBJ);
    int SetParam_ParsePacket(const char *dest_jpath,string &input_str,const char *parse_jpath);
    const char *GetObjFileNodeAttr(const char *jpath,const char *attr_name);
    //const char *GetObjFileNodeValue(const char *jpath);
    const char *GetParamValue(const char *jpath);
    bool GetParamValueStr(const char *jpath,string & dest_str);
    int ParseAction(ParseXml_Node parse_node,PV_AddParas_T &add_paras);
    bool IsReady();

	void  ReuseClear(int level=1);  ////for reuse ParseObj; sometime's we should clear some attr;
    //string ErrorStr;

    ObjFunc *_PubFunc; //ly_obj_func

private:
    string ObjectCfg;
    bool isObjectLoad;   //_ActXml 为外部传入指针，不需要析构
    
};

//ly_obj_func
typedef string (*ObjectFunc_T)(string func_field,string &func_return,ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras); //(用于存放para字符串和指针信息。)
typedef map<string, ObjectFunc_T> ObjProcFuncMap_T;

 class ObjectProcess::ObjFunc : public ParseProcess::Http_ParseFunc
 {
 public:
     ObjFunc();
     ~ObjFunc();
     string Do(const char *func_name,string func_field,string &func_return,ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     static string CONN_OBJ_CREATE(string func_field,string &func_return,ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     static string CONN_OBJ_CANCLE(string func_field,string &func_return,ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     static string DO_QUEUE_SEND(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     static string DO_QUEUE_RACT(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     static string DO_QUEUE_SIZE(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     static string CHK_QUEUE_TIMEOUT(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     static string START_TIME_TASK(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     static string CANCLE_TIME_TASK(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     static string TASK_CYCLE_RUN(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     static string TASK_CYCLE_KEY(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     static string CHECK_OBJS(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras);
     //static string DO_QUEUE_ERR(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras);

 private:
     ObjProcFuncMap_T FuncMapAdd;
 };

#endif
