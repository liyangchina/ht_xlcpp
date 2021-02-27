#include "stdafx.h"
#include "stdio.h"
#include <fstream>
#include <sstream>
#include "STRING_Helper.h"
#include "AvartarDBManager.h"
#include "ParseXml.h"
#include "string.h"
#include "Linux_Win.h"
#include <iostream>
//#include "WizBase.h" //ly mac
#include "ByteBase.h"
#include "ParseProcess.h"
#include "ParseObj.h"
#include "math.h"
#include "WizBase.h"
#include <time.h>
#include <string>

//ly queue
//#if !defined(__ANDROID__) && !defined(__APPLE__)
//ly queue
int AvartarDBManager::DoTaskTableCycleRun(ParseProcess *parent_obj, string &task_table_path, string &func_return, PV_AddParas_T &add_paras) { //ly task
    //初始化加载系统库
    string tableindex;
    BYTE table_r;
    if ((table_r=this->GetPathTableIndex(task_table_path, tableindex)) != 0)
        return table_r;
    
    BYTE *table_obj;
    BB_SIZE_T table_obj_size;
    if (!this->GetTableObject(tableindex, &table_obj, table_obj_size)) {
        this->SHOW_ERROR((string)"INIT():TASK_CYCLE(),Load table error!");
        return -1;
    }
    if (BB_GET_OBJ_TYPE(table_obj)!=BB_TABLE) {
        this->SHOW_ERROR((string)"INIT():TASK_CYCLE(),Load obj is not table,type is!"+STRING_Helper::iToStr(BB_GET_OBJ_TYPE(table_obj)));
        return -2;
    }
    BB_OBJ_STRU task_table_stru= BB_GET_OBJ_STRU(table_obj);
    
    //取得cell—width
    BB_SIZE_T cell_width=BB_GET_TABLE_CELL_WIDTH(task_table_stru.CellObj);
    
    //取得参数
    BYTE *rec_data,*task_field=NULL;
    BB_SIZE_T task_field_offset=0,task_field_size=0;//取得TaskField字段偏移
    if (!BB_GET_TABLE_FIELD_NAME_OFFSET_SIZE_STRU(&task_table_stru,"TaskField",&task_field_offset,&task_field_size)){
        this->SHOW_ERROR((string)"TASK_CYCLE("+task_table_path+")->TableField->TaskField not defined!");
        return -3;
    }
    BB_SIZE_T task_process_offset=0,task_process_size=0;//取得TaskProcessID字段偏移
    if (!BB_GET_TABLE_FIELD_NAME_OFFSET_SIZE_STRU(&task_table_stru,"TaskProcess",&task_process_offset,&task_process_size)){
        this->SHOW_ERROR((string)"TASK_CYCLE("+task_table_path+")->TableField->TaskPrcess not defined!");
        return -4;
    }
    
    //取得当前时间
    UInt64 now_time=(LW_GetTickCount()<< 16);
    //取得字段数组
    vector<string> field_array;
    
    //取得起点、结束指针
    UInt64 start_rec,end_rec;
    bool all_db;
    if (this->GetAddInfoUINT(task_table_stru.AddInfoObj, AVARTDB_ADDINFO_TASK_START_FIELD, start_rec) &&
        this->GetAddInfoUINT(task_table_stru.AddInfoObj, AVARTDB_ADDINFO_CYCLE_ADD_FIELD, end_rec))
        all_db = false; //确定是否全库搜索。
    else{
        all_db = true;
        start_rec=0;
        end_rec=task_table_stru.Num;
    }
    
    //今后可以分为三种方法：1、全库循环针对小库；2、双针递进；3 Map索引()。尚未实现。
    bool continue_free=true;
    ParseXml_Node code_node,process_node,act_node;
    string process_path;
    string step_path;
    string code_path;
    string old_process,old_step;
    bool end_skip=false;
    bool first_run=true;
    PV_AddParas_T task_paras;
    string bitmap_str;
    if (task_table_stru.Num!=0){
        for(BB_SIZE_T r=start_rec; !end_skip; r++){
            //cout << "star:" << start_rec << " end:" << end_rec  << " r:" << r << " r>=task_table_stru.Num_:" << task_table_stru.Num << endl;
            //取得记录位置和task_field;
            if (r>=task_table_stru.Num){
                if (all_db)
                    break; //全库循环
                else
                    r=0; //双指针循环
            }
            
            if (r==end_rec)
                end_skip=true;  //相同则做完自己再运行。
            rec_data=task_table_stru.Data+cell_width*r;
            task_field=rec_data+task_field_offset;
            //判断是否空闲
            UInt64 task_field_value=STRING_Helper::scanUINT64(task_field, 0);
            UInt64 task_state=task_field_value>>62;
            if (task_state==TASK_TABLE_FREE){
                if (!all_db && continue_free && r!=start_rec)
                    this->SetAddInfoUINT(task_table_stru.AddInfoObj,AVARTDB_ADDINFO_TASK_START_FIELD,r);//连续free则指针递进记录
                continue;//递进指针
            }else
                continue_free=false;//停止递进记录指针
            
            //判断是否到时;
            UInt64 do_time=task_field_value & 0x3FFFFFFFFFFFFFFF;
            if (now_time < do_time)
                continue;
            //初次运行的全局处理
            if (first_run){
                //取得代码段
                code_path=this->GetAddInfo(task_table_stru.AddInfoObj, "TaskCodePath");//[@Type='DEFAULT_CONN']
                if (code_path.size()==0){
                    this->SHOW_ERROR((string)"INIT():TASK_CYCLE("+task_table_path+")->AddInfo->TaskCodePath not defined!");
                    return -5;
                }
                code_node=parent_obj->_ActParseXml->GetNodeByXpath(code_path.c_str());
                if (!code_node){
                    this->SHOW_ERROR((string)"INIT():TASK_CYCLE("+task_table_path+")->AddInfo->TaskCodePath("+code_path+") not exisited!");
                    return -5;
                }
            }
            //取得process_id;step_id;
            string task_process="",task_step="";
            //取得Process处理路径
            if (task_process_size==4){//process+step;
                task_process=string((const char *)rec_data+task_process_offset,2);
                task_step=string((const char *)rec_data+task_process_offset+2,2);
                process_path="PRC_"+STRING_Helper::HBCStrToHBCStr(task_process, "char", "hex", false);
                process_node = code_node.child(process_path.c_str());
            }else if (task_process_size==2){//step only;
                task_step=string((const char *)rec_data+task_process_offset,2);
                process_node = code_node;
            }else{
                this->SHOW_ERROR((string)"INIT():TASK_CYCLE("+task_table_path+")->task_process_field len should be 4(process+step) or 2(step)");
                return -4;
            }
            
            if (!process_node){
                this->SHOW_ERROR((string)"INIT():TASK_CYCLE("+task_table_path+")->act_node is null!");
                return -4;
            }
            //取得step处理路径
            if (task_state==TASK_TABLE_START){
                act_node=process_node.child("START");
                if (!act_node){
                    this->SHOW_ERROR(code_path+"/START"+" not defined!");
                    return -7;
                }
            }else if (task_state==TASK_TABLE_WAIT){
                act_node=process_node.child("ERROR");
                if (!act_node){
                    this->SHOW_ERROR(code_path+"/ERROR"+" not defined!");
                    return -7;
                }
            }else if (task_state==TASK_TABLE_RCB){
                string step=(string)"STEP_"+STRING_Helper::HBCStrToHBCStr(task_step, "char", "hex", false);
                act_node=process_node.child(step.c_str());
                if (!act_node){
                    this->SHOW_ERROR(code_path+"/"+step+" not defined!");
                    return -7;
                }
            }
            
            //取得变量数组
            const char *paras=parent_obj->_ActParseXml->NodeAttr(act_node,"Paras");
            string nodeid_str;
            //取得node_id
            if (CreateTableIndexStr(&task_table_stru,r,nodeid_str) !=0){//包括有index_array 和 无。
                this->SHOW_ERROR((string)"DoTaskTableCycleRun_Do ADDINFO_INDEX_FIELD_NOT_DEFINED!");
                return -5;
            }
            if (paras!=NULL && paras[0]!=0){//取得参数用'.'
                string result;
                //取得bitmap
                string bitmap;
                this->GetTableFieldNameArrayBitmap(task_table_stru,paras,bitmap);
                
                //查询记录值；
                BYTE state = this->BuildBufferFromDB_OneNode(result, tableindex, nodeid_str, bitmap,"0xffffffffffffffff",0x60, false);
                if (state!=0){
                    this->SHOW_ERROR((string)"DoTaskTableCycleRun_Do get Record error!");
                    return -6;
                }
                parent_obj->_PubObj->SetNodeValue(".", result.c_str(), Http_ParseObj::STR_COPY_OBJ,(int)result.size());
            }else{
                //取得参数用表格替换
                parent_obj->_PubObj->TableObjStru=&task_table_stru;
                parent_obj->_PubObj->Rec=r;
            }
            
            //核心处理
            parent_obj->FUNCTION(act_node,add_paras);   //ACTIONS 操作
            
            //删除变量数组
            if (paras!=NULL && paras[0]!=0){//取得参数用'.'
                parent_obj->_PubObj->SetNodeValue(".", "", Http_ParseObj::STR_COPY_OBJ);//!!!提速此处可以改为指针模式。
            }else{//取得参数用表格替换
                parent_obj->_PubObj->TableObjStru=NULL;
            }
            //读取回写字段
            string return_str = "",task_data_w;
            parent_obj->_PubObj->GetNodeValueStr("@_RETURN",return_str);
            parent_obj->_PubObj->SetNodeValue("@_RETURN","",Http_ParseObj::STR_COPY_OBJ);    //使用后清空。
            if (return_str.size()!=task_field_size){//可以去除本测试，直接退出。
                this->SHOW_ERROR((string)"DoTaskTableCycleRun->'@_RETURN' not defined or length error!");
                return -6;
            }
            
            //回写数据库,TaskField=return_str
            string wbitmap;
            int rw;
            this->GetTableFieldNameArrayBitmap(task_table_stru,"TaskField",wbitmap);
            if ((rw=this->WriteDataToDB_OneField((BYTE *)return_str.c_str(),return_str.size(),tableindex,nodeid_str,wbitmap))<0 ) {
                //可能有nodeid后数据区为空的情况。
                this->SHOW_ERROR((string)"DoTaskTableCycleRun! write record error" + nodeid_str);
                return -1;
            };
            
            //回写数据库计数。
//            int field_id;
//            if ((field_id=BB_GET_TABLE_FIELD_NAME_ID(task_table_stru.CellObj,"TaskCount"))>=0){//如果存在回调累加。                       
//                BB_SIZE_T cnt=this->GetTableItemUINT(&task_table_stru, r, (BYTE)field_id);
//                this->SetTableItemUINT(&task_table_stru, r, field_id, cnt+1);
//            }
            
            //return 0;
        }
    }
    return 0;
}

