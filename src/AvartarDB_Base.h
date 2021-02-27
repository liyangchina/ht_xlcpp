#ifndef AVARTAR_DB_BASE
#define AVARTAR_DB_BASE

//#include "XML_Mini.h"
#include "ParseXml.h"
#include <vector>
#include <map>
#include <string>
#include "ByteBase.h"
#include "ParseBase.h"
#include "FileQueue.h"
#include "AvartarSortObj.h"
using namespace std;
#define AVARTDB_SAVE_FILE_NAME "SAVE_PATH"
#define AVARTDB_ADDINFO_TABLE_DIR "INDEX_TABLE_DIR"
#define AVARTDB_ADDINFO_OBJ_NAME "NAME"
#define AVARTDB_ADDINFO_MODEL_TYPE "MODEL_TYPE"
#define AVARTDB_ADDINFO_INDEX_MAX_NUM "INDEX_MAX_NUM"
#define AVARTDB_ADDINFO_LAST_CHG_TIME "LAST_CHG_TIME"
#define AVARTDB_ADDINFO_UPLOADFLAG_FIELD "UPLOADFLAG_FIELD"
#define AVARTDB_ADDINFO_ACCESS_MASK_FIELD "ACCESS_AUTH_FIELD"
#define AVARTDB_ADDINFO_ACCESS_MASK "ACCESS_AUTH"
#define AVARTDB_ADDINFO_INDEX_FIELD "INDEX_FIELD"
#define AVARTDB_ADDINFO_CYCLE_ADD_FIELD "CYCLE_ADD"
#define AVARTDB_ADDINFO_TASK_START_FIELD "TASK_START"
#define AVARTDB_SORT_FIELDS_NAME "SORT_FIELDS"
#define AVARTDB_EVAL_AFTER_WRITE "EVAL_AFTER_WRITE"
#define AVARTDB_SAVE_MALLOC_EXT "SAVE_MALLOC_EXT"
#define AVARTDB_CREATE_OBJ_HEAD_STR "N_"
#define AVARTDB_DO_INIT_SAVE_DB 1
#define AVARTDB_NEW_VERSION  
//#define AVARTDB_REG_TASK_TYPE BB_SIZE_T
#define AVARTDB_REG_SON_TASK_ID_MARK 0xff
#define AVARTDB_REG_PARENT_TASK_ID_MASK 0xffffff00
#define AVARTDB_AUTH_MASK_NULL STRING_Helper::HBCStrToHBCStr("0x0000000000000000","hex","char",false)
#define AVARTDB_AUTH_MASK_FULL STRING_Helper::HBCStrToHBCStr("0xFFFFFFFFFFFFFFFF","hex","char",false)
#define AVARTDB_AUTH_MASK_TYPE string
#define AVARTDB_SYS_INFO_TABLE0_PATH STRING_Helper::HBCStrToHBCStr("0x01040101","hex","char",false)
#define AVARTDB_SYS_QUEUE_TABLE0_PATH STRING_Helper::HBCStrToHBCStr("0x01040104","hex","char",false)
#define DEFAULT_CURSOR "1,10000"

class AvartarDB_Base : public ParseBase{
public:
//#define
#define ACTION_WRITE 1
#define ACTION_READ 6
#define ACTION_LOGIN 8

