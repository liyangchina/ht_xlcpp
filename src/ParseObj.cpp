#include "stdafx.h"

#include "STRING_Helper.h"
//#include "HTTP_Config.h"
#include <string>
#include "string.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "pugixml.hpp"
#include <math.h>
#include <map>

//#include <zlib.h>
//#include "tchar.h"
//#include "CDBManager.h"
#include "ParseXml.h"
#include "ParseObj.h"
#include "Linux_Win.h"
#include "ByteBase.h"

using namespace std;
using namespace pugi;

//构造函数初始化point指针为开始
Http_ParseObj::Http_ParseObj(){
	
	//创建Map对象
	this->ObjMap = new ParseObj_T;

	//this->Point=(ParseObj_T::iterator)0;
	this->Point = this->ObjMap->end();

	//创建_XmlBuf对象
	this->_XmlBuf = new vector<Http_ParseXML *>;
}

Http_ParseObj::~Http_ParseObj(){
	//this->Point=(ParseObj_T::iterator)LW_NULL;
	//this->Point=(ParseObj_T::iterator)0;
	//this->Point = this->ObjMap->end();
    this->TableObjStru=NULL;

	//删除Map对象；
	if (this->ObjMap!=0)
		delete this->ObjMap;

	//删除XMLBuf中存储的xml对象；
	vector<Http_ParseXML *>::iterator it;
	for(it=this->_XmlBuf->begin();it!=this->_XmlBuf->end();++it){
		Http_ParseXML * temp_xml=*it;
		if (temp_xml!=0)
			delete temp_xml;
	}
	//删除XmlBuf自身。
	if (this->_XmlBuf!=0)
		delete this->_XmlBuf;
    
}

int Http_ParseObj::CheckJpath(const char *jpath,string &obj_path, string &xml_path){
	string s(jpath);
	return this->CheckJpath(s,obj_path,xml_path);
}

//把parse_field按":"拆分为对象路径和xml路径
int Http_ParseObj::CheckJpath(string &jpath,string &obj_path, string &xml_path){
	//数据库分拆处理
	if (jpath.find(":")!=string::npos){
		vector<string> parse_field_p;
		STRING_Helper::stringSplit(jpath,parse_field_p,":");
		obj_path=parse_field_p[0];
		xml_path=parse_field_p[1];
		return JPATH_XML;
	}else{
		obj_path=jpath;
		xml_path="";
		return JPATH_OBJ;
	}
}

bool Http_ParseObj::BArrayGetSearchStr(string tmp_str,string &search_str) {
	string part_str("");
	char *next = 0, *start = (char *)tmp_str.data();
	for (;;) {
		if (*start == '\0')
			break;
		if ((STRING_Helper::strVector_GetNext(start, part_str, next, "[]") == 1) && STRING_Helper::CheckNum(part_str)) {//取得子字串STR
			string p;
			STRING_Helper::putUINTToStr(p, STRING_Helper::ParseInt(part_str));
			BB_SEARCH_STRU  srh = BB_NEW_SEARCH_CELL(BB_SEARCH_NULL, p.data(), p.size());
			search_str += string((const char*)&srh, BB_SEARCH_CELL_SIZE((BB_OBJ*)&srh));
		}
		else {//如果语法错误
			return false;
		}
		start = next;
	};
	return true;
}

