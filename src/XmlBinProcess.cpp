#include "stdafx.h"
#include "ParseProcess.h"
#include "ParseBase.h"
#include "HTTP_Config.h"
#include <string>
#include <fstream>
#include <sstream>
#include "pugixml.hpp"
#include <math.h>
#include "XmlBinProcess.h"
#ifdef SENSOR_DBM
	#include "CDBManager.h"
#endif
#include "ParseXml.h"
#include "ParseObj.h"
#include "STRING_Helper.h"
#include "Linux_Win.h"

using namespace std;
using namespace pugi;

XmlBinProcess::XmlBinProcess(void){
	//Init @Function
	this->_PubFunc = new Http_ParseFunc();
	//Init Param
	this->_PubObj = new Http_ParseObj();	//创建对象
	//Init ConfigXMl;
	this->_ActParseXml = new Http_ParseXML();
	//初始化解析对象,defined in LoadObjFile()
	//this->_PubPacket = new Http_ParsePacket(this->_ActParseXml,this);

	this->ObjectCfg="";
}

XmlBinProcess::~XmlBinProcess(void){
	//释放对象
	if (this->_PubObj!=0)
		delete this->_PubObj;			//释放对象
	if (this->_ActParseXml!=0)
		delete this->_ActParseXml;			//释放对象
	if (this->_PubFunc!=0)
		delete this->_PubFunc;
	if (this->_PubPacket!=0)
		delete this->_PubPacket;
}

bool XmlBinProcess::LoadCfgFile(const char *parse_cfg){

	int r=this->_ActParseXml->LoadFile(parse_cfg);
	this->ObjectCfg = parse_cfg;

	if (r!=Http_ParseXML::LOAD_XML_SUCCESS){
		this->ErrorStr = this->_ActParseXml->ErrorStr;
		return false;
	}else{
		//初始化解析对象
		this->_PubPacket = new Http_ParsePacket(this->_ActParseXml,this);
		return true;
	}
}