int AvartarDBManager::GetTableFieldNameArrayInfo(BB_OBJ_STRU &table_obj_stru,PV_AddParas_T &add_paras,string &bitmap_str){//ly cell_name;ly task此处可以提速
    BYTE *name;
    BB_SIZE_T name_size;
    bitmap_str.clear();
    //
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU(table_obj_stru.CellObj);
    for(BYTE i=0; i< cell_obj_stru.Num; i++){
        bitmap_str+=STRING_Helper::iToStr(i);
        if (BB_GET_TABLE_FIELD_NAME_STRU(&cell_obj_stru,i,&name,&name_size)){
            BB_SIZE_T n=strlen((const char *)name);
            if (n >= name_size){
                string name_str((const char*)name,name_size);
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("#"+name_str+""),STRING_Helper::iToStr(i)));
            }else{
                string name_str((const char*)name,n);
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string("#"+name_str+""),STRING_Helper::iToStr(i)));
            }
        }
    }
    return 0;
}

int AvartarDBManager::GetTableFieldNameArrayBitmap(BB_OBJ_STRU &table_obj_stru,const char *names,string &bitmap_str){//ly cell_name;ly task此处可以提速
    BYTE *field;
    BB_SIZE_T field_size;
    bitmap_str.clear();
    //
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU(table_obj_stru.CellObj);
    //
    if (names[0]=='*'){//全集
        for(BYTE i=0; i< cell_obj_stru.Num; i++){
            bitmap_str+=(char)i;
        }
    }else{//分享对比
        BB_SIZE_T names_len=strlen(names);
        for(BYTE i=0; i< cell_obj_stru.Num; i++){
            if (BB_GET_TABLE_FIELD_NAME_STRU(&cell_obj_stru,i,&field,&field_size)){
                BB_SIZE_T n=strlen((const char *)field);
                BB_SIZE_T name_str_len=n >= field_size ? field_size :n;
                const char* pos;
                if (((pos=strstr(names,(const char*)field))!=0) && ((*(pos+name_str_len))=='|' || ((pos-names)+name_str_len)==names_len))
                    bitmap_str+=(char)i;
            }
        }
        
    }
    return 0;
}