//功能：设置jpath所指的对象的值。
//参数：jpath:对象路径； value:字符串指针；set_type:STR/XML_POINT_MODE（只是指针）,STR/XML_COPY/PARSE_MODE（拷贝内容）
//返回：
//成功-SUCCESS，失败-;
int Http_ParseObj::SetNodeValue(const char *jpath, const char *value, int set_type,int len){
	string obj_path,xml_path;
	int l;

	if (this->CheckJpath(jpath,obj_path,xml_path)!=Http_ParseObj::JPATH_XML){//判断jpath,并把jpath";"拆分为对象路径和xml路径
		if ((l = obj_path.find("[")) > 0) {//ly 910
			string tmp_path = obj_path.substr(0, l);
			void *p;
			if ((p = this->GetObjNode(tmp_path.c_str(), set_type)) == NULL) {//取得节点对象
				this->ErrorStr = (string)"Http_ParseObj::SetNodeValue(): " + tmp_path + "not found!";  //这个差错不适合于递归中的嵌套（）
				return false;
			}
			string b_array = *(string *)p;

			//针对数组取得节点值,//读取所有[],[]数组内容,请该缺少头？？
			string search_str("");
			if (this->BArrayGetSearchStr(obj_path.substr(l),search_str)) {
				this->ErrorStr = (string)"[*][*]语法错误:" + obj_path;  //这个差错不适合于递归中的嵌套（）
				return false;
			}

			//取得对象KeyValue
			BB_OBJ *item;
			size_t item_size;
			//最好再次加入obj合法性校验。
			if (!BB_GET_OBJ_BY_PATH((BB_OBJ *)b_array.data(), (BB_OBJ *)search_str.data(), search_str.size(), &item, &item_size)) {
				this->ErrorStr = (string)"Object Node" + "{" + jpath + "} can't get obj_item!";
				return false;
			}
            this->ErrorStr = (string)"[*][*]语法错误:" + search_str;  //这个差错不适合于递归中的嵌套（）!!!此处需要测试。
            return false;
		}
		else {
			//如果是空值则删除
			if (set_type == Http_ParseObj::NULL_OBJ) {
				this->DeleteObjNode(jpath);
				return Http_ParseObj::SUCCESS;
			}
			//对象节点设置。
			if (set_type == Http_ParseObj::STR_COPY_OBJ || set_type == Http_ParseObj::STR_POINT_OBJ) {
				//设置字符串节点和字符串指针
				return this->SetObjNode(obj_path.c_str(), (void *)value, set_type, len);
			}
			else if (set_type == Http_ParseObj::XML_PARSE_OBJ || set_type == Http_ParseObj::XML_POINT_OBJ) {
				if (this->XmlObjCreate(obj_path.c_str(), value) != NULL)
					return Http_ParseObj::SUCCESS;
				else
					return Http_ParseObj::XML_CREATE_ERROR;
			}
		}
	}else{
		//Xml节点设置
		int src_set_type;
		Http_ParseXML *p_xml=(Http_ParseXML *)this->GetObjNode(obj_path.c_str(),src_set_type);
		if (p_xml!=NULL){
			//根据xml路径设置内容。
			if (p_xml->SetValue(xml_path.c_str(),value)==Http_ParseXML::XML_SUCCESS){
				return Http_ParseObj::SUCCESS;
			}else{
				this->ErrorStr=(string)"XML节点设置错误"+"{"+jpath+"}:"+p_xml->ErrorStr;
				return Http_ParseObj::XML_SET_ERROR;
			}
		}else{
			this->ErrorStr=(string)"XML Object Node no avalid!"+"{"+jpath+"}:"+obj_path;
			return Http_ParseObj::XML_SET_ERROR;
		}
	}
	return SET_VALUE_ERROR;
}
int Http_ParseObj::SetNodeValue(const char *jpath, int value){
	char value_str[10];
	itoa(value,value_str,10);

	return this->SetNodeValue(jpath,value_str,Http_ParseObj::STR_COPY_OBJ);
}
//功能：取得jpath所指的对象节点的值。 
//返回：
//	成功-返回字符串指针；失败返回NULL;
//	注：要把所指字符串一次性拿走，否则下次更改就不知道定位在哪里了。
const char *Http_ParseObj::GetNodeValue(const char *jpath){
	int set_type;	
	string obj_path,xml_path;

	if (this->CheckJpath(jpath,obj_path,xml_path)!=Http_ParseObj::JPATH_XML){//判断jpath,并把jpath";"拆分为对象路径和xml路径
		//取得普通Obj节点。
		void *p;
		p=this->GetObjNode(obj_path.c_str(),set_type);//取得节点对象
		if (p!=NULL && (set_type==Http_ParseObj::STR_COPY_OBJ)){
			//取得字符串节点
			string *ps=(string *)p;
			return ps->c_str();
		}if (p!=NULL && (set_type==Http_ParseObj::STR_POINT_OBJ)){
			//取得字符串节点
			return (const char *)p;
		}else if(set_type==Http_ParseObj::XML_PARSE_OBJ || set_type==Http_ParseObj::XML_POINT_OBJ){
			//取得XML对象。
			/*Http_ParseXML *p_xml=(Http_ParseXML *)this->GetObjNode(obj_path.c_str(),set_type);
			return Http_ParseXML::XmlToStr(p_xml->ObjXml);
			return NULL;*/
			this->ErrorStr=(string)"Object Node"+"{"+jpath+"} can not be return value!";
			return NULL;
		}else{
			this->ErrorStr=(string)"Object Node"+"{"+jpath+"}not exit!";
			return NULL;
		}
	}else{
		//取得XML节点值。
		const char *p;
		Http_ParseXML *p_xml=(Http_ParseXML *)this->GetObjNode(obj_path.c_str(),set_type);//根据对象路径取得xml文档。
		if (p_xml!=NULL && set_type==XML_PARSE_OBJ){
			//根据xml路径 取得内容
			if ((p=p_xml->GetNodeValue(xml_path.c_str()))!=NULL){
				return p;
			}else{
				this->ErrorStr=(string)"XML Node Reading Error!"+"{"+jpath+"}:"+p_xml->ErrorStr;
				return NULL;
			}
		}else{
			if (set_type != XML_PARSE_OBJ)
				this->ErrorStr=(string)"Obj is not xml!"+"{"+jpath+"}:"+obj_path;
			else
				this->ErrorStr=(string)"Object node not exit"+"{"+jpath+"}:"+obj_path;
			return NULL;
		}
	}
}

