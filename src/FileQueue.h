#ifndef FILE_QUEUE_H_
#define FILE_QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue> 
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
//#include "ObjectProcess.h"
#include "ByteBase.h"
#include "ParseXml.h"
#include "ParseBase.h"

using namespace std;

//存储文件队列
class FileQueue{
public:
	enum {
		OPEN_MODE_TRUNC = 1,
		OPEN_MODE_NORMAL = 0,
		SAVE_NO_SYNC = 0,
		SAVE_SYNC = 1,
		SAVE_NO_HEAD =2,
		READ_ALL=-1
	};
	typedef struct {
		fstream *f_id;
		BB_SIZE_T length;
		bool changed;
	}FileObj_T;
	typedef map<string, FileObj_T> FQueue_T;  //Session实际内容区域
	typedef FQueue_T::iterator POINT;
	FQueue_T FQueue;
	//this->FQueue.end();

	//加载文件，并写入队列
	int LoadFile(string &file_name, POINT &point, byte open_mode = OPEN_MODE_NORMAL);

	//关闭队列中的文件，并删除队列
	void UnLoadFile(POINT point);
	void UnLoadFile(string &file_name);
	int LoadFileReadAll(string &file_name, void **buf, BB_SIZE_T &length, byte open_mode=OPEN_MODE_NORMAL);
	int LoadFileWrite(string &file_name, const char* buf, BB_SIZE_T &length, BB_SIZE_T start, byte save_flag = SAVE_SYNC, byte open_mode = OPEN_MODE_NORMAL);
	UInt64  GetFileLength(POINT point);
	/*UInt64  GetFileLength(POINT point) {
		return point->second.length;
	}*/
	int WriteFile(string &file_name, const char* buf, BB_SIZE_T &length, BB_SIZE_T start, byte save_flag = SAVE_SYNC);
	int WriteFile(POINT point, const char* buf, BB_SIZE_T &length, BB_SIZE_T start, byte save_flag = SAVE_SYNC);
	int ReadFile(string &file_name, void **buf, BB_SIZE_T &length, BB_SIZE_T start);
	int ReadFile(POINT point, void **buf, BB_SIZE_T &length, BB_SIZE_T start);
	bool Commit(string file_name,bool debug=false);
	bool Commit(POINT point,bool debug=false);
	bool CommitAll(bool debug=false);
	
	~FileQueue();

private:
	
};

//单一队列对象 ly queue
typedef struct {
	string CellID;   //数据库节点号
	BYTE _Prio;	//优先级
	string PkgBuf;    //参数集（BB_ARRAY）
	ParseXml_Node ActNode;
}TableNodeQueueCell_T;  //对列元素

typedef queue<TableNodeQueueCell_T> TableNodeQueue_T;

class TableNodeQueue {
public:
	TableNodeQueue();
	~TableNodeQueue();
	bool PushItem(BYTE prio, string &node_id_str, string &pkg, ParseXml_Node actions_node);
	bool PopItem(string &node_id_str, string &pkg, ParseXml_Node &actions_node);
	BB_SIZE_T GetMemSize();
	BB_SIZE_T GetSize();
	TableNodeQueue_T *TableNodeQueueIndex;    //表格索引数组
private:
};


/*//单一队列对象 ly queue
typedef struct {
	//发送相关
	const char *dest;
	const char *src;
	int trans_type;
	bool close_after_send;
	void *trans_handle;
	ParseProcess *parentObj;
}SendAddParas_T;  //对列元素*/


/*typedef struct {
	string CellID;   //数据库节点号
	BYTE _Prio;	//优先级
    string PkgBuf;    //参数集（BB_ARRAY）
	//发送相关

	ParseXml_Node ActNode;
	ParseBase *srcObj;
	int trans_type;
	bool close_after_send;
	int time_out;
	int time_start;
}SendNodeQueueCell_T;  //对列元素*/

//ly_send_queue

typedef struct {
	string CellID;   //数据库节点号
	BYTE _Prio;	//优先级
	string PkgBuf;    //参数集（BB_ARRAY）
	//发送相关
	ParseXml_Node ActNode;
	ParseBase *srcObj;
	string src_conn_name;
	string dest_conn_name;
	int trans_type;
	bool close_after_send;
	int time_out;
	UInt64 time_start;
	string time_out_path;
	const char *rstate_field;
    unsigned char send_state;
}SendNodeQueueCell_T;  //对列元素

typedef queue<SendNodeQueueCell_T> SendNodeQueue_T;

//ly_send_queue
class SendNodeQueue {
public:
    enum {
		CONNECTING=0,
        WAITTING = 3,
        SENDED = 4,
		CANCLED=5,
        RECEIVED = 6
    };
	SendNodeQueue();
	~SendNodeQueue();
	bool PushItem(SendNodeQueueCell_T &in_send_queue);
	bool PopItem(SendNodeQueueCell_T &out_send_queue);
	BB_SIZE_T GetMemSize();
	BB_SIZE_T GetSize();
	SendNodeQueue_T *SendNodeQueueIndex;    //表格索引数组
private:
};


class FileStream{
public:

    bool IsOpen;
    fstream ft;
    //long Length;
    FileStream(string filename,byte open_mode=0){
        if (open_mode==0)
            ft.open(filename.c_str(),ios::in|ios::out|ios::binary);
        else if (open_mode==FileQueue::OPEN_MODE_TRUNC)
            ft.open(filename.c_str(),ios::in|ios::out|ios::binary|ios::trunc);
        else
            ft.open(filename.c_str(),ios::in|ios::out|ios::binary);

        if (!ft){// if not exit,create
            std::ofstream ft1(filename.c_str(),ios::binary);
            ft1.close();
            ft.open(filename.c_str(),ios::in|ios::out);
        }
        if (ft){
            ft.seekg (0, ios::beg); //cursor reset to begin
            this->IsOpen = true;
        }else{
            this->IsOpen = false;
        };
    };
    std::streamsize Read(byte *buf, uint64_t from, long length){
        if (IsOpen){
            this->ft.seekg (from);
            this->ft.read((char *)buf, length);
            return this->ft.gcount();
        }else{
            return -1;
        }
    };

    bool Write(string &buf,uint64_t from,long length){
        if ((size_t)length >buf.size())
            return this->Write((byte *)buf.data(),from,buf.size());
        else
            return this->Write((byte *)buf.data(),from,length);
    };
    long Length(){
        this->ft.seekg (0, ios::end);
        return(long)this->ft.tellg();// get file length;
    };
    bool Write(byte *buf, uint64_t from, long length){
        if (IsOpen){
            this->ft.seekg (from);
            this->ft.write((char *)buf, length);
            return true;
        }else
            return false;
    }
    void Close(){
        this->ft.close();
    }

};

#endif
