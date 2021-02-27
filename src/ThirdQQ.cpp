#include <iostream>
#include "STRING_Helper.h"
#include "WizBase.h"
#include "ParseObj.h"
#include "ParseExternal.h"
#include "sha1.h"
#include "cws.h"
#include "base64.h"
#include "Linux_Win.h"
#include "CChineseCode.h"
#include "ThirdQQ.h"
#include <string>
using namespace std;

string ThirdQQ::SendMsg(vector<string> &paras, string &func_return){
	//ThirdQQ::send_data(paras, func_return);
#ifndef OPEN_WRT

#else
	string method=paras[0];//取得SendMethod
	paras.erase(paras.begin());

	if (method=="SendData"){
		//装填data_point数据组
		//tx_data_point points[2] = {
		//	{10101, (char *)"10%", 0, 0},   //上传剩余电量值
		//	{100000770,(char *) "29℃" , 0, 0},   //上传温度数值;seq 在发送时是不需要填写的， ret_code 也不需要填写
		//};

		//装填data_point数据组
		tx_data_point points[2];
		for(int i=0; i<2; i++){
			vector<string> data_para;
			STRING_Helper::stringSplit(paras[i],data_para,":");
			//data_para[1].append(0);
			points[i].id=STRING_Helper::StrToi(data_para[0]);	
			//参数赋值
			strcpy(this->paraBuf[i],data_para[1].c_str());
			points[i].value=(this->paraBuf[i]);	
			points[i].ret_code=0;
			points[i].seq=0;
			printf("send points: %d:%s\n",points[i].id,points[i].value);
		}
		
		//发送操作
		unsigned int cookie = 0;
		printf("send process: \n");
		tx_report_data_points(points,2,&cookie, ThirdQQ::ret_callback);
		printf("send process end!\n");
	}
#endif

	func_return="success!";
	return "";
};
string ThirdQQ::RcvMsg(string &func_return){
	//cout <<"RcvMsg:" << this->rcvMsg << endl;
	string tmp2;
	if (this->rcvMsg.size()!=0){
		const char tmp=STRING_Helper::ParseInt(this->rcvMsg);
		string tmp2(&tmp,1);
		func_return=STRING_Helper::iToStr((unsigned int)this->rcvMsgId)+":"+STRING_Helper::HBCStrToHBCStr(tmp2,"char","hex",false);
	}else{
		func_return=STRING_Helper::iToStr((unsigned int)this->rcvMsgId)+":"+"";
	}
	this->rcvMsg="";
	return "";
}
string ThirdQQ::RcvCallBack(UInt64 id,string receive_data){
	this->rcvMsg=receive_data;
	this->rcvMsgId=id;
	printf("Receive %llu:%s",id,receive_data.c_str());
	return "";
}
string ThirdQQ::Func(vector<string> &paras, string &func_return){
	cout <<"FuncMsg:" << paras[0] << "  " <<paras[1]<< endl;
	return "";
}
#ifndef OPEN_WRT