bool Http_ParseObj::GetNodeValueStr(const char *jpath,string & dest_str){
	int set_type;	
	string obj_path,xml_path;
	//bool is_barray = false;
	int l;

	if (this->CheckJpath(jpath,obj_path,xml_path)!=Http_ParseObj::JPATH_XML){//判断jpath,并把jpath";"拆分为对象路径和xml路径
		//过滤数组节点
		if ((l = obj_path.find("[")) > 0) {//ly 910
			dest_str = "";
			string tmp_path = obj_path.substr(0, l);
			void *p;
			if ((p = this->GetObjNode(tmp_path.c_str(), set_type)) == NULL) {//取得节点对象
				this->ErrorStr = (string)"GetNodeValueStr()->GetObjNode: "+tmp_path+"not found!";  //这个差错不适合于递归中的嵌套（）
				return false;
			}
			string b_array = *(string *)p;

			//针对数组取得节点值,//读取所有[],[]数组内容,请该缺少头？？
			string search_str(""), part_str(""), tmp_str = obj_path.substr(l); //tmp_str = obj_path.substr(l+1,obj_path.size()-l-2);
			char *next = 0, *start = (char *)tmp_str.data();
			for (;;) {
				if (*start == '\0')
					break;
				if ((STRING_Helper::strVector_GetNext(start, part_str, next, "[]")==1) && STRING_Helper::CheckNum(part_str)) {//取得子字串STR
					string p;
					STRING_Helper::putUINTToStr(p, STRING_Helper::ParseInt(part_str));
					BB_SEARCH_STRU  srh = BB_NEW_SEARCH_CELL(BB_SEARCH_NULL, p.data(), p.size());
					search_str += string((const char*)&srh, BB_SEARCH_CELL_SIZE((BB_OBJ*)&srh));
				}else{//如果语法错误
					this->ErrorStr = (string)"[*][*]语法错误:" + search_str;  //这个差错不适合于递归中的嵌套（）
					return false;
				}
				start = next;
			};

			//取得对象KeyValue
			BB_OBJ *item;
			size_t item_size;
				//最好再次加入obj合法性校验。
			if (!BB_GET_OBJ_BY_PATH((BB_OBJ *)b_array.data(), (BB_OBJ *)search_str.data(), search_str.size(), &item, &item_size)) {
				this->ErrorStr = (string)"Object Node" + "{" + jpath + "} can't get obj_item!";
				return false;
			}

			dest_str = string((const char*)item, item_size);
			/*if ((BB_GET_OBJ_TYPE(item) == BB_ARRAY) && BB_GET_OBJ_DATA_LEN(item) == 0) {//针对空数组的专门处理
				if (BB_GET_OBJ_SIZE(item) == item_size) {
					cout << "Error:PubObj(Http_ParseObj::GetNodeValueStr(A[0][1][0]???))!" << endl;
					dest_str = "";
				}
				else {
					dest_str = string((const char*)item, item_size);
				}
			}			
			else
				dest_str = string((const char*)item, item_size);*/
			return true;
		}
		else {
			//取得普通Obj节点。
			void *p =this->GetObjNode(obj_path.c_str(),set_type);//取得节点对象
			if (p!=NULL && (set_type==Http_ParseObj::STR_COPY_OBJ)){
				//取得字符串节点
				dest_str=*(string *)p; //??if can does
				return true;
			}if (p!=NULL && (set_type==Http_ParseObj::STR_POINT_OBJ)){
				//取得字符串节点
				this->ErrorStr=(string)"Object Node"+"{"+jpath+"} can not be return point value!";
				return false;
			}else if(set_type==Http_ParseObj::XML_PARSE_OBJ || set_type==Http_ParseObj::XML_POINT_OBJ){
				//取得XML对象。
				this->ErrorStr=(string)"Object Node"+"{"+jpath+"} can not be return value!";
				return false;
			}else{
				this->ErrorStr=(string)"Object Node"+"{"+jpath+"}not exit!";
				return false;
			}
		}	
	}else{
		//取得XML节点值。
		const char *p;
		Http_ParseXML *p_xml=(Http_ParseXML *)this->GetObjNode(obj_path.c_str(),set_type);//根据对象路径取得xml文档。
		if (p_xml!=NULL && set_type==XML_PARSE_OBJ){
			//根据xml路径 取得内容
			if ((p=p_xml->GetNodeValue(xml_path.c_str()))!=NULL){
				dest_str =p;
				return true;
			}else{
				this->ErrorStr=(string)"XML Node Reading Error!"+"{"+jpath+"}:"+p_xml->ErrorStr;
				return false;
			}
		}else{
			if (set_type != XML_PARSE_OBJ)
				this->ErrorStr=(string)"Obj is not xml!"+"{"+jpath+"}:"+obj_path;
			else
				this->ErrorStr=(string)"Object node not exit"+"{"+jpath+"}:"+obj_path;
			return false;
		}
	}
}

