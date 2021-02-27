#ifndef HTTP_PARSE_OBJ_H
#define HTTP_PARSE_OBJ_H
#pragma once
#include <vector>
#include <string>
#include <map>
#include "pugixml.hpp"
#include "ParseXml.h"
#include "ByteBase.h"
using namespace pugi;
using namespace std;

//本对象用于存放解析操作中的公共对象使用。其中包括Req.****等等可更改对象，_***_***_等等不可更改的指针对象。
typedef pair<string, void *> ParseObjValue_T; //(用于存放para字符串和指针信息。)
typedef map<string, ParseObjValue_T> ParseObj_T ;//PubObj的对象Map容器类型
typedef map<string, ParseObjValue_T>::value_type ParseObj_ValueT;

class Http_ParseObj{	
public:
	enum {
		XML_NODE = 6,
		SET_XML_NODE = 5,
		XML_POINT_OBJ = 4,
		XML_PARSE_OBJ = 3,
		STR_COPY_OBJ = 2,
		STR_POINT_OBJ = 1,
		NULL_OBJ = 999,
		SUCCESS = 0,
		XML_SET_ERROR = -3,
		XML_CREATE_ERROR = -1,
		ERROR_POINT_NULL = -2,
		POINT_SON = -3,
		SET_VALUE_ERROR = -4,
		ERROR_OUT_SON = -5,
		JPATH_OBJ=0,
		XML_CLONE_ADD_CHILD=1,
		JPATH_XML=1
	};

	//构造函数：初始化point指针为第一条记录
	Http_ParseObj();
	~Http_ParseObj();
	int CheckJpath(const char *jpath,string &obj_path, string &xml_path);
	int CheckJpath(string &jpath,string &obj_path, string &xml_path);

	//功能：设置jpath所指的对象的值。
	//参数：jpath:对象路径； value:字符串指针；set_type:POINT_MODE（只是指针）,COPY_MODE（拷贝内容）
	//返回：
	//成功-SUCCESS，失败-ERROR_READ_ONLY;
	int SetNodeValue(const char *jpath, const char *value,int set_type,int len=-1);
	int SetNodeValue(const char *jpath, int value);
	int SetObjNode(const char *jpath, void *value,int set_type,int len=-1);
	int DeleteObjNode(const char *jpath);
	int DeleteObjNode(string jpath);
	//功能：设置point所指的对象的值。
	//成功-SUCCESS,失败1-ERROR_POINT_NULL;
	//int SetValue(const char *value, int set_type);
	Http_ParseXML *XmlObjCreate(const char *jpath, const char *xml_str);
	bool XmlNode(const char *jpath,ParseXml_Node &node);
	string& XmlNodeText(const char *jpath,string &xml_text);
	//string& XmlNodeValue(const char *jpath,string &xml_value);
	string& XmlAttrText(const char *jpath,string attr,string &xml_text);
	bool XmlChildNodes(const char *jpath,ParseXml_NodeSet &nodes);
	string& XmlChildNodesText(const char *jpath,string &xml_text,const char *split);
	string& XmlChildNodesText(const char *jpath,string &xml_text,const char *split,vector <string> &fields);
	bool XmlNodeLoadText(const char *jpath, const char *xml_str);
	ParseXml_Node XmlNodeClone(const char *jpath, ParseXml_Node src_node,int clone_type);

	//BArray类函数
	bool BArrayGetSearchStr(string tmp_str, string &search_str);//"[*][*]"

	int FuncValueLen(const char *jpath);

	//功能：设置jpath所指的对象的值。
	//参数：set_type:POINT_MODE,COPY_MODE
	//返回： 
	void _Insert(const char*jpath, void *value, int set_type,int len=-1);

	//功能：取得jpath所指的对象的值。
	//返回：
	//成功-返回字符串指针；失败返回NULL;
	//注：要把所指字符串一次性拿走，否则下次更改就不知道定位在哪里了。
	const char *GetNodeValue(const char *jpath);
	bool GetNodeValueStr(const char *jpath,string &dest_str);
	int GetNextNodeValueStr(const char *jpath, string &dest_str);
	int GetAllSonNodeValue(const char *jpath, vector<string> &dest_vector);
    int GetAllSonNodeNum(const char *jpath);
	int AddChildNodesValue(const char *jpath,string &result_str,const char *split_str);
	int AddChildNodesValue(const char *jpath,string &result_str);
	int  GetObjNodeType(const char *jpath);
	void  *GetObjNode(const char *jpath, int &set_type);
	void  *GetObjNode(const char *jpath);

	//功能：取得point所指的对象的值。
	//返回：
	//		成功-返回字符串指针；失败返回NULL; 
	//  注：要把所指字符串一次性拿走，否则下次更改就不知道定位在哪里了。
	//const char *GetValue();//取得point所指对象，保留字对象{_****_}的处理取值。

	//功能：设置jpath所指的对象的值。
	//参数：set_type:POINT_MODE,COPY_MODE
	//返回： 
	ParseObjValue_T * _ParseObjValue(const char *jpath);

	//功能：point定位到指定对象“****.****”,或者第一个“****.****.****”的子节点;
	//		-如果没有找到则指针定位在::end
	//返回：
	//		找到指定对象-SUCCESS; 找到指定对象子节点：POINT_SON；不存在指定对象和子节点-POINT_NULL;
	int Find(const char *jpath);  
	//int find_end(const char *jpath); //查找最后一个符合条件的对象指针。
	
	//功能：-point指针下移，到指定对象“****.****”,或者“****.****.****”的子节点;
	//		-如果没有找到则指针定位在::end
	//返回：
	//      指定对象-SUCCESS; 找到指定对象子节点：POINT_SON；不存在指定对象和子节点-POINT_NULL;	
	int  next(const char *jpath=0);  

	//功能：-删除节点及子节点;
	//		-如果没有找到则指针定位在::end
	//返回：
	//      指定对象-SUCCESS; 找到指定对象子节点：POINT_SON；不存在指定对象和子节点-POINT_NULL;	
	int Delete(const char *jpath,bool is_del_son=false,bool is_del_this=true);

	//功能：point指针下移，如果没有找到则指针定位在::end
	//返回：
	//      指定对象-SUCCESS; 不存在指定对象和子节点-POINT_NULL;
	//int  last(const char *jpath=0);//poing指针下移，已经查到对象的末尾则返回false;否则返回true

	string ErrorStr;
	ParseObj_T::iterator Point;  //对于it调用的指针，主要用于find定位和next()查找。
	ParseObj_T *ObjMap;//公共对象，允许用户在解析过程中添加，并可以任意调整。
    BB_OBJ_STRU *TableObjStru;
    BB_SIZE_T Rec;
private:
	vector<Http_ParseXML *>* _XmlBuf; //用于存放SetXmlObj中创建的xml对象，方便delete;

};


#endif