	typedef BB_SIZE_T AVARTDB_REG_TASK_TYPE;
	enum {
        DEFAULT_PAGE_COUNT=20,
		LOAD_XML_CONTENT=1,
		LOAD_XML_FILENAME=0,
		SET_LAST_ERROR = 1,
		FIELD_PARAS_SUCCESS = 0,
		FIELD_PARAS_ERROR = -1,
		DB_METHOD_ERROR = 7777,
		READ = 0,
		WRITE = 1,
		DB_OPEN = 2,
		DB_TYPE = 3,
		ENV_OPEN = 4,
		CURSOR_WRITE = 5,
		CURSOR_OPEN = 6,
		CURSOR_READ = 7,
		REPLACE_TYPE = 9,
		TXN_OPEN = 8,
		TXN_ABORT = 1,
		TXN_COMMIT = 0,
		TABLEINDEX_MAIN = 1,
		TABLEJENNIC = 3,
		PERSONALTABLE = 3,
		/*OPEN_MODE_TRUNC = 1,
		OPEN_MODE_NORMAL = 0,*/
		JENNIC_REQ_BITMAP_FMT_DATA = 0x000007e0,
		JENNIC_BITMAP_FMT_DATA = 0x00000080,
		JENNIC_BITMAP_FMT_FLAG = 0x007e0000,
		JENNIC_FMT_FIELD = 29,
		JENNIC_REQ_FMT_DATA_START = 21,
		JENNIC_ACCESSMARK_FIELD = 15,
		JENNIC_ACCESSLEVEL_FIELD = 31,
		JENNIC_UPLOADFLAG_FIELD = 0,
		CELLWIDTH_BUF_ADD_MAX = 250   ,
		DBCONFIG_NODE_VALUE = 1,
		DBCONFIG_NODE_NAME = 0,
		DBCONFIG_NODE_OBJECT_MALLOC = 2,
		DBCONFIG_NODE_OBJECT_COPY = 3,
        DBCONFIG_NODE_TABLE_RECORD = 4,

		METHOD_BUF_IS_TABLE = 2,
		QUERY_MAX_CONDITIONS_SIZE = 6,
		QUERY_CURSOR_STR_LEN = 6,
		ACTION_TYPE_CLOUD_LOCAL_MARK = 0xc0,
		ACTION_TYPE_CLOUD = 0x80,
		ACTION_TYPE_LOCAL = 0x40,
		ACTION_TYPE_RESULT_BLOCK_MARK = 0x1,
		ACTION_TYPE_SAVE_RESULT_MARK = 0x2,
		ACTION_TYPE_ADD_PARAS_RESULT_MARK = 0x4,
		ACTION_PACKET_TYPE_DATAR = 0x0,
		ACTION_PACKET_TYPE_DATAR_PACKET = 0x01,
		ACTION_PACKET_TYPE_DATAR_OBJECT = 0x02,
		ACTION_PACKET_TYPE_DATAR_ARRAY = 0x03,
		ACTION_MAX_PARAS_NUM = 500,
		//ACTION_WRITE = 1,
		ACTION_DELETE = 2,
		ACTION_CREATE_OBJ_BY_XML_MODEL = 3,
		ACTION_DELETE_OBJ_XML_CLUSTER = 4,
		ACTION_GET_TABLE_INFO = 5,
		//ACTION_READ = 6,
		ACTION_GET_CLUSTER_INFO = 7,
		//ACTION_LOGIN = 8,  //defined outside xml
		ACTION_READ_EXT=0xa,
		ACTION_WRITE_EXT=0xb,
		ACTION_GET_TABLE_INFO_TABLE_COUNT=1,

		ACTION_GET_CLUSTER_INFO_TYPE = 0x80000000,
		ACTION_GET_CLUSTER_INFO_NAME =0x40000000,
		ACTION_GET_CLUSTER_INFO_COUNT = 0x20000000,
		ACTION_GET_CLUSTER_MODEL_TYPE = 0x10000000,
		ACTION_GET_CLUSTER_SAVE_PATH = 0x08000000,
		ACTION_GET_CLUSTER_INFO_ADDINFO = 0x04000000,
        ACTION_GET_CLUSTER_INFO_INDEX_TYPE = 0x02000000,
		ACTION_PARAS_ARRAY_PACKET_HEAD_REC=0,
		ACTION_PARAS_ARRAY_PACKET_TASKID_REC=1,
		ACTION_PARAS_ARRAY_PACKET_SESSION_REC = 2,
		//ACTION_ERROR_PACKET_HEAD_NUM_ERROR=1,
		
