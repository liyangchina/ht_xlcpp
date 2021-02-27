#ifndef THIRD_BASE_H_
#define THIRD_BASE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <map>
#include "ParseObj.h"
#include "Linux_Win.h"

using namespace std;

class ThirdBase
{
private:

public:
	ThirdBase(){
	};
	~ThirdBase(){
	};
	virtual string SendMsg(vector<string> &paras, string &func_return){return "";};
	virtual string RcvMsg(string &func_return){return "";};
	virtual string RcvCallBack(UInt64 id,string receive_data){return "";};
	virtual string Func(vector<string> &paras, string &func_return){return "";};
};
#endif