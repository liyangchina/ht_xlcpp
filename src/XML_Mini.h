#pragma once
#ifndef XML_MINI
#define XML_MINI
#include <vector>
#include <string>
using namespace std;

class XML_Mini
{
public:
	XML_Mini(void);
	~XML_Mini(void);
	bool ReadXML_Field(string & xmlcontent, string field_name, string &field_content);
	int  ReadXMLByFile(string sXmlFilePath,string &xmlcontent);
	bool WriteXML_Field(string & xmlcontent, string field_name, string &field_content);
	int  WriteXMLToFile(string sXmlFilePath,string &xmlcontent);
	bool ReadXML_UserAccess(string & xmlcontent, string user_auth_field, string user_access_field, string user_name, string &user_access_str);
	bool ReadXML_UserAuthIn(string user_name,string auth_str);
	bool ReadXML_MenuActionIn(string auth_menu_access,string menu_name);
	//bool HasNode(string & xmlcontent,string node_name);
};
#endif
