#ifndef PARSE_XML
#define PARSE_XML

#include <string>
#include <fstream>
#include <sstream>
#include "pugixml.hpp"
//#include "XML_Mini.h"

using namespace std;
using namespace pugi;

typedef pugi::xml_document ParseXml_Doc;//PubXML的容器类型。
typedef pugi::xml_node ParseXml_Node;
typedef pugi::xpath_node_set ParseXml_NodeSet;
struct ParseXml_xml_string_writer: pugi::xml_writer
{
    std::string result;

    virtual void write(const void* data, size_t size)
    {
        result += std::string(static_cast<const char*>(data), size);
    }
};

//typedef pugi::xpath_node_set ParseXml_Nodes;

class Http_ParseXML{
public:
	enum{
		LOAD_XML_SUCCESS=0,
		LOAD_XML_ERROR=1,
		SAVE_XML_SUCCESS=0,
		SAVE_XML_ERROR=-1,
		XML_ERROR=-1,
		XML_SUCCESS=2
	};

	//构造函数：初始化point指针为第一条记录
	Http_ParseXML();
	Http_ParseXML(const char *);
	~Http_ParseXML();
	static string XmlToStr(ParseXml_Node node);
	static string XmlToStr(ParseXml_NodeSet nodes,const char* split);
	static string XmlValueToStr(ParseXml_NodeSet nodes,const char *split);
	int LoadFile(const char *file_name,unsigned int parse_way=parse_default);
	int Load(const char *xml_str);
	int SaveFile(void);
	int SaveFile(string encode_str);
	ParseXml_Node Child(ParseXml_Node parent_node,const char*node_name);
	ParseXml_Node FirstChild(ParseXml_Node parent_node);
	ParseXml_Node Parent(ParseXml_Node node);
	ParseXml_Node NextChild(ParseXml_Node parent_node);
	const char*NodeName(ParseXml_Node this_node);
	const char*NodeValue(ParseXml_Node this_node);
	const char*NodeAttr(ParseXml_Node this_node,const char*attr_name);
	string& GetNodeLine(ParseXml_Node this_node, string &node_line);
	//ParseXml_Node selectSingleNode(const char *jpath);
	int SetNodeValue(ParseXml_Node this_node, const char *value);
	ParseXml_Node GetNodeByXpath(const char *jpath);
	ParseXml_NodeSet GetNodeSetByXpath(const char *jpath);
	//bool DeleteNode(ParseXml_Node node);
	//ParseXml_Node Parent(ParseXml_Node child);
	
	//功能：设置jpath所指的对象的值。
	//参数：set_type:POINT_MODE,COPY_MODE
	//返回： 
	//const char *_GetValue(ParseObj_T::iterator point);

	//功能：point定位到指定对象“****.****”,或者第一个“****.****.****”的子节点;
	//		-如果没有找到则指针定位在::end
	//返回：
	//		找到指定对象-SUCCESS; 找到指定对象子节点：POINT_SON；不存在指定对象和子节点-POINT_NULL;
	//ParseXml_Nodes  selectNodes(ParseXml_Node now_node, const char *jpath);  
	//ParseXml_Nodes ChildNodes
	int Find(const char *jpath);


	//功能：取得jpath所指的对象的值。
	//返回：
	//成功-返回字符串指针；失败返回NULL;
	//注：要把所指字符串一次性拿走，否则下次更改就不知道定位在哪里了。
	const char *GetNodeValue(const char *jpath);
	int SetValue(const char *jpath,const char*value);
	ParseXml_Node AddNode(ParseXml_Node node, const char *node_name);
	ParseXml_Node AddNode(ParseXml_Node node, const char *node_name,bool is_clear);
	bool LoadSuccess();
	
	ParseXml_Doc ObjXml;//公共对象，允许用户在解析过程中添加，并可以任意调整。
	pugi::xml_parse_result ParseResult;
	string ErrorStr;
	string LocalEncode;
	string Encode;
	string FileName;
	unsigned int ParseWay;
	
private:
	bool Loaded;
};

#endif
