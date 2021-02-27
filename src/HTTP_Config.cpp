#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
//#include "windows.h"
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
#include "HTTP_Config.h"
#include "ParseObj.h"
#include "Linux_Win.h"


//#include "HTTP_Helpers.h"
JawsConfig *HTTP_Config::jawscfg;
#ifdef JAWS_CDBM
	CDBManager * HTTP_Config::DBxml;
#endif
#ifdef SENSOR_DBM
	SensorDBManager * HTTP_Config::SensorDB;
#endif
Http_ParseObj * HTTP_Config::SysPubObj;

JawsConfig::JawsConfig(const char *filename)
{
	this->readfileinfo(filename);
}
//返回配置结果
void JawsConfig::getValue(std::string AppName, std::string KeyName, std::string defaultValue,std::string & result)
{
	result=this->m_ConfigInfo_[AppName][KeyName];
	if(result=="")
		result=defaultValue;
}

int JawsConfig::getValue(std::string AppName, std::string KeyName, int defaultValue)
{
	int result;
	std::string restr;
	restr=this->m_ConfigInfo_[AppName][KeyName];
	if(restr=="")
		result=defaultValue;
	else
		result=atoi(restr.c_str());
	return result;
}

//add by zt 07/08/30
void JawsConfig::getValue(std::string AppName, std::map<std::string, std::string> & KeyApp)
{
	KeyApp = this->m_ConfigInfo_[AppName];
}

bool JawsConfig::readfileinfo(const char *filename)
{
	//读取ini配置文件
	std::ifstream stream(filename);
	//is load success
	if ( (stream.rdstate() & std::ifstream::failbit ) != 0 )
		this->isLoadSuccess = false;
	else
	    this->isLoadSuccess = true;

	string s,file_temp,file_str;     
	while (getline(stream,s)){
		file_str+=s+"\r\n";
	}
	const char* strconfig=file_str.c_str();

	//读取文件结束
	return parseIni(strconfig);
	stream.close();
	
	return true;
}

bool JawsConfig::parseIni(std::string  strconfig)
{
	size_t begin,end,ipos;
	ipos=0;
	std::string strLine,strLine1,strLine2;
	int reValue;
	std::string strName;
	while(1)
	{
		strLine="";
		begin=ipos;
		end=strconfig.find("\r\n",ipos);
		if(begin!=string::npos && end!=string::npos && end>begin)
		{
			strLine=strconfig.substr(begin,end-begin);
			ipos=end+2;
		}
		else if(begin!=string::npos && end==string::npos && strconfig.length()>begin)
		{
			strLine=strconfig.substr(begin);
			ipos=string::npos;
		}
		else if(end==string::npos)
			break;
		else
		{
			ipos=ipos+2;
			continue;
		}
		reValue=this->parseLine(strLine,strLine1,strLine2);
		switch (reValue)
		{
		case 1:
			strName=strLine1;	
			break;
		case 2:
			this->m_ConfigInfo_[strName][strLine1]=strLine2;
			break;
		default:
			break;
		}
		if(ipos==string::npos)
			break;
	}
	
	return true;
		

}


//解析单行的内容,返回0表示该行无内容,或内容不符合规格.
//1表示restr1为name,此时restr2
//2表示restr1为key,restr2为content
int JawsConfig::parseLine (std::string strLine,std::string & restr1,std::string & restr2)
{
	restr1="";
	restr2="";
	size_t begin,end,ipos;
	begin=strLine.find_first_not_of(" \r\n");
	end=strLine.find_last_not_of(" \r\n");
	if(begin==string::npos || end==string::npos)
		return 0;
	else if(strLine[begin]=='#')
		return 0;
	else if(strLine[begin]=='[' && strLine[end]==']')
	{
		restr1=strLine.substr(begin+1,end-begin-1);
		return 1;
	}
	else if(strLine.find("=") !=string::npos)
	{
		ipos=strLine.find("=");
		restr1=strLine.substr(begin,ipos-begin);
		restr2=strLine.substr(ipos+1,end-ipos);
		return 2;
	}
	else
		return 0;
	
}

void JawsConfig::test()
{
	cout<<"--------------------------------------------"<<endl;
	cout<<(unsigned int)this->m_ConfigInfo_.size()<<endl;
	_iniInfo::iterator ipos1;
	std::map <std::string, std::string> ::iterator ipos2;
	for(ipos1=this->m_ConfigInfo_.begin();ipos1!=this->m_ConfigInfo_.end(); ipos1++)
	{
		cout<<"---------------------------"<<endl;
		cout<<"["<<ipos1->first<<"]"<<endl;
		for(ipos2=ipos1->second.begin();ipos2!=ipos1->second.end();ipos2++)
		{
			cout<<ipos2->first<<"=";
			cout<<ipos2->second<<endl;
		}
		
	}
}