BYTE AvartarDBManager::DoQueueTableTask_Main(ParseProcess *parent_obj, string &func_return, PV_AddParas_T &add_paras) {
    PARSE_BUFF_STRU obj;
    BYTE err = 0xff, status;
    //BYTE task_type = 0;
    string err_str;
    BB_OBJ_STRU *queue_table_obj_stru = this->GetSysQueueTableStru();
    if (queue_table_obj_stru == NULL) {
        this->SHOW_ERROR("DoRegTableTask_Main(): queue table open error! ");
        return err;
    }
    
    //主任务循环
    BB_SIZE_T i;
    for (i=0; i<queue_table_obj_stru->Num; i++) {
        //取得task_type;status;
        this->GetTableItemUINT(queue_table_obj_stru, i, QUEUE_TABLE_Type);//reserved;
        //status = this->GetTableItemUINT(queue_table_obj_stru, i, QUEUE_TABLE_Status);
        status = ACTION_STATE_ON;
        
        if ((status == ACTION_STATE_ON) && this->CheckTaskRouting((*queue_table_obj_stru), i)) {
            //开始执行
            //BYTE rt = ACTION_SON_TASK_IS_LOCAL_END; //ly mac
            
            //枷锁主任务
            this->SetTaskState((*queue_table_obj_stru), ACTION_STATE_HOLD, i);
            this->SetTaskRouting((*queue_table_obj_stru), i); //重置时间
            
            //取得住队列
            string id, pkg;
            ParseXml_Node actions_node;
            BB_OBJ *obj_p = (BB_OBJ *)this->GetTableItemPoint(queue_table_obj_stru, i, QUEUE_TABLE_QPoint);
            if (obj_p == NULL) {
                err_str = err_str = "DoQueueTableTask_Main()->Get Queue Object error! ";
                goto Error;
            }
            if (BB_GET_OBJ_TYPE(obj_p) == BB_NULL)
                continue;
            
            //取得地址信息
            TableNodeQueue *queue;  BYTE type;
            if (!BB_GET_MALLOC_ADDRESS(obj_p, (void **)&queue, &type)) {
                this->SHOW_ERROR("DoQueueTableTask_Main()->Get Queue Object Item error! ");
                goto Error;
            }
            
            //运行队列
            if (parent_obj->DebugType== ParseProcess::DEBUG_NO)
                cout << ">>> "<<this->GetTableItemString(queue_table_obj_stru, i, QUEUE_TABLE_NAME) ; //debug
            for (int num = 0; queue->PopItem(id, pkg, actions_node); num++) {
                //调用队列中的Pkg操作
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string((string)"{@_QID}"), id));
                //add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string((string)"{@_PKG}"), pkg));
                if (parent_obj->_PubObj->SetNodeValue("@_PKG", pkg.c_str(), Http_ParseObj::STR_COPY_OBJ, pkg.size()) != Http_ParseObj::SUCCESS) {
                    err_str = err_str = "Attribute->'@_PKG'can't---maybe readonly! ";
                    add_paras.erase("{@_QUEUE_ID}");
                    goto Error;
                }
                parent_obj->FUNCTION(actions_node, add_paras);   //ACTIONS 操作
                add_paras.erase("{@_QID}");
                //add_paras.erase("{@_PKG}");
                parent_obj->_PubObj->SetNodeValue("@_PKG", "", Http_ParseObj::STR_COPY_OBJ);
            }
            if (parent_obj->DebugType == ParseProcess::DEBUG_NO)
                cout << ":" << i << endl; //debug
            //下一条记录
            this->SetTaskState((*queue_table_obj_stru), ACTION_STATE_ON, i);
            this->SetTaskRouting((*queue_table_obj_stru), i); //重置时间
        }
    }
    return 0;
    
Error:
    //再次可加入StatusError信息
    this->SetTaskState(*queue_table_obj_stru, ACTION_STATE_ON, i);  //所有差错先解锁再说
    string queue_name = this->GetTableItemString(queue_table_obj_stru, i, QUEUE_TABLE_NAME);
    this->SHOW_ERROR("DoRegTableTask_Main():queue_id(" + queue_name + ")、rec(" + STRING_Helper::iToStr(i) + ") process error! ");
    return err;
}

//ly queue
BYTE AvartarDBManager::DoQueueTableTask_PushItem(string &name, BYTE prio, string &cell_id_str, string &paras, ParseXml_Node actions_node) {
    BB_OBJ_STRU *queue_table_obj_stru = this->GetSysQueueTableStru();
    if (queue_table_obj_stru == NULL) {
        this->SHOW_ERROR("DoQueueTableTask_PushItem(): queue table open error! ");
        return 0xff;
    }
    
    //取得QUEUE指针
    BB_SIZE_T num=this->GetTableRecBySortKey(queue_table_obj_stru->ThisObj, QUEUE_TABLE_NAME,(BYTE *)name.data(),name.size());
    if (num == BB_SIZE_T_ERROR)
        return 0xff;        //？？ 和下面的：BB_GET_OBJ_TYPE(obj_p) == BB_NULL是否冲突
    BB_OBJ *obj_p = (BB_OBJ *)this->GetTableItemPoint(queue_table_obj_stru, num, QUEUE_TABLE_QPoint);
    if (obj_p == NULL) {
        this->SHOW_ERROR("DoQueueTableTask_PushItem()->Get Queue Object error! ");
        return 0xfe;
    }
    
    //取得队列指针，如果为空则创建该QueueObj
    TableNodeQueue *queue;
    if (BB_GET_OBJ_TYPE(obj_p) == BB_NULL) {
        //新建MALLOC_OBJ，并取得queue指针
        queue = new TableNodeQueue(); //不需要free，直用到最后???
        BB_OBJ *obj=BB_NEW_MALLOC((void*)&queue, NULL, 0, BB_MALLOC_TYPE_QUEUE, NULL);
        if (obj == NULL) {
            this->SHOW_ERROR("DoQueueTableTask_PushItem()->Create Queue Object error! ");
            return 0xfd;
        }
        BB_PUT_TABLE_ITEMS_DATA_STRU(queue_table_obj_stru, num, QUEUE_TABLE_QPoint, obj, BB_GET_OBJ_SIZE(obj));
        if (obj != NULL)
            free(obj);
    }else{
        //取得已建queue指针
        BYTE type;
        if (!BB_GET_MALLOC_ADDRESS(obj_p, (void **)&queue, &type)) {
            this->SHOW_ERROR("DoQueueTableTask_PushItem()->Get Queue Object Item error! ");
            return 0xfc;
        }
    }
    
    //插入对象
    queue->PushItem(prio, cell_id_str, paras, actions_node);
    this->SetTableItemUINT(queue_table_obj_stru, num, QUEUE_TABLE_QSize, queue->GetMemSize());
    this->SetTableItemUINT(queue_table_obj_stru, num, QUEUE_TABLE_QNum, queue->GetSize());
    return 0;
}

