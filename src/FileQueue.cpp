//#include "ConnQueue.h"
//#include "ObjectProcess.h"
#include <iostream>
#include "STRING_Helper.h"
#include "FileQueue.h"


//加载文件，并写入队列
int FileQueue::LoadFile(string &file_name, POINT &point,byte open_mode) {
	point = this->FQueue.find(file_name);
	if (point == this->FQueue.end()) {
		FileObj_T sdata;
		fstream *f;
		if (open_mode == OPEN_MODE_TRUNC)
			f = new fstream(file_name.c_str(), ios::in | ios::out | ios::binary | ios::trunc);
		else
			f = new fstream(file_name.c_str(), ios::in | ios::out | ios::binary);
		if (!f->good()) {
			delete f;
			return -1;//打开错误
		}

		//初始化数据
		sdata.f_id = f;
		sdata.changed = false;
		f->seekg(0, std::ios::end);
		sdata.length = f->tellg();
		std::pair<POINT, bool> ret = this->FQueue.insert(make_pair(file_name, sdata));
		if (ret.second == false) {
			cout << "Error:LoadFile(" << file_name << ") map FQueue.insert() error!" << endl;
			return -2;
		}

		point = ret.first;
	}

	return 0;//已经存在
}

//关闭队列中的文件，并删除队列
void FileQueue::UnLoadFile(POINT point) {
	//关闭文件,删除文件对象
	fstream *f = point->second.f_id;
	f->close();
	delete f;

	//删除旧的file_id;
	this->FQueue.erase(point);

	return;
}

void FileQueue::UnLoadFile(string &file_name) {
	POINT point = this->FQueue.find(file_name);
	if (point == this->FQueue.end())
		return;

	this->UnLoadFile(point);
}

int FileQueue::LoadFileReadAll(string &file_name, void **buf, BB_SIZE_T &length, byte open_mode) {
	//读取全部文件
	if (LW_access(file_name.c_str(), 0) <0)
		return -1;//如果文件不存在！
	if (open_mode == OPEN_MODE_TRUNC)
		return -2;//如果只是读取，则不要刷新文件

				  //打开文件操作
	POINT point; int r;
	if ((r = this->LoadFile(file_name, point, open_mode)) != 0)
		return -3;

	if (point->second.length == 0)
		return -4;

	//开内存
	BB_SIZE_T fl = point->second.length; //赋值

										 //读取信息
	fstream *f = point->second.f_id;//打开文件
	f->seekg(0, std::ios::beg);//从头开始
	(*buf) = new char[fl + 1];
	memset((*buf), 0, fl + 1);
	f->read((char *)(*buf), fl);
	return 0;
}

int FileQueue::LoadFileWrite(string &file_name, const char* buf, BB_SIZE_T &length, BB_SIZE_T start, byte save_flag, byte open_mode) {
	//打开文件操作
	POINT point; int r;
	if ((r = this->LoadFile(file_name, point, open_mode)) != 0)
		return r;
	fstream *f = point->second.f_id;//打开文件

									//定位指针
	f->seekg(start, std::ios::beg);//找到起点

								   //写入操作,
	f->write(buf, (std::streamsize)length);

	//flush操作
	if (save_flag == SAVE_SYNC) {
		f->sync();
	}
	point->second.changed = true;

	//成功
	return 0;
}

int FileQueue::ReadFile(string &file_name, void **buf, BB_SIZE_T &length, BB_SIZE_T start) {
	//打开文件操作
	POINT point = this->FQueue.find(file_name);
	if (point == this->FQueue.end())
		return -1;

	return this->ReadFile(point, buf, length, start);
}
int FileQueue::ReadFile(POINT point, void **buf, BB_SIZE_T &length, BB_SIZE_T start) {
	fstream *f = point->second.f_id;//打开文件

	if (start == (BB_SIZE_T)READ_ALL) {
	//读取全部
		length = point->second.length;
		start = 0;
	}else{
	//读取部分
		if (start > point->second.length)
			return -2;//判断是否超出

		if (point->second.length == 0 && length != 0)
			return 0;
		if ((length + start) > point->second.length)
			length = point->second.length - start;
	}

	//定位指针，并开内存
	f->seekg(start, std::ios::beg);//找到起点
	(*buf) = new char[length + 1];
	memset((*buf), 0, length + 1);

	//读取信息
	f->read((char *)(*buf), length);

	return 0;
}
int FileQueue::WriteFile(string &file_name, const char* buf, BB_SIZE_T &length, BB_SIZE_T start, byte save_flag) {
	//查找文件
	POINT point = this->FQueue.find(file_name);
	if (point == this->FQueue.end())
		return -1;

	return this->WriteFile(point,buf,length,start,save_flag);
}
int FileQueue::WriteFile(POINT point, const char* buf, BB_SIZE_T &length, BB_SIZE_T start, byte save_flag) {
	fstream *f = point->second.f_id;//打开文件
	f->seekg(start, std::ios::beg);//找到起点

    //写入操作,
	f->write(buf, (std::streamsize)length);

	//flush操作
	if ((save_flag & SAVE_SYNC )== SAVE_SYNC) {
#ifdef FILE_COMMIT
		this->Commit(point,true)
#else
		f->sync();
#endif
	}
	point->second.changed = true;

	//成功
	return 0;
}

bool FileQueue::Commit(string file_name, bool debug) {
	POINT point = this->FQueue.find(file_name);
	if (point!= this->FQueue.end()) 
		return this->Commit(point);
	else
		return false;
}

