#include "ConnQueue.h"
#include "ObjectProcess.h"
#include <iostream>
#include "STRING_Helper.h"
#include "WizBase.h"
//#include "ConnSSL.h"

Conn::Conn(int fd,void *arg,int protocol_type,const char*protocol_paras):m_fd(fd)
{
  //Init Obj and Load ObjConfig
	this->ProcessObj= new ObjectProcess();
	this->bev=arg;
	this->obj_name="";
	this->conn_ssl=NULL;
	//this->m_fd=fd //see m_fd(fd)

  //Init Queue
	m_Prev = NULL;
	m_Next = NULL;
  // Init State\Buffer
	this->case_step=CASE_START;
	this->packet_type=PACKET_CUT_LEN;
	this->buffer="";
	this->rsend_str="";
	this->close_after_send=false;
	this->close_after_read=false;
    this->time_ev=NULL; //time_task
}

void Conn::InitConnParam(const char *obj_name,const char *dest_host){
	//初始化参数
	if (dest_host!=0){
		string dest_ip_str(dest_host);
		this->ip_addr=dest_ip_str.substr(0,dest_ip_str.find(":"));
		this->ip_port=dest_ip_str.substr(dest_ip_str.find(":")+1);
		this->ip_port_val = STRING_Helper::StrToi(this->ip_port);
		this->host_addr=dest_host;
	}
	if (obj_name!=0){
		this->obj_name=obj_name;
		this->obj_name_orign=obj_name;
	}
}

void Conn::InitProcessObj(const char *obj_name){
	this->ProcessObj->InitObjectParam();
	this->ProcessObj->InitProcess("","");
	this->ProcessObj->SetParam("@_DEST_IP",this->ip_addr.c_str(),Http_ParseObj::STR_COPY_OBJ);
	this->ProcessObj->SetParam("@_DEST_PORT",this->ip_port.c_str(),Http_ParseObj::STR_COPY_OBJ);
	this->ProcessObj->SetParam("@_DEST_HOST",this->host_addr.c_str(),Http_ParseObj::STR_COPY_OBJ);
	this->ProcessObj->SetParam("@_OBJ_NAME",obj_name,Http_ParseObj::STR_COPY_OBJ);
	this->ProcessObj->SetParam("@_FD","1",Http_ParseObj::STR_COPY_OBJ);//初始化相关req值。
	
	//this->obj_name_orign=obj_name; //原ly thread
	if (strlen(obj_name) == 0)
		this->obj_name_orign = "";
	//later add_paras
	this->DoneProcessObjAddParams();
}

void Conn::DoneProcessObjAddParams(){
	if (this->add_pobj_paras.size()!=0){
		for (map<string, string> ::iterator it = this->add_pobj_paras.begin(); it != add_pobj_paras.end(); ++it)
			this->ProcessObj->SetParam(it->first.c_str(),it->second,Http_ParseObj::STR_COPY_OBJ);
	}
	this->add_pobj_paras.clear();  //ly crash 可以用swap代替清空。
}

void Conn::LaterProcessObjAddParams(map<string,string> *tmp_add_paras){
	//save to ->add_pobj_paras;
	this->add_pobj_paras.clear(); //ly crash 可以用swap代替清空。
	if (tmp_add_paras!=NULL &&tmp_add_paras->size()!=0)
		for (map<string, string> ::iterator it = tmp_add_paras->begin(); it != tmp_add_paras->end(); ++it)
			this->add_pobj_paras[it->first.c_str()]=it->second;
}

void Conn::SetActive(bool act){
	this->is_act=act;
}

void Conn::SetSSLObj(void *ssl_obj)
{
	this->conn_ssl = ssl_obj;
}

void *Conn::GetSSLObj()
{
	return (this->conn_ssl);
}

bool Conn::LoadObjCfg(const char *obj_name){
	this->obj_name=obj_name;

	string obj_file=obj_name;
	obj_file+= ".xml";
	cout << (string)"Loading:"+obj_file;
	if (!(ProcessObj->LoadObjectFile(obj_file.c_str()))){
		std::cout << "---LoadObject  Error:"  << ProcessObj->ErrorStr << endl;
		return false;
	}else{
		std::cout << "---Load ProcessObj Success !"<< endl;
		return true;
	};
	return true;
}