#else

	void ThirdQQ::on_query_subdevicelist(unsigned long long from_client, tx_data_point data_points){
		//
		printf("这是一个控制指令\n");

		//将设备列表信息转化为json字符串
		char* json_value = data_points.value;
		//char* json_value ="0" ;
		printf("========receive_data_complete===============\n");
		printf("value=%s\n",json_value);
		GB_ThirdMap["QQ"]->RcvCallBack(data_points.id,json_value);
		printf("============================================\n");
		//处理结果回传到手机上(注意seq的匹配,不要随便乱填)
		tx_ack_data_point(from_client, data_points.id, json_value, data_points.seq, data_points.ret_code, 0, 0);
	}

	//设备处理关心的指令
	void ThirdQQ::myreceivedata(unsigned long long from_client,tx_data_point * data_points, int data_points_count)
	{
		//Éè±žÉÏŽ«ÊýŸÝ×éÍê³Éºó»Øµ÷µÄÍšÖª
		/*printf("========receive_data_complete===============\n");
		printf("client=%d\n",from_client);
		printf("id=%d\n",data_points.id);
		printf("value=%s\n",data_points.value);
		printf("seq=%d\n",data_points.seq);
		printf("ret_code=%d\n",data_points.ret_code);
		printf("============================================\n");*/
	
		int i = 0;
		on_query_subdevicelist(from_client, data_points[i]);
	}

	void ThirdQQ::receive_data()
	{
		//挂接datapoint的处理函数
		tx_data_point_notify datanotify = {0};
		datanotify.on_receive_data_point = ThirdQQ::myreceivedata;
		tx_init_data_point(&datanotify);

	}

	void ThirdQQ::mysendmsg(const unsigned int cookie, int err_code)
	{
		printf("===========sendmsg========================\n");
		printf("cookie=%d\n", cookie);
		printf("err_code= %d\n",err_code);
		printf("===========================================\n");
    
	}

	/*
	 * 发送图文报警，
	 */
	void ThirdQQ::send_msg()
	{
		// 发送图片的结构化消息
		structuring_msg msg = {0};
		msg.msg_id = 1093;   //id值需与QQ物联官网上配置触发器值一样才能触发
		msg.file_path = (char *)"/www/QQ/message.txt";
		msg.thumb_path = (char *)"/www/QQ/picture.jpg";
		msg.title = (char *)"发现异常";
		msg.digest = (char *)"设备发现异常情况发现异常";
		msg.guide_words = (char *)"点击查看";
		//msg.to_targetids	="1719952316";
		//msg.to_targetids_count	=0;
		//设置回调函数
		tx_send_msg_notify notify = {0};
		notify.on_file_transfer_progress = 0;
		notify.on_send_structuring_msg_ret = ThirdQQ::mysendmsg;
		//发送操作
		tx_send_structuring_msg(&msg, &notify, 0);
	}

	/*
	 * 发送语音报警，
	 */
	void ThirdQQ::send_audio()
	{
		// 发送语音的结构化消息
		structuring_msg msg = {0};
		msg.msg_id = 2;   //id值需与QQ物联官网上配置触发器值一样才能触发
		msg.file_path = (char *)"/www/QQ/test.amr";
		msg.duration		=4;	//设备发送语音的时间长度
		msg.title = (char *)"语音报警";
		msg.digest = (char *)"收到语音报警";
		msg.guide_words = (char *)"点击查看";
		//msg.to_targetids	="1719952316";	//制定给某个绑定这发送
		//msg.to_targetids_count	=0;
		//设置回调函数
		tx_send_msg_notify notify = {0};
		notify.on_file_transfer_progress = 0;
		notify.on_send_structuring_msg_ret = ThirdQQ::mysendmsg;
		//发送操作
		tx_send_structuring_msg(&msg, &notify, 0);
	}

	/*
	 * 发送纯文字报警，
	 */
	void ThirdQQ::send_text()
	{
		/*tx_send_text_msg msg = {0};
		//msg.on_send_text_msg_ret = ThirdQQ::mysendmsg;
		msg.msg_id		=4;
		msg.text		="设备发现有异常！";
		msg.ret_callback	=NULL;
		msg.cookie		=NULL;
		//msg.to_targetids	=NULL;
		//msg.to_targetids_count	=0;*/
		//设置回调函数
		tx_send_msg_notify notify = {0};
    		notify.on_file_transfer_progress = 0;
   		notify.on_send_structuring_msg_ret = ThirdQQ::mysendmsg;
		//发送纯文字消息操作
		tx_send_text_msg(4,(char *)"设备发现有异常",0,0,0,0);
		//发送操作
		//tx_send_structuring_msg(&msg, &notify, 0);
	}

	//设备上传数据组后的回调通知
	void ThirdQQ::ret_callback(unsigned int cookie,int err_code)
	{
		printf("===========send_data========================\n");
		printf("cookie= %d\n",cookie);
		printf("err_code= %d\n",err_code);
		printf("===========================================\n");
	}

	void ThirdQQ::send_data(vector<string> &paras, string &func_return)
	{
		string method=paras[0];//取得SendMethod
		paras.erase(paras.begin());

		//if (method=="SendData"){
		//	//装填data_point数据组
		//	//tx_data_point points[2] = {
		//	//	{10101, (char *)"10%", 0, 0},   //上传剩余电量值
		//	//	{100000770,(char *) "29℃" , 0, 0},   //上传温度数值;seq 在发送时是不需要填写的， ret_code 也不需要填写
		//	//};

		//	//装填data_point数据组
		//	tx_data_point points[2];
		//	for(int i=0; i<2; i++){
		//		vector<string> data_para;
		//		STRING_Helper::stringSplit(paras[i],data_para,":");
		//		//data_para[1].append(0);
		//		points[i].id=STRING_Helper::StrToi(data_para[0]);	
		//		//参数赋值
		//		strcpy(this->paraBuf[i],data_para[1].c_str());
		//		points[i].value=(this->paraBuf[i]);	
		//		points[i].ret_code=0;
		//		points[i].seq=0;
		//		printf("send points: %d:%s\n",points[i].id,points[i].value);
		//	}
		//
		//	//发送操作
		//	unsigned int cookie = 0;
		//	printf("send process: \n");
		//	tx_report_data_points(points,2,&cookie, ThirdQQ::ret_callback);
		//	printf("send process end!\n");
		//}
	}

	//===============================================================//
	/**
	 * 登录完成的通知，errcode为0表示登录成功，其余请参考全局的错误码表
	 */
	void ThirdQQ::on_login_complete(int errcode) {
		printf("on_login_complete | code[%d]\n", errcode);
	}

	/**
	 * 在线状态变化通知， 状态（status）取值为 11 表示 在线， 取值为 21 表示  离线
	 * old是前一个状态，new是变化后的状态（当前）
	 */
	void ThirdQQ::on_online_status(int old, int new1) {
		printf("online status: %s\n", 11 == new1 ? "true" : "false"); 
	}
	/**
	 * 文件传输完成后的结果通知
	 */
	void ThirdQQ::ontransfercomplete(unsigned long long transfer_cookie, int err_code, tx_file_transfer_info *tran_info) {
		printf("================ontransfer complete=====transfer_cookie == %lld ====================\n", transfer_cookie);
		// 这个 transfer_cookie 是SDK内部执行文件传输（接收或发送） 任务 保存的一个标记，在回调完这个函数后，transfer_cookie 将失效
		// 可以根据 transfer_cookie 查询文件的信息

		//在完成回调里直接传回 tran_info
		// tx_file_transfer_info ftInfo = {0};
		// tx_query_transfer_info(transfer_cookie, &ftInfo);

		printf("errcode %d, bussiness_name [%s], file path [%s]\n", err_code, tran_info->bussiness_name,  tran_info->file_path);
		printf("===============================================================================\n");
	}

	/**
	 * 辅助函数: 从文件读取buffer
	 * 这里用于读取 license 和 guid
	 * 这样做的好处是不用频繁修改代码就可以更新license和guid
	 */
	bool ThirdQQ::readBufferFromFile(const char *pPath, unsigned char *pBuffer, int nInSize, int *pSizeUsed) {
		if (!pPath || !pBuffer) {
			return false;
		}

		int uLen = 0;
		FILE * file = fopen(pPath, "rb");
		if (!file) {
			return false;
		}

		fseek(file, 0L, SEEK_END);
		uLen = ftell(file);
		fseek(file, 0L, SEEK_SET);

		if (0 == uLen || nInSize < uLen) {
			printf("invalide file or buffer size is too small...\n");
			return false;
		}

		*pSizeUsed = fread(pBuffer, 1, uLen, file);
		// bugfix: 0x0a is a lineend char, no use.
		if (pBuffer[uLen-1] == 0x0a)
		{
			*pSizeUsed = uLen - 1;
			pBuffer[uLen-1] = '\0';
		}

		printf("len:%d, ulen:%d\n",uLen, *pSizeUsed);
		fclose(file);
		return true;
	}

	/**
	 * 辅助函数：SDK的log输出回调
	 * SDK内部调用改log输出函数，有助于开发者调试程序
	 */
	void ThirdQQ::log_func(int level, const char* module, int line, const char* message)
	{
		printf("%s\n", message);
	}

	/**
	 * SDK初始化
	 * 例如：
	 * （1）填写设备基本信息
	 * （2）打算监听哪些事件，事件监听的原理实际上就是设置各类消息的回调函数，
	 * 	例如设置CC消息通知回调：
	 * 	开发者应该定义如下的 my_on_receive_ccmsg 函数，将其赋值tx_msg_notify对象中对应的函数指针，并初始化：
	 *
	 * 			tx_msg_notify msgNotify = {0};
	 * 			msgNotify.on_receive_ccmsg = my_on_receive_ccmsg;
	 * 			tx_init_msg(&msgNotify);
	 *
	 * 	那么当SDK内部的一个线程收到对方发过来的CC消息后（通过服务器转发），将同步调用 msgNotify.on_receive_ccmsg 
	 */
	bool ThirdQQ::initDevice(const char *license_str , const char *license_file) {
		// 读取 license
		unsigned char license[256] = {0};
		int nLicenseSize = 0;
		if (!readBufferFromFile("/www/QQ/licence/licence.sign.file.txt", license, sizeof(license), &nLicenseSize)) {
			printf("[error]get license from file failed...\n");
			return false;
		}

		// 读取guid
		unsigned char guid[32] = {0};
		int nGUIDSize = 0;
		if(!readBufferFromFile("/www/QQ/licence/GUID_file.txt", guid, sizeof(guid), &nGUIDSize)) {
			printf("[error]get guid from file failed...\n");
			return false;
		}

		unsigned char svrPubkey[256] = {0};
		int nPubkeySize = 0;
		if (!readBufferFromFile(license_file==0 ? "/www/QQ/licence/1700002397.pem" : license_file, svrPubkey, sizeof(svrPubkey), &nPubkeySize))
		{
			printf("[error]get svrPubkey from file failed,file_name is :%s...\n",license_file==0 ? "/www/QQ/licence/1700002397.pem" : license_file);
			return NULL;
		}

		// 设备的基本信息
		unsigned int product_id=1700002397;

		tx_device_info info = {0};
		info.os_platform            =(char*) "Linux";

		info.device_name            = (char*)"demo1";
		info.device_serial_number   = (char *)guid;
		info.device_license         = (char *)license;
		info.product_version        = 1;
		info.network_type			= network_type_wifi;
		if (license_str!=0){
			printf(">>input license: %s",license_str);
			product_id=(unsigned int)atoi(license_str);
			info.product_id         = product_id;	
		}
			
		info.server_pub_key         = (char *)svrPubkey;

		// 设备登录、在线状态、消息等相关的事件通知
		// 注意事项：
		// 如下的这些notify回调函数，都是来自硬件SDK内部的一个线程，所以在这些回调函数内部的代码一定要注意线程安全问题
		// 比如在on_login_complete操作某个全局变量时，一定要考虑是不是您自己的线程也有可能操作这个变量
		tx_device_notify notify      = {0};
		notify.on_login_complete     = on_login_complete;
		notify.on_online_status      = on_online_status;
		notify.on_binder_list_change = NULL;

		// SDK初始化目录，写入配置、Log输出等信息
		// 为了了解设备的运行状况，存在上传异常错误日志 到 服务器的必要
		// system_path：SDK会在该目录下写入保证正常运行必需的配置信息
		// system_path_capicity：是允许SDK在该目录下最多写入多少字节的数据（最小大小：10K，建议大小：100K）
		// app_path：用于保存运行中产生的log或者crash堆栈
		// app_path_capicity：同上，（最小大小：300K，建议大小：1M）
		// temp_path：可能会在该目录下写入临时文件
		// temp_path_capicity：这个参数实际没有用的，可以忽略
		tx_init_path init_path = {0};
		init_path.system_path = (char *)"/www/QQ/";
		init_path.system_path_capicity = 100 * 1024;
		init_path.app_path = (char *)"/www/QQ/";
		init_path.app_path_capicity = 1024 * 1024;
		init_path.temp_path = (char *)"/www/QQ/";
		init_path.temp_path_capicity = 10 * 1024;

		// 设置log输出函数，如果不想打印log，则无需设置。
		// 建议开发在开发调试阶段开启log，在产品发布的时候禁用log。
		tx_set_log_func(log_func);

		// 初始化SDK，若初始化成功，则内部会启动一个线程去执行相关逻辑，该线程会持续运行，直到收到 exit 调用
		int ret = tx_init_device(&info, &notify, &init_path);
		if (err_null == ret) {
			printf(" >>> tx_init_device success\n");
		}
		else {
			printf(" >>> tx_init_device failed [%d]\n", ret);
			return false;
		}
	/********************************************************
	*	tx_msg_notify msgNotify = {0};
	*	msgNotify.on_receive_ccmsg = my_on_receive_ccmsg;
	*	tx_init_msg(&msgNotify);
	*******************************************************/
	/*
	tx_send_text_msg = {0};
		on_send_text_msg_ret = mysendmsg;
		msg_id		=1093;
		text		="你好!";
		ret_callback	=NULL;
		cookie		=NULL;
		o_targetids	=NULL;
		to_targetids_count	=0;
	*/	

		// 设置文件传输通知，并设置接收文件的目录，这里设置为 ./recv/
		tx_file_transfer_notify fileTransferNotify = {0};
		fileTransferNotify.on_transfer_complete = ontransfercomplete;
		tx_init_file_transfer(fileTransferNotify, "/www/QQ/recv/");

		return true;
	}

	/****************************************************************
	*  测试代码：
	*
	*  （1）while循环的作用仅仅是使 Demo进程不会退出，实际使用SDK时一般不需要
	*
	*  （2） 输入 "quit" 将会退出当前进程，这段逻辑存在的原因在于：
	*     					在某些芯片上，直接用Ctrl+C 退出易产生僵尸进程
	*
	*  （3）while循环里面的sleep(1)在这里是必须的，因为如果Demo进程后台运行，scanf没有阻塞作用，会导致当前线程跑满CPU
	*
	*****************************************************************/

	int ThirdQQ::Run(int argc ,char ** argv){
		char *license_file_name=0;
		char *license=0;

		if (argc>=5){
			license_file_name=argv[5];
		}
		if (argc>=4){
			license=argv[4];
		}
		if ( !initDevice((const char*)license,(const char *)license_file_name) ) {
			return -1;
		}
	
		// 你可以在做其他相关的事情

		// 调用设备接受指令函数
		ThirdQQ::receive_data();

		return 0;
	}

#endif



