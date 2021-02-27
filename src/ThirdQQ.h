#ifndef THIRD_QQ_H_
#define THIRD_QQ_H_

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
//QQ
#include "ThirdBase.h"
#ifdef THIRD_QQ
	#include "TXSDKCommonDef.h"
	#include "TXDeviceSDK.h"
	#include "TXDataPoint.h"
	#define THIRD_QQ_MAX_RCV_ITEM 2
#endif

using namespace std;
class ThirdQQ: public ThirdBase{
public:
	ThirdQQ(){
		this->rcvMsg="";
		this->rcvMsgId=0;
	}
	~ThirdQQ(){
	};
	virtual string SendMsg(vector<string> &paras, string &func_return);
	virtual string RcvMsg(string &func_return);
	virtual string RcvCallBack(UInt64 id,string receive_data); 
	virtual string Func(vector<string> &paras, string &func_return);

#ifndef OPEN_WRT
	//ALL RECEIVE FUNC
	//static void on_query_subdevicelist(unsigned long long from_client, tx_data_point data_points){};
	//static void myreceivedata(unsigned long long from_client,tx_data_point * data_points, int data_points_count){};
	//static void receive_data(){};
	//ALL SEND FUNC
	static void mysendmsg(const unsigned int cookie, int err_code){};//消息发送完成后的回调通知
	static void send_msg(){};	//发送图文报警
	static void send_text(){};//发送纯文字报警
	static void send_audio(){};	//发送语音报警
	static void send_data(vector<string> &paras, string &func_return){};	//发送datapoint
	static void ret_callback(unsigned int cookie,int err_code){};
	//ALL RUN FUNC
	/*static void on_login_complete(int errcode){};
	static void on_online_status(int old, int new1){};
	static void ontransfercomplete(unsigned long long transfer_cookie, int err_code, tx_file_transfer_info *tran_info){};
	static bool readBufferFromFile(const char *pPath, unsigned char *pBuffer, int nInSize, int *pSizeUsed){return true;};
	static void log_func(int level, const char* module, int line, const char* message){};
	static bool initDevice(){return true;};*/
	static int QQ(void){return 0;};
#else
	//ALL RECEIVE FUNC
	static void on_query_subdevicelist(unsigned long long from_client, tx_data_point data_points);
	static void myreceivedata(unsigned long long from_client,tx_data_point * data_points, int data_points_count);
	static void receive_data();
	//ALL SEND FUNC
	static void mysendmsg(const unsigned int cookie, int err_code);//消息发送完成后的回调通知
	static void send_msg();	//发送图文报警
	static void send_text();//发送纯文字报警
	static void send_audio();	//发送语音报警
	static void send_data(vector<string> &paras, string &func_return);	//发送datapoint
	static void ret_callback(unsigned int cookie,int err_code);
	//ALL RUN FUNC
	static void on_login_complete(int errcode);
	static void on_online_status(int old, int new1);
	static void ontransfercomplete(unsigned long long transfer_cookie, int err_code, tx_file_transfer_info *tran_info);
	static bool readBufferFromFile(const char *pPath, unsigned char *pBuffer, int nInSize, int *pSizeUsed);
	static void log_func(int level, const char* module, int line, const char* message);
	static bool initDevice( const char *license_str , const char *license_file);
	static int Run(int argc, char **argv);
#endif
	//
	string sendMsg;
	string rcvMsg;
	UInt64 rcvMsgId;
	//
	char paraBuf[10][30];
};
extern map<string, ThirdBase*> GB_ThirdMap;

#endif