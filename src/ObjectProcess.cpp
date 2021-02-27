#include "stdafx.h"
#include "ParseProcess.h"
//#include "HTTP_Config.h"
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>
#include "pugixml.hpp"
#include <math.h>
#include "ObjectProcess.h"
#ifdef SENSOR_DBM
#include "CDBManager.h"
#endif
#include "ParseXml.h"
#include "ParseObj.h"
#include "STRING_Helper.h" 
#include "Linux_Win.h"
#include "ParseExternal.h"
#include "ConnQueue.h"
#include "WizBase.h"
#include "FileQueue.h"

using namespace std;
using namespace pugi;

extern bool BitDB_LoadDB(byte tableindex);
int PUB_ObjectProcessNum=0;

string& ObjectProcess::ParseValueFunc(string &func_str, string &func_return, string &r_str,PV_AddParas_T &add_paras){
    string func_name,func_field,r;


    //取得函数 Name ；
    if(STRING_Helper::strStringCut(func_str,func_name,"@","(")!=STRING_Helper::FINDED){
        r_str=func_str+"function error: "+"unknowable{@**** syntax";
        return r_str;
    };

    //取得函数Param；
    if (STRING_Helper::strStringCut(func_str,func_field,func_name+"(",")")!=STRING_Helper::FINDED){
        r_str=func_str+"function error: "+"unconfined{@****( }syntax.";
        return r_str;
    };

    r=this->_PubFunc->Do(func_name.c_str(),func_field,func_return,this,add_paras);
    if (r.size()==0)
        r_str="";
    else
        r_str = func_str+"function error: "+r;//通过内置的函数类调用预定义函数.

    return r_str;
}

ObjectProcess::ObjectProcess(void){
    
    //Init @Function
    //this->_PubFunc = new Http_ParseFunc();
    this->_PubFunc = new ObjFunc(); //ly_func
    //Init Param
    this->_PubObj = new Http_ParseObj();	//创建对象
    //Init ConfigXMl;
    //this->_ActParseXml = new Http_ParseXML();
    this->_ActParseXml = NULL;  //ly obj_new
    //初始化解析对象,defined in LoadObjFile()
    //this->_PubPacket = new Http_ParsePacket(this->_ActParseXml,this); //改为在LoadObjFile()
    
    this->ObjectCfg="";
    this->ParseDeep=0;
    this->isObjectLoad = false; //ly obj_new

    //ly_send_queue
    this->sendQueue=new SendNodeQueue();
    this->doCellObj.send_state=SendNodeQueue::CONNECTING; //清空状态
    this->SendCBFuncHandle=NULL;
    PUB_ObjectProcessNum++; //CHECK OBJS 计数；

    //this->ProcessCount = 0;
    //add process
    //ParseProcess::ParseProcess_T ab=this->SEND_MSG;
    //this->ProcessMap.insert(pair<string, ParseProcess_T>((string )"SEND_MSG",ab));
}

ObjectProcess::~ObjectProcess(void){
    //释放对象
    if (this->_PubObj!=0)
        delete this->_PubObj;			//释放对象

    if (this->_ActParseXml!=0 && !this->isObjectLoad)
        delete this->_ActParseXml;			//释放对象ly obj_new
    if (this->_PubFunc!=0)
        delete this->_PubFunc;
    if (this->_PubPacket!=0 || (size_t)this->_PubPacket==0xffffffff)
        delete this->_PubPacket;

    //ly_send_queue
    if (this->sendQueue!=NULL){
        delete this->sendQueue; //ly_send_queue
    }
    PUB_ObjectProcessNum--;
}
/*
int ObjectProcess::ProcessMsg(string &msg, const char *msg_type,const char *src,const char *dest,OBJ_PROCESS_SendCBFunc_T send_cb_func,void *send_cb_handle)
{
    //initial head parameters;
    this->InitObjectParam();
    string date,time;
    STRING_Helper::GetNowDate(date,"-");
    STRING_Helper::GetNowTime(time,":");
    this->_PubObj->SetNodeValue("_NOW_TIME_",time.c_str(),Http_ParseObj::STR_COPY_OBJ);
    this->_PubObj->SetNodeValue("_NOW_DATE_",date.c_str(),Http_ParseObj::STR_COPY_OBJ);
    this->_PubObj->SetNodeValue("_SOURCE_ID_",src,Http_ParseObj::STR_COPY_OBJ);
    this->_PubObj->SetNodeValue("_DEST_ID_",dest,Http_ParseObj::STR_COPY_OBJ);
    
    //Set Call BackFunc
    this->SendCBFunc = send_cb_func;
    this->SendCBFuncHandle = send_cb_handle;
    
    //parse packet and go action
    if (this->SetParam_ParsePacket("_REQ_PACKET_XML_",msg,msg_type)){
        string msg_type_string(msg_type);
        msg_type_string.append("/__ACTION");
        this->GoAction(msg_type_string.c_str());
    }else
        this->SHOW_ERROR(this->_PubObj->ErrorStr);
    return (1);
}*/

int ObjectProcess::InitProcess(const char *src,const char *dest)
{
    
    string date,time;
    
    STRING_Helper::GetNowDate(date,"-");
    STRING_Helper::GetNowTime(time,":");
    this->_PubObj->SetNodeValue("_NOW_TIME_",time.c_str(),Http_ParseObj::STR_COPY_OBJ);
    this->_PubObj->SetNodeValue("_NOW_DATE_",date.c_str(),Http_ParseObj::STR_COPY_OBJ);
    this->_PubObj->SetNodeValue("_SOURCE_ID_",src,Http_ParseObj::STR_COPY_OBJ);
    this->_PubObj->SetNodeValue("_DEST_ID_",dest,Http_ParseObj::STR_COPY_OBJ);
    this->UserAddParas(this->AddParas,ObjectProcess_XML_USER_ADD_PARAS);
    this->ParseDeep=0;
    return (1);
}
//int ObjectProcess::DoProcess(const char *data, int data_len,const char *act_type,OBJ_PROCESS_SendCBFunc_T send_cb_func,void *send_cb_handle)
int ObjectProcess::DoProcess(const char *act_type,OBJ_PROCESS_SendCBFunc_T send_cb_func,void *send_cb_handle)
{
    //Set Call BackFunc
	this->ReuseClear(2);  //when reuse object for recv;
    this->_PubObj->SetNodeValue("@_FD","1",Http_ParseObj::STR_COPY_OBJ);//初始化相关req值。
    this->SendCBFunc = send_cb_func;
    this->SendCBFuncHandle = send_cb_handle;
    this->ParseDeep=0;
    
    this->GoAction(act_type);
    
    return (1);
}