//#endif
BYTE AvartarDBManager::DoRegTableTask_Main(string &reg_table,const char **cb_conn_name,string &func_return,PV_AddParas_T &add_paras){
    PARSE_BUFF_STRU obj;
    string packet_head="";
    BYTE err=0xff;BYTE status;
    string result_input_str("");
    int cb_main_task_rec=-1;
    BYTE task_type=0;
    AVARTDB_REG_TASK_TYPE task_id=0;
    AVARTDB_REG_TASK_TYPE cb_main_task_id;
    
    //取得Register表格对象地址
    BB_OBJ *reg_table_obj;
    BB_SIZE_T reg_table_obj_size;
    BB_OBJ_STRU reg_table_obj_stru;
    if (!this->GetTableObject(reg_table,&reg_table_obj,reg_table_obj_size)){
        this->SHOW_ERROR((string)"DoRegTableTask_Main(),Get TaskTable error!");
        err=DOREG_ERROR_REGTABLE_NOT_FOUND;
        return err;
    }
    reg_table_obj_stru=BB_GET_OBJ_STRU(reg_table_obj);
    
    //主任务循环
    
    for(;this->DoRegTable_Rec<reg_table_obj_stru.Num;){
        //取得task_type;task_id;
        task_type=this->GetTableItemUINT(&reg_table_obj_stru,this->DoRegTable_Rec,NEW_REG_TABLE_TaskType);
        task_id=this->GetTableItemUINT(&reg_table_obj_stru,this->DoRegTable_Rec,NEW_REG_TABLE_TaskID);
        if (this->TestFFFF(task_id,sizeof(task_id))){
            this->SHOW_ERROR((string)"DoRegTableTask_Main(),Get TaskID is not UINT32 Type!");
            err=DOREG_ERROR_TASK_ID_IS_NOT_UINT32;
            return err;
        }
        cb_main_task_id= task_id& AVARTDB_REG_PARENT_TASK_ID_MASK;   //换算主任务id.
        cb_main_task_rec=this->DoRegTable_Rec;
        //判断是否为主任务
        if (task_id !=(task_id & MASK_PARENT_TASK_ID)){
            this->DoRegTable_Rec=this->SkipSonTask(reg_table_obj_stru,cb_main_task_rec);
            continue;
        }
        
        //判断状态是否执行
        status=this->GetTableItemUINT(&reg_table_obj_stru,this->DoRegTable_Rec,NEW_REG_TABLE_Status);
        if ((status==ACTION_STATE_ON || status==ACTION_STATE_HOLD) && this->CheckTaskRouting(reg_table_obj_stru,cb_main_task_rec)){
            //开始执行
            BYTE rt=ACTION_SON_TASK_IS_LOCAL_END;
            
            //枷锁主任务
            this->SetTaskState(reg_table_obj_stru,ACTION_STATE_HOLD,cb_main_task_rec);
            this->SetTaskRouting(reg_table_obj_stru,cb_main_task_rec);
            //运行主任务
            (*cb_conn_name)=NULL;
            result_input_str = "";   //ly test;
            if ((rt=this->DoRegTableTask_Son(&reg_table_obj_stru,this->DoRegTable_Rec,cb_main_task_id,result_input_str,cb_conn_name,func_return,add_paras))!=ACTION_SON_TASK_IS_CLOUD_END && rt!=ACTION_SON_TASK_IS_LOCAL_END){
                this->SHOW_ERROR((string)"DoRegTableTask_Main()->DoRegTableTask_Son() Process error!");
                goto Error;
                
            }else if(rt==ACTION_SON_TASK_IS_LOCAL_END){
                //解锁主任务，如果不需要云端发送
                this->SetTaskState(reg_table_obj_stru,ACTION_STATE_ON,cb_main_task_rec);
                continue; //DoRegTable_Rec内部已递进
            }else if (rt==ACTION_SON_TASK_IS_CLOUD_END){
                //不解锁主任务，如果需要云端发送
                if ((*cb_conn_name)==NULL){
                    this->SHOW_ERROR((string)"DoRegTableTask_Main()->Send To Cloud but cb_conn_name is empty!");
                    goto Error;
                }
                this->DoRegTable_Rec=this->SkipSonTask(reg_table_obj_stru,cb_main_task_rec);  //已经自然地近返回
                break; //中止返回，等下次循环再从此处开始
            }
        }else
            this->DoRegTable_Rec=this->SkipSonTask(reg_table_obj_stru,cb_main_task_rec);//下一条主任务
    }
    if (this->DoRegTable_Rec==reg_table_obj_stru.Num)
        this->DoRegTable_Rec=0; //从头开始运行
    
    //存盘操作
    if (DOREG_DEBUG_SAVE_DB && (reg_table_obj_stru.LoadError!=true)){
        this->SaveDB(reg_table_obj_stru);  //今后改为外部存盘???
    }
    return 0;
    
Error:
    //再次可加入StatusError信息
    if (cb_main_task_rec!=-1)
        this->SetTaskState(reg_table_obj_stru,ACTION_STATE_ON,cb_main_task_rec);  //所有差错先解锁再说
    this->SHOW_ERROR("DoRegTableTask_Main():task_type("+STRING_Helper::iToStr(task_type)+")、task_id("+STRING_Helper::iToStr(task_id)+")、rec("+STRING_Helper::iToStr(this->DoRegTable_Rec)+")!");
    return err;
}