		ACTION_SON_TASK_IS_CLOUD_END=0x1,
		ACTION_SON_TASK_IS_LOCAL_END=0x0,
		DOREG_ERROR_REGTABLE_NOT_FOUND=3,
		DOREG_ERROR_INPUT_STR_PARAS_IS_OUT_OF_RANGE=4,
		DOREG_ERROR_TASK_ID_IS_NOT_UINT32=5,
		DOREG_ERROR_QUERY_STR_CANT_PARSE_TO_ARRAY_OBJS=6,
		DOREG_ERROR_CANT_PARSE_ADD_INPUT_STRING=7,
		/*DOREG_ERROR_CLOUD_PARASE_QUERY_STR_PROBLEM=8,
		DOREG_ERROR_CLOUD_PARASE_QUERY_STR_ADDINFO_PROBLEM=9,*/
		DOREG_ERROR_CLOUD_PARASE_QUERY_STR_NEXT_TASK_ID_IN_ADDINFO_DEFINED_ERROR=8,
		DOREG_ERROR_CLOUD_PARASE_QUERY_STR_NEXT_TASK_ID_IN_ADDINFO_IS_NOT_NEXT_REC=9,
		ACTION_ERROR_GET_TABLE_TASKID_OBJ_IS_NOT_BB_UINT8 = 0x8,
		ACTION_ERROR_GET_TABLE_INFO_UNKNOW_TASKID = 0x9,
		DOREG_ERROR_TASK_ID_IS_NOT_PARENT=0xa,
		DOREG_ERROR_TASK_ID_IS_NULL=0xb,
		DOREG_DEBUG_SAVE_DB=1,
		MARK_SON_TASK_ID=0xff,
		MASK_PARENT_TASK_ID=0xffffff00,
		ACTION_STATE_ON=1,
		ACTION_STATE_OFF=0,
		ACTION_STATE_HOLD=2,
		ACTION_ADD_FLAG_NO_ADD_INDEX=0xFE,
		ACTION_ADD_FLAG_ADD_INDEX=1,
		ACTION_ADD_FLAG_VERICAL = 2,
		SYS_INFO_NAME = 0,
		SYS_INFO_COUNT = 1,
		SYS_INFO_START_TIME = 2,
		SYS_INFO_END_TIME = 3,
		SYS_INFO_ALL_TIME = 4,
		SYS_INFO_DESCRIPT = 5,
		SYS_INFO_TABLE_MAX_CELL_NUM = 6
	};
	string NULL_STRING;  //定义一些需要传递地址的空字串
	
//    //公共函数
//    AvartarDB_Base(string db_filename);
//    AvartarDB_Base(char *xml_content,int filename_xml,string save_path);
//    bool AvartarDB_Init();
//    ~AvartarDB_Base();
    AvartarDB_Base(){
        //
        QFlags.insert(make_pair("READ", ACTION_READ));
        QFlags.insert(make_pair("WRITE", ACTION_WRITE));
        QFlags.insert(make_pair("LOGIN", ACTION_LOGIN));
        QFlags.insert(make_pair("CHGPWD", ACTION_LOGIN));
        QFlags.insert(make_pair("BITIDS", BB_TABLE_REC_WR_BITIDS));
        QFlags.insert(make_pair("COLUMN_IDS", BB_TABLE_REC_WR_BITIDS));
        QFlags.insert(make_pair("BITMAP", BB_TABLE_REC_WR_BITMAP));
        QFlags.insert(make_pair("BITSTARTEND", BB_TABLE_REC_WR_BITSTARTEND));
        QFlags.insert(make_pair("CLR_UPLOADFLAG", BB_TABLE_REC_WR_QUERY_UPLOADFLAG));
        QFlags.insert(make_pair("UN_UPLOADFLAG", BB_TABLE_REC_WR_QUERY_UN_UPLOADFLAG));
        QFlags.insert(make_pair("OPERATE_OR", BB_TABLE_PUT_OPERATE_OR)); //ly table_sync3
        QFlags.insert(make_pair("OPERATE_AND", BB_TABLE_PUT_OPERATE_AND)); //ly table_sync3
        QFlags.insert(make_pair("OPERATE_SET", BB_TABLE_PUT_OPERATE_SET)); //ly table_sync3
        QFlags.insert(make_pair("BITMAP_CONVERT", 0));
        QFlags.insert(make_pair("CONDITIONS", BB_TABLE_REC_WR_QUERY_CONDITIONS));
        QFlags.insert(make_pair("CURSOR", BB_TABLE_REC_WR_QUERY_CURSOR));
        QFlags.insert(make_pair("CONVERT", BB_TABLE_REC_WR_BITIDS_CONVERT));
        //"ACT|TID""READ",0x00000001],["PATH|BITMAP|CONDITIONS|CONVERT"
        QFlags.insert(make_pair("PATH", 0));
        QFlags.insert(make_pair("NODEID", 0));
        QFlags.insert(make_pair("CONTENT", 0));
    };
    ~AvartarDB_Base(){
    };
	int FUNCTION(ParseXml_Node parse_node, PV_AddParas_T &add_paras){
		return 0;
	};
//    bool _AvartarNewObj_GetAttr(BB_OBJ **r_obj,ParseXml_Node &parse_node, string &parse_path, const char **check_point,const char **obj_type,BYTE &open_mode,BB_SIZE_T &max_len);
//    bool AvartarNewObj_CheckType(const char *obj_type, BYTE &type);
//    bool _AvartarNewObj_GetSonNodeObjs(ParseXml_Node &parse_node, string &parse_path,BYTE cell_type, string &data, BYTE method);
//    string _AvartarNewObj_GetSonNodeObjs_GetSavePath(string src,int n);
//    bool AvartarNewObj_SortObjFieldInit(BB_OBJ *table_obj,BYTE field_id); //ly sort;
	BB_SIZE_T CutTableNodeidStr(string &index_cut_str,byte* buf, string &tableindex);
	int strTableToBinary(string & dest,string &src, BB_OBJ_STRU &cell_obj_stru, string split_row,PV_AddParas_T &add_paras);
	int strTableToBinary(string & dest,string &src, BB_OBJ_STRU &cell_obj_stru,PV_AddParas_T &add_paras);
	int strArrayToBinary(string & dest,string &src, BB_OBJ_STRU &cell_obj_stru,PV_AddParas_T &add_paras);
	int ParseToBinary_NextByPass(string &dest,string &tempContent, char* &start, char* &next, char *end, BB_OBJ_STRU *cell_obj_stru, PV_AddParas_T &add_paras,int rit=0,int deep=0, int rec = 0);
	int strArrayToBinary(string & dest,const char *src_str, BB_OBJ_STRU &cell_obj_stru,PV_AddParas_T &add_paras);
	int strArrayToBinary(string & dest, string &src, PV_AddParas_T &add_paras);
	int TableCfgArrayToBinary(string & dest, string &src_str, BB_OBJ_STRU *cell_obj_stru, PV_AddParas_T &add_paras);
	bool strCellToBinary(string & dest,string &src_str, BYTE field_type, BB_SIZE_T field_size,PV_AddParas_T &add_paras);
	int JsonArrayToBArray(string &src_str, string & dest, PV_AddParas_T &add_paras,int rit=0);//Barray数组操作[]
	int FuncArrayToBArray(string &src_str, string & dest, PV_AddParas_T &add_paras, int rit = 0);//Barray数组操作[]
	int BArrayToJsonArray(BB_OBJ *src_obj, string & dest, PV_AddParas_T &add_paras);//Barray数组操作[]
	int JsonBArrayToBArray(string &src_str, string & dest, PV_AddParas_T &add_paras,int rit=0,int deep=0,int rec=0);//Barray数组操作
	int JsonQueryToBArray(string &src_str, string & dest, PV_AddParas_T &add_paras);//JsonQuery解析操作
	BYTE _JsonQueryToBArray_Define(const char *obj_type);
	bool JsonCellToObjStr(string &src_str, string &dest, PV_AddParas_T &add_paras);
//    int WriteBufferToDB_OneNode(byte* buf, BB_SIZE_T &bufoffset,string &tableindex, string &nodeid,string bitmap, BYTE method);//is_offset:两种传参bitmap,offset_len模式
//    bool WriteBufferToDB_AllNodes(byte* buf, UInt32 len, string &tableindex, string bitmap, BYTE method,string conditions);
//    BYTE BuildBufferFromDB_OneNode(string &buf_str, string &tableindex, string &nodeid_str, string bitmap_str, AVARTDB_AUTH_MASK_TYPE access_mask, BYTE method=0,bool add_nodeid=true);
//    BYTE BuildBufferFromDB_AllNode(string &buf_str,string &tableindex, string &bitmap_str, string &condition, string sursor, AVARTDB_AUTH_MASK_TYPE access_mask,BYTE method=0,bool add_nodeid=true,bool group_result=false);
//    BYTE BuildBufferFromDB_AddNodeID(string &index_str,BB_OBJ_STRU table_obj_stru, BB_SIZE_T num, BYTE method);
	//系统信息队列
//    BB_OBJ_STRU SysInfoTableStru;
//    BB_OBJ *GetSysInfoTable();
//    bool SysInfoTimeTrack(const char *name, BYTE method, UInt64 this_time);
//    //系统队列
//    BB_OBJ_STRU SysQueueTableStru;
//    BB_OBJ_STRU *GetSysQueueTableStru();
	