int ObjectProcess::SetParam_ParsePacket(const char *to_obj,string &input_str,const char *parse_path){
    
    //创建目标xml根 node ；
    string parse_xml="<Root></Root>";//临时创建一个今后还可以变化.
    Http_ParseXML * temp_xml=this->_PubObj->XmlObjCreate(to_obj,parse_xml.c_str());//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
    if (temp_xml==NULL)
        this->SHOW_ERROR(this->_PubObj->ErrorStr);
    ParseXml_Node xml_node =temp_xml->FirstChild(temp_xml->ObjXml);
    
    //在根 node 写入相关的字段
    if (!this->_PubPacket->GetFieldsFormOriginStr(parse_path, input_str, xml_node,(string)to_obj+":"+"/Root")){
        // Object Param
        this->SHOW_ERROR(this->_PubObj->ErrorStr);
    };
    
    return (1);
}

bool ObjectProcess::LoadObjectFile(const char *parse_cfg){
    if (this->_ActParseXml==NULL)
        this->_ActParseXml = new Http_ParseXML();
    int r=this->_ActParseXml->LoadFile(parse_cfg);
    this->ObjectCfg = parse_cfg;
    
    if (r!=Http_ParseXML::LOAD_XML_SUCCESS){
        this->ErrorStr = this->_ActParseXml->ErrorStr;
        return false;
    }else{
        //初始化解析对象
        this->_PubPacket = new Http_ParsePacket(this->_ActParseXml,this); //ly ???
        return true;
    }
}

bool ObjectProcess::LoadObject(Http_ParseXML *xml_obj) {
    //ly obj_new
    if (!xml_obj->LoadSuccess()) {
        this->ErrorStr = this->_ActParseXml->ErrorStr;
        return false;
    }
    else {
        //初始化解析对象
        this->_ActParseXml = xml_obj;
        this->isObjectLoad = true;
        this->ObjectCfg = this->_ActParseXml->FileName;
        this->_PubPacket = new Http_ParsePacket(this->_ActParseXml, this); //ly ???
        return true;
    }
}

const char *ObjectProcess::GetObjFileNodeAttr(const char *jpath,const char *attr_name){
    if(ParseXml_Node node = this->_ActParseXml->GetNodeByXpath(jpath)){
        //for the object xml
        if (node){
            return this->_ActParseXml->NodeAttr(node,attr_name);
        }
    }else{
        cout <<"ObjectProcess::GetCfgNodeAttr()"<< this->_ActParseXml->ErrorStr << endl;
    }
    return 0;
}
/*
const char *ObjectProcess::GetObjFileNodeValue(const char *jpath){
    if(ParseXml_Node node = this->_ActParseXml->GetNodeByXpath(jpath)){
        //for the object xml
        if (node){
            return this->_ActParseXml->NodeValue(node);
        }
    }else{
        cout <<"ObjectProcess::GetObjFileNodeValue()"<< this->_ActParseXml->ErrorStr << endl;
    }
    return 0;
}*/

void ObjectProcess::InitObjectParam(void){
    if (this->_PubObj!=0)
        delete this->_PubObj;			//释放对象
    this->_PubObj = new Http_ParseObj();
    
    this->_PubObj->SetNodeValue("_THIS_CFG_XML_",(const char *)this->_ActParseXml->XmlToStr(this->_ActParseXml->ObjXml).c_str(),Http_ParseObj::XML_POINT_OBJ);//ly !!!今后改进，目前XML_POINT_OBJ 并未发成作用!!! 测试_THIS_XML_CFG_XML对象
}

void ObjectProcess::UserAddParas(PV_AddParas_T &add_paras,const char *user_add_paras_path){
    //实际操作
    ParseXml_NodeSet nodes=this->_ActParseXml->GetNodeSetByXpath(user_add_paras_path);

    for (ParseXml_NodeSet::const_iterator it = nodes.begin(); it != nodes.end();++it){
        ParseXml_Node actions_node = it->node();
        const char *obj_name = this->_ActParseXml->NodeName(actions_node);
        const char *obj_value = this->_ActParseXml->NodeValue(actions_node);
        string obj_name_str("@_");
        obj_name_str+=obj_name;
        this->_PubObj->SetNodeValue(obj_name_str.c_str(),obj_value,Http_ParseObj::STR_COPY_OBJ);
        //add_paras.insert(ParseProcess::PV_AddParas_T::value_type(obj_name_str,string(obj_value)));
    }
    return;
}

void ObjectProcess::UserDelParas(PV_AddParas_T &add_paras,const char *user_add_paras_path){
    //实际操作
    ParseXml_NodeSet nodes=this->_ActParseXml->GetNodeSetByXpath(user_add_paras_path);

    for (ParseXml_NodeSet::const_iterator it = nodes.begin(); it != nodes.end();++it){
        ParseXml_Node actions_node = it->node();
        const char *obj_name = this->_ActParseXml->NodeName(actions_node);
        string obj_name_str("@_");
        obj_name_str+=obj_name;
        this->_PubObj->DeleteObjNode(obj_name_str);
        //add_paras.erase(obj_name_str.c_str());
    }
    return;
}


int ObjectProcess::SetParam(const char *jpath, const char *value,int set_type){
    return this->_PubObj->SetNodeValue(jpath,value,set_type);
}


int ObjectProcess::SetParam(const char *jpath, const char *data,size_t len,int set_type){
    return this->_PubObj->SetNodeValue(jpath,data,set_type,len);
}

int ObjectProcess::SetParam(const char *jpath, string &src,int set_type){
    return this->_PubObj->SetNodeValue(jpath,src.data(),set_type,src.size());
}

int ObjectProcess::SetParam(const char *jpath, int value){
    return this->_PubObj->SetNodeValue(jpath,value);
}

const char *ObjectProcess::GetParamValue(const char *jpath){
    return this->_PubObj->GetNodeValue(jpath);
    //return 0;
}

bool ObjectProcess::GetParamValueStr(const char *jpath,string & dest_str){
    return this->_PubObj->GetNodeValueStr(jpath,dest_str);
};