BYTE AvartarDBManager::DoRegTableTask_CallBack(string &reg_table,string &input_str, const char **cb_conn_name,string &func_return,PV_AddParas_T &add_paras){
    PARSE_BUFF_STRU obj;
    string packet_head="";
    BYTE err;
    AVARTDB_REG_TASK_TYPE cb_task_rec=0;
    string result_input_str;
    int cb_main_task_rec=-1;
    BYTE rt;
    BB_SIZE_T next_rec;
    BYTE state;
    
    //取得Register表格对象地址
    BB_OBJ *reg_table_obj;
    BB_SIZE_T reg_table_obj_size;
    if (!this->GetTableObject(reg_table,&reg_table_obj,reg_table_obj_size)){
        this->SHOW_ERROR((string)"DoRegTableTask_CallBask(),Get TaskTable error!");
        return DOREG_ERROR_REGTABLE_NOT_FOUND;
    }
    BB_OBJ_STRU reg_table_obj_stru=BB_GET_OBJ_STRU(reg_table_obj);
    
    //解析回调输入包
    if ((err=this->ParseActionMessage(packet_head,input_str,obj,add_paras))!=0){
        return err; //请求数据结构错误
    }
    if (obj.state!=0 && obj.state!=0xFF){//state==0或ff都代表成功，不过ff还代表当前包为请求包返回值,00代表当前包为请求包
        this->SHOW_ERROR((string)"DoRegTableTask_CallBask(),result string is not successed! obj.state is error(not 0 or ff)!");
        return obj.state;
    }
    result_input_str=string((const char*)obj.input_stru.Data,obj.input_stru.DataLen);
    if (obj.task_id_obj==NULL){//task_id_obj为空的情况，必须有，因为是返回当前task
        this->SHOW_ERROR((string)"DoRegTableTask_CallBask(),callback obj.task_id_obj is Null!");
        return DOREG_ERROR_TASK_ID_IS_NULL;
    }
    
    //取得taskid,tasktype
    cb_task_rec=BB_GET_UINT_VALUE(obj.task_id_obj);	  //回调记录
    BYTE task_type=this->GetTableItemUINT(&reg_table_obj_stru,cb_task_rec,NEW_REG_TABLE_TaskType);
    //AVARTDB_REG_TASK_TYPE task_id=this->GetTableItemUINT(&reg_table_obj_stru,cb_task_rec,NEW_REG_TABLE_TaskID);
    //if (0xFFFFFFFFFFFFFFFF==task_id){   //ly mac
    UInt64 task_id_64=this->GetTableItemUINT(&reg_table_obj_stru,cb_task_rec,NEW_REG_TABLE_TaskID);
    if (0xFFFFFFFFFFFFFFFF==task_id_64){
        this->SHOW_ERROR((string)"DoRegTableTask_CallBack(),Get TaskID is not UINT32 Type!");
        return DOREG_ERROR_TASK_ID_IS_NOT_UINT32;
    }
    AVARTDB_REG_TASK_TYPE task_id=task_id_64;
    AVARTDB_REG_TASK_TYPE cb_main_task_id= task_id & AVARTDB_REG_PARENT_TASK_ID_MASK;   //换算主任务id.
    cb_main_task_rec = cb_task_rec;
    
    //回写当前记录值
    if ((task_type & ACTION_TYPE_SAVE_RESULT_MARK)==ACTION_TYPE_SAVE_RESULT_MARK){
        //写入结果集
        //BB_SIZE_T result_obj_size=result_input_str.size();
        if (!BB_PUT_TABLE_ITEMS_DATA_STRU(&reg_table_obj_stru,cb_task_rec,NEW_REG_TABLE_Result,(BYTE *)result_input_str.data(),result_input_str.size(), BB_TABLE_PUT_OPERATE_REPLACE)){
            this->SHOW_ERROR((string)"DoRegTableTask_CallBack()->BB_PUT_TABLE_ITEMS_DATA(Result_obj),IndexObj error! num="+STRING_Helper::iToStr(NEW_REG_TABLE_Result));
            goto Error;
        }
    }
    
    //主运行循环
    rt=ACTION_SON_TASK_IS_LOCAL_END;
    next_rec=cb_task_rec+1;
    //if ((next_rec>=reg_table_obj_stru.Num)||(task_type & ACTION_TYPE_RESULT_BLOCK_MARK)==ACTION_TYPE_RESULT_BLOCK_MARK && obj.act==ACTION_READ && (BB_GET_OBJ_DATA_LEN(obj.input_stru.Data)==0)){
    if ((next_rec>=reg_table_obj_stru.Num)||((task_type & ACTION_TYPE_RESULT_BLOCK_MARK)==ACTION_TYPE_RESULT_BLOCK_MARK && obj.act==ACTION_READ && (BB_GET_OBJ_DATA_LEN(obj.input_stru.Data)==0))){
        //不执行子操作  //ly mac warning && in ||
    }else{
        //执行子任务
        (*cb_conn_name)=NULL;
        if ((rt=this->DoRegTableTask_Son(&reg_table_obj_stru,next_rec,cb_main_task_id,result_input_str,cb_conn_name,func_return,add_paras))!=ACTION_SON_TASK_IS_CLOUD_END && rt!=ACTION_SON_TASK_IS_LOCAL_END){
            this->SHOW_ERROR((string)"DoRegTableTask_Son(),DoTask Error error!");
            goto Error;
        }
    }
    
    //存盘操作
    if (DOREG_DEBUG_SAVE_DB && ((reg_table_obj_stru.LoadError!=true))){
        this->SaveDB(reg_table_obj_stru);
    }
    
    //判断是否云端发送，否则解锁
    if (rt==ACTION_SON_TASK_IS_CLOUD_END){
        //如果需要云端发送
        if ((*cb_conn_name)==NULL){
            this->SHOW_ERROR((string)"DoRegTableTask_CallBack()->Send To Cloud but cb_conn_name is empty!");
            goto Error;
        }
    }else{
        //解锁操作
        cb_main_task_rec = BB_GET_ARRAY_NUM_BY_INDEX_STRU(&reg_table_obj_stru,(BYTE *)&cb_main_task_id,sizeof(cb_main_task_id));//取得主任务记录数
        state=ACTION_STATE_ON;
        if (!BB_PUT_TABLE_ITEMS_DATA_STRU(&reg_table_obj_stru,cb_main_task_rec,NEW_REG_TABLE_Status,(BYTE *)&state,sizeof(state),BB_TABLE_PUT_OPERATE_REPLACE)){
            this->SHOW_ERROR((string)"DoRegTableTask_CallBack()->BB_PUT_TABLE_ITEMS_DATA(Result_obj),IndexObj error! num="+STRING_Helper::iToStr(NEW_REG_TABLE_Result));
            goto Error;
        }
    }
    return 0;
    
Error:
    //再次可加入StatusError信息
    if (cb_main_task_rec!=-1)
        this->SetTaskState(reg_table_obj_stru,ACTION_STATE_ON,cb_main_task_rec);  //所有差错先解锁再说
    this->SHOW_ERROR("DoRegTableTask_CallBack():task_REC("+STRING_Helper::iToStr(cb_task_rec)+")!");
    return err;
}