int Http_ParseObj::GetAllSonNodeValue(const char *jpath, vector<string> &dest_vector) {
	//查找指定路径节点。
	if (this->Find(jpath) == Http_ParseObj::ERROR_POINT_NULL)
		return Http_ParseObj::ERROR_POINT_NULL;

	dest_vector.clear();
	string tmp;
	for (;;) {		
		if (this->GetNextNodeValueStr(jpath, tmp) != Http_ParseObj::SUCCESS)
			break;
		dest_vector.push_back(tmp);
	}

	//删除指定节点。
	return Http_ParseObj::SUCCESS;
}

int Http_ParseObj::GetAllSonNodeNum(const char *jpath) {
    if (this->Find(jpath)==Http_ParseObj::ERROR_POINT_NULL)
        return Http_ParseObj::ERROR_POINT_NULL;
    
    //if (strlen(split_str)==0)
    //判断删除子节点。
    int i=0;
    ParseObj_T::iterator temp1=this->Point;
    for(;;){
        //下一节点
        this->Point++;
        //如果结尾，则完成
        if (this->Point==this->ObjMap->end())
            break;
        //如果名称不包含，则完成
        if(this->Point->first.find(temp1->first)!=0)
            break;
        i++;
    }
    return i;
}

int Http_ParseObj::GetNextNodeValueStr(const char *jpath, string & dest_str) {
	ParseObj_T::iterator temp1 = this->Point;

	if (this->Point == this->ObjMap->end())
		return Http_ParseObj::ERROR_POINT_NULL;
	
	this->Point++;

	//如果结尾，则完成
	if (this->Point == this->ObjMap->end())
		return Http_ParseObj::ERROR_POINT_NULL;

	//如果名称不包含，则完成
	if (this->Point->first.find(temp1->first) != 0)
		return Http_ParseObj::ERROR_OUT_SON;

	ParseObj_T::iterator temp2 = this->Point;
	this->GetNodeValueStr(temp2->first.c_str(), dest_str);
	return Http_ParseObj::SUCCESS;
}
int Http_ParseObj::AddChildNodesValue(const char *jpath,string &result_str){
	return this->AddChildNodesValue(jpath,result_str,"");
}

int Http_ParseObj::AddChildNodesValue(const char *jpath,string &result_str,const char *split_str){
	//查找指定路径节点。
	if (this->Find(jpath)==Http_ParseObj::ERROR_POINT_NULL)
		return Http_ParseObj::ERROR_POINT_NULL;

	//if (strlen(split_str)==0)
	//判断删除子节点。
	string tmp_str;
	ParseObj_T::iterator temp1=this->Point;
	for(;;){
		//下一节点
		this->Point++;
		//如果结尾，则完成
		if (this->Point==this->ObjMap->end())
			break;
		//如果名称不包含，则完成
		if(this->Point->first.find(temp1->first)!=0)
			break;
		ParseObj_T::iterator temp2=this->Point;
		//累加结果节点
		this->GetNodeValueStr(temp2->first.c_str(),tmp_str);
        //cout <<temp2->first.c_str();
		result_str.append(tmp_str);
		if (strlen(split_str)!=0)
			result_str.append(split_str);
		//下一节点
	}
    //cout <<endl;

	//删除指定节点。
	return Http_ParseObj::SUCCESS;
}