//功能：主处理程序，
//动作：(1)初始化_PubObj_对象。
//		(2)根据uri:./PARSE/ACTION1打开，Load ActionParseXml(./PARSE/ACTION1.XML)动作配置文件。注：今后需要把改文件放入全局变量，或者通过链表的形式打开一次不再打开。
//		(3)接收请求动作吧相应的req.uri,req.host,req等内容的指针写入_REQ_URI,_REQ_DATE_,REQ_HOST等对象
//		(4)定位ActParseXml中第一个ACTIONS节点。
//		(5)调用ParseAction()解析第一个ACTIONS节点
//参数：request:输入对象；response:输出对象；
int ObjectProcess::GoAction(const char *start_path){
    //this->DebugType = ParseProcess::DEBUG_DEEP;
    this->DebugType = ParseProcess::DEBUG_NORMAL; //ly thread
    
    const char *track_p,*user_add_paras;
    bool is_track = false,is_user_add_paras=false;
    
    if (this->_PubPacket!=0){
        //解析运行ActionParse();
        if (start_path==0){
            //for the all action xml
            ParseXml_Node node= this->_ActParseXml->FirstChild(this->_ActParseXml->ObjXml);
            //for the object xml
            this->CfgNode=node;
            if (node){
                this->ParseDeep = 0;
                this->DEBUGY(node);
                //开启跟踪
                track_p = this->_ActParseXml->NodeAttr(node, "Track");
                if (track_p != NULL && strlen(track_p) != 0) {
                    this->TimeTrack(track_p, TRACK_START);
                    is_track = true;
                }
                //增补user_add_paras
                user_add_paras = this->_ActParseXml->NodeAttr(node,"UserParas");
                if (user_add_paras != NULL && strlen(user_add_paras) != 0) {
                    this->UserAddParas(this->AddParas,user_add_paras);
                    is_user_add_paras=true;
                }
                //操作
                this->ParseAction(node,this->AddParas);
                //删除user_add_paras
                if (is_user_add_paras){
                    this->UserDelParas(this->AddParas,user_add_paras);
                }
                //关闭跟踪
                if (is_track)
                    this->TimeTrack(track_p, TRACK_END);
            }
        }else if(ParseXml_Node node = this->_ActParseXml->GetNodeByXpath(start_path)){
            //for the object xml
            if (node){
                ParseXml_Node node_parent = this->_ActParseXml->Parent(node);
                this->CfgNode=node;
                const char *debug_set;
                //测试父节点是否Debug
                if (node_parent) {
                    debug_set = this->_ActParseXml->NodeAttr(node_parent, "Debug");
                    if (debug_set!=NULL && strstr(debug_set, "No") >= debug_set)
                        this->DebugType = ParseProcess::DEBUG_NO;
                    //测试本节点是否Debug
                    debug_set = this->_ActParseXml->NodeAttr(node, "Debug");
                    if (debug_set!=NULL && strstr(debug_set, "Yes") >= debug_set)
                        this->DebugType = ParseProcess::DEBUG_NORMAL;
                }
                
                //动作
                this->ParseDeep = 0;
                this->DEBUGY(node);
                //开启跟踪
                track_p = this->_ActParseXml->NodeAttr(node, "Track");
                if (track_p != NULL && strlen(track_p) != 0) {
                    this->TimeTrack(track_p, TRACK_START);
                    is_track = true;
                }
                //增补user_add_paras
                user_add_paras = this->_ActParseXml->NodeAttr(node, "UserParas");
                if (user_add_paras != NULL && strlen(user_add_paras) != 0) {
                    this->UserAddParas(this->AddParas,user_add_paras);
                    is_user_add_paras=true;
                }
                //操作
                this->ParseAction(node,this->AddParas);

                //删除user_add_paras
                if (is_user_add_paras){
                    this->UserDelParas(this->AddParas,user_add_paras);
                }
                //关闭跟踪
                if (is_track)
                    this->TimeTrack(track_p, TRACK_END);
                
                this->DebugType = ParseProcess::DEBUG_NORMAL;
            }
        }else
            this->SHOW_ERROR(this->_ActParseXml->ErrorStr);
    }
    
    return (1); //如果已经发送返回字串，或者由RESPONSE统一返回差错资源。
}

int ObjectProcess::ParseAction(ParseXml_Node parse_node,PV_AddParas_T &add_paras){
    this->ParseDeep++; //用于记录解析层次深度。tool = tool.next_sibling()
    
    //差错处理预制
    string err_str;
    bool err_break = false;
    bool is_err=false;
    const char *err_to_obj = this->_ActParseXml->NodeAttr(parse_node, "ErrTo");
    
    const char *err_break_str = this->_ActParseXml->NodeAttr(parse_node, "ErrBreak");
    if ((err_break != 0) && (strcmp(err_break_str, "true") != 0))
        err_break = true;
    
    bool is_track = false;
    for (ParseXml_Node tool = this->_ActParseXml->FirstChild(parse_node); tool; tool = this->_ActParseXml->NextChild(tool)) {
        //判断是否进入，Test条件判断
        const char *act_test = this->_ActParseXml->NodeAttr(tool, "Test");
        if (strlen(act_test) != 0 && !this->TestConditionStr(act_test, add_paras))
            continue;
        //过滤无用字符
        const char *tool_p=this->_ActParseXml->NodeName(tool); //???更改下面所有节点。
        //ly call_func; //ly rec_chg_mark //ly  table_sync3;
        if (strncmp(tool_p,"IN",2)==0)
            continue; //FOR->IN,今后改为_IN;
        if(strncmp(tool_p,"_",1)==0)
            continue; //过滤所有__的节点
        
        //判断是否需要跟踪
        const char *track_p = this->_ActParseXml->NodeAttr(tool, "Track");
        if (track_p != NULL && strlen(track_p) != 0) {
            this->TimeTrack(track_p, TRACK_START);
            is_track = true;
        } else
            is_track = false;
        //判断是否需要调试断点
        if (this->DebugType != ParseProcess::DEBUG_NO) {
            const char *debug = this->_ActParseXml->NodeAttr(tool, "Debug");
            if (strstr(debug, "BP") >= debug){//断点BreakPoint
                this->DEBUGY();
            }
        }
        
        if (strcmp(this->_ActParseXml->NodeName(tool),"ACTIONS")==0){
            this->DEBUGY(tool);
            this->ParseAction(tool,add_paras); //ParseAction递归调用ACTIONS的下级节点解析。
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"BREAK")==0){
            this->DEBUGY(tool);
            const char *err_to_obj1 = this->_ActParseXml->NodeAttr(tool, "ErrTo");
            if (err_to_obj1 != NULL && strlen(err_to_obj1) != 0) {
                string parse_buff;
                this->ParseValue(tool, parse_buff, add_paras);
                this->_PubObj->SetNodeValue(err_to_obj1, parse_buff.c_str(), Http_ParseObj::STR_COPY_OBJ);//设置差错提示对象
            }
            this->ParseDeep--; //用于记录解析层次深度。
            return ACT_BREAK;
            break;
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"SET_OBJ")==0){
            this->SET_OBJ(tool,add_paras);//则取得SET_OBJ的子节点，相关属性发送SET_OBJ函数；
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"PRINT")==0){
            this->PRINT(tool,add_paras);//则取得SET_OBJ的子节点，相关属性发送SET_OBJ函数；
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"DB_WRITE")==0){
            this->DB_WRITE(tool,add_paras);//则取得SET_OBJ的子节点，相关属性发送SET_OBJ函数；
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"SWITCH")==0){
              this->SWITCH(tool,add_paras);
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"ACTION")==0){
            this->ACTION(tool,add_paras);
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"FOR")==0){
            this->FOR(tool,add_paras);
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"DEL_OBJ")==0){
            this->DEL_OBJ(tool,add_paras);
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"CALL")==0){
            this->CALL(tool,add_paras);
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"SEND")==0){
            this->SEND(tool,add_paras);
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"PARSE")==0){
            this->PARSE(tool,add_paras);
#ifdef SENSOR_DBM
                }else if (strcmp(this->_ActParseXml->NodeName(tool),"DB_SEARCH")==0){
                    this->DB_SEARCH(tool,add_paras);
                }else if (strcmp(this->_ActParseXml->NodeName(tool),"DB_WRITE_BKDB")==0){
                    this->DB_WRITE_BKDB(tool,add_paras);//则取得SET_OBJ的 sub node ,相关 attribute 发送SET_OBJ函数；
#endif
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"SEND_MSG")==0){
            this->SEND_MSG(tool,add_paras);
        }
        else if (strcmp(this->_ActParseXml->NodeName(tool), "SET_QUEUE") == 0) {
            this->SET_QUEUE(tool, add_paras);
        }else if (strcmp(this->_ActParseXml->NodeName(tool), "DB_QUERY") == 0) {
            this->DB_QUERY(tool, add_paras);
        }else if (strcmp(this->_ActParseXml->NodeName(tool), "DB_QUERY_EXT") == 0) {
            this->DB_QUERY_EXT(tool, add_paras);
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"FUNCTION")==0 ||strcmp(this->_ActParseXml->NodeName(tool),"LocalEncode")==0 || strcmp(this->_ActParseXml->NodeName(tool),"ParsePacket")==0 ){
            ;//非动作节点
        }else{
            string err_str("不可识别的解析节点：<");err_str+=this->_ActParseXml->NodeName(tool);err_str+=">!";
            this->SHOW_ERROR(err_str);
        }
        //差错处理
        is_err=false;
        if (err_to_obj != NULL && strlen(err_to_obj) != 0 && (is_err=this->GetLastError(err_str))) {
            this->_PubObj->SetNodeValue(err_to_obj, err_str.c_str(), Http_ParseObj::STR_COPY_OBJ);//设置差错提示对象
        }
        if (err_break && is_err)
            break;//差错退出
        if (is_track)
            this->TimeTrack(track_p, TRACK_END);
    }
    
    this->ParseDeep--; //用于记录解析层次深度。
    return ACT_SUCCESS;
}