BYTE AvartarDBManager::DoRegTableTask_Son(BB_OBJ_STRU *reg_table_obj_stru,BB_SIZE_T &start_task_rec,AVARTDB_REG_TASK_TYPE cb_main_task_id,string &result_input_str, const char **cb_conn_name,string &func_return,PV_AddParas_T &add_paras){
    //初始化
    BYTE err=0;
    BYTE task_type=0;
    AVARTDB_REG_TASK_TYPE task_id=0;
    
    //子任务循环
    for(;start_task_rec<(*reg_table_obj_stru).Num;){
        //取得task_type;task_id;
        task_type=this->GetTableItemUINT(reg_table_obj_stru,start_task_rec,NEW_REG_TABLE_TaskType);
        task_id=this->GetTableItemUINT(reg_table_obj_stru,start_task_rec,NEW_REG_TABLE_TaskID);
        
        //判断是否执行
        if (cb_main_task_id !=(task_id & MASK_PARENT_TASK_ID)){
            return ACTION_SON_TASK_IS_LOCAL_END;//如果是子层回调但超出同层任务，则退出。
        }
        
        BYTE status=this->GetTableItemUINT(reg_table_obj_stru,start_task_rec,NEW_REG_TABLE_Status);
        //判断状态是否执行
        if (status==0){
            start_task_rec++;
            continue; //如果状态==0，则跳过
        }
        
        //读取记录信息
        (*cb_conn_name)=(const char *)this->GetTableItemPoint(reg_table_obj_stru,start_task_rec,NEW_REG_TABLE_ConnName);
        string packet_head="";
        string query_str=this->GetTableItemString(reg_table_obj_stru,start_task_rec,NEW_REG_TABLE_QueryStr);
        //char *rem=(char *)this->GetTableItemPoint(reg_table_obj_stru,start_task_rec,NEW_REG_TABLE_Rem); //ly mac
        this->GetTableItemPoint(reg_table_obj_stru,start_task_rec,NEW_REG_TABLE_Rem);
        
        //Action操作
        PARSE_BUFF_STRU obj;
        switch (task_type & ACTION_TYPE_CLOUD_LOCAL_MARK){
            case ACTION_TYPE_LOCAL: //本地操作
            {
                //取得obj各参数
                string test=string((const char*)query_str.data(),BB_GET_OBJ_SIZE((BB_OBJ *)query_str.data()));
                this->DEBUGY(STRING_Helper::iToStr(start_task_rec)+"(LOCAL):"+STRING_Helper::HBCStrToHBCStr(test,"char","hex",true));
                if ((err=this->ParseActionMessage(packet_head,query_str,obj,add_paras))!=0){
                    goto Error; //请求数据结构错误
                }
                
                if ((task_type & ACTION_TYPE_ADD_PARAS_RESULT_MARK)==ACTION_TYPE_ADD_PARAS_RESULT_MARK){
                    //把上一循环的结果作为组合参数输入
                    if (result_input_str.size()!=0 && !this->_DoRegTableTask_AddResultToParas(result_input_str,obj.paras)){
                        //如果第一个字符为0，则累加参数操作。???需测试。
                        this->SHOW_ERROR((string)"DoRegTableTask_New(),InputObj->Paras2["+STRING_Helper::iToStr(obj.paras.size())+"] must have set as 'buff_obj'!");
                        err=DOREG_ERROR_CANT_PARSE_ADD_INPUT_STRING;	goto Error;
                    }
                }
                //执行本地操作
                if (obj.act==ACTION_READ){
                    if ((err=this->DoActionParas(obj,result_input_str,add_paras))!=0)
                        goto Error; //SHOW_ERROR|ERROR_NUM 在DoActionParas()中//obj.act==ACTION_READ &&
                }else{
                    string tmp_result;
                    //ly test1
                    //cout << "obj.act:" << obj.act << ";obj.flag:" << obj.flag << ";obj.paras.size():" << obj.paras.size() << endl;
                    if ((err=this->DoActionParas(obj,tmp_result,add_paras))!=0)
                        goto Error; //SHOW_ERROR|ERROR_NUM 在DoActionParas()中//obj.act==ACTION_READ &&
                }
            }
                break;
            case ACTION_TYPE_CLOUD: //云端操作
            {
                //生成发送字串
                //初步判断query_str的合法性
                string test=string((const char*)query_str.data(),BB_GET_OBJ_SIZE((BB_OBJ *)query_str.data()));
                this->DEBUGY(STRING_Helper::iToStr(start_task_rec)+"(CLOUD):"+STRING_Helper::HBCStrToHBCStr(test,"char","hex",true));
                if ((err=this->ParseActionMessage(packet_head,query_str,obj,add_paras))!=0){
                    goto Error; //请求数据结构错误//ly 应该在此解决paras对象重定向转换。
                }
                
                if ((task_type & ACTION_TYPE_ADD_PARAS_RESULT_MARK)==ACTION_TYPE_ADD_PARAS_RESULT_MARK){
                    //把上一循环的结果作为组合参数输入
                    if (result_input_str.size()!=0 && !this->_DoRegTableTask_AddResultToParas(result_input_str,obj.paras)){
                        //如果第一个字符为0，则累加参数操作。???需测试。
                        this->SHOW_ERROR((string)"DoRegTableTask_New(),InputObj->Paras2["+STRING_Helper::iToStr(obj.paras.size())+"] must have set as 'buff_obj'!");
                        err=DOREG_ERROR_CANT_PARSE_ADD_INPUT_STRING;	goto Error;
                    }
                }
                
                //自动制定下一条记录
                BB_UINT_STRU task_id_obj=BB_NEW_CELL_UINT((UInt64)(start_task_rec),BB_UINT32);
                obj.task_id_obj=(BB_OBJ *)&task_id_obj;
                this->BuildResultMessage(obj,func_return,add_paras);//ly：在此重构数据包。
                return ACTION_SON_TASK_IS_CLOUD_END;
            }
        }
        
        //下一步
        start_task_rec++;
    }
    return ACTION_SON_TASK_IS_LOCAL_END;
    
Error:
    //再次可加入StatusError信息
    this->SHOW_ERROR("DoRegTableTask_Son():task_type("+STRING_Helper::iToStr(task_type)+")、task_id("+STRING_Helper::iToStr(task_id)+")、rec("+STRING_Helper::iToStr(start_task_rec)+")!");
    return err;
}

bool AvartarDBManager::SetTaskState(BB_OBJ_STRU &reg_table_obj_stru,BYTE status, BB_SIZE_T task_rec, BYTE task_state_field){
    if (!BB_PUT_TABLE_ITEMS_DATA_STRU(&reg_table_obj_stru,task_rec, task_state_field,(BYTE *)&status,sizeof(status),BB_TABLE_PUT_OPERATE_REPLACE)){
        this->SHOW_ERROR((string)"SetTaskState()->BB_PUT_TABLE_ITEMS_DATA(Status) Error! num="+STRING_Helper::iToStr(task_state_field));
        return false;
    }
    return true;
}

