 //------------------------------------------------------
 //功能：HTTP_Process声明
 //作者syh
 //时间06-5-10
 //------------------------------------------------------
#ifndef XmlBinProcess_H
#define XmlBinProcess_H

#include "ParseProcess.h"
#include <vector>
#include <stdlib.h>
#include <string>
#include <string.h>
#include "pugixml.hpp"
#include "XML_Mini.h"
#include "ParseXml.h"
#include "ParseObj.h"
#include "Linux_Win.h"
using namespace pugi;
using namespace std;

//new jaws add;
typedef string HTTP_Request;
typedef string HTTP_Response;
class XmlBinProcess : public ParseProcess
{
public:
	XmlBinProcess(void);
	~XmlBinProcess(void);
	bool LoadCfgFile(const char *parse_cfg);
	void InitParam(void);
	int  SetParam_ParsePacket(const char *dest_jpath,string &input_str,const char *parse_jpath);
	bool ParseBinToXmlNode(ParseXml_Node to_xml_node, string &input_str,const char *parse_path);
	bool ParseBinToXmlStr(const char * input_bin, int len,const char *parse_path, string &out_xml_str,const char *xml_root_name);
	bool ParseBinToXmlStr(string &input_str,const char *parse_path, string &out_xml_str,const char *xml_root_name);
	bool ParseBinToXmlsStr(string &input_str,const char *parse_path, string &out_xml_str,const char *xml_root_name);
	bool ParseXmlStrToBin(string &input_xml_str,const char *parse_path, string &out_bin,const char *xml_root_name);
	bool HexToBin(string &in, string &out);
	bool BinToHex(const char *in, int len, string &out);
	bool BinToHex(string &in, string &out);
	string ErrorStr;
private:

	string ObjectCfg;
};

#endif