string ObjectProcess::Parse(string src){
    PV_AddParas_T add_paras;
    string dest;
    this->ParseValue(src,dest,add_paras);
    return dest;
}

ParseXml_NodeSet ObjectProcess::GetXmlNodes(const char *jpath){
    return this->_ActParseXml->GetNodeSetByXpath(jpath);
}

ParseXml_Node ObjectProcess::GetXmlNode(const char *jpath){
    return this->_ActParseXml->GetNodeByXpath(jpath);
}

const char *ObjectProcess::GetXmlNodeAttr(ParseXml_Node node,const char *attr){
    return this->_ActParseXml->NodeAttr(node,attr);
}

const char *ObjectProcess::GetXmlNodeName(ParseXml_Node node){
    return this->_ActParseXml->NodeName(node);
}

/*
ParseXml_Node ObjectProcess::GetConnSingleNode(const char *conn_name){
    string err_str,act_path(conn_name);
    
    string xpath=string("/XMLParse/Connect/CONNECT[@Name='")+act_path+"']";
    ParseXml_Node actions_node;
    ParseXml_NodeSet nodes = this->GetXmlNodes(xpath.c_str());
    if (nodes.size()==0)
        err_str=string("can't find definiation of <CONNECT Name='")+act_path+"'/>!";
    else if (nodes.size()>1)
        err_str=string("<CONNECT Name='")+act_path+"'/>is multiple definition!";
    else{
        //取得真实函数 node 
        ParseXml_NodeSet::const_iterator it = nodes.begin();
        pugi::xpath_node xnode = *it;
        actions_node = xnode.node();
    }
    return actions_node;
}*/

bool ObjectProcess::IsReady(){
    if (this->ObjectCfg.size()!=0)
        return true;
    else
        return false;
}

void ObjectProcess::ReuseClear(int level){//for reuse ParseObj; sometime's we should clear some attr;
	if (level<=2){
		this->CLEAR_LAST_ERROR(); //in parseBase; not clear this->ErrorStr="" directly
	}
}