bool AvartarDBManager::SetTaskRouting(BB_OBJ_STRU &reg_table_obj_stru,BB_SIZE_T task_rec, BYTE last_conn_time_field, BYTE re_conn_cycle_field){
    //时间判断
    UInt64 now_time=time(NULL);
    string now_time_str = STRING_Helper::putUINTToStr(now_time,64);
    if (!BB_PUT_TABLE_ITEMS_DATA_STRU(&reg_table_obj_stru,task_rec, last_conn_time_field,(BYTE *)now_time_str.data(),now_time_str.size(),BB_TABLE_PUT_OPERATE_REPLACE)){
        this->SHOW_ERROR((string)"SetTaskRouting()->BB_PUT_TABLE_ITEMS_DATA(Status) Error! time="+STRING_Helper::iToStr(now_time));
        return false;
    }
    //以下为测试
    //UInt64 re_conn_cycle=this->GetTableItemUINT(&reg_table_obj_stru,task_rec, re_conn_cycle_field);
    //UInt64 last_conn_time=this->GetTableItemUINT(&reg_table_obj_stru,task_rec, last_conn_time_field);
    return true;
}

bool AvartarDBManager::CheckTaskRouting(BB_OBJ_STRU &reg_table_obj_stru,BB_SIZE_T task_rec, BYTE last_conn_time_field, BYTE re_conn_cycle_field){
    //时间判断
    UInt64 re_conn_cycle=this->GetTableItemUINT(&reg_table_obj_stru,task_rec, re_conn_cycle_field);
    UInt64 last_conn_time=this->GetTableItemUINT(&reg_table_obj_stru,task_rec, last_conn_time_field);
    UInt64 now_time=time(NULL);
    if (now_time >(last_conn_time+re_conn_cycle))
        return true;
    else
        return false;
}

bool AvartarDBManager::_DoRegTableTask_AddResultToParas(string &act_result_str,vector<string>  &paras){
    //把read_数组的data内容解析加入paras
    //BB_SIZE_T obj_size=act_result_str.size();
    BB_OBJ *obj=(BB_OBJ *)act_result_str.data();
    BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
    
    BYTE cell_type= BB_GET_OBJ_TYPE(obj_stru.CellObj);
    if (obj_stru.Type!=BB_ARRAY || (cell_type!=BB_BYTES && cell_type!=BB_BYTES_EXT && cell_type!=BB_NULL))
        return false;
    
    BB_OBJ *item_obj,*bytes_obj_p=(BB_OBJ *)obj_stru.Data,*start_p=bytes_obj_p;
    BB_SIZE_T item_obj_size;
    BB_SIZE_T rec=0,objs_data_size=obj_stru.DataLen;
    for(;bytes_obj_p<(start_p + objs_data_size) ;bytes_obj_p+=item_obj_size,rec++){
        item_obj=(BB_OBJ *)bytes_obj_p;
        item_obj_size=BB_GET_OBJ_SIZE(bytes_obj_p);
        paras.push_back(string((const char *)item_obj,item_obj_size));
    }
    return true;
}