int XmlBinProcess::SetParam_ParsePacket(const char *to_obj,string &input_str,const char *parse_path){

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


bool XmlBinProcess::ParseBinToXmlStr(const char * input_bin, int len,const char *parse_path, string &out_xml_str,const char *xml_root_name){
	string input_str(input_bin,len);
	return this->ParseBinToXmlStr(input_str,parse_path,out_xml_str,xml_root_name);
}

bool XmlBinProcess::ParseBinToXmlsStr(string &input_bin,const char *parse_path, string &out_xml_str,const char *xml_root_name){

	//把bin 转换为hex;!!!去掉此处尝试
	string input_hex;
	this->BinToHex(input_bin,input_hex);

	//批量取得解析节点
	out_xml_str=(string)"<"+xml_root_name+">";
	ParseXml_NodeSet nodes = this->_ActParseXml->GetNodeSetByXpath(parse_path);
	for (ParseXml_NodeSet::const_iterator it=nodes.begin();it!=nodes.end();it++){
		//取得第一层解析节点
		ParseXml_Node parse_node = it->node();
		//获得第一层目标输出
			//创建目标xml根 node ；
		string dest_xml_name = this->_PubPacket->_NodeAttr(parse_node,"Name");
		string xml_str=(string)"<"+dest_xml_name+"></"+dest_xml_name+">";//临时创建一个今后还可以变化.
			//创建内存中的xml对象
		string to_obj="_PARSE_XML_";
		Http_ParseXML * temp_xml=this->_PubObj->XmlObjCreate(to_obj.c_str(),xml_str.c_str());//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
		if (temp_xml==NULL){//!!!是否曾删除对象
			this->ErrorStr = this->_PubObj->ErrorStr;
		}
		//解析操作
		ParseXml_Node xml_node =temp_xml->FirstChild(temp_xml->ObjXml);
		if (!this->_PubPacket->GetFieldsFormOriginStr(parse_node, input_hex, xml_node, to_obj+":/"+dest_xml_name)){
			this->ErrorStr = this->_PubPacket->ErrorStr;
			return false;
		}
		
	//生成字符串并删除xml文档
	out_xml_str.append(temp_xml->XmlToStr(xml_node));
	}
	//添加根节点
	out_xml_str.append((string)"</"+xml_root_name+">");//临时创建一个今后还可以变化.

	return true;
}

bool XmlBinProcess::ParseBinToXmlStr(string &input_bin,const char *parse_path, string &out_xml_str,const char *xml_root_name){

	//创建目标xml根 node ；
	string xml_str=(string)"<"+xml_root_name+"></"+xml_root_name+">";//临时创建一个今后还可以变化.

	//创建内存中的xml对象
	string to_obj="_PARSE_XML_";
	Http_ParseXML * temp_xml=this->_PubObj->XmlObjCreate(to_obj.c_str(),xml_str.c_str());//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
	if (temp_xml==NULL){//!!!是否曾删除对象
		this->ErrorStr = this->_PubObj->ErrorStr;
	}

	//把bin 转换为hex;!!!去掉此处尝试
	string input_hex;
	this->BinToHex(input_bin,input_hex);

	//指向根路径，并解析
	ParseXml_Node xml_node =temp_xml->FirstChild(temp_xml->ObjXml);
	if (!this->_PubPacket->GetFieldsFormOriginStr(parse_path, input_hex, xml_node, to_obj+":/"+xml_root_name)){
		this->ErrorStr = this->_PubPacket->ErrorStr;
		return false;
	}

	//生成字符串并删除xml文档
	out_xml_str = temp_xml->XmlToStr(xml_node);
	return true;
}

bool XmlBinProcess::ParseXmlStrToBin(string &input_xml_str,const char *parse_path, string &out_bin,const char *xml_root_name){

	//创建xml文档
	string to_obj="_PARSE_XML_";
	Http_ParseXML * temp_xml=this->_PubObj->XmlObjCreate(to_obj.c_str(),input_xml_str.c_str());//把 String 解析后写入pub_obj,如果返回值为NULL则不需创建->
	if (temp_xml==NULL){//!!!是否曾删除对象
		this->ErrorStr = this->_PubObj->ErrorStr;
	}

	//指向根路径，并解析
	ParseXml_Node xml_node =temp_xml->FirstChild(temp_xml->ObjXml);
	out_bin =this->_PubPacket->GetOriginStrFromFields(parse_path, xml_node,to_obj+":/"+xml_root_name,"char");
	//???差错提示

	return true;
}

bool XmlBinProcess::ParseBinToXmlNode(ParseXml_Node to_xml_node, string &input_str,const char *parse_path){

	//在根 node 写入相关的字段
	string to_obj_xml_path="_PARSE_XML_:/Root";//只是一个描述，在ParseValue中使用而已。

	return this->_PubPacket->GetFieldsFormOriginStr(parse_path, input_str, to_xml_node,to_obj_xml_path);
}

bool XmlBinProcess::HexToBin(string &in, string &out){
	out = this->_PubPacket->HBCStrToHBCStr(in,"hex","char",false);
	return true;
}

bool XmlBinProcess::BinToHex(string &in, string &out){
	out = this->_PubPacket->BinaryToHBCStr(in.data(),in.size(),"hex",true);
	return true;
}

bool XmlBinProcess::BinToHex(const char *in, int len, string &out){
	out = this->_PubPacket->BinaryToHBCStr(in,len,"hex",true);
	return true;
}

void XmlBinProcess::InitParam(void){
	if (this->_PubObj!=0)
		delete this->_PubObj;			//释放对象
	this->_PubObj = new Http_ParseObj();

	this->_PubObj->SetNodeValue("_THIS_CFG_XML_",(const char *)this->_ActParseXml->XmlToStr(this->_ActParseXml->ObjXml).c_str(),Http_ParseObj::XML_POINT_OBJ);//ly !!!今后改进，目前XML_POINT_OBJ 并未发成作用!!! 测试_THIS_XML_CFG_XML对象
}