	typedef struct{
		//函数说明
		//BB_OBJ *packet_head;
		BYTE state;
		BYTE flag;
		BYTE add_flag;
		BYTE act;
		BYTE packet_type;
		BB_OBJ *task_id_obj;
		BB_OBJ *session_obj;
		string auth_mark;
		//UInt64 para_mark;
		//string test_str;
		//函数体
		BB_OBJ *input_obj;
		BB_OBJ_STRU input_stru;
		BB_OBJ_STRU add_info_stru;
		vector<string>  paras;
	}PARSE_BUFF_STRU;

	bool CreateDir(string path);
	bool _GetConvertToRequertTable(string &src_table_path,BB_OBJ *convert_table_obj,string &bitmapstr,BB_OBJ **request_table_obj);
	//std::string GetNodeParseValue(string tableindex,string nodeid_str, string &r_parse_value,const char *split_str);
	int JennicDataParseLen(byte *format_offset_reportflag);
	string JennicDataParseValue(string fmt_str, string data_str);
//    bool _BuildBufferFromDB_ReturnTableHead(BB_OBJ *table_obj,BB_OBJ *convert_table,string &bitmapstr,BB_OBJ **return_obj);
	bool SaveDB(string &tableindex,  byte open_mode = FileQueue::OPEN_MODE_NORMAL);
	bool SaveDB(BB_OBJ *table_obj,  byte open_mode = FileQueue::OPEN_MODE_NORMAL);
	bool SaveDB(BB_OBJ_STRU &table_obj_stru, byte open_mode = FileQueue::OPEN_MODE_NORMAL);
	bool SaveDBNode(string &tableindex, string &node_id, byte save_flag = FileQueue::SAVE_SYNC);
	bool SaveDBNode(BB_OBJ_STRU &table_obj_stru, string &node_id, byte save_flag = FileQueue::SAVE_SYNC);
	bool DBGetSaveFileName(BB_OBJ_STRU &table_obj_stru, string &file_name);
	bool DBCommitAll(bool debug=true);
	bool DBCommitAll(BB_OBJ_STRU &table_obj_stru);
	bool LoadDB(BB_OBJ *add_info,BYTE **obj_load,BB_SIZE_T &length,byte open_mode=0);
//    int LoadExt(BB_OBJ_STRU &table_obj_stru, string &nodeid_str, string &ext_name,BYTE **buf, BB_SIZE_T &start, BB_SIZE_T &length, byte open_mode=0, bool from_start=true);
//    int SaveExt(BB_OBJ_STRU &table_obj_stru, string &nodeid_str, string &ext_name, BYTE *buf, BB_SIZE_T &start, BB_SIZE_T &length, byte open_mode=0);
//    int CreatDirExt(const char *,string);
    