int ObjectProcess::SEND_MSG(ParseXml_Node parse_node,PV_AddParas_T &add_paras)
{
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度。tool = tool.next_sibling()

    //发送操作
    //取得节点配置字段。
    string err_str="";
    bool is_queue=false,send_as_queue_null=true;

    //Return Config
    //cout << "step1"  << endl;
    ParseXml_Node content_node=this->_ActParseXml->Child(parse_node,"CONTENT");
    ParseXml_Node head_node=this->_ActParseXml->Child(parse_node,"HEAD");
    string trans_method = this->_ActParseXml->NodeAttr(parse_node,"TransMethod");
    string close_str = this->_ActParseXml->NodeAttr(parse_node,"CloseAfterSend");
    const char *rstate = this->_ActParseXml->NodeAttr(parse_node, "Rstate");
    string rstate_str = "";
    if (rstate != NULL && strlen(rstate) != 0)
        rstate_str = rstate;

    //ly_send_queue
    const char*queue_str = this->_ActParseXml->NodeAttr(parse_node, "Queue");
    if ((queue_str!=NULL) && (strstr(queue_str,"true")==queue_str)){
        is_queue=true;
    } else
        is_queue=false;
    const char*send_queue_null = this->_ActParseXml->NodeAttr(parse_node, "SendQueueNull");
    if ((send_queue_null!=NULL) && (strstr(send_queue_null,"false")==send_queue_null)){
        send_as_queue_null=false;
    } else
        send_as_queue_null=true;
    ParseXml_Node rsp_act_node=this->_ActParseXml->Child(parse_node,"_RACTIONS");
    const char *send_timeout_str = this->_ActParseXml->NodeAttr(parse_node,"RTimeOut");
    int send_timeout;
    if ((send_timeout_str!=NULL) && !STRING_Helper::CheckNum(send_timeout_str)){
        send_timeout=Default_SendQueue_TimeVal;
    } else
        send_timeout=STRING_Helper::StrToi(send_timeout_str);

    bool close_after_send=false;
    if (close_str=="true")
        close_after_send = true;

    //int trans_type=TRANS_TYPE_NORMAL;
    int trans_type=TRANS_TYPE_HT;
    bool check_length=false;
    if (trans_method!="") {//测试是否需要测试准确的数据长度等信息
        if (trans_method=="TRANS_TYPE_HT"){
            trans_type=TRANS_TYPE_HT;
            check_length=false;
        }
    }

    //ParseXml_Node error_node=this->_ActParseXml->Child(parse_node,"ERROR_RETURN");
    if (trans_method=="HTTP") {//测试是否需要测试准确的数据长度等信息
        trans_type=TRANS_TYPE_NORMAL;
        check_length=true;
    }
    string bin_content = this->_ActParseXml->NodeAttr(parse_node,"BinContent");
    string dest_obj_str = this->_ActParseXml->NodeAttr(parse_node,"Dest");
    const char *src = this->_ActParseXml->NodeAttr(parse_node,"Src");
    //this->DEBUGY("step2");
    //re computing conent and head
    string content(""),head(""),dest_obj("");
    if (content_node)
        err_str=this->ParseValue(content_node,content,add_paras);//对HEAD和内容进行解析。
    if (head_node)
        err_str=this->ParseValue(head_node,head,add_paras);//对HEAD和内容进行解析。

    if (dest_obj_str.size()!=0)
        err_str=this->ParseValue(dest_obj_str,dest_obj,add_paras);
    else
        err_str="Attribute 'Dest' must be set! ";

    //具体发送内容长度计算计算
    string bin_str="0";
    int content_len=0;
    int data_to_bin=0;
    if (bin_content.size()==0){//普通的字串数据包(以\0结尾)
        content_len=content.size();
    }else if (bin_content=="HEX_TO_BIN"){//二进制流数据包,尚需对hex的合法性校验？
        if (this->_PubPacket->CheckNum(content,"hex")){
            bin_str = this->_PubPacket->HBCStrToHBCStr(content,"hex","char",false);//可以进行hex到二进制流的转换，中间可能有\0
            content_len = bin_str.size();
            data_to_bin=1;
        }else
            err_str=(string )"CONTENT:"+content+"must be 16 hex number！";
    }else
        err_str="BinContent属性只能='hex_bin'";

    //cout << "step3"  << endl;
    //header
    if ((err_str.size()==0) && (trans_type==TRANS_TYPE_NORMAL)){
        if (!head_node )
            err_str = "TRANS_TYPE_NORMAL Is for http, than you must set Head ";
        if (head_node)
            err_str=this->ParseValue(head_node,head,add_paras);
        //内容长度头补充设置
        if(err_str.size()==0 && check_length){//测试Content Length长度是否合理
            //对于Content-Length长度不准确的补充处理。
            string cls;
            int cl;
            STRING_Helper::strStringCutField(head,cls,"Content-Length:","\r\n"); cl=atoi(cls.c_str());
            if (cl!=content_len){
                //替换为正确的长度。
                char a[10];	_itoa(content_len,a,10);
                this->DEBUGY((string)"Content-Length:"+cls+" not be equle to "+a+"!");
                STRING_Helper::strReplace(head,string("Content-Length:")+cls+"\r\n",string("Content-Length:")+a+"\r\n");//长度替换操作
            }
        }
    }

    //sending
    int r=0;
    if (err_str==""){
        char *send_data;char *send_content;int send_len;
        //开启内存空间
        send_data = (char *) malloc (sizeof(char)*(head.size()+2+content_len+4));//这个空间多加10位应该够用，哪怕后面的HBC_Binary转换也是从大转小。

        //拷贝题头字串；已解决head为空的情况，面向http和非http
        if (head!="")
            head+="\r\n";
        memcpy((void *)send_data,(const void *)head.data(),head.size());
        send_content=send_data+head.size();

        //拷贝内容数据，为二进制流；
        if (data_to_bin==0){//普通的字串数据包
            memcpy((void *)send_content,(const void *)content.data(),content_len);//拷贝内容区域
        }else if (data_to_bin==1){//二进制流数据包
            memcpy((void *)send_content,(const void *)bin_str.data(),content_len);
        }
        send_len = head.size()+content_len;//初步取得发送长度

        //初始化字段
        //cout << "step4"  << endl;
        //string tmp(send_data,send_len);
        //cout << tmp << endl;
        //发送操作
        if (is_queue==false){ //如果队列为空也发送
            if ((r=this->SendCBFunc((const char*)send_data, send_len, dest_obj.c_str(), src,
                                    trans_type, close_after_send, this->SendCBFuncHandle))!=0)
                err_str=(string)"Sending Error: r="+STRING_Helper::iToStr(r);
        }else{
            //ly_send_queue
            SendNodeQueueCell_T cell_obj;
            //判断是否有该链接
            Conn *dest_conn;
            if ((dest_conn = GB_ConnQueue->FindConnByObjName(dest_obj.c_str(), 0,true)) == NULL){
                err_str=(string)"SendMsgAddQueue()->Send Error: DestObj: "+dest_obj+" not exited!";
                r=-1;
            }else{
                //目标对象添加队列
                cell_obj._Prio = 0;
                cell_obj.CellID = dest_obj.c_str();
                cell_obj.PkgBuf = string(send_data,send_len);
                cell_obj.ActNode = rsp_act_node;

                //发送增补
                cell_obj.srcObj = this;//dest_conn->ProcessObj;
                //Conn *conn=(Conn *)this->SendCBFuncHandle;
                cell_obj.src_conn_name = this->GetParamValue("@_CONN_NAME"); //conn->obj_name.c_str();
                cell_obj.dest_conn_name = dest_obj.c_str();
                cell_obj.trans_type= trans_type;
                cell_obj.close_after_send=close_after_send;
                cell_obj.time_out=send_timeout;
                cell_obj.time_out_path="";
                cell_obj.time_start=0;
                cell_obj.rstate_field=rstate;
                cell_obj.send_state=SendNodeQueue::WAITTING;

                //this->DEBUGY((string)"begin push:"+STRING_Helper::iToStr(dest_conn->ProcessObj->sendQueue->GetSize()));
                if (!dest_conn->ProcessObj->sendQueue->PushItem(cell_obj)){
                    //压入队列处理
                    err_str=(string)"SendMsgAddQueue()->PushItem() error!";
                    r=-1;
                }
                if (r==0 && send_as_queue_null){
                    //立即发送处理
                    err_str=dest_conn->ProcessObj->DoQueueSendDest();
                    if(err_str.size()!=0)
                        r=-1;
                }

            }
        }
        //释放内存空间
        free(send_data);
    }

    if (rstate_str.size()!=0) {
        if (r != 0) {
            const char *err = "0x08";
            this->_PubObj->SetNodeValue(rstate_str.c_str(), err, Http_ParseObj::STR_COPY_OBJ); //失败，返回原
        }else
            this->_PubObj->SetNodeValue(rstate_str.c_str(), "", Http_ParseObj::STR_COPY_OBJ); //成功
    }

    //后续动作
    if(err_str!="")
        this->SHOW_ERROR(err_str);

    this->ParseDeep--;
    return ParseProcess::ACT_SUCCESS;
}

string ObjectProcess::DoQueueSendDest(){
    int r=0;
    string err_str("");

    if (this->sendQueue->GetSize()==0)
        return 0;

    SendNodeQueueCell_T *cell=&(this->doCellObj);
    if (cell->send_state==SendNodeQueue::SENDED){
        //发送对象未进入状态
        err_str=(string)"DO_QUEUE_SEND() parnet->doCellObj.state is SendNodeQueue::SENDED---may be some task not end!";
        goto end;
    }

    this->DEBUGY((string)"PopItem: size="+STRING_Helper::iToStr(this->sendQueue->GetSize()));
    if (this->sendQueue->PopItem(this->doCellObj)){
        //立即发送
        cell->time_start =LW_GetTickCount();//取得当前时间
        if (this->SendCBFunc((const char*)cell->PkgBuf.c_str(), cell->PkgBuf.size(),
                                                 "@THIS_FD", "", cell->trans_type, cell->close_after_send,
                                                 this->SendCBFuncHandle)!=0){

            err_str=(string)"SendMsgAddQueue()->Sending Error: r="+STRING_Helper::iToStr(r);//???this->SendCBFuncHandle
            cell->send_state=SendNodeQueue::CANCLED;
            goto end;
        } else
            cell->send_state=SendNodeQueue::SENDED;
    }
    end:
    return err_str;
}