BB_SIZE_T AvartarDBManager::SkipSonTask(BB_OBJ_STRU &reg_table_obj_stru,BB_SIZE_T task_rec){
    AVARTDB_REG_TASK_TYPE task_id,last_task_id=0xFFFFFFFF;
    BB_SIZE_T j=0;
    for(j=task_rec; j<reg_table_obj_stru.Num; j++){
        task_id=this->GetTableItemUINT(&reg_table_obj_stru,j,NEW_REG_TABLE_TaskID);	
        if (last_task_id==0xFFFFFFFF)
            last_task_id=(task_id & MASK_PARENT_TASK_ID);//第一次进入赋值
        if ((task_id & MASK_PARENT_TASK_ID)!=last_task_id)
            break;
    }
    if (j>reg_table_obj_stru.Num)
        j=0;  //循环超出，或者已开始就超出情况。
    
    return j;//跳过子记录 ly:next
}
/*
BYTE QueueTableTask::DoMainCycle(ParseProcess *parent_obj, string &func_return, PV_AddParas_T &add_paras) {
    PARSE_BUFF_STRU obj;
    BYTE err = 0xff, status;
    //BYTE task_type = 0;
    string err_str;
    BB_OBJ_STRU *queue_table_obj_stru = this->GetSysQueueTableStru();
    if (queue_table_obj_stru == NULL) {
        this->SHOW_ERROR("DoRegTableTask_Main(): queue table open error! ");
        return err;
    }
    
    //主任务循环
    BB_SIZE_T i;
    for (i=0; i<queue_table_obj_stru->Num; i++) {
        //取得task_type;status;
        this->GetTableItemUINT(queue_table_obj_stru, i, QUEUE_TABLE_Type);//reserved;
        //status = this->GetTableItemUINT(queue_table_obj_stru, i, QUEUE_TABLE_Status);
        status = ACTION_STATE_ON;
        
        if ((status == ACTION_STATE_ON) && this->CheckTaskRouting((*queue_table_obj_stru), i)) {
            //开始执行
            //BYTE rt = ACTION_SON_TASK_IS_LOCAL_END; //ly mac
            
            //枷锁主任务
            this->SetTaskState((*queue_table_obj_stru), ACTION_STATE_HOLD, i);
            this->SetTaskRouting((*queue_table_obj_stru), i); //重置时间
            
            //取得住队列
            string id, pkg;
            ParseXml_Node actions_node;
            BB_OBJ *obj_p = (BB_OBJ *)this->GetTableItemPoint(queue_table_obj_stru, i, QUEUE_TABLE_QPoint);
            if (obj_p == NULL) {
                err_str = err_str = "DoQueueTableTask_Main()->Get Queue Object error! ";
                goto Error;
            }
            if (BB_GET_OBJ_TYPE(obj_p) == BB_NULL)
                continue;
            
            //取得地址信息
            TableNodeQueue *queue;  BYTE type;
            if (!BB_GET_MALLOC_ADDRESS(obj_p, (void **)&queue, &type)) {
                this->SHOW_ERROR("DoQueueTableTask_Main()->Get Queue Object Item error! ");
                goto Error;
            }
            
            //运行队列
            if (parent_obj->DebugType== ParseProcess::DEBUG_NO)
                cout << ">>> "<<this->GetTableItemString(queue_table_obj_stru, i, QUEUE_TABLE_NAME) ; //debug
            for (int num = 0; queue->PopItem(id, pkg, actions_node); num++) {
                //调用队列中的Pkg操作
                add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string((string)"{@_QID}"), id));
                //add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string((string)"{@_PKG}"), pkg));
                if (parent_obj->_PubObj->SetNodeValue("@_PKG", pkg.c_str(), Http_ParseObj::STR_COPY_OBJ, pkg.size()) != Http_ParseObj::SUCCESS) {
                    err_str = err_str = "Attribute->'@_PKG'can't---maybe readonly! ";
                    add_paras.erase("{@_QUEUE_ID}");
                    goto Error;
                }
                parent_obj->FUNCTION(actions_node, add_paras);   //ACTIONS 操作
                add_paras.erase("{@_QID}");
                //add_paras.erase("{@_PKG}");
                parent_obj->_PubObj->SetNodeValue("@_PKG", "", Http_ParseObj::STR_COPY_OBJ);
            }
            if (parent_obj->DebugType == ParseProcess::DEBUG_NO)
                cout << ":" << i << endl; //debug
            //下一条记录
            this->SetTaskState((*queue_table_obj_stru), ACTION_STATE_ON, i);
            this->SetTaskRouting((*queue_table_obj_stru), i); //重置时间
        }
    }
    return 0;
    
Error:
    //再次可加入StatusError信息
    this->SetTaskState(*queue_table_obj_stru, ACTION_STATE_ON, i);  //所有差错先解锁再说
    string queue_name = this->GetTableItemString(queue_table_obj_stru, i, QUEUE_TABLE_NAME);
    this->SHOW_ERROR("DoRegTableTask_Main():queue_id(" + queue_name + ")、rec(" + STRING_Helper::iToStr(i) + ") process error! ");
    return err;
}

//ly queue
BYTE QueueTableTask::DoQueueTableTask_PushItem(string &name, BYTE prio, string &cell_id_str, string &paras, ParseXml_Node actions_node) {
    BB_OBJ_STRU *queue_table_obj_stru = this->GetSysQueueTableStru();
    if (queue_table_obj_stru == NULL) {
        this->SHOW_ERROR("DoQueueTableTask_PushItem(): queue table open error! ");
        return 0xff;
    }
    
    //取得QUEUE指针
    BB_SIZE_T num=this->GetTableRecBySortKey(queue_table_obj_stru->ThisObj, QUEUE_TABLE_NAME,(BYTE *)name.data(),name.size());
    if (num == BB_SIZE_T_ERROR)
        return 0xff;        //？？ 和下面的：BB_GET_OBJ_TYPE(obj_p) == BB_NULL是否冲突
    BB_OBJ *obj_p = (BB_OBJ *)this->GetTableItemPoint(queue_table_obj_stru, num, QUEUE_TABLE_QPoint);
    if (obj_p == NULL) {
        this->SHOW_ERROR("DoQueueTableTask_PushItem()->Get Queue Object error! ");
        return 0xfe;
    }
    
    //取得队列指针，如果为空则创建该QueueObj
    TableNodeQueue *queue;
    if (BB_GET_OBJ_TYPE(obj_p) == BB_NULL) {
        //新建MALLOC_OBJ，并取得queue指针
        queue = new TableNodeQueue(); //不需要free，直用到最后???
        BB_OBJ *obj=BB_NEW_MALLOC((void*)&queue, NULL, 0, BB_MALLOC_TYPE_QUEUE, NULL);
        if (obj == NULL) {
            this->SHOW_ERROR("DoQueueTableTask_PushItem()->Create Queue Object error! ");
            return 0xfd;
        }
        BB_PUT_TABLE_ITEMS_DATA_STRU(queue_table_obj_stru, num, QUEUE_TABLE_QPoint, obj, BB_GET_OBJ_SIZE(obj));
        if (obj != NULL)
            free(obj);
    }else{
        //取得已建queue指针
        BYTE type;
        if (!BB_GET_MALLOC_ADDRESS(obj_p, (void **)&queue, &type)) {
            this->SHOW_ERROR("DoQueueTableTask_PushItem()->Get Queue Object Item error! ");
            return 0xfc;
        }
    }
    
    //插入对象
    queue->PushItem(prio, cell_id_str, paras, actions_node);
    this->SetTableItemUINT(queue_table_obj_stru, num, QUEUE_TABLE_QSize, queue->GetMemSize());
    this->SetTableItemUINT(queue_table_obj_stru, num, QUEUE_TABLE_QNum, queue->GetSize());
    return 0;
}

BB_OBJ_STRU * QueueTableTask::GetSysQueueTableStru() {
    BYTE *table_obj;
    BB_SIZE_T table_obj_size;
    if (this->SysQueueTableStru.ThisObj == NULL) {
        //初始化加载系统库
        string sys_info_table(AVARTDB_SYS_QUEUE_TABLE0_PATH);
        if (!this->GetTableObject(sys_info_table, &table_obj, table_obj_size)) {
            this->SHOW_ERROR((string)"INIT():SYS_INFO_QUEUE_TABLE0(),Load table error!");
            return NULL;
        }
        if (BB_GET_OBJ_TYPE(table_obj)!=BB_TABLE) {
            this->SHOW_ERROR((string)"INIT():SYS_INFO_QUEUE_TABLE0(),Load obj is not table,type is!"+STRING_Helper::iToStr(BB_GET_OBJ_TYPE(table_obj)));
            return NULL;
        }
        this->SysQueueTableStru = BB_GET_OBJ_STRU(table_obj);
        
    }
    return &(this->SysQueueTableStru);
}

UInt64 QueueTableTask::GetTableItemUINT(BB_OBJ_STRU *table_obj_stru,BB_SIZE_T rec,BYTE col){
    //取得item;
    BYTE *item;
    BB_SIZE_T item_size;
    
    if (BB_GET_TABLE_ITEMS_DATA_STRU(table_obj_stru,rec,col,&item,&item_size)){//取得对象
        string tmp=string((const char *)item,item_size);
        return STRING_Helper::scanUINTStr(tmp);
    }
    return 0xFFFFFFFFFFFFFFFF;
}

bool QueueTableTask::CheckTaskRouting(BB_OBJ_STRU &reg_table_obj_stru,BB_SIZE_T task_rec, BYTE last_conn_time_field, BYTE re_conn_cycle_field){
    //时间判断
    UInt64 re_conn_cycle=this->GetTableItemUINT(&reg_table_obj_stru,task_rec, re_conn_cycle_field);
    UInt64 last_conn_time=this->GetTableItemUINT(&reg_table_obj_stru,task_rec, last_conn_time_field);
    UInt64 now_time=time(NULL);
    if (now_time >(last_conn_time+re_conn_cycle))
        return true;
    else
        return false;
}*/

