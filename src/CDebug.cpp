#include "stdafx.h"
#ifndef CDEBUG_CPP
#define CDEBUG_CPP
#include "fstream"
#include "CDebug.h"
#include "Linux_Win.h"
#include "string.h"
//#ifdef WIN32
//	#include<windows.h>
//#else
//#endif

CDebug::CDebug(const char *process_name)
{
#ifdef _DEBUG
	if(!init(process_name)){
			cerr << "Error: Init Debug.xml失败. " << endl;
			return ;
	}else{
			cout << ">> Init Debug.xml OK." << endl;
			return ;
	}
#endif
}

CDebug::~CDebug()
{	
}

bool CDebug::init(const char *process_name)
{
	this->sDebugDir = "./Debug";
#ifdef _DEBUG_FILE
	this->findOrCreatFload(this->sDebugDir);//创建Debug目录。

	//取得当前时间
	time_t t;
	time(&t);
	tm * lt = localtime(&t);
	int day = (lt->tm_year + 1900)*10000 + (lt->tm_mon + 1)*100 + lt->tm_mday;
	int sec = lt->tm_hour *10000 + lt->tm_min * 100 + lt->tm_sec;
	char filename[32];
	char filename_bak[32];
	memset(filename_bak, 0, 32);
	memset(filename, 0, 32);
	sprintf(filename, ".txt");
	sprintf(filename_bak, "_%d_%d.txt", day, sec);

	this->sAction=process_name; 

	this->sDebugFile=this->sDebugDir+"\\"+process_name+"_Debug"+filename;
	this->sDebugFileBak=this->sDebugDir+"\\"+process_name+"_Debug"+filename_bak;

	//备份旧文件
	rename(this->sDebugFile.c_str(),this->sDebugFileBak.c_str());

	//清空文件
	std::ofstream debug_file(this->sDebugFile.c_str());
	debug_file <<"DEBUG:起始日期:"<<lt->tm_year + 1900<<"年"<<lt->tm_mon+1<<"月"<<lt->tm_mday<<"日"<< "  ";
	debug_file <<"时间:"<<lt->tm_hour<<":"<<lt->tm_min<<":"<<lt->tm_sec<<endl;
	debug_file.close();
#else
	this->sDebugFile="";
#endif

	//是否显示普通Debug信息
	this->bNormalDebugToFile = false;
	this->bNormalDebug = true;
	this->bDeepDebug = false;
	this->bErrorDebugToFile = false;
	this->bErrorDebug = true;
	
	return true;
}

void CDebug::show(string &s)
{
	this->show(static_cast<const char *>(s.c_str()));
}

void CDebug::showError(string &s)
{
	this->showError(static_cast<const char *>(s.c_str()),"");
}

void CDebug::showError(string &s,const char *e)
{
	this->showError(static_cast<const char *>(s.c_str()),e);
}

void CDebug::show(const char * m)
{
	//#ifndef RECOVER  
		string s="DEBUG:";
		if (this->bNormalDebug){
			cout << s+m << endl;
		}
		if (this->bNormalDebug && this->sDebugFile!="" && this->bNormalDebugToFile){
			std::ofstream debug_file(this->sDebugFile.c_str(),std::ios::app);
			debug_file << s+m << endl;
			debug_file.close();
		}
	//#endif
}

void CDebug::showError(const char * m)
{
	this->showError(m,"");
}

void CDebug::showError(const char * m,const char * e){
	string s;
	s=e;
	if (s!="")
		s=s+":"+m;
	else
		s=s+"ERROR:"+m;

	if (this->bErrorDebug)
		cout <<s << endl;
	if (this->bErrorDebug && this->sDebugFile!="" && this->bErrorDebugToFile){
		std::ofstream debug_file(this->sDebugFile.c_str(),std::ios::app);
		debug_file << s << endl;
		debug_file.close();
	}
}

//查找目录,如果未发现则创建
bool  CDebug::findOrCreatFload(string &path){
	if (LW_mkdir(path.c_str ()) != 0)
	{//如果没有存在则创建文件夹:
		return false;
	}
	return true;
}
#endif