//把jpath对象（xml文档），转化成xml字符串。
bool Http_ParseObj::XmlNode(const char *jpath,ParseXml_Node &node){
	int set_type;
	string obj_path,xml_path;
	int check_jpath= this->CheckJpath(jpath,obj_path,xml_path);

	//返回xml对象
	Http_ParseXML *p_xml=(Http_ParseXML *)this->GetObjNode(obj_path.c_str(),set_type);//取得xml对象

	if (p_xml!=NULL && (set_type==Http_ParseObj::XML_PARSE_OBJ || set_type== Http_ParseObj::XML_POINT_OBJ)){
		//如果是xml对象。
		if (check_jpath==Http_ParseObj::JPATH_XML){
				//取得普通xpath节点
				node = p_xml->GetNodeByXpath(xml_path.c_str());
			//}
		}else{
			//如果是xml文档根节点。取得域值集合，并按split累加
			node = p_xml->GetNodeByXpath(".");
		}
		return true;
	}else{
		this->ErrorStr=(string)""+"{"+jpath+"}"+" variable not exit or Xml not exit !";
		return false;
	}
}

//把jpath对象（xml文档），转化成xml字符串。
string &Http_ParseObj::XmlAttrText(const char *jpath,string attr,string &xml_text){
	ParseXml_Node node;
	if (this->XmlNode(jpath,node)){
		xml_text = node.attribute(attr.c_str()).value();
	}else
		xml_text="";

	return xml_text;
}

//把jpath对象（xml文档），转化成xml字符串。
string &Http_ParseObj::XmlNodeText(const char *jpath,string &xml_text){
	ParseXml_Node node;
	if (this->XmlNode(jpath,node))
		xml_text = Http_ParseXML::XmlToStr(node);
	else
		xml_text="";

	return xml_text;
}

/*//把jpath对象（xml文档），转化成xml字符串。
string &Http_ParseObj::XmlNodesText(const char *jpath,string &xml_text,string &split){
	ParseXml_NodeSet nodes;
	if (this->XmlChildNodes(jpath,nodes))
		xml_text = Http_ParseXML::XmlToStr(nodes,split);
	else
		xml_text="";

	return xml_text;
}*/

//把jpath对象（xml文档），转化成xml字符串。
//string &Http_ParseObj::XmlNodeValue(const char *jpath,string &xml_value){
//	ParseXml_Node node;
//	if (this->XmlNode(jpath,node))
//		xml_value = Http_ParseXML::GetNodeValue(node);
//	else
//		xml_value="";
//
//	return xml_value;
//}


//把jpath对象（xml文档），转化成xml字符串。
string &Http_ParseObj::XmlChildNodesText(const char *jpath,string &xml_text,const char *split){
	ParseXml_NodeSet nodes;
	const char *jpath_temp;
	if (this->XmlChildNodes(jpath,nodes)){
		jpath_temp=jpath+strlen(jpath)-1;
		if (jpath_temp[0]=='.')
			xml_text = Http_ParseXML::XmlValueToStr(nodes,split);
		else
			xml_text = Http_ParseXML::XmlToStr(nodes,split);
	}else
		xml_text="";

	return xml_text;
}


//把jpath对象（xml文档），转化成xml字符串。
string &Http_ParseObj::XmlChildNodesText(const char *jpath,string &xml_text,const char *split,vector <string> &fields){
	size_t i;
	string spath=jpath;

	for (i=0; i< fields.size(); i++){
		string s;
		xml_text +=this->XmlNodeText((spath+"/"+fields[i]).c_str(),s)+split;
	}

	if (xml_text.size()!=0){
		xml_text.substr(0,xml_text.size()-strlen(split));
		return xml_text;
	}else
		return xml_text;
}

//把jpath对象（xml文档），转化成xml字符串。
bool Http_ParseObj::XmlChildNodes(const char *jpath,ParseXml_NodeSet &nodes){
	int set_type;
	string obj_path,xml_path;
	int check_jpath= this->CheckJpath(jpath,obj_path,xml_path);

	//返回xml对象
	Http_ParseXML *p_xml=(Http_ParseXML *)this->GetObjNode(obj_path.c_str(),set_type);//取得xml对象

	if (p_xml!=NULL && (set_type==Http_ParseObj::XML_PARSE_OBJ || set_type== Http_ParseObj::XML_POINT_OBJ)){
		//如果是xml对象。
		if (check_jpath==Http_ParseObj::JPATH_XML){
			//如果存在xml子路径
			//if (xml_path.compare(xml_path.size()-2,2,"/*")==0 || xml_path.compare(xml_path.size()-2,2,"\\*")==0)
				//nodes = p_xml->GetNodeSetByXpath(xml_path.c_str());	
			//else
				nodes = p_xml->GetNodeSetByXpath((xml_path).c_str());	
		}else{
			//如果是xml文档根节点。取得域值集合，并按split累加
			nodes = p_xml->GetNodeSetByXpath(".");
		}
		return true;
	}else{
		this->ErrorStr=(string)"{"+jpath+"}"+"variable not exit or Xml not exit !";
		return false;
	}
}