bool FileQueue::Commit(POINT point, bool debug) {
	if (point != this->FQueue.end()) {
		if (point->second.changed == true) {
			if (debug)
				cout << "Commit " << point->first << endl;
			//关闭再打开
			fstream *f1 = point->second.f_id;
			f1->close();
			delete f1;
			point->second.changed = false;
			fstream *f2 = new fstream(point->first.c_str(), ios::in | ios::out | ios::binary);
			if (!f2->good()) {
				cout << "file_commit error(" << point->first << ")" << endl;
				delete f2;
				return false;//打开错误
			}
			else
				point->second.f_id = f2;
		}
	}
	return  true;
}

bool FileQueue::CommitAll(bool debug) {
	for (FQueue_T::iterator point=this->FQueue.begin(); point != this->FQueue.end(); point++) {
		if (!this->Commit(point,debug))
			return false;
	}
	return true;
}

UInt64  FileQueue::GetFileLength(POINT point) {
	return point->second.length;
}

FileQueue::~FileQueue() {
	//遍历所有子对象，并关闭。
	for (FQueue_T::iterator point; point != this->FQueue.end(); point++) {
		fstream *f = point->second.f_id;
		f->close();
		delete f;
	}
};


// ly queue
TableNodeQueue::TableNodeQueue() {
	this->TableNodeQueueIndex = new TableNodeQueue_T();
};

TableNodeQueue::~TableNodeQueue() {
	if (this->TableNodeQueueIndex)
		delete this->TableNodeQueueIndex;
};

bool TableNodeQueue::PushItem(BYTE prio, string &cell_id_str, string &pkg, ParseXml_Node actions_node) {

	TableNodeQueueCell_T cell_obj;
	cell_obj._Prio = prio;
	cell_obj.CellID = cell_id_str;
	cell_obj.PkgBuf = pkg;
	cell_obj.ActNode = actions_node;
	this->TableNodeQueueIndex->push(cell_obj);

	return true;
};

bool TableNodeQueue::PopItem(string &cell_id_str, string &pkg, ParseXml_Node &actions_node) {
	//内容插入操作
	//TableNodeQueueCell_T cell_obj = this->TableNodeQueueIndex->top();
	if (this->TableNodeQueueIndex->empty())
		return false;
	TableNodeQueueCell_T cell_obj = this->TableNodeQueueIndex->front();
	cell_id_str = cell_obj.CellID;
	pkg = cell_obj.PkgBuf;
	actions_node = cell_obj.ActNode;
	this->TableNodeQueueIndex->pop();

	//返回num
	return true;
};

BB_SIZE_T TableNodeQueue::GetMemSize() {
#ifdef TableNodeQueue_VectorCell
	return this->TableNodeQueueIndex->capacity();
#else
	return 0;
#endif
}
BB_SIZE_T TableNodeQueue::GetSize() {
	return this->TableNodeQueueIndex->size();
}

//ly_send_queue
SendNodeQueue::SendNodeQueue() {
	this->SendNodeQueueIndex = new SendNodeQueue_T();
};

SendNodeQueue::~SendNodeQueue() {
	if (this->SendNodeQueueIndex)
		delete this->SendNodeQueueIndex;
};

bool SendNodeQueue::PushItem(SendNodeQueueCell_T &in_send_queue) {
	//公共头
	/*SendNodeQueueCell_T cell_obj;
	cell_obj._Prio = in_send_queue._Prio;
	cell_obj.CellID = in_send_queue.CellID;
	cell_obj.PkgBuf = in_send_queue.PkgBuf;
	cell_obj.ActNode = in_send_queue.ActNode;

	//发送增补
	cell_obj.srcObj = in_send_queue.srcObj;
	cell_obj.trans_type= in_send_queue.trans_type;
	cell_obj.close_after_send=in_send_queue.close_after_send;
	cell_obj.time_out=in_send_queue.time_out;
    cell_obj.time_out_path=in_send_queue.time_out_path;
	this->SendNodeQueueIndex->push(cell_obj);*/
	this->SendNodeQueueIndex->push(in_send_queue);

	return true;
};

bool SendNodeQueue::PopItem(SendNodeQueueCell_T &out_send_queue) {
	if (this->SendNodeQueueIndex->empty()){
        out_send_queue.CellID = "";
        out_send_queue.PkgBuf = "";
		//actions_node = NULL;
		//发送增补
        out_send_queue.srcObj= NULL;
        out_send_queue.trans_type= 0;
        out_send_queue.close_after_send=true;
        out_send_queue.time_out=0;
        out_send_queue.time_out=NULL;
		out_send_queue.rstate_field="";
		out_send_queue.send_state=SendNodeQueue::CANCLED;
		return false;
	}
	//内容插入操作
	out_send_queue=this->SendNodeQueueIndex->front();
	/*SendNodeQueueCell_T cell_obj = this->SendNodeQueueIndex->front();
    out_send_queue.CellID = cell_obj.CellID;
    out_send_queue.PkgBuf = cell_obj.PkgBuf;
    out_send_queue.ActNode = cell_obj.ActNode;

	//发送增补
    out_send_queue.srcObj= cell_obj.srcObj;
    out_send_queue.trans_type= cell_obj.trans_type;
    out_send_queue.close_after_send=cell_obj.close_after_send;
    out_send_queue.time_out=cell_obj.time_out;
    out_send_queue.time_start=cell_obj.time_start;
    out_send_queue.time_out_path=cell_obj.time_out_path;*/
	this->SendNodeQueueIndex->pop();
	return true;
};

BB_SIZE_T SendNodeQueue::GetMemSize() {
#ifdef SendNodeQueue_VectorCell
	return this->SendNodeQueueIndex->capacity();
#else
	return 0;
#endif
}

BB_SIZE_T SendNodeQueue::GetSize() {
	return this->SendNodeQueueIndex->size();
}