	bool TestFFFF(string &test_str);
    bool TestByteAll(BYTE *p, BB_SIZE_T len, BYTE cmp_chr);
	bool TestFFFF(UInt64 test_num, BB_SIZE_T len);
	bool TestMask(string & s1, string s2);
	bool TestMask(const char *s1, BB_SIZE_T s1_len, const char *s2, BB_SIZE_T s2_len);
//    BB_OBJ *AvartarNewObj(string parse_path);
//    BB_OBJ *AvartarNewObj(string parse_path,string &content);

/******各类STL 类的BB_BASE操作扩充*******/
	//整体和查询类
	bool AddSearchCell(string &search_str, BYTE search_type, const char *new_cell, BYTE new_cell_size);
	bool AddSearchCell(string &search_str, BYTE search_type, string new_cell);
	bool GetTableObject(string &search_str, BB_OBJ **obj, BB_SIZE_T &obj_size);
	bool GetTableRedirValue(BB_OBJ *obj, BB_SIZE_T obj_size, string &ret_str);
	bool GetTableRedirObj(BB_OBJ *obj, BB_SIZE_T obj_size, BB_OBJ **value_obj, BB_SIZE_T &value_obj_size);
    bool GetHeaderStrObj(string &search_str, string &header);
    bool CreateArray_H2C(string &header_search_str,const char *src_content,BB_SIZE_T src_content_size,BB_OBJ **dest_obj, BB_SIZE_T &dest_obj_size);
	//AddInfo类
	string GetAddInfo(BB_OBJ *add_info_obj, string key);
	string GetAddInfo(string &tableindex, string key);
	bool GetAddInfoUINT(string &tableindex, const char *key, UInt64 &num);
	bool GetAddInfoUINT(BB_OBJ *add_info_obj, const char *key, UInt64 &num);
    //UInt64 GetAddInfoUINT(BB_OBJ *add_info_obj, const char *key);
	bool SetAddInfoUINT(string &tableindex, const char *key, UInt64 num);
	bool SetAddInfoUINT(BB_OBJ *add_info_obj, const char *key, UInt64 num);
	bool GetAddInfoObj(string &tableindex, const char *key, BB_OBJ **item, BB_SIZE_T *item_size);
	bool GetAddInfoObj(BB_OBJ *add_info_obj, const char *key, BB_OBJ **item, BB_SIZE_T *item_size);
    bool GetAddInfoString(BB_OBJ *add_info_obj, const char *key, string &dest);
	//数组类
	bool GetArrayRecString(BB_OBJ_STRU &obj_stru, BB_SIZE_T num, string &r_str);
    bool GetArrayRecsVector(BB_OBJ_STRU &obj_stru, vector<string> &vec);
    bool GetArrayRecsAddStr(BB_OBJ_STRU &obj_stru, string &r);
	bool GetArrayRecUINT(BB_OBJ_STRU &obj_stru, BB_SIZE_T num, UInt64 &ret);
    //cell 类
    int GetTableCellName2ID(BB_OBJ_STRU &cell_obj_stru,const char *field_str); //ly cell_name
    string ConvertBitmap_ids2Bitmap(string &bitmap_str_ids);