Conn::~Conn()
{
	//删除GB_Object
	if (this->ProcessObj!=NULL)
		delete this->ProcessObj;
    
    if (this->time_ev!=NULL)
        WIZBASE_CancleQueueTimeTask(this);

	//if (this->bev!=NULL)
		//bufferevent_free(this->bev);

	//if (this->conn_ssl !=NULL)
		//delete this->conn_ssl; //ly memleak2 old there

	//删除协议对象
	//if (p_obj != NULL)
		//delete this->p_obj;
}

ConnQueue::ConnQueue()
{
  this->ConnNum = 0;
  //建立头尾结点，并调整其指针
  m_head = new Conn(0);
  m_tail = new Conn(0);
  m_head->m_Prev = m_tail->m_Next = NULL;
  m_head->m_Next = m_tail;
  m_tail->m_Prev = m_head;
}

ConnQueue::~ConnQueue()
{
  Conn *tcur, *tnext;
  tcur = m_head;
  //循环删除链表中的各个结点
  while( tcur != NULL )
  {
    tnext = tcur->m_Next;
    delete tcur;
    tcur = tnext;
  }
}

Conn *ConnQueue::InsertConn(int fd,void *bev, int protocol_type, const char*protocol_paras)
{
  Conn *c = new Conn(fd,bev, protocol_type, protocol_paras);
  Conn *next = this->m_head->m_Next;

  c->m_Prev = this->m_head;
  c->m_Next = this->m_head->m_Next;
  this->m_head->m_Next = c;
  next->m_Prev = c;
  this->ConnNum++;
  return c;
}

void ConnQueue::DeleteConn(Conn *c)
{
	try {
		c->m_Prev->m_Next = c->m_Next;
		c->m_Next->m_Prev = c->m_Prev;
		delete c;
        this->ConnNum--;
	}
	catch (double)                                    //捕获并处理异常 
	{
		cout << "DeleteConn(conn) error! conn may be already relased! .\n";
	}
}
  
void ConnQueue::PrintQueue()
{
  Conn *cur = m_head->m_Next;
  while( cur->m_Next != NULL )
  {
    printf("%d,%s", cur->m_fd,cur->obj_name.c_str());
    cur = cur->m_Next;
  }
  printf("\n");
}

/*Conn *ConnQueue::FindConnByObjName(const char *obj_name){
	return this->FindConnByObjName(obj_name,0);
 }*/

Conn *ConnQueue::FindConnByObjName(const char *obj_name,TestFind_T test_func,bool is_act){
	Conn *conn=this->m_head;
	while(1){
		conn=conn->m_Next;
		if(conn==this->m_tail)
			break;
		else if ((test_func!=0) && test_func(conn,obj_name) && (is_act || conn->is_act))
			return conn;
		else if ((test_func==0) && this->TestFunc(conn,obj_name) && (is_act || conn->is_act))
			return conn;
	}
	return NULL;
 }

int ConnQueue::CountConn(){
    int i=0;
    Conn *conn=this->m_head;
    while(1){
        conn=conn->m_Next;
        if(conn==this->m_tail)
            break;
        i++;
    }
    return i;
}

Conn *ConnQueue::FirstConn(bool is_act) {
	Conn *conn = this->m_head;
	if (conn != this->m_tail && (is_act || conn->is_act))
		return conn;
	else
		return NULL;
}

Conn *ConnQueue::NextConn(Conn *this_conn, bool is_act) {
	Conn *conn = this_conn->m_Next;
	if (conn != this->m_tail && (is_act || conn->is_act))
		return conn;
	else
		return NULL;
}


bool ConnQueue::TestFunc(Conn *conn, const char *obj_name){
	if (conn->obj_name==obj_name)
		return true;
	else
		return false;
}

bool ConnQueue::TestObjName(Conn *conn, const char *obj_name) {
	if (conn->obj_name == obj_name)
		return true;
	else
		return false;
}

bool ConnQueue::TestOrignName(Conn *conn, const char *obj_name){
	if (conn->obj_name_orign==obj_name)
		return true;
	else
		return false;
}

