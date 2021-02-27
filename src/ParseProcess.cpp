#include "stdafx.h"
#include <iostream>
#include "ParseProcess.h"
//#include "HTTP_Config.h"
#include "STRING_Helper.h"
//#if !defined(__ANDROID__) && !defined(__APPLE__)
//#include "b64.h"
#include "sha1.h"
#include "cws.h"
#include "base64.h"
//#endif
//#include "BitDB.h"
#ifdef SENSOR_DBM
    #include "SensorDBManager.h"
#endif
#ifdef JAWS_CDBM
    #include "CDBManager.h"
#endif
#ifdef NO_CCODE
#else
#include "CChineseCode.h"
#endif
#include "Linux_Win.h"

#include <string>
#include <stdio.h>
#include "string.h"
#include <fstream>
#include <sstream>
#include "pugixml.hpp"
#include <math.h>
#include "WizBase.h"
#include "ParseExternal.h"
#include <fstream>
#include "AvartarDBManager.h"
#include<algorithm>
#include<cstring>
#ifdef WIN32
	#include "shellapi.h"
	#include "tchar.h"
    #include "io.h"
#else
    #include<dirent.h>
#endif


#ifdef THIRD_QQ
#include "ThirdQQ.h"
#endif
//#include "BitDB.h"
//#include <zlib.h>
//#include "URLCoder.h"
//#include "tchar.h"
#include <stdio.h>
#include <string>
#include <sys/stat.h>
//#include <unistd.h>
/*extern void BitDB_SaveDB(byte tableindex);
 extern bool WriteBufferToDB_AllNodes(byte *buf, UInt32 len, byte tableindex, UInt32 bitmap, bool sync, bool jennicuart);
 extern void WriteBufferToDB_OneNode(byte* buf, UInt32 &bufoffset, byte tableindex, UInt32 nodeid, UInt32 bitmap, bool sync, bool jennicuart);
 extern long BuildBufferArrayFromDB(string &buf_str,byte tableindex, UInt32 nodeid,UInt32 bitmap, bool jennicuart);
 extern long BuildBufferFromDB(string &buf_str,byte tableindex, UInt32 nodeid,UInt32 bitmap, bool jennicuart);*/
//extern int  lws_b64_encode_string(const char *in, int in_len,  char *out,  int out_size);

//extern int sha1(const char *source, int source_len,char *buf);

using namespace std;
using namespace pugi;


/*ParseProcess::~ParseProcess(void){
 
 }*/

void ParseProcess::setAddress(const string &ip, int port)
{
    char a[20];
    this->ip_ = ip;
    this->port_ = port;
    itoa(this->port_,a,10);
    this->host_ = this->ip_+":"+a;
}

bool ParseProcess::TimeTrack(const char *name, BYTE method) {
    UInt64 this_time = LW_GetTickCount();//取得当前时间
    if (method == TRACK_START)
        return GB_AvartarDB->SysInfoTimeTrack(name, AvartarDBManager::SYS_INFO_START_TIME, this_time);
    else
        return GB_AvartarDB->SysInfoTimeTrack(name, AvartarDBManager::SYS_INFO_END_TIME, this_time);
}

/*bool ParseProcess::TimeTrack(const char *name, BYTE method) {
	UInt64 this_time = LW_GetTickCount();//取得当前时间
	BB_OBJ *table_obj = GB_AvartarDB->GetSysInfoTable();
	if (table_obj == NULL)
 return false;
	int rec;
	//取得name所对应的记录
	if ((rec = BB_GET_ARRAY_NUM_BY_INDEX_STRU(&(GB_AvartarDB->SysInfoTableStru), (BYTE *)name, strlen(name))) < 0) {
 if (rec != -1) {
 this->SHOW_ERROR((string)"TimeTrack->GET_SYS_INFO_TABLE error0 [" + name + "] not found!");
 return false;
 }
 //添加索引、记录
 if ((rec = BB_APPEND_ARRAY_REC_BY_INDEX_STRU(&(GB_AvartarDB->SysInfoTableStru), NULL, 0, (BYTE *)name, strlen(name)))<0) {
 this->SHOW_ERROR((string)"TimeTrack->GET_SYS_INFO_TABLE error1 [" + name + "] not found!");
 return false;
 }
	}
	if (method > AvartarDBManager::SYS_INFO_TABLE_MAX_CELL_NUM) {
 this->SHOW_ERROR((string)"TimeTrack->GET_SYS_INFO_TABLE error [" + STRING_Helper::iToStr(method) + "] is tool large!");
 return false;
	}
 
	switch (method) {
	case SYS_INFO_START_TIME:
 //起始设置
 BB_PUT_TABLE_ITEMS_DATA_STRU(&(GB_AvartarDB->SysInfoTableStru), rec, SYS_INFO_START_TIME, (BYTE *)&this_time, sizeof(this_time), BB_TABLE_PUT_OPERATE_SET);
 break;
	case SYS_INFO_END_TIME:
 //读取//SYS_INFO_COUNT, SYS_INFO_ALL_TIME
 BYTE bitmap_base = 0x80;
 BYTE bitmap_read = (bitmap_base >> SYS_INFO_START_TIME) & (bitmap_base >> SYS_INFO_ALL_TIME) & (bitmap_base >> SYS_INFO_COUNT);
 UInt64 read_uint64[3];
 BB_READ_TABLE_BY_NUM_STRU(&(GB_AvartarDB->SysInfoTableStru), rec, (BYTE *)read_uint64, 0, &bitmap_read, 1, BB_TABLE_REC_WR_BITMAP);
 
 //累加或写入操作
 UInt64 write_uint64[3];
 write_uint64[0] = read_uint64[2]+(this_time - read_uint64[0]);
 write_uint64[1] = read_uint64[1]++;
 write_uint64[2] = this_time;
 BYTE bitmap_write = (bitmap_base >> SYS_INFO_ALL_TIME) & (bitmap_base >> SYS_INFO_COUNT);
 if ((rec = BB_WRITE_TABLE_BY_NUM_STRU(&(GB_AvartarDB->SysInfoTableStru), (BYTE *)&write_uint64, 0, rec, &bitmap_write, 1, BB_TABLE_REC_WR_BITMAP)) < 0) {
 this->SHOW_ERROR((string)"TimeTrack->BB_WRITE_TABLE_BY_NUM_STRU,Write table[" + name + "]error!");
 return false;
 }
 break;
	default:
 return false;
 break;
	}
	return true;
 }*/

/*
 void ParseProcess::TimeTrack(const char *name, BYTE method) {
	UInt64 this_time = LW_GetTickCount();//取得当前时间
	BB_OBJ *table_obj = GB_AvartarDB->SysInfoTableStru.ThisObj;
 
	switch (method) {
	case SYS_INFO_START_TIME:
 //起始设置
 UInt64 rec = GB_AvartarDB->GetSysInfoValue(name);
 GB_AvartarDB->SetSysInfoValue(rec, SYS_INFO_START_TIME, this_time);
 if (rec != 0xFFFFFFFFFFFFFFFF)
 this->SHOW_ERROR((string)"TimeTrack(): Step Name[" + name + "]not defined!");
 
 break;
	case SYS_INFO_END_TIME:
 this->ProcessCount++;
 UInt64 start_time, rec=GB_AvartarDB->GetSysInfoValue(name);
 if (rec != 0xFFFFFFFFFFFFFFFF){
 start_time=GB_AvartarDB->GetSysInfoValue(rec, SYS_INFO_START_TIME);
 GB_AvartarDB->SetSysInfoValue(rec, SYS_INFO_ALL_TIME, this_time-start_time, true);
 GB_AvartarDB->SetSysInfoValue(rec, SYS_INFO_COUNT, 1, true);
 GB_AvartarDB->SetSysInfoValue(rec, SYS_INFO_END_TIME, 1);
 }
 
 break;
	default:
 break;
	}
 }*/

//功能:-循环parse_node node 的 sub node ,进行相关动作 node 解析.
//		-CASE ACTIONS: ParseAction递归调用ACTIONS的下级 node 解析.
//		-CASE SET_OBJ: 则取得SET_OBJ的 sub node ,相关 attribute 发送SET_OBJ函数；
//		-CASE _CASE_:其它函数同SET_OBJ;
//注:	自身可以被ACTIONs和_CASE_和_FOR_等动作递归调用.
int ParseProcess::ParseAction(ParseXml_Node parse_node,PV_AddParas_T &add_paras){
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    
    for (ParseXml_Node tool = this->_ActParseXml->FirstChild(parse_node); tool; tool = this->_ActParseXml->NextChild(tool))
    {
        //判断是否进入,TestConditions判断
        const char *act_test=this->_ActParseXml->NodeAttr(tool,"Test");
        if (strlen(act_test)!=0 && !this->TestConditionStr(act_test,add_paras))
            continue;
        
        if (strcmp(this->_ActParseXml->NodeName(tool),"ACTIONS")==0){
            this->DEBUGY(tool);
            this->ParseAction(tool,add_paras); //ParseAction递归调用ACTIONS的下级 node 解析.
            
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"SET_OBJ")==0){
            this->SET_OBJ(tool,add_paras);//则取得SET_OBJ的 sub node ,相关 attribute 发送SET_OBJ函数；
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"PRINT")==0){
            this->PRINT(tool,add_paras);//则取得SET_OBJ的 sub node ,相关 attribute 发送SET_OBJ函数；
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"DB_WRITE")==0){
            this->DB_WRITE(tool,add_paras);//则取得SET_OBJ的 sub node ,相关 attribute 发送SET_OBJ函数；
#ifdef SENSOR_DBM
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"DB_SEARCH")==0){
            this->DB_SEARCH(tool,add_paras);
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"DB_WRITE_BKDB")==0){
            this->DB_WRITE_BKDB(tool,add_paras);//则取得SET_OBJ的 sub node ,相关 attribute 发送SET_OBJ函数；
#endif
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
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"PARSE")==0){
            this->PARSE(tool,add_paras);
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"SEND")==0){
            this->SEND(tool,add_paras);
        }else if (strcmp(this->_ActParseXml->NodeName(tool), "SET_QUEUE") == 0) {
            this->SET_QUEUE(tool, add_paras);
        }
        else if (strcmp(this->_ActParseXml->NodeName(tool),"FUNCTION")==0 ||strcmp(this->_ActParseXml->NodeName(tool),"LocalEncode")==0 || strcmp(this->_ActParseXml->NodeName(tool),"ParsePacket")==0 ){
            ;//非动作 node
        }else{
            string err_str("不可识别的解析 node :<");err_str+=this->_ActParseXml->NodeName(tool);err_str+=">!";
            this->SHOW_ERROR(err_str);
        }
    }
    
    this->ParseDeep--; //用于记录解析层次深度.
    
    return 0;
}

int ParseProcess::ACTION(ParseXml_Node parse_node,PV_AddParas_T &add_paras){
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    this->DEBUGY(parse_node);
    string dest;
    const char *add_act_str=this->_ActParseXml->NodeAttr(parse_node,"ActStr");
    
    //普通动作
    ParseValue(parse_node,dest,add_paras);
    
    if(strcmp(add_act_str,"true")==0){
        //增加 node 动作
        string xml;
        xml=string("<ACTIONS>")+dest+"</ACTIONS>";
        Http_ParseXML * temp_xml = new Http_ParseXML();//创建xml Object .
        if (temp_xml->Load(xml.c_str())!=Http_ParseXML::LOAD_XML_SUCCESS)
            this->SHOW_ERROR(this->_PubObj->ErrorStr);
        else{
            //动作处理
            parse_node.append_copy(temp_xml->FirstChild(temp_xml->ObjXml));//???该模式似乎会累计xml
            this->ParseAction(parse_node,add_paras); //ParseAction递归调用ACTIONS的下级 node 解析.
        }
        //if (temp_xml)
        	//delete temp_xml;
    }
    
    this->ParseDeep--;
    return ACT_SUCCESS;
}

int ParseProcess::PARSE(ParseXml_Node parse_node, PV_AddParas_T &add_paras){
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    string err_str("");
    
    //取得 Source  node 内容
    string parse_buff;
    this->ParseValue(parse_node,parse_buff,add_paras);
    
    //ParsePacket操作
    //取得Xpath of ParseNode ???差错提示
    string parse_path,parse_path_temp = this->_ActParseXml->NodeAttr(parse_node,"Packet");//没有 attribute 为空的处理.
    STRING_Helper::strTrim(parse_path_temp);
    if (parse_path_temp.compare(0,1,"{")==0)
        this->ParseValue(parse_path_temp,parse_path,add_paras);
    else
        parse_path=parse_path_temp;
    //string parse_path2=this->_ActParseXml->NodeAttr(parse_node,"Packet2");
    
    //Parse Method:BIN_LEN|HTTP
    string method=this->_ActParseXml->NodeAttr(parse_node,"Method");
    
    //创建目标xml根 node ；
    //取得临时存储目标
    string to_obj;
    string to_obj_tmp=this->_ActParseXml->NodeAttr(parse_node,"ToObj");
    if (to_obj_tmp.size()==0)
        to_obj="@_THIS_PARSE_OBJ";   //???{FD}
    else{
        this->ParseValue(to_obj_tmp,to_obj,add_paras);
    }
    
    
    // Config DEBUGY
    const char* is_debug=this->_ActParseXml->NodeAttr(parse_node,"Debug");
    if (strstr(is_debug,"SetTo")!=0)
        this->_PubPacket->SetDebug();
    
    //在根 node 写入相关的字段
    if (method=="@_PACKET_XML" || method.size()==0){//采用xml解析模式
        string parse_xml="<Root></Root>";//临时创建一个今后还可以变化.
        Http_ParseXML * temp_xml=this->_PubObj->XmlObjCreate(to_obj.c_str(),parse_xml.c_str());//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
        if (temp_xml==NULL)
            this->SHOW_ERROR(this->_PubObj->ErrorStr);
        ParseXml_Node xml_node =temp_xml->FirstChild(temp_xml->ObjXml);
        if (!this->_PubPacket->GetFieldsFormOriginStr(parse_path.c_str(),parse_buff, xml_node,to_obj+":"+"/Root")){
            // Object Param
            this->SHOW_ERROR(this->_PubPacket->ErrorStr);
        };
        
    }else if (method=="@_PACKET_OBJ"){//采用xml解析模式
        //清空子节点和创建节点
        this->_PubObj->Delete(to_obj.c_str(),true);
        this->_PubObj->SetNodeValue(to_obj.c_str(),"",Http_ParseObj::STR_COPY_OBJ);
        //解析操作
        if (!this->_PubPacket->GetFieldsFormOriginStr(parse_path.c_str(),parse_buff,this->_PubObj,to_obj)){
            // Object Param
            this->SHOW_ERROR(this->_PubPacket->ErrorStr);
        };
    }else {//采用Method解析模式
        //清空子节点和创建节点
        this->_PubObj->Delete(to_obj.c_str(),true);
        this->_PubObj->SetNodeValue(to_obj.c_str(),"",Http_ParseObj::STR_COPY_OBJ);
        err_str = ParseExt::DECODE(method.c_str(),parse_buff,this->_PubObj,to_obj);
    };
    
    //关闭debug
    if (strstr(is_debug,"SetTo")!=0)
        this->_PubPacket->CloseDebug();
    //--------------------------------
    if(err_str!=""){
        this->SHOW_ERROR(err_str);
        this->ParseDeep--;
        return ParseProcess::ACT_ERROR;
    }else{
        this->ParseDeep--;
        return ParseProcess::ACT_SUCCESS;
    }
}

//功能:路线判断函数;
//动作:
//`	- 循环所有condition node (为and关系;)
//		//循环condition所有 sub node .
//		(1)CASE @tag=”include”;判断是否所指定的 node 包含.
//      (2)CASE @tag=”!include"
//	- 组合上述判断Conditions(****or*****)and(****,****)
//  - 如果false则什么也不做,如果true则调用parse_node下其它的 node 递归ParseAction
// 转发操作解析函数
/*int ParseProcess::SWITCH(ParseXml_Node parse_node,PV_AddParas_T &add_paras){
	this->DEBUGY(parse_node);
	this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
	int r;
 
	for (ParseXml_Node tool = this->_ActParseXml->FirstChild(parse_node); tool; tool = this->_ActParseXml->NextChild(tool))
 {
 if (strcmp(this->_ActParseXml->NodeName(tool),"CASE")==0){
 r=this->SWITCH_CASE(tool,false,add_paras);
 if (r==SWITCH_CASE_BREAK || r==ACT_ERROR)
 break; //ParseAction递归调用ACTIONS的下级 node 解析.
 else
 continue;//SWITCH_CASE_NEXT;继续测试下一个CASE node .
 }else if (strcmp(this->_ActParseXml->NodeName(tool),"DEFAULT")==0){
 this->DEBUGY(tool);
 int r=this->SWITCH_CASE(tool,true,add_paras);
 if (r==SWITCH_CASE_BREAK || r==ACT_ERROR)
 break; //ParseAction递归调用ACTIONS的下级 node 解析.
 else
 continue;//SWITCH_CASE_NEXT;继续测试下一个CASE node .
 }else{
 //提示差错xml.
 }
 }
 
	this->ParseDeep--; //用于记录解析层次深度.tool = tool.next_sibling()
	if (r==ACT_ERROR)
 return ACT_ERROR;
	else
 return ACT_SUCCESS;
 }*/

int ParseProcess::SWITCH(ParseXml_Node parse_node, PV_AddParas_T &add_paras) {
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    int r=ACT_ERROR;
    
    const char *type = this->_ActParseXml->NodeAttr(parse_node, "Type");
    if (type != NULL && strstr(type, "Deep")) {
        for (ParseXml_Node tool = this->_ActParseXml->FirstChild(parse_node); tool; tool = this->_ActParseXml->NextChild(tool))
        {
            if (strcmp(this->_ActParseXml->NodeName(tool), "CASE") == 0) {
                r = this->SWITCH_CASE(tool, false, add_paras);
                if (r == SWITCH_CASE_BREAK || r == ACT_ERROR)
                    break; //ParseAction递归调用ACTIONS的下级 node 解析.
                else
                    continue;//SWITCH_CASE_NEXT;继续测试下一个CASE node .
            }
            else if (strcmp(this->_ActParseXml->NodeName(tool), "DEFAULT") == 0) {
                this->DEBUGY(tool);
                int r = this->SWITCH_CASE(tool, true, add_paras);
                if (r == SWITCH_CASE_BREAK || r == ACT_ERROR)
                    break; //ParseAction递归调用ACTIONS的下级 node 解析.
                else
                    continue;//SWITCH_CASE_NEXT;继续测试下一个CASE node .
            }
            else {
                //提示差错xml.
            }
        }
    }
    else {
        for (ParseXml_Node tool = this->_ActParseXml->FirstChild(parse_node); tool; tool = this->_ActParseXml->NextChild(tool))
        {
            if (strcmp(this->_ActParseXml->NodeName(tool), "CASE") == 0) {
                r = this->SWITCH_CASE_SIMPLE(tool, false, add_paras);
                if (r == SWITCH_CASE_BREAK || r == ACT_ERROR)
                    break; //ParseAction递归调用ACTIONS的下级 node 解析.
                else
                    continue;//SWITCH_CASE_NEXT;继续测试下一个CASE node .
            }
            else if (strcmp(this->_ActParseXml->NodeName(tool), "DEFAULT") == 0) {
                this->DEBUGY(tool);
                this->ParseAction(tool, add_paras);//递归运行CASE指定的各类操作.
                break;
            }
            else {
                //提示差错xml.
                const char *test=this->_ActParseXml->NodeName(tool);
                this->SHOW_ERROR((string)"UNKown node name("+test+") in 'SWITCH'! ");
            }
        }
    }
    
    
    this->ParseDeep--; //用于记录解析层次深度.tool = tool.next_sibling()
    if (r == ACT_ERROR)
        return ACT_ERROR;
    else
        return ACT_SUCCESS;
}

int ParseProcess::SWITCH_CASE_SIMPLE(ParseXml_Node parse_node, bool is_default, PV_AddParas_T &add_paras) {
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    
    string err_str("");
    int rt = ParseProcess::SWITCH_CASE_NEXT;
    
    //取得查询条件
    const char *act_test = this->_ActParseXml->NodeAttr(parse_node, "Test");
    string act_test_str("");
    if (act_test == NULL || strlen(act_test) == 0) {
        rt = ACT_ERROR;
        this->SHOW_ERROR("CASE's Test Attribute should not be Null!");
        goto out;
    }
    else
        act_test_str = act_test;
    
    //判断是否进入,TestConditions判断
    if (this->TestConditionStr(act_test_str, err_str, add_paras)) {
        this->DEBUGY(parse_node);
        int r_tmp = this->ParseAction(parse_node, add_paras);//递归运行CASE指定的各类操作.
        if (r_tmp == ACT_BREAK)
            rt = SWITCH_CASE_BREAK;
        else if (r_tmp == ACT_SUCCESS)
            rt = SWITCH_CASE_NEXT;
        else
            rt = ACT_ERROR;
    }
    else {
        if (err_str.size() == 0)
            rt = SWITCH_CASE_NEXT;
        else
            rt = ACT_ERROR;//如果测试出现了语法错误
    }
out:
    this->ParseDeep--; //用于记录解析层次深度.tool = tool.next_sibling()
    return rt;
}

int ParseProcess::SWITCH_CASE(ParseXml_Node parse_node,bool is_default,PV_AddParas_T &add_paras){
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    
    int r, rt=ParseProcess::SWITCH_CASE_NEXT;
    
    for (ParseXml_Node tool = this->_ActParseXml->FirstChild(parse_node); tool; tool = this->_ActParseXml->NextChild(tool))
    {
        if (strcmp(this->_ActParseXml->NodeName(tool),"TEST")==0 || is_default==true){
            r= this->SWITCH_CASE_TEST(tool,add_paras);
            if (r==SWITCH_CASE_TEST_FALSE){
                rt =SWITCH_CASE_NEXT; //如果 Conditions判断为错误,则不作本CASE后续操作.
                break;
            }else if (r==ACT_ERROR){
                rt = ACT_ERROR;
                break;
            }else
                continue;
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"ACTIONS")==0){
            this->DEBUGY(tool);
            this->ParseAction(tool,add_paras);//递归运行CASE指定的各类操作.
            continue;
        }else if (strcmp(this->_ActParseXml->NodeName(tool),"BREAK")==0){
            this->DEBUGY(tool);
            rt = SWITCH_CASE_BREAK;//如果BREAK则告诉上层跳出SWITCH循环.
            break;
        }else{
            rt = ACT_ERROR;//提示差错xml.
            break;
        }
    }
    
    this->ParseDeep--; //用于记录解析层次深度.tool = tool.next_sibling()
    return rt;
}

int ParseProcess::SWITCH_CASE_TEST(ParseXml_Node parse_node,PV_AddParas_T &add_paras){
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    int rt=SWITCH_CASE_TEST_TRUE;
    
    for (ParseXml_Node tool = this->_ActParseXml->FirstChild(parse_node); tool; tool = this->_ActParseXml->NextChild(tool))
    {
        if (strcmp(this->_ActParseXml->NodeName(tool),"QueryCondition")==0){
            int r=this->SWITCH_CASE_TEST_CONDITION(tool,add_paras);
            if (r==SWITCH_CASE_TEST_CONDITION_FALSE){
                rt = SWITCH_CASE_TEST_FALSE; //如果 Conditions判断为错误,则不作本CASE后续操作.
                break;
            }else if (r==ACT_ERROR){
                rt = ACT_ERROR;
                break;
            }
        }else{
            //提示差错xml.
        }
    }
    
    this->ParseDeep--; //用于记录解析层次深度.tool = tool.next_sibling()
    return rt; //前面综合前面不是错误是为错误.
}

//该 node 集为 or 关系
/*int ParseProcess::SWITCH_CASE_TEST_CONDITION(ParseXml_Node parse_node,PV_AddParas_T &add_paras){
	this->DEBUGY(parse_node);
	this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
	int rt=SWITCH_CASE_TEST_CONDITION_FALSE;
 
	string left,right,condition,temp;
	const char *left_r;
 
	for (ParseXml_Node tool = this->_ActParseXml->FirstChild(parse_node); tool; tool = this->_ActParseXml->NextChild(tool))
 {
 //left="";
 //right="";
 this->DEBUGY(tool);
 //如果是TEST_ACTION
 if (strcmp(this->_ActParseXml->NodeName(tool),"TEST_ACTION")==0){
 this->ParseValue(tool,temp,add_paras);
 string test_value=this->_ActParseXml->NodeAttr(tool,"TestValue");
 if (temp.size()==0)
 temp="NULL";
 if (test_value.find(temp)!=string::npos)
 rt=SWITCH_CASE_TEST_CONDITION_TRUE;
 else
 rt=SWITCH_CASE_TEST_CONDITION_FALSE;
 }else{
 //如果是普通操作符
 //取得比较操作符.
 condition=this->_ActParseXml->NodeAttr(tool,"Operate");
 if (condition.find("{")!=string::npos){//可能是运算操作
 this->ParseValue(condition,temp,add_paras);
 condition=temp;
 }
 //取得左键值
 left_r = this->_PubObj->GetNodeValue(this->_ActParseXml->NodeName(tool));
 if (left_r==NULL){
 this->DEBUGY("",this->_PubObj->ErrorStr);
 rt=ACT_ERROR;
 break;
 }
 left=left_r;
 //取得右值
 this->ParseValue((string)this->_ActParseXml->NodeValue(tool),right,add_paras);
 
 //各类Conditions判断操作
 if (condition=="=="){
 if (left==right){
 rt=SWITCH_CASE_TEST_CONDITION_TRUE;
 break;
 }
 }else if (condition=="="|| condition=="include"){
 if (left.find(right)!=string::npos){
 rt = SWITCH_CASE_TEST_CONDITION_TRUE;
 break;
 }
 }else if (condition=="=!"|| condition=="!="){
 if (left!=right){
 rt = SWITCH_CASE_TEST_CONDITION_TRUE;
 break;
 }
 }else if (condition==">"|| condition=="!<"){//尚未提供此功能,在此处添加
 double d_left=this->_PubPacket->_StrTof(left);
 double d_right=this->_PubPacket->_StrTof(right);
 if (d_left >  d_right){
 rt = SWITCH_CASE_TEST_CONDITION_TRUE;
 break;
 }
 }else if (condition=="<" || condition=="!>"){//尚未提供此功能,在此处添加
 double d_left=this->_PubPacket->_StrTof(left);
 double d_right=this->_PubPacket->_StrTof(right);
 if (d_left <d_right){
 rt = SWITCH_CASE_TEST_CONDITION_TRUE;
 break;
 }
 }else{
 rt=ACT_ERROR;
 this->SHOW_ERROR("QueryCondition syntax错误!");
 }
 }
	
 }
	this->ParseDeep--; //用于记录解析层次深度.tool = tool.next_sibling()
 
	return rt; //前面综合前面不是错误是为错误.
 }*/

int ParseProcess::SWITCH_CASE_TEST_CONDITION(ParseXml_Node parse_node,PV_AddParas_T &add_paras){
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    //int rt=SWITCH_CASE_TEST_CONDITION_FALSE;
    int rt=SWITCH_CASE_TEST_CONDITION_FALSE;
    
    string left,right,condition,temp;
    const char *left_r;
    
    for (ParseXml_Node tool = this->_ActParseXml->FirstChild(parse_node); tool; tool = this->_ActParseXml->NextChild(tool))
    {
        //left="";
        //right="";
        this->DEBUGY(tool);
        //如果是TEST_ACTION
        if (strcmp(this->_ActParseXml->NodeName(tool),"TEST_ACTION")==0){
            this->ParseValue(tool,temp,add_paras);
            string test_value=this->_ActParseXml->NodeAttr(tool,"TestValue");
            if (temp.size()==0)
                temp="NULL";
            if (test_value.find(temp)!=string::npos)
                rt=SWITCH_CASE_TEST_CONDITION_TRUE;
            else
                rt=SWITCH_CASE_TEST_CONDITION_FALSE;
        }else{
            //如果是普通操作符
            //取得比较操作符.
            condition=this->_ActParseXml->NodeAttr(tool,"Operate");
            if (condition.find("{")!=string::npos){//可能是运算操作
                this->ParseValue(condition,temp,add_paras);
                condition=temp;
            }
            //取得左键值
            if (strcmp(this->_ActParseXml->NodeName(tool),"TEST_NODE")==0){//可以用TEST_NODE代替NodeName
                string node_attr = this->_ActParseXml->NodeAttr(tool,"NodeName");
                this->ParseValue(node_attr,temp,add_paras);
                if (temp.size()==0)
                    left_r = NULL;
                else
                    left_r = this->_PubObj->GetNodeValue(temp.c_str());
            }else
                left_r = this->_PubObj->GetNodeValue(this->_ActParseXml->NodeName(tool));
            
            if (left_r==NULL){
                this->DEBUGY("",this->_PubObj->ErrorStr);
                rt=ACT_ERROR;
                break;
            }
            left=left_r;
            //取得右值
            this->ParseValue((string)this->_ActParseXml->NodeValue(tool),right,add_paras);
            
            rt = this->TestCondition(condition,left,right);
            if (rt==ACT_ERROR)
                this->SHOW_ERROR("QueryCondition syntax error! ");
            else if (rt == SWITCH_CASE_TEST_CONDITION_TRUE)
                break;
        }
        
    }
    this->ParseDeep--; //用于记录解析层次深度.tool = tool.next_sibling()
    
    return rt; //前面综合前面不是错误是为错误.
}

//功能:针对ActParseXml 中SET_OBJ节动作；主要是吧 Source 于node.value的 String ,进行解析后, Config 入to_obj Object  node 中.
//Param:parse_node_xpath:当前ActParseXml所处配置 node ;to_obj为 Config 的目标 node ;parse_type解析方式.
//动作:(1)CASE STR2OBJ:把 String 解析后写入pub_obj（先）
//		(2)CASE XML2OBJ:吧xml node 解析后对应生成pub_obj的 node 写入pub_obj
//		(3)CASE OBJ2XML:吧obj node 解析后对应生成xml String .
//返回:ACT_SUCCESS成功;否则失败.
int ParseProcess::SET_OBJ(ParseXml_Node parse_node,PV_AddParas_T &add_paras){
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    int r=ParseProcess::ACT_SUCCESS;
    
    const char *to_obj=this->_ActParseXml->NodeAttr(parse_node,"To");
    string to_obj_buf;
    if (strstr(to_obj,"{")!=NULL){//增加对To node 的解析
        this->ParseValue((string)to_obj,to_obj_buf,add_paras);
        to_obj=to_obj_buf.c_str();
    }
    //    const char *default_value=this->_ActParseXml->NodeAttr(parse_node,"Default");
    //    string default_value_str= default_value==NULL ? "" : default_value ;
    //    if (STRING_Helper::PUB_FindStr(default_value_str, "{")==0){//增加对To node 的解析
    //        this->ParseValue((string)default_value,default_value_str,add_paras);
    //    }
    const char *parse_type=	this->_ActParseXml->NodeAttr(parse_node,"ParseType");
    
    if (strlen(parse_type)==0 || strcmp(parse_type,"STR_COPY_OBJ")==0 ){
        // String COPY入obj node .
        const char *is_append = this->_ActParseXml->NodeAttr(parse_node,"IsAppend");
        
        //获取 Source  String
        string parse_buff;
        const char *trans = this->_ActParseXml->NodeAttr(parse_node, "Trans");
        if (trans != 0 && strstr(trans, "BIN_TO_HEX") != NULL) {//增加对Trans 的解析
            string parse_buff_tmp;
            this->ParseValue(parse_node, parse_buff_tmp, add_paras);
            parse_buff = STRING_Helper::HBCStrToHBCStr(parse_buff_tmp, "char", "hex",false);
        }else
            this->ParseValue(parse_node,parse_buff,add_paras);//正常
        
        if (strstr(is_append,"true")==is_append){
            //对于累加操作
            const char *append_split = this->_ActParseXml->NodeAttr(parse_node,"AppendSplit");
            const char *old_value = this->_PubObj->GetNodeValue(to_obj);
            if (old_value!=NULL){
                if (strlen(append_split)!=0)
                    parse_buff.insert(0,append_split);
                parse_buff.insert(0,old_value);
            }
        }
        //写入操作
        string src_type= this->_ActParseXml->NodeAttr(parse_node,"SrcType");
        if (src_type.size()!=0 && src_type.find("BIN")!=string::npos)
            r=this->_PubObj->SetNodeValue(to_obj,parse_buff.data(),Http_ParseObj::STR_COPY_OBJ,parse_buff.size());
        else
            r=this->_PubObj->SetNodeValue(to_obj,parse_buff.c_str(),Http_ParseObj::STR_COPY_OBJ);//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
        // Config DEBUGY
        const char* is_debug=this->_ActParseXml->NodeAttr(parse_node,"Debug");
        if (strstr(is_debug,"SetTo")!=0)
            this->DEBUGY(parse_buff);
        
        //判断结果是否正确
        if (r==Http_ParseObj::SET_VALUE_ERROR)
            this->SHOW_ERROR(this->_PubObj->ErrorStr);
        
    }else if (strcmp(parse_type,"DEL_OBJ")==0){
        
        //写入操作
        r=this->_PubObj->DeleteObjNode(to_obj);//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
        
        //半段结果是否正确
        if (r==Http_ParseObj::SET_VALUE_ERROR)
            this->SHOW_ERROR(this->_PubObj->ErrorStr);
        
    }else if (strcmp(parse_type,"STR_POINT_OBJ")==0){
        // String 指针存入obj node .
        /*string parse_buff;
         
         //获取 Source  String
         this->ParseValue(parse_node,parse_buff,add_paras);
         this->SetObjNode(jpath,temp_xml,Http_ParseObj::STR_POINT_OBJ);*/
        this->SHOW_ERROR((string)"不可识别的动作ParseType attribute :"+parse_type+"!");
        
    }else if (strcmp(parse_type,"XML_PARSE_OBJ")==0){
        // String 变化成XML后写入OBJ node .
        string parse_buff;
        this->ParseValue(parse_node,parse_buff,add_paras);
        
        Http_ParseXML * temp_xml=this->_PubObj->XmlObjCreate(to_obj,parse_buff.c_str());//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
        if (temp_xml==NULL)
            this->SHOW_ERROR(this->_PubObj->ErrorStr);
        
    }
    else if (strcmp(parse_type, "JSON_PARSE_OBJ") == 0) {
        // String 变化成XML后写入OBJ node .
        string parse_buff;
        this->ParseValue(parse_node, parse_buff, add_paras);
        
        Http_ParseXML * temp_xml = this->_PubObj->XmlObjCreate(to_obj, parse_buff.c_str());//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
        if (temp_xml == NULL)
            this->SHOW_ERROR(this->_PubObj->ErrorStr);
        
    }
    else if (strcmp(parse_type,"XML_PARSE_NODE")==0){
        // String 变化成XML后写入OBJ node .
        string parse_buff;
        this->ParseValue(parse_node,parse_buff,add_paras);
        
        if (!this->_PubObj->XmlNodeLoadText(to_obj,parse_buff.c_str()))//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
            this->SHOW_ERROR(this->_PubObj->ErrorStr);
        
    }else if (strcmp(parse_type,"SET_XML_NODE")==0){
        // Config xml文档 node 值
        string parse_buff;
        this->ParseValue(parse_node,parse_buff,add_paras);
        if (parse_buff[0]=='<'){
            //新的复合 node  Config 逻辑
            if (!this->_PubObj->XmlNodeLoadText(to_obj,parse_buff.c_str()))//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
                this->SHOW_ERROR(this->_PubObj->ErrorStr);
        }else{
            //旧的单 node  Config 逻辑
            int r =this->_PubObj->SetNodeValue(to_obj,parse_buff.c_str(),Http_ParseObj::SET_XML_NODE);
            if (r==Http_ParseObj::XML_SET_ERROR)
                this->SHOW_ERROR(this->_PubObj->ErrorStr);
        }
        
    }else if (strcmp(parse_type,"PACKET_PARSE_XML")==0){
        //取得 Source  node 内容
        string parse_buff;
        this->ParseValue(parse_node,parse_buff,add_paras);
        
        //取得Xpath of Node
        string parse_path,parse_path_temp = this->_ActParseXml->NodeAttr(parse_node,"ParsePath");//没有 attribute 为空的处理.
        STRING_Helper::strTrim(parse_path_temp);
        if (parse_path_temp.compare(0,1,"{")==0)
            this->ParseValue(parse_path_temp,parse_path,add_paras);
        else
            parse_path=parse_path_temp;
        
        //创建目标xml根 node ；
        string parse_xml="<Root></Root>";//临时创建一个今后还可以变化.
        Http_ParseXML * temp_xml=this->_PubObj->XmlObjCreate(to_obj,parse_xml.c_str());//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
        if (temp_xml==NULL)
            this->SHOW_ERROR(this->_PubObj->ErrorStr);
        ParseXml_Node xml_node =temp_xml->FirstChild(temp_xml->ObjXml);
        
        // Config DEBUGY
        const char* is_debug=this->_ActParseXml->NodeAttr(parse_node,"Debug");
        if (strstr(is_debug,"SetTo")!=0)
            this->_PubPacket->SetDebug();
        
        //在根 node 写入相关的字段
        if (!this->_PubPacket->GetFieldsFormOriginStr(parse_path.c_str(),parse_buff, xml_node,(string)to_obj+":"+"/Root")){
            // Object Param
            this->SHOW_ERROR(this->_PubPacket->ErrorStr);
        };
        //关闭debug
        if (strstr(is_debug,"SetTo")!=0)
            this->_PubPacket->CloseDebug();
        
    }else if (strcmp(parse_type,"PACKET_PARSE_XML")==0){
        //取得 Source  node 内容
        string parse_buff;
        this->ParseValue(parse_node,parse_buff,add_paras);
        
        //取得Xpath of Node
        string parse_path,parse_path_temp = this->_ActParseXml->NodeAttr(parse_node,"ParsePath");//没有 attribute 为空的处理.
        STRING_Helper::strTrim(parse_path_temp);
        if (parse_path_temp.compare(0,1,"{")==0)
            this->ParseValue(parse_path_temp,parse_path,add_paras);
        else
            parse_path=parse_path_temp;
        
        //创建目标xml根 node ；
        string parse_xml="<Root></Root>";//临时创建一个今后还可以变化.
        Http_ParseXML * temp_xml=this->_PubObj->XmlObjCreate(to_obj,parse_xml.c_str());//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
        if (temp_xml==NULL)
            this->SHOW_ERROR(this->_PubObj->ErrorStr);
        ParseXml_Node xml_node =temp_xml->FirstChild(temp_xml->ObjXml);
        
        // Config DEBUGY
        const char* is_debug=this->_ActParseXml->NodeAttr(parse_node,"Debug");
        if (strstr(is_debug,"SetTo")!=0)
            this->_PubPacket->SetDebug();
        
        //在根 node 写入相关的字段
        if (!this->_PubPacket->GetFieldsFormOriginStr(parse_path.c_str(),parse_buff, xml_node,(string)to_obj+":"+"/Root")){
            // Object Param
            this->SHOW_ERROR(this->_PubPacket->ErrorStr);
        };
        //关闭debug
        if (strstr(is_debug,"SetTo")!=0)
            this->_PubPacket->CloseDebug();
        
    }else{
        this->SHOW_ERROR((string)"不可识别的动作ParseType attribute :"+parse_type+"!");//差错提示
    }
    
    this->ParseDeep--; //用于记录解析层次深度.tool = tool.next_sibling()
    return r;
}

int ParseProcess::PRINT(ParseXml_Node parse_node,PV_AddParas_T &add_paras){
    //debug控制
    const char *debug = this->_ActParseXml->NodeAttr(parse_node, "Debug");
    if (this->DebugType == ParseProcess::ACT_SUCCESS && strstr(debug, "force") != debug)
        return 0;
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    string parse_buff;
    
    //获取 Source  String
    string r_str = this->ParseValue(parse_node,parse_buff,add_paras);
    
    //Get Trans

    string trans= this->_ActParseXml->NodeAttr(parse_node,"Trans");
    const char *show_son_obj = this->_ActParseXml->NodeAttr(parse_node, "ShowSonObj");
    string filter = this->_ActParseXml->NodeAttr(parse_node, "Filter");
    string rem = this->_ActParseXml->NodeAttr(parse_node, "Rem");


    if (trans.size()!=0 && trans.find("BIN_TO_HEX")!=string::npos){
        if (this->DebugType == ParseProcess::DEBUG_NO &&  strstr(debug, "force") == debug)
            cout << (rem.size() == 0 ? "" : rem + ": ") << this->_PubPacket->HBCStrToHBCStr(parse_buff, "char", "hex", true) << endl;
        else
            this->DEBUGY(this->_PubPacket->HBCStrToHBCStr(parse_buff, "char", "hex", true));
    }
    else if (show_son_obj!=0 && strlen(show_son_obj)!=0) {
        if (this->_PubObj->Find(show_son_obj) != Http_ParseObj::ERROR_POINT_NULL) {
            ParseObj_T::iterator temp1 = this->_PubObj->Point;
            for (;;) {
                //下一节点
                this->_PubObj->Point++;
                //如果结尾，则完成
                if (this->_PubObj->Point == this->_PubObj->ObjMap->end())
                    break;
                //如果名称不包含，则完成
                if (this->_PubObj->Point->first.find(temp1->first) != 0)
                    break;
                ParseObj_T::iterator temp2 = this->_PubObj->Point;
                //累加结果节点
                string find_tmp = temp1->first + "." + filter;
                if (!(filter.size() != 0 && temp2->first.find(find_tmp) == 0)) {
                    if (this->DebugType == ParseProcess::DEBUG_NO && strstr(debug,"force")==debug)
                        cout << (rem.size()==0 ? "" : rem+": " ) << temp2->first + ":" + this->_PubObj->GetNodeValue(temp2->first.c_str()) << endl;
                    else
                        this->DEBUGY(temp2->first+":"+ this->_PubObj->GetNodeValue(temp2->first.c_str()));
                }
            }
        }
        
    }
    else {
        if (this->DebugType == ParseProcess::DEBUG_NO &&  strstr(debug, "force") == debug)
            cout << (rem.size() == 0 ? "" : rem + ": ") <<  parse_buff << endl;
        else
            this->DEBUGY(parse_buff);
    }
    
    
    //半段结果是否正确
    if (r_str.size()!=0)
        this->SHOW_ERROR(r_str);
    
    this->ParseDeep--; //用于记录解析层次深度.tool = tool.next_sibling()
    return ParseProcess::ACT_SUCCESS;
}


int ParseProcess::DEL_OBJ(ParseXml_Node parse_node,PV_AddParas_T &add_paras){
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    int r=ParseProcess::ACT_SUCCESS;
    
    const char *to_obj=this->_ActParseXml->NodeAttr(parse_node,"To");
    string to_obj_buf;
    if (strstr(to_obj,"{")!=NULL){//增加对To node 的解析
        this->ParseValue((string)to_obj,to_obj_buf,add_paras);
        to_obj=to_obj_buf.c_str();
    }
    const char *parse_type=	this->_ActParseXml->NodeAttr(parse_node,"ParseType");
    
    if (strlen(parse_type)==0){//删除当前 node
        //删除操作
        if (this->_PubObj->Find(to_obj)==Http_ParseObj::SUCCESS){
            r=this->_PubObj->DeleteObjNode(to_obj);//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
            //判断结果是否正确
            if (r==Http_ParseObj::SET_VALUE_ERROR){
                this->SHOW_ERROR(this->_PubObj->ErrorStr);
            }
        }/*else{
          this->SHOW_ERROR("未发现待删除的 Object !");
          }*/
        
    }else if (strcmp(parse_type,"OBJ_AND_SON")==0){//删除当前 node 和 sub node
        //删除操作
        int set_type;
        if (this->_PubObj->GetObjNode(to_obj,set_type)!=NULL){
            if (!(set_type==Http_ParseObj::STR_COPY_OBJ && set_type==Http_ParseObj::STR_POINT_OBJ)){
                this->SHOW_ERROR((string)"删除子 Object 暂时不支持XML类 node :"+STRING_Helper::iToStr(set_type));
                cout << "set_type:" << set_type << endl;
            }else
                this->_PubObj->Delete(to_obj,true);
        }
    }else{
        this->SHOW_ERROR((string)"不可识别的动作ParseType attribute :"+parse_type+"!");//差错提示
    }
    
    this->ParseDeep--; //用于记录解析层次深度.tool = tool.next_sibling()
    return r;
}


#ifdef SENSOR_DBM

//功能:针对ActParseXml 中SET_OBJ节动作；主要是吧 Source 于node.value的 String ,进行解析后, Config 入to_obj Object  node 中.
//Param:parse_node_xpath:当前ActParseXml所处配置 node ;to_obj为 Config 的目标 node ;parse_type解析方式.
//动作:(1)CASE STR2OBJ:把 String 解析后写入pub_obj（先）
//		(2)CASE XML2OBJ:吧xml node 解析后对应生成pub_obj的 node 写入pub_obj
//		(3)CASE OBJ2XML:吧obj node 解析后对应生成xml String .
//返回:ACT_SUCCESS成功;否则失败.
int ParseProcess::DB_WRITE_BKDB(ParseXml_Node parse_node,PV_AddParas_T &add_paras){
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    int r=ParseProcess::ACT_SUCCESS;
    string err_str("");
    
    const char *to_obj=this->_ActParseXml->NodeAttr(parse_node,"To");
    string to_obj_buf;
    if (strstr(to_obj,"{")!=NULL){//增加对To node 的解析
        err_str=this->ParseValue((string)to_obj,to_obj_buf,add_paras);
        to_obj=to_obj_buf.c_str();
    }
    string by_cursor=this->_ActParseXml->NodeAttr(parse_node,"ByCursor");
    
    const char* replace_off=this->_ActParseXml->NodeAttr(parse_node,"ReplaceOff");
    const char* replace_len=this->_ActParseXml->NodeAttr(parse_node,"ReplaceLen");
    u_int32_t rep_off=-1;
    u_int32_t rep_len=0;
    
    
    if (strlen(replace_off)!=0){
        if (strstr(replace_off,"FromEnd")!=NULL)
            rep_off=-2;
        else if ((rep_off=STRING_Helper::ParseInt(replace_off))==ParseProcess::Http_ParsePacket::NaN){
            err_str="ReplaceOff  attribute 错误!";
        }
    }
    if (strlen(replace_len)!=0 && (rep_len=STRING_Helper::ParseInt(replace_len))==ParseProcess::Http_ParsePacket::NaN)
        err_str="ReplaceOff  attribute 错误!";
    
    const char *key_obj=this->_ActParseXml->NodeAttr(parse_node,"Key");
    string key_obj_buf;
    if (strstr(key_obj,"{")!=NULL){//增加对To node 的解析
        err_str=this->ParseValue((string)key_obj,key_obj_buf,add_paras);
        key_obj=key_obj_buf.c_str();
    }
    
    const char *method_str=this->_ActParseXml->NodeAttr(parse_node,"Method");//对于WriteRecordByCursor,尚未启用
    
    //获取content String
    string parse_buff;
    err_str = this->ParseValue(parse_node,parse_buff,add_paras);
    
    //写入操作
    if (err_str.size()==0){
        if (by_cursor.size()>0 && by_cursor=="true"){
            u_int32_t method;
            if (strlen(method_str)!=0 && ((method=HTTP_Config::SensorDB->GetMethod(SensorDBManager::CURSOR_WRITE,method_str))!= SensorDBManager::DB_METHOD_ERROR)){
                cout << "writerecord" << endl;
                r=HTTP_Config::SensorDB->WriteRecordByCursor(key_obj_buf,(void *)parse_buff.data(), (u_int32_t)parse_buff.size(),string(to_obj),method,rep_off,rep_len);//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
                cout << "success" << endl;
            }else
                err_str="'Method' attribute set error,should be 'DB_CURRENT' etc ";
        }else{
            cout << "writerecord" << endl;
            r=HTTP_Config::SensorDB->WriteRecord(key_obj_buf,(void *)parse_buff.data(), (u_int32_t)parse_buff.size(),string(to_obj),0,rep_off,rep_len);//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
            cout << "success" << endl;
            
        }
    }
    //HTTP_Config::SensorDB->Commit(string(to_obj));
    
    
    // Config DEBUGY
    const char* is_debug=this->_ActParseXml->NodeAttr(parse_node,"Debug");
    if (strstr(is_debug,"SetTo")!=0)
        this->DEBUGY((string)+"key: "+key_obj_buf+" Content:"+parse_buff);
    
    //半段结果是否正确
    if (r!=BA_SUCCESS){
        err_str=(string)"写入数据库错误!DB:"+to_obj+"; key:"+key_obj_buf;
    }
    if (err_str.size()!=0){
        this->SHOW_ERROR(err_str);
        string cancle_str("@_DB_ACT_CANCLE_");
        if (this->_PubObj->Find(cancle_str.c_str()) ==Http_ParseObj::SUCCESS)
            this->_PubObj->SetNodeValue("@_DB_ACT_CANCLE_","true",Http_ParseObj::STR_COPY_OBJ);//这种情况是数据库差错情况,可能需要上层函数进行txn->abort等操作.
    }
    
    this->ParseDeep--; //用于记录解析层次深度.tool = tool.next_sibling()
    return r;
}


int ParseProcess::DB_SEARCH(ParseXml_Node parse_node,PV_AddParas_T &add_paras){
    //跟踪开头
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    string err_str("");
    string result_append("");//可能的累加结果集；
    int r,i,j;
    
    ParseXml_Node actions_node=this->_ActParseXml->Child(parse_node,"ACTIONS");;
    if (!actions_node)
        err_str="'ACTIONS' node  not exist or definition error!";
    string act_test=this->_ActParseXml->NodeAttr(actions_node,"Test");
    
    //对IN的内容进行解析.
    //取得查询Conditions,允许  not exist,可直接取得空值.
    //ParseXml_Node key_node=this->_ActParseXml->Child(parse_node,"Key");
    //ParseXml_Node index_node=this->_ActParseXml->Child(parse_node,"Index");
    //ParseXml_Node query_node=this->_ActParseXml->Child(parse_node,"Query");
    //const char *key_str = this->_ActParseXml->NodeValue(key_node);
    //const char *index_str = this->_ActParseXml->NodeValue(index_node);
    //const char *query_str = this->_ActParseXml->NodeValue(query_node);
    
    
    //取得最大记录数 attribute
    const char *num_str = this->_ActParseXml->NodeAttr(parse_node,"MaxNum");
    string num; this->ParseValue(num_str,num,add_paras);num+='\0';//有可能0补充
    int count_in_page = this->_PubPacket->_StrToi(num); //cout << to << to.size() <<endl;
    //取得第一条记录 attribute
    const char *page_str = this->_ActParseXml->NodeAttr(parse_node,"Page");
    string page; this->ParseValue(page_str,page,add_paras);page+='\0';//有可能0补充
    int page_num=this->_PubPacket->_StrToi(page);
    
    const char *from_db_str = this->_ActParseXml->NodeAttr(parse_node,"FromDB");
    const char *is_parse_xml = this->_ActParseXml->NodeAttr(parse_node,"IsParseXml");
    
    //取得查询光标移动方法
    const char *readtype_str = this->_ActParseXml->NodeAttr(parse_node,"ReadMethod");
    u_int32_t readtype;
    if (strlen(readtype_str)!=0){
        readtype=HTTP_Config::SensorDB->GetMethod(SensorDBManager::CURSOR_READ,readtype_str);
        if(readtype==SensorDBManager::DB_METHOD_ERROR)
            err_str="ReadMethod  attribute set error, should be DB_NEXT、DB_FIRSTetc!";
    }else{
        readtype=HTTP_Config::SensorDB->GetMethod(SensorDBManager::CURSOR_READ,"DB_NEXT");
    }
    
    //取得查询光标移动方法
    const char *method_str = this->_ActParseXml->NodeAttr(parse_node,"CursorMethod");
    u_int32_t method=0;
    if (strlen(method_str)!=0){
        method=HTTP_Config::SensorDB->GetMethod(SensorDBManager::CURSOR_OPEN,method_str);
        if(method==SensorDBManager::DB_METHOD_ERROR)
            err_str="CursorMethod  attribute set error, should be DB_READ_UNCOMMITTED,DB_READ_COMMITTED,SNAPSHOTetc!";
    }
    
    
    //取得查询光标移动方法
    const char *txn_flag_str = this->_ActParseXml->NodeAttr(parse_node,"TXNMethod");
    u_int32_t txn_flag=0;
    if (strlen(txn_flag_str)!=0){
        txn_flag=HTTP_Config::SensorDB->GetMethod(SensorDBManager::TXN_OPEN,txn_flag_str);
        if(txn_flag==SensorDBManager::DB_METHOD_ERROR)
            err_str="TXNMethod  attribute set error, should be DB_READ_UNCOMMITTED,DB_READ_COMMITTED,SNAPSHOT etc!";
    }
    
    //返回页内数值和总数字段.注:页码已在请求中.
    const char* R_Total=this->_ActParseXml->NodeAttr(parse_node,"R_Total");
    const char* R_Count=this->_ActParseXml->NodeAttr(parse_node,"R_Count");
    const char* R_Buffer=this->_ActParseXml->NodeAttr(parse_node,"R_Buffer");
    const char* R_Keys=this->_ActParseXml->NodeAttr(parse_node,"R_Keys");
    const char* R_BufferMethod=this->_ActParseXml->NodeAttr(parse_node,"R_BufferMethod");
    
    //其他准备
    //解决多重FOR循环处理,如果多重为FOR_NUM_2 or FOR_VALUE_2
    string stack_num("");
    PV_AddParas_T::iterator it;
    for (it = add_paras.find((string)"{@_FOR_NUM_"+stack_num+"}"); it!=add_paras.end(); it = add_paras.find((string)"{@_FOR_NUM_"+stack_num+"}") ){
        if (stack_num=="")
            stack_num="1";
        else
            stack_num=this->_PubPacket->_iToStr((this->_PubPacket->_StrToi(stack_num)+1),10);
    }
    // Config DEBUGY
    const char* is_debug=this->_ActParseXml->NodeAttr(parse_node,"Debug");
    
    //循环操作处理
    if (err_str.size()==0){
        string dbname(from_db_str);
        unsigned long db_count=HTTP_Config::SensorDB->Count(dbname);//取得结果记录总数；
        //string key(from_id_str)
        //清空结果缓存
        if (strstr(R_BufferMethod,"Append")==R_BufferMethod)
            this->_PubObj->SetNodeValue(R_Buffer,"",Http_ParseObj::STR_COPY_OBJ);
        else
            this->_PubObj->Delete(R_Buffer,true);
        this->_PubObj->Delete(R_Keys,true);
        
        //循环执行宏代码
        if (HTTP_Config::SensorDB->TXNBegin(dbname,txn_flag)){
            string db_act_cancle(""),err_str("");
            if (HTTP_Config::SensorDB->CursorBegin(dbname,method,true)){
                
                //循环查询
                for (i=0,j=(page_num-1)*count_in_page; i <= count_in_page; i++,j++){
                    string s_i = this->_PubPacket->_iToStr(i+1,10);
                    //数据库查询操作.
                    void * pdata = NULL;	unsigned long dataLen;
                    void * pkey = NULL;		unsigned long keyLen;
                    
                    if (i==0){
                        r=HTTP_Config::SensorDB->GetRecordByCursor(&pkey,&keyLen,&pdata, &dataLen,dbname,HTTP_Config::SensorDB->GetMethod(SensorDBManager::CURSOR_READ,"DB_FIRST")); //如果key==空,则数据库
                    }else
                        r=HTTP_Config::SensorDB->GetRecordByCursor(&pkey,&keyLen,&pdata, &dataLen,dbname,readtype);	//如果key==空,则遍历数据库
                    
                    //判断结果是否正确
                    if (r!=BA_SUCCESS){
                        SAFE_FREE(pdata); SAFE_FREE(pkey); pkey=NULL; pdata = NULL;//释放空间
                        if (r!=BA_NOTFOUND)
                            err_str=string("读数据库错误!DB:")+dbname+"; id:";
                        break;
                    }
                    
                    //DEBUGY处理
                    if (strlen(is_debug)!=0 && strstr(is_debug,"PvOut")!=0){
                        string debug_key((char *)pkey,keyLen);
                        string debug_pdata((char *)pdata,dataLen);
                        this->DEBUGY((string)+"key: "+debug_key+" Content:"+debug_pdata);
                    }
                    
                    //传参,是否需要拷贝缓存数据.
                    if (strlen(R_Buffer)!=0){//缓存内容
                        if ((strlen(R_BufferMethod)!=0) && (strstr(R_BufferMethod,"Append")==R_BufferMethod) && dataLen!=0){//增补性加入.
                            string new_str((const char *)pdata,dataLen-1);
                            result_append.append(new_str);
                        }else{// Object 加入
                            string s_r_buffer(R_Buffer);
                            s_r_buffer+="."+s_i;
                            this->_PubObj->SetNodeValue(s_r_buffer.c_str(),(const char *)pdata,Http_ParseObj::STR_COPY_OBJ,(int)dataLen);  // Object 加入.
                        }
                    }
                    if (strlen(R_Keys)!=0){//缓存keys
                        string s_r_key(R_Keys);
                        s_r_key+=+"."+s_i;
                        this->_PubObj->SetNodeValue(s_r_key.c_str(),(const char *)pkey,Http_ParseObj::STR_COPY_OBJ,(int)keyLen);  // Object 行加入.
                    }
                    
                    //传参,内部部分尽量用指针.
                    this->_PubObj->SetNodeValue((string("@_FOR_KEY_"+stack_num)).c_str(),(const char *)pkey,Http_ParseObj::STR_POINT_OBJ,i);
                    this->_PubObj->SetNodeValue((string("@_FOR_DATA_"+stack_num)).c_str(),(const char *)pdata,Http_ParseObj::STR_POINT_OBJ);
                    this->_PubObj->SetNodeValue((string("@_FOR_DATA_LEN_"+stack_num)).c_str(),(this->_PubPacket->_iToStr((int)dataLen,10)).c_str(),Http_ParseObj::STR_COPY_OBJ);
                    this->_PubObj->SetNodeValue((string("@_DB_ACT_CANCLE_"+stack_num)).c_str(),"",Http_ParseObj::STR_COPY_OBJ);
                    if (strcmp(is_parse_xml,"true")==0)
                        this->_PubObj->SetNodeValue((string("@_FOR_DATA_XML_"+stack_num)).c_str(),(const char *)pdata,Http_ParseObj::XML_PARSE_OBJ);
                    add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string((string)"{@_FOR_NUM_"+stack_num+"}"),this->_PubPacket->_iToStr(i,10) ));
                    
                    if (act_test.size()==0 || this->TestConditionStr(act_test,err_str,add_paras)){//全部进入循环.？？？
                        //操作
                        this->DEBUGY(actions_node);
                        this->ParseDeep++;
                        this->ParseAction(actions_node,add_paras); //ACTIONS 操作；
                        this->ParseDeep--;
                    }
                    if (err_str.size()!=0)
                        break;
                    
                    //删除Param.
                    db_act_cancle =this->_PubObj->GetNodeValue((string("@_DB_ACT_CANCLE_")+stack_num).c_str());
                    this->_PubObj->DeleteObjNode(string("@_DB_ACT_CANCLE_")+stack_num);
                    this->_PubObj->DeleteObjNode(string("@_FOR_KEY_")+stack_num);
                    this->_PubObj->DeleteObjNode(string("@_FOR_DATA_")+stack_num);
                    this->_PubObj->DeleteObjNode(string("@_FOR_DATA_LEN_")+stack_num);
                    if (strcmp(is_parse_xml,"true")==0)
                        this->_PubObj->DeleteObjNode(string("@_FOR_DATA_XML_")+stack_num);
                    add_paras.erase("{@_FOR_NUM_"+stack_num+"}");
                    
                    //释放空间
                    SAFE_FREE(pdata); SAFE_FREE(pkey); pkey=NULL; pdata = NULL;//释放空间
                    //取得下一个FromID
                    if (db_act_cancle.size()!=0){
                        break;
                    }
                }
                HTTP_Config::SensorDB->CursorEnd(dbname); //关闭查询操作
            }
            if (db_act_cancle.size()==0)
                HTTP_Config::SensorDB->TXNEnd(dbname,SensorDBManager::TXN_COMMIT); //关闭查询操作
            else
                HTTP_Config::SensorDB->TXNEnd(dbname,SensorDBManager::TXN_ABORT);
        }
        
        if (strlen(R_Count)==0)
            this->_PubObj->SetNodeValue(R_Count,this->_PubPacket->_iToStr(i,10).c_str(),Http_ParseObj::STR_COPY_OBJ);//!!!今后应改为str指针,模式传入.
        if (strlen(R_Total)==0)
            this->_PubObj->SetNodeValue(R_Total,this->_PubPacket->_iToStr(db_count,10).c_str(),Http_ParseObj::STR_COPY_OBJ);//!!!今后应改为str指针,模式传入.
        if (result_append.size()!=0)
            this->_PubObj->SetNodeValue(R_Buffer,result_append.c_str(),Http_ParseObj::STR_COPY_OBJ);//累加类型结果集.
    }
    
    if(err_str!=""){
        this->SHOW_ERROR(err_str);
        this->ParseDeep--;
        return ParseProcess::ACT_ERROR;
    }else{
        this->ParseDeep--;
        return ParseProcess::ACT_SUCCESS;
    }
}
#endif

int ParseProcess::DB_WRITE(ParseXml_Node parse_node,PV_AddParas_T &add_paras){
    //跟踪开头
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    string err_str("");
    
    //取得Action
    string action;
    const char *action_obj=this->_ActParseXml->NodeAttr(parse_node,"Action");
    if (strlen(action_obj)==0)
        action="Save";
    else
        action=action_obj;
    //取得Key
    string key_obj_buf;
    const char *key_obj=this->_ActParseXml->NodeAttr(parse_node,"Key");
    if (strlen(key_obj)!=0){
        if (strstr(key_obj,"{")!=NULL){//增加对To node 的解析
            this->ParseValue((string)key_obj,key_obj_buf,add_paras);
            key_obj=key_obj_buf.c_str();
        }
    }else{
        err_str=string("DB_WRITE  node 必须指定Key attribute !");
    }
    
    //取得DefaultFields
    string default_fields_str;
    string filter_no_str("");
    const char *default_fields=this->_ActParseXml->NodeAttr(parse_node,"DefaultFields");
    const char *filter=this->_ActParseXml->NodeAttr(parse_node,"DefaultFieldsFilter");
    if (strlen(default_fields)!=0){
        if (strstr(default_fields,":")!=0){//判断是否为XML node  variable
            if (strstr(default_fields,"{")!=NULL){//增加对DefaultFields node 的解析
                this->ParseValue((string)default_fields,default_fields_str,add_paras);
                default_fields=default_fields_str.c_str();
            }
        }else{
            err_str=string("DefaultFields  attribute 目前只支持xml node!");
        }
        //测试xml node 是否存在.
        /*int i;
         Http_ParseXML *p_xml=(Http_ParseXML *)this->_PubObj->GetObjNode(obj_path.c_str(),i)
         if(!p_xml){
         err_str=string("DefaultFields  attribute :")+default_fields+"所指Xml路径  not exist!";
         }*/
        //取得过滤字段；
        
    }else{
        err_str=string("DB_WRITE  node 必须指定DefaultFields node .如果没有请用SET_OBJ创建xml node ");
    }
    
    //取得数据库定义 node
    ParseXml_Node parse_db_node,parse_db_write_act;
    const char *parse_db_name=this->_ActParseXml->NodeAttr(parse_node,"ParseDB");
    if (strlen(parse_db_name)==0)
        parse_db_name=this->_ActParseXml->NodeAttr(parse_node,"To");
    
    if (strlen(parse_db_name)!=0){
        //函数操作
        if (ParseXml_Node parse_db_node = this->_ActParseXml->GetNodeByXpath(((string)"/XMLParse/ParseDB/"+parse_db_name).c_str())){
            parse_db_write_act= this->_ActParseXml->GetNodeByXpath(((string)"/XMLParse/ParseDB/"+parse_db_name+"/WRITE_ACTION").c_str());
            if (!parse_db_write_act)
                err_str=string("DB_WRITE  node must define ``WRITE_ACTION action node !");
        }else{
            err_str=string("未发现ParaeDB 定义 node ='")+parse_db_name+"'的数据库定义!";
        }
    }else{
        err_str=string("DB_WRITE  node 必须指定ParseDB attribute !");
    }
    
    
    //调用函数操作
    if(err_str.size()==0){
        //取得Param;
        add_paras.insert(ParseProcess::PV_AddParas_T::value_type((string)"{@_PARA_Key}",key_obj));
        add_paras.insert(ParseProcess::PV_AddParas_T::value_type((string)"{@_PARA_DefaultFields}",default_fields));
        add_paras.insert(ParseProcess::PV_AddParas_T::value_type((string)"{@_PARA_DefaultFieldsFilter}",filter));
        add_paras.insert(ParseProcess::PV_AddParas_T::value_type((string)"{@_PARA_Action}",action));
        //取得AddFields补充 node ；
        ParseXml_Node tool = this->_ActParseXml->Child(parse_node,"AddFields");
        //添加XML补充字段 node .
        string field_value("");
        string field_name;
        /*filter_no_str+="|";
         //过滤操作；今后改进成|| range 的查询
         if (filter_no_str.size()!=0 && filter_no_str.find(field_name+"|")!=string::npos)
         continue;*/
        for (ParseXml_Node field = this->_ActParseXml->FirstChild(tool); field; field = this->_ActParseXml->NextChild(field)){
            field_name = this->_ActParseXml->NodeName(field);
            
            this->ParseValue(field,field_value,add_paras);
            this->_PubObj->SetNodeValue((string(default_fields)+"/"+field_name).c_str(),field_value.c_str(),Http_ParseObj::SET_XML_NODE);//这个需要跟踪测试
        }
        
        //函数调用操作
        this->_DB_WRITE_FUNCTION(parse_db_write_act,add_paras);   //ACTIONS 操作
        
        //删除Param
        add_paras.erase((string)"{@_PARA_Key}");
        add_paras.erase((string)"{@_PARA_DefaultFields}");
        add_paras.erase((string)"{@_PARA_DefaultFieldsFilter}");
        add_paras.erase((string)"{@_PARA_Action}");
    }
    
    if(err_str!=""){
        this->SHOW_ERROR(err_str);
        this->ParseDeep--;
        return ParseProcess::ACT_ERROR;
    }else{
        this->ParseDeep--;
        return ParseProcess::ACT_SUCCESS;
    }
}


//取得 Source 字段
int ParseProcess::_DB_WRITE_FUNCTION(ParseXml_Node parse_node, PV_AddParas_T &add_paras){
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    
    this->ParseAction(parse_node,add_paras); //ACTIONS 操作；
    
    this->ParseDeep--;
    return ParseProcess::ACT_SUCCESS;
}


int ParseProcess::FUNCTION(ParseXml_Node parse_node, PV_AddParas_T &add_paras){
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()

    this->ParseAction(parse_node,add_paras); //ACTIONS 操作；

    this->ParseDeep--;
    return ParseProcess::ACT_SUCCESS;
}

int ParseProcess::CALL(ParseXml_Node parse_node, PV_AddParas_T &add_paras){
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    string err_str("");
    
    //取得下级函数 node
    ParseXml_Node actions_node;
    const char *func_name=this->_ActParseXml->NodeAttr(parse_node,"Function");
    if (strlen(func_name)==0){
        func_name=this->_ActParseXml->NodeAttr(parse_node,"Func");
    }
    
    const char *act_path=this->_ActParseXml->NodeAttr(parse_node,"ActPath");
    string act_path_str;
    if (act_path && strlen(act_path)!=0){//ly table_sync
        err_str = this->ParseValue(act_path,act_path_str,add_paras);
        act_path=act_path_str.c_str();
    }
    //ly table_sync
    const char *act_cbid=this->_ActParseXml->NodeAttr(parse_node,"ActCBID");
    string act_cbid_str;
    if (err_str.size()==0){
        if (act_cbid && strlen(act_cbid)!=0){
            err_str = this->ParseValue((string)"//CALL[@CBID=\""+act_cbid+"\"]/THEN",act_cbid_str,add_paras);
            act_path=act_cbid_str.c_str();
        }
    }
    //获取 Source  String
    string parse_buff="";
    if (err_str.size()==0)
        err_str = this->ParseValue(parse_node,parse_buff,add_paras);
    
    if ((err_str.size()==0) && (strlen(func_name)!=0)){
        //函数操作
        string xpath=string("/XMLParse/FUNCTION/")+func_name;
        ParseXml_NodeSet nodes = this->_ActParseXml->GetNodeSetByXpath(xpath.c_str());
        if (nodes.size()==0)
            err_str=string("can't find definiation of FUNCTION Name='")+func_name+"!";
        else if (nodes.size()>1)
            err_str=string("FUNCTION Name='")+func_name+"'is multiple definition!";
        else{
            //取得真实函数 node
            ParseXml_NodeSet::const_iterator it = nodes.begin();
            pugi::xpath_node xnode = *it;
            actions_node = xnode.node();
        }
    }else if ((err_str.size()==0) && (strlen(act_path)!=0)){
        //函数操作
        string xpath=act_path;
        ParseXml_NodeSet nodes = this->_ActParseXml->GetNodeSetByXpath(xpath.c_str());
        if (nodes.size()==0)
            err_str=string("can't find definiation of ActPath='")+act_path+"!";
        else if (nodes.size()>1)
            err_str=string("ActPath='")+act_path+"'is multiple definition!";
        else{
            //取得真实函数 node
            ParseXml_NodeSet::const_iterator it = nodes.begin();
            pugi::xpath_node xnode = *it;
            actions_node = xnode.node();
        }
    }else if (err_str.size()==0){
        err_str=string("Call  node must defin Function's  attribute !");
    }
    
    //调用函数操作
    if(err_str.size()==0){
        //循环取得Param
        for (pugi::xml_attribute_iterator ait = parse_node.attributes_begin(); ait != parse_node.attributes_end(); ++ait)
        {
            if(strcmp(ait->name(),"FUNCTION")==0 || strcmp(ait->name(),"FUNC")==0 || strcmp(ait->name(),"ActPath")==0 || strcmp(ait->name(),"Test")==0)
                continue;
            if (strstr(ait->value(),"{")!=NULL){//如果可能需要parseValue(
                string r_str="",value_str="",src_str=ait->value();
                r_str=this->ParseValue(src_str,value_str,add_paras);
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type((string)"{@_"+func_name+"_"+ait->name()+"}",value_str));
            }else
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type((string)"{@_"+func_name+"_"+ait->name()+"}",ait->value()));
        }
        //pugi::xml_attribute_iterator tmp=parse_node.attributes_end();
        //添加内容
        if (func_name!=NULL && strlen(func_name)!=0){
        	this->_PubObj->SetNodeValue(((string)"@_"+func_name+"_CONTENT").c_str(),parse_buff.data(),Http_ParseObj::STR_COPY_OBJ,parse_buff.size());
        }
        //取得回调地址参数 //ly table_sync
        const char *cb_id=this->_ActParseXml->NodeAttr(parse_node,"CBID");
        string node_addr_hex= (cb_id!=NULL && strlen(cb_id)!=0) ? cb_id : "";
        if (cb_id==NULL || strlen(cb_id)==0){
            BB_SIZE_T node_addr=(BB_SIZE_T)parse_node.internal_object();
            string node_addr_str=STRING_Helper::putUINTToStr(node_addr,sizeof(node_addr)*8);
            node_addr_hex=STRING_Helper::HBCStrToHBCStr(node_addr_str,"char","hex",false);
            xml_attribute a=parse_node.append_attribute("CBID");
            a.set_value(node_addr_hex.c_str());
        }
        //this->DEBUGY(parse_node);
        add_paras.erase((string)"{@_"+func_name+"_CBID}");//???这里可能会删除上层CALL 对{@__CBID}的调用；今后采用随机数func_name解决！
        add_paras.insert(ParseProcess::PV_AddParas_T::value_type((string)"{@_"+func_name+"_CBID}",node_addr_hex));
//      add_paras.insert(ParseProcess::PV_AddParas_T::value_type((string)"{@_"+func_name+"_THIS_CALL_PATH}",(string)parse_node.path()));
        
//        string test=(string)"//CALL[@CBID=\""+node_addr_hex+"\"]";
//        ParseXml_Node n=this->_ActParseXml->GetNodeByXpath(test.c_str());

        
        this->FUNCTION(actions_node,add_paras);   //ACTIONS 操作
        if (func_name!=NULL && strlen(func_name)!=0)
            this->_PubObj->DeleteObjNode(((string)"@_"+func_name+"_CONTENT").c_str());
        //删除内容
        
        //循环删除Param
        for (pugi::xml_attribute_iterator ait = parse_node.attributes_begin(); ait != parse_node.attributes_end(); ++ait)
        {
            if(strcmp(ait->name(),"FUNCTION")==0 || strcmp(ait->name(),"FUNC")==0 || strcmp(ait->name(),"ActPath")==0 || strcmp(ait->name(),"Test")==0)
                continue;
            
            add_paras.erase(string("{@_")+func_name+"_"+ait->name()+"}");
        }
    }
    
    if(err_str!=""){
        this->SHOW_ERROR(err_str);
        this->ParseDeep--;
        return ParseProcess::ACT_ERROR;
    }else{
        this->ParseDeep--;
        return ParseProcess::ACT_SUCCESS;
    }
}
/*
int ParseProcess::CALL(ParseXml_Node parse_node, PV_AddParas_T &add_paras){
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    string err_str("");
    
    //取得下级函数 node
    ParseXml_Node actions_node;
    const char *func_name=this->_ActParseXml->NodeAttr(parse_node,"Function");
    if (strlen(func_name)==0){
        func_name=this->_ActParseXml->NodeAttr(parse_node,"Func");
    }
    
    const char *act_path;
    act_path=this->_ActParseXml->NodeAttr(parse_node,"ActPath");
    //获取 Source  String
    string parse_buff="";
    err_str = this->ParseValue(parse_node,parse_buff,add_paras);
    
    //ly table_sync
    string act_node_str("");
    if (err_str.size()==0){
        const char *act_node=this->_ActParseXml->NodeAttr(parse_node,"ActNode");
        if (act_node && strlen(act_node)!=0)
            err_str = this->ParseValue(string(act_node),act_node_str,add_paras);
    }
    
    if ((err_str.size()==0) && (strlen(func_name)!=0)){
        //函数操作
        string xpath=string("/XMLParse/FUNCTION/")+func_name;
        ParseXml_NodeSet nodes = this->_ActParseXml->GetNodeSetByXpath(xpath.c_str());
        if (nodes.size()==0)
            err_str=string("can't find definiation of FUNCTION Name='")+func_name+"!";
        else if (nodes.size()>1)
            err_str=string("FUNCTION Name='")+func_name+"'is multiple definition!";
        else{
            //取得真实函数 node
            ParseXml_NodeSet::const_iterator it = nodes.begin();
            pugi::xpath_node xnode = *it;
            actions_node = xnode.node();
        }
    }else if ((err_str.size()==0) && (strlen(act_path)!=0)){
        //函数操作
        string xpath=act_path;
        ParseXml_NodeSet nodes = this->_ActParseXml->GetNodeSetByXpath(xpath.c_str());
        if (nodes.size()==0)
            err_str=string("can't find definiation of ActPath='")+act_path+"!";
        else if (nodes.size()>1)
            err_str=string("ActPath='")+act_path+"'is multiple definition!";
        else{
            //取得真实函数 node
            ParseXml_NodeSet::const_iterator it = nodes.begin();
            pugi::xpath_node xnode = *it;
            actions_node = xnode.node();
        }
    }else if ((err_str.size()==0) && (act_node_str.size()!=0)){//ly table_sync
        BB_SIZE_T act_node_addr=STRING_Helper::scanUINTIDStr(act_node_str,act_node_str.size()/2*8);
        ParseXml_Node *node=(ParseXml_Node *)act_node_addr;
        actions_node=*node;
    }else if (err_str.size()==0){
        err_str=string("Call  node must defin Function's  attribute !");
    }
    
    //调用函数操作
    if(err_str.size()==0){
        //循环取得Param
        for (pugi::xml_attribute_iterator ait = parse_node.attributes_begin(); ait != parse_node.attributes_end(); ++ait)
        {
            if(strcmp(ait->name(),"FUNCTION")==0 || strcmp(ait->name(),"FUNC")==0 || strcmp(ait->name(),"ActPath")==0 || strcmp(ait->name(),"Test")==0)
                continue;
            if (strstr(ait->value(),"{")!=NULL){//如果可能需要parseValue(
                string r_str="",value_str="",src_str=ait->value();
                r_str=this->ParseValue(src_str,value_str,add_paras);
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type((string)"{@_"+func_name+"_"+ait->name()+"}",value_str));
            }else
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type((string)"{@_"+func_name+"_"+ait->name()+"}",ait->value()));
        }
        //pugi::xml_attribute_iterator tmp=parse_node.attributes_end();
        //添加内容
        if (func_name!=NULL && strlen(func_name)!=0){
            this->_PubObj->SetNodeValue(((string)"@_"+func_name+"_CONTENT").c_str(),parse_buff.data(),Http_ParseObj::STR_COPY_OBJ,parse_buff.size());
        }
        //取得回调地址参数 //ly table_sync
        BB_SIZE_T node_addr=(BB_SIZE_T)(&actions_node);
        string node_addr_str=STRING_Helper::putUINTToStr(node_addr,sizeof(node_addr)*8);
        add_paras.insert(ParseProcess::PV_AddParas_T::value_type((string)"{@_"+func_name+"_THIS_CALL_NODE}",
                                                                 STRING_Helper::HBCStrToHBCStr(node_addr_str,"char","hex",false)));
        
        this->FUNCTION(actions_node,add_paras);   //ACTIONS 操作
        if (func_name!=NULL && strlen(func_name)!=0)
            this->_PubObj->DeleteObjNode(((string)"@_"+func_name+"_CONTENT").c_str());
        //删除内容
        
        //循环删除Param
        for (pugi::xml_attribute_iterator ait = parse_node.attributes_begin(); ait != parse_node.attributes_end(); ++ait)
        {
            if(strcmp(ait->name(),"FUNCTION")==0 || strcmp(ait->name(),"FUNC")==0 || strcmp(ait->name(),"ActPath")==0 || strcmp(ait->name(),"Test")==0)
                continue;
            
            add_paras.erase(string("{@_")+func_name+"_"+ait->name()+"}");
        }
    }
    
    if(err_str!=""){
        this->SHOW_ERROR(err_str);
        this->ParseDeep--;
        return ParseProcess::ACT_ERROR;
    }else{
        this->ParseDeep--;
        return ParseProcess::ACT_SUCCESS;
    }
}*/

int ParseProcess::SEND(ParseXml_Node parse_node, PV_AddParas_T &add_paras){
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    string err_str("");
    //取得 Source  node 内容
    string parse_buff;
    this->ParseValue(parse_node,parse_buff,add_paras);
    
    //ParsePacket操作
    //取得Xpath of ParseNode ???差错提示
    string parse_path,parse_path_temp = this->_ActParseXml->NodeAttr(parse_node,"Packet");//没有 attribute 为空的处理.
    STRING_Helper::strTrim(parse_path_temp);
    if (parse_path_temp.compare(0,1,"{")==0)
        this->ParseValue(parse_path_temp,parse_path,add_paras);
    else
        parse_path=parse_path_temp;
    
    string parse_path2=this->_ActParseXml->NodeAttr(parse_node,"Packet2");
    
    //创建目标xml根 node ；
    //取得临时存储目标
    string to_obj=this->_ActParseXml->NodeAttr(parse_node,"ParseToObj");
    if (to_obj.size()==0)
        to_obj="@THIS_PARSE_XML";   //???{FD}
    string parse_xml="<Root></Root>";//临时创建一个今后还可以变化.
    Http_ParseXML * temp_xml=this->_PubObj->XmlObjCreate(to_obj.c_str(),parse_xml.c_str());//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
    if (temp_xml==NULL)
        this->SHOW_ERROR(this->_PubObj->ErrorStr);
    ParseXml_Node xml_node =temp_xml->FirstChild(temp_xml->ObjXml);
    
    // Config DEBUGY
    const char* is_debug=this->_ActParseXml->NodeAttr(parse_node,"Debug");
    if (strstr(is_debug,"SetTo")!=0)
        this->_PubPacket->SetDebug();
    
    //Parse Method:BIN_LEN|HTTP
    string method=this->_ActParseXml->NodeAttr(parse_node,"Method");
    
    //在根 node 写入相关的字段
    if (method.size()==0){
        const char *is_trans=this->_ActParseXml->NodeAttr(parse_node,"Trans");  //ly add
        if (strstr(is_trans,"BIN_TO_HEX")==is_trans){
            string tmp_parse_buff=this->_PubPacket->HBCStrToHBCStr(parse_buff,"char",LW_HEX_STR,true);
            if (!this->_PubPacket->GetFieldsFormOriginStr(parse_path.c_str(),tmp_parse_buff, xml_node,to_obj+":"+"/Root")){
                // Object Param
                this->SHOW_ERROR(this->_PubPacket->ErrorStr);
            };
        }else{
            if (!this->_PubPacket->GetFieldsFormOriginStr(parse_path.c_str(),parse_buff, xml_node,to_obj+":"+"/Root")){
                // Object Param
                this->SHOW_ERROR(this->_PubPacket->ErrorStr);
            };
        }
    }
    
    //关闭debug
    if (strstr(is_debug,"SetTo")!=0)
        this->_PubPacket->CloseDebug();
    //--------------------------------
    
    //const char *parse_type=	this->_ActParseXml->NodeAttr(parse_node,"ParseType");
    //取得下级函数 __ACTION__ node
    ParseXml_Node actions_node;
    //函数操作
    string xpath=parse_path2+"/__ACTIONS__";
    ParseXml_NodeSet nodes = this->_ActParseXml->GetNodeSetByXpath(xpath.c_str());
    if (nodes.size()==0){
        //err_str=string("can't find definiation of ")+xpath+"!";
    }else{
        //取得真实函数 node
        ParseXml_NodeSet::const_iterator it = nodes.begin();
        pugi::xpath_node xnode = *it;
        actions_node = xnode.node();
        //调用函数操作
        add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string((string)"{@_THIS_PACKET_LEN}"),this->_PubPacket->_iToStr(parse_buff.size(),10)));
        add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string((string)"{@_THIS_PACKET}"),parse_buff));
        add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string((string)"{@_FD}"),"FD_1"));
        this->FUNCTION(actions_node,add_paras);   //ACTIONS 操作
        add_paras.erase("{@_FD}");
        add_paras.erase("{@_THIS_PACKET}");
        add_paras.erase("{@_THIS_PACKET_LEN}");
    };
    
    //调用函数操作
    //if(err_str.size()==0){
    //
    //}
    
    if(err_str!=""){
        this->SHOW_ERROR(err_str);
        this->ParseDeep--;
        return ParseProcess::ACT_ERROR;
    }else{
        this->ParseDeep--;
        return ParseProcess::ACT_SUCCESS;
    }
}

string ParseProcess::_GetNodeAttrStr(ParseXml_Node parse_node, const char* attr, PV_AddParas_T &add_paras) {
    const char* temp = this->_ActParseXml->NodeAttr(parse_node, attr);//没有 attribute 为空的处理.
    if (temp == 0 || strlen(temp) == 0)
        return "";
    string parse_path_temp(temp);
    if (parse_path_temp.compare(0, 1, "{") == 0) {
        string parse_path;
        this->ParseValue(parse_path_temp, parse_path, add_paras);
        return parse_path;
    }
    else
        return parse_path_temp;
}

//ly queue;
int ParseProcess::SET_QUEUE(ParseXml_Node parse_node, PV_AddParas_T &add_paras) {
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    string err_str("");
    //取得 Source  node 内容
    string parse_buff;
    this->ParseValue(parse_node, parse_buff, add_paras);
    
    //ParsePacket操作
    //取得Xpath of ParseNode ???差错提示
    string name = this->_GetNodeAttrStr(parse_node, "Qpath", add_paras); //队列名称路径
    string cell_id = this->_GetNodeAttrStr(parse_node, "Qid", add_paras); //队列名称路径
    string pkg = this->_GetNodeAttrStr(parse_node, "Pkg",add_paras); //队列包
    string prio_str = this->_GetNodeAttrStr(parse_node, "Prio", add_paras); //优先级
    if (!STRING_Helper::CheckNum(prio_str)) {
        err_str = "Attribute->'Prio' is should be number! ";
    }else{
        BYTE prio = STRING_Helper::StrToi(prio_str);
        //取得下级函数 __ACTION__ node
        //ParseXml_Node actions_node = this->_ActParseXml->Child(parse_node, "ACTIONS");
        ParseXml_Node actions_node = parse_node;
        if (GB_AvartarDB->DoQueueTableTask_PushItem(name, prio, cell_id, pkg, actions_node) != 0)
            err_str = (string)"Queue Path(" + name + ") is not exited or Queue Table not found!";
    }
    
    if (err_str != "") {
        this->SHOW_ERROR(err_str);
        this->ParseDeep--;
        return ParseProcess::ACT_ERROR;
    }
    else {
        this->ParseDeep--;
        return ParseProcess::ACT_SUCCESS;
    }
}

//ly queue;
int ParseProcess::DB_QUERY_EXT(ParseXml_Node parse_node, PV_AddParas_T &add_paras) {
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    string err_str("");
    string parse_buff("");
    string to_obj_buf,to_arr_buff,header_arr_buff;
    const char *to_obj=NULL,*to_arr=NULL,*header_arr=NULL,*is_debug,*err_obj=NULL;
    int start_pos = 0;
    char *start;
    const char *rec_num_obj;
    string arr_result;
    BB_OBJ *result_obj=NULL;
    BB_SIZE_T result_size;


    //DB参数
    const char *db_name=this->_ActParseXml->NodeAttr(parse_node,"DB");
    if(db_name==NULL || strstr(db_name,"AVT_DB")!=db_name){
        err_str=(string)"DB="+db_name+" not support now!";
        goto end1;
    }
    //目标参数
    to_obj=this->_ActParseXml->NodeAttr(parse_node,"To");
    if (to_obj!=NULL && strstr(to_obj,"{")!=NULL){//增加对To node 的解析
        this->ParseValue((string)to_obj,to_obj_buf,add_paras);
        to_obj=to_obj_buf.c_str();
    }
    //目标参数
    to_arr=this->_ActParseXml->NodeAttr(parse_node,"Result");
    if (to_arr!=NULL && strstr(to_arr,"{")!=NULL){//增加对To node 的解析
        this->ParseValue((string)to_arr,to_arr_buff,add_paras);
        to_arr=to_arr_buff.c_str();
    }
    //转换需求
    header_arr=this->_ActParseXml->NodeAttr(parse_node,"HeaderPath");
    if (header_arr!=NULL && strstr(header_arr,"{")!=NULL){//增加对To node 的解析
        this->ParseValue((string)header_arr,header_arr_buff,add_paras);
        header_arr=header_arr_buff.c_str();
    }


    //目标参数
	err_obj=this->_ActParseXml->NodeAttr(parse_node,"Err");

    // 参数：DEBUGY
    is_debug=this->_ActParseXml->NodeAttr(parse_node,"Debug");
    // 参数RFind
    rec_num_obj=this->_ActParseXml->NodeAttr(parse_node,"Num");

    //！取得Value
    this->ParseValue(parse_node,parse_buff,add_paras);//正常

    //Query操作
    start = (char *)parse_buff.c_str();
    if ((STRING_Helper::C_CmpFirstStr(start, start_pos, "[,", ",", parse_buff.size(), true) >= 0)) {
        string query_str = (string) "%B" + parse_buff;
        string result_str,query_barray,func_return(""),packet_head("");
        //转换成二进制流数组
        if (GB_AvartarDB->JsonBArrayToBArray(query_str, query_barray, add_paras) < 0) {//ly err_block
            err_str = "query data must be json as '[[***]]' ";
            goto end1;
        }
        //Query操作
        BYTE r=GB_AvartarDB->DoActionMessage(packet_head, query_barray, "", func_return, add_paras);
        //DEBUG操作
        if (strstr(is_debug,"SetTo")!=0) {
            if (func_return.size() != 0) {
                BB_OBJ * src_obj = (BB_OBJ *)func_return.data();
                string dest;
                if (GB_AvartarDB->BArrayToJsonArray(src_obj, dest, add_paras) < 0){
                    this->DEBUGY("[[\"error\"," + GB_AvartarDB->GET_LAST_ERROR() + "]]");
                }else
                    this->DEBUGY("[" + dest + "]");
            }
            else
                this->DEBUGY("[]");
        }
        //写入操作;//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
        if(to_obj!=NULL && to_obj[0]!=0){
            if (this->_PubObj->SetNodeValue(to_obj,func_return.c_str(),Http_ParseObj::STR_COPY_OBJ,func_return.size())
                ==Http_ParseObj::SET_VALUE_ERROR){
                err_str=this->_PubObj->ErrorStr;
                goto end1;
            }
        }
        //取得结果数组
        //this->ParseValue((string)"{"+to_obj+"[1]}",arr_result,add_paras);
        if (r==0){
            if(to_arr!=NULL && to_arr[0]!=0){
                //生成结果数组
                BB_GET_ARRAY_REC_BY_NUM((BB_OBJ *)func_return.c_str(),1,&result_obj,&result_size);
                if (header_arr!=NULL && header_arr[0]!=0){
                    //生成结果数组，并替换表格头；
                    BB_OBJ *dest_obj;
                    BB_SIZE_T dest_obj_size;
                    if (!GB_AvartarDB->CreateArray_H2C(header_arr_buff,(const char *)result_obj,result_size,&dest_obj,dest_obj_size)){
                        err_str="Result_Header Path is not found!";
                        goto end1;
                    }
                    if (this->_PubObj->SetNodeValue(to_arr,(const char *)dest_obj,Http_ParseObj::STR_COPY_OBJ,dest_obj_size)
                        ==Http_ParseObj::SET_VALUE_ERROR){
                        free(dest_obj);
                        err_str=this->_PubObj->ErrorStr;
                        goto end1;
                    }
                    free(dest_obj);
                }else {
                    if (this->_PubObj->SetNodeValue(to_arr,(const char *)result_obj,Http_ParseObj::STR_COPY_OBJ,result_size)
                        ==Http_ParseObj::SET_VALUE_ERROR){
                        err_str=this->_PubObj->ErrorStr;
                        goto end1;
                    }
                }
            }
        }

        //查询的记录数
        if (rec_num_obj!=NULL && strlen(rec_num_obj)!=0){
            if(r==0){
                char *tmp=(char *)result_obj;
                char null_array[]={(char)0x80,0x01,0x00,0x00};
                if (STRING_Helper::C_StrStrLen(tmp,null_array,4)==tmp){
                    this->_PubObj->SetNodeValue(rec_num_obj,"0",Http_ParseObj::STR_COPY_OBJ);
                } else{
                    BB_SIZE_T rec_num=BB_GET_ARRAY_NUM(result_obj);
                    this->_PubObj->SetNodeValue(rec_num_obj,STRING_Helper::iToStr(rec_num).c_str(),Http_ParseObj::STR_COPY_OBJ);
                }
            } else
                this->_PubObj->SetNodeValue(rec_num_obj,"0",Http_ParseObj::STR_COPY_OBJ);
        }
    }
    else {
        err_str = "query data must be json as '[[***]]'!";
    }

    end1:
    if (err_str != "") {
        this->SHOW_ERROR(err_str);
        if ((err_obj!=NULL) && (strlen(err_obj)>0))
        	this->_PubObj->SetNodeValue(err_obj,err_str.c_str(),Http_ParseObj::STR_COPY_OBJ);
        this->ParseDeep--;
        return ParseProcess::ACT_ERROR;
    }
    else {
        this->ParseDeep--;
        return ParseProcess::ACT_SUCCESS;
    }
}

//ly queue;
int ParseProcess::DB_QUERY(ParseXml_Node parse_node, PV_AddParas_T &add_paras) {
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    string err_str("");
    string parse_buff("");
    string to_obj_buf,to_arr_buff,header_arr_buff;
    const char *to_obj=NULL,*to_arr=NULL,*header_arr=NULL,*is_debug,*err_obj=NULL;
    int start_pos = 0;
    char *start;
    const char *rec_num_obj;
    string arr_result;
    BB_OBJ *result_obj=NULL;
    BB_SIZE_T result_size;
    
    
    //DB参数
    const char *db_name=this->_ActParseXml->NodeAttr(parse_node,"DB");
    if(db_name==NULL || strstr(db_name,"AVT_DB")!=db_name){
        err_str=(string)"DB="+db_name+" not support now!";
        goto end1;
    }
    //目标参数
    to_obj=this->_ActParseXml->NodeAttr(parse_node,"To");
    if (to_obj!=NULL && strstr(to_obj,"{")!=NULL){//增加对To node 的解析
        this->ParseValue((string)to_obj,to_obj_buf,add_paras);
        to_obj=to_obj_buf.c_str();
    }
    //目标参数
    to_arr=this->_ActParseXml->NodeAttr(parse_node,"Result");
    if (to_arr!=NULL && strstr(to_arr,"{")!=NULL){//增加对To node 的解析
        this->ParseValue((string)to_arr,to_arr_buff,add_paras);
        to_arr=to_arr_buff.c_str();
    }
    //转换需求
    header_arr=this->_ActParseXml->NodeAttr(parse_node,"Result_Header");
    if (header_arr!=NULL && strstr(header_arr,"{")!=NULL){//增加对To node 的解析
        this->ParseValue((string)header_arr,header_arr_buff,add_paras);
        header_arr=header_arr_buff.c_str();
    }
    
    
    //目标参数
    err_obj=this->_ActParseXml->NodeAttr(parse_node,"Err");
    
    // 参数：DEBUGY
    is_debug=this->_ActParseXml->NodeAttr(parse_node,"Debug");
    // 参数RFind
    rec_num_obj=this->_ActParseXml->NodeAttr(parse_node,"Num");
    
    //！取得Value
    this->ParseValue(parse_node,parse_buff,add_paras);//正常
    
    //Query操作
    start = (char *)parse_buff.c_str();
    if ((STRING_Helper::C_CmpFirstStr(start, start_pos, "[,", ",", parse_buff.size(), true) >= 0)) {
        string query_str = (string) "%B" + parse_buff;
        string result_str,query_barray,func_return(""),packet_head("");
        //转换成二进制流数组
        if (GB_AvartarDB->JsonBArrayToBArray(query_str, query_barray, add_paras) < 0) {//ly err_block
            err_str = "query data must be json as '[[***]]' ";
            goto end1;
        }
        //Query操作
        BYTE r=GB_AvartarDB->DoActionMessage(packet_head, query_barray, "", func_return, add_paras);
        //DEBUG操作
        if (strstr(is_debug,"SetTo")!=0) {
            if (func_return.size() != 0) {
                BB_OBJ * src_obj = (BB_OBJ *)func_return.data();
                string dest;
                if (GB_AvartarDB->BArrayToJsonArray(src_obj, dest, add_paras) < 0){
                    this->DEBUGY("[[\"error\"," + GB_AvartarDB->GET_LAST_ERROR() + "]]");
                }else
                    this->DEBUGY("[" + dest + "]");
            }
            else
                this->DEBUGY("[]");
        }
        //写入操作;//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
        if(to_obj!=NULL && to_obj[0]!=0){
            if (this->_PubObj->SetNodeValue(to_obj,func_return.c_str(),Http_ParseObj::STR_COPY_OBJ,func_return.size())
                ==Http_ParseObj::SET_VALUE_ERROR){
                err_str=this->_PubObj->ErrorStr;
                goto end1;
            }
        }
        //取得结果数组
        //this->ParseValue((string)"{"+to_obj+"[1]}",arr_result,add_paras);
        if (r==0){
            BB_GET_ARRAY_REC_BY_NUM((BB_OBJ *)func_return.c_str(),1,&result_obj,&result_size);
            if(to_arr!=NULL && to_arr[0]!=0){
                //生成结果数组
                if (header_arr!=NULL && header_arr[0]!=0){
                    //生成结果数组，并替换表格头；
                    BB_OBJ *dest_obj;
                    BB_SIZE_T dest_obj_size;
                    if (!GB_AvartarDB->CreateArray_H2C(header_arr_buff,(const char *)result_obj,result_size,&dest_obj,dest_obj_size)){
                        err_str="Result_Header Path is not found!";
                        goto end1;
                    }
                    if (this->_PubObj->SetNodeValue(to_arr,(const char *)dest_obj,Http_ParseObj::STR_COPY_OBJ,dest_obj_size)
                        ==Http_ParseObj::SET_VALUE_ERROR){
                        free(dest_obj);
                        err_str=this->_PubObj->ErrorStr;
                        goto end1;
                    }
                    free(dest_obj);
                }else {
                    if (this->_PubObj->SetNodeValue(to_arr,(const char *)result_obj,Http_ParseObj::STR_COPY_OBJ,result_size)
                        ==Http_ParseObj::SET_VALUE_ERROR){
                        err_str=this->_PubObj->ErrorStr;
                        goto end1;
                    }
                }
            }
        }
        
        //查询的记录数
        if (rec_num_obj!=NULL && strlen(rec_num_obj)!=0){
            if(r==0){
                char *tmp=(char *)result_obj;
                char null_array[]={(char)0x80,0x01,0x00,0x00};
                if (STRING_Helper::C_StrStrLen(tmp,null_array,4)==tmp){
                    this->_PubObj->SetNodeValue(rec_num_obj,"0",Http_ParseObj::STR_COPY_OBJ);
                } else{
                    BB_SIZE_T rec_num=BB_GET_ARRAY_NUM(result_obj);
                    this->_PubObj->SetNodeValue(rec_num_obj,STRING_Helper::iToStr(rec_num).c_str(),Http_ParseObj::STR_COPY_OBJ);
                }
            } else
                this->_PubObj->SetNodeValue(rec_num_obj,"0",Http_ParseObj::STR_COPY_OBJ);
        }
    }
    else {
        err_str = "query data must be json as '[[***]]'!";
    }
    
end1:
    if (err_str != "") {
        this->SHOW_ERROR(err_str);
        if ((err_obj!=NULL) && (strlen(err_obj)>0))
            this->_PubObj->SetNodeValue(err_obj,err_str.c_str(),Http_ParseObj::STR_COPY_OBJ);
        this->ParseDeep--;
        return ParseProcess::ACT_ERROR;
    }
    else {
        this->ParseDeep--;
        return ParseProcess::ACT_SUCCESS;
    }
}


//ly_send_queue
/*int ParseProcess::SendQueue_PushItem(const char *msg, int msg_len,const char *dest,const char *src,int trans_type,bool close_after_send,ParseXml_Node rsp_actions_node,int timeout) {
    //写入队列（已创建）
    string pkg=string(msg,msg_len);
    string node_id=dest;
    Conn *dest_conn;
    //判断是否有该链接
    if ((dest_conn = GB_ConnQueue->FindConnByObjName(dest, 0)) == NULL){
        this->SHOW_ERROR((string)"SendMsgAddQueue()->Send Error: DestObj"+dest+"hasn't create!");
        return -1;
    }
    //目标对象添加队列
    dest_conn->ProcessObj->sendQueue->PushItem(0,node_id,pkg,rsp_actions_node,this,trans_type,close_after_send,timeout);

    return 0;
}*/

//int ParseProcess::SendQueue_PopSend() {
//    //目标对象添加队列
//    string node_id_str;
//    string send_pkg;
//    int trans_type;
//    bool close_after_send;
//    int time_out;
//    int r=0;
//    if (this->sendQueue->PopItem(node_id_str, send_pkg, this->sendQueue_SrcNode,&(this->sendQueue_SrcObj), trans_type,close_after_send,this->sendQueue_TimeVal)){
//        //发送操作
//        r=this->SendCBFunc((const char*)send_pkg.c_str(), send_pkg.size(), "@THIS_FD", "", trans_type, close_after_send, this->SendCBFuncHandle);  //???this->SendCBFuncHandle
//        //设置超时时间：
//        this->sendQueue_TimeStart =LW_GetTickCount();//取得当前时间
//        this->sendQueue_TimeVal=time_out; //用弹出时间
//    }
//    return r;
//}
//
//int ParseProcess::SendQueue_RspAct(const char *msg, int msg_len) {//返回数据包长度
////调用队列中的Pkg操作
//    PV_AddParas_T add_paras;
//    if (this->sendQueue_SrcObj==NULL)
//        return -1;
//
//    if (this->sendQueue_SrcObj->_PubObj->SetNodeValue("@__PKG", msg, Http_ParseObj::STR_COPY_OBJ, msg_len) != Http_ParseObj::SUCCESS) {
//        this->SHOW_ERROR("Attribute->can't read'@__PKG'---maybe readonly! ");
//        return -2;
//    }
//    this->sendQueue_SrcObj->FUNCTION(this->sendQueue_SrcNode, add_paras);   //ACTIONS 操作
//    this->sendQueue_SrcObj->_PubObj->SetNodeValue("@__PKG", "", Http_ParseObj::STR_COPY_OBJ);
//
//    return 0;
//}
//
//int ParseProcess::SendQueue_PopAllAct(const char *msg, int msg_len) {
//    //目标对象添加队列
//    string node_id_str;
//    string send_pkg;
//    int trans_type;
//    bool close_after_send;
//    int num=0;
//    int time_out;
//
//    while(this->sendQueue->PopItem(node_id_str, send_pkg, this->sendQueue_SrcNode,&(this->sendQueue_SrcObj), trans_type,close_after_send,time_out)){
//        //返回操作
//        int r=this->SendCBFunc((const char*)send_pkg.c_str(), send_pkg.size(), "@THIS_FD", "", trans_type, close_after_send, this->SendCBFuncHandle);
//        num++;
//    }
//    return 0;
//}
//
//int ParseProcess::SendQueue_Size() {//返回数据包长度
////调用队列中的Pkg操作
//    return this->sendQueue->GetSize();
//}
//
//int ParseProcess::SendQueue_ChkTimeOut(const char *conn_name) {//返回数据包长度
//    //查找指定的连接是否有超时处理。
//
//    Conn *dest_conn;
//    //判断是否有该链接
//    if ((dest_conn = GB_ConnQueue->FindConnByObjName(conn_name, 0)) == NULL){
//        //this->SHOW_ERROR((string)"SendQueue_ChkTimeOut()->Send Error: DestObj"+dest+"hasn't create!");
//        return -1;
//    }
//
//    PV_AddParas_T add_paras;
//    UInt64 now=LW_GetTickCount();//取得当前时间
//
//    if (now>(dest_conn->ProcessObj->sendQueue_TimeStart+dest_conn->ProcessObj->sendQueue_TimeVal)){
//        //超时处理操作
//        ParseXml_Node err_node=dest_conn->ProcessObj->_ActParseXml->Child(dest_conn->ProcessObj->CfgNode,dest_conn->ProcessObj->sendQueue_TimeOutErrPath);
//        dest_conn->ProcessObj->FUNCTION(err_node, add_paras);   //ACTIONS 操作
//    }
//}

int ParseProcess::FOR(ParseXml_Node parse_node, PV_AddParas_T &add_paras){
    this->DEBUGY(parse_node);
    this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
    
    string err_str("");
    ParseXml_Node in_node=this->_ActParseXml->Child(parse_node,"IN");
    //ly call_func; //ly rec_chg_mark //ly  table_sync3;
    if (in_node==NULL || !in_node)
        in_node=this->_ActParseXml->Child(parse_node,"_IN_");
    //ParseXml_Node actions_node=this->_ActParseXml->Child(parse_node,"ACTIONS");
    ParseXml_Node actions_node=parse_node; //???今后替换parse_node;
    
    const char *type=this->_ActParseXml->NodeAttr(parse_node,"Type");
    const char* is_debug=this->_ActParseXml->NodeAttr(parse_node,"Debug");
    
    //判断是否存在就的FOR_NUM
    string stack_num("");
    PV_AddParas_T::iterator it2;
    for (it2 = add_paras.find((string)"{@_FOR_NUM_"+stack_num+"}"); it2!=add_paras.end(); it2 = add_paras.find((string)"{@_FOR_NUM_"+stack_num+"}") ){
        if (stack_num=="")
            stack_num="1";
        else
            stack_num=this->_PubPacket->_iToStr((this->_PubPacket->_StrToi(stack_num)+1),10);
    }
    
    //取得解析 node
    if (strlen(type)==0)
        err_str="TYPE node is'nt exist or definition error! ";
    if (!actions_node)
        err_str="ACTIONS node is not exist or definition error! ";
    
    //对IN的内容进行解析.
    if (err_str.size()==0 && strstr(type,"FROM_TO")!=0){
        //取得begin end attribute
        const char *from_str = this->_ActParseXml->NodeAttr(parse_node,"From");
        const char *to_str = this->_ActParseXml->NodeAttr(parse_node,"To");
        const char *step_str = this->_ActParseXml->NodeAttr(parse_node,"Step");
        string from; this->ParseValue(from_str,from,add_paras);from+='\0';
        string to;
        this->ParseValue(to_str,to,add_paras);to+='\0';//有可能0补充
        string step; this->ParseValue(step_str,step,add_paras);//有可能0补充
        
        string test_condition,dest_condition;
        const char *break_test=this->_ActParseXml->NodeAttr(parse_node,"BreakTest");
        
        int from_i = this->_PubPacket->_StrToi(from);
        int to_i = this->_PubPacket->_StrToi(to); //cout << to << to.size() <<endl;
        int step_i;
        if (step!="")
            step_i = this->_PubPacket->_StrToi(step);
        else
            step_i=1;
        
        //???请在这里补充判断from to 是否为合法数字 String .
        
        //循环操作处理
        string value_str,err_str(""),break_test_str(break_test);
        for (int i=from_i; i <= to_i; i+=step_i){
            //判断是否进入,TestConditions判断
            if (strlen(break_test)!=0 && this->TestConditionStr(break_test_str,err_str,add_paras)){
                if (strstr(is_debug,"PvBreak")!=0){
                    string deb_str; this->ParseValue(break_test_str,deb_str,add_paras);
                    if (strstr(is_debug,"PvBreakIn")!=0)
                        this->DEBUGY((string)"FOR_BREAK: "+break_test+" :"+deb_str);
                    else
                        this->DEBUGY((string)"FOR_BREAK: "+deb_str);
                }
                break;
            }if (err_str.size()!=0)
                break;
            
            //取得Param值操作
            add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string((string)"{@_FOR_NUM_"+stack_num+"}"),this->_PubPacket->_iToStr(i,10) ));
            this->DEBUGY(actions_node);
            this->ParseDeep++;
            this->ParseAction(actions_node,add_paras); //ACTIONS 操作；
            this->ParseDeep--;
            add_paras.erase("{@_FOR_NUM_"+stack_num+"}");
            ////To换算.
            //if (strstr(to_str,"{")!=NULL){
            //	this->ParseValue(to_str,to,add_paras);to+='\0';
            //	int to_i = this->_PubPacket->_StrToi(to); //cout << to << to.size() <<endl;
            //}
        }
        
    }
	else if (err_str.size()==0 && (strstr(type,"CUT_PACKET_LEN")!=0 || strstr(type,"CUT_LEN")!=0 )){
        //取得LenStart LenLen
        string cut_obj_tmp(this->_ActParseXml->NodeAttr(parse_node,"CutObj"));
        string cut_obj("");
        string in_buff("");
        if (cut_obj_tmp.size()!=0){//原方式
            this->ParseValue(cut_obj_tmp,cut_obj,add_paras);
            //string in_buff = this->_PubObj->GetNodeValueStr(cut_obj.data(),cut_obj.size());
            if (!this->_PubObj->GetNodeValueStr(cut_obj.c_str(),in_buff)){
                err_str=this->_PubObj->ErrorStr;
                goto end;
            }
        }else{//ly call_func; //ly rec_chg_mark //ly  table_sync3;
            err_str=this->ParseValue(in_node,in_buff,add_paras);   //解析IN node
            if (err_str.size()!=0)
                goto end;
        };
        
        if (cut_obj.size()==0 && in_buff.size()==0){
            err_str="Type='CUT_PACKET_LEN' mode must set 'CutObj' or '_IN_' attribute !";
            goto end;
        }
        
        string len_start_str(this->_ActParseXml->NodeAttr(parse_node,"LenStart"));
        string len_len_str(this->_ActParseXml->NodeAttr(parse_node,"LenLen"));
        string len_str_tmp(this->_ActParseXml->NodeAttr(parse_node,"Len"));
        string len_str;
        
        //取得待解析源 String
        const char *break_test=this->_ActParseXml->NodeAttr(parse_node,"BreakTest");
//        if (cut_obj.size()==0){
//            err_str="Type='CUT_PACKET_LEN' mode must set 'CutObj' attribute !";
//            goto end;
//        }
        
        
        //循环操作处理
        if (len_str_tmp.size()==0){//len_start; len_len处理。
            if (len_start_str.size()==0 || len_len_str.size()==0){
                err_str="Type='CUT_PACKET_LEN' mode must set 'LenStart' and 'LenLen' or 'len' attribute !";
                goto end;
            }
            size_t len_start=STRING_Helper::ParseInt(len_start_str.c_str(),16);//???0x操作
            size_t len_len = STRING_Helper::ParseInt(len_len_str.c_str(),16);
            
            string packet_str;
            for (int i=0;;i++){
                //MatchStr指针适应处理
                //SkipStr跳过处理
                //测试BreakTest
                if (strlen(break_test)!=0 && this->TestConditionStr(break_test,add_paras))
                    break;
                //切分长度处理
                if (in_buff.size() > len_start+len_len){//对比最小字串
                    //读取Len, 街区字段不规范字段
                    size_t packet_len = STRING_Helper::ParseInt(in_buff.substr(len_start,len_len),16)*2;  //from Hex To Byte
                    if (packet_len < len_start) {//这种情况一般都是packet错误的情况
                        err_str=(string)"packet_len get error: packet_len("+in_buff.substr(len_start,len_len)+") !";
                        goto end;
                    }
                    
                    if (in_buff.size() >= packet_len){
                        //取得数据包
                        packet_str= in_buff.substr(0,packet_len);  //??
                        in_buff.erase(0,packet_len);
                        //尝试取得Len;每次都需运行，因为可能以当前包来核算
                        add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_VALUE_"+stack_num+"}"),packet_str));
                        add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_NUM_"+stack_num+"}"),this->_PubPacket->_iToStr((int)i,10) ));
                        add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_SIZE_"+stack_num+"}"),this->_PubPacket->_iToStr((int)packet_str.size(),10) ));
                        this->DEBUGY(actions_node);
                        this->ParseDeep++;
                        this->ParseAction(actions_node,add_paras); //ACTIONS 操作；
                        this->ParseDeep--;
                        add_paras.erase("{@_FOR_VALUE_"+stack_num+"}");
                        add_paras.erase("{@_FOR_NUM_"+stack_num+"}");
                        add_paras.erase("{@_FOR_SIZE_"+stack_num+"}");
                        continue;
                    }
                }
                break;
            }
            //剩余数据包回写字段,等待下次累计
            this->_PubObj->SetNodeValue(cut_obj.c_str(),in_buff.c_str(),Http_ParseObj::STR_COPY_OBJ);//string????SetNodeValueStr();
        }else{//Len公式模式。
            string packet_str;
            for (int i=0;;i++){
                //MatchStr指针适应处理
                //SkipStr跳过处理
                //测试BreakTest
                if (strlen(break_test)!=0 && this->TestConditionStr(break_test,add_paras))
                    break;
                //读取Len, 截取字段不规范字段
                this->ParseValue(len_str_tmp,len_str,add_paras);
                size_t packet_len = STRING_Helper::ParseInt(len_str);  //from Hex To Byte
                if (packet_len==STRING_Helper::NaN || packet_len==0) {//这种情况一般都是packet错误的情况
                    err_str=(string)"packet_len get error: packet_len("+len_str_tmp+") !";
                    goto end;
                }
                //截取操作。
                if (in_buff.size() >= packet_len){
                    //取得数据包
                    packet_str= in_buff.substr(0,packet_len);  //??
                    in_buff.erase(0,packet_len);
                    //尝试取得Len;每次都需运行，因为可能以当前包来核算
                    add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_VALUE_"+stack_num+"}"),packet_str));
                    add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_NUM_"+stack_num+"}"),this->_PubPacket->_iToStr((int)i,10) ));
                    add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_SIZE_"+stack_num+"}"),this->_PubPacket->_iToStr((int)packet_str.size(),10) ));
                    //this->DEBUGY(actions_node);
                    //this->ParseDeep++;
                    this->ParseAction(actions_node,add_paras); //ACTIONS 操作；
                    //this->ParseDeep--;
                    //剩余数据包回写字段,等待下次累计
                    add_paras.erase("{@_FOR_VALUE_"+stack_num+"}");
                    add_paras.erase("{@_FOR_NUM_"+stack_num+"}");
                    add_paras.erase("{@_FOR_SIZE_"+stack_num+"}");
                    if (in_buff.size()==0){
                        break;
                    }
                    if (cut_obj.size()!=0)
                        this->_PubObj->SetNodeValue(cut_obj.c_str(),in_buff.data(),Http_ParseObj::STR_COPY_OBJ,in_buff.size());//string????SetNodeValueStr();
                    continue;
                }
                break;
            }
            
        };
        
    }
	else if (err_str.size()==0 && strstr(type,"LEN_BIN")!=0){
        if (!in_node)
            err_str="IN node does not exist or definition error! ";
        
        //取得待解析 String
        string in_buff;
        this->DEBUGY(in_node);
        this->ParseDeep++;
        this->ParseValue(in_node,in_buff,add_paras);   //解析IN node
        this->ParseDeep--;
        
        //循环操作处理
        int i=0;
        for (;;){
            //get length and value_str
            size_t len=STRING_Helper::BinaryStrToUInt32(in_buff);
            if ((in_buff.size()==0)||((sizeof(UInt32)+len) > in_buff.size())){
                //cout << "Error(FOR): length out of range!";
                break;
            }
            string value_str=in_buff.substr(sizeof(UInt32),len);
            
            add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_VALUE_"+stack_num+"}"),value_str));
            add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_NUM_"+stack_num+"}"),this->_PubPacket->_iToStr((int)i,10) ));
            this->DEBUGY(actions_node);
            this->ParseDeep++;
            this->ParseAction(actions_node,add_paras); //ACTIONS 操作；
            this->ParseDeep--;
            add_paras.erase("{@_FOR_VALUE_"+stack_num+"}");
            add_paras.erase("{@_FOR_NUM_"+stack_num+"}");
            
            //next value
            in_buff.erase(0,sizeof(UInt32)+len);
            i++;
        }
        
    }
    else if (err_str.size()==0 && strstr(type,"CUT_LEN")!=0){
        if (!in_node)
            err_str="IN node does not exist or definition error! ";
        
        //取得待解析 String
        string in_buff;
        this->DEBUGY(in_node);
        this->ParseDeep++;
        this->ParseValue(in_node,in_buff,add_paras);   //解析IN node
        this->ParseDeep--;
        
        //循环操作处理
        int i=0;
        for (;;){
            //get length and value_str
            size_t len=STRING_Helper::BinaryStrToUInt32(in_buff);
            if ((in_buff.size()==0)||((sizeof(UInt32)+len) > in_buff.size())){
                //cout << "Error(FOR): length out of range!";
                break;
            }
            string value_str=in_buff.substr(sizeof(UInt32),len);
            
            add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_VALUE_"+stack_num+"}"),value_str));
            add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_NUM_"+stack_num+"}"),this->_PubPacket->_iToStr((int)i,10) ));
            this->DEBUGY(actions_node);
            this->ParseDeep++;
            this->ParseAction(actions_node,add_paras); //ACTIONS 操作；
            this->ParseDeep--;
            add_paras.erase("{@_FOR_VALUE_"+stack_num+"}");
            add_paras.erase("{@_FOR_NUM_"+stack_num+"}");
            
            //next value
            in_buff.erase(0,sizeof(UInt32)+len);
            i++;
        }
        
    }
	else if (err_str.size()==0 && strstr(type,"BEGIN_END")!=0){
        if (!in_node)
            err_str="IN node does not exist or definition error! ";
        //循环切分 String 的FOR方法.
        //取得begin end attribute
        string begin_str(this->_ActParseXml->NodeAttr(parse_node,"Begin"));
        string end_str(this->_ActParseXml->NodeAttr(parse_node,"End"));
        string begin; this->ParseValue(begin_str,begin,add_paras);
        string end; this->ParseValue(end_str,end,add_paras);
        if ((begin.size()==0) && ( end.size()== 0) ){
            err_str="Type='BEGIN_END' mode must have Begin、End attribute !";
            goto end;
        }
        
        //取得待解析 String
        string in_buff;
        this->DEBUGY(in_node);
        this->ParseDeep++;
        this->ParseValue(in_node,in_buff,add_paras);   //解析IN node
        this->ParseDeep--;
        
        //循环拆分待解析 String .
        vector<string> parse_field;
        STRING_Helper::stringSplit(in_buff,parse_field,end+begin);
        
        //循环操作处理
        string value_str;size_t begin_l=begin.size(); size_t end_l=end.size();
        for (size_t i=0; i < parse_field.size(); i++){
            //过滤头尾不规范字段
            
            if (i==0){
                //cout << parse_field[i] <<endl;
                if (parse_field[i].compare(0,begin_l,begin)!=0)
                    continue;
                if (parse_field.size()==1)
                    value_str=parse_field[i];
                else
                    value_str=parse_field[i]+end;
            }else if (i==parse_field.size()-1){//过滤头尾不规范字段
                if (parse_field[i].compare(parse_field[i].size()-end_l,end_l,end)!=0)
                    continue;//此处反复循环会出错!!
                value_str=begin+parse_field[i];
            }else
                value_str=begin+parse_field[i]+end;
            //提取相关 node 信息进行操作.
            //....value_str->add_para
            //判断是否存在就的FOR_NUM
            //string stack_num("");
            //PV_AddParas_T::iterator it;
            //for (it = add_paras.find((string)"{@_FOR_NUM_"+stack_num+"}"); it!=add_paras.end(); it = add_paras.find((string)"{@_FOR_NUM_"+stack_num+"}") ){
            //	if (stack_num=="")
            //		stack_num="1";
            //	else
            //		stack_num=this->_PubPacket->_iToStr((this->_PubPacket->_StrToi(stack_num)+1),10);
            //}
            add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_VALUE_"+stack_num+"}"),value_str));
            add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_NUM_"+stack_num+"}"),this->_PubPacket->_iToStr((int)i,10) ));
            this->DEBUGY(actions_node);
            this->ParseDeep++;
            this->ParseAction(actions_node,add_paras); //ACTIONS 操作；
            this->ParseDeep--;
            add_paras.erase("{@_FOR_VALUE_"+stack_num+"}");
            add_paras.erase("{@_FOR_NUM_"+stack_num+"}");
        }
    }
    else if (err_str.size()==0 && strstr(type,"SPLIT")!=0){
        if (!in_node)
            err_str="IN node does not exist or definition error! ";
        //取得 split attribute
        string split_str(this->_ActParseXml->NodeAttr(parse_node,"Split"));
        string split; this->ParseValue(split_str,split,add_paras);
        if (split.size()==0){
            err_str="Type='SPLIT' mode must have Split attribute !";
            goto end;
        }
        
        //取得待解析 String
        string in_buff;
        this->DEBUGY(in_node);
        this->ParseDeep++;
        this->ParseValue(in_node,in_buff,add_paras);   //解析IN node
        this->ParseDeep--;
        
        //循环拆分待解析 String .
        vector<string> parse_field;
        STRING_Helper::stringSplit(in_buff,parse_field,split);
        
        //循环操作处理
        for (size_t i=0; i < parse_field.size(); i++){
            add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_VALUE_"+stack_num+"}"),parse_field[i]));
            add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_NUM_"+stack_num+"}"),this->_PubPacket->_iToStr((int)i,10) ));
            this->DEBUGY(actions_node);
            this->ParseDeep++;
            this->ParseAction(actions_node,add_paras); //ACTIONS 操作；
            this->ParseDeep--;
            add_paras.erase("{@_FOR_VALUE_"+stack_num+"}");
            add_paras.erase("{@_FOR_NUM_"+stack_num+"}");
        }
    }
	else if (err_str.size()==0 && strstr(type,"XML_NODE_SET")!=0){
        //循环 node 的FOR方法.
        if (!in_node)
            err_str="IN node does not exist or definition error! ";
        //取得待解析 String
        string jpath;
        this->ParseValue(in_node,jpath,add_paras);
        
        //循环取得 node 集
        ParseXml_NodeSet nodes;
        pugi::xpath_node node;
        if (jpath.size()==0)
            err_str="<IN> node is empty,can't find xpath!";
        else if(!this->_PubObj->XmlChildNodes(jpath.c_str(),nodes))
            err_str=this->_PubObj->ErrorStr;
        else{
            //循环操作处理
            int i=1;
            for (ParseXml_NodeSet::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
            {
                node = *it;
                
                //add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_VALUE_"+stack_num+"}"),value_str));
                this->DEBUGY((string)"-"+jpath+"["+this->_PubPacket->_iToStr(i,10)+"]");
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_NODE_PATH_"+stack_num+"}"),jpath+"["+this->_PubPacket->_iToStr(i,10)+"]"));
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_NODE_VALUE_"+stack_num+"}"),this->_ActParseXml->NodeValue(node.node())));
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_NUM_"+stack_num+"}"),this->_PubPacket->_iToStr(i,10) ));
                this->ParseAction(actions_node,add_paras);
                add_paras.erase("{@_FOR_NODE_PATH_"+stack_num+"}");
                add_paras.erase("{@_FOR_NODE_VALUE_"+stack_num+"}");
                add_paras.erase("{@_FOR_NUM_"+stack_num+"}");
                i++;
            }
        }
    }
	else if (err_str.size() == 0 && strstr(type, "OBJ_NODE_SET") != 0) {
        //循环 node 的FOR方法.
        //if (!in_node)
        //err_str = "IN node does not exist or definition error! ";
        //取得待解析 String
        //取得LenStart LenLen
        string in_obj_tmp(this->_ActParseXml->NodeAttr(parse_node, "InObj"));
        string jpath;
        this->ParseValue(in_obj_tmp, jpath, add_paras);
        
        //循环取得 node 集
        ParseXml_NodeSet nodes;
        pugi::xpath_node node;
        vector<string> dest_vector;
        if (jpath.size() == 0)
            err_str = "<IN> node is empty,can't find xpath!";
        else if (this->_PubObj->GetAllSonNodeValue(jpath.c_str(), dest_vector) == Http_ParseObj::ERROR_POINT_NULL)
            err_str = (string)"Object Parent Node("+ jpath +") is NULL!";
        else {
            //循环操作处理
            int i = 1;
            //判断删除子节点。
            //string tmp_str;
            for (unsigned int j = 0; j < dest_vector.size();j++) {
                this->DEBUGY((string)"-" + jpath + "[" + this->_PubPacket->_iToStr(i, 10) + "]");
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_NODE_PATH_" + stack_num + "}"), jpath + "[" + this->_PubPacket->_iToStr(i, 10) + "]"));
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_NODE_VALUE_" + stack_num + "}"), dest_vector[j]));
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_FOR_NUM_" + stack_num + "}"), this->_PubPacket->_iToStr(i, 10)));
                this->ParseAction(actions_node, add_paras);
                add_paras.erase("{@_FOR_NODE_PATH_" + stack_num + "}");
                add_paras.erase("{@_FOR_NODE_VALUE_" + stack_num + "}");
                add_paras.erase("{@_FOR_NUM_" + stack_num + "}");
                i++;
            }
        }
    }
	else if (err_str.size()==0)
        err_str="The 'Type' property is not supported !";
    
    //返回差错信息
end:
    if(err_str!=""){
        this->SHOW_ERROR(err_str);
        this->ParseDeep--;
        return ParseProcess::ACT_ERROR;
    }else{
        this->ParseDeep--;
        return ParseProcess::ACT_SUCCESS;
    }
}

string& ParseProcess::ParseValueFunc(string &func_str, string &func_return, string &r_str,PV_AddParas_T &add_paras){
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

void ParseProcess::getHttpHeader(const char * header_name,string &header_content){
    header_content="";
}

ParseProcess::Http_ParseFunc::Http_ParseFunc(){
    //初始化:HttpParseProcess指针；
    //初始化:函数列表-指针.
    //this->FuncMap = new ParseFuncMap_T;
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"STR_LEN",ParseProcess::Http_ParseFunc::STR_LEN));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"ARRAY_LEN",ParseProcess::Http_ParseFunc::ARRAY_LEN));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"PUT",ParseProcess::Http_ParseFunc::PUT));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"GET",ParseProcess::Http_ParseFunc::GET));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"BARRAY_LEN",ParseProcess::Http_ParseFunc::BARRAY_LEN));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"BARRAY_UPDATE",ParseProcess::Http_ParseFunc::BARRAY_UPDATE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"BARRAY_INSERT",ParseProcess::Http_ParseFunc::BARRAY_INSERT));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"BARRAY_FIND",ParseProcess::Http_ParseFunc::BARRAY_FIND));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"BARRAY_EMPTY",ParseProcess::Http_ParseFunc::BARRAY_EMPTY));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"BARRAY_DEL",ParseProcess::Http_ParseFunc::BARRAY_DEL));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"OBJ_VALID",ParseProcess::Http_ParseFunc::OBJ_VALID));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"CONN_VALID",ParseProcess::Http_ParseFunc::CONN_VALID));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"OBJ_SON_NUM",ParseProcess::Http_ParseFunc::OBJ_SON_NUM));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"OBJ_SON_CLEAR",ParseProcess::Http_ParseFunc::OBJ_SON_CLEAR));
    //this->FuncMap.insert(pair<string, ParseFunc_T>((string )"NUM_LEN",ParseProcess::Http_ParseFunc::STR_LEN));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"XML_TO_STR",ParseProcess::Http_ParseFunc::XML_TO_STR));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"UTF8_TO_GB2312",ParseProcess::Http_ParseFunc::UTF8_TO_GB2312));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"GB2312_TO_UTF8",ParseProcess::Http_ParseFunc::GB2312_TO_UTF8));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"SAVE_FILE",ParseProcess::Http_ParseFunc::SAVE_FILE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"FILE_EXITS",ParseProcess::Http_ParseFunc::FILE_EXITS));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"FILE_DEL",ParseProcess::Http_ParseFunc::FILE_DEL));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"FILE_RENAME",ParseProcess::Http_ParseFunc::FILE_RENAME));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"FILE_MKDIR",ParseProcess::Http_ParseFunc::FILE_MKDIR));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"FILE_SAVE",ParseProcess::Http_ParseFunc::FILE_SAVE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"FILE_READ",ParseProcess::Http_ParseFunc::FILE_READ));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"FILE_DIR",ParseProcess::Http_ParseFunc::FILE_DIR));
	this->FuncMap.insert(pair<string, ParseFunc_T>((string )"FILE_DIR_EXT", ParseProcess::Http_ParseFunc::FILE_DIR_EXT));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"FILE_DIR_ALL",ParseProcess::Http_ParseFunc::FILE_DIR_ALL));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string) "FILE_READ_INFO", ParseProcess::Http_ParseFunc::FILE_READ_INFO));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string) "CHKSUM", ParseProcess::Http_ParseFunc::CHKSUM));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"FILE_READ_EXT", ParseProcess::Http_ParseFunc::FILE_READ_EXT));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"FILE_SAVE_EXT", ParseProcess::Http_ParseFunc::FILE_SAVE_EXT));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"PACKET_PARSE_XML",ParseProcess::Http_ParseFunc::PACKET_PARSE_XML));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"XML_PARSE_PACKET",ParseProcess::Http_ParseFunc::XML_PARSE_PACKET));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"GET_PARSE_TYPE",ParseProcess::Http_ParseFunc::GET_PARSE_TYPE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"BINARY_TO_HBC",ParseProcess::Http_ParseFunc::BINARY_TO_HBC));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"SUB_STR",ParseProcess::Http_ParseFunc::SUB_STR));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"SET_NUM_TO_ID", ParseProcess::Http_ParseFunc::SET_NUM_TO_ID));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"CUT_STR",ParseProcess::Http_ParseFunc::CUT_STR));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"POINT_TO_STR",ParseProcess::Http_ParseFunc::POINT_TO_STR));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"PARSE_FUNC",ParseProcess::Http_ParseFunc::PARSE_FUNC));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"EVAL",ParseProcess::Http_ParseFunc::EVAL));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"PARSE_LEN",ParseProcess::Http_ParseFunc::PARSE_LEN));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"PARSE_ENCODE",ParseProcess::Http_ParseFunc::PARSE_ENCODE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"SYS_GET_MAC",ParseProcess::Http_ParseFunc::SYS_GET_MAC));
#if !defined(__ANDROID__) && !defined(TARGET_OS_IPHONE) && !defined(TARGET_IPHONE_SIMULATOR)
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"SYS_CMD",ParseProcess::Http_ParseFunc::SYS_CMD));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"SYS_EXIT",ParseProcess::Http_ParseFunc::SYS_EXIT));
	this->FuncMap.insert(pair<string, ParseFunc_T>((string)"EXEC_CMD", ParseProcess::Http_ParseFunc::EXEC_CMD));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"POPEN", ParseProcess::Http_ParseFunc::POPEN));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"POPEN_READ", ParseProcess::Http_ParseFunc::POPEN_READ));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"POPEN_CLOSE", ParseProcess::Http_ParseFunc::POPEN_CLOSE));
#endif
#if !defined(__ANDROID__) && !defined(__APPLE__)
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"WEBSOCKET_ACCEPT_KEY",ParseProcess::Http_ParseFunc::WEBSOCKET_ACCEPT_KEY));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"WEBSOCKET_FRAME_LEN",ParseProcess::Http_ParseFunc::WEBSOCKET_FRAME_LEN));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"WEBSOCKET_FRAME_TYPE",ParseProcess::Http_ParseFunc::WEBSOCKET_FRAME_TYPE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"WEBSOCKET_FRAME_PAYLOAD",ParseProcess::Http_ParseFunc::WEBSOCKET_FRAME_PAYLOAD));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"WEBSOCKET_ENCODE_FRAME",ParseProcess::Http_ParseFunc::WEBSOCKET_ENCODE_FRAME));
    /*this->FuncMap.insert(pair<string, ParseFunc_T>((string )"BITDB_LEN_CRC",ParseProcess::Http_ParseFunc::BITDB_LEN_CRC));
     this->FuncMap.insert(pair<string, ParseFunc_T>((string )"BITDB_BUF_WRITE_TO",ParseProcess::Http_ParseFunc::BITDB_BUF_WRITE_TO));
     this->FuncMap.insert(pair<string, ParseFunc_T>((string )"BITDB_BUF_READ_FROM",ParseProcess::Http_ParseFunc::BITDB_BUF_READ_FROM));*/
#endif
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"CONN_OBJ_CREATE",ParseProcess::Http_ParseFunc::CONN_OBJ_CREATE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"CONN_OBJ_STATE",ParseProcess::Http_ParseFunc::CONN_OBJ_STATE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"DO_QUEUE_TABLE", ParseProcess::Http_ParseFunc::DO_QUEUE_TABLE));
    //this->FuncMap.insert(pair<string, ParseFunc_T>((string )"DO_QUEUE_SEND", ParseProcess::Http_ParseFunc::DO_QUEUE_SEND));
//    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"DO_QUEUE_RACT", ParseProcess::Http_ParseFunc::DO_QUEUE_RACT));
//    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"DO_QUEUE_SIZE", ParseProcess::Http_ParseFunc::DO_QUEUE_SIZE));
//    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"DO_QUEUE_ERR", ParseProcess::Http_ParseFunc::DO_QUEUE_ERR));
//    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"CHK_QUEUE_TIMEOUT", ParseProcess::Http_ParseFunc::CHK_QUEUE_TIMEOUT));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"ROUTING_CREAT_PACKET",ParseProcess::Http_ParseFunc::ROUTING_CREAT_PACKET));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"SUB_STR_NUM",ParseProcess::Http_ParseFunc::SUB_STR_NUM));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"STR_REPLACE",ParseProcess::Http_ParseFunc::STR_REPLACE));//STR_BIN_TO_CHAR
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"STR_BIN_TO_CHAR",ParseProcess::Http_ParseFunc::STR_BIN_TO_CHAR));//STR_BIN_TO_CHAR
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"SUB_STR_SPLIT",ParseProcess::Http_ParseFunc::SUB_STR_SPLIT));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"STR_SPLIT_FROM_TO",ParseProcess::Http_ParseFunc::STR_SPLIT_FROM_TO));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"STR_SPLIT_ARRAY",ParseProcess::Http_ParseFunc::STR_SPLIT_ARRAY));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"PLUS",ParseProcess::Http_ParseFunc::PLUS));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"MINUS",ParseProcess::Http_ParseFunc::MINUS));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"DIVI",ParseProcess::Http_ParseFunc::DIVI));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"MOD",ParseProcess::Http_ParseFunc::MOD));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"MULT",ParseProcess::Http_ParseFunc::MULT));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"ERR_CODE_STR", ParseProcess::Http_ParseFunc::ERR_CODE_STR));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"ERR_GET_LAST_ERROR", ParseProcess::Http_ParseFunc::ERR_GET_LAST_ERROR));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"CHECK_ACTION_QUERY", ParseProcess::Http_ParseFunc::CHECK_ACTION_QUERY));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"CHECK_CONN", ParseProcess::Http_ParseFunc::CHECK_CONN));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"IS_NUM", ParseProcess::Http_ParseFunc::IS_NUM));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"IS_TEXT", ParseProcess::Http_ParseFunc::IS_TEXT));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"TO_TEXT", ParseProcess::Http_ParseFunc::TO_TEXT));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"COUNTER", ParseProcess::Http_ParseFunc::COUNTER));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"HEX2IP", ParseProcess::Http_ParseFunc::HEX2IP));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"IP2HEX", ParseProcess::Http_ParseFunc::IP2HEX));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"POW",ParseProcess::Http_ParseFunc::POW));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"ROUND_TIME_STR",ParseProcess::Http_ParseFunc::ROUND_TIME_STR));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"IS_TIME_OUT",ParseProcess::Http_ParseFunc::IS_TIME_OUT));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"GET_NOW_DATE",ParseProcess::Http_ParseFunc::GET_NOW_DATE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"GET_NOW_TIME",ParseProcess::Http_ParseFunc::GET_NOW_TIME));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"HBC_TO_HBC",ParseProcess::Http_ParseFunc::HBC_TO_HBC));
    //this->FuncMap.insert(pair<string, ParseFunc_T>((string )"CHECK_CRC",ParseProcess::Http_ParseFunc::CHECK_CRC));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"HBC_TO_CRC",ParseProcess::Http_ParseFunc::HBC_TO_CRC));
#ifdef JAWS_CDBM
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"GET_XML_DB_DOC",ParseProcess::Http_ParseFunc::GET_XML_DB_DOC));
#endif
#ifdef SENSOR_DBM
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"GET_SENSOR_DB_DOC",ParseProcess::Http_ParseFunc::GET_SENSOR_DB_DOC));
#endif
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"IIF",ParseProcess::Http_ParseFunc::IIF));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"GET_HTTP_HEADER",ParseProcess::Http_ParseFunc::GET_HTTP_HEADER));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"NUMBER_TO_FLOAT",ParseProcess::Http_ParseFunc::NUMBER_TO_FLOAT));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"DEBUGY",ParseProcess::Http_ParseFunc::DEBUGY));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"SET_OBJ_VALUE",ParseProcess::Http_ParseFunc::SET_OBJ_VALUE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"GET_OBJ_VALUE",ParseProcess::Http_ParseFunc::GET_OBJ_VALUE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"GET_NODE_PATH",ParseProcess::Http_ParseFunc::GET_NODE_PATH));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"TEST_CONDITION",ParseProcess::Http_ParseFunc::TEST_CONDITION));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"MASK",ParseProcess::Http_ParseFunc::MASK));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"EQUALS",ParseProcess::Http_ParseFunc::EQUALS));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"FIND",ParseProcess::Http_ParseFunc::FIND));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"SEEK",ParseProcess::Http_ParseFunc::SEEK));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"AVTDB_BUF_WRITE_TO",ParseProcess::Http_ParseFunc::AVTDB_BUF_WRITE_TO));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"AVTDB_BUF_WRITE_TO_JENNIC",ParseProcess::Http_ParseFunc::AVTDB_BUF_WRITE_TO_JENNIC));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"AVTDB_FUNC",ParseProcess::Http_ParseFunc::AVTDB_FUNC));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"SESSION_FUNC", ParseProcess::Http_ParseFunc::SESSION_FUNC));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"AVTDB_BUF_READ_FROM",ParseProcess::Http_ParseFunc::AVTDB_BUF_READ_FROM));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"THIRD_RCV_MSG",ParseProcess::Http_ParseFunc::THIRD_RCV_MSG));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"THIRD_SEND_MSG",ParseProcess::Http_ParseFunc::THIRD_SEND_MSG));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"THIRD_FUNC",ParseProcess::Http_ParseFunc::THIRD_FUNC));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"CALL_FUNC",ParseProcess::Http_ParseFunc::CALL_FUNC));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"GB_PARAM_VALUE",ParseProcess::Http_ParseFunc::GB_PARAM_VALUE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"GET_CONN_PARAM",ParseProcess::Http_ParseFunc::GET_CONN_PARAM));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"CNT_CONN_PARAM",ParseProcess::Http_ParseFunc::CNT_CONN_PARAM));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string )"GB_PARAM_SET",ParseProcess::Http_ParseFunc::GB_PARAM_SET));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"HTTP_UNESCAPE", ParseProcess::Http_ParseFunc::HTTP_UNESCAPE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"HTTP_ESCAPE", ParseProcess::Http_ParseFunc::HTTP_ESCAPE));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"HTTP_GB2312ToUTF8", ParseProcess::Http_ParseFunc::HTTP_GB2312ToUTF8));
	this->FuncMap.insert(pair<string, ParseFunc_T>((string)"IS_UTF8", ParseProcess::Http_ParseFunc::IS_UTF8));
    this->FuncMap.insert(pair<string, ParseFunc_T>((string)"HTTP_UTF8ToGB2312", ParseProcess::Http_ParseFunc::HTTP_UTF8ToGB2312));
}

string ParseProcess::Http_ParseFunc::Do(const char *func_name,string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    ParseFuncMap_T::iterator func;
    //return this->FuncMap[func_name](func_field,func_return,parent);
    if ((func = this->FuncMap.find(func_name))!=this->FuncMap.end()){
        if (func_field.find("{")!=string::npos){//如果需要add_paras解析,例如@FUNC({}/***,****).
            string func_field1;
            parent->ParseValue(func_field,func_field1,add_paras);
            //return this->FuncMap[func_name](func_field1,func_return,parent,add_paras);
            return func->second(func_field1, func_return, parent, add_paras);
        }else{
            //return this->FuncMap[func_name](func_field,func_return,parent,add_paras);
            return func->second(func_field,func_return,parent,add_paras);
        }
        
    }else{
        return (string)"系统中没有@"+func_name+"定义";
    }
}

ParseProcess::Http_ParseFunc::~Http_ParseFunc(){
    //if (this->FuncMap!=0)
    //delete this->FuncMap;
}

string ParseProcess::Http_ParseFunc::OBJ_VALID(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    //vector<string> paras;
    //ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);

    //读取Param信息
    if (func_field==""){//Param数目错误
        return (string)"OBJ_VALID()parameter error ,Param1: Object " + func_field;
    }

    //
    parent->_PubObj->ErrorStr="";
    string tmp;
    if (!(parent->_PubObj->GetNodeValueStr(func_field.c_str(),tmp)))
    	func_return = "false";
    else
    	func_return = "true";

    return "";
}

string ParseProcess::Http_ParseFunc::CONN_VALID(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    //vector<string> paras;
    //ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    
    vector<string> paras;
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,1,parent,add_paras))<1){//Param数目错误
        return (string)"OBJ_SON_NUM()parameter error ,Param1: Object  or ' String '" + paras[0];
    }
    
    //取得CONN
    func_return="";
    Conn *conn;
    if ((conn=GB_ConnQueue->FindConnByObjName(paras[0].c_str(), 0,true))==NULL){
        func_return="false";
    }else
        func_return = "true";
    
    return "";
}

string ParseProcess::Http_ParseFunc::OBJ_SON_NUM(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,3,parent,add_paras))<1){//Param数目错误
        return (string)"OBJ_SON_NUM()parameter error ,Param1: Object  or ' String '" + paras[0];
    }
    //读取Param信息
    /*if (func_field==""){//Param数目错误
        return (string)"OBJ_SON_NUM()parameter error ,Param1: Object " + func_field;
    }
    string para0;
    if (STRING_Helper::PUB_FindStrIs(func_field,"\""))
        para0=STRING_Helper::strFastCut(para0,(string)"\"",(string)"\"",0);
    else if (STRING_Helper::PUB_FindStrIs(func_field,"\'"))
        para0=STRING_Helper::strFastCut(para0,(string)"'",(string)"'",0);
    else
        para0=func_field;*/

    int i=0;
    i=parent->_PubObj->GetAllSonNodeNum(paras[0].c_str());
    func_return = STRING_Helper::iToStr(i<=0 ? 0 :i);
    
    return "";
}
string ParseProcess::Http_ParseFunc::OBJ_SON_CLEAR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,2,parent,add_paras))<1){//Param数目错误
        return (string)"OBJ_SON_CLEAR()parameter error ,Param1: Object  or ' String '" + paras[0];
    }
    //读取Param信息
    /*if (func_field==""){//Param数目错误
        return (string)"OBJ_SON_CLEAR()parameter error ,Param1: Object " + func_field;
    }
    
    string para0;
    if (STRING_Helper::PUB_FindStrIs(func_field,"\""))
        para0=STRING_Helper::strFastCut(para0,(string)"\"",(string)"\"",0);
    else if (STRING_Helper::PUB_FindStrIs(func_field,"\'"))
        para0=STRING_Helper::strFastCut(para0,(string)"'",(string)"'",0);
    else
        para0=func_field;*/
    
    if (parent->_PubObj->Delete(paras[0].c_str(),true,false)==Http_ParseObj::SUCCESS)
        func_return = "true";
    else
        func_return = "false";
    
    return "";
}

string ParseProcess::Http_ParseFunc::ARRAY_LEN(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,3,parent,add_paras))<1){//Param数目错误
        return (string)"ARRAY_LEN()parameter error ,Param1: Object  or ' String ', param2 is hex, param3 UInt32/UInt16" + paras[0];
    }

    //计算长度
    char p[20];
    //itoa(strlen(temp),p,10);
    if (paras_num==1){
        itoa(paras[0].size(),p,10);
        func_return=p;
    }else if (paras[1]=="hex"){
        itoa(paras[0].size()/2,p,16);
        if (paras[2]=="UInt32")
            func_return=parent->_PubPacket->SetNumToID((string)p,4,"0");
        if (paras[2]=="UInt16")
            func_return=parent->_PubPacket->SetNumToID((string)p,2,"0");
        if (paras[2]=="UInt8")
            func_return=parent->_PubPacket->SetNumToID((string)p,1,"0");
    }
    return "";
}

string ParseProcess::Http_ParseFunc::PUT(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,4,parent,add_paras))<3){//Param数目错误
        return (string)"PUT()parameter error ,Param1: Object  or ' String ', param2 is hex, param3 UInt32/UInt16" + paras[0];
    }
    
    
    //取得被查询对象; 今后可以扩充为对变量的查询。
    if(STRING_Helper::PUB_FindStr(paras[0],"//")<0){//语法差错
        return (string)"PUT() paras[0] should be " + "//*./.' or //*/*/* !";
    }
    
    //取得查询字串
    //    BB_SEARCH_STRU  srh = BB_NEW_SEARCH_CELL(BB_SEARCH_NULL, paras[0].data(), paras[0].size());
    //    string search_str = string((const char*)&srh, BB_SEARCH_CELL_SIZE((BB_OBJ*)&srh));
    string search_str("");
    if (GB_AvartarDB->GetPathAllIndex(paras[0],search_str)) {
        return (string)"//*/*/*语法错误:" + paras[0];  //这个差错不适合于递归中的嵌套
    }
    
    BB_OBJ *obj;
    BB_SIZE_T obj_size;
    string root="";
    GB_AvartarDB->GetTableObject(root, &obj, obj_size);
    
    //通过search_st取得对象值。
    BB_OBJ *item=NULL;
    BB_SIZE_T item_size;
    if (!BB_GET_OBJ_BY_PATH_ALL((BB_OBJ *)obj, (BB_OBJ *)search_str.data(), (int)search_str.size(), &item, &item_size)) {
        return (string)"PUT() paras[0] {" + search_str + "} is error!";
    }
    
    //取得对象的可能的bit值
    int j,k,w;
    if ((paras[1].size()!=0) && (j=STRING_Helper::PUB_FindStr(paras[1],"b"))>=0 && (k=STRING_Helper::PUB_FindStr(paras[1],"%"))>0){
        //判断是否位操作
        if (j > k)
            return (string)"PUT() paras[2] should be b*% ！";
        string num_str=paras[1].substr(j+1,k-j-1);
        if (!STRING_Helper::CheckNum(num_str))
            return (string)"PUT() paras[2] should be b*%, * is num!";
        //取得写入bit：0-1
        int d=STRING_Helper::StrToi(paras[2]);
        if (d!=0 && d!=1)
            return (string)"PUT(b*%) paras[1] should 1|0!";
        //位写入操作
        int b_num=STRING_Helper::StrToi(num_str);
        int n=b_num%8;
        int m=b_num/8;
        if (((n==0) && (n>item_size)) || ((n!=0) && ((n+1)>item_size)))
            return (string)"PUT(b*%) paras[2] b*%, is too long!";
//      unsigned char *r=(n==0 ? item+m : item+m+1);   //取得更改字段
//        if (n==0){//当前字段最后一位
//            *r = (*r) & (0xff|);
//        }else{//其他位
//            unsigned char r2=d<7>>n;
//            *r = (*r) & r2;
//        }
        unsigned char *r=item+m;
        if (d==1)
            (*r)=(*r) | (0x80 >> n);
        else
            (*r)=(*r) & (~(0x80 >> n));
        int tmp=1+1;
    }else if ((paras[1].size()!=0) && (j=STRING_Helper::PUB_FindStr(paras[1],"c"))>=0 && (k=STRING_Helper::PUB_FindStr(paras[1],"%"))>0){//判断是否位操作
        if (j > k)
            return (string)"PUT() paras[1] should be c*% ！";
        string num_str=paras[1].substr(j+1,k-j-1);
        vector <string> nums;
        STRING_Helper::stringSplit(num_str,nums,"-");
        if (!STRING_Helper::CheckNum(nums[0]) || (nums.size()>1 && !STRING_Helper::CheckNum(nums[1])))
            return (string)"PUT() paras[1] should be c*-*%, * is num!";
        //位写入操作
        int n=STRING_Helper::StrToi(nums[0]);
        if (n>item_size)
            return (string)"PUT() paras[1] c*-*%, n out of item range!";
        if (nums.size()>1)
            w=STRING_Helper::StrToi(nums[1]);
        else
            w=1;
        if ((n+w)>item_size)
            return (string)"PUT() paras[1] c*-*%, w out of item range!";

        //byte 写入；
        string src((const char*)item+n,w);
        string dest("");
        if(STRING_Helper::PUB_FindStr(paras[2], "0x")==0){
            dest=STRING_Helper::HBCStrToHBCStr(paras[2], "hex", "char", true);
        }else
            dest=paras[2];
        if (w != dest.size())
            return (string)"PUT() paras[1] c*-*%, width should be equal to paras[2] byte width!";
        //byte 写入；
        BYTE *u=(BYTE *)item+n;
        for(int i=0; i<w; i++){
            *(u+i)=(BYTE)dest.at(i);
        }
    }else{
        if (paras[1].size()>item_size)
            return (string)"put(b*%) paras[1] is too long!";
        strncpy((char *)item,(char *)(paras[1].data()),paras[1].size());
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::GET(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,3,parent,add_paras))<1){//Param数目错误
        return (string)"ARRAY_LEN()parameter error ,Param1: Object  or ' String ', param2 is hex, param3 UInt32/UInt16" + paras[0];
    }
    
    
    //取得被查询对象; 今后可以扩充为对变量的查询。
    if(STRING_Helper::PUB_FindStr(paras[0],"//")<0){//语法差错
        return (string)"GET() paras[0] should be " + "//*./.' or //*/*/* !";
    }
    
    //取得查询字串
//    BB_SEARCH_STRU  srh = BB_NEW_SEARCH_CELL(BB_SEARCH_NULL, paras[0].data(), paras[0].size());
//    string search_str = string((const char*)&srh, BB_SEARCH_CELL_SIZE((BB_OBJ*)&srh));
    string search_str("");
    if (GB_AvartarDB->GetPathAllIndex(paras[0],search_str)) {
        return (string)"//*/*/*语法错误:" + paras[0];  //这个差错不适合于递归中的嵌套
    }

    BB_OBJ *obj;
    BB_SIZE_T obj_size;
    string root="";
    GB_AvartarDB->GetTableObject(root, &obj, obj_size);
    
    //通过search_st取得对象值。
    BB_OBJ *item=NULL;
    BB_SIZE_T item_size;
    if (!BB_GET_OBJ_BY_PATH_ALL((BB_OBJ *)obj, (BB_OBJ *)search_str.data(), (int)search_str.size(), &item, &item_size)) {
        return (string)"GET() paras[0] {" + search_str + "} is error!";
    }
    
    //取得对象的可能的bit值
    int j,k,w;
    if ((paras[1].size()!=0) && (j=STRING_Helper::PUB_FindStr(paras[1],"b"))>=0 && (k=STRING_Helper::PUB_FindStr(paras[1],"%"))>0){
        //判断是否位操作
        if (j > k)
            return (string)"GET() paras[1] should be b*% ！";
        string num_str=paras[1].substr(j+1,k-j-1);
        if (!STRING_Helper::CheckNum(num_str))
            return (string)"GET() paras[1] should be b*%, * is num!";
        //位写入操作
        int b_num=STRING_Helper::StrToi(num_str);
        int n=b_num%8;
        int m=b_num/8;
        if (((n==0) && (n>item_size)) || ((n!=0) && ((n+1)>item_size)))
            return (string)"GET(b*%) paras[2] b*%, is too long!";
        //位读取操
        unsigned char r=(n==0 ? item[m] : item[m+1]);   //取得更改字段
        if (n==0){//当前字段最后以为
			r = r & 1; //r & 0b00000001;
        }else{//其他位
            r=r<<(m)>>7;
        }
        func_return=STRING_Helper::uToStr(r);
    }else if ((paras[1].size()!=0) && (j=STRING_Helper::PUB_FindStr(paras[1],"c"))>=0 && (k=STRING_Helper::PUB_FindStr(paras[1],"%"))>0){//判断是否位操作
        if (j > k)
            return (string)"GET() paras[1] should be c*% ！";
        string num_str=paras[1].substr(j+1,k-j-1);
        vector <string> nums;
        STRING_Helper::stringSplit(num_str,nums,"-");
        if (!STRING_Helper::CheckNum(nums[0]) || (nums.size()>1 && !STRING_Helper::CheckNum(nums[1])))
            return (string)"GET() paras[1] should be c*-*%, * is num!";
        //位写入操作
        int n=STRING_Helper::StrToi(nums[0]);
        if (n>item_size)
            return (string)"GET(b*%) paras[1] c*-*%, n out of item range!";
        if (nums.size()>1)
            w=STRING_Helper::StrToi(nums[1]);
        else
            w=1;
        if ((n+w)>item_size)
            return (string)"GET(b*%) paras[1] c*-*%, w out of item range!";
        //位读取操
        string temp((const char*)item+n,w);
        func_return=STRING_Helper::HBCStrToHBCStr(temp, "char", "hex", true);
    }else{
        func_return.append((const char *)item,item_size);
    }
    
    return "";
};

string ParseProcess::Http_ParseFunc::BARRAY_LEN(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    func_return="";
    
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,3,parent,add_paras))<1){//Param数目错误
        return (string)"BB_ARRAY_LEN()parameter error ,Param1: Object  or ' String ', param2 is hex, param3 UInt32/UInt16" + paras[0];
    }
    
    //计算长度
    char p[20];
    int len=0;
    if (paras[0].size()==0){
        len=0; //ly task
    }else{
        BB_OBJ *obj=(BB_OBJ *)paras[0].data();
        BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
        if (obj_stru.LoadError){
            return "BB_ARRAY_LEN() object is not BARRAY!";
        }
        len=(int)BB_GET_ARRAY_NUM_STRU(&obj_stru);
    }
    
    //返回值处理
    if (paras_num==1){
        itoa(len,p,10);
        func_return=p;
    }else if (paras[1]=="hex"){
        itoa(len/2,p,16);
        if (paras[2]=="UInt32")
            func_return=parent->_PubPacket->SetNumToID((string)p,4,"0");
        if (paras[2]=="UInt16")
            func_return=parent->_PubPacket->SetNumToID((string)p,2,"0");
        if (paras[2]=="UInt8")
            func_return=parent->_PubPacket->SetNumToID((string)p,1,"0");
    }
    return "";
}

string ParseProcess::Http_ParseFunc::BARRAY_INSERT(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    func_return="";
    
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras))<3){//Param数目错误
        return (string)"BB_ARRAY_UPDATE()parameter error ,Param1: Object  or ' String ', param2 is hex, param3 UInt32/UInt16" + paras[0];
    }
    //取得参数
    BB_SIZE_T max=0,index_pos=0;
    if(paras.size()>=3 && (paras[2].size()!=0) && STRING_Helper::CheckNum(paras[2]))
        max=STRING_Helper::ParseInt(paras[2]);
    if(paras.size()>=4 && paras[3].size()!=0 && STRING_Helper::CheckNum(paras[3]))
        index_pos=STRING_Helper::ParseInt(paras[3]);
    
    if (index_pos >= max){
        return "BB_ARRAY_INSERT(src) object is out off range!";
    }
    //
    BB_SIZE_T i=0;
    string result="";
    BYTE cell_null = 0x0;
    bool has_add=false;
    BB_OBJ_STRU dest_stru=BB_GET_OBJ_STRU((BB_OBJ *)paras[1].data());
    
    //读的原始对象
    bool src_is_null=true;
    BB_OBJ_STRU src_stru;
    BB_OBJ *src_obj;
    if (paras[0].size()!=0){
        src_obj=(BB_OBJ *)paras[0].data();
        src_stru=BB_GET_OBJ_STRU(src_obj);
        if (src_stru.LoadError){
            return "BB_ARRAY_INSERT(src) object is not BARRAY!";
        }
        src_is_null=false;
    }
        
    
    //取得输入位置
    BB_OBJ *dest2;
    BB_SIZE_T dest2_len;
    if (dest_stru.LoadError || dest_stru.Type!=BB_ARRAY){//今后扩展为BB_ARRAY判断
        return "BB_ARRAY_INSERT(src[][]) cell is not BARRAY!";
    }
    if (index_pos >= dest_stru.Num)
        return ("BB_ARRAY_INSERT(src[][] is out of range("+STRING_Helper::iToStr(index_pos)+">"+STRING_Helper::iToStr(dest_stru.Num)+")!");
    if (!BB_GET_ARRAY_REC_BY_NUM_STRU(&dest_stru,index_pos,&dest2, &dest2_len))
        return ("BB_ARRAY_INSERT(src[][] error!");
    string pos_cell=string((const char *)dest2,dest2_len);
    int pos=STRING_Helper::ParseInt(pos_cell);
        
    //插入记录
    for(i=0; i<max; i++){
        //取得目标比较元素
        if (i==pos){
            //替换内容;
            result+=paras[1];
            has_add=true;
            continue;
        }
        //取得每条记录
        if (src_is_null){//空数组情况
            string str_null="";
            BB_OBJ *p_obj;
            p_obj=BB_NEW_ARRAY(&cell_null, NULL, NULL, 0, (void *)str_null.data(), str_null.size(), NULL);;
            result+=string((const char *)p_obj,BB_GET_OBJ_SIZE(p_obj));
            free(p_obj);
        }else{//非空数组
            BB_OBJ *src1;
            BB_SIZE_T src1_len;
            BB_GET_ARRAY_REC_BY_NUM_STRU(&src_stru, i, &src1, &src1_len);//此处可以提高效率
            result+=string((const char*) src1, src1_len);        }
    }
    
    //创建新外壳字串数组
    BB_OBJ *dest_obj = BB_NEW_ARRAY(&cell_null, NULL, NULL, 0, (void *)result.data(), result.size(), NULL);
    func_return=string((const char*) dest_obj, BB_GET_OBJ_SIZE(dest_obj));
    free(dest_obj);
    return "";
}

string ParseProcess::Http_ParseFunc::BARRAY_EMPTY(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    func_return="";
    
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,3,parent,add_paras))<1){//Param数目错误
        return (string)"BB_ARRAY_FIND()parameter error ,Param1: Object  or ' String ', param2 is hex, param3 UInt32/UInt16" + paras[0];
    }
    //取得参数
    BB_SIZE_T null_pos=BB_SIZE_T_ERROR;
    if(paras.size()>=2 && paras[1].size()!=0 && STRING_Helper::CheckNum(paras[1]))
        null_pos=STRING_Helper::ParseInt(paras[1]);
    //
    BB_SIZE_T i=0,end_len=0;
    //string result="";
    if (paras[0].size()!=0){//如果来源数组不为空
        //读的原始对象
        BB_OBJ *src_obj=(BB_OBJ *)paras[0].data();
        BB_OBJ_STRU src_stru=BB_GET_OBJ_STRU(src_obj);
        if (src_stru.LoadError){
            return "BB_ARRAY_UPDATE(src) object is not BARRAY!";
        }
        //插入记录
        for(i=0; i<src_stru.Num; i++){
            //取得每条记录
            BB_OBJ *src1;
            BB_SIZE_T src1_len;
            BB_GET_ARRAY_REC_BY_NUM_STRU(&src_stru, i, &src1, &src1_len);//此处可以提高效率
            
            //取得源比较元素
            BB_OBJ_STRU src1_stru=BB_GET_OBJ_STRU(src1);
            if (src1_stru.LoadError || src1_stru.Type!=BB_ARRAY){//今后扩展为BB_ARRAY判断
                return "BB_ARRAY_FIND(src[][]) cell is not BARRAY!";
            }
            if (src1_stru.DataLen==0){//全为空的位置
                func_return=STRING_Helper::uToStr(i);
                break;
            }else if (null_pos!=BB_SIZE_T_ERROR){//指定字段为空的位置
                //取得源比较元素
                BB_OBJ *src2;
                BB_SIZE_T src2_len;
                //BB_OBJ_STRU src1_stru=BB_GET_OBJ_STRU(src1);
                if (src1_stru.LoadError || src1_stru.Type!=BB_ARRAY){//今后扩展为BB_ARRAY判断
                    return "BB_ARRAY_EMPTY(src[][]) cell is not BARRAY!";
                }
                BB_GET_ARRAY_REC_BY_NUM_STRU(&src1_stru,null_pos,&src2, &src2_len);
                if (src2_len==0){
                    func_return=STRING_Helper::uToStr(i);
                    break;
                }
            }
            
            //再添加
            if (i==(src_stru.Num-1))
                end_len=src1_len;
        }
    }else
        func_return=STRING_Helper::uToStr(0);
    if (func_return.size()==0)
        return "BB_ARRAY_EMPTY(src[][]) BARRAY is full!";
    else
        return "";
}

string ParseProcess::Http_ParseFunc::BARRAY_FIND(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    func_return="";
    
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras))<3){//Param数目错误
        return (string)"BB_ARRAY_FIND()parameter error ,Param1: Object  or ' String ', param2 is hex, param3 UInt32/UInt16" + paras[0];
    }
    //取得参数
    BB_SIZE_T return_pos=0xffff,comp_pos=0xffff;
    if(paras.size()>=3 && (paras[2].size()!=0) && STRING_Helper::CheckNum(paras[2]))
        comp_pos=STRING_Helper::ParseInt(paras[2]);
    if(paras.size()>=4 && paras[3].size()!=0 && STRING_Helper::CheckNum(paras[3]))
        return_pos=STRING_Helper::ParseInt(paras[3]);
    //
    BB_SIZE_T i=0,end_len=0;
    string result="";
    
    if (paras[1].size()==0){//今后扩展为BB_ARRAY判断
        return "BB_ARRAY_FIND(dest) is Empty!";
    }
    
    if (paras[0].size()!=0){//如果来源数组不为空
        //读的原始对象
        BB_OBJ *src_obj=(BB_OBJ *)paras[0].data();
        BB_OBJ_STRU src_stru=BB_GET_OBJ_STRU(src_obj);
        if (src_stru.LoadError){
            return "BB_ARRAY_UPDATE(src) object is not BARRAY!";
        }
        //插入记录
        for(i=0; i<src_stru.Num; i++){
            //取得每条记录
            BB_OBJ *src1;
            BB_SIZE_T src1_len;
            BB_GET_ARRAY_REC_BY_NUM_STRU(&src_stru, i, &src1, &src1_len);//此处可以提高效率
            
            //判断内容相似性
            string src_cell;
            if (comp_pos==0xffff){
                //全数组比较
                src_cell=string((const char *)src1,src1_len);
                if (src_cell==paras[1]){//原版返回
                    func_return=paras[0];
                    return "";
                }
            }else{
                //取得源比较元素
                BB_OBJ *src2;
                BB_SIZE_T src2_len;
                BB_OBJ_STRU src1_stru=BB_GET_OBJ_STRU(src1);
                if (src1_stru.LoadError || src1_stru.Type!=BB_ARRAY){//今后扩展为BB_ARRAY判断
                    return "BB_ARRAY_FIND(src[][]) cell is not BARRAY!";
                }
                if (src1_stru.DataLen==0)
                    continue;
                BB_GET_ARRAY_REC_BY_NUM_STRU(&src1_stru,comp_pos,&src2, &src2_len);
                //比较
                if (src2_len==paras[1].size() && memcmp((const char *)src2,(const char *)paras[1].c_str(),src2_len)==0){
                    //替换内容;
                    if (return_pos==0xffff){
                        func_return=STRING_Helper::uToStr(i);
                    }
                }
            }
            //再添加
            if (i==(src_stru.Num-1))
                end_len=src1_len;
        }
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::BARRAY_UPDATE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    func_return="";
    
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras))<3){//Param数目错误
        return (string)"BB_ARRAY_UPDATE()parameter error ,Param1: Object  or ' String ', param2 is hex, param3 UInt32/UInt16" + paras[0];
    }
    //取得参数
    BB_SIZE_T max=0,comp_pos=0xffff;
    if(paras.size()>=3 && (paras[2].size()!=0) && STRING_Helper::CheckNum(paras[2]))
        max=STRING_Helper::ParseInt(paras[2]);
    if(paras.size()>=4 && paras[3].size()!=0 && STRING_Helper::CheckNum(paras[3]))
        comp_pos=STRING_Helper::ParseInt(paras[3]);
    bool replace_null=false;
    if (paras[4]=="true")
        replace_null=true; //空值replace
    //
    BB_SIZE_T i=0,end_len=0;
    string result="";
    BYTE cell_null = 0x0;
    bool has_add=false;
    BB_OBJ_STRU dest_stru=BB_GET_OBJ_STRU((BB_OBJ *)paras[1].data());
    
    if (paras[0].size()!=0){//如果来源数组不为空
        //读的原始对象
        BB_OBJ *src_obj=(BB_OBJ *)paras[0].data();
        BB_OBJ_STRU src_stru=BB_GET_OBJ_STRU(src_obj);
        if (src_stru.LoadError){
            return "BB_ARRAY_UPDATE(src) object is not BARRAY!";
        }
        //插入记录
        for(i=0; i<src_stru.Num; i++){
            //取得每条记录
            BB_OBJ *src1;
            BB_SIZE_T src1_len;
            BB_GET_ARRAY_REC_BY_NUM_STRU(&src_stru, i, &src1, &src1_len);//此处可以提高效率
            
            //判断内容相似性
            string src_cell;
            if (comp_pos==0xffff){
                //全数组比较
                src_cell=string((const char *)src1,src1_len);
                if (src_cell==paras[1]){//原版返回
                    func_return=paras[0];
                    return "";
                }
            }else{
                //取得源比较元素
                BB_OBJ *src2;
                BB_SIZE_T src2_len;
                BB_OBJ_STRU src1_stru=BB_GET_OBJ_STRU(src1);
                if (src1_stru.LoadError || src1_stru.Type!=BB_ARRAY){//今后扩展为BB_ARRAY判断
                    return "BB_ARRAY_UPDATE(src[][]) cell is not BARRAY!";
                }
                BB_GET_ARRAY_REC_BY_NUM_STRU(&src1_stru,comp_pos,&src2, &src2_len);
                //src_cell=string((const char *)src2,src2_len);
                //取得目标比较元素
                BB_OBJ *dest2;
                BB_SIZE_T dest2_len;
                if (dest_stru.LoadError || dest_stru.Type!=BB_ARRAY){//今后扩展为BB_ARRAY判断
                    return "BB_ARRAY_UPDATE(src[][]) cell is not BARRAY!";
                }
                BB_GET_ARRAY_REC_BY_NUM_STRU(&dest_stru,comp_pos,&dest2, &dest2_len);
                //src_cell=string((const char *)dest2,dest2_len);
                //比较
                if ((src2_len==dest2_len && memcmp((const char *)src2,(const char *)dest2,src2_len)==0)||(replace_null && src2_len==0)){
                    //替换内容;
                    result.insert(0,paras[1]);
                    has_add=true;
                    continue;
                }
            }
            //再添加
            result+=string((const char*) src1, src1_len);
            if (i==(src_stru.Num-1))
                end_len=src1_len;
        }
    }else
        i++;
    //添加新记录
    if (!has_add){//此处可以提速！
        result.insert(0,paras[1]);
    }
    //限制，先入先出:去掉头记录
    if (max!=0 && i>=max)
        result.erase(result.size()-end_len,end_len);
    //创建新外壳字串数组
    BB_OBJ *dest_obj = BB_NEW_ARRAY(&cell_null, NULL, NULL, 0, (void *)result.data(), result.size(), NULL);
    func_return=string((const char*) dest_obj, BB_GET_OBJ_SIZE(dest_obj));
    free(dest_obj);
    return "";
}



string ParseProcess::Http_ParseFunc::BARRAY_DEL(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    func_return="";
    
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras))<3){//Param数目错误
        return (string)"BB_ARRAY_DEL()parameter error ,Param1: Object  or ' String ', param2 is hex, param3 UInt32/UInt16" + paras[0];
    }
    //取得参数
    BB_SIZE_T max=0,comp_pos=0xffff;
    if(paras.size()>=3 && (paras[2].size()!=0) && STRING_Helper::CheckNum(paras[2]))
        max=STRING_Helper::ParseInt(paras[2]);
    if(paras.size()>=4 && paras[3].size()!=0 && STRING_Helper::CheckNum(paras[3]))
        comp_pos=STRING_Helper::ParseInt(paras[3]);
    //
    BB_SIZE_T i=0,end_len=0;
    string result="";
    BYTE cell_null = 0x0;
    bool has_del=false;
    BB_OBJ_STRU dest_stru=BB_GET_OBJ_STRU((BB_OBJ *)paras[1].data());
    
    if (paras[0].size()!=0){//如果来源数组不为空
        //读的原始对象
        BB_OBJ *src_obj=(BB_OBJ *)paras[0].data();
        BB_OBJ_STRU src_stru=BB_GET_OBJ_STRU(src_obj);
        if (src_stru.LoadError){
            return "BB_ARRAY_UPDATE(src) object is not BARRAY!";
        }
        //插入记录
        for(i=0; i<src_stru.Num; i++){
            //取得每条记录
            BB_OBJ *src1;
            BB_SIZE_T src1_len;
            BB_GET_ARRAY_REC_BY_NUM_STRU(&src_stru, i, &src1, &src1_len);//此处可以提高效率
            
            //判断内容相似性
            string src_cell;
            if (comp_pos==0xffff){
                //全数组比较
                src_cell=string((const char *)src1,src1_len);
                if (src_cell==paras[1]){//原版返回
                    //替换内容;
                    has_del=true;
                    continue;
                }
            }else{
                //取得源比较元素
                BB_OBJ *src2;
                BB_SIZE_T src2_len;
                BB_OBJ_STRU src1_stru=BB_GET_OBJ_STRU(src1);
                if (src1_stru.LoadError || src1_stru.Type!=BB_ARRAY){//今后扩展为BB_ARRAY判断
                    return "BB_ARRAY_UPDATE(src[][]) cell is not BARRAY!";
                }
                BB_GET_ARRAY_REC_BY_NUM_STRU(&src1_stru,comp_pos,&src2, &src2_len);
                //src_cell=string((const char *)src2,src2_len);
                //取得目标比较元素
                BB_OBJ *dest2;
                BB_SIZE_T dest2_len;
                if (dest_stru.LoadError || dest_stru.Type!=BB_ARRAY){//今后扩展为BB_ARRAY判断
                    return "BB_ARRAY_UPDATE(src[][]) cell is not BARRAY!";
                }
                BB_GET_ARRAY_REC_BY_NUM_STRU(&dest_stru,comp_pos,&dest2, &dest2_len);
                //比较
                if (src2_len==dest2_len && memcmp((const char *)src2,(const char *)dest2,src2_len)==0){
                    //替换内容;
                    has_del=true;
                    continue;
                }
            }
            //再添加
            result+=string((const char*) src1, src1_len);
            if (i==(src_stru.Num-1))
                end_len=src1_len;
        }
    }else
        i++;

    //创建新外壳字串数组
    BB_OBJ *dest_obj = BB_NEW_ARRAY(&cell_null, NULL, NULL, 0, (void *)result.data(), result.size(), NULL);
    func_return=string((const char*) dest_obj, BB_GET_OBJ_SIZE(dest_obj));
    free(dest_obj);
    return "";
}

string ParseProcess::Http_ParseFunc::STR_LEN(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    
    
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,3,parent,add_paras))<1){//Param数目错误
        return (string)"STR_LEN()parameter error ,Param1: Object  or ' String ', param2 is hex, param3 UInt32/UInt16" + paras[0];
    }
    
    //计算长度
    char p[20];
    //itoa(strlen(temp),p,10);
    if (paras_num==1){
        itoa(paras[0].size(),p,10);
        func_return=p;
    }else if (paras[1]=="hex"){
        itoa(paras[0].size()/2,p,16);
        if (paras[2]=="UInt32")
            func_return=parent->_PubPacket->SetNumToID((string)p,4,"0");
        if (paras[2]=="UInt16")
            func_return=parent->_PubPacket->SetNumToID((string)p,2,"0");
        if (paras[2]=="UInt8")
            func_return=parent->_PubPacket->SetNumToID((string)p,1,"0");
    }
    return "";
}

//string ParseProcess::Http_ParseFunc::NUM_LEN(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
//	vector<string> paras;
//	ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
//	if (paras.size()!=2){//Param数目错误
//		return (string)"NUM_LEN()parameter error ,Param1: Object  or ' String ; Param2:hex|bit'";
//	}
//	//取得Param一
//	const char *temp;
//	const char *temp2;
//	string temp_s,temp_s2;
//
//	if (CheckParaQuot(paras[0],temp_s)){
//		// String Param
//		temp=temp_s.c_str();
//	}else{
//		// Object Param
//		temp= parent->_PubObj->GetNodeValue(paras[0].c_str());
//		if (temp==NULL)
//			return parent->_PubObj->ErrorStr;
//	}
//	if (CheckParaQuot(paras[1],temp_s2)){
//		// String Param
//		temp2=temp_s2.c_str();
//	}else{
//		// Object Param
//		temp2= parent->_PubObj->GetNodeValue(paras[1].c_str());
//		if (temp2==NULL)
//			return parent->_PubObj->ErrorStr;
//	}
//
//	//计算长度
//	char p[20];
//	itoa(strlen(parent->_PubPacket->BinaryToHBCStr(temp,atoi(para2),para3,false);),p,10);
//	func_return=p;
//	return "";
//}

string ParseProcess::Http_ParseFunc::XML_TO_STR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    //计算 String  Length 函数.{@STR_LEN({*******})}
    vector<string> paras;
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
	//const char *temp1_p;
    string temp1,temp2,temp3,temp4;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()<1){//Param数目错误
        return (string)"@XML_TO_STR()Param1错误:Param1: Source  Object ,Param2:无->now node , 'CHILDS'->all sub node ,Param3:Filter fields";
    }
    //取得Param一
    if (CheckParaQuot(paras[0],temp1)){
        // String Param,带引号Param为“node node 转化的xml node ”,临时作用,今后再改进
        if (temp1!=""){
            const char *temp2_p=parent->_PubObj->GetNodeValue(temp1.c_str());
			if (parent->_PubObj->ErrorStr.size()!=0)
                return parent->_PubObj->ErrorStr;
			temp2 = temp2_p;
            temp1 = temp2;
        }
    }else{
        // Object Param
        temp1=paras[0];
    }
    //取得Param二
    if (paras.size()>=2){
        if (CheckParaQuot(paras[1],temp2)){
            // String Param
            STRING_Helper::strTrim(temp2);
        }else{
            // Object Param
            parent->_PubObj->ErrorStr="@PACKET_PARSE_XML()Param2 error:Param1: Source  Object ,Param2:无->当前 node , 'CHILDS'->所有 sub node ,Param3:Filter过滤字段;";
            return parent->_PubObj->ErrorStr;
        }
    }
    
    //取得Param三
    if (paras.size()==3){
        if (CheckParaQuot(paras[2],temp3)){
            // String Param
            STRING_Helper::strTrim(temp3);
        }else{
            // Object Param
            char *p3 =(char *)parent->_PubObj->GetNodeValue(paras[2].c_str());
            if (p3==NULL)
                return parent->_PubObj->ErrorStr;
            else
                temp3=p3;
        }
    }
    
    //函数操作
    if (temp2.find("FIELDS")==0){
        // node 名符合列表的子记录
        vector <string> fields;
        STRING_Helper::stringSplit(temp3,fields,";");
        parent->_PubObj->XmlChildNodesText(temp1.c_str(),func_return,"",fields);
    }else if (temp2=="CHILDS" && temp3.size()!=0){
        // node 名符合列表的子记录
        vector <string> fields;
        int k=0;
        string equal="",act=" or ";
        
        /*if (temp3.size()!=0){*/
        if (temp3.compare(0,1,"!")==0){//最前面有!,则针对不等于的过滤
            equal="!";k=1;act=" and ";
        }
        string temp3_1 = temp3.substr(k);
        STRING_Helper::stringSplit(temp3_1,fields,";");
        //}else{
        //	if (temp2.compare(7,1,"!")==0){//最前面有!,则针对不等于的过滤
        //		equal="!";k=1;act=" and ";
        //	}
        //	STRING_Helper::stringSplit(temp2.substr(7+k),fields,";");
        //}
        size_t i;
        for(i=0; i<fields.size(); i++){
            if (i==0)
                temp1+=string("[")+"name()"+equal+"='"+fields[i]+"'";
            else
                temp1+=string(act+"name()")+equal+"='"+fields[i]+"'";
        }
        if (i>0)
            temp1+="]";
        parent->_PubObj->XmlChildNodesText(temp1.c_str(),func_return,"");
    }else if (paras.size()==2 && temp2=="CHILDS"){
        parent->_PubObj->XmlChildNodesText(paras[0].c_str(),func_return,"");
    }else if (paras.size()==3 && temp2=="CHILDS_TEXT"){
        parent->_PubObj->XmlChildNodesText(paras[0].c_str(),func_return,temp3.c_str());
    }else if (temp2.find("ATTR:")==0){
        parent->_PubObj->XmlAttrText(temp1.c_str(),temp2.substr(5),func_return);
	}
	else if (temp2.find("VALUE") == 0) {
		const char *p1=parent->_PubObj->GetNodeValue(temp1.c_str());
		if (p1 != NULL)
			func_return = p1;
		else {
			func_return = "";
			return parent->_PubObj->ErrorStr;
		}
	}  
    else
        parent->_PubObj->XmlNodeText(temp1.c_str(),func_return);
    
    //返回值判断
    if (func_return.size()==0){
        parent->SHOW_ERROR(parent->_PubObj->ErrorStr);
        return parent->_PubObj->ErrorStr;	//返回差错信息
    }else{
        return "";	//返回成功
    }
}

string ParseProcess::Http_ParseFunc::UTF8_TO_GB2312(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    const char *temp;
    string temp_s;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()!=1){//Param数目错误
        return (string)"UTF8_TO_GB2312()parameter error ,Param1: Object  or ' String '";
    }
    if (paras.size()==1 && CheckParaQuot(paras[0],temp_s)){
        // String Param
        temp=temp_s.c_str();
    }else{
        // Object Param
        temp= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (temp==NULL)
            return parent->_PubObj->ErrorStr;
    }
    
    //实际操作
#ifdef NO_CCODE
    func_return = temp;
#else
    CChineseCode::UTF_8ToGB2312(func_return, temp);
#endif
    return "";
}
string ParseProcess::Http_ParseFunc::GB2312_TO_UTF8(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    string temp_s;
    const char *temp;
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()!=1){//Param数目错误
        return (string)"GB2312_TO_UTF8()parameter error ,Param1: Object  or ' String '";
    }
    if (paras.size()==1 && CheckParaQuot(paras[0],temp_s)){
        // String Param
        temp=temp_s.c_str();
    }else{
        // Object Param
        temp= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (temp==NULL)
            return parent->_PubObj->ErrorStr;
    }
    
    //实际操作
#ifdef NO_CCODE
    func_return = temp;
#else
    CChineseCode::GB2312ToUTF_8(func_return, temp); //今后增加
#endif
    return "";
}

string ParseProcess::Http_ParseFunc::SAVE_FILE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    //vector<string> paras;
    string temp_s;
    string filen_s;
    //ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    vector<string> paras;
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,4,parent,add_paras))<2){//Param数目错误
        return (string)"SAVE_FILE() parameter error ,Param1:file name,Param2:contents:,Param3: is_append " + paras[0];
    }
    
    parent->savefiles(paras[0].c_str(),paras[1].data(),paras[1].size(),(paras[2]=="true" || paras[2]=="append") ? true : false, (paras[3] == "txt") ? false : true);
    return "";
}
string ParseProcess::Http_ParseFunc::FILE_SAVE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    
    func_return="";
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,6,parent,add_paras))<2){//Param数目错误
        return (string)"FILE_SAVE()parameter error ,Param1-filename, param2-content, param3-append, para4-binary" + paras[0];
    }
    //check_dir
    if (paras[5]=="CREATE_DIR"){
        //查询上层目录是否存在，不存在则创建。
        parent->CreatDirExt(paras[0].c_str(),"/");
    }
     
    //存文件操作
    if (paras[0].size()==0)
        return (string)"FILE_SAVE()parameter error ,Param1: Object  or ' String '" + paras[0];
    if (paras[4]=="add_son"){
        string data="";
        parent->_PubObj->AddChildNodesValue(paras[1].c_str(),data); //取得所有子节点信息，累加；
        //cout << STRING_Helper::HBCStrToHBCStr(data, "char", "hex", true) << endl;
        if (parent->savefiles(paras[0].c_str(),data.data(),data.size(),(paras[2]=="true" || paras[2]=="append") ? true : false, (paras[3] == "txt") ? false : true)<0){
            return "error writing in:"+paras[0];
        }
        func_return=STRING_Helper::uToStr(data.size());
    }if (paras[4]=="add_array"){
        //取得数组内容
        BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU((BB_OBJ *)(paras[1].c_str()));
        string data="";
        if (!GB_AvartarDB->GetArrayRecsAddStr(obj_stru,data))
            return "Param1 is not Array!";
        if (paras[5]=="DEBUG")
            cout << STRING_Helper::HBCStrToHBCStr(data, "char", "hex", true) << endl;
        if (parent->savefiles(paras[0].c_str(),data.data(),data.size(),(paras[2]=="true" || paras[2]=="append") ? true : false, (paras[3] == "txt") ? false : true)<0){
            return "error writing in:"+paras[0];
        }
        func_return=STRING_Helper::uToStr(data.size());
    }else{
        if (parent->savefiles(paras[0].c_str(),paras[1].data(),paras[1].size(),(paras[2]=="true" || paras[2]=="append") ? true : false, (paras[3] == "txt") ? false : true)<0){
            return "error writing in:"+paras[0];
        }
        func_return=STRING_Helper::uToStr(paras[1].size());
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::FILE_EXITS(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,1,parent,add_paras))<1){//Param数目错误
        return (string)"FILE_EXITS()parameter error ,Param1:file names " + paras[0];
    }
    
    //存文件操作
    //临时文件名存储操作
    /*std::fstream file(paras[0].c_str(),ios::in|ios::binary);
    //std::ifstream file(paras[0].c_str());
    if(file.good())
    {
        file.close();
        func_return="true";
    }else
        func_return = "false";*/
	//table_dir
	if (LW_access(paras[0].c_str(), 0) < 0)
		func_return = "false";
	else
		func_return = "true";
    return "";
}

string ParseProcess::Http_ParseFunc::FILE_DEL(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,1,parent,add_paras))<1){//Param数目错误
        return (string)"FILE_DEL()parameter error ,Param1:file names " + paras[0];
    }
    //文件是否存在。
    if (LW_access(paras[0].c_str(),0)<0)
        return "file not exited!";
    
    //删除操作
    int i=LW_del(paras[0].c_str());
    func_return =STRING_Helper::iToStr(i);
    
    return "";
}


string ParseProcess::Http_ParseFunc::FILE_RENAME(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}s
    vector<string> paras;
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,3,parent,add_paras))<2){//Param数目错误
        return (string)"FILE_RENAME()parameter error ,Param1:src name, Param2:dest name,Paras3:DEL_OLD " + paras[0];
    }
    //文件是否存在。
    func_return="";
    if (LW_access(paras[0].c_str(),0)<0)
        return "file not exited!";
    //删除旧文件。
    if ((LW_access(paras[1].c_str(),0)>=0) && (paras[2]=="DEL_OLD"))
        LW_del(paras[1].c_str());
    //改名操作
    int i=LW_rename(paras[0].c_str(),paras[1].c_str());
    func_return =STRING_Helper::iToStr(i);

    if (i!=0)
        return (string)"@FILE_RENAME("+paras[0]+","+paras[1]+") error!";
    else
        return "";
}

string ParseProcess::Http_ParseFunc::FILE_MKDIR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,2,parent,add_paras))<1){//Param数目错误
        return (string)"FILE_RENAME()parameter error ,Param1:src name, Param2:dest name,Paras3:DEL_OLD " + paras[0];
    }
    //文件是否存在。
    func_return="";
    if (LW_access(paras[0].c_str(),0)>=0)
        return "file already exited!";

    //改名操作
    int i=LW_mkdir(paras[0].c_str());
    func_return =STRING_Helper::iToStr(i);
    
    if (i!=0)
        return (string)"@FILE_MKDIR("+paras[0]+","+paras[1]+") error!";
    else
        return "";
}


string ParseProcess::Http_ParseFunc::FILE_READ(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    int paras_num;
    BB_SIZE_T start = 0, length = 0;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,4,parent,add_paras))<1){//Param数目错误
        return (string)"FILE_READ()parameter error ,Param1:file names " + paras[0];
    }
    
    //取得start leng
    if (!STRING_Helper::CheckNum(paras[2]) || !STRING_Helper::CheckNum(paras[3])) {
        return (string)"FILE_READ()parameter error: param[2], paras[3] should be num!";
    }
    if (paras[2].size() != 0 && paras[3].size() != 0) {
        start = STRING_Helper::ParseInt(paras[2]);
        length = STRING_Helper::ParseInt(paras[3]);
    }
	
    
    //存文件操作
    func_return = "";
	string rt = "";
	BB_SIZE_T fl = 0;
	//BB_SIZE_T len = 0;
	if (paras[0].size() != 0) {
		if (LW_access(paras[0].c_str(), 0) < 0)
			return "FILE_READ(), file:" + paras[0] + " not found!";//判断文件是否已经存在

		//paras[1]可以做bin text操作。
		ios_base::openmode mod = ios::in | ios::out | ios::binary;
		if (paras.size() >= 2 && paras[1] == "txt")
			mod = ios::in | ios::out;
		std::fstream f(paras[0].c_str(), mod);

		if (f.good())
		{
			//读取全部文档
			f.seekg(0, std::ios::end);
			fl = f.tellg();
			if (paras[2].size() != 0 && paras[3].size() != 0) {
				//读取部分文件
				if (start > fl) {
					length = 0;
					goto end1;
				}
				//	return "FILE_READ() paras[2]:start is large than file len";  //参数超出范围
				if ((start + length) > fl)
					length = (fl - start);//调整length；
				f.seekg(start);
			}
			else {
				//读取全文
				f.seekg(0, std::ios::beg);
				length = fl;
			}
			//内存操作
			char *buffer = (char *)malloc(length + 1);
			memset(buffer, 0, length + 1);
			f.read(buffer, length);
			func_return.append(buffer, length);
			//清空缓存
			if (buffer)
				free(buffer);
			f.close();
		}
		else
			rt = (string)"FILE_READ()parameter error ,Param1:file names " + paras[0];
	}
end1:
	if (paras.size() == 5) {
		parent->_PubObj->SetNodeValue(paras[4].c_str(), STRING_Helper::uToStr(length).c_str(), Http_ParseObj::STR_COPY_OBJ);
	}
	return rt;
}

string ParseProcess::Http_ParseFunc::FILE_DIR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,6,parent,add_paras)<1){//Param数目错误
        return (string)"FILE_DIR()parameter error ,Param1:src dir; Paras2:filter; Param3: FILE&DIR test!;Param4:ADD_" + paras[0];
    }
    func_return="";

	//补充
	BYTE cell_null = NULL;
	vector<string> file_array;
	bool include_dir = STRING_Helper::PUB_FindStrIs(paras[3], "DIR_INCLUDE_DIR") ? true : false;
	bool add_dir = STRING_Helper::PUB_FindStrIs(paras[3], "ADD_DIR") ? true : false;

#ifdef WIN32
    //目录查询操作
    long hFile = 0;
    struct _finddata_t fileInfo;
    string pathName, exdName;
    if ((hFile = _findfirst(pathName.assign(paras[0]).append(paras[1]=="" ? "\\*.*" : paras[1]).c_str(), &fileInfo)) == -1) {//查询*.*或者paras[1]指定匹配的文件
        return (string)"DIR("+paras[0]+") not exited!";
    }
    do
    {
        //跳过'.'和'..'两个目录
        if(fileInfo.name[0] == '.')
            continue;
        //读取文件或目录
        
        if (!include_dir && fileInfo.attrib&_A_SUBDIR)
            continue;//判断文件的属性是文件夹还是文件

		//过滤
		int a = strlen(fileInfo.name);
		int b = STRING_Helper::PUB_FindStr(fileInfo.name, paras[1].c_str()) + paras[1].size();
		if (paras[1].size() != 0 && a != b)
			continue;
		//累加字串
		file_array.push_back(add_dir ? paras[0] + "/" + fileInfo.name : fileInfo.name);

    } while (_findnext(hFile, &fileInfo) == 0);
    //关闭
    _findclose(hFile);
  
#else
    //读取目录
    string dirname;
    DIR *dp;
    struct dirent *dirp;
    if((dp = opendir(paras[0].c_str())) == NULL)
    {
        return (string)"DIR("+paras[0]+") not exited!";
    }
   
    //取得文件列表
    while((dirp = readdir(dp)) != NULL)
    {
        //跳过'.'和'..'两个目录
        if(dirp->d_name[0] == '.')
            continue;
        //读取文件或目录
        if ((dirp->d_type == DT_DIR)  && (!include_dir))
            continue;
        //过滤
        int a=strlen(dirp->d_name);
        int b=STRING_Helper::PUB_FindStr(dirp->d_name,paras[1].c_str())+paras[1].size();
        if (paras[1].size()!=0 && a!=b)
            continue;
        //累加字串
        file_array.push_back(add_dir ? paras[0]+"/"+dirp->d_name : dirp->d_name);
    }
    
    //关闭
    closedir(dp);
    
#endif
	//排序操作
	if (paras[4] == "DESC")
		sort(file_array.begin(), file_array.end(), greater<string>());
	else if (paras[4] == "ASC")
		sort(file_array.begin(), file_array.end(), less<string>());

	//只取得记录数
	if (paras[2] == "NUM") {
		func_return = STRING_Helper::uToStr(file_array.size());
		return "";
	}
	//计数
	BB_SIZE_T num = STRING_Helper::NaN;
	if ((paras[2].size() != 0) && STRING_Helper::CheckNum(paras[2])) {
		num = STRING_Helper::StrToi(paras[2]);
	}
	if (paras[5] == "BARRAY") {
		//数组结果集
		string content = "";
		for (size_t n = 0; (n<file_array.size()) && (n<num); n++) {
			BB_OBJ *cell_obj = NULL;
			cell_obj = BB_NEW_BYTES_ALL((void *)(file_array[n].c_str()), file_array[n].size(), NULL);
			content += string((const char*)cell_obj, BB_GET_OBJ_SIZE(cell_obj));
			if (cell_obj != NULL)
				free(cell_obj);
		}
		BB_OBJ *dest_obj = NULL;
		dest_obj = BB_NEW_ARRAY(&cell_null, NULL, NULL, 0, content.data(), content.size(), NULL);
		func_return = string((const char*)dest_obj, BB_GET_OBJ_SIZE(dest_obj));
		free(dest_obj);
	}
	else {
		//字串换行结果集
		string split;
		for (size_t n = 0; (n<file_array.size()) && (n<num); n++) {
			//读取指定数量
			if (n == 0)
				func_return += file_array[n];
			else
				func_return += (paras[5].size() == 0 ? "\r\n" : paras[5]) + file_array[n];
		}
	}
    //if (func_return.size()!=0)
        //func_return.erase(func_return.size()-2);//删除最后“\r\n”

    return "";
}

/*
string ParseProcess::Http_ParseFunc::FILE_DIR_EXT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
	// String 转换.{@STR_LEN({*******})}
	vector<string> paras;
	if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 6, parent, add_paras)<1) {//Param数目错误
		return (string)"FILE_DIR()parameter error ,Param1:src dir; Paras2:filter; Param3: FILE&DIR test!;Param4:SORT" + paras[0];
	}
	func_return = "";

	//补充
	BYTE cell_null = NULL;
	vector<vector<string>> file_arrays;
	bool include_dir = STRING_Helper::PUB_FindStrIs(paras[3], "NODIR") ? false : true;
	string filter = paras[2].size()>0 ? paras[2] : "";

#ifdef WIN32
	//目录查询操作
	long hFile = 0;
	struct _finddata_t fileInfo;
	string pathName, exdName;
	if ((hFile = _findfirst(pathName.assign(paras[0]).append("\\*.*").c_str(), &fileInfo)) == -1) {//查询*.*或者paras[1]指定匹配的文件
		return (string)"DIR(" + paras[0] + ") not exited!";
	}
	do
	{
		//跳过'.'和'..'两个目录
		if (fileInfo.name[0] == '.')
			continue;
		//读取文件或目录

		if (!include_dir && fileInfo.attrib&_A_SUBDIR)
			continue;//判断文件的属性是文件夹还是文件

		//过滤
		if (filter.size() > 0) {
			int a = strlen(fileInfo.name);
			int b = STRING_Helper::PUB_FindStr(fileInfo.name, filter.c_str()) + filter.size();
			if (filter.size() != 0 && a != b)
				continue;
		}

		//累加字串
		vector<string> file_arr;
		file_arr.push_back(fileInfo.attrib&_A_SUBDIR ? "1" : "0");
		file_arr.push_back(fileInfo.name);
		file_arr.push_back(STRING_Helper::uToStr(fileInfo.size));
		file_arr.push_back(STRING_Helper::uToStr(fileInfo.time_create));
		file_arr.push_back(STRING_Helper::uToStr(fileInfo.time_write));
		file_arrays.push_back(file_arr);
	} while (_findnext(hFile, &fileInfo) == 0);
	//关闭
	_findclose(hFile);
#else
	//读取目录
	string dirname;
	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(paras[0].c_str())) == NULL)
	{
		return (string)"DIR(" + paras[0] + ") not exited!";
	}

	//取得文件列表
	while ((dirp = readdir(dp)) != NULL)
	{
		//跳过'.'和'..'两个目录
		if (dirp->d_name[0] == '.')
			continue;
		//读取文件或目录
		if ((dirp->d_type == DT_DIR) && (!include_dir))
			continue;
        
		//过滤
        if (filter.size() > 0) {
            int a = strlen(dirp->d_name);
            int b = STRING_Helper::PUB_FindStr(dirp->d_name, filter.c_str()) + filter.size();
            if (filter.size() != 0 && a != b)
                continue;
        }

        //累加字串
        vector<string> file_arr;
        file_arr.push_back(dirp->d_type == DT_DIR ? "1" : "0");
        file_arr.push_back(dirp->d_name);
        //读取深层信息
        struct stat st;
        string ss = paras[0] + "/" + dirp->d_name;
        if (stat(ss.data() , &st)==0){
            file_arr.push_back(STRING_Helper::uToStr(st.st_size));
            #if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
                file_arr.push_back(STRING_Helper::uToStr(st.st_ctimespec.tv_sec));
                file_arr.push_back(STRING_Helper::uToStr(st.st_mtimespec.tv_sec));
            #else
                file_arr.push_back(STRING_Helper::uToStr(st.st_ctime));
                file_arr.push_back(STRING_Helper::uToStr(st.st_mtime));
            #endif
        }
        file_arrays.push_back(file_arr);
	}

	//关闭
	closedir(dp);

#endif
	//排序操作
//    if (paras[4] == "DESC")
//        sort(file_arrays.begin(), file_arrays.end(), greater<string>());
//    else if (paras[4] == "ASC")
//        sort(file_arrays.begin(), file_arrays.end(), less<string>());

	//计数
	BB_SIZE_T num = ParseProcess::Http_ParseFunc::FILE_DIR_MAX_NUM;//最大返回数目
	BB_SIZE_T start = 0;
	vector <string> num_arr;
	STRING_Helper::stringSplit(paras[1],num_arr,",");
	if ((num_arr.size() != 0) && STRING_Helper::CheckNum(num_arr[0]))
		num = STRING_Helper::StrToi(num_arr[0]);
	if ((num_arr.size() > 1) && STRING_Helper::CheckNum(num_arr[1]))
		start = STRING_Helper::StrToi(num_arr[1]);

	if (paras[5] == "BARRAY") {
		//数组结果集
//        string content = "";
//        for (size_t n = 0; (n<file_arrays.size()) && (n<num); n++) {
//            BB_OBJ *cell_obj = NULL;
//            cell_obj = BB_NEW_BYTES_ALL((void *)(file_array[n].c_str()), file_array[n].size(), NULL);
//            content += string((const char*)cell_obj, BB_GET_OBJ_SIZE(cell_obj));
//            if (cell_obj != NULL)
//                free(cell_obj);
//        }
//        BB_OBJ *dest_obj = NULL;
//        dest_obj = BB_NEW_ARRAY(&cell_null, NULL, NULL, 0, content.data(), content.size(), NULL);
//        func_return = string((const char*)dest_obj, BB_GET_OBJ_SIZE(dest_obj));
//        free(dest_obj);
	}
	else {
		//字串换行结果集
		string split;
		func_return += "[";
		for (size_t n = start; (n<file_arrays.size()) && (n<num); n++) {
			//读取指定数量
			func_return += "[";
			vector<string> &file_arr = file_arrays[n];
			for (size_t m = 0; m< file_arr.size(); m++) {
				if (m == (file_arr.size() - 1))
					func_return += (string)"\""+ file_arr[m]+"\"";
				else
					func_return += (string)"\"" + file_arr[m] + "\",";
			}
			if (n == (file_arrays.size() - 1))
				func_return += "]";
			else
				func_return += "],";
		}
		func_return += "]";
	}

	return "";
}*/

string ParseProcess::Http_ParseFunc::FILE_DIR_EXT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 6, parent, add_paras)<1) {//Param数目错误
        return (string)"FILE_DIR()parameter error ,Param1:src dir; Paras2:filter; Param3: FILE&DIR test!;Param4:SORT" + paras[0];
    }
    func_return = "";
    int max_field=4;
    
    //补充
    BYTE cell_null = NULL;
    map< string,vector<string> > file_map;
    bool include_dir = STRING_Helper::PUB_FindStrIs(paras[3], "NODIR") ? false : true;
    string filter = paras[2].size()>0 ? paras[2] : "";
    //排序信息
    bool is_sort=false,is_asc=true;
    int sort_field=1;
    if(paras[4].size()>0){//ly shop
        vector<string> sort_inf;
        STRING_Helper::stringSplit(paras[4], sort_inf, "_");
        if (sort_inf.size()<2 || !STRING_Helper::CheckNum(sort_inf[0]))
            return (string)"FILE_DIR_EXT()parameter error ,Param4 must be:'[FIELD_NUM]_asc|des'" + paras[4];
        sort_field=STRING_Helper::StrToi(sort_inf[0]);
        if (sort_field>4 || (sort_inf[1]!="des" && sort_inf[1]!="asc" ))
            return (string)"FILE_DIR_EXT()parameter error ,Param4 '[FIELD_NUM] out of range 4' or not asc|des" +paras[4];
        is_sort=true; 
        is_asc=sort_inf[1]=="des" ? false : true;
    }
    
#ifdef WIN32
    //目录查询操作
    long hFile = 0;
    struct _finddata_t fileInfo;
    string pathName, exdName;
    if ((hFile = _findfirst(pathName.assign(paras[0]).append("\\*.*").c_str(), &fileInfo)) == -1) {//查询*.*或者paras[1]指定匹配的文件
        return (string)"DIR(" + paras[0] + ") not exited!";
    }
    int s=0;
    do
    {
        //跳过'.'和'..'两个目录
        if (fileInfo.name[0] == '.')
            continue;
        //读取文件或目录
        
        if (!include_dir && fileInfo.attrib&_A_SUBDIR)
            continue;//判断文件的属性是文件夹还是文件
        
        //文件名过滤
        if ((filter.size() > 0) && (STRING_Helper::PUB_FindStr(fileInfo.name, filter.c_str())<0)) {
            continue;
        }
        //类型过滤
//        if (filter.size() > 0) {
//            int a = strlen(fileInfo.name);
//            int b = STRING_Helper::PUB_FindStr(fileInfo.name, filter.c_str()) + filter.size();
//            if (filter.size() != 0 && a != b)
//                continue;
//        }
        
        //累加字串
        vector<string> file_arr;
        file_arr.push_back(fileInfo.attrib&_A_SUBDIR ? "1" : "0");
        file_arr.push_back(fileInfo.name);
        file_arr.push_back(STRING_Helper::uToStr(fileInfo.size));
        file_arr.push_back(STRING_Helper::uToStr(fileInfo.time_create));
        file_arr.push_back(STRING_Helper::uToStr(fileInfo.time_write));
        //file_arrays.push_back(file_arr);
        string key=((sort_field!=2) ? file_arr[sort_field] : STRING_Helper::uToHex(fileInfo.size,8))+STRING_Helper::uToHex(s++,4);
        file_map.insert(pair<string, vector<string>>(key, file_arr));
    } while (_findnext(hFile, &fileInfo) == 0);
    //关闭
    _findclose(hFile);
#else
    //读取目录
    string dirname;
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(paras[0].c_str())) == NULL)
    {
        return (string)"DIR(" + paras[0] + ") not exited!";
    }
    
    //取得文件列表
    int s=0;
    while ((dirp = readdir(dp)) != NULL)
    {
        //跳过'.'和'..'两个目录
        if (dirp->d_name[0] == '.')
            continue;
        //读取文件或目录
        if ((dirp->d_type == DT_DIR) && (!include_dir))
            continue;
        
        //文件名过滤
        if ((filter.size() > 0) && (STRING_Helper::PUB_FindStr(dirp->d_name, filter.c_str())<0)) {
            continue;
        }
        //类型过滤
//        if (filter.size() > 0) {
//            int a = strlen(dirp->d_name);
//            int b = STRING_Helper::PUB_FindStr(dirp->d_name, filter.c_str()) + filter.size();
//            if (filter.size() != 0 && a != b)
//                continue;
//        }
        
        //累加字串
        vector<string> file_arr;
        file_arr.push_back(dirp->d_type == DT_DIR ? "1" : "0");
        file_arr.push_back(dirp->d_name);
        //读取深层信息
        struct stat st;
        string ss = paras[0] + "/" + dirp->d_name;
        if (stat(ss.data() , &st)==0){
            file_arr.push_back(STRING_Helper::uToStr(st.st_size));
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
            file_arr.push_back(STRING_Helper::uToStr(st.st_ctimespec.tv_sec));
            file_arr.push_back(STRING_Helper::uToStr(st.st_mtimespec.tv_sec));
#else
            file_arr.push_back(STRING_Helper::uToStr(st.st_ctime));
            file_arr.push_back(STRING_Helper::uToStr(st.st_mtime));
#endif
        }
        //file_arrays.push_back(file_arr);
        string key=((sort_field!=2) ? file_arr[sort_field]: STRING_Helper::uToHex(st.st_size,8))+STRING_Helper::uToHex(s++,4);;
        file_map.insert(pair<string, vector<string> >(key, file_arr));
    }
    
    //关闭
    closedir(dp);
    
#endif
    //计数
    BB_SIZE_T num = ParseProcess::Http_ParseFunc::FILE_DIR_MAX_NUM;//最大返回数目
    BB_SIZE_T start = 0;
    vector <string> num_arr;
    STRING_Helper::stringSplit(paras[1],num_arr,";");
    if ((num_arr.size() == 1) && STRING_Helper::CheckNum(num_arr[0]))
        num = STRING_Helper::StrToi(num_arr[0]);
    if ((num_arr.size() > 1) && STRING_Helper::CheckNum(num_arr[1]) && STRING_Helper::CheckNum(num_arr[0])){
        num = STRING_Helper::StrToi(num_arr[1]);
        start = STRING_Helper::StrToi(num_arr[0]);
    }
    
    if (paras[5] == "BARRAY") {
        //数组结果集
//        string content = "";
//         for (size_t n = 0; (n<file_arrays.size()) && (n<num); n++) {
//         BB_OBJ *cell_obj = NULL;
//         cell_obj = BB_NEW_BYTES_ALL((void *)(file_array[n].c_str()), file_array[n].size(), NULL);
//         content += string((const char*)cell_obj, BB_GET_OBJ_SIZE(cell_obj));
//         if (cell_obj != NULL)
//         free(cell_obj);
//         }
//         BB_OBJ *dest_obj = NULL;
//         dest_obj = BB_NEW_ARRAY(&cell_null, NULL, NULL, 0, content.data(), content.size(), NULL);
//         func_return = string((const char*)dest_obj, BB_GET_OBJ_SIZE(dest_obj));
//         free(dest_obj);
    }
    else {
        //字串换行结果集
        string split;
        func_return += "[";
        size_t n=0;
        map<string,vector<string> >::iterator iter=file_map.begin();
        map<string,vector<string> >::reverse_iterator riter=file_map.rbegin();
        for (;iter!=file_map.end() && riter!=file_map.rend(); ) {
            //读取指定数量
            if (n>=start && n<(start+num)){//翻页判断
                vector<string> &file_arr= is_asc ? iter->second : riter->second ;
                func_return += "[";
                for (size_t m = 0; m< file_arr.size(); m++) {
                    if (m == (file_arr.size() - 1))
                        func_return += (string)"\""+ file_arr[m]+"\"";
                    else
                        func_return += (string)"\"" + file_arr[m] + "\",";
                }
                if (n == (file_map.size() - 1) || n==(start+num-1))
                    func_return +=(string)",\""+STRING_Helper::uToStr(file_map.size())+"\"]";
                else
                func_return += "],";
            }
            
            if (is_asc)
                ++iter;
            else
                ++riter;
            
            n++;
        }
        
        func_return += "]";
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::FILE_DIR_ALL(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,6,parent,add_paras)<1){//Param数目错误
        return (string)"FILE_DIR_ALL()parameter error ,Param1:src dir; Paras2:num; Param3: filter, Param4: options" + paras[0];
    }
    func_return="";
    
    //补充
    vector<string> r_model;
    int r_type=0;
    if (STRING_Helper::PUB_FindStrIs(paras[3], "DIR"))
        r_type =ParseProcess::Http_ParseFunc::DIR_INCLUDE_DIR;
    if (STRING_Helper::PUB_FindStrIs(paras[3], "FILE"))
        r_type |=ParseProcess::Http_ParseFunc::DIR_INCLUDE_FILE;
    if (r_type==0)
        r_type=ParseProcess::Http_ParseFunc::DIR_INCLUDE_DIR | ParseProcess::Http_ParseFunc::DIR_INCLUDE_FILE;

    STRING_Helper::stringSplit(paras[3],r_model,";");
    string &filter=paras[2];
    string &search_dir=paras[0];
    //计数
    BB_SIZE_T num = ParseProcess::Http_ParseFunc::FILE_DIR_MAX_NUM;//最大返回数目
    if ((paras[1].size() != 0) && STRING_Helper::CheckNum(paras[1])) {
        num = STRING_Helper::StrToi(paras[1]);
    }
    //查询目录下的文件
    string r_str=ParseProcess::Http_ParseFunc::_FILE_DIR_ALL_TMP(func_return,search_dir,num,filter,r_type,r_model);
    if (r_str.size()!=0)
        func_return="";
    return r_str;
}

string ParseProcess::Http_ParseFunc::_FILE_DIR_ALL_TMP(string &add_str, string &search_dir,BB_SIZE_T num,string &filter,int r_type,vector<string> & r_model){
    add_str="";
    
#ifdef WIN32
    //目录查询操作
    long hFile = 0;
    struct _finddata_t fileInfo;
    string pathName, exdName;
    if ((hFile = _findfirst(pathName.assign(search_dir).append("\\*.*").c_str(), &fileInfo)) == -1) {//查询*.*或者filter指定匹配的文件
        return (string)"DIR("+search_dir+") not exited!";
    }
    add_str="[";
    string r_str;
    bool finded=false;
    //
    for(size_t i=0;i<num;)
    {
//        if(!((fileInfo.name[0] == '.' && fileInfo.name[1]==0)||(fileInfo.name[0] == '.' && fileInfo.name[1]=='.' && fileInfo.name[2]==0)) &&
//            !((filter.size() > 0) && (STRING_Helper::PUB_FindStr(fileInfo.name, filter.c_str())<0)))
        if(!((fileInfo.name[0] == '.' && fileInfo.name[1]==0)||(fileInfo.name[0] == '.' && fileInfo.name[1]=='.' && fileInfo.name[2]==0)))
        {
            string son_str="";
			//读取文件或目录
			if (fileInfo.attrib&_A_SUBDIR) {
				//查询子字串 
				string tmp_search_str = search_dir + "/" + fileInfo.name;
				r_str = ParseProcess::Http_ParseFunc::_FILE_DIR_ALL_TMP(son_str, tmp_search_str, num, filter, r_type, r_model);
				if (r_str.size() != 0) {
					_findclose(hFile);
					return r_str;
				}//
                //累加字串
                if ((r_type & ParseProcess::Http_ParseFunc::DIR_INCLUDE_DIR) || son_str!="[]"){
                    if(finded)
                        add_str+=",";
                    add_str+=(string)"[\"" + fileInfo.name + "\"," + son_str + "]";
                    finded=true;
                }
                i++;
			}
            //文件名过滤
            if ((filter.size() > 0) && (STRING_Helper::PUB_FindStr(fileInfo.name, filter.c_str())<0)) {
                if (_findnext(hFile, &fileInfo)!= 0)
                    break;
                else
                    continue;
            }
            
            //读取文件
            if (!(fileInfo.attrib&_A_SUBDIR) && (r_type & ParseProcess::Http_ParseFunc::DIR_INCLUDE_FILE)) {
                //累加字串
                if (finded)
                    add_str += ",";
                add_str += (string)"\"" + fileInfo.name + "\"";
                i++;
                finded=true;
            }
				
		}
        if (_findnext(hFile, &fileInfo)!= 0)
			break;
    };
	add_str += "]";
    //关闭
    _findclose(hFile);
    
#else
    //读取目录
    string dirname;
    DIR *dp;
    struct dirent *dirp;
    if((dp = opendir(search_dir.c_str())) == NULL)
    {
        return (string)"DIR("+search_dir+") not exited!";
    }
    
    //取得文件列表
    add_str="[";
    string r_str="";
    bool finded=false;
    for(size_t i=0;i<num;)
    {
        if ((dirp = readdir(dp)) == NULL)
            break;
        
        //跳过'.'和'..'两个目录
        if((dirp->d_name[0] == '.' && dirp->d_name[1]==0)||(dirp->d_name[0] == '.' && dirp->d_name[1]=='.' && dirp->d_name[2]==0))
            continue;
        
        //
        string son_str="";
        //读取深层目录
        if ((dirp->d_type == DT_DIR)){
            //查询子字串
            string tmp_search_str=search_dir+"/"+dirp->d_name;
            r_str=ParseProcess::Http_ParseFunc::_FILE_DIR_ALL_TMP(son_str,tmp_search_str,num,
                                                                  filter,r_type,r_model);
            if (r_str.size()!=0){
                closedir(dp);
                return r_str;
            }
            //累加字串
            if ((r_type & ParseProcess::Http_ParseFunc::DIR_INCLUDE_DIR) || son_str!="[]"){
                if(finded)
                    add_str+=",";
                add_str+=(string)"[\"" + dirp->d_name + "\"," + son_str + "]";
                finded=true;
            }
            i++;
        }
        
        //文件名过滤
        if ((filter.size() > 0) && (STRING_Helper::PUB_FindStr(dirp->d_name, filter.c_str())<0)) {
            continue;
        }
        
        //读取文件
        if ((dirp->d_type != DT_DIR) && (r_type & ParseProcess::Http_ParseFunc::DIR_INCLUDE_FILE)){
            if(finded)
                add_str+=",";
            add_str+=(string)"\""+dirp->d_name+"\"";
            i++;
            finded=true;
        }
    }
    add_str+="]";
    //关闭
    closedir(dp);
#endif
    return "";
}

string ParseProcess::Http_ParseFunc::FILE_READ_INFO(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    int paras_num;
    if ((paras_num = ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 4, parent, add_paras))<2) {//Param数目错误
        return (string)"FILE_READ_INFO()parameter error ,Param1:file names " + paras[0];
    }
    
    //存文件操作
    //临时文件名存储操作
    std::fstream f(paras[0].c_str(), ios::in | ios::out | ios::binary);
    //std::ifstream f(paras[0].c_str());
    func_return = "";
    if (f.good())
    {
        //读取全部文档
        //char buf[1025];
        f.seekg(0, std::ios::end);
        long fl = f.tellg();
        
        if (paras[1] == "check_sum") {
            //读取文件内容
            f.seekg(0, std::ios::beg);
            char *buffer = new char[fl + 1];
            memset(buffer, 0, fl + 1);
            f.read(buffer, fl);
            int from=0;//to=fl;
            if (paras[3].size()!=0){
                from=STRING_Helper::ParseInt(paras[2]);
            }
            //循环计算文件的长度
            UInt32 sum=0;
            for (long i = from; i < fl; i++) {
                sum += (unsigned char)buffer[i];
            }
            //返回数值
            if (paras[2] == "16" || paras[2]=="hex"){
                char str[17];
                sprintf(str, "%x", sum);
                func_return=STRING_Helper::SetNumToID(str,8);
            }else{
                func_return = STRING_Helper::iToStr(sum);
            }
            delete[]buffer;
        }
        else if (paras[1] == "pkg_num") {
            if (!STRING_Helper::CheckNum(paras[2]))
                return "FILE_READ_INFO()parameter error ,Param1:file names, Paras2[pkg_num], Paras3 pkg_len should >0 ";
            unsigned int pkg_len = STRING_Helper::ParseInt(paras[2].c_str());
            if (pkg_len==0)
                return "FILE_READ_INFO()parameter error ,Param1:file names, Paras2[pkg_num], Paras3 pck_len should >0";
            unsigned int pkg_num = (fl / pkg_len) + (fl%pkg_len == 0 ? 0 : 1);
            if (paras[3]=="16" || paras[3]=="hex")
                func_return = STRING_Helper::iToStr(pkg_num,16);
            else
                func_return = STRING_Helper::iToStr(pkg_num);
        }else if (paras[1] == "file_len") {
            if (paras[2]=="hex" || paras[2]=="16")
                func_return = STRING_Helper::iToStr(fl,16);
            else
                func_return = STRING_Helper::iToStr(fl);
        }
        else
            return "FILE_READ_INFO()parameter error ,Param1:file names, Paras2[check_sum or pkg_num], Paras3";
        f.close();
    }
    else
        func_return = (string)"file("+paras[0]+") open is not good()";
    
    return "";
}

string ParseProcess::Http_ParseFunc::CHKSUM(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    int paras_num;
    if ((paras_num = ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 3, parent, add_paras))==0) {//Param数目错误
        return (string)"CHKSUM()parameter error ,Param1:file names " + paras[0];
    }
    //取得连续你钻丝
    string data;
    if (paras[1]=="add_son"){
        parent->_PubObj->AddChildNodesValue(paras[1].c_str(),data); //取得所有子节点信息，累加；
    }else if (paras[1]=="add_array"){
        BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU((BB_OBJ *)(paras[0].c_str()));
        if (!GB_AvartarDB->GetArrayRecsAddStr(obj_stru,data))
            return "Param0 is not Array!";
    }else
        data=paras[0];
    
    //SUM累加
    UInt32 sum=0;
    unsigned char *p=(unsigned char *)(data.data());
    for (BB_SIZE_T i = 0; i <data.size(); i++) {
        sum += (unsigned char)p[i];
    }
    //返回数值
    if (paras[2] == "16" || paras[2]=="hex"){
        char str[17];
        sprintf(str, "%x", sum);
        func_return=STRING_Helper::SetNumToID(str,8);
    }else{
        func_return = STRING_Helper::iToStr(sum);
    }
    return "";
}

string ParseProcess::Http_ParseFunc::FILE_SAVE_EXT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    return "";
}

string ParseProcess::Http_ParseFunc::FILE_READ_EXT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    int paras_num;
    if ((paras_num = ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 5, parent, add_paras))<5) {//Param数目错误
        return (string)"FILE_READ_EXT()parameter error ,Param1:file names " + paras[0];
    }
    string index_table = STRING_Helper::HBCStrToHBCStr(paras[0], "hex", "char", false);
    string nodeid_str = STRING_Helper::HBCStrToHBCStr(paras[1], "hex", "char", false);
    string ext_name = paras[2];
    BB_SIZE_T start = STRING_Helper::ParseInt(paras[3]);
    BB_SIZE_T length = STRING_Helper::ParseInt(paras[4]);
    
    //取得文件名称
    BB_OBJ *table_obj;
    BB_SIZE_T table_obj_size;
    if (!GB_AvartarDB->GetTableObject(index_table, &table_obj, table_obj_size)) {
        return (string)"FILE_READ_EXT() table("+paras[0]+") load error!";
    }
    BB_OBJ_STRU table_obj_stru = BB_GET_OBJ_STRU(table_obj);
    
    BYTE * buf = NULL;
    int rt;
    if ((rt = GB_AvartarDB->LoadExt(table_obj_stru, nodeid_str, ext_name, &buf, start, length, 0)) != 0) {
        return (string)"FILE_READ_EXT() table(" + paras[0] + ") Read error("+STRING_Helper::iToStr(rt)+")!";;
    }
    
    func_return = string((const char*)buf, length);
    if (buf)
        free(buf);
    
    return "";
}

string ParseProcess::Http_ParseFunc::PACKET_PARSE_XML(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    const char *p1;
    string temp1,temp2;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()!=2){//Param数目错误
        return (string)"@PACKET_PARSE_XML()parameter error :Param1: Source  String ,Param2:Xpath of Node ."+func_field;
    }
    //取得Param一
    if ( CheckParaQuot(paras[0],temp1)){
        // String Param
        p1=temp1.c_str();
    }else{
        // Object Param
        p1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (p1==NULL)
            return parent->_PubObj->ErrorStr;
    }
    //取得Param二
    if (CheckParaQuot(paras[1],temp2)){
        // String Param
    }else{
        // Object Param
        parent->_PubObj->ErrorStr="@PACKET_PARSE_XML()parameter error :Param1: Source  String ,Param2:Xpath of Node .";
        return parent->_PubObj->ErrorStr;
    }
    
    //实际操作
    if (parent->_PubPacket->GetFieldsFormOriginStr(p1,temp1, parent->_PubPacket->destXml,"")){
        func_return = "asfasfasfsdfasf";
    }else{
        // Object Param
        return parent->_PubPacket->ErrorStr;
    }
    return "";
}

string ParseProcess::Http_ParseFunc::XML_PARSE_PACKET(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    const char *p2,*p3;
    string temp1,temp2,temp3;
    ParseXml_Node node;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()!=3){//Param数目错误
        return (string)"@XML_PARSE_PACKET()parameter error :Param1: Source xml Object ,Param2:Xpath of Node  String ,Param3: hex,char,bin.";
    }
    //取得Param一: Source  Object ,
    if (CheckParaQuot(paras[0],temp1)){
        // String Param,注:该Param必须是 Object ,而且还不能是xml子目录
        parent->_PubObj->ErrorStr="@XML_PARSE_PACKET()parameter error :Param1: Source xml Object ,Param2:Xpath of Node  String .";
        return parent->_PubObj->ErrorStr;
    }else{
        // Object Param,取得XML node
        if (!parent->_PubObj->XmlNode(paras[0].c_str(),node))
            return parent->_PubObj->ErrorStr;
    }
    //取得Param二:Xpath of Node
    if (CheckParaQuot(paras[1],temp2)){
        // String Param
        p2=temp2.c_str();
    }else{
        // Object Param,则取得 Object 值.
        p2= parent->_PubObj->GetNodeValue(paras[1].c_str());
        if (p2==NULL)
            return parent->_PubObj->ErrorStr;
    }
    
    //取得Param三:Xpath of Node
    if (CheckParaQuot(paras[2],temp3)){
        // String Param
        p3=temp3.c_str();
    }else{
        // Object Param,则取得 Object 值.
        p3= parent->_PubObj->GetNodeValue(paras[2].c_str());
        if (p2==NULL)
            return parent->_PubObj->ErrorStr;
    }
    
    //实际操作
    func_return = parent->_PubPacket->GetOriginStrFromFields(p2,node,paras[0],p3);
    return "";
}

string ParseProcess::Http_ParseFunc::GET_PARSE_TYPE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    const char* p2;
    string temp1,temp2;
    Http_ParseXML *p_xml;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()!=2){//Param数目错误
        return (string)"@GET_PARSE_TYPE()parameter error :Param1:Xpath of Node ,Param2:Conditions of  XML.";
    }
    //取得Param一:Xpath of Node
    if (CheckParaQuot(paras[0],temp1)){
        // String Param
        p2=temp1.c_str();
    }else{
        // Object Param
        return string("@GET_PARSE_TYPE()parameter error :Param1:Xpath of Node ,Param2:ConditionsXML.");
    }
    
    //取得Param二: Source  Object ,
    if (CheckParaQuot(paras[1],temp2)){
        // String Param,注:该Param必须是 Object ,而且还不能是xml子目录
        parent->_PubObj->ErrorStr="@GET_PARSE_TYPE()parameter error :Param1:Xpath of Node ,Param2:Conditions of XML Object .";
        return parent->_PubObj->ErrorStr;
    }else{
        // Object Param
        int src_set_type;
        if(paras[1].find(":")!=string::npos)
            return string("@GET_PARSE_TYPE()parameter error ,Param2:not support xml sub node ,must be xml Object ");
        
        p_xml=(Http_ParseXML *)parent->_PubObj->GetObjNode(paras[1].c_str(),src_set_type);
        if (p_xml==NULL)
            return parent->_PubObj->ErrorStr;
    }
    
    //实际操作
    const char *r= parent->_PubPacket->GetParseType(p2,p_xml->FirstChild(p_xml->ObjXml));
    if (r!=NULL){
        func_return=r;
        return "";
    }else{
        return (string)"@XML_PARSE_PACKET()can't find destinct node "+paras[1];
    }
}

string ParseProcess::Http_ParseFunc::BINARY_TO_HBC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    string temp1,temp2,temp3;
    const char *para1,*para2,*para3;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()>3){//Param数目错误
        return (string)"@BINARY_TO_HBC()parameter error :Param1: Object  or  String ,Param2: Object  or  String ";
    }
    
    if (paras.size()==3){
        //解析Param1内容
        if (CheckParaQuot(paras[0],temp1)){
            // String Param
            para1=temp1.c_str();
        }else{
            // Object Param
            para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
            if (para1==NULL)
                return parent->_PubObj->ErrorStr;
        }
        //解析Param2内容
        if ( CheckParaQuot(paras[1],temp2)){
            // String Param
            para2=temp2.c_str();
        }else{
            // Object Param
            para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
            if (para2==NULL)
                return parent->_PubObj->ErrorStr;
        }
        
        //解析Param3内容
        if ( CheckParaQuot(paras[2],temp3)){
            // String Param
            para3=temp3.c_str();
        }else{
            // Object Param
            para3= parent->_PubObj->GetNodeValue(paras[2].c_str());
            if (para3==NULL)
                return parent->_PubObj->ErrorStr;
        };
        func_return=parent->_PubPacket->BinaryToHBCStr((const char *)para1,atoi(para2),para3,false);
    }else if (paras.size()==2){
        //读取Param信息
        if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,3,parent,add_paras)<1){//Param数目错误
            return (string)"@HBC_TO_HBC()parameter error :Param1: src string ,Param2: from hex char ,Param3: to hex,char,Param4:if add head. ";
        }
        func_return=parent->_PubPacket->BinaryToHBCStr((const char *)paras[0].c_str(),paras[0].size(),paras[1],false);
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::POINT_TO_STR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    string temp1,temp2,temp3;
    const char *para1,*para2;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()!=2){//Param数目错误
        return (string)"@MEM_TO_HBC()parameter error :Param1: STR POINT OBJ  ,Param2: STR POINT OBJ LENGTH ";
    }
    
    //解析Param1内容
    if (CheckParaQuot(paras[0],temp1)){
        // String Param
        para1=temp1.c_str();
    }else{
        // Object Param
        para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (para1==NULL)
            return parent->_PubObj->ErrorStr;
    }
    //解析Param2内容
    if ( CheckParaQuot(paras[1],temp2)){
        // String Param
        para2=temp2.c_str();
    }else{
        // Object Param
        para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
        if (para2==NULL)
            return parent->_PubObj->ErrorStr;
    }
    
    func_return.append((const char *)para1,atoi(para2));
    
    return "";
}

string ParseProcess::Http_ParseFunc::PARSE_ENCODE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,8,parent,add_paras)<2){//Param数目错误
        return (string)"@PARSE_ENCODE()parameter error :" + paras[0];
    }
    //string tmp_return;
    //string tmp=ParseExt::ENCODE(paras,tmp_return);
    //func_return=tmp_return;
    func_return="";
    string tmp=ParseExt::ENCODE(paras,func_return);
    return tmp;
}


string ParseProcess::Http_ParseFunc::PARSE_LEN(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,3,parent,add_paras)<1){//Param数目错误
        return (string)"@PARSE_LEN()parameter error :" + paras[0];
    }
    return ParseExt::LEN(paras,func_return);
}

#define PARSE_PROCESS_MAX_FUNC_PARAM 8
string ParseProcess::Http_ParseFunc::PARSE_FUNC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,PARSE_PROCESS_MAX_FUNC_PARAM,parent,add_paras)<3){//Param数目错误;
        return (string)"@PARSE_ENCODE()parameter error :" + paras[0];
    }
    return ParseExt::FUNC(paras,func_return);
}

string ParseProcess::Http_ParseFunc::EVAL(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,3,parent,add_paras)<1){//Param数目错误;
        return (string)"@EVAL()parameter error :" + paras[0];
    }
    string rt=parent->ParseValue(paras[0],func_return,add_paras);
    return rt;
}

#if !defined(__ANDROID__) && !defined(__APPLE__)
string ParseProcess::Http_ParseFunc::WEBSOCKET_ACCEPT_KEY(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,2,parent,add_paras)<1){//Param数目错误
        return (string)"@WEBSOCKET_ACCEPT_KEY()parameter error :" + paras[0];
    }
    //new websocket v13:
    unsigned char sha[20]={0};
    string tmp_str=paras[0]+"258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    
    //SHA1((const unsigned char *)tmp_str.c_str(),tmp_str.length(),sha);
    sha1((const unsigned char *)tmp_str.c_str(),tmp_str.length(),(unsigned char *)sha);
    string tmp_str3;
    string tmp_str2((const char *)sha,20);
    tmp_str3 = STRING_Helper::HBCStrToHBCStr(tmp_str2,LW_BINARY_STR,LW_HEX_STR,false);
    //parent->_PubPacket->BinaryToHBCStr((const char *)sha,20,tmp_str3,false);
    cout <<"sha1: "<< tmp_str3 << endl;
    func_return = base64_encode(sha,20);
    
    return "";
}

string ParseProcess::Http_ParseFunc::WEBSOCKET_FRAME_TYPE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,1,parent,add_paras)<1){//Param数目错误
        return (string)"@WEBSOCKET_FRAME_TYPE()parameter error :" + paras[0];
    }
    enum ws_frame_type type;
    type = ws_parse_input_frame((unsigned char *)paras[0].data(),paras[0].size());
    
    switch(type){
        case WS_TEXT_FRAME: func_return="WS_TEXT_FRAME";break;
        case WS_BINARY_FRAME: func_return="WS_BINARY_FRAME";break;
        case WS_PING_FRAME: func_return="WS_PING_FRAME";break;
        case WS_PONG_FRAME: func_return="WS_PONG_FRAME";break;
        case WS_CLOSING_FRAME: func_return="WS_CLOSING_FRAME";break;
        case WS_INCOMPLETE_FRAME: func_return="WS_INCOMPLETE_FRAME";break;
        case WS_ERROR_FRAME: func_return="WS_ERROR_FRAME";break;
        default:
            func_return="WS_ERROR_FRAME";
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::WEBSOCKET_FRAME_LEN(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,1,parent,add_paras)<1){//Param数目错误
        return (string)"@WEBSOCKET_FRAME_LEN()parameter error :" + paras[0];
    }
    
    if (paras[0].size() <2)
        return "frame is incomplate !";
    if (paras[0].data()[0]=='\0')
        return "frame is error !";
    
    size_t len=ws_frame_len((unsigned char *)paras[0].data());
    
    func_return=STRING_Helper::iToStr(len);
    return "";
}
string ParseProcess::Http_ParseFunc::WEBSOCKET_FRAME_PAYLOAD(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,1,parent,add_paras)<1){//Param数目错误
        return (string)"@WEBSOCKET_FRAME_PAYLOAD()parameter error :" + paras[0];
    }
    uint64_t len=paras[0].size();
    unsigned char * r=extract_payload((unsigned char *)paras[0].data(), &len);
    if (r!=NULL){
        func_return.append((const char *)r,(size_t)len);
        return "";
    }else{
        func_return="";
        return "Can't extract payload.";
    }
}


string ParseProcess::Http_ParseFunc::WEBSOCKET_ENCODE_FRAME(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,3,parent,add_paras)<1){//Param数目错误
        return (string)"@WEBSOCKET_ENCODE_FRAME()parameter error :" + paras[0];
    }
    ws_frame_type frame_type;
    if (paras[1]=="WS_TEXT_FRAME"){
        frame_type=WS_TEXT_FRAME;
    }else if (paras[1]=="WS_BINARY_FRAME") {
        frame_type=WS_BINARY_FRAME;
    }else if (paras[1]=="WS_PING_FRAME"){
        frame_type=WS_PING_FRAME;
    }else if (paras[1]=="WS_PONG_FRAME"){
        frame_type=WS_PONG_FRAME;
    }else if (paras[1]== "WS_CLOSING_FRAME"){
        frame_type=WS_CLOSING_FRAME;
    }/*else if (paras[1]== "WS_FRAME"){
      frame_type=WS_FRAME;
      }*/else{
          func_return="";
          return "error set WS_FREAME TYPE !";
      };
    
    int frame_option;
    if (paras[2]== "WS_FINAL_FRAME" || paras[2]==""){
        frame_option=FINAL_FRAME;
    }else if (paras[2]== "WS_NEXT_FRAME"){
        frame_option=NEXT_FRAME;
    }else{
        func_return="";
        return "error set WS_FREAME OPTION !";
    };
    
    uint8_t *out_frame;
    size_t out_frame_len;
    if (paras[0].size()!=0){
        ws_make_frame((uint8_t *)paras[0].data(), paras[0].size(),&out_frame, &out_frame_len,frame_type, frame_option);
        func_return.append((const char*)out_frame,out_frame_len);
    }else{
        func_return="";
        return "WS_FRAME_ERROR: input value is null !";
    }
    return "";
}
#endif
string ParseProcess::Http_ParseFunc::ROUTING_CREAT_PACKET(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,4,parent,add_paras)<3){//Param数目错误
        return (string)"@ROUTING_CREAT_PACKET()parameter error :" + func_field;
    }
    if (paras[0]=="URL_PATH"){
        if (paras[3]=="" )
            WIZBASE_create_routing_packet(func_return,WIZBASE_PATH_TYPE_URL_PATH,paras[1],paras[2],0);
        else if (STRING_Helper::CheckNum(paras[3]))
            WIZBASE_create_routing_packet(func_return,WIZBASE_PATH_TYPE_URL_PATH,paras[1],paras[2],STRING_Helper::StrToi(paras[3]));
        else
            return (string)"@ROUTING_CREAT_PACKET()parameter 4 error :"+paras[3]+" must be integer!";
        return "";
    }else
        return (string)"@ROUTING_CREAT_PACKET()parameter 1 error :"+paras[0];
}

string ParseProcess::Http_ParseFunc::CONN_OBJ_CREATE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    string err_str("");
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras)<1){//Param数目错误
        return (string)"@CONN_OBJ_CREATE()parameter error :" + func_field;
    }
    //get paras->tmp add paras ly_add
    ParseProcess::PV_AddParas_T tmp_paras;
    if (paras.size()>=3 && paras[2].size()!=0){
        vector <string> tmp_array;
        STRING_Helper::stringSplit(paras[2].c_str(),tmp_array, ";");
        for(unsigned int  i=0; i < tmp_array.size(); i++){
            tmp_paras.insert(ParseProcess::PV_AddParas_T::value_type(tmp_array[i], "" ));
            parent->_PubObj->GetNodeValueStr(tmp_array[i].c_str(),tmp_paras[tmp_array[i]]);
        }
    }
    //do func
    if (paras[0].size()!=0){
        if (paras[0].find("/")==string::npos || paras[0].find("\\")==string::npos){
            string path="//XMLParse/Connect/CONNECT[@Name='"; path+=paras[0]; path+="']";
            err_str = WIZBASE_create_connect_obj(path.c_str(),paras[1],&tmp_paras,paras[3].c_str(),paras.size()>4 ? paras[4].c_str():NULL);  //
            func_return=err_str;
        }else{
            err_str = WIZBASE_create_connect_obj(paras[0].c_str(),paras[1],&tmp_paras,paras[3].c_str(),paras.size()>4 ?paras[4].c_str() :NULL);
            func_return=err_str;
        }
        return err_str;
    }else{
        func_return="";
        return (string)"@CONN_OBJ_CREATE()parameter 1 error :"+paras[0];
    }
}

string ParseProcess::Http_ParseFunc::CONN_OBJ_STATE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,4,parent,add_paras)<3){//Param数目错误
        return (string)"@CONN_OBJ_STATE()parameter error :" + func_field;
    }
    if (paras[0]=="URL_PATH"){
        if (paras[3]=="" )
            WIZBASE_create_routing_packet(func_return,WIZBASE_PATH_TYPE_URL_PATH,paras[1],paras[2],0);
        else if (STRING_Helper::CheckNum(paras[3]))
            WIZBASE_create_routing_packet(func_return,WIZBASE_PATH_TYPE_URL_PATH,paras[1],paras[2],STRING_Helper::StrToi(paras[3]));
        else
            return (string)"@CONN_OBJ_STATE()parameter 4 error :"+paras[3]+" must be integer!";
        return "";
    }else
        return (string)"@CONN_OBJ_STATE()parameter 1 error :"+paras[0];
}

string ParseProcess::Http_ParseFunc::DO_QUEUE_TABLE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node;
    
    //读取Param信息
    //if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 3, parent, add_paras)<2) {//Param数目错误
    //	return (string)"@DO_QUEUE_TABLE()parameter error :" + func_field;
    //}
    
    //操作
    BYTE r = 0;
    string err_str("");
    if ((r = GB_AvartarDB->DoQueueTableTask_Main(parent, func_return, add_paras)) != 0) {
        parent->GetLastError(err_str);
    }
    
    return err_str;
}
/*
string ParseProcess::Http_ParseFunc::DO_QUEUE_SEND(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node,timeout_err_node;
    int time_val;

    //操作
    BYTE r = 0;
    string err_str("");
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,2,parent,add_paras)<0){//Param数目错误
        return (string)"@CONN_OBJ_STATE()parameter error :" + func_field;
    }

    SendNodeQueueCell_T *cell=&(parent->doCellObj);
    if (parent->sendQueue->PopItem(parent->doCellObj)){
        //发送操作
        cell->time_out=paras[0].size()==0 ?STRING_Helper::StrToi(paras[1]) :Default_SendQueue_TimeVal ;
        cell->time_out_path=paras[1].size()==0 ? paras[1].c_str(): "_TIMEOUT_ERR";
        cell->time_start =LW_GetTickCount();//取得当前时间
        r=parent->SendCBFunc(cell->PkgBuf.c_str(), cell->PkgBuf.size(), "@THIS_FD", "", cell->trans_type, cell->close_after_send, parent->SendCBFuncHandle);  //???this->SendCBFuncHandle
        if (r < 0)
            err_str="SendQueue_PopSend() error!";
    }

    return err_str;
}

string ParseProcess::Http_ParseFunc::DO_QUEUE_RACT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node;

    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras)<1) {//Param数目错误
    	return (string)"@DO_QUEUE_RACT()parameter error :" + func_field;
    }

    //操作
    BYTE r = 0;
    string err_str("");

    //调用队列中的Pkg操作
    SendNodeQueueCell_T *cell=&(parent->doCellObj);
    if (cell->srcObj==NULL)
        goto end;

    if (cell->srcObj->_PubObj->SetNodeValue("@__PKG", paras[0].c_str(), Http_ParseObj::STR_COPY_OBJ, paras[0].size()) != Http_ParseObj::SUCCESS) {
       err_str="Attribute->can't read'@__PKG'---maybe readonly! ";
    }

    cell->srcObj->FUNCTION(cell->ActNode, add_paras);   //ACTIONS 操作
    cell->srcObj->_PubObj->SetNodeValue("@__PKG", "", Http_ParseObj::STR_COPY_OBJ);

end:
    return err_str;
}

string ParseProcess::Http_ParseFunc::DO_QUEUE_SIZE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node;

    //操作
    BYTE num= 0;
    string err_str("");
    num=parent->sendQueue->GetSize();
    func_return=STRING_Helper::iToStr(num);

    return err_str;
}


string ParseProcess::Http_ParseFunc::CHK_QUEUE_TIMEOUT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node;
    string err_str;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras)<1) {//Param数目错误
        return (string)"@DO_QUEUE_RACT()parameter error :" + func_field;
    }

//查找指定的连接是否有超时处理。
    Conn *dest_conn;
    //判断是否有该链接
    if ((dest_conn = GB_ConnQueue->FindConnByObjName(paras[0].c_str(), 0)) == NULL){
        err_str=(string)"SendQueue_ChkTimeOut()->Send Error: DestObj"+paras[0]+"hasn't find!";
    }else{
        UInt64 now=LW_GetTickCount();//取得当前时间
        SendNodeQueueCell_T *cell_obj=&(dest_conn->ProcessObj->doCellObj);
        if (now>(dest_conn->ProcessObj->doCellObj.time_start+dest_conn->ProcessObj->doCellObj.time_out*1000)){
            //超时处理操作
            ParseXml_Node err_node=dest_conn->ProcessObj->_ActParseXml->Child(dest_conn->ProcessObj->CfgNode,dest_conn->ProcessObj->doCellObj.time_out_path==NULL ? sendQueue_TimeOutErrPath: dest_conn->ProcessObj->doCellObj.time_out_path);
            dest_conn->ProcessObj->FUNCTION(err_node, add_paras);   //ACTIONS 操作
        }
    }

    return err_str;
}

string ParseProcess::Http_ParseFunc::DO_QUEUE_ERR(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node;

    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras)<1) {//Param数目错误
        return (string)"@DO_QUEUE_ERR()parameter error :" + func_field;
    }

    //操作
    BYTE r = 0;
    string err_str("");

    SendNodeQueueCell_T *cell=&(parent->doCellObj);
    while(parent->sendQueue->PopItem(parent->doCellObj)){
        r=parent->SendCBFunc((const char*)cell->PkgBuf.c_str(), cell->PkgBuf.size(), "@THIS_FD", "", cell->trans_type, cell->close_after_send, parent->SendCBFuncHandle);  //???this->SendCBFuncHandle
    }
    return err_str;
}*/


/**
 * safari版websocket响应值计算
 */
/*void websocket_accept_key_old(const char* key1,int key1Len,const char* key2,int key2Len,char* body,char* out){
 char tmp[16];
 unsigned long long kv1=0;
 unsigned long long kv2=0;
 int c1=0;
 int c2=0;
 int i;
 if(key1Len<=0) key1Len=strlen(key1);
 if(key2Len<=0) key2Len=strlen(key2);
 for(i=0;i<key1Len;i++){
 if(key1[i]>='0' && key1[i]<='9') kv1=kv1*10+(key1[i]-'0');
 else if(key1[i]==' ') c1++;
 }
 for(i=0;i<key2Len;i++){
 if(key2[i]>='0' && key2[i]<='9') kv2=kv2*10+(key2[i]-'0');
 else if(key2[i]==' ') c2++;
 }
 kv1/=c1;
 kv2/=c2;
 for(i=3;i>=0;i--){
 tmp[i]=kv1&0xff;
 tmp[i+4]=kv2&0xff;
 kv1=kv1>>8;
 kv2=kv2>>8;
 }
 for(i=0;i<8;i++){
 tmp[i+8]=body[i];
 }
 md5(tmp,16,out);
 out[16]=0;
 }
 */

string ParseProcess::Http_ParseFunc::MASK(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 4, parent, add_paras)<2) {//Param数目错误
        return (string)"@MASK()parameter error :" + func_field;
    }
    
    //具体过滤操作
    string s1=parent->_PubPacket->HBCStrToHBCStr(paras[0],LW_HEX_STR,LW_BINARY_STR,false);
    string s2=parent->_PubPacket->HBCStrToHBCStr(paras[1],LW_HEX_STR,LW_BINARY_STR,false);
    if(paras[0]=="0xffff"){
        func_return="1";
    }else if (s1.size()==1 && s2.size()==1 ){
        unsigned char i1=*(s1.data());
        unsigned char i2=*(s2.data());
        unsigned char t1 = i1&i2;
        func_return=(t1==0 ? "0" : "1");
    }else if (s1.size()==2 && s2.size()==2 ){
        unsigned char i1=*(s1.data());
        unsigned char i3=*(s1.data()+1);
        unsigned char i2=*(s2.data());
        unsigned char i4=*(s2.data()+1);
        unsigned char t1 = i1&i2;
        unsigned char t2 = i3&i4;
        func_return=(((t1==0) && (t2==0)) ? "0" : "1");
    }else if (s1.size()==4 && s2.size()==4){
        string s1=parent->_PubPacket->HBCStrToHBCStr(paras[0],LW_HEX_STR,LW_BINARY_STR,false);
        unsigned char i1=*(s1.data());
        unsigned char i3=*(s1.data()+1);
        unsigned char i5=*(s1.data()+2);
        unsigned char i7=*(s1.data()+3);
        string s2=parent->_PubPacket->HBCStrToHBCStr(paras[1],LW_HEX_STR,LW_BINARY_STR,false);
        unsigned char i2=*(s2.data());
        unsigned char i4=*(s2.data()+1);
        unsigned char i6=*(s2.data()+2);
        unsigned char i8=*(s2.data()+3);
        unsigned char t1 = i1&i2;
        unsigned char t2 = i3&i4;
        unsigned char t3 = i5&i6;
        unsigned char t4 = i7&i8;
        func_return=(((t1==0) && (t2==0) && (t3==0) && t4==0) ? "0" : "1");
    }else{
        return "@MARK(para1,para1,size),only support size 1,2, 4 mask ";
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::EQUALS(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    string temp1,temp2;
    const char *para1,*para2;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()!=2){//Param数目错误
        return (string)"@PLUS()parameter error :Param1: Object  or  String ,Param2: Object  or  String ";
    }
    
    //解析Param1内容
    if (CheckParaQuot(paras[0],temp1)){
        // String Param
        para1=temp1.c_str();
    }else{
        // Object Param
        para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (para1==NULL)
            return parent->_PubObj->ErrorStr;
        temp1=para1;
    }
    //解析Param2内容
    if ( CheckParaQuot(paras[1],temp2)){
        // String Param
        para2=temp2.c_str();
    }else{
        // Object Param
        para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
        if (para2==NULL)
            return parent->_PubObj->ErrorStr;
        temp2=para2;
    }
    
    //具体过滤操作
    if((para1==NULL && para2==NULL) || (temp1==temp2)){
        func_return="1";
    }else{
        func_return="0";
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::PLUS(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 4, parent, add_paras)<2) {//Param数目错误
        return (string)"@PLUS()parameter error :" + func_field;
    }
    
    //实际操作
    func_return = "";
	const char *p1 = paras[2].c_str(),*p2;
	if ((p2=strstr(p1,"h%"))==p1) {
		int i1 = STRING_Helper::ParseInt(paras[0]);
		int i2 = STRING_Helper::ParseInt(paras[1]);
		BYTE i3 = STRING_Helper::ParseInt(p2+2);
        if (i1 == STRING_Helper::NaN || i2 == STRING_Helper::NaN)
            return ("@PLUS(params) error: param1 or param2 is not number!");
		if (i3 == STRING_Helper::NaN)
			return ("@PLUS(h% i) error: i should be number");
		func_return = STRING_Helper::SetNumToID(STRING_Helper::iToStr(i1 + i2, 16), i3);
	}else if ((p2 = strstr(p1, "H%")) == p1) {
		int i1 = STRING_Helper::ParseInt(paras[0]);
		int i2 = STRING_Helper::ParseInt(paras[1]);
		BYTE i3 = STRING_Helper::ParseInt(p2 + 2);
        if (i1 == STRING_Helper::NaN || i2 == STRING_Helper::NaN)
            return ("@PLUS(params) error: param1 or param2 is not number!");
		if (i3 == STRING_Helper::NaN)
			return ("@PLUS(H% i) error: i should be number");
		func_return = "0x";
		func_return += STRING_Helper::SetNumToID(STRING_Helper::iToStr(i1 + i2, 16), i3);
	}
	else if (paras[2].size() == 3 && paras[2] == "hex") {
        if (paras[3].size()!=0 && STRING_Helper::CheckNum(paras[3])) {
            UInt64 i1 = STRING_Helper::scanUINTStr(
                                                   STRING_Helper::HBCStrToHBCStr(paras[0], "hex", "char", false));
            UInt64 i2 = STRING_Helper::scanUINTStr(
                                                   STRING_Helper::HBCStrToHBCStr(paras[1], "hex", "char", false));
            string temp3 = parent->_PubPacket->_iToStr(i1 + i2, 16);
            func_return=(string)"0x"+STRING_Helper::SetNumToID(temp3,STRING_Helper::ParseInt(paras[3]));
        }else{
            UInt64 i1 = STRING_Helper::scanUINTStr(
                                                   STRING_Helper::HBCStrToHBCStr(paras[0], "hex", "char", false));
            UInt64 i2 = STRING_Helper::scanUINTStr(
                                                   STRING_Helper::HBCStrToHBCStr(paras[1], "hex", "char", false));
            func_return = parent->_PubPacket->_iToStr(i1 + i2, 16);
        }
    }
    else if (paras[2] == "fload") {
        double i1 = STRING_Helper::ParseFload(paras[0]);
        double i2 = STRING_Helper::ParseFload(paras[1]);
        func_return = STRING_Helper::dToStr(i1 + i2);
    }else{
        int i1 = STRING_Helper::ParseInt(paras[0]);
        int i2 = STRING_Helper::ParseInt(paras[1]);
		func_return = STRING_Helper::iToStr(i1 + i2, 10);
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::MULT(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 4, parent, add_paras)<2) {//Param数目错误
        return (string)"@MULT()parameter error :" + func_field;
    }
    
    func_return = "";
	const char *p1 = paras[2].c_str(), *p2;
	if ((p2 = strstr(p1, "h%")) == p1) {
		int i1 = STRING_Helper::ParseInt(paras[0]);
		int i2 = STRING_Helper::ParseInt(paras[1]);
		BYTE i3 = STRING_Helper::ParseInt(p2 + 2);
		if (i3 == STRING_Helper::NaN)
			return ("@PLUS(h% i) error: i should be number");
		func_return = STRING_Helper::SetNumToID(STRING_Helper::iToStr(i1 * i2, 16), i3);
	}else if (paras[2] == "hex") {
        UInt64 i1 = STRING_Helper::scanUINTStr(STRING_Helper::HBCStrToHBCStr(paras[0], "hex", "char", false));
        UInt64 i2 = STRING_Helper::scanUINTStr(STRING_Helper::HBCStrToHBCStr(paras[1], "hex", "char", false));
        func_return = STRING_Helper::iToStr(i1*i2, 16);
    }else if (paras[2] == "fload"){
        double i1 = STRING_Helper::ParseFload(paras[0]);
        double i2 = STRING_Helper::ParseFload(paras[1]);
        if (paras[3].size()!=0){
            char str[25];
            double i3 =i1*i2;
            if (i3!=0){
                sprintf(str, paras[3].c_str(), i3);
                func_return=str;
            }else
                func_return="0";
        }else
            func_return = STRING_Helper::dToStr(i1*i2);
    }else {
        int i1 = STRING_Helper::ParseInt(paras[0]);
        int i2 = STRING_Helper::ParseInt(paras[1]);
        func_return = STRING_Helper::iToStr(i1*i2, 10);
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::MINUS(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras; 
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 4, parent, add_paras)<2) {//Param数目错误
        return (string)"@MINUS()parameter error :" + func_field;
    }
    func_return = "";
    
    //实际操作
    if (paras[2] == "hex") {
        UInt64 i1 = STRING_Helper::scanUINTStr(STRING_Helper::HBCStrToHBCStr(paras[0], "hex", "char", false));
        UInt64 i2 = STRING_Helper::scanUINTStr(STRING_Helper::HBCStrToHBCStr(paras[1], "hex", "char", false));
        func_return = parent->_PubPacket->_iToStr(i1-i2, 16);
    }else if ((paras.size() >=3) && paras[2]=="fload"){ //ly mac
        double i1=STRING_Helper::ParseFload(paras[0]);
        double i2=STRING_Helper::ParseFload(paras[1]);
        func_return=STRING_Helper::dToStr(i1-i2);
    }else{
        int i1=STRING_Helper::ParseInt(paras[0]);
        int i2=STRING_Helper::ParseInt(paras[1]);
        func_return=STRING_Helper::iToStr(i1-i2,10);
    }
    //实际操作
    return "";
}
string ParseProcess::Http_ParseFunc::DIVI(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 4, parent, add_paras)<2) {//Param数目错误
        return (string)"@DIVI()parameter error :" + func_field;
    }
    func_return = "";
    
    //实际操作
    if (paras[2] == "hex") {
        UInt64 i1 = STRING_Helper::scanUINTStr(STRING_Helper::HBCStrToHBCStr(paras[0], "hex", "char", false));
        UInt64 i2 = STRING_Helper::scanUINTStr(STRING_Helper::HBCStrToHBCStr(paras[1], "hex", "char", false));
        func_return = parent->_PubPacket->_iToStr(i1/i2, 16);
    }if (paras[2] == "byte%") {
        double i1=STRING_Helper::ParseFload(paras[0]);
        double i2=STRING_Helper::ParseFload(paras[1]);
        string tmp = STRING_Helper::iToStr((i1/i2)*100, 16);
        func_return= (string)STRING_Helper::SetNumToID(tmp, 2);
    }else if (paras[2]=="fload"){ //ly mac
        double i1=STRING_Helper::ParseFload(paras[0]);
        double i2=STRING_Helper::ParseFload(paras[1]);
        //实际操作
        if (paras[3].size()!=0){
            char str[25];
            double i3 =i1/i2;
            if (i3!=0){
                sprintf(str, paras[3].c_str(), i3);
                func_return=str;
            }else
                func_return="0";
        }else
            func_return=STRING_Helper::dToStr(i1/i2);
    }else{
        int i1=STRING_Helper::ParseInt(paras[0]);
        int i2=STRING_Helper::ParseInt(paras[1]);
        //实际操作
        func_return=STRING_Helper::iToStr(i1/i2,10);
    }
    return "";
}

string ParseProcess::Http_ParseFunc::MOD(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 4, parent, add_paras)<2) {//Param数目错误
        return (string)"@DIVI()parameter error :" + func_field;
    }
    func_return = "";
    
    //实际操作
    if (paras[2] == "hex") {
        UInt64 i1 = STRING_Helper::scanUINTStr(STRING_Helper::HBCStrToHBCStr(paras[0], "hex", "char", false));
        UInt64 i2 = STRING_Helper::scanUINTStr(STRING_Helper::HBCStrToHBCStr(paras[1], "hex", "char", false));
        func_return = STRING_Helper::iToStr(i1%i2, 16);
    }else{
        int i1=STRING_Helper::ParseInt(paras[0]);
        int i2=STRING_Helper::ParseInt(paras[1]);
        //实际操作
        func_return=STRING_Helper::iToStr(i1%i2,10);
    }
    return "";
}


string ParseProcess::Http_ParseFunc::NUMBER_TO_FLOAT(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    string temp1,temp2,temp3;
    const char *para1,*para2,*para3;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()!=3){//Param数目错误
        return (string)"@DIVI()parameter error :Param1: Source Data ,Param2:The number of positive and negative decomposition ,Param3:decimal ";
    }
    //解析Param1内容
    if ( CheckParaQuot(paras[0],temp1)){
        // String Param
        para1=temp1.c_str();
    }else{
        // Object Param
        para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (para1==NULL)
            return parent->_PubObj->ErrorStr;
    }
    //解析Param2内容
    if (CheckParaQuot(paras[1],temp2)){
        // String Param
        para2=temp2.c_str();
    }else{
        // Object Param
        para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
        if (para2==NULL)
            return parent->_PubObj->ErrorStr;
    }
    
    //解析Param2内容
    if (CheckParaQuot(paras[2],temp3)){
        // String Param
        para3=temp3.c_str();
    }else{
        // Object Param
        para3= parent->_PubObj->GetNodeValue(paras[2].c_str());
        if (para3==NULL)
            return parent->_PubObj->ErrorStr;
    }
    
    //实际操作
    double i1=STRING_Helper::ParseInt(para1);
    double i2=STRING_Helper::ParseInt(para2);
    double i3=STRING_Helper::ParseInt(para3);
    if (i3==0)		i3=1;//无小数情况
    
    //实际操作
    if (i1>i2)
        func_return=parent->_PubPacket->_dToStr((-1)*(i1-i2)/i3);
    else
        func_return=parent->_PubPacket->_dToStr(i1/i3);
    return "";
}

string ParseProcess::Http_ParseFunc::ERR_CODE_STR(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 3, parent, add_paras)<2) {//Param数目错误
        return (string)"@PLUS()parameter error :" + func_field;
    }
    
    func_return = "";
    //实际操作
    ParseXml_Node node;
    
    string err_path;
    string err_code("0");
    string act_str("");
    int r0, r1;
    if (paras[2] == "bin" || paras[2] == "char") {
        r0 = paras[0].at(0);
        r1 = paras[1].at(0);
        err_code=string("0x") + STRING_Helper::iToStr(r1, 16);
        act_str= string("0x") + STRING_Helper::SetNumToID(STRING_Helper::iToStr(r0, 16),2,"0");
    }
    else{
        if (!STRING_Helper::CheckNum(paras[0],"hex") || !STRING_Helper::CheckNum(paras[1], "hex"))
            return "ERR_CODE_STR(): ErrorCode must be hex number!";
        r0 = STRING_Helper::ParseInt(paras[0]);
        r1 = STRING_Helper::ParseInt(paras[1]);
        err_code = paras[1];
        act_str = paras[0];
    }
    
    
    if (r1<7 || r1>=0x60)
        err_path = (string)ParseProcess_XML_ERROR_CODE_PATH + "/Err[@Act='0x00' and @Code='" + err_code + "']";
    else
        err_path = (string)ParseProcess_XML_ERROR_CODE_PATH + "/Err[@Act='" + act_str +"' and @Code='"+ err_code +"']";
    if ((node=parent->_ActParseXml->GetNodeByXpath(err_path.c_str()))){
        func_return = parent->_ActParseXml->NodeValue(node);
    }
    else {
        func_return = err_code;
    }
    
    return "";
}


string ParseProcess::Http_ParseFunc::ERR_GET_LAST_ERROR(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    //读取Param信息
    func_return = GB_AvartarDB->GET_LAST_ERROR();
    
    return "";
}

string ParseProcess::Http_ParseFunc::CHECK_CONN(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras) < 1) {//Param数目错误
        return (string)"@CHECK_CONN()parameter error :" + func_field;
    }
    
    Conn *conn;
    if ((conn=GB_ConnQueue->FindConnByObjName(paras[0].c_str(), 0,true))!=NULL){
        if (paras[1].size()!=0 && paras[1]=="ex_self"){
            Conn *conn_self=(Conn *)parent->SendCBFuncHandle;
            if (conn==conn_self)
                func_return= "false";
            else
                func_return = "true";
        }else
            func_return = "true";
    }else
        func_return = "false";
    
    //func_return = "true";
    return "";
}

string ParseProcess::Http_ParseFunc::IS_NUM(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 1, parent, add_paras) < 1) {//Param数目错误
        return (string)"@IS_NUM()parameter error :" + func_field;
    }
    
    if (STRING_Helper::CheckNum(paras[0]))
        func_return = "true";
    else
        func_return = "false";
    
    return "";
}

string ParseProcess::Http_ParseFunc::IS_TEXT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 1, parent, add_paras) < 1) {//Param数目错误
        return (string)"@IS_NUM()parameter error :" + func_field;
    }
    const char *p=paras[0].c_str();
    size_t len=paras[0].size();
    bool is_text=true;
    //循环判断是否打印字符
    for (size_t i=0; i<len; i++){
        if ((!isprint(*p)) && !(iscntrl(*p))){
            is_text=false;
            break;
        }
            
        if ((*p)==0)
            break;
        p++;
    };
    if (is_text)
        func_return = "true";
    else
        func_return = "false";
    
    return "";
}


string ParseProcess::Http_ParseFunc::TO_TEXT(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    func_return="";
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras) < 1) {//Param数目错误
        return (string)"@TO_TEXT()parameter error :" + func_field;
    }
    enum CHAR_TYPE{NONE,ASCII,UTF8};
    CHAR_TYPE chg_type=NONE;
    
    if (paras[1]=="ASCII")
        chg_type=ASCII;
    if (paras[1]=="UTF8")
        chg_type=UTF8;
    //循环判断是否打印字符
    size_t i;
    string chg_str="";
    char *p=(char *)(paras[0].c_str());
    size_t len=paras[0].size();
    for (i=0; i<len; i++){
        unsigned char d=(*p);
        if (chg_type==NONE){//截断模式
            if ((d > 0x7e) || (d==0))
                break;
            p++;
        }else if(chg_type==ASCII){//转换模式
            if ((d > 0x7e) || (d==0))
                func_return+='?';
            else
                func_return+=d;
            p++;
        }else if(chg_type==UTF8){//转换模式 https://blog.csdn.net/zison_sun/article/details/5815746
            int nBytes=_TO_TEXT_UTF8_LEN(d);
            if (nBytes==-1 || (i+nBytes)>len || nBytes >3){//差错处理; //已包含if(d<(0xC0)){ // (11000000): 值介于0x80与0xC0之间的为无效UTF-8字符
                func_return+="?";
                p++;
            }else if ((d & 0x80) == 0){//普通英文字符
                func_return+=d;
                p++;
            }else{//UTF8处理
                bool IsUTF8=true;
                if (d<(0xE0)){ // (11100000): 此范围内为2字节UTF-8字符
                    if ((p[1] & (0xC0)) != 0x80)
                        IsUTF8 = false;
                }else if (d<(0xF0)){  // (11110000): 此范围内为3字节UTF-8字符
                    if ((p[1] & (0xC0)) != 0x80 || (p[2] & (0xC0)) != 0x80)
                        IsUTF8 = false;
                }
                for (int j=0; j<nBytes; j++){
                    if (IsUTF8)
                        func_return+=p[j];
                    else
                        func_return+='?';
                }
                p+=nBytes;
            }
        }
    };
    
    if (chg_type==NONE){//只针对截断模式
        func_return.append(paras[0].c_str(),i);
    }
    func_return.push_back('\0');
    
    return "";
}
    
int ParseProcess::Http_ParseFunc::_TO_TEXT_UTF8_LEN(unsigned char chr){
    int nBytes=1;
    if (chr >= 0x80) {
        if (chr >= 0xFC && chr <= 0xFD){
            nBytes = 6;
        }
        else if (chr >= 0xF8){
            nBytes = 5;
        }
        else if (chr >= 0xF0){
            nBytes = 4;
        }
        else if (chr >= 0xE0){
            nBytes = 3;
        }
        else if (chr >= 0xC0){
            nBytes = 2;
        }
        else{
            return -1;
        }
    }
    return nBytes;
};

string ParseProcess::Http_ParseFunc::COUNTER(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 1, parent, add_paras) < 1) {//Param数目错误
        return (string)"@CHECK_CONN()parameter error :" + func_field;
    }
    
    Conn *conn;
    if ((conn=GB_ConnQueue->FindConnByObjName(paras[0].c_str(), 0,true))!=NULL)
        func_return = "true";
    else
        func_return = "false";
    
    //func_return = "true";
    return "";
}

//192.0.0.168:port换算uin64(0x*****)字串。
string ParseProcess::Http_ParseFunc::HEX2IP(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras) < 1) {//Param数目错误
        return (string)"@HEX2IP()parameter error :" + func_field;
    }
    
    //取得U64数值。
    UInt64 ip_port_u64;
    if (paras[1].size()!=0){
        string ip_port_u64_str =STRING_Helper::HBCStrToHBCStr(paras[0], "hex", "char", false);
        ip_port_u64=STRING_Helper::scanUINTStr(ip_port_u64_str);
    }else
        ip_port_u64=STRING_Helper::scanUINTStr(paras[0]);
    
    func_return="";
    for(size_t i=0; i<4; i++){
        int tmp=(ip_port_u64 >> ((3-i)*8+32)) & 0xff;
        func_return += STRING_Helper::iToStr(tmp);
        func_return += (i==3 ? ":" : ".");
    }
    func_return += STRING_Helper::iToStr(ip_port_u64 & 0x00000000ffffffff);
    
    return "";
}

string ParseProcess::Http_ParseFunc::IP2HEX(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras) < 1) {//Param数目错误
        return (string)"@IP2HEX()parameter error :" + func_field;
    }
    
    vector<string> ip_port,ip_hex;
    string dest_hex;
    func_return.clear();
    //区分IP/PORT
    if (paras[0].find(":")>0){
        STRING_Helper::stringSplit(paras[0],ip_port,":");
    }else{
        return "@IP2HEX Paras[0] is not ip:port address";
    }
    
    //区分IP
    STRING_Helper::stringSplit(ip_port[0],ip_hex,".");
    if (ip_hex.size()!=4)
        return "@IP2HEX Paras[0] is not ip(**.**.**.**) address";
    
    UInt64 ip_port_u64=0;
    //(ip[0] << 24) + (ip[1] << 16) + (ip[2] << 8) + ip[3]
    for(size_t i=0; i<4; i++){
        if (!STRING_Helper::CheckNum(ip_hex[i]))
            return (string)"@IP2HEX Paras[0] is not ip_("+ip_hex[i]+") address";
        UInt64 tmp=(UInt64)STRING_Helper::ParseInt(ip_hex[i]);
        UInt64 tmp2=tmp << ((3-i)*8+32);
        ip_port_u64+=tmp2;
    }
    UInt64 port=STRING_Helper::ParseInt(ip_port[1]);
    ip_port_u64+=port;
    
    if (paras[1]=="hex"){
        string tmp=STRING_Helper::putUINTToStr(ip_port_u64,64);
        func_return = STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", paras[1]=="true" ? true : false);
    }else
        func_return = STRING_Helper::putUINTToStr(ip_port_u64,64);
        
    return "";
}

string ParseProcess::Http_ParseFunc::CHECK_ACTION_QUERY(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 5, parent, add_paras)<2) {//Param数目错误
        return (string)"@PLUS()parameter error :" + func_field;
    }
    
    func_return = "";
    if (paras[1] == "ARRAY") {
        //取得传输数组对象
        BB_OBJ *p = (BB_OBJ *)paras[0].data();
        if (BB_GET_OBJ_TYPE(p) != BB_ARRAY)
            return "CHECK_ACTION_QUERY() query is not error";
        BB_OBJ_STRU p_stru = BB_GET_OBJ_STRU(p);
        //循环第一层数组
        for (unsigned int i = 0; i < p_stru.Num && i<(paras.size()-2); i++) {
            //取得数组子对象
            BYTE *obj;
            BB_SIZE_T obj_size;
            BB_GET_ARRAY_OBJ_BY_NUM(p, i, &obj, &obj_size);
            if (BB_GET_OBJ_TYPE((BB_OBJ *)obj) != BB_ARRAY)
                return "CHECK_ACTION_QUERY() query[] is not array";
            BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU((BB_OBJ *)obj);
            //取得对应数组描述
            vector<string> para_array;
            STRING_Helper::stringSplit(paras[2+i].c_str(), para_array, "|");
            //循环第二层数组
            if (obj_stru.Num < para_array.size()) {
                func_return = (string)"[" + STRING_Helper::iToStr(i, 16) + "][" + STRING_Helper::iToStr(obj_stru.Num+1) + "] is empty";
                return "";
            }
            for (unsigned int j = 0; j < obj_stru.Num && j<para_array.size(); j++) {
                //取得数组子对象
                BYTE *item;
                BB_SIZE_T item_size;
                BB_GET_ARRAY_OBJ_BY_NUM(obj, j, &item, &item_size);
                BYTE b_type = BB_GET_OBJ_TYPE((BB_OBJ *)item);
                if (para_array[j].compare(0, 3, "ID") == 0) {
                    if ((b_type == BB_BYTES || b_type == BB_BYTES_EXT))
                        if (para_array[j].size()<4 || para_array[j].compare(3, 1, "0") != 0 || (para_array[j].compare(3, 1, "0") == 0 && item_size != 0))
                            continue;
                    func_return = (string)"[" + STRING_Helper::iToStr(i, 16) + "][" + STRING_Helper::iToStr(j) + "] should be \"0x*****\"!";
                }
                else if (para_array[j].compare(0, 3, "TXT") == 0) {
                    if (BB_IS_OBJ_STRING(item))
                        continue;
                    func_return = (string)"[" + STRING_Helper::iToStr(i, 16) + "][" + STRING_Helper::iToStr(j) + "] should be \"******\"!";
                }
                else if (para_array[j].compare(0, 5, "ARRAY") == 0) {
                    BB_OBJ_STRU arr_obj = BB_GET_OBJ_STRU(item);
                    if (arr_obj.Type == BB_ARRAY && arr_obj.CellObj[0] == BB_NULL)
                        continue;
                    func_return = (string)"[" + STRING_Helper::iToStr(i, 16) + "][" + STRING_Helper::iToStr(j) + "] should be \"ARRAY\"!";
                }
                /*if (para_array[j].compare(0, 3, "TXT") == 0 || para_array[j].compare(0, 3, "ID") == 0) {
                 if ((b_type == BB_BYTES || b_type == BB_BYTES_EXT))
                 if (para_array[j].size()<4 || para_array[j].compare(3, 1, "0") != 0 ||(para_array[j].compare(3, 1, "0") == 0 && item_size != 0))
                 continue;
                 }
                 else if (para_array[j].compare(0, 5, "ARRAY") == 0) {
                 BB_OBJ_STRU arr_obj = BB_GET_OBJ_STRU(item);
                 if (arr_obj.Type == BB_ARRAY && arr_obj.CellObj[0] == BB_NULL)
                 continue;
                 }
                 else if (para_array[j].compare(0, 4, "FLAG") == 0) {
                 string item_str = string((const char*)item, item_size);
                 BYTE type;
                 if (item_str.size() == 0) {
                 func_return = (string)"[" + STRING_Helper::iToStr(i, 16) + "][" + STRING_Helper::iToStr(j) + "] flag not set";
                 return "";
                 }
                 continue;
                 }*/
                
                return "";
            }
        }
    }
    else
        return "CHECK_ACTION_QUERY()ONLY SUPPORT ARRAY";
    
    return "";
}

string ParseProcess::Http_ParseFunc::POW(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    string temp1,temp2,temp3("");
    const char *para1,*para2,*para3;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()<2){//Param数目错误
        return (string)"@POW()parameter error :Param1:Source,Object or String,Param2: square number,Object or  String ,Param3:square number  Default ";
    }
    //解析Param1内容
    if ( CheckParaQuot(paras[0],temp1)){
        // String Param
        para1=temp1.c_str();
    }else{
        // Object Param
        para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (para1==NULL)
            return parent->_PubObj->ErrorStr;
    }
    //解析Param2内容
    if (CheckParaQuot(paras[1],temp2)){
        // String Param
        para2=temp2.c_str();
    }else{
        // Object Param
        para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
        if (para2==NULL)
            return parent->_PubObj->ErrorStr;
    }
    
    //解析Param2内容
    if (paras.size() >=3){
        if (CheckParaQuot(paras[2],temp3)){
            // String Param
            para3=temp3.c_str();
        }else{
            // Object Param
            para3= parent->_PubObj->GetNodeValue(paras[2].c_str());
            if (para3==NULL)
                return parent->_PubObj->ErrorStr;
        }
    }
    
    //实际操作
    int i1=STRING_Helper::ParseInt(para1);
    int i2=STRING_Helper::ParseInt(para2);
    
    //实际操作
    if (i2==0 && paras.size()==3)
        func_return=para3;
    else
        func_return=parent->_PubPacket->_iToStr((int)pow((double)i1,i2),10); //!!ly change 必须有 range 限制,尚无;今后改为dtoStr
    return "";
}

string ParseProcess::Http_ParseFunc::ROUND_TIME_STR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    string temp1;
    const char *para1=NULL;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    
    //解析Param1内容
    if (paras.size()==1){
        if (CheckParaQuot(paras[0],temp1)){
            // String Param
            para1=temp1.c_str();
        }else{
            // Object Param
            para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        }
    }
    if (paras.size()==0)
        STRING_Helper::strRandTime_Us(func_return);
    else if (strstr(para1,"S")!=0)
        STRING_Helper::strRandTime(func_return);
    else
        return (string)"ROUND_TIME_STR--parameter error !,should be 'S' or ''";
    
    return "";
}

string ParseProcess::Http_ParseFunc::IS_TIME_OUT(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    const char *p1;
    string temp1,temp2;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()!=2){//Param数目错误
        return (string)"@PACKET_PARSE_XML()parameter error :param1:Last time,param2: interval ."+func_field;
    }
    //取得Param一
    if ( CheckParaQuot(paras[0],temp1)){
        // String Param
        p1=temp1.c_str();
    }else{
        // Object Param
        p1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (p1==NULL)
            return parent->_PubObj->ErrorStr;
    }
    //取得Param二
    if (CheckParaQuot(paras[1],temp2)){
    }else{
        // Object Param
        parent->_PubObj->ErrorStr="@PACKET_PARSE_XML()parameter error :Param1:Last time,Param2: interval .";
        return parent->_PubObj->ErrorStr;
    }
    //实际操作:注:今后应该加入数值校验
    int beg = atoi(p1);
    int add = atoi(temp2.c_str());
    string now_str(STRING_Helper::strRandTime());
    int now= atoi(now_str.c_str());
    if (now > (beg+add))
        func_return ="true";
    else
        func_return ="false";
    return "";
}


/*string ParseProcess::Http_ParseFunc::ROUND_TIME(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
 
	STRING_Helper::strRandTime(func_return);
	return "";
 }*/

string ParseProcess::Http_ParseFunc::GET_NOW_DATE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    string temp1;
    const char *para1=NULL;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    
    //解析Param1内容
    if (paras.size()==1){
        if (CheckParaQuot(paras[0],temp1)){
            // String Param
            para1=temp1.c_str();
        }else{
            // Object Param
            para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        }
    }
    
    if (para1==NULL){
        STRING_Helper::GetNowDate(func_return);
    }else{
        STRING_Helper::GetNowDate(func_return,para1);
    }
    return "";
}

string ParseProcess::Http_ParseFunc::GET_NOW_TIME(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras_new;
    int paras_num;
    if ((paras_num = ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras_new, 3, parent, add_paras))<0) {//Param数目错误
        return (string)"GET_NOW_TIME() parameter parse error" + func_field;
    }
    
    vector<string> paras;
    string temp1,temp2;
    const char *para1=NULL,*para2=NULL;
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    
    //解析Param1内容
    if (paras.size()>=1){
        if (CheckParaQuot(paras[0],temp1)){
            // String Param
            para1=temp1.c_str();
        }else{
            // Object Param
            para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        }
    }
    //解析Param1内容
    if (paras.size()==2){
        if (CheckParaQuot(paras[1],temp2)){
            // String Param
            para2=temp2.c_str();
        }else{
            // Object Param
            para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
        }
    }
    
    if (para1==NULL)
        STRING_Helper::GetNowTime(func_return);
    else if (strstr(para1,"SEC")==para1){//ly sec
        long t_sec=time(NULL);
        if (para2!=NULL && strstr(para2,"hex")==para2){
            string time_bin=STRING_Helper::putUINTToStr(t_sec,32);
            func_return=STRING_Helper::HBCStrToHBCStr(time_bin, "char", "hex", true);
        }else if (para2!=NULL && strstr(para2,"int")==para2){
            func_return=STRING_Helper::strRandTime();
        }else{
            func_return=STRING_Helper::putUINTToStr(t_sec,32);
        }
    }else if (strstr(para1,"TM7")==para1){//ly sec
        time_t t;
        time(&t);
        if (paras.size()==2 && STRING_Helper::CheckNum(para2)){
            t+=STRING_Helper::ParseInt(para2);//当前时间累加
        }
        tm * lt = localtime(&t);
        UInt16 year=lt->tm_year+1900; 
        UInt8 month=lt->tm_mon+1,mday=lt->tm_mday,wday=lt->tm_wday,hour=lt->tm_hour,minute=lt->tm_min,second=lt->tm_sec;
        string time_bin=STRING_Helper::putUINTToStr(year,16);
        time_bin+=(unsigned char)month; time_bin+=(unsigned char)mday; time_bin+=(unsigned char)wday; time_bin+=(unsigned char)hour;
        time_bin+=(unsigned char)minute; time_bin+=(unsigned char)second;
        if (para2!=NULL && strstr(para2,"hex")==para2){
            func_return=STRING_Helper::HBCStrToHBCStr(time_bin, "char", "hex", true);
        }else{
            func_return=time_bin;
        }
    }else if (strstr(para1,"UTC")==para1){//ly sec
        time_t t;
        time(&t);
        string tmp_str="";
        if (paras_new.size()>=2 && paras_new[1].size()!=0 && STRING_Helper::CheckNum(paras_new[1])){
            if (paras_new[2]=="-")
                t-=STRING_Helper::ParseInt(paras_new[1]);
            else
                t+=STRING_Helper::ParseInt(paras_new[1]);//当前时间累加
        }
        tm * lt = localtime(&t);
        UInt16 year=lt->tm_year+1900;
        UInt8 month=lt->tm_mon+1,mday=lt->tm_mday,wday=lt->tm_wday,hour=lt->tm_hour,minute=lt->tm_min,second=lt->tm_sec;
        tmp_str+=STRING_Helper::iToStr(year)+"-";
        tmp_str+=month < 10 ? ((string)"0"+STRING_Helper::iToStr(month)):STRING_Helper::iToStr(month);
        tmp_str+="-";
        tmp_str+=mday < 10 ? ((string)"0"+STRING_Helper::iToStr(mday)):STRING_Helper::iToStr(mday);
        tmp_str+="T";
        tmp_str+=hour < 10 ? ((string)"0"+STRING_Helper::iToStr(hour)):STRING_Helper::iToStr(hour);
        tmp_str+=":";
        tmp_str+=minute < 10 ? ((string)"0"+STRING_Helper::iToStr(minute)):STRING_Helper::iToStr(minute);
        tmp_str+=":";
        tmp_str+=second < 10 ? ((string)"0"+STRING_Helper::iToStr(second)):STRING_Helper::iToStr(second);
        tmp_str+=".000Z";
        func_return = tmp_str;
    }else if (strstr(para1,"STD")==para1){//ly sec
        time_t t;
        time(&t);
        string tmp_str="";
        if (paras_new.size()>=2 && paras_new[1].size()!=0 && STRING_Helper::CheckNum(paras_new[1])){
            if (paras_new[2]=="-")
                t-=STRING_Helper::ParseInt(paras_new[1]);
            else
                t+=STRING_Helper::ParseInt(paras_new[1]);//当前时间累加
        }
        tm * lt = localtime(&t);
        UInt16 year=lt->tm_year+1900;
        UInt8 month=lt->tm_mon+1,mday=lt->tm_mday,wday=lt->tm_wday,hour=lt->tm_hour,minute=lt->tm_min,second=lt->tm_sec;
        tmp_str+=STRING_Helper::iToStr(year)+"-";
        tmp_str+=month < 10 ? ((string)"0"+STRING_Helper::iToStr(month)):STRING_Helper::iToStr(month);
        tmp_str+="-";
        tmp_str+=mday < 10 ? ((string)"0"+STRING_Helper::iToStr(mday)):STRING_Helper::iToStr(mday);
        tmp_str+=" ";
        tmp_str+=hour < 10 ? ((string)"0"+STRING_Helper::iToStr(hour)):STRING_Helper::iToStr(hour);
        tmp_str+=":";
        tmp_str+=minute < 10 ? ((string)"0"+STRING_Helper::iToStr(minute)):STRING_Helper::iToStr(minute);
        tmp_str+=":";
        tmp_str+=second < 10 ? ((string)"0"+STRING_Helper::iToStr(second)):STRING_Helper::iToStr(second);
        func_return = tmp_str;
    }else if (strstr(para1,"STM6_")==para1){//ly sec
        string tmp_str="";
        time_t t;
        time(&t);
        if (paras.size()==2 && STRING_Helper::CheckNum(para2)){
            t+=STRING_Helper::ParseInt(para2);//当前时间累加
        }
        tm * lt = localtime(&t);
        UInt16 year=lt->tm_year+1900;
        UInt8 month=lt->tm_mon+1,mday=lt->tm_mday,wday=lt->tm_wday,hour=lt->tm_hour,minute=lt->tm_min,second=lt->tm_sec;
        tmp_str+=STRING_Helper::iToStr(year)+"_";
        tmp_str+=month < 10 ? ((string)"0"+STRING_Helper::iToStr(month)):STRING_Helper::iToStr(month);
        tmp_str+="_";
        tmp_str+=mday < 10 ? ((string)"0"+STRING_Helper::iToStr(mday)):STRING_Helper::iToStr(mday);
        tmp_str+="_";
        tmp_str+=hour < 10 ? ((string)"0"+STRING_Helper::iToStr(hour)):STRING_Helper::iToStr(hour);
        tmp_str+="_";
        tmp_str+=minute < 10 ? ((string)"0"+STRING_Helper::iToStr(minute)):STRING_Helper::iToStr(minute);
        tmp_str+="_";
        tmp_str+=second < 10 ? ((string)"0"+STRING_Helper::iToStr(second)):STRING_Helper::iToStr(second);
        func_return = tmp_str;
    }else{
        STRING_Helper::GetNowTime(func_return,para1);
    }
    return "";
}


string ParseProcess::Http_ParseFunc::HBC_TO_HBC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    /*vector<string> paras;
     const char *p1,*p2,*p3,*p4;
     string temp1,temp2,temp3,temp4;
     ParseXml_Node node;
     
     //读取Param信息
     ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
     if (paras.size()!=4){//Param数目错误
     return (string)"@XML_PARSE_PACKET()parameter error :Param1: Source xml Object ,Param2: Source hex char bin,Param3: hex,char,bin,Param4:weather have 0x etc. ";
     }
     //取得Param一: Source  Object ,
     if (CheckParaQuot(paras[0],temp1)){
     // String Param
     p1=temp1.c_str();
     }else{
     // Object Param
     if ((p1=parent->_PubObj->GetNodeValue(paras[0].c_str()))==NULL)
     return parent->_PubObj->ErrorStr;
     }
     
     //取得Param二:Xpath of Node
     if (CheckParaQuot(paras[1],temp2)){
     // String Param
     p2=temp2.c_str();
     }else{
     // Object Param
     if ((p2=parent->_PubObj->GetNodeValue(paras[1].c_str()))==NULL)
     return parent->_PubObj->ErrorStr;
     }
     
     
     //取得Param三:Xpath of Node
     if (CheckParaQuot(paras[2],temp3)){
     // String Param
     p3=temp3.c_str();
     }else{
     // Object Param
     if ((p3=parent->_PubObj->GetNodeValue(paras[2].c_str()))==NULL)
     return parent->_PubObj->ErrorStr;
     }
     
     //取得Param四:Xpath of Node
     if (CheckParaQuot(paras[3],temp4)){
     // String Param
     p4=temp4.c_str();
     }else{
     // Object Param
     if ((p4=parent->_PubObj->GetNodeValue(paras[3].c_str()))==NULL)
     return parent->_PubObj->ErrorStr;
     }*/
    
    vector<string> paras;
    //ParseXml_Node node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,4,parent,add_paras)<1){//Param数目错误
        return (string)"@HBC_TO_HBC()parameter error :Param1: src string ,Param2: from hex char ,Param3: to hex,char,Param4:if add head. ";
    }
    
    //实际操作
    func_return = parent->_PubPacket->HBCStrToHBCStr(paras[0],paras[1],paras[2],paras[3].find("true")!=string::npos ? true:false);
    //cout << func_return.size() << endl;
    return "";
}

string ParseProcess::Http_ParseFunc::HBC_TO_CRC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    const char *p1,*p2,*p3;
    string temp1(""),temp2(""),temp3("");
    ParseXml_Node node;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()<=1){//Param数目错误
        return (string)"@XML_PARSE_PACKET()parameter error :Param1: Source xml Object ,Param2: Begin ,Param3:  End .";
    }
    //取得Param一: Source  Object ,
    if (CheckParaQuot(paras[0],temp1)){
        // String Param
        p1=temp1.c_str();
    }else{
        // Object Param
        if ((p1=parent->_PubObj->GetNodeValue(paras[0].c_str()))==NULL)
            return parent->_PubObj->ErrorStr;
        temp1=p1;
    }
    
    //取得Param二:Xpath of Node
    if (paras.size() >=2 ){
        if(CheckParaQuot(paras[1],temp2)){
            // String Param
            p2=temp2.c_str();
        }else{
            // Object Param
            if ((p2=parent->_PubObj->GetNodeValue(paras[1].c_str()))==NULL)
                return parent->_PubObj->ErrorStr;
            temp2=p2;
        }
    }
    
    
    //取得Param三:Xpath of Node
    if (paras.size() >=3){
        if (CheckParaQuot(paras[2],temp3)){
            // String Param
            p3=temp3.c_str();
        }else{
            // Object Param
            if ((p3=parent->_PubObj->GetNodeValue(paras[2].c_str()))==NULL)
                return parent->_PubObj->ErrorStr;
            temp3 = p3;
        }
    }
    
    //实际操作
    func_return = parent->_PubPacket->HBCStrToCRCStr(temp1, temp2=="" ? 0 : atoi(p2), temp3=="" ? 0 :atoi(p3));
    return "";
}

/*
 string ParseProcess::Http_ParseFunc::BITDB_LEN_CRC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
	vector<string> paras;
	const char *p1,*p2,*p3,*p4;
	string temp1(""),temp2(""),temp3(""),temp4("");
	ParseXml_Node node;
 
	//读取Param信息
	ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
	if (paras.size()<1){//Param数目错误
 return (string)"@BITDB_LEN_CRC()parameter error :Param1: Source xml Object ,Param2: PK ADD LEN ,Param3: CRC Begin ,Param4:  CRC End .";
	}
	//取得Param一: Source  Object ,
	if (!CheckParaQuot(paras[0],temp1)){
 if ((p1=parent->_PubObj->GetNodeValue(paras[0].c_str()))==NULL)
 return parent->_PubObj->ErrorStr;
 temp1=p1;
	}
 
	//取得Param二:Xpath of Node
	if (paras.size() >=2 && CheckParaQuot(paras[1],temp2)){
 if ((p2=parent->_PubObj->GetNodeValue(paras[1].c_str()))==NULL)
 return parent->_PubObj->ErrorStr;
 temp2=p2;
	}
 
 
	//取得Param三:Xpath of Node
	if (paras.size() >=3) && CheckParaQuot(paras[2],temp3)){
 if ((p3=parent->_PubObj->GetNodeValue(paras[2].c_str()))==NULL)
 return parent->_PubObj->ErrorStr;
 temp3 = p3;
	}
 
	//取得Param four:Xpath of Node
	if (paras.size() >=4){
 if (CheckParaQuot(paras[3],temp4)){
 // String Param
 p4=temp4.c_str();
 }else{
 // Object Param
 if ((p4=parent->_PubObj->GetNodeValue(paras[3].c_str()))==NULL)
 return parent->_PubObj->ErrorStr;
 temp4 = p4;
 }
	}
 
	//实际操作
	int crc_start = (temp3=="" ? 0 : atoi(p3));
	int crc_end = (temp4=="" ? 0 :atoi(p4));
	int pk_len = (temp2=="" ? 0 : atoi(p2));
	//replace packet len
	char packet_len_buf[10];
	itoa((temp1.size()-(strlen("[@_PACKET_LEN_REPLACE]")-4))/2+pk_len,packet_len_buf,16);
	string packet_len_str=parent->_PubPacket->SetNumToID((string)packet_len_buf,4,"0");
	STRING_Helper::strReplace(temp1,"[@_PACKET_LEN_REPLACE]",packet_len_str);
 
	//add crc
 
	func_return = temp1+parent->_PubPacket->HBCStrToCRCStr(temp1, crc_start, crc_end)+"0d0a";
	return "";
 }*/


string ParseProcess::Http_ParseFunc::SUB_STR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    //vector<string> paras;
    const char *p1,*p2,*p3=NULL;
    string temp1,temp2,temp3;
    ParseXml_Node node;
    int obj_type=Http_ParseObj::STR_COPY_OBJ;
    
    //读取Param信息
    /*ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
     if (paras.size()< 2){//Param数目错误
     return (string)"@XML_PARSE_PACKET()parameter error :Param1: Source  String ,Param2: Begin ,Param3: Length.";
     }*/
    vector<string> paras;
    int paras_num;
    if ((paras_num = ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras))<2) {//Param数目错误
        return (string)"SUB_STR() parameter error ,Param1:file name,Param2:contents:,Param3: is_append " + paras[0];
    }

    p1 = paras[0].c_str();
    p2 = paras[1].c_str();
    if (paras.size()>=3)
        p3 = paras[2].c_str();
    
    //实际操作
    if (paras.size()>=3 && (strstr(p3,"left")==p3 || strstr(p3,"right")==p3 || strstr(p3,"all")==p3)){
        if (obj_type==Http_ParseObj::STR_COPY_OBJ){
            size_t pos;
            if (paras.size()>=4 && paras[3]=="end")//ly add
                pos=paras[0].rfind(paras[1]);//从后面查找
            else
                pos=paras[0].find(paras[1]);
            //if ((pos=paras[0].find(paras[1]))==string::npos){
            if (pos==string::npos){
                return ((string)"@SUB_STR() not find:"+paras[1]);
            }else if (strstr(p3,"left")==p3){
                func_return = paras[0].substr(0,pos);
            }else if (strstr(p3,"right")==p3){
                func_return = paras[0].substr(pos+paras[1].size());
            }else{
                func_return = paras[0].erase(pos,pos+paras[1].size());
            }
            return "";
        }else
            return ("@SUB_STR() not support Http_ParseObj::STR_POINT_OBJ mode at left|right|all!");
    }else if (paras.size()>=4 && paras[3]=="between"){
        if (obj_type==Http_ParseObj::STR_COPY_OBJ){
            size_t pos;
            if ((pos=paras[0].find(paras[1]))==string::npos){
                return ((string)"@SUB_STR() not find:["+paras[1]+"]!");
            }
            string left=paras[0].substr(pos+paras[1].size());
            if ((pos=left.find(paras[2]))==string::npos){
                return ((string)"@SUB_STR() not find:["+paras[2])+"]!";
            }
            func_return =left.substr(0,pos);
        }else
            return ("@SUB_STR() not support Http_ParseObj::STR_POINT_OBJ mode at left|right|all!");
    }else {
        size_t start=STRING_Helper::ParseInt(p2);
        int len=0;
        if (paras.size()>=3)
            len=STRING_Helper::ParseInt(p3);
        if (obj_type==Http_ParseObj::STR_POINT_OBJ){
            //对于指针型 variable STR_POINT_OBJ
            if (paras.size()>=3){
                string c1((char *)p1+start,len);
                func_return=c1;
            }else{
                string c1((char *)p1+start,len);//???这里len有错!!!
                return ("@SUB_STR() not support Http_ParseObj::STR_POINT_OBJ mode at 2 paras!");
                //func_return=c1;
            }
        }else {
            //对于普通obj variable
            if (start > paras[0].size()){
                return (string)"@SUB_STR() start Param out of  Length  range .";
            }else{
                if (paras.size()>=3){
                    if (paras[0].size()<(size_t)(start)){// String 超出 range 的情况
                        func_return = "";
                    }else{
                        func_return = paras[0].substr(start,len);
                    }
                }else
                    func_return = paras[0].substr(start);
            }
        }
    }
    if(paras.size()>4 && STRING_Helper::PUB_FindStrIs(paras[4],"bin")){//ly table_filter
        func_return=STRING_Helper::HBCStrToHBCStr(func_return, "hex","char", false);
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::CUT_STR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    //const char *p1,*p2,*p3;
    string temp1,temp2,temp3;
    ParseXml_Node node;
    int obj_type=Http_ParseObj::STR_COPY_OBJ;//???上述操作暂时无用
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,4,parent,add_paras)<2){//Param数目错误
        return (string)"@CUT_STR()parameter error :" + paras[0];
    }
    if (paras[3]=="HEX2BIN"){
        paras.clear();
        ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,4,parent,add_paras, PARAS_BITMAP_0X_2_BIN);
    }
    
    //实际操作
    if (paras.size()>=3 && (STRING_Helper::PUB_FindStrIs(paras[2],"left") || STRING_Helper::PUB_FindStrIs(paras[2],"right")|| STRING_Helper::PUB_FindStrIs(paras[2],"split") || STRING_Helper::PUB_FindStrIs(paras[2],"all"))){
        if (obj_type==Http_ParseObj::STR_COPY_OBJ){
            int pos;
            if ((pos=STRING_Helper::PUB_FindStr(paras[0],paras[1]))!=-1){
                return ((string)"@CUT_STR() not find:"+paras[1]);
            }else if (paras[2]=="left"){
                func_return = paras[0].substr(0,pos-1);
            }else if (paras[2]=="right"){
                func_return = paras[0].substr(pos+paras[1].size());
            }else if (paras[2]=="split"){
                vector<string> arr;
                STRING_Helper::stringSplit(paras[0],arr,paras[1]);
                func_return="";
                for(BB_SIZE_T u=0; u<arr.size(); u++)
                    func_return += arr[u];
            }else{
                func_return = paras[0].erase(pos,pos+paras[1].size());
            }
            return "";
        }else
            return ("@CUT_STR() not support Http_ParseObj::STR_POINT_OBJ mode!");
    }else{
        size_t start=STRING_Helper::ParseInt(paras[1]);
        int len=STRING_Helper::ParseInt(paras[2]);
        if (obj_type==Http_ParseObj::STR_POINT_OBJ){
            return "CUT_STR's paras[0] could not be STR_POINT_OBJ";
        }else {
            //对于普通obj variable
            if (start > paras[0].size()){
                return (string)"@CUT_STR() start Param out of  Length  range .";
            }else{
                if (paras.size()>=3){
                    if (paras[0].size()<(size_t)(start+len)){// String 超出 range 的情况
                        func_return = "";
                    }else{
                        func_return = paras[0].substr(start,len);
                    }
                }else
                    func_return = paras[0].substr(start);
            }
        }
    }
    
    return "";
}
string ParseProcess::Http_ParseFunc::SET_NUM_TO_ID(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 3, parent, add_paras)<2) {//Param数目错误
        return (string)"@SET_NUM_TO_ID(str,split)parameter error :str must be hex num and split must be num" + paras[0];
    }
    
    if (!STRING_Helper::CheckNum(paras[0], "hex") || !STRING_Helper::CheckNum(paras[1])) {
        return "SET_NUM_TO_ID(str,split),str must be hex num and split must be num";
    }
    
    int len = STRING_Helper::ParseInt(paras[1]);
    string split;
    if (paras.size() > 2 && paras[2].size() != 0)
        split = paras[2];
    else
        split = "0";
    func_return = "";
    
    func_return = STRING_Helper::SetNumToID(paras[0], len, split.c_str());
    return "";
}

string ParseProcess::Http_ParseFunc::SUB_STR_NUM(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    string temp1,temp2,temp3;
    ParseXml_Node node;
    //int obj_type;
    
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,4,parent,add_paras)<2){//Param数目错误
        return (string)"@SUB_STR_NUM()parameter error :" + paras[0];
    }
    
    if (!STRING_Helper::CheckNum(paras[2])){
        return (string)"@SUB_STR_NUM() Param2 and Param3 must be number .";
    }
    size_t start=STRING_Helper::ParseInt(paras[1]);
    size_t len=STRING_Helper::ParseInt(paras[2]);
    int radix;
    if (paras[3].size()==0)
        radix=16;
    else
        radix=STRING_Helper::ParseInt(paras[3]);
    if (!(radix==16 || radix==10 || radix==2))
        return (string)"@SUB_STR_NUM() Param4(radix) must be 16|10|2 .";
    //对于普通obj variable
    if (start > paras[0].size()){
        return (string)"@SUB_STR_NUM() start is out of  range .";
    }else{
        
        if (paras[0].size()<(size_t)(start+len)){// String 超出 range 的情况
            return (string)"@SUB_STR_NUM() start+len is out of  range .";
        }else{
            if (paras[3].size()!=0)
                func_return = STRING_Helper::iToStr(STRING_Helper::ParseInt(paras[0].substr(start,len),radix));
            else
                func_return = STRING_Helper::iToStr(STRING_Helper::ParseInt(paras[0].substr(start,len),radix));
        }
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::SUB_STR_SPLIT(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras)<3){//Param数目错误
        return (string)"@SUB_STR_SPLIT()parameter error=>param1:src string; param2:split; param3:sel num; param4;HEX2BIN; param5 skip null" + paras[0];
    }
    if (paras[3]=="HEX2BIN"){
        paras.clear();
        ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,4,parent,add_paras, PARAS_BITMAP_0X_2_BIN);
    }

    //实际操作
    vector <string> flags;
    if (paras[4]=="false")
        STRING_Helper::stringSplit(paras[0], flags,paras[1],false);
    else
        STRING_Helper::stringSplit(paras[0], flags,paras[1]);
    
    if (flags.size()==0){
        return (string)"@SUB_STR_SPLIT paras[0] is null";
    }
    
    if (paras[2]=="END")
        func_return =flags[flags.size()-1];
    else if(!STRING_Helper::CheckNum(paras[2]))
        return (string)"@SUB_STR_SPLIT paras[2] should be num or 'END'!";
    else
        func_return =flags[STRING_Helper::StrToi(paras[2])];
    
    return "";
}

string ParseProcess::Http_ParseFunc::STR_SPLIT_FROM_TO(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras)<3){//Param数目错误
        return (string)"@STR_SPLIT_FROM_TO()parameter error=>param1:from hex; param2:to hex; param3:hex byte width" + func_field;
    }
    
    //参数检验
    const char paras_num_type=paras[2].at(0);
    if (!(paras_num_type=='h' || paras_num_type=='H' || paras_num_type=='i'))
        return (string)"STR_SPLIT_FROM_TO() paras[2] should be h*% or H*% or i*%; *(cell_byte width) 1-2-4-8";
    if ((paras[2].size()==0) || (paras[2].at(2))!='%')
        return (string)"STR_SPLIT_FROM_TO() paras[2] should be h*% or H*% or i*%; *(cell_byte width) 1-2-4-8";
    //处理
    if (paras_num_type=='h' || paras_num_type=='H'){
        if (!STRING_Helper::CheckNum(paras[0],"hex") || !STRING_Helper::CheckNum(paras[1],"hex"))
            return (string)"STR_SPLIT_FROM_TO() paras[1] paras[0] should be number!";
        
        //取得宽度。
        string cell_width_str=paras[2].substr(1,1);
        int cell_width=16;
        if (cell_width_str.size()>0 && STRING_Helper::CheckNum(cell_width_str)){
            cell_width=STRING_Helper::ParseInt(cell_width_str)*8;
        }else{
            return (string)"@STR_SPLIT_FROM_TO() paras[2] should be h*%, *(cell_width) is 1-2-4-8 ";
        }
        //取得From、To数字
        string from_bin=STRING_Helper::HBCStrToHBCStr(paras[0], "hex", "char", false);
        UInt64 from = STRING_Helper::scanUINTStr(from_bin);
        string to_bin=STRING_Helper::HBCStrToHBCStr(paras[1], "hex", "char", false);
        UInt64 to = STRING_Helper::scanUINTStr(to_bin);
        
        //循环添加；split 字串
        func_return="";
        for(UInt64 i=from; i<=to; i++){
            string tmp=STRING_Helper::putUINTToStr(i,cell_width);
            if (i==to)
                func_return+=paras[3]+STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", false);
            else
                func_return+=paras[3]+STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", false)+";";
        }
    }else{
        func_return="@STR_SPLIT_FROM_TO() paras[2] i*% not support now!";
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::STR_SPLIT_ARRAY(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras)<2){//Param数目错误
        return (string)"@STR_SPLIT_ARRAY()parameter error :" + paras[0];
    }
    if (paras[3]=="HEX2BIN"){
        paras.clear();
        ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,4,parent,add_paras, PARAS_BITMAP_0X_2_BIN);
    }
    BYTE cell_null = 0x0;
    
    //准备
    if (!STRING_Helper::CheckNum(paras[1])){
        //分拆
        vector <string> flags;
        if (paras[2]=="false")
            STRING_Helper::stringSplit(paras[0], flags,paras[1],false);
        else
            STRING_Helper::stringSplit(paras[0], flags,paras[1]);
        if (flags.size()==0){
            return (string)"@STR_SPLIT_ARRAY paras[0] is null";
        }
        //生成数组
        string tmp("");
        for(size_t i=0; i<flags.size();i++){
            BB_OBJ *buf = BB_NEW_OBJ_STRING(flags[i].c_str(),flags[i].size(),NULL);
            if (buf!=NULL)
                free(buf);
            tmp+=string((const char *)buf,BB_GET_OBJ_SIZE(buf));
        }
        BB_OBJ *dest_obj = BB_NEW_ARRAY(&cell_null, NULL, NULL, 0, tmp.data(), tmp.size(), NULL);
        func_return+=string((const char *)dest_obj,BB_GET_OBJ_SIZE(dest_obj));
        if (dest_obj!=NULL)
            free(dest_obj);
    }else{
        //例子：<SET_OBJ To="TOKEN_ARRAY" Debug="PvIn" SrcType="BIN">[{@STR_SPLIT_ARRAY(CONTENT,'128','64|64','STRING')}]</SET_OBJ>
        int first_len=STRING_Helper::ParseInt(paras[1]);
        int second_array[]={0,0};
        if(paras[2].size()!=0){
            vector<string>seconds;
            STRING_Helper::stringSplit(paras[2],seconds,"|");
            second_array[0]=STRING_Helper::ParseInt(seconds[0]);
            second_array[1]=STRING_Helper::ParseInt(seconds[1]);
            if ((first_len-second_array[1])!=second_array[0])
                return (string)"@SUB_STR_SPLIT paras2=n1+n2 should be equal to paras1.size() ";
        }
        
        //数组操作
        const char* data=paras[0].data();
        string content;
        BB_OBJ *dest_obj;
        size_t start;
        BB_OBJ *buf = (BB_OBJ *)(BB_OBJ *)malloc(first_len + 30);
        for(start=0; start<paras[0].size(); start=start+first_len){
            //循环生成cell;后内存处理在外。
            const char *p=data+start;
            if (second_array[0]==0){
                dest_obj = BB_NEW_BYTES_ALL((void *)p, first_len, buf);
            }else{
                string tmp("");
                const char *p2=p;
                for(int i=0; i<2; i++){
                    BB_OBJ * second=BB_NEW_BYTES_ALL((void *)p2, second_array[i], buf);
                    tmp+=string((const char*) second, BB_GET_OBJ_SIZE(second));
                    p2+=second_array[i];
                }
                dest_obj = BB_NEW_ARRAY(&cell_null, NULL, NULL, 0, tmp.data(), tmp.size(), buf);
            }
            content+=string((const char*) dest_obj, BB_GET_OBJ_SIZE(dest_obj));
        }
        if (buf)
            free(buf);
        //生成外壳
        dest_obj = BB_NEW_ARRAY(&cell_null, NULL, NULL, 0, content.data(), content.size(), NULL);
        func_return=string((const char*) dest_obj, BB_GET_OBJ_SIZE(dest_obj));
        free(dest_obj);
    }
    return "";
}

string ParseProcess::Http_ParseFunc::STR_REPLACE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,9,parent,add_paras)<3){//Param数目错误
        return (string)"@SUB_REPLACE()parameter error :" + paras[0];
    }
    if (paras[4]=="HEX2BIN"){
        paras.clear();
        ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras, PARAS_BITMAP_0X_2_BIN);
    }
    
    //实际操作
    if (paras[3]=="ALL"){
        STRING_Helper::strReplaceAll(paras[0],paras[1],paras[2]);
        if (paras[5].size()!=0 ){
            STRING_Helper::strReplaceAll(paras[0],paras[5],paras[6]);
        }
        if (paras[7].size()!=0){
            STRING_Helper::strReplaceAll(paras[0],paras[7],paras[8]);
        }
    }else
        STRING_Helper::strReplace(paras[0],paras[1],paras[2]);
    
    func_return =paras[0];
    
    return "";
}

string ParseProcess::Http_ParseFunc::STR_BIN_TO_CHAR(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    if (func_field.size()==0)
        return "@STR_BIN_TO_CHAR somthing error1 ";
    
    const char *tmp;
    if ((tmp=parent->_PubObj->GetNodeValue(func_field.c_str()))==NULL)
        return "@STR_BIN_TO_CHAR somthing error1";
    func_return =tmp;
    
    return "";
    /*vector<string> paras;
    string temp1,temp2,temp3("");
    const char *para1,*para2,*para3;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()<2){//Param数目错误
        return (string)"@PLUS()parameter error :Param1:DateBase Name ,Param2: doc ID, Param3:";
    }
    
    //解析Param1内容
    if (CheckParaQuot(paras[0],temp1)){
        // String Param
        para1=temp1.c_str();
    }else{
        // Object Param
        para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (para1==NULL)
            return parent->_PubObj->ErrorStr;
        temp1=para1;
    }
    //解析Param2内容
    if ( CheckParaQuot(paras[1],temp2)){
        // String Param
        para2=temp2.c_str();
    }else{
        // Object Param
        para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
        if (para2==NULL)
            return parent->_PubObj->ErrorStr;
        temp2=para2;
    }
    
    const char *tmp;
    if ((tmp=parent->_PubObj->GetNodeValue(para1))==NULL)
        return "@STR_BIN_TO_CHAR somthing error1";
    func_return =tmp;
    
    return "";*/
}

#ifdef JAWS_CDBM
string ParseProcess::Http_ParseFunc::GET_XML_DB_DOC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    string temp1,temp2,temp3("");
    const char *para1,*para2,*para3;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()<2){//Param数目错误
        return (string)"@PLUS()parameter error :Param1:DateBase Name ,Param2: doc ID, Param3:";
    }
    
    //解析Param1内容
    if (CheckParaQuot(paras[0],temp1)){
        // String Param
        para1=temp1.c_str();
    }else{
        // Object Param
        para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (para1==NULL)
            return parent->_PubObj->ErrorStr;
        temp1=para1;
    }
    //解析Param2内容
    if ( CheckParaQuot(paras[1],temp2)){
        // String Param
        para2=temp2.c_str();
    }else{
        // Object Param
        para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
        if (para2==NULL)
            return parent->_PubObj->ErrorStr;
        temp2=para2;
    }
    
    //解析Param3内容
    if(paras.size()>=3){
        if (CheckParaQuot(paras[2],temp3)){
            // String Param
            para3=temp3.c_str();
        }else{
            // Object Param
            para3= parent->_PubObj->GetNodeValue(paras[2].c_str());
            if (para3==NULL)
                return parent->_PubObj->ErrorStr;
            temp3=para3;
        }
    }
    
    //实际操作
    if (HTTP_Config::DBxml->ReadXML(para2,func_return,para1,temp3=="add_son")!=BA_NOTFOUND){
        string isDel;
        if (HTTP_Config::DBxml->ReadXML_Field(func_return,"FDelete",isDel) && isDel=="true")
            func_return="";
        /*if (HTTP_Config::DBxml->ReadXML_Field(func_return,"FDelete",isDel) && isDel=="true"){
         func_return=="";
         return (string)"XML_DB DOC--'"+para2+"'已删除!";
         }else
         return "";*/
    }else{
        func_return="";
        //return (string)"XML_DB DOC--'"+para1+"'  not exist or '"+para2+"'  not exist!";
    }
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}
#endif
#ifdef SENSOR_DBM
string ParseProcess::Http_ParseFunc::GET_SENSOR_DB_DOC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    string temp1,temp2,temp3("");
    const char *para1,*para2,*para3;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()<2){//Param数目错误
        return (string)"@PLUS()parameter error :Param1:DateBase Name ,Param2:doc ID";
    }
    
    //解析Param1内容
    if (CheckParaQuot(paras[0],temp1)){
        // String Param
        para1=temp1.c_str();
    }else{
        // Object Param
        para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (para1==NULL)
            return parent->_PubObj->ErrorStr;
        temp1=para1;
    }
    //解析Param2内容
    if ( CheckParaQuot(paras[1],temp2)){
        // String Param
        para2=temp2.c_str();
    }else{
        // Object Param
        para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
        if (para2==NULL)
            return parent->_PubObj->ErrorStr;
        temp2=para2;
    }
    
    //解析Param3内容
    if(paras.size()>=3){
        if (CheckParaQuot(paras[2],temp3)){
            // String Param
            para3=temp3.c_str();
        }else{
            // Object Param
            para3= parent->_PubObj->GetNodeValue(paras[2].c_str());
            if (para3==NULL)
                return parent->_PubObj->ErrorStr;
            temp3=para3;
        }
    }
    
    //实际操作
    u_int32_t method;
    if (temp3.size()!=0 && ((method=HTTP_Config::SensorDB->GetMethod(SensorDBManager::READ,para3))== SensorDBManager::DB_METHOD_ERROR)){
        //查询操作
        int result;
        void * pdata = NULL;	unsigned long dataLen;
        if((result = HTTP_Config::SensorDB->ReadRecord(temp3,&pdata, &dataLen,temp1,method))==BA_SUCCESS){
            func_return.assign(static_cast<char *>(pdata),dataLen);
            SAFE_FREE(pdata); pdata = NULL;//释放空间
        }else if (result==BA_NOTFOUND){
            func_return="";
            return (string)"SENSOR_DB DOC--'"+para1+"'  not exist or '"+para2+"'  not exist!";
        }else
            func_return="";
        return (string)"SENSOR_DB DOC--'"+para1+"'System Error at reading time!";
    }else{
        func_return="";
        return (string)"SENSOR_DB DOC--Param3,READ_METHOD Error!";
    }
    
    
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

#endif

string ParseProcess::Http_ParseFunc::IIF(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    string temp1,temp2,temp3,temp4="";
    const char *para1;
    bool condition=true;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()<3){//Param数目错误
        return (string)"@IIF()parameter error :Param1:Conditions,Param2:Actions in success,Param3:Actions in error,Param4:other definition: e.g. 'EMPTY'";
    }
    
    //解析Param1双Param
    //解析Param3内容
    if(paras.size()==4){
        if (CheckParaQuot(paras[3],temp4)){
            // String Param
        }
    }
    
    //解析Param1内容,Conditions判断
    if (CheckParaQuot(paras[0],temp1)){
        // String Param,为空和'1','0';则为错.
        para1=temp1.c_str();
        if (strlen(para1)==0)
            condition=false;//暂时只做为空的判断.
        else{
            string err_str;
            condition= parent->TestConditionStr(temp1,err_str,add_paras);
            if (err_str.size()!=0)
                return err_str;
        }
    }else{
        // Object Param只要为空则为错
        if (!parent->_PubObj->GetNodeValueStr(paras[0].c_str(),temp1))
            condition=false;//暂时只做为空 or   not exist的判断.
        else if(temp1.size()==0 && !STRING_Helper::PUB_FindStrIs(temp4, "EMPTY"))
            condition=false;
    }
    
    //解析Param2内容
    if (condition){
        if ( CheckParaQuot(paras[1],temp2)){
            // String Param
            //para2=temp2.c_str();
        }else{
            // Object Param
            //para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
            //if (para2==NULL)
                //return parent->_PubObj->ErrorStr;
        	if (!parent->_PubObj->GetNodeValueStr(paras[1].c_str(),temp2))
        		return parent->_PubObj->ErrorStr;
        	//para2=temp2.c_str();
        }
        if (temp4.size()!=0 && temp2.size()!=0  && STRING_Helper::PUB_FindStrIs(temp4, "H%")){
            func_return=STRING_Helper::HBCStrToHBCStr(temp2, "char", "hex", true);
        }else if (temp4.size()!=0 && temp2.size()!=0 && STRING_Helper::PUB_FindStrIs(temp4, "h%"))
            func_return=STRING_Helper::HBCStrToHBCStr(temp2, "char", "hex", false);
        else
            func_return = temp2;
    }
    
    //解析Param3内容
    if (!condition){
        if ( CheckParaQuot(paras[2],temp3)){
            // String Param
            //para3=temp3.c_str();
        }else{
            // Object Param
//            para3= parent->_PubObj->GetNodeValue(paras[2].c_str());
//            if (para3==NULL)
//                return parent->_PubObj->ErrorStr;
        	if (!parent->_PubObj->GetNodeValueStr(paras[1].c_str(),temp3))
        	        return parent->_PubObj->ErrorStr;
        }
        if (temp4.size()!=0 && temp3.size()!=0 && STRING_Helper::PUB_FindStrIs(temp4, "H%")){
            func_return=temp3.size()==0 ? "" : STRING_Helper::HBCStrToHBCStr(temp3, "char", "hex", true);
        }else if (temp4.size()!=0 && temp3.size()!=0  && STRING_Helper::PUB_FindStrIs(temp4, "h%"))
            func_return=STRING_Helper::HBCStrToHBCStr(temp3, "char", "hex", false);
        else
            func_return = temp3;
    }
    
    return "";
}

string ParseProcess::Http_ParseFunc::SET_OBJ_VALUE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    string temp1,temp2,temp3,temp4;
    const char *para1,*para2,*para3,*para4;
    int method=Http_ParseObj::STR_COPY_OBJ;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()<3){//Param数目错误
        return (string)"@BINARY_TO_HBC()parameter error :Param1: Object  Name ,Param2: Object Content ,Param3: Config type;Param3:is global variables,";
    }
    
    //解析Param1内容
    if (CheckParaQuot(paras[0],temp1)){
        // String Param
        para1=temp1.c_str();
    }else{
        // Object Param
        para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (para1==NULL)
            return parent->_PubObj->ErrorStr;
        temp1=para1;
    }
    //解析Param2内容
    if ( CheckParaQuot(paras[1],temp2)){
        // String Param
        para2=temp2.c_str();
    }else{
        // Object Param
        para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
        if (para2==NULL)
            return parent->_PubObj->ErrorStr;
        temp2=para2;
    }
    
    //解析Param3内容
    if(paras.size()>=3){
        if (CheckParaQuot(paras[2],temp3)){
            // String Param
            para3=temp3.c_str();
        }else{
            // Object Param
            para3= parent->_PubObj->GetNodeValue(paras[2].c_str());
            if (para3==NULL)
                return parent->_PubObj->ErrorStr;
            temp3=para3;
        }
    }
    //解析Param3内容
    if(paras.size()>=4){
        if (CheckParaQuot(paras[3],temp4)){
            // String Param
            para4=temp4.c_str();
        }else{
            // Object Param
            para4= parent->_PubObj->GetNodeValue(paras[3].c_str());
            if (para4==NULL)
                return parent->_PubObj->ErrorStr;
            temp4=para4;
        }
    }
    
    //具体操作细节
    //if (temp4=="SYS_PUBLIC")
    //HTTP_Config::SysPubObj->SetNodeValue(para1,para2,method);
    //else
    parent->_PubObj->SetNodeValue(para1,para2,method);
    
    func_return="";
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::GET_OBJ_VALUE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    string temp1,temp2;
    const char *para1,*para2;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()<1){//Param数目错误
        return (string)"@BINARY_TO_HBC()parameter error :Param1: Object  Name ,Param2:If global variables ,";
    }
    
    //解析Param1内容
    if (CheckParaQuot(paras[0],temp1)){
        // String Param
        para1=temp1.c_str();
    }else{
        // Object Param
        para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (para1==NULL)
            return parent->_PubObj->ErrorStr;
        temp1=para1;
    }
    
    //解析Param2内容
    if (paras.size()>1){
        if ( CheckParaQuot(paras[1],temp2)){
            // String Param
            para2=temp2.c_str();
        }else{
            // Object Param
            para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
            if (para2==NULL)
                return parent->_PubObj->ErrorStr;
            temp2=para2;
        }
    }
    
    //具体操作细节
    const char *c_ret;
    //if (temp2=="SYS_PUBLIC")
    //c_ret=HTTP_Config::SysPubObj->GetNodeValue(para1);
    //else
    c_ret=parent->_PubObj->GetNodeValue(para1);
    if (c_ret!=NULL)
        func_return=c_ret;
    else
        func_return="";
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::GET_NODE_PATH(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    string temp1,temp2;
    const char *para1,*para2;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    
    //
    func_return=parent->CfgNode.path();
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}


string ParseProcess::Http_ParseFunc::TEST_CONDITION(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    string temp1,temp2;
    const char *para1,*para2;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()<1){//Param数目错误
        return (string)"@BINARY_TO_HBC()parameter error :Param1: Object Name ,Param2: additional Param,";
    }
    
    //解析Param1内容
    if (CheckParaQuot(paras[0],temp1)){
        // String Param
        para1=temp1.c_str();
    }else{
        // Object Param
        para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (para1==NULL)
            return parent->_PubObj->ErrorStr;
        temp1=para1;
    }
    //解析Param2内容
    if (paras.size()>1){
        if ( CheckParaQuot(paras[1],temp2)){
            // String Param
            para2=temp2.c_str();
        }else{
            // Object Param
            para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
            if (para2==NULL)
                return parent->_PubObj->ErrorStr;
            temp2=para2;
        }
    }
    
    
    //具体操作细节
    string test_str;
    
    //解析 variable 操作
    //parent->ParseValue(temp1,test_str,add_paras);
    
    // String 变换操作
    /*vector<string> or_str;
     vector<string> and_str;
     string left,right,condition,err_str("");
     
     STRING_Helper::stringSplit(test_str,or_str,"or");
     int r_or = SWITCH_CASE_TEST_CONDITION_FALSE;
     
     for(size_t i=0; i<or_str.size(); i++){
     STRING_Helper::stringSplit(or_str[i],and_str,"and");
     int r_and = SWITCH_CASE_TEST_CONDITION_TRUE;
     
     for(size_t j=0; j<and_str.size(); j++){
     parent->GetConditionPart(and_str[j],condition,left,right);
     r_and = parent->TestCondition(condition,left,right);
     if (r_and==ACT_ERROR){
     err_str="QueryCondition syntax错误!";
     break;
     }else if (r_and == SWITCH_CASE_TEST_CONDITION_FALSE)
     break;
     }
     
     if (err_str.size()){
     //如果发生错误
     break;
     }else if (r_and==SWITCH_CASE_TEST_CONDITION_TRUE){
     r_or=SWITCH_CASE_TEST_CONDITION_TRUE;
     break;
     }
     }
     //返回赋值
     if (r_or==SWITCH_CASE_TEST_CONDITION_TRUE)
     func_return="true";
     else
     func_return="false";*/
    
    string err_str("");
    if (parent->TestConditionStr(temp1,err_str,add_paras))
        func_return="true";
    else
        func_return="false";
    
    return err_str; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::FIND(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    /*vector<string> paras;
    string temp1,temp2,temp3;
    const char *para1,*para2,*para3;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()<2){//Param数目错误
        return (string)"@FIND()parameter error :Param1: Object  or  String ,Param2: Object  or  String ";
    }
    
    //解析Param1内容
    if (CheckParaQuot(paras[0],temp1)){
        // String Param
        para1=temp1.c_str();
    }else{
        // Object Param
        para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (para1==NULL)
            return parent->_PubObj->ErrorStr;
        temp1=para1;
    }
    //解析Param2内容
    if ( CheckParaQuot(paras[1],temp2)){
        // String Param
        para2=temp2.c_str();
    }else{
        // Object Param
        para2= parent->_PubObj->GetNodeValue(paras[1].c_str());
        if (para2==NULL)
            return parent->_PubObj->ErrorStr;
        temp2=para2;
    }
    
    //解析Param2内容
    if (paras.size()>=3){
        if ( CheckParaQuot(paras[2],temp3)){
            // String Param
            para3=temp3.c_str();
        }else{
            // Object Param
            para3= parent->_PubObj->GetNodeValue(paras[2].c_str());
            if (para3==NULL)
                return parent->_PubObj->ErrorStr;
            temp3=para3;
        }
    }*/
    vector<string> paras;
    //读取Param信息
    int paras_num;
    if ((paras_num=ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,1,parent,add_paras))<1){//Param数目错误
        return (string)"FIND()parameter error ,Param1: Object  or ' String '" + paras[0];
    }
    
    //具体过滤操作
    size_t i;
    //if ((i=temp1.find(temp2))!=string::npos){
    if ((i=paras[0].find(paras[1]))!=string::npos){
        func_return = parent->_PubPacket->_iToStr((int)i,10);
    }else
        func_return =  "-1";
    return "";
    
}

string ParseProcess::Http_ParseFunc::SEEK(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,2,parent,add_paras)<2){//Param数目错误
        return (string)"@SEEK()parameter error=>param1:from hex; param2:to hex; param3:hex byte width" + func_field;
    }
    
    //具体过滤操作
    bool from_end=(paras.size()>=2 && paras[3]=="end") ? true : false;
    size_t pos=(paras.size()>=3 && STRING_Helper::CheckNum(paras[4])) ? STRING_Helper::ParseInt(paras[4]) : 0;

    size_t i= from_end ? paras[0].rfind(paras[1],pos) : paras[0].find(paras[1],pos);
    if (i!=string::npos){
        func_return = parent->_PubPacket->_iToStr((int)i,10);
    }else
        func_return =  "-1";
    return "";
}

string ParseProcess::Http_ParseFunc::SESSION_FUNC(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 5, parent, add_paras) < 1) {//Param数目错误
        return (string)"@AVTDB_FUNC() parameter error : " + paras[0] + "--- Param1: Function Name ,Param2: TableIndex ,Param3: ....";
    }
    //get tableindex->tablepath;
    //cout << "USER_FUNC" << endl;
    func_return = "";
    if (paras[0] == "NEW") {
        int r;
        //string test[100];
        //for (int i = 0; i < 100; i++) {
        //	 GB_SessionBuf->New(test[i], paras[1]);
        //}
        if ((r = GB_SessionBuf->New(func_return, paras[1], paras[2] == "hex" ? true : false)) < 0) {
            return "creat session error! err_code=" + STRING_Helper::iToStr(r);
        }
    }
    else if (paras[0] == "GET") {//ly 910
        int r;
        if ((r = GB_SessionBuf->Get(paras[1], func_return,paras[2] == "hex" ? true : false)) < 0) { //ly table_dir paras[2]
            if (r == -1) {
                func_return = "";
                return "";
                //return (string)"get session error! not found key=" + paras[1];
            }
            else
                return (string)"get session error! key type must be uint64 binary!";
        }
    }
    else if (paras[0] == "DEL_TIME_OUT") {
        if (paras[1].size() == 0)
            GB_SessionBuf->OlderDel(0);
        else {
            if (!STRING_Helper::CheckNum(paras[1]))
                return (string)"@SESSION_FUNC() parameter error :  Param1(time_out): should be num!";
            GB_SessionBuf->OlderDel(STRING_Helper::ParseInt(paras[1]));
        }
        
    }
    else
        return (string)"@SESSION_FUNC('" + paras[0] + "')is not support!";
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::AVTDB_FUNC(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 5, parent, add_paras, PARAS_BITMAP_0X_2_BIN) < 1) {//Param数目错误
        return (string)"@AVTDB_FUNC() parameter error : " + paras[0] + "--- Param1: Function Name ,Param2: TableIndex ,Param3: ....";
    }
    
    //get tableindex->tablepath
    
    //cout << "AVTDB_FUNC" << endl;
    if (paras[0] == "NORMAL_ACT") {
        BYTE err_char = GB_AvartarDB->DoActionMessage(paras[1], paras[2], paras[3].c_str(), func_return, add_paras);
        if (err_char != 0) {
            //string err_code=string((const char*)BB_GET_OBJ_DATA((BB_OBJ*)func_return.data()), BB_GET_OBJ_DATA_LEN((BB_OBJ*)func_return.data()))
            //return "AVTDB_FUNC(NORMAL_ACT) error code:" + STRING_Helper::HBCStrToHBCStr(func_return,"char","hex",true);
            return "";
        }
        else
            return "";
    }
    else if (paras[0] == "SEEK") {
        //ly sort
        if (paras.size() < 4)
            return "SEEK() parameters error!";
        if (paras[2].size() == 0)
            return "SEEK() parameters(2) error! field is empty";
        if (paras[3].size() == 0)
            return "SEEK() parameters(3) error! field_value is empty";
        if (paras[4].size() == 0)
            return "SEEK() parameters(4) error! field_id should be hex";
        
        BB_SIZE_T num;
        BYTE field_id = paras[2].at(0);
        BB_OBJ *obj;
        BB_SIZE_T obj_size;
        if (!GB_AvartarDB->GetTableObject(paras[1], &obj, obj_size)) {
            return "GetTableRecBySortKey(),Load table error!";
        }
        BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
        if ((num = GB_AvartarDB->GetTableRecBySortKey(obj, field_id, (BYTE *)(paras[3].c_str()),paras[3].size())) != BB_SIZE_T_ERROR){
            func_return = GB_AvartarDB->GetTableItemString(&obj_stru, num,paras[4].at(0));
            //以下为测试
            BYTE num_str_buf[8];
            BB_PUT_UINT_TO_BUFF(num_str_buf, num, 2);
            //if (func_return.at(0) != num_str_buf[0] || func_return.at(1) != num_str_buf[1]) {
            string test = string((char *)num_str_buf, 2);
            if (test!=func_return) {
                return "SEEK() rec_num(" + test + ") not equal to NodeID(" + func_return + ")";
            }
            string mac_test = GB_AvartarDB->GetTableItemString(&obj_stru, num, 2);
            if (mac_test != paras[3]) {
                return "SEEK() Seek mac(" + paras[3] + ") not equal to MAC(" + mac_test + ") in rec_num(" + test + ")";
            }
        }
        else
            return "";
    }
    else if (paras[0] == "JSON_ACT") {
        int		para_pos = 2;
        string para_2;
        char *start = (char *)paras[2].c_str();
        int start_pos = 0;
        string tempContent;
        if ((STRING_Helper::C_CmpFirstStr(start, start_pos, "[,", ",", paras[para_pos].size(), true) >= 0)) {
            string query_str = (string) "%B" + paras[para_pos];
            string result_str;
            //转换成二进制流数组
            if (GB_AvartarDB->JsonBArrayToBArray(query_str, para_2, add_paras) < 0) {//ly err_block
                func_return = "";
                return "query data must be json as '[[***]]' ";
            }
            if (paras[4] == "DEBUG") {
                //char err_char = GB_AvartarDB->DoActionMessage(paras[1], para_2, paras[3].c_str(), func_return, add_paras); //ly mac
                GB_AvartarDB->DoActionMessage(paras[1], para_2, paras[3].c_str(), func_return, add_paras);
                if (func_return.size() != 0) {
                    BB_OBJ * src_obj = (BB_OBJ *)func_return.data();
                    string dest;
                    if (GB_AvartarDB->BArrayToJsonArray(src_obj, dest, add_paras) < 0)
                        parent->DEBUGY("[[\"error\"," + GB_AvartarDB->GET_LAST_ERROR() + "]]");
                    else
                        parent->DEBUGY("[" + dest + "]");
                }
                else
                    parent->DEBUGY("[]");
            }else {
                //char err_char = GB_AvartarDB->DoActionMessage(paras[1], para_2, paras[3].c_str(), func_return, add_paras);
                GB_AvartarDB->DoActionMessage(paras[1], para_2, paras[3].c_str(), func_return, add_paras);
            }
            return "";
        }
        else {
            func_return = "";
            return (string)"@AVTDB_FUNC('JARRAY_TO_BARRAY') error: query data must be json as '[[***]]']]!";
        }
    }
    else if (paras[0] == "JSON_QUERY_ACT") {
        int para_pos = 1;
        char *start = (char *)paras[para_pos].c_str(); int start_pos = 0;
        string tempContent;
        if ((STRING_Helper::C_CmpFirstStr(start, start_pos, "[,", ",", paras[para_pos].size(), true) >= 0)) {
            //string query_str = (string) "%B(" + paras[para_pos] + ")";
            string query_str = (string) "%B" + paras[para_pos] + "";
            //转换成二进制流戍卒
            if (GB_AvartarDB->JsonBArrayToBArray(query_str, tempContent, add_paras, 0) < 0) {
                func_return = (string)"[[\"error\",\"" + GB_AvartarDB->GET_LAST_ERROR() + "\"]]";
                return tempContent;
            }
        }
        else if ((STRING_Helper::C_CmpFirstStr(start, start_pos, "{", ",", paras[para_pos].size(), true) >= 0)) {
            //string query_str = (string) "%Q(" + paras[para_pos] + ")";
            string query_str = (string) "%Q" + paras[para_pos];
            if (GB_AvartarDB->JsonQueryToBArray(query_str, tempContent, add_paras) < 0) {
                func_return = (string)"[[\"error\",\"" + GB_AvartarDB->GET_LAST_ERROR() + "\"]]";
                return tempContent;
            }
        }
        else {
            func_return = (string)"[[\"error\" query data must be json as '[[***]]']]!";
            return func_return;
        }
        string packet_head_null = "";
        BYTE err_char = GB_AvartarDB->DoActionMessage(packet_head_null, tempContent, "",func_return, add_paras);
        if (err_char != 0 && err_char != 0xff) {
            func_return = (string)"[[\"ERROR\",\"ErrNum=" + string("") +STRING_Helper::iToStr(err_char) + "\"]]";
            return func_return;
        }
    }
    else if (paras[0] == "JARRAY_TO_BARRAY" || paras[0]=="JSON_TO_BARRAY") {
        int para_pos = 1;
        char *start = (char *)paras[para_pos].c_str(); int start_pos = 0;
        string tempContent;
        /*if ((STRING_Helper::C_CmpFirstStr(start, start_pos, "[\"%A", ",", paras[para_pos].size(), true) >= 0)) {
         string query_str = paras[para_pos].substr(2, paras[para_pos].size()-4);
         if (GB_AvartarDB->JsonArrayToBArray(query_str, tempContent, add_paras) < 0) {
         func_return = (string)"[[\"ERROR\",\"" + GB_AvartarDB->GET_LAST_ERROR() + "\"]]";
         return tempContent;
         }
         }else*/ if ((STRING_Helper::C_CmpFirstStr(start, start_pos, "[,", ",", paras[para_pos].size(), true) >= 0)) {
             string query_str = (string) "%B" + paras[para_pos];
             string result_str;
             //转换成二进制流数组
             if (GB_AvartarDB->JsonBArrayToBArray(query_str, func_return, add_paras) < 0) {
                 func_return = "";
                 return "query data must be json as '[[***]]' ";
             }
             //func_return = string((const char*)BB_GET_OBJ_DATA((BB_OBJ*)result_str.data()), BB_GET_OBJ_DATA_LEN((BB_OBJ *)result_str.data()));//多了一层需要去掉。
         }else {
             func_return = "";
             return (string)"@AVTDB_FUNC('JARRAY_TO_BARRAY') error: query data must be json as '[[***]]']]!";
         }
    }else if (paras[0] == "JOBJ_TO_PUT") {
        int j=STRING_Helper::PUB_FindStr(paras[1],"{");
        int k=STRING_Helper::PUB_FindStr(paras[1],"}");
        int m=STRING_Helper::PUB_FindStr(paras[1],":");
        string add=paras.size()>2 ? paras[2] : "";
        if (k>0 && j>0 && k>j){
            cout <<paras[1].size() << "," << k-j-2 << endl;
            string tmp=paras[1].substr(j+1,k-j-1);
            vector<string> items;
            STRING_Helper::stringSplit(tmp,items,",");
            string r1="\"",r2="[";
            for(BB_SIZE_T i=0; i<items.size(); i++){
                int n=STRING_Helper::PUB_FindStr(items[i],":");
                r1+=add+items[i].substr(1,n-2);
                if (n>0)
                    r2+=items[i].substr(n+1);
                else
                    r2+=string("\"\"");
                if (i<(items.size()-1)){
                    r1+=",";
                    r2+=",";
                }
            }
            r1+="\"";
            r2+="]";
            if (STRING_Helper::PUB_FindStrIs(paras[3],"1"))
                func_return=r1;
            else if (STRING_Helper::PUB_FindStrIs(paras[3],"2"))
                func_return=r2;
            else
                func_return=r1+string(",")+r2;
        }else {
            func_return = "";
            return (string)"@AVTDB_FUNC('JOBJ_TO_PUT') error: query data must be json as '{***:nnnn,***:mmmm}']]!";
        }
    }
    else if (paras[0] == "BARRAY_CHANG") {
        
    }
    else if (paras[0] == "RELOAD_ALL") {
        //删除对象
        delete GB_AvartarDB;
        GB_AvartarDB = new AvartarDBManager("./DBConfig.xml");
        func_return = "";
    }
    else if (paras[0] == "DO_REG_TABLE_MAIN") {
        const char *dest_conn_name = NULL;
        GB_AvartarDB->DoRegTableTask_Main(paras[1], &dest_conn_name, func_return, add_paras);
        if (paras[2].size() != 0 && dest_conn_name != NULL && strlen(dest_conn_name)>0)
            parent->_PubObj->SetNodeValue(paras[2].c_str(), dest_conn_name, Http_ParseObj::STR_COPY_OBJ);
    }
    else if (paras[0] == "DO_REG_TABLE_CALLBACK") {
        const char *dest_conn_name = NULL;
        GB_AvartarDB->DoRegTableTask_CallBack(paras[1], paras[2], &dest_conn_name, func_return, add_paras);
        if (paras[3].size() != 0 && dest_conn_name != NULL && strlen(dest_conn_name)>0)
            parent->_PubObj->SetNodeValue(paras[3].c_str(), dest_conn_name, Http_ParseObj::STR_COPY_OBJ);
    }
    else if (paras[0] == "TRANS_JSON_TO_BARRAY") {
        //const char *dest_conn_name = NULL;
        GB_AvartarDB->CLEAR_LAST_ERROR();
        //string query_str = (string) "%B(" + paras[1] + ")";
        string query_str = (string) "%B" + paras[1];
        if (GB_AvartarDB->JsonBArrayToBArray(query_str, func_return, add_paras, 0) < 0) {
            func_return = "[[\"ERROR\",\"" + GB_AvartarDB->GET_LAST_ERROR() + ",\"]]";
        }

    }
    else if (paras[0] == "BARRAY_TO_JSON") {
        if (paras[1].size() != 0) {
            BB_OBJ * src_obj = (BB_OBJ *)paras[1].data();
            string dest;
            //cout << STRING_Helper::HBCStrToHBCStr(paras[1], "char", "hex", true) << endl;
            if (GB_AvartarDB->BArrayToJsonArray(src_obj, dest, add_paras)<0)
                func_return = "[[\"ERROR\",\"" + GB_AvartarDB->GET_LAST_ERROR() + "\"]]";
            else
                func_return = "[" + dest + "]";
        }else
            func_return = "[]";
    }
    /*else if (paras[0] == "GET_NODE_PARSE_VALUE") {//这里需要专门改进和测试 ly 2016/9/4
        byte tableindex = STRING_Helper::ParseInt(paras[1], 16);
        string table_path("");
        GB_AvartarDB->AddSearchCell(table_path, BB_SEARCH_CONTENT_STX, (const char *)&tableindex, sizeof(tableindex)); //ly add: 改为可以自适应长度
        UInt16 nodeid = STRING_Helper::ParseInt(paras[2], 16);
        string nodeid_str = STRING_Helper::putUINTToStr(nodeid, 16);
        return GB_AvartarDB->GetNodeParseValue(table_path, nodeid_str, func_return, ";"); //参数：nodeid,ValueResultStr(split by ;)
    }*/
    else if (paras[0] == "GET_NODE_PARSE_VALUE") {//这里需要专门改进和测试 ly 2016/9/4
            /*byte tableindex = STRING_Helper::ParseInt(paras[1], 16);
            string table_path("");
            GB_AvartarDB->AddSearchCell(table_path, BB_SEARCH_CONTENT_STX, (const char *)&tableindex, sizeof(tableindex)); //ly add: 改为可以自适应长度
            UInt16 nodeid = STRING_Helper::ParseInt(paras[2], 16);
            string nodeid_str = STRING_Helper::putUINTToStr(nodeid, 16);*/
            return GB_AvartarDB->GetNodeParseValue(paras[1], paras[2], func_return, ";"); //参数：nodeid,ValueResultStr(split by ;)
    }
    else if (paras[0] == "GET_NODE_FIELD_VALUE_BY_NODEID") {
        /*string table_path = STRING_Helper::HBCStrToHBCStr(paras[1], "hex", "char", false);
         string nodeid_str = STRING_Helper::HBCStrToHBCStr(paras[2], "hex", "char", false);
         string bitmap_str = STRING_Helper::HBCStrToHBCStr(paras[3], "hex", "char", false);*/
        
        string temp_str, temp_str2;
        func_return = "";
        GB_AvartarDB->BuildBufferFromDB_OneNode(temp_str, paras[1], paras[2], paras[3], AVARTDB_AUTH_MASK_FULL, BB_TABLE_REC_WR_BITIDS, false);
        BYTE *t_obj = (BYTE *)temp_str.data();
        if (BB_IS_UINT(t_obj)) {
            UInt64 num = BB_GET_UINT_VALUE(t_obj);
            temp_str2 = STRING_Helper::iToStr(num);
        }
        else
            temp_str2.append((const char*)BB_GET_OBJ_DATA(t_obj), BB_GET_OBJ_DATA_LEN(t_obj));
        if (paras.size() > 3 && (paras[4].find("hex") != string::npos)) {
            func_return = parent->_PubPacket->BinaryToHBCStr(temp_str2.data(), temp_str2.size(), "hex", false);
        }
        else {
            func_return = temp_str2;
        }
    }
    else if (paras[0] == "GET_NODE_FIELD_ARRAY_BY_NODEID") {
        if (paras.size()<5)
            return (string)"@AVTDB_FUNC(GET_NODE_FIELD_VALUE_CONDITIONS) parameter error : must >=5";
        /*string table_path = STRING_Helper::HBCStrToHBCStr(paras[2], "hex", "char", false);
         string nodeid_str = STRING_Helper::HBCStrToHBCStr(paras[3], "hex", "char", false);
         string bitmap_str = STRING_Helper::HBCStrToHBCStr(paras[4], "hex", "char", false);*/
        
        func_return = "";
        string tmp_str;
        GB_AvartarDB->BuildBufferFromDB_OneNode(tmp_str, paras[2], paras[3], paras[4], AVARTDB_AUTH_MASK_FULL, BB_TABLE_REC_WR_BITIDS, false);
        if (tmp_str.size() != 0)
            ParseProcess::Http_ParseFunc::_AVTDB_FUNC_BB_ARRAY_TO_OBJ(parent, tmp_str, paras[1], paras.size()>5 ? paras[5].c_str() : "");
    }
    else if (paras[0] == "GET_NODE_FIELD_ARRAY_BY_CONDITIONS") {
        if (paras.size()<5)
            return (string)"@AVTDB_FUNC(GET_NODE_FIELD_VALUE_CONDITIONS) parameter error : must >=5";
        /*string table_path = STRING_Helper::HBCStrToHBCStr(paras[2], "hex", "char", false);
         string conditions = STRING_Helper::HBCStrToHBCStr(paras[3], "hex", "char", false);
         string bitmap_str = STRING_Helper::HBCStrToHBCStr(paras[4], "hex", "char", false);*/
        string cursor = STRING_Helper::HBCStrToHBCStr("0x000000010001", "hex", "char", false);//只读第一条记录
        
        func_return = "";
        string tmp_str;
        GB_AvartarDB->BuildBufferFromDB_AllNode(tmp_str, paras[2], paras[4], paras[3], cursor, AVARTDB_AUTH_MASK_FULL, BB_TABLE_REC_WR_BITIDS);
        
        //取得结果数组到PubObj对象
        if (tmp_str.size() != 0)
            ParseProcess::Http_ParseFunc::_AVTDB_FUNC_BB_ARRAY_TO_OBJ(parent, tmp_str, paras[1], paras.size()>5 ? paras[5].c_str() : "");
    }
    else if (paras[0] == "PUT_NODE_FIELD_VALUE") {
        BB_SIZE_T bufoffset = 0;
        //创建value对象
        BB_OBJ *data_obj = BB_NEW_BYTES_ALL(paras[1].data(), paras[1].size(), NULL);
        string data_str_tmp((const char*)data_obj, BB_GET_OBJ_SIZE(data_obj));
        if (data_obj)
            free(data_obj);
        BB_OBJ cell = BB_NULL;
        data_obj = BB_NEW_ARRAY(&cell, NULL, NULL, 0, data_str_tmp.data(), data_str_tmp.size(), NULL);
        string data_str((const char*)data_obj, BB_GET_OBJ_SIZE(data_obj));
        if (data_obj)
            free(data_obj);
        
        //写入操作
        if (GB_AvartarDB->WriteBufferToDB_OneNode((unsigned char*)data_str.data(), bufoffset, paras[2], paras[3], paras[4], 0xe0)<0) {
            func_return = (string)"ACTION_ERROR_WRITE_ACTION_ONE_NODE_ERROR! at nodeid:" + paras[3];
        };
        func_return = "";
    }
    else if (paras[0] == "COMMIT_ALL") {
        bool debug = (((paras.size() > 2) && (paras[1] == "debug")) ? true: false);
        if (!GB_AvartarDB->DBCommitAll(debug)) {
            func_return = (string)"DB_COMMIT_ALL Error!";
        }else
            func_return = "";
    }
    else
        func_return = (string)"Function '" + paras[0] + "()'is not support!";
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

bool ParseProcess::Http_ParseFunc::_AVTDB_FUNC_BB_ARRAY_TO_OBJ(ParseProcess *parent, string src_arr, string dest_obj, const char *hex_para) {
    if (src_arr.size() != 0) {
        BB_OBJ *item_obj;
        BB_SIZE_T item_obj_size, i = 0;
        BB_OBJ *bytes_obj_p = (BB_OBJ *)src_arr.data(); //取得元素宽度;
        parent->_PubObj->SetNodeValue(dest_obj.c_str(), "", Http_ParseObj::STR_COPY_OBJ);
        
        for (; bytes_obj_p < ((BB_OBJ *)src_arr.data() + src_arr.size()); bytes_obj_p += item_obj_size, i++) {
            item_obj = (BB_OBJ *)bytes_obj_p;
            item_obj_size = BB_GET_OBJ_SIZE(bytes_obj_p);
            //do somthing there;
            if (BB_IS_UINT(item_obj)) {
                UInt64 num = BB_GET_UINT_VALUE(item_obj);
                parent->_PubObj->SetNodeValue((dest_obj + "." + STRING_Helper::iToStr(i)).c_str(), STRING_Helper::iToStr(num).c_str(), Http_ParseObj::STR_COPY_OBJ);
            }
            else {
                if (strstr(hex_para,"hex") >=hex_para) {
                    string temp_str2 = parent->_PubPacket->BinaryToHBCStr((const char*)BB_GET_OBJ_DATA(item_obj), BB_GET_OBJ_DATA_LEN(item_obj), "hex", false);
                    parent->_PubObj->SetNodeValue((dest_obj + "." + STRING_Helper::iToStr(i)).c_str(), temp_str2.c_str(), Http_ParseObj::STR_COPY_OBJ);
                }
                else
                    parent->_PubObj->SetNodeValue((dest_obj + "." + STRING_Helper::iToStr(i)).c_str(), (const char*)BB_GET_OBJ_DATA(item_obj), Http_ParseObj::STR_COPY_OBJ, BB_GET_OBJ_DATA_LEN(item_obj));
            }
        }
    }
    return true;
}
/*
 string ParseProcess::Http_ParseFunc::AVTDB_BUF_WRITE_TO_JENNIC(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
	// Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
	vector<string> paras;
	ParseXml_Node node;
	//读取Param信息
	if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 6, parent, add_paras)<4) {//Param数目错误
 return (string)"@AVTDB_BUF_WRITE_TO()parameter error : " + paras[0] + "--- Param1: Data  Name ,Param2: TableIndex ,Param3: NodeID;Param4:bitmap,Param5:JennicUART";
	}
 
	//具体操作细节,creater mem
	cout << "step1" << endl;
	string result_str;
	if (paras[5] == "false")
 result_str = paras[0];
	else
 parent->_PubObj->AddChildNodesValue(paras[0].c_str(), result_str); //!!!enforce this place;
 
 //string result_str = parent->_PubPacket->HBCStrToHBCStr(result_hex,"hex","char",false);
	byte *buf = (byte *)malloc(result_str.size() + 1);
	UInt32 len = result_str.size();
	memcpy(buf, result_str.data(), len);//今后可改为直接采用data();
 
 //get params
	cout << "step2" << endl;
 
	func_return = "";
	string table_path = parent->_PubPacket->HBCStrToHBCStr(paras[1], "hex", "char", false);
	string bitmap_str = parent->_PubPacket->HBCStrToHBCStr(paras[3], "hex", "char", false);
	string nodeid_str = parent->_PubPacket->HBCStrToHBCStr(paras[2], "hex", "char", false);
	UInt32 temp_len = len;
 
	//string tmp=STRING_Helper::putUINTToStr(0xFFFFFFFF,32);
	BYTE method = 0;
	if (paras[4] == "" || paras[4] == "BITMAP")
 method = 0;
	else if (paras[4] == "BITSTARTEND")
 method = 64;
	else
 method = 0;//BITALL
	string err_str("");
 
	if (paras[2] == "0xFFFFFFFF") {
 //cout << "in:0xffffff" << endl;
 GB_AvartarDB->WriteBufferToDB_AllNodes((byte *)buf, temp_len, table_path, bitmap_str, method, "");
 GB_AvartarDB->SaveDB(table_path);
	}
	else {
 //cout << "in:" << nodeid <<endl;
 UInt32 bufoffset = 0;
 GB_AvartarDB->WriteBufferToDB_OneNode((byte *)buf, bufoffset, table_path, nodeid_str, bitmap_str, method);
 if (bufoffset != len)
 err_str = "Error in WriteBufferToDB_OneNode(), bufoffset!=orign len!";
 else
 GB_AvartarDB->SaveDB(table_path);
	}
	free(buf); //clear mem
 
	return err_str; //该函数始终返回正确,如果为空,则代表文档  not exist.
 }*/

string ParseProcess::Http_ParseFunc::AVTDB_BUF_WRITE_TO_JENNIC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,7,parent,add_paras, PARAS_BITMAP_0X_2_BIN)<4){//Param数目错误
        return (string)"@AVTDB_BUF_WRITE_TO()parameter error : "+paras[0]+"--- Param1: Data  Name ,Param2: TableIndex ,Param3: NodeID;Param4:bitmap,Param5:JennicUART";
    }
    
    if (STRING_Helper::PUB_FindStr(paras[1],"//")==0){
        GB_AvartarDB->GetPathTableIndex(paras[1],paras[1]);
    }
    
    //具体操作细节,creater mem
    //cout << "step1" << endl;
    string result_str;
    //是否否累计子记录
    if (paras[5] == "add_son")
        parent->_PubObj->AddChildNodesValue(paras[0].c_str(),result_str); //!!!enforce this place;
    else
        result_str = paras[0];
    
    //string result_str = parent->_PubPacket->HBCStrToHBCStr(result_hex,"hex","char",false);
    byte *buf = (byte *)malloc(result_str.size()+1);
    UInt32 len = result_str.size();
    memcpy(buf,result_str.data(),len);//今后可改为直接采用data();
    
    //get params
    //cout << "step2" << endl;
    
    func_return="";
    UInt32 temp_len=len;
    
    //string tmp=STRING_Helper::putUINTToStr(0xFFFFFFFF,32);
    BYTE method=0;
    if (paras[4]=="" || paras[4]=="BITMAP")
        method=0;
    else if (paras[4]=="BITSTARTEND")
        method= BB_TABLE_REC_WR_BITSTARTEND;
    else
        method=0;//BITALL
    
    if (STRING_Helper::PUB_FindStrIs(paras[6],"UN_UPLOADFLAG"))
        method=method|BB_TABLE_REC_WR_QUERY_UN_UPLOADFLAG; //ly table_sync3
    
    string err_str("");
    
    BB_SIZE_T bufoffset= 0;
    if ((method & BB_TABLE_REC_WR_BITMARK) == BB_TABLE_REC_WR_BITSTARTEND) { //ly table_sync3 BB_TABLE_REC_WR_BITMARK
        int r;
        if (paras[2] == "0xFFFFFFFF") {
            err_str = "Error in WriteBufferToDB_AllNodes(), WriteBufferToDB_AllNodes()  not support BB_TABLE_REC_WR_BITSTARTEND !";
            //cout << "in:0xffffff" << endl;
            //GB_AvartarDB->WriteBufferToDB_AllNodes((byte *)buf, temp_len, paras[1], paras[3], method, "");
            bufoffset = len;
            r=GB_AvartarDB->WriteBufferToDB_OneNode((byte *)buf, bufoffset, paras[1], paras[2], paras[3], method);
        }
        else {
            bufoffset = len;
            r=GB_AvartarDB->WriteBufferToDB_OneNode((byte *)buf, bufoffset, paras[1], paras[2], paras[3], method);
        }
        if (r < 0) {
            //差错
            err_str = string("Error in write,err_num:") + string("-") + STRING_Helper::iToStr((BYTE)(AvartarDBManager::ACTION_ERROR_WRITE_ACTION_ONE_NODE_ERROR - r));
            func_return = "";
        }
        else {
            func_return = paras[2];	//取得写入节点号
            //if (paras[6]=="save")
            if (STRING_Helper::PUB_FindStrIs(paras[6],"save"))
                GB_AvartarDB->SaveDBNode(paras[1], paras[2], FileQueue::SAVE_SYNC); //ly table_sync3
        }
        
    }
    else {
        func_return = "";
        bufoffset = 0;
        if (paras[2] == "0xFFFFFFFF") {
            GB_AvartarDB->WriteBufferToDB_AllNodes((byte *)buf, temp_len, paras[1], paras[3], method, "");
            if (temp_len != len)
                err_str = "Error in WriteBufferToDB_AllNodes(), bufoffset !=orign len!";
        }
        else {
            GB_AvartarDB->WriteBufferToDB_OneNode((byte *)buf, bufoffset, paras[1], paras[2], paras[3], method);
            if (bufoffset != len)
                err_str = "Error in WriteBufferToDB_OneNode(), bufoffset!=orign len!";
        }
        //if (paras[6]=="save")
        if (STRING_Helper::PUB_FindStrIs(paras[6],"save"))
            GB_AvartarDB->SaveDB(paras[1]);//ly table_sync3
    }
    
    free(buf); //clear mem
    
    return err_str; //该函数始终返回正确,如果为空,则代表文档  not exist.
}
string ParseProcess::Http_ParseFunc::AVTDB_BUF_WRITE_TO(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras)<4){//Param数目错误
        return (string)"@AVTDB_BUF_WRITE_TO()parameter error : "+paras[0]+"--- Param1: Data  Name ,Param2: TableIndex ,Param3: NodeID;Param4:bitmap,Param5:JennicUART";
    }
    
    //具体操作细节,creater mem
    cout << "step1" << endl;
    string result_str;
    parent->_PubObj->AddChildNodesValue(paras[0].c_str(),result_str); //!!!enforce this place;
    //string result_str = parent->_PubPacket->HBCStrToHBCStr(result_hex,"hex","char",false);
    byte *buf = (byte *)malloc(result_str.size()+1);
    UInt32 len = result_str.size();
    memcpy(buf,result_str.data(),len);//今后可改为直接采用data();
    
    //get params
    cout << "step2" << endl;
    BB_SIZE_T temp_len=len;
    //new
    string table_path=STRING_Helper::HBCStrToHBCStr(paras[1],"hex","char",false);
    UInt32 nodeid=STRING_Helper::ParseInt(paras[2],16);
    string nodeid_str=STRING_Helper::putUINTToStr(nodeid,32);
    UInt64 bitmap=((UInt64)(STRING_Helper::ParseULong(paras[3],16)))<<32;//把32字段提升为64字段;???今后在此处加入新的查询bitmap长短的协议判断。
    string bitmap_str=STRING_Helper::putUINTToStr(bitmap,64);
    string conditions=STRING_Helper::HBCStrToHBCStr(paras[5],"hex","char",false);
    BYTE flag_method=(STRING_Helper::ParseInt(paras[4],16));
    
    if (nodeid==0xFFFFFFFF || conditions.size()!=0){
        //cout << "in:0xffffff" << endl;
        GB_AvartarDB->WriteBufferToDB_AllNodes((byte *)buf, temp_len, table_path, bitmap_str,flag_method,conditions);
        GB_AvartarDB->SaveDB(table_path);
    }else{
        //cout << "in:" << nodeid <<endl;
        GB_AvartarDB->WriteBufferToDB_OneNode((byte *)buf, temp_len, table_path, nodeid_str, bitmap_str,flag_method);
        GB_AvartarDB->SaveDB(table_path);
    }
    
    free(buf); //clear mem
    func_return="";
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}
string ParseProcess::Http_ParseFunc::AVTDB_BUF_READ_FROM(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,8,parent,add_paras)<3){//Param数目错误
        return (string)"@AVTDB_BUF_READ_FROM()parameter error :"+paras[0]+"---Param1: Nodeid ,Param2: tableindex ,Param3: method Para4:add_method";
    }
    
    //具体操作细节
    string table_path;
    if (STRING_Helper::PUB_FindStr(paras[1],"//")==0)
        GB_AvartarDB->GetPathTableIndex(paras[1],table_path);
    else
        table_path=STRING_Helper::HBCStrToHBCStr(paras[1],"hex","char",false);
    //
//    UInt32 nodeid=STRING_Helper::ParseInt(paras[0],16);
//    string nodeid_str=STRING_Helper::putUINTToStr(nodeid,32);
    UInt32 nodeid=STRING_Helper::ParseULong(paras[0],16);
    string nodeid_str=STRING_Helper::HBCStrToHBCStr(paras[0], "hex", "char", false); //ly table_dir error
    
    BB_BITMAP_TYPE bitmap=sizeof(BB_BITMAP_TYPE)==64 ? ((UInt64)(STRING_Helper::ParseULong(paras[2], 16)))<<32 :((UInt64)(STRING_Helper::ParseULong(paras[2], 16)));//把32字段提升为64字段;???今后在此处加入新的查询bitmap长短的协议判断。ly_table_dir
    string bitmap_str=sizeof(BB_BITMAP_TYPE)==64 ? STRING_Helper::putUINTToStr(bitmap,64) : STRING_Helper::putUINTToStr(bitmap,32);
    string conditions;
    if (paras[3]=="0" || paras[3].size()==0)
        conditions="";
    else
        conditions=STRING_Helper::HBCStrToHBCStr(paras[3],"hex","char",false);
    
    BYTE flag_method,flag_src=0;
    if (STRING_Helper::CheckNum(paras[5]))
        flag_method=(STRING_Helper::ParseInt(paras[5],16));
    else{//ly table_dir
        if (!GB_AvartarDB->_ParseActionMessage_ActFlag(paras[5], &flag_method, &flag_src)) {
            return "paras[5] flag is incorrect!";
        }
    }
    //
    string cursor;
    if (paras[6]=="0")
        cursor="";//0x0
    else
        cursor=STRING_Helper::HBCStrToHBCStr(paras[6],"hex","char",false);
    
    func_return="";
    string temp_str("");
    
    //添加SearchSTX头：描述查询方式。
    
    //string table_path("");
    //GB_AvartarDB->AddSearchCell(table_path,BB_SEARCH_CONTENT_STX,(const char *)paras[1].c_str(),paras[1].size());
    //conditions=
    if (paras.size()>3 && (paras[4].find("bin")!=string::npos)){
        if (nodeid==0xFFFFFFFF || conditions.size()!=0)
            GB_AvartarDB->BuildBufferFromDB_AllNode(func_return,table_path, bitmap_str,conditions,cursor, AVARTDB_AUTH_MASK_FULL,flag_method,paras[6]=="ADD_NODEID" ? true : false);
        else{
            GB_AvartarDB->BuildBufferFromDB_OneNode(func_return,table_path, nodeid_str,bitmap_str, AVARTDB_AUTH_MASK_FULL,flag_method,paras[6]=="ADD_NODEID" ? true : false);
        }
    }else{
        string temp_str("");
        if (nodeid==0xFFFFFFFF || conditions.size()!=0)
            GB_AvartarDB->BuildBufferFromDB_AllNode(temp_str,table_path, bitmap_str,conditions,cursor, AVARTDB_AUTH_MASK_FULL,flag_method);
        else{
            GB_AvartarDB->BuildBufferFromDB_OneNode(temp_str,table_path, nodeid_str,bitmap_str, AVARTDB_AUTH_MASK_FULL,flag_method,paras[6]=="ADD_NODEID" ? true : false);
        }
        func_return = parent->_PubPacket->BinaryToHBCStr(temp_str.data(),temp_str.size(),"hex",false);
    }
    
    //cout << func_return << endl;
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::THIRD_SEND_MSG(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,2,parent,add_paras)<1){//Param数目错误
        return (string)"@THIRD_SEND_MSG() parameter error : "+paras[0]+"--- Param1: Third System Name ,Param2: Send Message";
    }
    //
    string sys=paras[0];
    paras.erase(paras.begin());
#ifdef THIRD_QQ
    return GB_ThirdMap[sys]->SendMsg(paras,func_return);
#endif
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::THIRD_RCV_MSG(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,1,parent,add_paras)<1){//Param数目错误
        return (string)"@THIRD_RCV_MSG() parameter error : "+paras[0]+"--- Param1: Third System Name ";
    }
#ifdef THIRD_QQ
    return GB_ThirdMap[paras[0]]->RcvMsg(func_return);
#endif
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::THIRD_FUNC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras)<1){//Param数目错误
        return (string)"@THIRD_FUNC() parameter error : "+paras[0]+"--- Param1: Function Name ,Param2: TableIndex ,Param3: ....";
    }
    string sys=paras[0];
    paras.erase(paras.begin());
#ifdef THIRD_QQ
    return GB_ThirdMap[paras[0]]->Func(paras,func_return);
#endif
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

//ly call_func
string ParseProcess::Http_ParseFunc::CALL_FUNC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras)<1){//Param数目错误
        return (string)"@CALL_FUNC() parameter error : "+paras[0]+"--- Param1: Function Name ,Param2: TableIndex ,Param3: ....";
    }
    //取得节点
    ParseXml_Node actions_node;
    string xpath=string("/XMLParse/FUNCTION/")+paras[0];
    ParseXml_NodeSet nodes = parent->_ActParseXml->GetNodeSetByXpath(xpath.c_str());
    if (nodes.size()==0)
        return string("can't find definiation of FUNCTION Name='")+paras[0]+"!";
    else if (nodes.size()>1)
        return string("FUNCTION Name='")+paras[0]+"'is multiple definition!";
    else{
        //取得真实函数 node
        ParseXml_NodeSet::const_iterator it = nodes.begin();
        pugi::xpath_node xnode = *it;
        actions_node = xnode.node();
    }
    
    //FUNCTION 操作
    parent->FUNCTION(actions_node, add_paras);

    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::GB_PARAM_SET(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,3,parent,add_paras)<2){//Param数目错误
        return (string)"@GB_PARAM_SET()parameter "+paras[0]+"---error :Param1:param name; Param2:param value";
    }
    
    //具体操作细节
    if (paras[2].size()==0)
        GB_Gateway->SetParam(paras[0].c_str(),paras[1].data(),paras[1].size(),Http_ParseObj::STR_COPY_OBJ);
    else if (paras[2]=="append"){
        string tmp_str;
        GB_Gateway->GetParamValueStr(paras[0].c_str(),tmp_str);
        tmp_str.append(paras[1]);
        GB_Gateway->SetParam(paras[0].c_str(),tmp_str.data(),tmp_str.size(),Http_ParseObj::STR_COPY_OBJ);
    }else if (paras[2]=="plus"){
        string tmp_str="";
        if (GB_Gateway->GetParamValueStr(paras[0].c_str(),tmp_str)){
            if (STRING_Helper::CheckNum(tmp_str) && STRING_Helper::CheckNum(paras[1])){
                int old=STRING_Helper::ParseInt(tmp_str);
                old+=STRING_Helper::ParseInt(paras[1]);
                tmp_str=STRING_Helper::iToStr(old);
                GB_Gateway->SetParam(paras[0].c_str(),tmp_str.data(),tmp_str.size(),Http_ParseObj::STR_COPY_OBJ);
                return "";
            }else
                return (string)"@GB_PARAM_SET("+paras[0]+",plus) value should be num";
        }else{
            return (string)"@GB_PARAM_SET("+paras[0]+") obj is not exited";
        }
    }else if (paras[2]=="minus"){
        string tmp_str="";
        if (GB_Gateway->GetParamValueStr(paras[0].c_str(),tmp_str)){
            if (STRING_Helper::CheckNum(tmp_str) && STRING_Helper::CheckNum(paras[1])){
                int old=STRING_Helper::ParseInt(tmp_str);
                old-=STRING_Helper::ParseInt(paras[1]);
                tmp_str=STRING_Helper::iToStr(old);
                GB_Gateway->SetParam(paras[0].c_str(),tmp_str.data(),tmp_str.size(),Http_ParseObj::STR_COPY_OBJ);
                return "";
            }else
                return (string)"@GB_PARAM_SET("+paras[0]+",plus) value should be num";
        }else{
            return (string)"@GB_PARAM_SET("+paras[0]+") obj is not exited";
        }
    }
    //GB_PARAM.SetNodeValue(paras[0].c_str(), paras[1].data(), Http_ParseObj::STR_COPY_OBJ, paras[1].size());
    
    //cout << func_return << endl;
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::HTTP_UNESCAPE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras)<1) {//Param数目错误
        return (string)"@HTTP_UNESCAPE()parameter " + paras[0] + "---error :Param1:source string;";
    }
    string tmp;
    STRING_Helper::HTTP_UnEscString(paras[0].c_str(), tmp);
    if (STRING_Helper::HTTP_IsUTF8((char *)tmp.c_str(),tmp.size()))
        STRING_Helper::UTF_8ToGB2312(func_return, (const char*)tmp.c_str());
    else
        func_return = tmp;
    
    //STRING_Helper::HTTP_UnEscape(paras[0].c_str(), func_return);
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::HTTP_ESCAPE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras)<1) {//Param数目错误
        return (string)"@HTTP_ESCAPE()parameter " + paras[0] + "---error :Param1:source string;";
    }
    
    STRING_Helper::HTTP_EscString(paras[0].c_str(), func_return);
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::HTTP_BASE64_ENCODE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {//ly table_dir
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras)<1) {//Param数目错误
        return (string)"@HTTP_BASE64_ENCODE()parameter " + paras[0] + "---error :Param1:source string;";
    }
    func_return =base64_encode((unsigned char const*)(paras[0].c_str()),paras[0].size());
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::HTTP_BASE64_DECODE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {//ly table_dir
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras)<1) {//Param数目错误
        return (string)"@HTTP_BASE64_DECODE()parameter " + paras[0] + "---error :Param1:source string;";
    }
    
    func_return = base64_decode(paras[0]);
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::HTTP_GB2312ToUTF8(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras)<1) {//Param数目错误
        return (string)"@GB_PARAM_SET()parameter " + paras[0] + "---error :Param1:source string;";
    }
    
    STRING_Helper::GB2312ToUTF_8(func_return,paras[0].c_str());
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}


string ParseProcess::Http_ParseFunc::IS_UTF8(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {//ly shop
	// Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
	vector<string> paras;
	ParseXml_Node node;
	//读取Param信息
	if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras)<1) {//Param数目错误
		return (string)"@GB_PARAM_SET()parameter " + paras[0] + "---error :Param1:source string;";
	}

	bool is_utf8=STRING_Helper::HTTP_IsUTF8((char *)(paras[0].c_str()),paras[0].size());
	func_return = is_utf8 ? "true" : "false";
	return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::HTTP_UTF8ToGB2312(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息  
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras)<1) {//Param数目错误
        return (string)"@GB_PARAM_SET()parameter " + paras[0] + "---error :Param1:source string;";
    }
    
    STRING_Helper::UTF_8ToGB2312(func_return, paras[0].c_str());
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::GB_PARAM_VALUE(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,1,parent,add_paras)!=1){//Param数目错误
        return (string)"@GB_PARAM_VALUE()parameter error :Param1: Gb_param_name";
    }
    
    //具体操作细节
    func_return="";
    GB_Gateway->GetParamValueStr(paras[0].c_str(),func_return);
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::GET_CONN_PARAM(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // Config  Object 值,包括全局 variable .{@STR_LEN({*******})}
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,2,parent,add_paras)<2){//Param数目错误
        return (string)"@GB_PARAM_VALUE()parameter error :Param1: Gb_param_name";
    }
    //取得CONN
    func_return="";
    Conn *conn;
    if (paras[0]!="@THIS_FD"){
        if ((conn=GB_ConnQueue->FindConnByObjName(paras[0].c_str(), 0,true))==NULL){
            return (string)"CONN("+paras[0]+") not valid!";
        }
    }else
        conn=(Conn *)(parent->SendCBFuncHandle);
    
    //取得变量
    if (conn->ProcessObj->GetParamValueStr(paras[1].c_str(),func_return)==false){
        func_return="";
    }
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

#define GET_CONN_COUNT_MAX 0xFFFF

string ParseProcess::Http_ParseFunc::CNT_CONN_PARAM(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras)<4){//Param数目错误
        return (string)"@CNT_CONN_PARAM()parameter error :Param1: Gb_param_name";
    }
    //取得CONN
    Conn *conn;
    func_return="";
    if ((conn=GB_ConnQueue->FindConnByObjName(paras[0].c_str(), 0,true))==NULL)
        return (string)"CONN("+paras[0]+") not valid!";
    
    //取得变量
    string value_str;
    if (!conn->ProcessObj->GetParamValueStr(paras[1].c_str(),value_str))
        value_str="0";
    
    //数字处理
    if (!STRING_Helper::CheckNum(value_str))
        return (string)"PARAM("+paras[1]+")VALUE("+value_str+") should be num!";
    
    //value++
    int num=STRING_Helper::ParseInt(value_str);
    int max= STRING_Helper::CheckNum(paras[2]) ? GET_CONN_COUNT_MAX : STRING_Helper::ParseInt(paras[2]);
    num= (num==max) ? num=0 : num+1;

    //value to str
    if (STRING_Helper::CheckNum(paras[2],"hex") && STRING_Helper::CheckNum(paras[3])){
        string tmp_value_str = STRING_Helper::iToStr(num,16);
        func_return = STRING_Helper::SetNumToID(tmp_value_str, STRING_Helper::ParseInt(paras[3]), "0");
        string tmp="0x";
        conn->ProcessObj->SetParam(paras[1].c_str(),tmp+=func_return);
    }else{
        func_return = STRING_Helper::iToStr(num);
        //reset value
        conn->ProcessObj->SetParam(paras[1].c_str(),func_return);
    }

    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::SYS_GET_MAC(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras)<1){//Param数目错误
        return (string)"@GB_PARAM_VALUE()parameter error :Param1: Gb_param_name";
    }
    //取得MAC
    if (paras[0]=="HEX" || paras[0]=="hex"){
        string mac,mac_bin;
        LW_GetLocalMac(mac);
        vector<string> arr;
        STRING_Helper::stringSplit(mac,arr,":");
        if (paras[0]=="HEX")
            func_return="0x0000";
        else
            func_return="0000";
        for(BB_SIZE_T u=0; u<arr.size(); u++)
            func_return += arr[u];
    }else
        LW_GetLocalMac(func_return);
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}


#if !defined(__ANDROID__) && !defined(TARGET_OS_IPHONE) && !defined(TARGET_IPHONE_SIMULATOR)
string ParseProcess::Http_ParseFunc::SYS_CMD(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras)<1){//Param数目错误
        return (string)"@GB_PARAM_VALUE()parameter error :Param1: Gb_param_name";
    }
	//cout << paras[0].c_str() << endl;
	if (paras[1].size()==0) {
		//执行命令
		func_return=STRING_Helper::iToStr(system(paras[0].c_str()));
	}else {
		//执行命令
        string cmd_str;
        if (STRING_Helper::PUB_FindStrIs(paras[0],"{")){
            string cmd_str1;
            string rt=parent->ParseValue(paras[0],cmd_str1,add_paras);
            if (rt.size()!=0){
                func_return = "CMD Config (" + paras[0] + ") paras {*} error!";//判断文件是否已经存在
                return "";
            }
            cmd_str = cmd_str1+">"+paras[1];
        }else{
            cmd_str = paras[0]+ " >" + paras[1];
        }
        //清理文件
        remove(paras[1].c_str());
        //执行命令
		int i=system(cmd_str.c_str());
		//存文件操作
		func_return = "";
		if (LW_access(paras[1].c_str(), 0) < 0) {
			func_return = "Return code:" + STRING_Helper::iToStr(i) + "!";
		}
		else {
			//读取文件操作
			std::fstream f(paras[1].c_str());
			if (f.good())
			{
				std::ostringstream tmp;
				tmp << f.rdbuf();
				if (func_return.size() == 0 && i != 0) {
					func_return = "Error code:"+STRING_Helper::iToStr(i)+"!";
				}else
                    func_return=tmp.str();
                f.close();
			}else
				func_return = "Output file" + paras[1] + " open error!" + STRING_Helper::iToStr(i);//判断文件是否已经存在
		}
	}
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::SYS_EXIT(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    vector<string> paras;
    ParseXml_Node node;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field,paras,5,parent,add_paras)<1){//Param数目错误
        return (string)"@GB_PARAM_VALUE()parameter error :Param1: Gb_param_name";
    }
    
    BYTE exit_code=0;
    if (paras[0].size()!=0 && STRING_Helper::CheckNum(paras[0])){
        exit_code=(BYTE)STRING_Helper::ParseInt(paras[0]);
    }
    exit(exit_code);
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}
//ly exec
#ifdef WIN32
	SHELLEXECUTEINFO *ParseProcessFunc_OldShExecInf;
	SHELLEXECUTEINFO ParseProcessFunc_ShExecInf[] = { 0 };
#else	
	FILE *GB_fp_open=NULL;
#endif

string ParseProcess::Http_ParseFunc::EXEC_CMD(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
	vector<string> paras;
	ParseXml_Node node;
	//bool open_cmd;
	//读取Param信息
	if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 5, parent, add_paras)<1) {//Param数目错误
		return (string)"@GB_PARAM_VALUE()parameter error :Param1: Gb_param_name";
	}
	//cout << paras[0].c_str() << endl;
#ifdef WIN32
	if(ParseProcessFunc_OldShExecInf !=NULL)
	{
		//关闭程序
		if (ParseProcessFunc_OldShExecInf->hProcess != NULL) {
			TerminateProcess(ParseProcessFunc_OldShExecInf->hProcess, 0);
			ParseProcessFunc_OldShExecInf->hProcess = NULL;
		}
		free(ParseProcessFunc_OldShExecInf);//释放空间
	}
	SHELLEXECUTEINFO *shExecInfo = new SHELLEXECUTEINFO();
	//SHELLEXECUTEINFO *shExecInfo = ParseProcessFunc_ShExecInf;
	shExecInfo->cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo->fMask = SEE_MASK_NOCLOSEPROCESS;
	shExecInfo->hwnd = NULL;
	shExecInfo->lpVerb = _T("open");
	wchar_t *file = STRING_Helper::StringToLPCWSTR(paras[0].c_str());
	shExecInfo->lpFile = (wchar_t *)file;
	wchar_t *parameters= STRING_Helper::StringToLPCWSTR(paras[1].c_str());
	shExecInfo->lpParameters = (wchar_t *)parameters;
	//shExecInfo->lpDirectory = _T("C:\\PROGRA~1\\INTERN~1\\");
	shExecInfo->lpDirectory = NULL;
	shExecInfo->nShow = SW_SHOW;
	shExecInfo->hInstApp = NULL;
	
	//open_cmd=ShellExecuteEx(shExecInfo);
	ShellExecuteEx(shExecInfo);
	string task_id = STRING_Helper::iToStr((int)shExecInfo->hInstApp);
	if (!parameters)
		delete[]parameters;
	if (!file)
		delete[]file;
	if (paras[2]!="NO_CLOSE")
		ParseProcessFunc_OldShExecInf = shExecInfo;

#else
	if (GB_fp_open!=NULL)
		pclose(GB_fp_open);

	if ((GB_fp_open = popen(paras[0].c_str(), "r")) == NULL) {
		perror("Fail to popen\n");
        cout << "EXEC_CMD failure !" <<endl;
	}
#endif

	return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}
#endif

//#ifndef WIN32
string ParseProcess::Http_ParseFunc::POPEN(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node;
    //bool open_cmd;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 5, parent, add_paras)<1) {//Param数目错误
        return (string)"@GB_PARAM_VALUE()parameter error :Param1: Gb_param_name";
    }
#ifndef WIN32
    FILE *fp_open;
    if ((fp_open = popen(paras[0].c_str(), paras[1].size()==0 ? "r" : "w")) == NULL) {
        func_return= "";//不成功
    }else
        func_return=STRING_Helper::putUINTToStr((uint64)fp_open,8*sizeof(size_t));//成功

	return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
#else
	func_return = "";
	return "POPEN not support in Windows system!";
#endif
}
/*
#define PARSE_PROCESS_POPEN_READ_MAX 1024
string ParseProcess::Http_ParseFunc::POPEN_READ(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node;
    //bool open_cmd;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 3, parent, add_paras)<1) {//Param数目错误
        return (string)"@POPEN_READ()parameter error :Param1: Gb_param_name";
    }
    FILE *fp_open;
    if (paras[0].size()==4){
        fp_open=(FILE *)STRING_Helper::scanUINT32((BYTE *)(paras[0].c_str()),0);
    }else
    fp_open=(FILE *)STRING_Helper::scanUINT64((BYTE *)(paras[0].c_str()),0);
    if (fp_open==NULL)
    return (string)"@POPEN_READ() paras[0] is Null!";
    
    //取得是否清楚换行符
    bool no_rt=true,read_null2;
    if(paras[3]=="INC_RN")
    no_rt=false;
    //取得读取行数。
    int lnum=1;
    if (paras[2]=="ALL" && STRING_Helper::CheckNum(paras[2]))
    lnum=PARSE_PROCESS_POPEN_READ_MAX;
    else
    lnum=STRING_Helper::ParseInt(paras[1]);
    //读取包长度
    int read_len=1024;
    if (paras[1].size()!=0 && STRING_Helper::CheckNum(paras[1]))
    read_len=STRING_Helper::ParseInt(paras[1])+1;
    
    //读取
    char *buf_ps=(char *)malloc(read_len);
    func_return="";
    int read_null_num=0;
    while(1){
        if (lnum==0 || read_null_num==2)
        break; //如果发现达到了最大行数；或连续读取2次为空
        if (fgets(buf_ps, read_len, fp_open)!=NULL){
            //读取一行,有内容：
            if (no_rt){//转换回车符,过滤
                size_t len=strlen(buf_ps)-1;
                if (buf_ps[len]=='\n')
                buf_ps[len]=0;
                if (buf_ps[len-1]=='\r')
                buf_ps[len-1]=0;
            }
            func_return.append(buf_ps);
        }else{
            //读取一行无内容：
            read_null_num++;
            func_return="_END_";
        }
        lnum--;
    }
    if (buf_ps)
    free(buf_ps);
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}*/

string ParseProcess::Http_ParseFunc::POPEN_READ(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node;
    //bool open_cmd;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras)<1) {//Param数目错误
        return (string)"@GB_PARAM_VALUE()parameter error :Param1: Gb_param_name";
    }
    FILE *fp_open;
    if (paras[0].size()==4){
        fp_open=(FILE *)STRING_Helper::scanUINT32((BYTE *)(paras[0].c_str()),0);
    }else
        fp_open=(FILE *)STRING_Helper::scanUINT64((BYTE *)(paras[0].c_str()),0);
    
    bool no_rt=true;
    //读取
    int read_len=1024;
    
    if (paras[1].size()!=0 && STRING_Helper::CheckNum(paras[1]))
        read_len=STRING_Helper::ParseInt(paras[1])+1;
    
    //读取
    char *buf_ps=(char *)malloc(read_len);
    if (fgets(buf_ps, read_len, fp_open)!=NULL){
        if (no_rt){
            size_t len=strlen(buf_ps)-1;
            if (buf_ps[len]=='\n')
                buf_ps[len]=0;
            if (buf_ps[len-1]=='\r')
                buf_ps[len-1]=0;
        }
        func_return=buf_ps;
    }else
        func_return="_END_";
    if (buf_ps)
        free(buf_ps);
    
    return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
}

string ParseProcess::Http_ParseFunc::POPEN_CLOSE(string func_field, string &func_return, ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras) {
    vector<string> paras;
    ParseXml_Node node;
    //bool open_cmd;
    //读取Param信息
    if (ParseProcess::Http_ParseFunc::FieldParseCheckQuot(func_field, paras, 2, parent, add_paras)<1) {//Param数目错误
        return (string)"@GB_PARAM_VALUE()parameter error :Param1: Gb_param_name";
    }
#ifndef WIN32
	FILE *fp_open;
	if (paras[0].size() == 4)
		fp_open = (FILE *)STRING_Helper::scanUINT32((BYTE *)(paras[0].c_str()), 0);
	else
		fp_open = (FILE *)STRING_Helper::scanUINT64((BYTE *)(paras[0].c_str()), 0);

	pclose(fp_open);

	return ""; //该函数始终返回正确,如果为空,则代表文档  not exist.
#else
	func_return = "";
	return "PCLOSE not support in Windows system!";
#endif
    
}

//#endif

string ParseProcess::Http_ParseFunc::GET_HTTP_HEADER(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    string temp1;
    const char *para1;
    
    //读取Param信息
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    if (paras.size()!=1){//Param数目错误
        return (string)"@BINARY_TO_HBC()parameter error :Param1:HTTP HEADER  String ";
    }
    
    //解析Param1内容
    if (CheckParaQuot(paras[0],temp1)){
        // String Param
        para1=temp1.c_str();
    }else{
        // Object Param
        para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
        if (para1==NULL)
            return parent->_PubObj->ErrorStr;
    }
    
    //实际操作
    //const char *head = parent->Request->headers()[(const char *)para1].value();
    /*if (head!=NULL){
     func_return=head;
     return "";
     }else{
     func_return="";
     return "";
     }*/
    parent->getHttpHeader((const char *)para1,func_return);
    return "";
    //return (string)"header'"+para1+"'  not exist!";
}

string ParseProcess::Http_ParseFunc::DEBUGY(string func_field,string &func_return,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras){
    // String 转换.{@STR_LEN({*******})}
    vector<string> paras;
    //string temp1,temp2,temp3;
    //const char *para1,*para2,*para3;
    
    /*//读取Param信息
     ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
     if (paras.size()!=1){//Param数目错误
     return (string)"@BINARY_TO_HBC()parameter error :Param1:HTTP HEADER  String ";
     }
     
     //解析Param1内容
     if (CheckParaQuot(paras[0],temp1)){
     // String Param
     para1=temp1.c_str();
     }else{
     // Object Param
     para1= parent->_PubObj->GetNodeValue(paras[0].c_str());
     if (para1==NULL)
     return parent->_PubObj->ErrorStr;
     }
     
     //实际操作
     const char *head = parent->Request->headers()[(const char *)para1].value();*/
    
    func_return="";
    return "";
    //return (string)"header'"+para1+"'  not exist!";
}

//对于Param的预处理,首先根据,分组,然后去除\".
int ParseProcess::Http_ParseFunc::FieldParse(string &func_field, vector<string>  &paras){
    STRING_Helper::stringSplit(func_field,paras,",");
    /*for(size_t i=0; paras.size()>i;i++){
     //判断是否为
     STRING_Helper::strReplace(paras[i],"\"","");
     };*/
    return 0;
}
//对于单引号和双引号Param和 variable 的分离
int ParseProcess::Http_ParseFunc::FieldParseCheckQuot(string &func_field, vector<string>  &paras, int paras_num,ParseProcess *parent, ParseProcess::PV_AddParas_T &add_paras,UInt16 bitmap){
    
    //split src paras
    ParseProcess::Http_ParseFunc::FieldParse(func_field,paras);
    int paras_len = (int)paras.size();
    
    //get converted paras
    string temp;
    for(size_t i=0;i<paras.size();i++){
        //取得Param二:Xpath of Node
        STRING_Helper::strTrim(paras[i]);
        bool finded=false;
        //cout << paras[i] << endl;
        int i1=STRING_Helper::PUB_FindStr(paras[i],"\"");
        int i2=STRING_Helper::PUB_FindStr(paras[i],"'");
        //判断内外，切割
        if( i1!=-1 && i2!=-1){//""在外
            if (i1<i2)
                finded=STRING_Helper::strFastCut(paras[i],(string)"\"",(string)"\"",0);
            else
                finded=STRING_Helper::strFastCut(paras[i],(string)"'",(string)"'",0);
        }else if (i1!=-1 && i2==-1){//‘在外
            finded=STRING_Helper::strFastCut(paras[i],(string)"\"",(string)"\"",0);
        }else if (i1==-1 && i2!=-1)
            finded=STRING_Helper::strFastCut(paras[i],(string)"'",(string)"'",0);
        //cout << paras[i] << endl;
        //取值
        if(!finded){
            if (!(parent->_PubObj->GetNodeValueStr(paras[i].c_str(),paras[i]))){// Object Param
					string tmp=paras[i]+" not valid!";
					paras[0]=tmp;
					return -1;
            }
            continue;
        }else if ((bitmap==PARAS_BITMAP_0X_2_BIN) && (paras[i].compare(0, 2, "0x") == 0 || paras[i].compare(0, 2, "0X") == 0)) {
            paras[i] = STRING_Helper::HBCStrToHBCStr(paras[i], "hex", "char",false); //自动转换处理
        }
        
        //string param;
    }
    
    //add empty;
    for(int l=(int)paras.size(); l<paras_num; l++){
        paras.push_back((string)"");
    }
    
    return paras_len;
}

//对于单引号和双引号Param和 variable 的分离
bool ParseProcess::Http_ParseFunc::CheckParaQuot(string &paras,string &temp_s){
    STRING_Helper::strTrim(paras);
    //if (paras.find("\"")!=0 && paras.find("\'")!=0 )
    //return false;//如果起止不为"",''
    
    return (STRING_Helper::strStringCutFrom(paras,temp_s,"\"","\"",0)==STRING_Helper::FINDED || STRING_Helper::strStringCutFrom(paras,temp_s,"'","'",0)==STRING_Helper::FINDED);
}


/*8888888888888888*/
ParseProcess::Http_ParsePacket::Http_ParsePacket(Http_ParseXML *parse_xml,ParseProcess *parentProcess){
    //初始化解析文档 Object
    this->objXmlDoc = parse_xml;
    this->objXml=this->objXmlDoc->FirstChild(this->objXmlDoc->ObjXml);
    
    this->rootNode="/XMLParse/ParsePacket/";
    this->parentProcess=parentProcess;
    
    //这里还需添加初始化destXml Object . or 者在每次Parse的时候初始化.!!
    this->destXmlDoc = new Http_ParseXML();
    this->destXmlDoc->Load("<Root></Root>");
    this->destXml = this->destXmlDoc->FirstChild(this->destXmlDoc->ObjXml);
    //其它补充
    this->doDebug=false;
}

ParseProcess::Http_ParsePacket::~Http_ParsePacket(){
    if (this->destXmlDoc)
    	delete this->destXmlDoc;
}

//======================//解析主函数===========================
const char *ParseProcess::Http_ParsePacket::GetParseType(const char *xpath, ParseXml_Node from_node){
    ParseXml_Node ps;
    string path,test_array="";
    
    for (ParseXml_Node tool = this->objXmlDoc->FirstChild(from_node); tool; tool = this->objXmlDoc->NextChild(tool)){
        if (tool==this->objXmlDoc->FirstChild(from_node))
            test_array+="";
        else
            test_array+=" and ";
        test_array+=this->objXmlDoc->NodeName(tool);
        test_array+="='";
        test_array+=this->objXmlDoc->NodeValue(tool);
        test_array+="'";
    }
    
    //取得解析描述 node
    if (xpath[0]=='/' || xpath[0]=='\\')
        path=xpath;
    else
        path=this->rootNode+xpath;
    
    if ((ps=this->_GetNode(this->objXml,path+"["+test_array+"]"))){
        return this->_NodeAttr(ps,"Name");
    }else if ((ps=this->_GetNode(this->objXml,path+"/_OriginToField_"+"["+test_array+"]"))){
        ParseXml_Node parent=ps.parent();
        return this->_NodeAttr(parent,"Name");
    }else
        return NULL;
}

//函数:取得ParsePacket解析 node
//Param:通过parse_node路径取得具体的配置 node ,parse_type补充二级分支
ParseXml_Node ParseProcess::Http_ParsePacket::GetParseNode(const char *parse_node, const char *parse_type){
    ParseXml_Node ps;
    
    //取得解析描述 node
    if (strlen(parse_node)==0)
        return PARSE_PACKET_NODE_NULL;
    
    if (this->_TypeOf(parse_node,"string")){
        if (parse_node[0]=='/' ||parse_node[0]=='\\')
            ps=this->_GetNode(this->objXml,(string)parse_node+"/*");
        else
            ps=this->_GetNode(this->objXml,this->rootNode+"*[@Name='"+parse_node+"']");
    }else
        return ps;
    
    return this->_GetParseNode(ps,parse_type);
}



ParseXml_Node ParseProcess::Http_ParsePacket::_GetParseNode(ParseXml_Node parse_node, const char *parse_type){
    ParseXml_Node ps;
    string ptype;
    
    if (!parse_node)
        return PARSE_PACKET_NODE_NULL;
    
    if (this->_IsNull(parse_type))
        ptype="_FieldToOrigin_";
    else
        ptype=parse_type;
    
    //对于描述 node 有（_OriginToField_、_FieldToOrigin_的情况）
    if ((ps=this->_GetNode(parse_node,ptype))){
        if (!this->_GetNode(parse_node,"_FieldToOrigin_") || !this->_GetNode(parse_node,"_OriginToField_") ){
            PUB_Error(this->S1+" node ["+this->_NodeName(parse_node)+"] Irregular,{_OriginToField_} and {_FieldToOrigin_} must available seemtime!");
            return PARSE_PACKET_NODE_NULL;
        }
        return ps;
    }else
        return parse_node;
}

/***************************XML 对象解析法 Start ********************************************/

//函数:正向解析:从字符流 node 中取得系列字段值 Object .
//Param:parse_node:解析配置xml node  or 者路径；origin_str:原始 String ；dest_node生成目标xml的根 node ；其它为内部中转 variable .
bool ParseProcess::Http_ParsePacket::GetFieldsFormOriginStr(const char *parse_path, string &origin_str, ParseXml_Node dest_node,string this_path){
    ParseXml_Node ps;
    this->nowParsePath=this_path;
    
    if ((ps=this->GetParseNode(parse_path,"_OriginToField_"))){
        this->_GetFieldsFormOriginStr(ps,origin_str,dest_node);
        return true;
    }else{
        this->ErrorStr=this->S1+" node ["+parse_path+"] can't found!";
        PUB_Error(this->ErrorStr);
        return false;
    }
}


bool ParseProcess::Http_ParsePacket::GetFieldsFormOriginStr(ParseXml_Node parse_node, string &origin_str, ParseXml_Node dest_node,string this_path){
    ParseXml_Node ps;
    this->nowParsePath=this_path;
    
    if ((ps=this->_GetParseNode(parse_node,"_OriginToField_"))){
        //lyadd
        this->_GetFieldsFormOriginStr(ps,origin_str,dest_node);//old
        return true;
    }else{
        this->ErrorStr=this->S1+" parse node not found!";
        PUB_Error(this->ErrorStr);
        return false;
    }
}

bool ParseProcess::Http_ParsePacket::_GetFieldsFormOriginStr(ParseXml_Node ps, string &origin_str, ParseXml_Node dest_node){
    string origin_str_bit,ps_name,temp1,to_obj("");
    ParseXml_Node pn;
    ParseObjT p=this->parseObj;
    DestObjT d=this->destObj;
    ParseXml_Node n;
    string hex_head;
    
    ps_name=this->_NodeAttr(ps,"Name");
    hex_head=this->_NodeAttr(ps,"HexHead");
    
    
    //把原始字段统一转换为二进制字段.
    if (origin_str=="")
        return false;
    origin_str_bit = this->HBCStrToHBCStr(origin_str,"",LW_BIT_STR,false);
    this->OriginStr = this->HBCStrToHBCStr(origin_str,"",LW_HEX_STR,false); //js
    if (origin_str!="" && origin_str_bit==""){
        //PUB_Error(this->S1+" node ["+origin_str+"] Out of original Number String, irregular Number!");
        this->ErrorStr=this->S1+" node ["+origin_str+"] Out of original Number String, irregular Number!";
        PUB_Error(this->ErrorStr);
        return false;
    }
    
    //cout << "GetFieldsFormOriginStr....=> origin_str_bit." << endl;;
    //循环递进累加 node
    d.start=d.len=d.end=0;
    for(ParseXml_Node pn = ps.first_child(); pn; pn = pn.next_sibling()){
        //???判断__类字段不予解析
        const char * node_name = pn.name();
        if (strstr(node_name,"__")==node_name)
            continue;
        //读取解析字段 node 相关信息
        this->_GetParseFieldObj(pn,p);//通过配置 node 取得相关 attribute Param,并适当_ParseValue所以需要from_node.
        
        if (p.test!="" && (p.test=="false" || p.test=="0"))//ly:js no syn
            continue;
        
        if (p.len=="" && p.end=="" && p.start==""){//js
            //如果是直接赋值
            n=this->_AddNode(dest_node,p.name,true);
            this->_SetNodeValue(n,p.value);
            this->DEBUGY(n);
            continue;
        }
        
        //解析换算字段位置Param
        if (!this->GetFieldBitPosition(p,d,this->_StrLen(origin_str_bit))){
            //???
            return false;
        }
        
        //三值合法性校验
        if((d.end-d.start)<-1 || d.end >= this->_StrLen(origin_str_bit) || d.start<0){
            this->ErrorStr=this->S1+" node ["+p.name+"] Out of original range of  String '"+origin_str+"', start:"+this->_iToStr(d.start,10)+" end:"+this->_iToStr(d.end,10)+" len:"+this->_iToStr(d.len,10);
            PUB_Error(this->ErrorStr);
            //PUB_Error(this->S1+" node ["+p.name+"] Out of original range of  String '"+origin_str+"', start:"+this->_iToStr(d.start,10)+" end:"+this->_iToStr(d.end,10)+" len:"+this->_iToStr(d.len,10));
            return false;
        }
        
        //从原始字段中取得字段值:
        
        if (p.result_eval.size()!=0){//ly:js no syn
            temp1 = origin_str_bit.substr(d.start,d.len);
            STRING_Helper::strReplace(p.result_eval,(string)"{@_THIS_RESULT_VALUE_}",this->HBCStrToHBCStr(temp1,LW_BIT_STR,p.bit,true));//转换成Bit attribute 所指类型,并添加0x,0b之类的描述头.
            //ParseXml_Node dn=this->_GetNode(dest_node,p.name);
            d.value=this->_ParseValue(p.result_eval,p.name);
        }else{
            temp1 = origin_str_bit.substr(d.start,d.len);
            if (hex_head.size()!=0 && hex_head=="false")
                d.value=this->HBCStrToHBCStr(temp1,LW_BIT_STR,p.bit,false);
            else
                d.value=this->HBCStrToHBCStr(temp1,LW_BIT_STR,p.bit,true);//转换成Bit attribute 所指类型,并添加0x,0b之类的描述头.
        }
        
        //cout << "Routing xml beging" << endl;
        // Config dest_xml node
        //XML_Set(dest_node,{N:p.name,A:[["Bstart",d.start],["Len",d.len],["Bend",d.end]],V:""+d.value},true);
        this->_GetFieldsFormOriginStr_XML_Set(dest_node,d,p);
        //cout << "Routing xml end.." << endl;
        
    }
    return true;
}

/***************************XML 对象解析法 End ********************************************/

/***************************Binary 对象解析法 Start ********************************************/
// 流对象解析法
bool ParseProcess::Http_ParsePacket::GetFieldsFormOriginStr(const char *parse_path, string &origin_str, Http_ParseObj *dest_node, string &obj_name){
    ParseXml_Node ps;
    this->nowParsePath="";
    
    if ((ps=this->GetParseNode(parse_path,"_OriginToField_"))){
        this->_GetFieldsFormOriginStr(ps,origin_str,dest_node,obj_name);
        return true;
    }else{
        this->ErrorStr=this->S1+" node ["+parse_path+"] can't found!";
        PUB_Error(this->ErrorStr);
        return false;
    }
}

bool ParseProcess::Http_ParsePacket::_GetFieldsFormOriginStr(ParseXml_Node ps, string &origin_str, Http_ParseObj *dest_node, string &obj_name){
    string origin_str_bit,ps_name,temp1,to_obj("");
    ParseXml_Node pn;
    ParseObjT p=this->parseObj;
    DestObjT d=this->destObj;
    ParseXml_Node n;
    string hex_head;
    
    ps_name=this->_NodeAttr(ps,"Name");
    hex_head=this->_NodeAttr(ps,"HexHead");
    
    
    //把原始字段统一转换为二进制字段.
    if (origin_str=="")
        return false;
    origin_str_bit = this->HBCStrToHBCStr(origin_str,"",LW_BIT_STR,false);
    this->OriginStr = this->HBCStrToHBCStr(origin_str,"",LW_HEX_STR,false); //js
    if (origin_str!="" && origin_str_bit==""){
        //PUB_Error(this->S1+" node ["+origin_str+"] Out of original Number String, irregular Number!");
        this->ErrorStr=this->S1+" node ["+origin_str+"] Out of original Number String, irregular Number!";
        PUB_Error(this->ErrorStr);
        return false;
    }
    
    //cout << "GetFieldsFormOriginStr....=> origin_str_bit." << endl;;
    //循环递进累加 node
    d.start=d.len=d.end=0;
    for(ParseXml_Node pn = ps.first_child(); pn; pn = pn.next_sibling()){
        //???判断__类字段不予解析
        const char * node_name = pn.name();
        if (strstr(node_name,"__")==node_name)
            continue;
        //读取解析字段 node 相关信息
        this->_GetParseFieldObj(pn,p);//通过配置 node 取得相关 attribute Param,并适当_ParseValue所以需要from_node.
        
        if (p.test!="" && (p.test=="false" || p.test=="0"))//ly:js no syn
            continue;
        
        if (p.len=="" && p.end=="" && p.start==""){//js
            //如果是直接赋值
            /*n=this->_AddNode(dest_node,p.name,true);
             this->_SetNodeValue(n,p.value);*/
            //dest_node.SetNodeValue(p.name,p.value);//写入操作
            dest_node->SetNodeValue((obj_name+"."+p.name).c_str(),p.value.data(),Http_ParseObj::STR_COPY_OBJ,p.value.size());
            this->DEBUGY((obj_name+"."+p.name)+":"+this->HBCStrToHBCStr(p.value,"hex",LW_HEX_STR,true));
            continue;
        }
        
        //解析换算字段位置Param
        if (!this->GetFieldBitPosition(p,d,this->_StrLen(origin_str_bit))){
            //???
            return false;
        }
        
        //三值合法性校验
        if((d.end-d.start)<-1 || d.end >= this->_StrLen(origin_str_bit) || d.start<0){
            this->ErrorStr=this->S1+" node ["+p.name+"] Out of original range of  String '"+origin_str+"', start:"+this->_iToStr(d.start,10)+" end:"+this->_iToStr(d.end,10)+" len:"+this->_iToStr(d.len,10);
            PUB_Error(this->ErrorStr);
            //PUB_Error(this->S1+" node ["+p.name+"] Out of original range of  String '"+origin_str+"', start:"+this->_iToStr(d.start,10)+" end:"+this->_iToStr(d.end,10)+" len:"+this->_iToStr(d.len,10));
            return false;
        }
        
        //从原始字段中取得字段值:
        
        if (p.result_eval.size()!=0){//ly:js no syn
            temp1 = origin_str_bit.substr(d.start,d.len);
            STRING_Helper::strReplace(p.result_eval,(string)"{@_THIS_RESULT_VALUE_}",this->HBCStrToHBCStr(temp1,LW_BIT_STR,p.bit,true));//转换成Bit attribute 所指类型,并添加0x,0b之类的描述头.
            d.value=this->_ParseValue(p.result_eval,p.name);
        }else{
            temp1 = origin_str_bit.substr(d.start,d.len);
            if (hex_head.size()!=0 && hex_head=="false")
                d.value=this->HBCStrToHBCStr(temp1,LW_BIT_STR,p.bit,false);
            else
                d.value=this->HBCStrToHBCStr(temp1,LW_BIT_STR,p.bit,true);//转换成Bit attribute 所指类型,并添加0x,0b之类的描述头.
        }
        
        //写入操作
        //(* dest_node)(p.name,d.value);
        dest_node->SetNodeValue((obj_name+"."+p.name).c_str(),d.value.data(),Http_ParseObj::STR_COPY_OBJ,d.value.size());
        this->DEBUGY((obj_name+"."+p.name)+":"+this->HBCStrToHBCStr(d.value,"hex",LW_HEX_STR,true));
    }
    return true;
}

//把解析结果值和相关 attribute 写入目标 node .
bool ParseProcess::Http_ParsePacket::_GetFieldsFormOriginStr_XML_Set(ParseXml_Node dest_node,DestObjT &d,ParseObjT &p){
    ParseXml_Node n;
    
    n=this->_AddNode(dest_node,p.name,true);
    this->_SetNodeAttr(n,"Bstart",d.start);
    this->_SetNodeAttr(n,"Len",d.len);
    this->_SetNodeAttr(n,"Bend",d.end);
    if (p.unit.size()!=0)
        this->_SetNodeAttr(n,"Unit",p.unit.c_str());
    if (p.remark.size()!=0)
        this->_SetNodeAttr(n,"Remark",p.remark.c_str());
    this->_SetNodeValue(n,d.value);
    
    this->DEBUGY(n);
    
    return true;
}
//反向解析:从字符流 node 中取得系列字段值 Object .
//parse_node:解析配置xml node  or 者路径；from_node,xml字段 node 的根 node ；其它为内部中转 variable .
string ParseProcess::Http_ParsePacket::GetOriginStrFromFields(const char *parse_path, ParseXml_Node from_node,string this_path,const char *radix){
    ParseXml_Node ps;
    
    this->nowParsePath=this_path;
    ps=this->GetParseNode(parse_path,"_FieldToOrigin_");
    return this->_GetOriginStrFromFields(ps,from_node,radix);
    
}
string ParseProcess::Http_ParsePacket::_GetOriginStrFromFields(ParseXml_Node ps, ParseXml_Node from_node,const char *radix){
    //出入 Object 初始化.
    string origin_str,origin_str_bit,pn,from_str,from_bit1,from_bit,origin_str1,ps_name;
    ParseObjT p=this->parseObj;
    DestObjT d=this->destObj;
    ParseXml_Node node;
    
    //取得解析描述 node
    ps_name=this->_NodeAttr(ps,"Name");
    
    //初始化origin_str;
    origin_str="";
    origin_str_bit = this->HBCStrToHBCStr(origin_str,"",LW_BIT_STR,false);//!!!今后改为空，以下去除
    if (origin_str!=""){
        PUB_Error(this->S1+" node ["+ps_name+"] Out of original  String , no regular Number!");
        origin_str_bit = this->HBCStrToHBCStr(origin_str,"",LW_BIT_STR,false);
    }else
        origin_str_bit = "";
    
    //循环递进累加 node 
    d.start=d.len=d.end=0;
    for(ParseXml_Node pn = ps.first_child(); pn; pn = pn.next_sibling()){
        //读取解析 node  Name 
        this->_GetParseFieldObj(pn,p);//通过配置 node 取得相关 attribute Param,并适当_ParseValue所以需要from_node.
        
        //if (p.test!="" && (p.test=="false" || p.test=="0"))//ly:js no syn
        //	continue;
        //如果是描述 node 则调过
        if (p.len == "" && p.end == "" && p.start == "") {//js
            continue;
        }
        
        //解析换算字段位置Param
        if (!this->GetFieldBitPosition(p,d,this->_StrLen(origin_str_bit))){
            return this->S1+"";
        }
        
        //三值合法性校验
        if((d.end-d.start)<-1 || d.start<0){
            PUB_Error(this->S1+" node ["+ps_name+"] range  start:"+this->_iToStr(d.start,10)+" end:"+this->_iToStr(d.end,10)+" len:"+this->_iToStr(d.len,10)+" out of the border!");
            return this->S1+"";
        }
        
        //取得 Source 值,并平铺成复核目标 Length 的,二进制 String .
        if (!(node=this->_GetNode(from_node,p.name))){
            PUB_Error(this->S1+" node ["+p.name+"]  not exist!");
            return this->S1+"";
        }
        from_str=this->_NodeValue(node);   //取得xml中的字段值
        
        //补充处理;尚未测试!!!
        if (p.src_eval.size()!=0){//ly:js no syn				
            STRING_Helper::strReplace(p.src_eval,(string)"{@_THIS_SOURCE_VALUE_}",from_str);//转换成Bit attribute 所指类型,并添加0x,0b之类的描述头.
            //ParseXml_Node dn=this->_GetNode(from_node,p.name);
            from_str=this->_ParseValue(p.src_eval,p.name);
        }
        
        //把值转换为二进制
        from_bit=this->HBCStrToHBCStr(from_str,p.bit,LW_BIT_STR,false);
        
        if (from_str==""){
            
            from_str="0";
        }/*else if (STRING_Helper::ParseInt(from_str)==NaN){
          PUB_Error(this->S1+" node ["+ps_name+"]内容不是标准的数字!");
          return this->S1+"";
          }*/
        
        from_bit1=this->SetNumToID(from_bit,d.len,"0");  //如果不齐则前面补充0000.
        
        //在指定位置替换相关二进制字段
        if(d.start>this->_StrLen(origin_str)){
            //如果origin_str Length 小于d.start则,补齐0000,累加from_bit;
            origin_str1 = this->SetNumToID(origin_str,d.start-this->_StrLen(origin_str)-1,"0");
            origin_str1 += from_bit1;
        }else{
            origin_str1=this->ReplaceBit(origin_str,d.start,from_bit1);
        }
        origin_str=origin_str1;
    }	  
    this->OriginStr = origin_str;  //js
    if (radix)
        return this->HBCStrToHBCStr(origin_str,LW_BIT_STR,radix,true);
    else
        return origin_str;
}

//取得字段解析 node 的具体Param.
//field_node:解析 node ;data_node数据补充 node ,作为_ParseValue补充.
void ParseProcess::Http_ParsePacket::_GetParseFieldObj(ParseXml_Node field_node, ParseObjT &p_obj){
    ParseXml_Node pn;
    
    //取得解析 node    
    pn=field_node;
    //dn=this->_GetNode(data_node,this->_NodeName(pn));
    
    
    p_obj.name=this->_NodeName(pn);
    p_obj.len =this->_ParseValue(this->_NodeAttr(pn,"Len"),p_obj.name);        
    p_obj.start=this->_ParseValue(this->_NodeAttr(pn,"Bstart"),p_obj.name);
    p_obj.end =this->_ParseValue(this->_NodeAttr(pn,"Bend"),p_obj.name);
    p_obj.bit =this->_NodeAttr(pn,"Bit");
    p_obj.type =this->_NodeAttr(pn,"Type");
    p_obj.from_end =this->_NodeAttr(pn,"FromEnd");
    p_obj.value = this->_ParseValue(this->_NodeValue(pn),p_obj.name);//js,now_pathParam3还有问题
    string error="";
    //string test_str=this->_ParseValue(this->_NodeAttr(pn,"Test"),p_obj.name);
    //ly_change: not sync old version; ly:js no syn
    string test_str=this->_ParseValue(this->_NodeAttr(pn,"Test"),p_obj.name);
    if (test_str.size()==0)
        p_obj.test="";
    else{
        if (this->parentProcess->TestConditionStr(test_str,error))
            p_obj.test="1";
        else{
            p_obj.test="0";
        }
    }
    
    p_obj.result_eval =this->_NodeAttr(pn,"ResultEval");
    p_obj.src_eval=this->_NodeAttr(pn,"SourceEval");
    p_obj.remark = this->_NodeAttr(pn,"Remark");
    p_obj.unit = this->_NodeAttr(pn,"Unit");
    
    p_obj.radix=( p_obj.bit==LW_BINARY_STR ? "8" :(p_obj.bit==LW_HEX_STR ? "4" : (p_obj.bit==LW_BIT_STR ? "1":"")) );
    
    if (!PUB_FindStrIs(p_obj.bit,LW_BINARY_STR) && !PUB_FindStrIs(p_obj.bit,LW_HEX_STR)&& !PUB_FindStrIs(p_obj.bit,LW_BIT_STR)){
        PUB_Error(this->S1+" node ["+p_obj.name+"]Bit attribute 错误( attribute must be:char | hex |bin)!");
        //return PARSE_PACKET_NULL;
    }
    //return p_obj;
}

//==辅助函数--------------
//函数:对单一字段 node 的 Length 解析,取得stat,end,len;
//Param:p_*为解析 node  Config ,d_*为上一解析结果（为递进查询使用；
bool ParseProcess::Http_ParsePacket::GetFieldBitPosition(ParseObjT &p_obj, DestObjT &d_obj,int origin_len){
    int old_start=d_obj.start,old_len=d_obj.len;	    
    
    //初始化 variable     
    if (p_obj.len!=""){
  	    	//if ((d_obj.len=STRING_Helper::ParseInt(p_obj.len))==NaN){ //ly mac
        if (!STRING_Helper::CheckNum(p_obj.len)){
            PUB_Error(this->S1+"node pointed by ["+p_obj.name+"]'s len attribute ["+this->_iToStr(d_obj.len,10)+"] can't be create!");
            return false;
        }
        d_obj.len=STRING_Helper::ParseInt(p_obj.len);
        d_obj.len=d_obj.len*STRING_Helper::ParseInt(p_obj.radix);
        //采用递进查询法.
        if (p_obj.start=="" && p_obj.end==""){
            //自动累加递进法 //从前往后（p_obj.len为正）从后向前（p_obj.len为负）
            if (p_obj.from_end!="true"){
                //从前往后,通过前一次的 node 计算后一次的 node ;
                d_obj.start=old_start+(old_len-1)+1;
                d_obj.end=d_obj.start+(d_obj.len-1);
            }else{//从后向前.
                d_obj.end=old_start-1;
                d_obj.start=d_obj.end-(d_obj.len-1);//通过前一次的 node 计算后一次的 node ;
            }
        }else{
            //绝对路径递进法,只允许 Config start;
            if(p_obj.start!=""){
                //根据起始 node 计算
                if (!STRING_Helper::CheckNum(p_obj.start)){
                    PUB_Error(this->S1+"node pointed by["+p_obj.name+"'s start attribute ["+p_obj.start+"] can't be create!");
                    return false;
                }
                d_obj.start=STRING_Helper::ParseInt(p_obj.start)*STRING_Helper::ParseInt(p_obj.radix);
                //if (d_obj.start==NaN){
                //PUB_Error(this->S1+"node pointed by["+p_obj.name+"'s start attribute ["+p_obj.start+"] can't be create!");
                //return false;
                //} //ly mac
                d_obj.end=d_obj.start+(d_obj.len-1); //已有d_obj.start;
            }else if (p_obj.end!=""){//？这里还有错误,并没有按bstart处理.
                if (!STRING_Helper::CheckNum(p_obj.end)){
                    PUB_Error(this->S1+"node pointed by["+p_obj.name+"]'s end attribute ["+p_obj.end+"] can't be create!");
                    return false;
                }
                d_obj.end=0+(origin_len-1);
                //if (d_obj.end==NaN){
                //PUB_Error(this->S1+"node pointed by["+p_obj.name+"]'s end attribute ["+p_obj.end+"] can't be create!");
                //return false;
                //} //ly mac
                d_obj.start=d_obj.end-(d_obj.len-1)-STRING_Helper::ParseInt(p_obj.end)*STRING_Helper::ParseInt(p_obj.radix);
            }
        }
        
    }else{
        //采用绝对查询法.
        if(p_obj.start=="" || p_obj.end==""){
            PUB_Error(this->S1+" node ["+p_obj.name+"]如果没有定义Len则必须绝对指定start 和 end node '");
            return false;
        }
        //取得起始
        if(!STRING_Helper::CheckNum(p_obj.start)  || !STRING_Helper::CheckNum(p_obj.end)){
            PUB_Error(this->S1+" node ["+p_obj.name+"]所指定start 和 end node 无法确取得数字'");
            return false;
        }
        d_obj.start=STRING_Helper::ParseInt(p_obj.start);
        d_obj.end  =STRING_Helper::ParseInt(p_obj.end);
        //if(d_obj.start==NaN || d_obj.end==NaN){
        //PUB_Error(this->S1+" node ["+p_obj.name+"]所指定start 和 end node 无法确取得数字'");
        //return false;
        //} //ly mac
        
        //计算三值.
        d_obj.len=d_obj.end-d_obj.start+1;//已有d_start、d_end
    }
    
    return true;
}


//函数: String 进制转换.把temp从src_hbc转成dest_hbc进制数;并判断是否增加add_head（0x,0b）
//函数: String 进制转换.把temp从src_hbc转成dest_hbc进制数;并判断是否增加add_head（0x,0b）
string ParseProcess::Http_ParsePacket::HBCStrToHBCStr(const char *temp, const char *src_hbc, const char *dest_hbc, bool add_head){
    string temp_s=temp,src_hbc_s=src_hbc,dest_hbc_s=dest_hbc;
    return this->HBCStrToHBCStr(temp_s,src_hbc_s,dest_hbc_s,add_head);
}

string ParseProcess::Http_ParsePacket::HBCStrToHBCStr(string &temp, const char *src_hbc, const char *dest_hbc, bool add_head){
    string src_hbc_s=src_hbc,dest_hbc_s=dest_hbc;
    return this->HBCStrToHBCStr(temp,src_hbc_s,dest_hbc_s,add_head);
}

string ParseProcess::Http_ParsePacket::HBCStrToHBCStr(string &temp, string &src_hbc, const char *dest_hbc, bool add_head){
    string dest_hbc_s=dest_hbc;
    
    return this->HBCStrToHBCStr(temp,src_hbc,dest_hbc_s,add_head);
}

string ParseProcess::Http_ParsePacket::HBCStrToHBCStr(string &temp, const char* src_hbc, string &dest_hbc, bool add_head){
    string src_hbc_s=src_hbc;
    
    return this->HBCStrToHBCStr(temp,src_hbc_s,dest_hbc,add_head);
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
string ParseProcess::Http_ParsePacket::BinaryToHBCStr(const char* data, size_t data_len, const char *dest_hbc, bool add_head){
    string temp(dest_hbc);
    return this->BinaryToHBCStr(data,data_len,temp,add_head);
}

string ParseProcess::Http_ParsePacket::BinaryToHBCStr(const char* data, size_t data_len, string &dest_hbc, bool add_head){
    string ns="",s,d_bit,ds,ds1,add_head_str;
    int dest_num;
    size_t i;
    unsigned short int num;
    
    s="";   
    //data全部生成二进制 String . 
    for (i=0;i<data_len; i++){
        //取得 Source 字符数据.
        num = data[i];//获取ascii
        if (num >= 0xff00)
            num =num-0xff00; //!!!这个部分不是很确定,为什么会有ff之类,也学只能适用于32位c++
        d_bit=this->SetNumToID(this->_iToStr(num,2),8,"0"); //转换为二进制,并填补位数.
        s+=d_bit;
    }
    
    //生成目标 String 
    ds="";
    for(i=0; i<(size_t)this->_StrLen(s);){
        ds1=ds;
        if (dest_hbc==LW_HEX_STR){
            dest_num = STRING_Helper::ParseInt(s.substr(i,4),2);
            ds+=this->_iToStr(dest_num,16);
            i+=4;
        }else if (dest_hbc==LW_BINARY_STR){
            dest_num=STRING_Helper::ParseInt(s.substr(i,8),2);
            //if (dest_num!=0) //only_c++
            ds+=this->_iToChar(dest_num);
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
string ParseProcess::Http_ParsePacket::HBCStrToCRCStr(string &src_hbc, int start_hex, int end_hex){
    string temp,buf;
    unsigned char crc= 0;
    size_t i;
    //unsigned char buf[3];
    
    //把16进制转为字符流
    temp = this->HBCStrToHBCStr(src_hbc,LW_HEX_STR,LW_BINARY_STR,false);
    
    //循环累加crc
    unsigned char *dd=(unsigned char *)temp.data();
    for (i=start_hex; i<temp.size()-end_hex; i++){
        crc = Http_ParseProcess_CRC_TABLE[crc^dd[i]];
    }
    //生成crc双字节
    buf+=0x30+(crc>>4);
    buf+=0x30+(crc&0x0f);
    /*buf+=0x30+(crc&0x0f);
     buf+=0x30+(crc>>4);*/
    
    //把crc转为hex
    return this->HBCStrToHBCStr(buf,LW_BINARY_STR,LW_HEX_STR,false);
}

string ParseProcess::Http_ParsePacket::BinaryStrToCRCStr(string &src_hbc, int start_hex, int end_hex){
    string temp,buf;
    unsigned char crc= 0;
    size_t i;
    
    //循环累加crc
    unsigned char *dd=(unsigned char *)src_hbc.data();
    for (i=start_hex; i<src_hbc.size()-end_hex; i++){
        crc = Http_ParseProcess_CRC_TABLE[crc^dd[i]];
    }
    //生成crc双字节
    buf+=0x30+(crc>>4);
    buf+=0x30+(crc&0x0f);
    
    //把crc转为hex
    return buf;
}


//各种进制 String 之间的转换-》已经具有了字符流 String 的转换能力
string ParseProcess::Http_ParsePacket::HBCStrToHBCStr(string &temp, string &src_hbc, string &dest_hbc, bool add_head){
    if (src_hbc=="char"){
        return this->BinaryToHBCStr(temp.c_str(),temp.size(),dest_hbc,add_head);
    };
    
    string ns="",s,d_bit,ds,ds1,add_head_str,temp1;
    int i,k;
    unsigned char dest_num,num;
    //unsigned int 
    
    //判断缺省数字类型；
    if (this->_IsNull(src_hbc)){
        if (PUB_FindStr(temp,"0x")==0 && this->CheckNum(temp,LW_HEX_STR))
            src_hbc=LW_HEX_STR;
        else if (PUB_FindStr(temp,"0b")==0 && this->CheckNum(temp,LW_BIT_STR))
            src_hbc=LW_BIT_STR;
        else 
            src_hbc=LW_BINARY_STR;
    }
    if (src_hbc==LW_HEX_STR && PUB_FindStr(temp,"0x")==0)
        temp1=temp.substr(2);
    else if (src_hbc==LW_BIT_STR && PUB_FindStr(temp,"0b")==0)
        temp1=temp.substr(2);
    else
        temp1=temp;
    
    if (src_hbc==dest_hbc){//如果同类型转换则直接赋值
        ds=temp1;
    }else{
        s="";   
        //temp全部生成二进制 String . 
        k=this->_StrLen(temp1);
        for (i=0;i<k;){
            //取得 Source 字符数据.
            if (src_hbc==LW_BINARY_STR){//八位
                num = this->_CharAt(temp1,i);//获取ascii
                d_bit=this->SetNumToID(this->_iToStr(num,2),8,"0"); //转换为二进制,并填补位数.
                i+=1;
            }else if (src_hbc==LW_HEX_STR){//如果Hex16进制.
                num = STRING_Helper::ParseInt(temp1.substr(i,1),16);
                d_bit=this->SetNumToID(this->_iToStr(num,2),4,"0"); //转换为二进制,并填补位数.
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
        for(i=0; i<this->_StrLen(s);){
            ds1=ds;
            if (dest_hbc==LW_HEX_STR){
                dest_num = STRING_Helper::ParseInt(s.substr(i,4),2);
                ds+=this->_iToStr(dest_num,16);
                i+=4;
            }else if (dest_hbc==LW_BINARY_STR){
                dest_num=STRING_Helper::ParseInt(s.substr(i,8),2);
                //if (dest_num!=0) //only_c++
                ds+=dest_num; //this->_iToChar(dest_num);
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

//函数:补齐缺失 Length 的字段为split所指定值
string ParseProcess::Http_ParsePacket::SetNumToID(string s,int len, const char*split){
    string str1;
    int i,j;
    
    if (len<=0)
        return this->S1+"";
    if (!split)
        split="0";
    
    if (!this->_TypeOf(s, "string"))
        str1=this->_StrAdd("",s,true);
    else
        str1 = s;
    
    j = len-this->_StrLen(str1);
    for(i=0;i<j;i++)
        str1=this->_StrAdd(str1,split,false);
    return str1;
}

//函数:判断数字 String 合法性,Radix:进制,len:要求 Length ；
bool ParseProcess::Http_ParsePacket::CheckNum(string &num_str){
    
    return this->CheckNum(num_str, "", (int)num_str.size());
}

bool ParseProcess::Http_ParsePacket::CheckNum(string &num_str, const char *Radix){
    
    return this->CheckNum(num_str, Radix, (int)num_str.size());
}
//js,c++
bool ParseProcess::Http_ParsePacket::CheckNum(string &num_str, const char *Radix, int len){
    int i,j,test_len;
    string strTemp,type; 	
    
    if ( this->_StrLen(num_str)== 0 && (this->_IsNull(len) || len==0)) 
        return true;
    
    if((Radix && PUB_FindStr(Radix,LW_BIT_STR)>=0) ||  (PUB_FindStr(num_str,"0b")==0 || PUB_FindStr(num_str,"0B")==0)){
        strTemp="01bB"; 
        type=LW_BIT_STR;
    }else if ((Radix && PUB_FindStr(Radix,LW_HEX_STR)>=0) ||(PUB_FindStr(num_str,"0x")==0 || PUB_FindStr(num_str,"0X")==0)){
        strTemp="xX0123456789abcdefABCDEF";
        type=LW_HEX_STR;
    }else if (Radix && PUB_FindStr(Radix,"num")>=0){
        strTemp=".0123456789"; 
        type="num";
    }else{
        strTemp=".0123456789xbXB";
    }
    
    //测试 String 正确性
    for (i=0;i<this->_StrLen(num_str);i++) 
    { 
        j=PUB_FindStr(strTemp,this->_SubStr(num_str,i,1));
        if (j==-1) 
        { 
            //说明有字符不是数字 
            return false; 
        } 
    } 
    
    // Length 正确性
    if (type==LW_BIT_STR || type==LW_HEX_STR) 
        test_len=this->_StrLen(num_str)-2;
    else
        test_len=this->_StrLen(num_str);
    
    if (!this->_IsNull(len) && test_len>len)
        return false;
    
    //说明是数字 
    return true; 
}

//函数:交叉替换 String 的处理.
string ParseProcess::Http_ParsePacket::ReplaceBit(string &origin_str,int d_start, string &from_bit){
    string s1,s2,s3;
    
    s1=origin_str.substr(0,d_start);
    s2=from_bit;
    if (this->_StrLen(origin_str) >(d_start+this->_StrLen(from_bit))){
        //如果未超出.
        s3=origin_str.substr(d_start+this->_StrLen(from_bit));
    }else{
        s3="";
    }
    return s1+s2+s3;
}

//===============各类js同步过滤函数.
bool ParseProcess::Http_ParsePacket::_TypeOf(ParseXml_Node &obj,const char *type){
    return true;
}
bool ParseProcess::Http_ParsePacket::_TypeOf(string &obj,const char *type){
    return true;
}
bool ParseProcess::Http_ParsePacket::_TypeOf(const char *obj,const char *type){
    return true;
}
bool ParseProcess::Http_ParsePacket::_IsNull(ParseXml_Node &obj,const char *type){
    //if(obj==false) ly mac ???
    if (obj==NULL)
        return true;
    else
        return false;
}

bool ParseProcess::Http_ParsePacket::_IsNull(const char *obj){
    if (strlen(obj)==0)
        return true;
    else
        return false;
}

bool ParseProcess::Http_ParsePacket::_IsNull(string &obj){
    if (obj=="")
        return true;
    else
        return false;
}

bool ParseProcess::Http_ParsePacket::_IsNull(int len){
    return true;
}

bool _IsNull(const char *obj){
    if (obj==NULL ||strlen(obj)==0)
        return true;
    else
        return false;
}
ParseXml_Node ParseProcess::Http_ParsePacket::_GetNode(ParseXml_Node n, string jpath){
    return this->_GetNode(n,jpath.c_str());
}

ParseXml_Node ParseProcess::Http_ParsePacket::_GetNode(ParseXml_Node n, const char *jpath){
    pugi::xml_node tool;
    xpath_node xml_node;
    try	{
        xml_node = n.select_single_node(jpath);
        
    }catch (const pugi::xpath_exception& e){
        this->ErrorStr=e.what();
        return tool;
    }				
    if (xml_node){
        return xml_node.node();
    }else{
        return tool;
    }
}

ParseXml_Node ParseProcess::Http_ParsePacket::_AddNode(ParseXml_Node node, string node_name,bool is_clear){
    if (is_clear)
        node.remove_child(node_name.c_str());
    pugi::xml_node descr = node.append_child(node_name.c_str());
    return descr;
}

bool ParseProcess::Http_ParsePacket::_SetNodeValue(ParseXml_Node n, string &value){
    //xml_node n1=n.first_child();
    //if (n1)
    //return n1.first_child().set_value(value.c_str());//如果已经存在该 node 
    //else
    return n.text().set(value.c_str());//如果没有存在该 node .
}

bool ParseProcess::Http_ParsePacket::_SetNodeAttr(ParseXml_Node node, const char *att_name, int value){
    //string value_str;
    //value_str=this->_iToStr(value,10);
    
    pugi::xml_attribute attr;
    if (!(attr=node.attribute(att_name)))
        attr = node.append_attribute(att_name);
    
    return attr.set_value(value);
}

bool ParseProcess::Http_ParsePacket::_SetNodeAttr(ParseXml_Node node, const char *att_name, const char *value){
    //string value_str;
    //value_str=this->_iToStr(value,10);
    
    pugi::xml_attribute attr;
    if (!(attr=node.attribute(att_name)))
        attr = node.append_attribute(att_name);
    
    return attr.set_value(value);
}

const char*ParseProcess::Http_ParsePacket::_NodeName(ParseXml_Node this_node){
    return this_node.name();
}

const char*ParseProcess::Http_ParsePacket::_NodeValue(ParseXml_Node this_node){
    //return this_node.first_child().value();
    return this_node.text().get();
}

const char*ParseProcess::Http_ParsePacket::_NodeAttr(ParseXml_Node this_node,const char*attr_name){
    return this_node.attribute(attr_name).value();
}

int ParseProcess::Http_ParsePacket::_StrLen(string &s){
    return (int)s.size();
}

/*
 int ParseProcess::Http_ParsePacket::_ParseInt(const char *s){
	string s1=s;
	return STRING_Helper::ParseInt(s1);
 }
 
 
 // String 转换位数字
 int ParseProcess::Http_ParsePacket::_ParseInt(string s){
	//判断是否为争取的 String 
	if (!this->CheckNum(s))
 return NaN;
 
	if (s.compare(0,2,"0x")==0 || s.compare(0,2,"0X")==0){//0x*** String ；
 return STRING_Helper::ParseInt(s.substr(2),16);
	}else if (s.compare(0,2,"0b")==0 || s.compare(0,2,"0B")==0){//0b*** String ；
 return STRING_Helper::ParseInt(s.substr(2),2);//ly change 2014;
	}else
 return STRING_Helper::ParseInt(s,10);//普通 String ；
 }
 
 int ParseProcess::Http_ParsePacket::_ParseInt(string s, int radix){
 
	if (radix==10){//十进制 String 转换成数值；
 return atoi(s.c_str());
	}else if (radix==2){//而进制 String 转换成数值；
 int temp=1,x=0;
 for(int i=s.size()-1;i>=0;i--,temp+=temp){
 if(s[i]=='1')
 x+=temp;
 }
 return x;
 
	}else if (radix==16){//16进制转换成数值；
 unsigned char num=0;
 int result=0;
 int nsqr=1;
 for(int i=s.size()-1;i>=0;i--){
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
 return NaN;
 }*/

//函数:数字转 String ,无0b,0x等标志.
string ParseProcess::Http_ParsePacket::_iToStr(int num,int radix){
    char buf[64];
    string s;
    itoa(num,buf,radix);
    return s=buf;
}

//函数:数字转 String ,无0b,0x等标志.
int ParseProcess::Http_ParsePacket::_StrToi(string str){
    str+='\0';
    return atoi(str.c_str());
}


//函数:数字转 String ,无0b,0x等标志.
double ParseProcess::Http_ParsePacket::_StrTof(string str){
    str+='\0';
    if (str.compare(0, 2, "0x") == 0)
        return STRING_Helper::ParseInt(str);//string temp_s=this->HBCStrToHBCStr(str,"hex","",false)//应该有一个16进制转换
    else
        return atof(str.c_str());
}
/*template <typename T> string ParseProcess::Http_ParsePacket::ToStr(T val)
 {
	ostringstream oss;
 
	oss<<val;
 
	return oss.str();
 }*/

string ParseProcess::Http_ParsePacket::_dToStr(double val)
{
    ostringstream oss;
    
    oss<<val;
    
    return oss.str();
}

string ParseProcess::Http_ParsePacket::_iToChar(int num){
    char a;
    a=(char )num;
    if (num!=0)
        return this->S1+a;
    else{
        string s(1,0);
        return s;
    }
}

string ParseProcess::Http_ParsePacket::_SubStr(string &s,int pos,int len){
    return s.substr(pos,len);
}

char ParseProcess::Http_ParsePacket::_CharAt(string &s,int pos){
    //const char* c=s.c_str();
    //return *(c+pos);
    return s.at(pos);
}

string ParseProcess::Http_ParsePacket::_StrAdd(string &s,string &add_str){
    
    string s1=this->_StrAdd(s,add_str,true);
    return s1;
}
string ParseProcess::Http_ParsePacket::_StrAdd(const char*s,string &add_str,bool from){
    string s1=s;
    return this->_StrAdd(s1,add_str,from);
}
string ParseProcess::Http_ParsePacket::_StrAdd(string &s,const char *add_str,bool from){
    string add_str1=add_str;
    return this->_StrAdd(s,add_str1,from);
}

string ParseProcess::Http_ParsePacket::_StrAdd(string &s,string &add_str,bool from){
    string s1;
    if (from)
        s+=add_str;
    else
        s.insert(0,add_str);
    s1=s;
    return s1;
}

string ParseProcess::Http_ParsePacket::_ParseValue(const char*str,string now_path){
    string temp=this->S1+str;
    return this->_ParseValue(temp,now_path);
}

string ParseProcess::Http_ParsePacket::_ParseValue(string &str,string now_path){
    string dest;
    ParseProcess::PV_AddParas_T add_paras;
    //传入替换Param.
    add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_PARENT_NODE_}"),string(this->nowParsePath)));
    add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_THIS_NODE_VALUE_}"),string(this->nowParsePath+"/"+now_path)));
    add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("{@_ORIGIN_STR_}"),this->OriginStr)); //js
    
    this->parentProcess->ParseValue(str,dest,add_paras);
    
    add_paras.erase("{@_PARENT_NODE_}");
    add_paras.erase("{@_THIS_NODE_VALUE_}");
    add_paras.erase("{@_ORIGIN_STR_}");
    
    return dest;
}
int ParseProcess::Http_ParsePacket::PUB_FindStr(const char *str,const char*s){
    string s1=str,s2=s;
    return this->PUB_FindStr(s1,s2);
    //return (strstr(str,s)-str);
}

int ParseProcess::Http_ParsePacket::PUB_FindStr(string &str,const char*s){
    string s1=s;
    return this->PUB_FindStr(str,s1);
}

int ParseProcess::Http_ParsePacket::PUB_FindStr(string &str,string s){
    size_t i=str.find(s);
    if (i==string::npos)
        return -1;
    else
        return (int)i;
    
}
bool ParseProcess::Http_ParsePacket::PUB_FindStrIs(string &str,const char*s){
    string s1=s;
    return this->PUB_FindStrIs(str,s1);
}

bool ParseProcess::Http_ParsePacket::PUB_FindStrIs(string &str,string s){
    return str.find(s)!=string::npos;
}

void ParseProcess::Http_ParsePacket::PUB_Error(string str){
    this->parentProcess->SHOW_ERROR(str);
    this->ErrorStr=str;
}

void ParseProcess::Http_ParsePacket::SetDebug(){
    this->doDebug=true;
}
void ParseProcess::Http_ParsePacket::CloseDebug(){
    this->doDebug=false;
}
void ParseProcess::Http_ParsePacket::DEBUGY(string str){
    if(!this->doDebug)
        return;
    this->DEBUGY(str.c_str());
}
void ParseProcess::Http_ParsePacket::DEBUGY(ParseXml_Node n){
    if(!this->doDebug)
        return;
    
    string s,s1;
    s=this->parentProcess->_ActParseXml->XmlToStr(n);
    if (s.size()>1)
        s1=s.substr(0,s.size()-1);//去除回车
    else
        s1=s;
    
    this->DEBUGY(s1.c_str());
}

void ParseProcess::Http_ParsePacket::DEBUGY(const char *str){
    if(!this->doDebug)
        return;
    this->parentProcess->ParseDeep++;
    this->parentProcess->DEBUGY(str);
    this->parentProcess->ParseDeep--;
}