string ObjectProcess::DoQueueRcvAction(SendNodeQueueCell_T *cell_obj,string &pkg,string &state_value, ParseProcess::PV_AddParas_T &add_paras){
    //int r=0;
//操作=
    string err_str("");

    //调用队列中的Pkg操作
    if (cell_obj->send_state!=SendNodeQueue::SENDED){
        //发送对象未进入状态
        err_str=(string)"DoQueueRcvAction() parnet->doCellObj.state should be !(state=SendNodeQueue::SENDED)"+STRING_Helper::iToStr(cell_obj->send_state);
        goto end;
    }
    if (cell_obj->srcObj==NULL){
        err_str=(string)"DoQueueRcvAction() parnet->doCellObj->srcObj not exited!";
        goto end;
    }
    this->DEBUGY((string)"src_obj:"+cell_obj->src_conn_name);
    Conn *src_cell;
    if ((src_cell=GB_ConnQueue->FindConnByObjName(cell_obj->src_conn_name.c_str(), 0,true)) == NULL){
        err_str=(string)"DoQueueRcvAction() parnet->doCellObj->srcObj ("+cell_obj->src_conn_name+") has been cancled!";
        goto end;
    }

    if (pkg.size()!=0){
        if (cell_obj->srcObj->_PubObj->SetNodeValue("@__PKG", pkg.data(), Http_ParseObj::STR_COPY_OBJ, pkg.size()) != Http_ParseObj::SUCCESS) {
            err_str="Attribute->can't read'@__PKG'---maybe readonly! ";
            goto end;
        }
        cell_obj->srcObj->_PubObj->SetNodeValue(cell_obj->rstate_field,state_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
        cell_obj->srcObj->FUNCTION(cell_obj->ActNode, add_paras);   //ACTIONS 操作
        cell_obj->srcObj->_PubObj->SetNodeValue(cell_obj->rstate_field,"", Http_ParseObj::STR_COPY_OBJ);
        cell_obj->srcObj->_PubObj->SetNodeValue("@__PKG", "", Http_ParseObj::STR_COPY_OBJ);
    }

end:

    return err_str;
}

ObjectProcess::ObjFunc::ObjFunc():ParseProcess::Http_ParseFunc() {//ly_func
    this->FuncMapAdd.insert(pair<string, ObjectFunc_T>((string )"CONN_OBJ_CREATE", ObjectProcess::ObjFunc::CONN_OBJ_CREATE));
    this->FuncMapAdd.insert(pair<string, ObjectFunc_T>((string )"CONN_OBJ_CANCLE", ObjectProcess::ObjFunc::CONN_OBJ_CANCLE));
    this->FuncMapAdd.insert(pair<string, ObjectFunc_T>((string )"DO_QUEUE_SEND", ObjectProcess::ObjFunc::DO_QUEUE_SEND));
    this->FuncMapAdd.insert(pair<string, ObjectFunc_T>((string )"DO_QUEUE_RACT", ObjectProcess::ObjFunc::DO_QUEUE_RACT));
    this->FuncMapAdd.insert(pair<string, ObjectFunc_T>((string )"DO_QUEUE_SIZE", ObjectProcess::ObjFunc::DO_QUEUE_SIZE));
    //this->FuncMapAdd.insert(pair<string, ObjectFunc_T>((string )"DO_QUEUE_ERR", ObjectProcess::ObjFunc::DO_QUEUE_ERR));
    this->FuncMapAdd.insert(pair<string, ObjectFunc_T>((string )"CHK_QUEUE_TIMEOUT", ObjectProcess::ObjFunc::CHK_QUEUE_TIMEOUT));
    this->FuncMapAdd.insert(pair<string, ObjectFunc_T>((string )"START_TIME_TASK", ObjectProcess::ObjFunc::START_TIME_TASK));
    this->FuncMapAdd.insert(pair<string, ObjectFunc_T>((string )"CANCLE_TIME_CYCLE", ObjectProcess::ObjFunc::CANCLE_TIME_TASK));
    this->FuncMapAdd.insert(pair<string, ObjectFunc_T>((string )"TASK_CYCLE_RUN", ObjectProcess::ObjFunc::TASK_CYCLE_RUN));
    this->FuncMapAdd.insert(pair<string, ObjectFunc_T>((string )"CHECK_OBJS", ObjectProcess::ObjFunc::CHECK_OBJS));
    this->FuncMapAdd.insert(pair<string, ObjectFunc_T>((string )"TASK_CYCLE_KEY", ObjectProcess::ObjFunc::TASK_CYCLE_KEY));
}

ObjectProcess::ObjFunc::~ObjFunc(){
    //if (this->FuncMap!=0)
    //delete this->FuncMap;
}

string ObjectProcess::ObjFunc::Do(const char *func_name,string func_field,string &func_return,ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    ParseFuncMap_T::iterator func_map;
    ObjProcFuncMap_T::iterator func_map_add;
    if ((func_map_add = this->FuncMapAdd.find(func_name))!=this->FuncMapAdd.end()){
        if (func_field.find("{")!=string::npos){//如果需要add_paras解析,例如@FUNC({}/***,****).
            string func_field1;
            parent->ParseValue(func_field,func_field1,add_paras);
            return func_map_add->second(func_field1, func_return, parent, add_paras);
        }else
            return func_map_add->second(func_field,func_return,parent,add_paras);
    }else if ((func_map = this->FuncMap.find(func_name))!=this->FuncMap.end()){
        if (func_field.find("{")!=string::npos){//如果需要add_paras解析,例如@FUNC({}/***,****).
            string func_field1;
            parent->ParseValue(func_field,func_field1,add_paras);
            return func_map->second(func_field1, func_return, parent, add_paras);
        }else
            return func_map->second(func_field,func_return,parent,add_paras);
    } else{
        return (string)"@System not defined @"+func_name+"!";
    }
}

string ObjectProcess::ObjFunc::CONN_OBJ_CREATE(string func_field,string &func_return,ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    string err_str;
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,6,parent,add_paras)<1){//Param数目错误
        return (string)"@CONN_OBJ_CREATE()parameter error :" + func_field;
    }

    err_str=ParseProcess::Http_ParseFunc::CONN_OBJ_CREATE(func_field,func_return,parent,add_paras);

    /*if (err_str.size()==0){
        Conn *dest_conn;
        //判断是否有该链接
        string obj_name=paras.size()>4 ? paras[4].c_str():paras[0];
        if ((dest_conn = GB_ConnQueue->FindConnByObjName(obj_name.c_str(), 0,true))!= NULL) {
            //加入等待，方便CONN_ERR回调。
            SendNodeQueueCell_T *cell = &(dest_conn->ProcessObj->doCellObj);  //连接目标对象。
            //parent->SendCBFuncHandle=dest_conn;
            cell->send_state = SendNodeQueue::CONNECTING;
            cell->time_start=LW_GetTickCount();
            if (paras[5].size()!=0 && STRING_Helper::CheckNum(paras[5])){
                cell->time_out=STRING_Helper::StrToi(paras[5].c_str());
            } else{
                cell->time_out=Default_SendQueue_ConnVal ;
            }
            cell->time_out_path=(paras[1].size()!=0 ? paras[1].c_str(): "_CONN_ERR");
        }
    }*/

    return err_str;
}