int Session::New(string &out_key, string &data, bool out_key_hex) {
	//取得session随机数
	Session_T session_id = this->_CreateSessionKey();//创建session_id
	Session_T stime_key=this->_SetTimeKey(session_id);//创建SessionTime;

	//创建SessionTimeBuf
	SessionData_T sdata;
	sdata.data = data;
	sdata.stime_key = stime_key;
	this->SessionBuf[session_id] = sdata;

	//返回key
	STRING_Helper::putUINTToStr(out_key, (UInt64)session_id);
	if (out_key_hex)
		out_key =  STRING_Helper::HBCStrToHBCStr(out_key, "char", "hex", false);

	return 0; //今后加入测试是否有重复key。
};

int Session::Get(string &in_key, string &data, bool in_key_hex) {
	Session_T session_id;
	if (in_key_hex) {
		//string tmp1 = STRING_Helper::HBCStrToHBCStr(in_key, "char", "hex", true);
		//if (tmp1.size() != 8 || !STRING_Helper::CheckNum(tmp1))
        string tmp1 = STRING_Helper::HBCStrToHBCStr(in_key, "hex", "char", true);
        if (tmp1.size() != 8)
			return -2;	//"Session::Get(string &in_key) parameter error :  in_key should be UInt64 binary!";
		session_id = STRING_Helper::scanUINTStr(tmp1);
	}
	else {
		if (in_key.size() != 8)
			return -2;	//"Session::Get(string &in_key) parameter error :  in_key should be uint64 string!" << endl;
		session_id = STRING_Helper::scanUINTStr(in_key);
	}
	 
	//查询SessionBuf
	SessionBuff_T::iterator point = this->SessionBuf.find(session_id);
	if (point != this->SessionBuf.end()) {
		data = point->second.data;
		//重置stime_key记录
		this->_SetTimeKey(session_id, &(point->second));
		return 0;
	}
	else {
		data = "";
		return -1;
	}
};

int Session::OlderDel(int in_time) {
	//time_t t = time(NULL) - (in_time == 0 ? 60 : in_time);  //时间差 ly mac
	UInt64 t= time(NULL) - (in_time == 0 ? 60 : in_time);
	UInt64 t1 = t << 32;

	SessionTime_T::iterator point, old;
	point = this->SessionTime.begin();
	while ((point != this->SessionTime.end()) && (point->first < t1)) {
		old = point;
		point++;
		//删除session缓存记录
		SessionBuff_T::iterator point2 = this->SessionBuf.find(old->second);
		if (point2 != this->SessionBuf.end())
			this->SessionBuf.erase(point2);
		//删除超过之前的记录,session_time记录；
		this->SessionTime.erase(old);
	}
	return 0; 
};

Session::Session_T Session::_SetTimeKey(Session_T session_id, SessionData_T *session_data) {
	//覆盖旧的key
	if (session_data != NULL) {
		SessionTime_T::iterator point = this->SessionTime.find(session_data->stime_key);
		if (point != this->SessionTime.end()) {
			//删除旧的stime_key;
			this->SessionTime.erase(point);
		}
	}

	//创建新SessionTime;
	Session_T t1= this->_CreateSessionKey();
	this->SessionTime[t1] = session_id;

	//如果必要重置旧的session_data指向此处
	if(session_data!=NULL)
		session_data->stime_key = t1;
	return t1;
}

Session::Session_T Session::_CreateSessionKey(void) {
	if (this->randPlus < 0xffff)
		randPlus++;
	else
		randPlus = 0;  //尽量不重复的session

	//创建SessionBuf
	//UInt64 tmp1 = LW_GetTickCount(); //取得系统启动时间
	UInt64 tmp1 = time(NULL);
	UInt64 hight = tmp1 << 32;  //取得后32位最近时间
								//取得rand
	UInt64 tmp2 = LW_GetTickCount();
	UInt32 low1 = (tmp2 << 48) >> 32;
	srand(LW_GetTickCount() + this->randPlus);
	UInt32 low2 = rand(); //取得随机数；
	UInt64 key = hight + low1 + low2; //取得时间顺序随机数GetTickCount+Rand
	//cout << "hight:" << hight << " low1:" << low1 << " low2" << low2 << " key:" << key << endl;

	return key;
}