Http_ParseXML *Http_ParseObj::XmlObjCreate(const char *jpath, const char *xml_str){
	Http_ParseXML * temp_xml = new Http_ParseXML();//创建xml对象。

	int r=temp_xml->Load(xml_str);//从字符串中load xml对象。
	if (r==Http_ParseXML::LOAD_XML_SUCCESS){//this->request_.headers()["X-Curtain-Auth"].value()
		this->ErrorStr=temp_xml->ErrorStr;
		//查询是否存在，同名路径,且为xml对象。如果发现则delete 该对象并从列表中删除
		this->DeleteObjNode(jpath);
		//实际创建xml新对象
		this->_XmlBuf->push_back(temp_xml);//!!!记录xml对象，便于删除。
		this->SetObjNode(jpath,temp_xml,Http_ParseObj::XML_PARSE_OBJ);
		//cout << temp_xml->XmlToStr(temp_xml->ObjXml) << endl;
		return temp_xml;
	}else{
		this->ErrorStr=temp_xml->ErrorStr;
		delete temp_xml;//删除对象。
		return NULL;
	}
}

bool Http_ParseObj::XmlNodeLoadText(const char *jpath, const char *xml_str){
	//测试jpath是否为正确的xml节点

	//创建临时对象
	Http_ParseXML * temp_xml = new Http_ParseXML();//创建xml对象。
	int r=temp_xml->Load(xml_str);//从字符串中load xml对象。

	if (r!=Http_ParseXML::LOAD_XML_SUCCESS){
		this->ErrorStr=temp_xml->ErrorStr;
		delete temp_xml;//删除对象。
		return false;
	}


	//克隆操作
	if (this->XmlNodeClone(jpath,temp_xml->FirstChild(temp_xml->ObjXml),XML_CLONE_ADD_CHILD)){
		//释放对象
		delete temp_xml;
		return true;
	}else{
		delete temp_xml;
		return false;
	}
}

ParseXml_Node Http_ParseObj::XmlNodeClone(const char *jpath, ParseXml_Node src_node,int clone_type){
	//测试jpath是否为正确的xml节点
	string obj_path,xml_path;
	ParseXml_Node node;
	ParseXml_Node false_node;
	int set_type;
	//分析jpath
	int check_jpath= this->CheckJpath(jpath,obj_path,xml_path);

	//返回xml对象
	Http_ParseXML *p_xml=(Http_ParseXML *)this->GetObjNode(obj_path.c_str(),set_type);//取得目标xml对象

	if (p_xml!=NULL && (set_type==Http_ParseObj::XML_PARSE_OBJ || set_type== Http_ParseObj::XML_POINT_OBJ)){
		//如果是xml对象,取得目标node节点。
		if (check_jpath==Http_ParseObj::JPATH_XML){
			//如果存在xml子路径
				node = p_xml->GetNodeByXpath((xml_path).c_str());	
		}else{
			//如果是xml文档根节点。取得域值集合，并按split累加
			node = p_xml->GetNodeByXpath(".");
		}
		if(!node){
			this->ErrorStr=(string)"{"+jpath+"}"+"xml node not exist !";
			return false_node;
		}

		//克隆操作,之后应该把该函数移入xml对象中
		if (clone_type==XML_CLONE_ADD_CHILD){
			//if ((ParseXml_Node to_node=node.first_child())){
			//	return node.insert_copy_after(src_node,to_node);
			//}else{
			return node.append_copy(src_node);
			//}
		}else
			return false_node;
	}else{
		this->ErrorStr=(string)"{"+jpath+"}"+"必须是xml节点对象";
		return false_node;
	}
}