string ObjectProcess::ObjFunc::CONN_OBJ_CANCLE(string func_field,string &func_return,ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    string err_str("");
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,2,parent,add_paras)<1){//Param数目错误
        return (string)"@CONN_OBJ_CANCLE()parameter error :" + func_field;
    }
    Conn *dest_conn;
    //判断是否有该链接
    if ((dest_conn = GB_ConnQueue->FindConnByObjName(paras[0].c_str(), 0,true))== NULL)
        return "CONN_OBJ_CANCLEI(@CONN_NAME) CONN_NAME is not found!";
    //if (dest_conn!=(Conn *)parent->SendCBFuncHandle)
        //return "CONN_OBJ_CANCLEI(@CONN_NAME) CONN_NAME is not this conn!";
    
    string obj_name;
    SendNodeQueueCell_T *cell=&(parent->doCellObj);
    if (paras[0].size()==0 && cell->send_state!=SendNodeQueue::CONNECTING){//关闭当前 !!!一般不能直接用于关闭自己，而应该使用<SET_OBJ To="@_CLOSE_AFTER_READ">true</SET_OBJ>，这样确保conn和当前对象都能够关闭。
        Conn *conn=(Conn *)parent->SendCBFuncHandle;
        if (conn!=NULL && conn->is_act){
            cout << "doing cancle1 " << paras[0] <<endl;
            WIZBASE_conn_disconnect(conn);
        }
    }else {//关闭指定对象
        Conn *conn;
        if ((conn=GB_ConnQueue->FindConnByObjName(paras[0].c_str(), 0,true))!=NULL){
            cout << "doing cancle2 " << paras[0] <<endl;
            WIZBASE_conn_disconnect(conn);
        }else
            err_str=(string)"object name("+paras[0]+") not exited!";
    }
     
    return err_str;
}

string ObjectProcess::ObjFunc::DO_QUEUE_SEND(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node,timeout_err_node;
    //int time_val;

    //操作
    //BYTE r = 0;
    string err_str("");
    size_t num=0;

    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,2,parent,add_paras)<0){//Param数目错误
        return (string)"@DO_QUEUE_SEND()parameter error :" + func_field;
    }

    SendNodeQueueCell_T *cell=&(parent->doCellObj);

    if((num=parent->sendQueue->GetSize()) != 0) {
        err_str=parent->DoQueueSendDest();
        cell->time_out=paras[0].size()!=0 ? STRING_Helper::StrToi(paras[0]) : Default_SendQueue_TimeVal ;
        cell->time_out_path=(paras[1].size()!=0 ? paras[1].c_str(): "_TIMEOUT_ERR");
    }
    func_return=STRING_Helper::iToStr((int)num);
    
    return err_str;
}

string ObjectProcess::ObjFunc::START_TIME_TASK(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node,timeout_err_node;
    func_return="";

    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,2,parent,add_paras)<0){//Param数目错误
        return (string)"@START_TIME_TASK()parameter error :" + func_field;
    }
    const char *conn_name=paras[0].c_str();
    Conn *ctx=GB_ConnQueue->FindConnByObjName(conn_name,0,true);
    if (WIZBASE_StartQueueTimeTask(ctx)==-1){
        return (string)"/XMLParse/Connect/CONNECT[@Name='"+ ctx->obj_name_orign + "']_TIME_TASK not defined!";
    }
    
    return "";
}

string ObjectProcess::ObjFunc::CANCLE_TIME_TASK(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node,timeout_err_node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,2,parent,add_paras)<0){//Param数目错误
        return (string)"@CANCLE_TIME_TASK()parameter error :" + func_field;
    }
    const char *conn_name=paras[0].c_str();
    Conn *ctx=GB_ConnQueue->FindConnByObjName(conn_name,0,true);
    WIZBASE_CancleQueueTimeTask(ctx); //time_task;
    
    return "";
}

string ObjectProcess::ObjFunc::DO_QUEUE_RACT(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node;
    string err_str;

    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 3, parent, add_paras)<1) {//Param数目错误
        return (string)"@DO_QUEUE_RACT()parameter error :" + func_field;
    }

    BYTE send_state;
    if (paras[1]=="")
        send_state=SendNodeQueue::RECEIVED;
    else
        send_state=SendNodeQueue::CANCLED;
    
    //用于发送成功后的回调
    parent->DEBUGY((string)"RcvAct: msg(Now) size="+STRING_Helper::iToStr(paras[0].size()));
    if (parent->doCellObj.send_state==SendNodeQueue::SENDED){
        SendNodeQueueCell_T *cell=&(parent->doCellObj);
        err_str=parent->DoQueueRcvAction(cell,paras[0],paras[1],add_paras);
        cell->send_state=send_state;
    } else
        parent->DEBUGY((string)"connect object p"+paras[0]+"is not ready(SENDED),but("+STRING_Helper::iToStr(parent->doCellObj.send_state)+")!");
    
    if (paras[2]=="POP_ALL"){
        //用于缓存队列中的所有内容，依次发送。
        parent->DEBUGY((string)"RcvAct: msg(ALL) num="+STRING_Helper::iToStr(parent->sendQueue->GetSize()+1));
        while(parent->sendQueue->PopItem(parent->doCellObj)){
            //回调操作
            SendNodeQueueCell_T *cell=&(parent->doCellObj);
            cell->send_state=SendNodeQueue::SENDED;
            err_str=parent->DoQueueRcvAction(cell,paras[0],paras[1],add_paras);
            cell->send_state=send_state;
            if (err_str.size()!=0)
                break;
        }
    }

    /*if (paras[2]=="POP_ALL"){
        //用于缓存队列中的所有内容，依次发送。
        parent->DEBUGY((string)"RcvAct: msg(ALL) num="+STRING_Helper::iToStr(parent->sendQueue->GetSize()+1));
        while(parent->sendQueue->PopItem(parent->doCellObj)){
            //回调操作
            SendNodeQueueCell_T *cell=&(parent->doCellObj);
            cell->send_state=SendNodeQueue::SENDED;
            err_str=parent->DoQueueRcvAction(cell,paras[0],paras[1],add_paras);
            cell->send_state=send_state;
            if (err_str.size()!=0)
                break;
        }
    }else{//只用于发送成功后的回调
        parent->DEBUGY((string)"RcvAct: msg(Now) size="+STRING_Helper::iToStr(paras[0].size()));
        if (parent->doCellObj.send_state==SendNodeQueue::SENDED){
            SendNodeQueueCell_T *cell=&(parent->doCellObj);
            err_str=parent->DoQueueRcvAction(cell,paras[0],paras[1],add_paras);
            cell->send_state=send_state;
        } else
            parent->DEBUGY((string)"connect object p"+paras[0]+"is not ready(SENDED),but("+STRING_Helper::iToStr(parent->doCellObj.send_state)+")!");
    }*/

    return err_str;
}