//任务队列管理table->priority_queue->cell(node_id,para_buf)
//ly queue
/*
TaskQueue::TaskQueue() {
	this->TableQueueIndex = new TaskQueueObj_TableIndex_T();
};
TaskQueue::~TaskQueue() {
	if (this->TableQueueIndex)
		delete this->TableQueueIndex;
};

bool TaskQueue::PushItem(BB_OBJ *table_obj, BYTE pri_queue_id, TaskQueueCell_T &cell_obj) {
																														  //查询table_obj 是否存在,如果不存在则插入
	BB_SIZE_T table_obj_sz = (BB_SIZE_T)table_obj; //??? ly test 应该在路由中测试。
	TaskQueueObj_TableIndex_T::iterator p;
	if ((p = this->TableQueueIndex->find(table_obj_sz)) == this->TableQueueIndex->end()) {
		//创建空白数组
		TaskQueueObj_PriQueue_T field_map;
		pair< TaskQueueObj_TableIndex_T::iterator, bool > ret = this->TableQueueIndex->insert(make_pair(table_obj_sz, field_map));
		if (!ret.second) {
			GB_AvartarDB->SHOW_ERROR((string)"TaskQueueObj::InsertItem(table_obj) error!");
			return false;
		}
		p = ret.first;
	}

	//判断字段是否存在，如果不存在则插入
	TaskQueueObj_PriQueue_T::iterator f;
	TaskQueue_T queue;
	if ((f = p->second.find(pri_queue_id)) == p->second.end()) {
		//创建空白数组
		std::pair< TaskQueueObj_PriQueue_T::iterator, bool > ret = p->second.insert(make_pair(pri_queue_id, queue));
		if (!ret.second) {
			GB_AvartarDB->SHOW_ERROR((string)"TaskQueueObj::InsertItem(field_id) error! pri_queue_id=" + STRING_Helper::iToStr(pri_queue_id));
			return false;
		}
		f = ret.first;
	}

	//内容插入操作
	f->second.push(cell_obj);
	return true;
};

bool TaskQueue::PopItem(BB_OBJ *table_obj, BYTE pri_queue_id, TaskQueueCell_T &cell_obj) {
	//查询
	BB_SIZE_T table_obj_sz = (BB_SIZE_T)table_obj;
	TaskQueueObj_TableIndex_T::iterator p;
	if ((p = this->TableQueueIndex->find(table_obj_sz)) == this->TableQueueIndex->end())
		return false;

	TaskQueueObj_PriQueue_T::iterator f;
	if ((f = p->second.find(pri_queue_id)) == p->second.end())
		return false;

	//内容插入操作
	cell_obj = f->second.front();
	f->second.pop();

	//返回num
	return true;
};

PrioQueue::PrioQueue() {
	this->TableQueueIndex = new PrioQueueObj_TableIndex_T();
};
PrioQueue::~PrioQueue() {
	if (this->TableQueueIndex)
		delete this->TableQueueIndex;
};

bool PrioQueue::PushItem(string &table_obj, BYTE prio, string &node_id_str, string &paras, ParseXml_Node actions_node) {
	//查询table_obj 是否存在,如果不存在则插入
	PrioQueueObj_TableIndex_T::iterator p;
	if ((p = this->TableQueueIndex->find(table_obj)) == this->TableQueueIndex->end()) {
		//创建空白数组
		PrioQueue_T field_map;
		pair< PrioQueueObj_TableIndex_T::iterator, bool > ret = this->TableQueueIndex->insert(make_pair(table_obj, field_map));
		if (!ret.second) {
			GB_AvartarDB->SHOW_ERROR((string)"PrioQueueObj::InsertItem(table_obj) error!");
			return false;
		}
		p = ret.first;
	}

	PrioQueueCell_T cell_obj;
	cell_obj.prio = prio;
	cell_obj.NodeID = node_id_str;
	cell_obj.ParaBuf = paras;
	cell_obj.ActNode = actions_node;
	p->second.push(cell_obj);

	return true;
};

bool PrioQueue::PopItem(string &table_obj, string &node_id_str, string &paras, ParseXml_Node &actions_node) {
	//查询
	PrioQueueObj_TableIndex_T::iterator p;
	if ((p = this->TableQueueIndex->find(table_obj)) == this->TableQueueIndex->end())
		return false;

	//内容插入操作
	PrioQueueCell_T cell_obj = p->second.top();
	node_id_str = cell_obj.NodeID;
	paras = cell_obj.ParaBuf;
	actions_node = cell_obj.ActNode;
	p->second.pop();

	//返回num
	return true;
};*/