int Http_ParseObj::SetObjNode(const char *jpath, void *value,int set_type,int len){
	//判断是否已经存在;
	ParseObj_T::iterator f;
	string jpath1(jpath);
	STRING_Helper::strTrim(jpath1);//必须去除两边空位

	//设置对象值
	if ((f=this->ObjMap->find(jpath1))==this->ObjMap->end()){ //新记录插入
		this->_Insert(jpath1.c_str(),value,set_type,len);
	}else{//替换性插入
		string temp= f->first;
		this->ObjMap->erase(f);
		this->_Insert(temp.c_str(),value,set_type,len);
	}
 
	return Http_ParseObj::SUCCESS;
}
//删除操作，目前可以删除XML_PARSE_OBJ;XML_POINT_OBJ;STR_COPY_OBJ；STR_POINT_OBJ
int Http_ParseObj::DeleteObjNode(string jpath){
	return this->DeleteObjNode(jpath.c_str());
}
int Http_ParseObj::DeleteObjNode(const char *jpath){
	int old_set_type;	
	void *p =(Http_ParseXML *)this->GetObjNode(jpath,old_set_type);
	if ((p!=NULL)){
		//删除XML_OBJ
		if (old_set_type==Http_ParseObj::XML_PARSE_OBJ){
			Http_ParseXML *temp_old_xml=(Http_ParseXML *)p;			
			vector<Http_ParseXML *>::iterator it;
			for(it=this->_XmlBuf->begin(); it!=this->_XmlBuf->end(); ++it){
				if (*it==temp_old_xml){
					this->_XmlBuf->erase(it);//删除旧队列
					break;
				}
			}
			delete temp_old_xml;//删除旧xml对象??这里是否能否彻底释放对象??
			//删除ObjNode当前节点
		}
		if (old_set_type==Http_ParseObj::XML_PARSE_OBJ||old_set_type==Http_ParseObj::XML_POINT_OBJ||old_set_type==Http_ParseObj::STR_COPY_OBJ || old_set_type==Http_ParseObj::STR_POINT_OBJ){
			//删除ObjNode,所有情况
			ParseObj_T::iterator f;
			f=this->ObjMap->find(jpath);
			this->ObjMap->erase(f);
		}
		
		return Http_ParseObj::SUCCESS;
	}else
		return Http_ParseObj::SUCCESS;
}


//功能：设置jpath所指的对象的值。
//参数：set_type:POINT_MODE,COPY_MODE
//返回： 
void Http_ParseObj::_Insert(const char*jpath, void *value, int set_type, int len){
	//设置对象值
	ParseObjValue_T v;
	void *value2;

	//针对定长处理
	if ((len>=0) && (set_type==Http_ParseObj::STR_COPY_OBJ)){
		string value1((char *)value,len);
		value2 = NULL;
		v = make_pair(value1,value2);
	}else{
		string value1;
		//设置pair值
		if (set_type==Http_ParseObj::STR_POINT_OBJ){//只是记录指针STR_POINT_OBJ
			value1 = "STR_POINT_OBJ";
			value2 = (void *)value;
		}else if (set_type==Http_ParseObj::XML_PARSE_OBJ){//只是记录指针XML_PARSE_OBJ
			value1 = "XML_PARSE_OBJ";
			value2 = (void *)value;
		}else if (set_type==Http_ParseObj::XML_POINT_OBJ){//只是记录指针XML_POINT_OBJ
			value1 = "XML_POINT_OBJ";
			value2 = (void *)value;
		}else {//拷贝字符串记录STR_COPY_OBJ)
			value1 = (char *)value; 
			value2 = NULL;
		}
		v = make_pair(value1,value2);
	}
	this->ObjMap->insert(ParseObj_ValueT(string(jpath),v));
}

int  Http_ParseObj::GetObjNodeType(const char *jpath){
	int obj_type;
	if (this->GetObjNode(jpath,obj_type)!=NULL)
		return obj_type;
	else
		return -1;
}

void  *Http_ParseObj::GetObjNode(const char *jpath){
	int set_type;
	return this->GetObjNode(jpath,set_type);
}


void  *Http_ParseObj::GetObjNode(const char *jpath, int &set_type){
	ParseObjValue_T *p;
	//string obj_path,xml_path;

	p=this->_ParseObjValue(jpath);
	if(p==NULL)
		return NULL;//没有找到。

	if(p->second==NULL){
		set_type=Http_ParseObj::STR_COPY_OBJ;
		return (void *)&(p->first);//COPY_MODE
	}
	if (p->first=="STR_POINT_OBJ"){
		set_type = Http_ParseObj::STR_POINT_OBJ;
		return p->second;//返回指针，POINT_MODE
	}else if(p->first=="XML_PARSE_OBJ"){
		set_type = Http_ParseObj::XML_PARSE_OBJ;
		return p->second;
	}else if(p->first=="XML_POINT_OBJ"){
		set_type = Http_ParseObj::XML_POINT_OBJ;
		return p->second;
	}else
		return NULL; //未定义结果错误;
	
}