string ObjectProcess::ObjFunc::DO_QUEUE_SIZE(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node;

    //操作
    BYTE num= 0;
    string err_str("");
    num=parent->sendQueue->GetSize();
    func_return=STRING_Helper::iToStr(num);

    return err_str;
}

string ObjectProcess::ObjFunc::CHK_QUEUE_TIMEOUT(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node;
    string err_str="";
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras)<1) {//Param数目错误
        return (string)"@CHK_QUEUE_TIMEOUT()parameter error :" + func_field;
    }

//查找指定的连接是否有超时处理。
    Conn *dest_conn;
    //判断是否有该链接
    if ((dest_conn = GB_ConnQueue->FindConnByObjName(paras[0].c_str(), ConnQueue::TestOrignName,true))!= NULL){
        UInt64 now=LW_GetTickCount();//取得当前时间
        SendNodeQueueCell_T *cell=&(dest_conn->ProcessObj->doCellObj);

        if (cell->send_state==SendNodeQueue::SENDED && (paras[1].size()==0 || STRING_Helper::PUB_FindStrIs(paras[1],"RCV"))){
            if (now>(cell->time_start+cell->time_out*1000)){
                //超时处理操作
                string act_node = (string) "/XMLParse/Connect/CONNECT[@Name='"
                                  + dest_conn->obj_name_orign + "']/" + cell->time_out_path;
                ParseXml_Node err_node=parent->GetXmlNode(act_node.c_str());
                if (err_node)
                    dest_conn->ProcessObj->FUNCTION(err_node, add_paras);   //ACTIONS 操作
                cell->send_state = SendNodeQueue::CANCLED;
            }
        }else if (cell->send_state==SendNodeQueue::CONNECTING && STRING_Helper::PUB_FindStrIs(paras[1],"CONN")){
            if (now>(cell->time_start+cell->time_out*1000) && dest_conn->obj_name_orign.size()!=0){
                //超时处理操作
                string act_node = (string) "/XMLParse/Connect/CONNECT[@Name='"
                                  + dest_conn->obj_name_orign + "']/" + cell->time_out_path;
                ParseXml_Node err_node=parent->GetXmlNode(act_node.c_str());
                if (err_node)
                    dest_conn->ProcessObj->FUNCTION(err_node, add_paras);   //ACTIONS 操作
                cell->send_state = SendNodeQueue::CANCLED;
            }
        }
    }

    return err_str;
}

static UInt64 OBJ_FUNC_TASK_CYCLE_KEY_COUNT=0;
string ObjectProcess::ObjFunc::TASK_CYCLE_KEY(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {//ly task
    vector<string> paras;
    ParseXml_Node node,timeout_err_node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,4,parent,add_paras)<1){//Param数目错误
        return (string)"@TASK_CYCLE_KEY()parameter error :" + func_field;
    }
    //取得STATE
    int state=0;
    int pos=0;
    if ((state=(BYTE)STRING_Helper::C_CmpFirstStr((char *)paras[0].c_str(),pos,"FREE|START|RCB|WAIT","|",(int)paras[0].size(),true))<0){
        return (string)"@TASK_CYCLE_RUN()parameter error :" + func_field;
    }
    //取得超时累加
    int time_add;
    if (state==0){
        func_return=STRING_Helper::HBCStrToHBCStr("0x0000000000000000", "hex", "char", false);
        return "";
    }else if(paras[1]==""){
        time_add=0;
    }else{
        if (!STRING_Helper::CheckNum(paras[1])){
            return (string)"@TASK_CYCLE_RUN()parameter error(paras[1] should be num):" + func_field;
        }
        time_add=STRING_Helper::StrToi(paras[1]);
    }
    
    //取得state
    UInt64 h_state=state;
    h_state=h_state<< 62;
    
    //取得累加区分值
    UInt64 h_count=0;
    if (paras[3].size()==0){
        h_count=OBJ_FUNC_TASK_CYCLE_KEY_COUNT;
        if (h_count < 0xffff)
            h_count++;
        else
            h_count= 0;  //尽量不重复的session
    }else if (!STRING_Helper::CheckNum(paras[3])){
        return (string)"@TASK_CYCLE_RUN()parameter error(paras[3] should be num!):" + func_field;
    }else{
        h_count=STRING_Helper::StrToi(paras[3]);
        h_count=(h_count<<48)>>48;
    }
    //取得时间
    UInt64 tmp2 = LW_GetTickCount()+time_add*1000;
    UInt64 h_ms = tmp2 << 16;
    
    UInt64 key = h_state + h_ms + h_count; //取得时间顺序随机数GetTickCount+Rand
    if (paras[2].size()!=0){
        string result=STRING_Helper::putUINTToStr(key, 64);
        func_return=STRING_Helper::HBCStrToHBCStr(result, "char", "hex", (paras[2]=="h%" ? false:true));
    }else
        func_return=STRING_Helper::putUINTToStr(key, 64);
    
    return "";
}

string ObjectProcess::ObjFunc::TASK_CYCLE_RUN(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
   
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,2,parent,add_paras)<0){//Param数目错误
        return (string)"@TASK_CYCLE_RUN()parameter error :" + func_field;
    }
    
    if (GB_AvartarDB->DoTaskTableCycleRun(parent, paras[0],func_return,add_paras)!= 0)
        return (string)"TASK_CYCLE_RUN()->DoTaskTableCycleRun() error!";
    
    return "";
}


string ObjectProcess::ObjFunc::CHECK_OBJS(string func_field, string &func_return, ObjectProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 1, parent, add_paras) < 1) {//Param数目错误
        return (string)"@CHECK_OBJS()parameter error :" + func_field;
    }
    
    if (paras[0]=="COUNT_CONN"){
        int i=GB_ConnQueue->CountConn();
        func_return=STRING_Helper::iToStr(i);
    }else if (paras[0]=="CONNS_NUM"){
        int i=GB_ConnQueue->ConnNum;
        func_return=STRING_Helper::iToStr(i);
    }else if (paras[0]=="OBJS_NUM"){
        int i=PUB_ObjectProcessNum;
        func_return=STRING_Helper::iToStr(i);
    }else
        func_return = "";
    
    return "";
}

