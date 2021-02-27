/* -*- c++ -*- */
// Hey, Emacs!  This is a C++ file!
// HTTP_Config.h,v 1.7 1999/07/06 01:39:29 shuston Exp

// ============================================================================
//
// = LIBRARY
//   jaws
//
// = FILENAME
//    HTTP_Config.h
//
// = AUTHOR
//    James Hu
//
// ============================================================================

// = Forward declaration.
#pragma once

#ifndef HTTP_CONFIG
#define HTTP_CONFIG

#include <vector>
#include <map>
#include <string>
#include "Linux_Win.h"
#ifdef SENSOR_DBM
	#include "SensorDBManager.h"
#endif
#include "ParseObj.h"

#define CONFIGXML "./jawsconfig.ini"

typedef std::map<std::string,std::map<std::string,std::string>  >  _iniInfo;

class JawsConfig
{
public:
	JawsConfig(const char * filename);
	void getValue(std::string AppName, std::string KeyName, std::string defaultValue,std::string & result);
	int  getValue(std::string AppName, std::string KeyName, int defaultValue);
	void getValue(std::string AppName, std::map<std::string, std::string> & KeyApp);
	void test();
	bool isLoadSuccess;
private:
	bool readfileinfo(const char *filename);
	bool parseIni(std::string  strconfig); 
	int parseLine (std::string strLine,std::string & restr1,std::string & restr2); 
	
private:
	_iniInfo m_ConfigInfo_;

};

class HTTP_Config
{
public:
  static JawsConfig *jawscfg;
#ifdef SENSOR_DBM
  static SensorDBManager *SensorDB;
#endif
#ifdef JAWS_CDBM
  static CDBManager *DBxml;
#endif

static Http_ParseObj *SysPubObj;

private:

};

#endif

