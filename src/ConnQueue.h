#ifndef CONN_QUEUE_H_
#define CONN_QUEUE_H_

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
#include "ObjectProcess.h"
#include "ByteBase.h"
#include "STRING_Helper.h"

using namespace std;

class Conn;
class ConnQueue;
typedef bool (*TestFind_T)(Conn *conn, const char *obj_name); //(用于存放para字符串和指针信息。)
//typedef void (*Free)

class Conn
{
friend class ConnQueue;
private:
  const int m_fd;			//socket的ID

  Conn *m_Prev;				//前一个结点的指针
  Conn *m_Next;				//后一个结点的指针

  //Conn(int fd=0,void *arg=0,int protocol_type, const char*protocol_paras=NULL);
  Conn(int fd = 0, void *arg = 0, int protocol_type=0, const char*protocol_paras =0);
  ~Conn();

public:
  int GetFd() { return m_fd; }; 
  bool LoadObjCfg(const char *obj_name);
  void InitProcessObj(const char *obj_name);
  void InitConnParam(const char *obj_name,const char *dest_host);
  void LaterProcessObjAddParams(map<string,string> *tmp_add_params);
  void DoneProcessObjAddParams();
  void SetActive(bool is_act);
  void SetSSLObj(void *ssl_obj);
  void *GetSSLObj();

  //变量
  //ConnSSL *p_obj;	//缺省协议对象
  ObjectProcess *ProcessObj;
  map <string ,string > add_pobj_paras; // tmp add_on PorcessObj paras;
  void *bev;
  void *conn_ssl;
  struct event* time_ev; //time_task

  string obj_name;
  string obj_name_orign;
  string host_addr;
  string ip_addr;
  int ip_port_val;
  string ip_port;
  //解析步骤管理
  //int case_step;
  enum{
    PROTOCAL_TYPE_NULL = 0,
	PROTOCAL_TYPE_SSL = 1,
	CASE_START=1,
	CASE_SKHD=2,
	CASE_NORMAL_STEP=3,
	CASE_NEXT_STEP=4,
	CASE_ROUTING_STEP=5,
	CASE_LOAD_PACKET=6,
	CASE_PROCESS=7,
	CASE_ROUTING_PROCESS=8,
	CASE_END=9,
	CASE_RSEND_STEP=10,
	CASE_ROUTING_ERROR=11,
	CHECK_START_STEP=0,
	CHECK_NEXT_STEP=1,
	MIN_PACKET_LEN =1,
    PACKET_LEN_END = STRING_Helper::NaN-1
  };
  int case_step;
  enum{
	  PACKET_CUT_LEN=1,
	  PACKET_BEGIN_END=2
  };

  //解析包管理
  int packet_type;
  size_t packet_len; 
  string proc_step;
  string packet_end;
  string packet_begin;
  //数据缓存管理
  string buffer;
  string rsend_str; 
  int bufstate;
  //int min_len;
  bool is_act;
  bool close_after_send;
  bool close_after_read;
  map<string, string> case_param; //case 间传参处理
};

class ConnQueue
{
private:
  Conn *m_head;
  Conn *m_tail;

public:
  int ConnNum;
  ConnQueue();
  ~ConnQueue();
  Conn *InsertConn(int fd,void *dev, int protocol_type = 0, const char*protocol_paras = 0);
  void DeleteConn(Conn *c);
  void PrintQueue(void);
  Conn *FindConnByObjName(const char *obj_name,TestFind_T test_func,bool is_act=false);
  int CountConn();
  Conn *FirstConn(bool is_act = false);
  Conn *NextConn(Conn *this_conn, bool is_act = false);
  //Conn *FindConnByObjName(const char *obj_name);
  bool TestFunc(Conn *conn, const char *obj_name);
  static bool TestObjName(Conn *conn, const char *obj_name);
  static bool TestOrignName(Conn *conn, const char *obj_name);
};

class Session
{
public:
	typedef UInt64 Session_T;
	typedef struct {
		UInt64 stime_key;
		string data;
	}SessionData_T;
	typedef map<Session_T, SessionData_T> SessionBuff_T;  //Session实际内容区域
	typedef map<Session_T, Session_T> SessionTime_T; //Session时间索引。
	
	SessionBuff_T SessionBuf;
	SessionTime_T SessionTime;