/*pugi::xml_document Http_ParseObj::GetValue(const char *jpath){
	void *p;
	int set_type;

	p=this->GetValue(jpath,set_type);	
	if (p!=NULL && (set_type==Http_ParseObj::XML_PARSE_OBJ || set_type==Http_ParseObj::XML_POINT_OBJ))
		return (pugi::xml_document)p;
	else
		return NULL;
}*/


ParseObjValue_T * Http_ParseObj::_ParseObjValue(const char *jpath){
	ParseObj_T::iterator find;
	string jpath1(jpath);
	STRING_Helper::strTrim(jpath1);//必须去除两边空位

	if ((find=this->ObjMap->find(jpath1))==this->ObjMap->end()){//如果未存
		return NULL;
	}else{//如果已存在,返回字串。
		ParseObjValue_T *p=&(find->second);
		return p;
	}
}

//功能：取得取得jpath所指的对象的长度；
//返回：成功>=0; 差错：-1；
int Http_ParseObj::FuncValueLen(const char *jpath){
	const char *buff;
	if ((buff=this->GetNodeValue(jpath))!=NULL)
		return (int)strlen(buff);
	else
		return -1;
}


//功能：point定位到指定对象“****.****”,或者第一个“****.****.****”的子节点;
//		-如果没有找到则指针定位在::end
//返回：
//		找到指定对象-SUCCESS; 找到指定对象子节点：POINT_SON；不存在指定对象和子节点-POINT_NULL;
int Http_ParseObj::Find(const char *jpath){
	this->Point=this->ObjMap->find(jpath);
	if (this->Point!=this->ObjMap->end()){
		return Http_ParseObj::SUCCESS;
	}else
		return Http_ParseObj::ERROR_POINT_NULL;
}

//int Http_ParseObj::find_end(const char *jpath){
//	if (this->find(jpath)==Http_ParseObj::SUCCESS){
//		for(;;){
//			ParseObj_T::iterator last_point=this->Point;
//			if (this->next(jpath)==Http_ParseObj::ERROR_POINT_NULL)
//				break;
//		}
//		this->Point=last_point;
//	}else
//		return Http_ParseObj::ERROR_POINT_NULL;
//}

//功能：-point指针下移，到指定对象“****.****”,或者“****.****.****”的子节点;
//		-如果没有找到则指针定位在::end
//返回：
//      指定对象-SUCCESS; 找到指定对象子节点：POINT_SON；不存在指定对象和子节点-POINT_NULL;	
//int Http_ParseObj::next(const char *jpath){
//	this->Point++;
//	if (this->Point!=this->ObjMap->end()){
//		return Http_ParseObj::SUCCESS;
//	}else
//		return Http_ParseObj::ERROR_POINT_NULL;
//}

//??一下内容尚未测试??
int Http_ParseObj::Delete(const char *jpath,bool is_del_son,bool is_del_this){
	//查找指定路径节点。
	if (this->Find(jpath)==Http_ParseObj::ERROR_POINT_NULL)
		return Http_ParseObj::ERROR_POINT_NULL;

	//判断删除子节点。
	string key_str=this->Point->first;
	ParseObj_T::iterator temp1=this->Point;
    //size_t len=strlen(jpath);
	if (is_del_son){
		for(;;){
			//如果结尾，则完成
			if (this->Point==this->ObjMap->end())
				break;
			//如果名称不包含，则完成
			if(this->Point->first.find(key_str)!=0)
				break;
			ParseObj_T::iterator temp2=this->Point;
			this->Point++;
            //如果是自己，可能不删除
            if (!is_del_this && temp2==temp1)
                continue;
            this->ObjMap->erase(temp2);
		}
		return Http_ParseObj::SUCCESS;
	}

	//删除指定节点。
	//cout << "Del:" << temp1->first << endl;
    this->ObjMap->erase(temp1);
	return Http_ParseObj::SUCCESS;
}

//功能：point指针下移，如果没有找到则指针定位在::end
//返回：
//      指定对象-SUCCESS; 不存在指定对象和子节点-POINT_NULL;
int Http_ParseObj::next(const char *jpath){
	this->Point++;
	if (this->Point!=this->ObjMap->end()){
		return Http_ParseObj::SUCCESS;
	}else
		return Http_ParseObj::ERROR_POINT_NULL;
}
//
//int Http_ParseObj::last(const char *jpath){
//	this->Point--;
//	if (this->Point!=this->ObjMap->end()){
//		return Http_ParseObj::SUCCESS;
//	}else
//		return Http_ParseObj::ERROR_POINT_NULL;
//}