	//表格类
	BB_SIZE_T GetTableRecBySortKey(BB_OBJ *table_obj, BYTE field_id, BYTE * field_value, BB_SIZE_T field_value_len); //查找索引信息
	BB_SIZE_T GetTableRecBySortKey(string &tableindex, BYTE field_id, const char * field_value, BB_SIZE_T field_value_len);
	UInt64 GetTableItemUINT(BB_OBJ_STRU *table_obj_stru, BB_SIZE_T rec, BYTE col);
	bool SetTableItemUINT(BB_OBJ_STRU *table_obj_stru, BB_SIZE_T rec, BYTE col, UInt64 num);
	UInt64 GetTableItemObj_UINT(BB_OBJ_STRU *table_obj_stru, BB_SIZE_T rec, BYTE col);
	string GetTableItemString(BB_OBJ_STRU *table_obj_stru, BB_SIZE_T rec, BYTE col);
	string GetTableItemObj_String(BB_OBJ_STRU *table_obj_stru, BB_SIZE_T rec, BYTE col);
	void *GetTableItemPoint(BB_OBJ_STRU *table_obj_stru, BB_SIZE_T rec, BYTE col);
	string GetObjDataStr(string &obj_str) {
		BB_OBJ *obj = (BB_OBJ *)obj_str.data();
		if (obj != NULL)
			return string((char *)BB_GET_OBJ_DATA(obj), BB_GET_OBJ_DATA_LEN(obj));
		else
			return string("");
	}

	string BaseSavePath;
	bool Loaded;
	//string ErrorStr;
protected:
	int way;
//    std::map<std::string, BYTE> DBTypes;
    std::map<std::string, BYTE> QFlags;
	BB_OBJ *rootObj;
//    BB_SIZE_T DoRegTable_Rec; //DoRegTable_New的启示位置。
	FileQueue FQueue; //文件队列
	AvartarSortObj *SortObj; //排序对象

};

#endif
