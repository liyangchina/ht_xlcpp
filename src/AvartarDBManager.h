#ifndef AVARTAR_DB
#define AVARTAR_DB

//#include "XML_Mini.h"
#include "ParseXml.h"
#include "ByteBase.h"
#include <vector>
#include <map>
#include <string>
#include "AvartarDB_Action.h"
//#if !defined(__ANDROID__) && !defined(__APPLE__)
#include "ParseProcess.h"
//#include "ObjectPorcess.h"
//#endif
using namespace std;

class AvartarDBManager : public AvartarDB_Action{
public:
    enum{
        REG_TABLE_TaskType = 0,
        REG_TABLE_TaskInfo = 1,
        REG_TABLE_From_Table = 2,
        REG_TABLE_To_Table = 3,
        REG_TABLE_ConvertTable = 4,
        REG_TABLE_RFlag = 5,
        REG_TABLE_RBitmap = 6,
        REG_TABLE_RCondition = 7,
        NEW_REG_TABLE_TaskType = 0,
        NEW_REG_TABLE_TaskID = 1,
        NEW_REG_TABLE_ConnName = 2,
        NEW_REG_TABLE_NextStep = 3,
        NEW_REG_TABLE_Status = 4,
        NEW_REG_TABLE_ReConnCycle = 5,
        NEW_REG_TABLE_LastConnTime = 6,
        //NEW_REG_TABLE_ActFlag=7,
        NEW_REG_TABLE_QueryStr = 7,
        NEW_REG_TABLE_Rem = 8,
        NEW_REG_TABLE_Result = 9,
        
        //ly queue
        QUEUE_TABLE_Type = 0,
        QUEUE_TABLE_Prio = 1,
        QUEUE_TABLE_NAME = 2,
        QUEUE_TABLE_StatusError = 3,
        QUEUE_TABLE_Status = 4,
        QUEUE_TABLE_ReConnCycle = 5,
        QUEUE_TABLE_LastConnTime = 6,
        QUEUE_TABLE_CodeStr = 7,
        QUEUE_TABLE_Rem = 8,
        QUEUE_TABLE_QPoint = 9,
        QUEUE_TABLE_QSize = 10,
        QUEUE_TABLE_QNum = 11,
        //ly task
        TASK_TABLE_START=1,
        TASK_TABLE_RCB=2,
        TASK_TABLE_WAIT=3,
        TASK_TABLE_FREE=0
    };
	//公共函数
	//AvartarDBManager(string sensor_db_xml);
	AvartarDBManager(string sensor_db_xml):AvartarDB_Action(sensor_db_xml){
        this->DoRegTable_Rec = 0;
	}
	AvartarDBManager(char * xml, int filename_or_xml,string save_path):AvartarDB_Action(xml,filename_or_xml,save_path){
        this->DoRegTable_Rec = 0;
	}
	~AvartarDBManager(){
	}
    BB_SIZE_T DoRegTable_Rec; //DoRegTable_New的启示位置。
    
//#if !defined(__ANDROID__) && !defined(__APPLE__)
	BYTE DoQueueTableTask_Main(ParseProcess *parent_obj, string &func_return, PV_AddParas_T &add_paras);
	BYTE DoQueueTableTask_PushItem(string &name, BYTE prio, string &node_id_str, string &paras, ParseXml_Node actions_node);
    int DoTaskTableCycleRun(ParseProcess *parent_obj,string &table_path, string &func_return, PV_AddParas_T &add_paras);
    //int DoTaskTableCycleRun_Do(ParseProcess *parent_obj, BB_OBJ_STRU &table_stru, string &table_path,BB_SIZE_T rec, string &bitmap_str,ParseXml_Node actions_node,string &func_return, PV_AddParas_T &add_paras);
    int GetTableFieldNameArrayInfo(BB_OBJ_STRU &cell_obj_stru,PV_AddParas_T &add_paras,string &bitmap_str); //ly cell_name
    int GetTableFieldNameArrayBitmap(BB_OBJ_STRU &cell_obj_stru,const char *names,string &bitmap_str); //ly cell_name
//#endif
	BYTE DoRegTableTask_Main(string &reg_table,const char **cb_conn_name,string &func_return,PV_AddParas_T &add_paras);
	BYTE DoRegTableTask_CallBack(string &reg_table,string &input_str, const char **cb_conn_name,string &func_return,PV_AddParas_T &add_paras);
	BYTE DoRegTableTask_Son(BB_OBJ_STRU *reg_table_stru,BB_SIZE_T &start_task_rec,AVARTDB_REG_TASK_TYPE cb_main_task_id,string &last_input_str, const char **cb_conn_name,string &func_return,PV_AddParas_T &add_paras);
	bool _DoRegTableTask_AddResultToParas(string &act_result_str,vector<string>  &paras);
	BB_SIZE_T SkipSonTask(BB_OBJ_STRU &reg_table_obj_stru,AVARTDB_REG_TASK_TYPE task_id);
	bool SetTaskState(BB_OBJ_STRU &reg_table_obj_stru,BYTE status, BB_SIZE_T task_rec, BYTE task_state_field= NEW_REG_TABLE_Status);
	bool SetTaskRouting(BB_OBJ_STRU &reg_table_obj_stru,BB_SIZE_T task_rec, BYTE last_conn_time = NEW_REG_TABLE_LastConnTime, BYTE re_conn_cycle = NEW_REG_TABLE_ReConnCycle);
	bool CheckTaskRouting(BB_OBJ_STRU &reg_table_obj_stru,BB_SIZE_T task_rec, BYTE last_conn_time = NEW_REG_TABLE_LastConnTime, BYTE re_conn_cycle = NEW_REG_TABLE_ReConnCycle);
};
/*
class QueueTableTask: public ParseBase{
public:
    //系统队列
    BB_OBJ_STRU SysQueueTableStru;
    BB_OBJ_STRU *GetSysQueueTableStru();
    UInt64 GetTableItemUINT(BB_OBJ_STRU *table_obj_stru,BB_SIZE_T rec,BYTE col);
    bool CheckTaskRouting(BB_OBJ_STRU &reg_table_obj_stru,BB_SIZE_T task_rec, BYTE last_conn_time_field, BYTE re_conn_cycle_field);
public:
    BYTE DoMainCycle(ParseProcess *parent_obj, string &func_return, PV_AddParas_T &add_paras);
    BYTE DoPushItem(string &name, BYTE prio, string &node_id_str, string &paras, ParseXml_Node actions_node);
private:
}*/

#endif
