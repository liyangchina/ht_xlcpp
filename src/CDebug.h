#ifndef CDEBUG_H
#define CDEBUG_H

#define DEBUG_OUT
#define DEBUG_OBJ
//#define _DEBUG
#include "time.h"
#include <iostream>
#include <sstream>

using namespace std;

class CDebug
{
public:
	CDebug(const char *process_name); //构造函数
	~CDebug();//希构函数
	  bool init(const char *process_name); //初始化
	  void show(const char * m);
	  void showError(const char * m);
	  void showError(const char * m,const char *e);
	  void show(string &s);
	  void showError(string &s);
	  void showError(string &s,const char *e);

	  bool findOrCreatFload(string &path);

	  string sAction;//当前的程序名称XMLServiceRead、Write....
	  string sDebugFile; //Debug文件输出指针。
	  string sDebugFileBak; //Debug文件输出指针。
	  bool bNormalDebug; //是否现实D类信息
	  bool bNormalDebugToFile; //是否现实D类信息,存入DebugFile文件中。
	  bool bDeepDebug; //是否现实D2类信息
	  bool bDeepDebugToFile; //是否现实D2类信息,存入DebugFile文件中。
	  bool bErrorDebug; //是否现实D2类信息
	  bool bErrorDebugToFile; //是否现实D2类信息,存入DebugFile文件中。
	  string sDebugDir; //Debug文件的存储路径
};
static CDebug *CDebug_Obj= new CDebug("THIS_PROCESS");

#ifdef DEBUG_OUT
#define D(m) CDebug_Obj->show(m)
#define E(m) CDebug_Obj->showError(m)
#define E2(m,n) CDebug_Obj->showError(m,n)
#define E3(m,n,o) CDebug_Obj->showError(m,n,o)
#else
#define D(m)
#define E(m)
#define E2(m,n)
#define E3(m,n,o) 
#endif

#ifdef DEBUG_OBJ
//#define D2(m)  CDebug_Obj->show(m)
#define D2(m,n) CDebug_Obj->show(m,n)
#define D3(m,n,o) CDebug_Obj->show(m,n,o)
#else
//#define D2(m)
#define D2(m,n)
#define D3(m,n,o)
#endif

#endif//CDEBUG_H