	int New(string &out_key, string &data, bool out_hex_key=false);
	int Get(string &in_key, string &data, bool in_hex_key=false);
	int OlderDel(int time=0);
	UInt64 _SetTimeKey(Session_T session, SessionData_T *session_data=NULL);
	Session_T _CreateSessionKey(void);
	Session() 
	{
		this->randPlus = 0;
	};
	~Session() {};
private:
	int randPlus;
};

//存储文件队列
//ly queue
/*typedef struct {
	string NodeID;   //数据库节点号
	string ParaBuf;    //参数集（BB_ARRAY）
}TaskQueueCell_T;  //队列元素

typedef std::queue<TaskQueueCell_T> TaskQueue_T;  //单一队列
typedef std::map<BYTE, TaskQueue_T> TaskQueueObj_PriQueue_T; //队列组（按优先级）
typedef std::map<BB_SIZE_T, TaskQueueObj_PriQueue_T> TaskQueueObj_TableIndex_T; //面向表(或路径)的队列集

class TaskQueue{
public:
	TaskQueue();
	~TaskQueue();
	bool PushItem(BB_OBJ *table_obj, BYTE pri_queue_id, TaskQueueCell_T &cell_obj);
	bool PopItem(BB_OBJ *table_obj, BYTE pri_queue_id, TaskQueueCell_T &cell_obj);
	TaskQueueObj_TableIndex_T *TableQueueIndex;    //表格索引数组
    private:

};*/

/*
//有优先顺序的队列 ly queue
typedef struct {
	string NodeID;   //数据库节点号
	BYTE prio;	//优先级
	string ParaBuf;    //参数集（BB_ARRAY）
	ParseXml_Node ActNode;
}PrioQueueCell_T;  //对列元素
bool operator < (const PrioQueueCell_T &t1, PrioQueueCell_T &t2)
{
	return t1.prio < t2.prio; // 按照z 的顺序来决定t1 和t2 的顺序
}

typedef std::priority_queue<PrioQueueCell_T> PrioQueue_T;  //单一队列
typedef std::map<string, PrioQueue_T> PrioQueueObj_TableIndex_T; //面向表(或路径)的队列集
class PrioQueue {
public:
	PrioQueue();
	~PrioQueue();bool PushItem(string &name, BYTE prio, string &node_id_str, string &paras,ParseXml_Node actions_node);
	bool PopItem(string &name, string &node_id_str, string &paras, ParseXml_Node &actions_node);
	PrioQueueObj_TableIndex_T *TableQueueIndex;    //表格索引数组
private:
};*/

//typedef std::priority_queue<TableNodeQueueCell_T> TableNodeQueue_T; 
//bool operator < (const TableNodeQueueCell_T &t1, TableNodeQueueCell_T &t2)
//{
//	return t1._Prio < t2._Prio; // 按照z 的顺序来决定t1 和t2 的顺序
//}

template<typename Data>
class concurrent_queue
{
private:
	int _size;
	struct queue_block
	{
		Data q[0x40];
		unsigned short head, tail;
		queue_block *next;
		queue_block() { head = tail = 0; next = NULL; }
	};
	queue_block *head, *tail;

	//mutable boost::mutex the_mutex;
public:

	concurrent_queue() { head = tail = NULL; }
	~concurrent_queue()
	{
		while (head)
		{
			queue_block *p = head;
			head = head->next;
			delete p;
		}
	}
	void push(const Data& data)
	{
		//boost::mutex::scoped_lock lock(the_mutex);
		if (!head)
			head = tail = new queue_block;
		if (((tail->tail + 1) & 0x3f) == tail->head)
		{
			tail->next = new queue_block;
			tail = tail->next;
		}
		tail->q[tail->tail] = data;
		tail->tail = (tail->tail + 1) & 0x3f;
		_size++;
	}
	bool empty() const
	{
		//boost::mutex::scoped_lock lock(the_mutex);
		return head == NULL;
	}
	Data& front()
	{
		//boost::mutex::scoped_lock lock(the_mutex);
		return head->q[head->head];
	}
	Data const& front() const
	{
		//boost::mutex::scoped_lock lock(the_mutex);
		return head->q[head->head];
	}
	void pop()
	{
		//boost::mutex::scoped_lock lock(the_mutex);
		head->head = (head->head + 1) & 0x3f;
		if (head->head == head->tail)
		{
			queue_block *p = head;
			head = head->next;
			delete p;
		}
		_size--;
	}
	int size()
	{
		//boost::mutex::scoped_lock lock(the_mutex);
		return _size;
	}
};

#endif


