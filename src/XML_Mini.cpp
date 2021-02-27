#include "stdafx.h"
#include "XML_Mini.h"
#include <vector>
#include <string>
#include "string.h"
#include <stdio.h>
#include "STRING_Helper.h"
#include <fstream>

using namespace std;

XML_Mini::XML_Mini(void)
{
};

XML_Mini::~XML_Mini(void)
{
};

//读取xml文档中的字段内容
bool XML_Mini::ReadXML_Field(string & xmlcontent, string field_name, string &field_content){
	//如果发现<fieldname/> 
	if (xmlcontent.find((string)"<"+field_name+"/>")!=xmlcontent.npos){
		field_content="";
		return true;
	}
	//取得<fieldname...></fieldname>之间的内容
	string str1;
	if (!STRING_Helper::strStringCutField(xmlcontent,str1,(string)"<"+field_name,(string)"</"+field_name+">"))
		return false;
	//取得">之后的内容
	size_t first = str1.find_first_of(">");
	if(first == str1.npos)
		return true;
	field_content = str1.substr(first+1,str1.size()-(first+1));
	return true;
}

//读取xml制定用户在文档中的权限
bool XML_Mini::ReadXML_UserAccess(string & xmlcontent, string user_auth_field, string user_access_field, string user_gnid, string &user_access_str){
	//测试user_name是否存在user_auth_field字段中;
	string user_auth_str;
	if (!this->ReadXML_Field(xmlcontent,user_auth_field,user_auth_str))
		return false;
	//string user_name1;
	//STRING_Helper::strStringCutField(user_name,user_name1,(string)"(",(string)")");
	if (!this->ReadXML_UserAuthIn(user_gnid,user_auth_str))
		return false;//？？？该处是否应该采用包含关系
	//循环查找处理

	//如果存在，则读取user_access_field字段的内容;
	if (!this->ReadXML_Field(xmlcontent,user_access_field,user_access_str))
		return false;
	else
		return true;
};

bool XML_Mini::ReadXML_UserAuthIn(string user_gnid,string auth_str){

	vector<string> auths;
	vector<string>::iterator scit;
    vector<string>::iterator seit;
	STRING_Helper::stringSplit(auth_str,auths,";");//把auth_str按";"拆分.

	seit = auths.end();
	for(scit = auths.begin(); scit != seit; ++scit)
    {
		string ac= (*scit).substr((*scit).find("(")+1,(*scit).size()-((*scit).find("(")+1)-1);//取得gnid中的(*)
		if (user_gnid.find(ac)!=user_gnid.npos)
			return true;
    }
	return false;
};

//读取xml制定用户在文档中的权限
bool XML_Mini::ReadXML_MenuActionIn(string auth_menu_access,string menu_name){
	//cout << (auth_menu_access.find(menu_name+";")) <<endl;
	//cout << (auth_menu_access.find((string)";"+menu_name)) <<endl;
	//cout << (auth_menu_access==menu_name) << endl;
	//测试user_name是否存在user_auth_field字段中;
	if(auth_menu_access.find(menu_name+";")!=auth_menu_access.npos)
		return true;
	else if (auth_menu_access.find((string)";"+menu_name)!=auth_menu_access.npos)
		return true;
	else if (auth_menu_access==menu_name)
        return true;
	else
        return false;
}

int XML_Mini::ReadXMLByFile(string sXmlFilePath,string &xmlcontent) 
{ 
	char currch; 	
	string str;
  
	//读取文件内容
	//PCTSTR szFileName;
	std::ifstream ifs;//打开文件
#ifdef _LY_CLR
	ifs.open(STRING_Helper::AnsiToUnicode(sXmlFilePath.c_str()));
#else
	ifs.open(sXmlFilePath.c_str());
#endif
	if (!ifs)
		return -3; //如果打不开文件
	str.erase(); 
	while(1) 
	{ 
		ifs.get(currch); 
		if ( ifs.bad() )//there is an unrecoverable error. 
			return -1; 
		if (ifs.eof())
			break;
		if (currch == '\0') 
			break; //？？？
		else 
			str += currch; 
	} 
	ifs.close();//关闭这个文件。

	xmlcontent=str;
	return 0;       
}

//写入xml文件字段的函数,注:必须先有该字段.
bool XML_Mini::WriteXML_Field(string & xmlcontent, string field_name, string &field_content){
	//准备工作,找到field所在节点
	string split_str;
	string f_c;
	this->ReadXML_Field(xmlcontent,field_name,f_c);
	split_str = (string)"<"+field_name+">"+f_c+"</"+field_name+">";

	//如果发现<fieldname/> 
	if (xmlcontent.find(split_str)==string::npos){
		return false;
	}
	//取得<fieldname...></fieldname>前后的内容
	vector<string> sub_content;
	STRING_Helper::stringSplit(xmlcontent,sub_content,split_str);
	if(sub_content.size()<2)
		return false;//把content拆分.	

	//更新字段
	xmlcontent.clear();
	xmlcontent=sub_content[0]+"<"+field_name+">"+field_content+"</"+field_name+">"+sub_content[1];
	return true;
};

int XML_Mini::WriteXMLToFile(string sXmlFilePath,string &xmlcontent){
	ofstream save_file; 
#ifdef _LY_CLR
	save_file.open(STRING_Helper::AnsiToUnicode(sXmlFilePath.c_str()));
#else
	save_file.open(sXmlFilePath.c_str());
#endif
	save_file << xmlcontent;
	save_file.close();
	return 0;
};

/*
int XML_Mini::ReadXMLByFile(string sXmlFilePath,string &xmlcontent) 
{ 
	LPCTSTR szFileName;
	LPCTSTR szFileContent;
	STRING_Helper::Ascii2WideString(sXmlFilePath,szFileName)
	HANDLE=OpenFile(szFileName);
	szFileContent = 

	char currch;
	string str;
  
	//读取文件内容

	std::ifstream ifs(sXmlFilePath.c_str());//打开文件
	if (!ifs)
		return -3; //如果打不开文件
	str.erase(); 
	while( !ifs.eof() ) 
	{ 
	ifs.get(currch); 
	if ( ifs.bad() )//there is an unrecoverable error. 
	return -1; 
	if (currch == '\0') 
	break; //？？？
	else 
	str += currch; 
	} 
	ifs.close();//关闭这个文件。

	xmlcontent=str;
	return 0; 
}*/
