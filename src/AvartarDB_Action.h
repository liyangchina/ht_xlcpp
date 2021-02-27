#ifndef AVARTARDB_ACTION_H_
#define AVARTARDB_ACTION_H_
//#include "ConnQueue.h"
#include "AvartarDB_Base.h"

class AvartarDB_Action : public AvartarDB_Base{
public:
    enum{
        ACTION_ERROR_PACKET_HEAD_ACT_NOT_SUPPORT=2,
        ACTION_ERROR_PACKET_HEAD_PARAS_NUM_ERROR=3,
        ACTION_ERROR_PACKET_PARAS_NUM_ERROR=4,
        ACTION_ERROR_PACKET_PARAS0_TABLE_IS_NOT_HEX_BYTES=5,
        ACTION_ERROR_PACKET_PARAS0_TABLE_NOT_FOUND=6,
        ACTION_ERROR_PACKET_PARSE_OBJ_LEN_LARGER_THEN_INPUT_STR_LEN = 0x60,
        ACTION_ERROR_PACKET_PARSE_OBJ_NUM_LARGER_THEN_MAX_NUM_OR_STRU_ERROR = 0x61,
        ACTION_ERROR_PACKET_PARSE_OBJ_IS_NOT_ARRAY_OBJ = 0x62,
        ACTION_ERROR_PACKET_PARSE_HEAD_STR_IS_NOT_3_OR_0 = 0x63,
        ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_IS_NOT_ARRAY = 0x64,
        ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_IS_NOT_ARRAY = 0x65,
        ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_GET_HEAD_FLAG_ERROR = 0x66,
        ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_HEAD_FLAG_SIZE_IS_NOT_3 = 0x67,
        ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_GET_HEAD_FLAG_IS_INCORRECT = 0x68,
        ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_IS_NOT_DEFINED = 0x69,
        ACTION_ERROR_PACKET_PARSE_GET_PARAS_OBJ_PROBLEM = 0x6a,
        ACTION_ERROR_PACKET_PARSE_GET_PARAS_OBJ_CAN_NOT_REDIR = 0x6b,
        ACTION_ERROR_PACKET_PARSE_GET_PARAS_ADDINFO_SESSION_IS_NOT_UINT = 0x6c,
        ACTION_ERROR_PACKET_PARSE_DEFAULT_0X6C = 0x6d,
        ACTION_ERROR_PACKET_PARAS0_TABLE_PATH_ERROR = 0x6e,
        ACTION_ERROR_PACKET_UNKNOW_ACTION = 0x6f,
        ACTION_ERROR_PACKET_PARAS_BITMAP_ARRAY_CELL_IS_NOT_UINT8 = 0x70,
        ACTION_ERROR_PACKET_PARAS_WFORMATE_IS_NOT_BYTES_4 = 0x71,
        ACTION_ERROR_PACKET_PARAS_BITMAP_INDEX_MUST_SET_AS_FIRST_CELL =0x72,
        ACTION_ERROR_PACKET_PARAS_BITMAP_IS_NO_UINT8_STRING = 0x73,
        ACTION_ERROR_PACKET_PARAS_BITMAP_IS_NO_JFIELD_STRING = 0x74,
        ACTION_ERROR_READ_CONVERT_TABLE_NOT_FOUND=7,
        ACTION_ERROR_READ_RESULT_IS_LARGER_THEN_256_16=8,
        ACTION_ERROR_READ_TASK_ID_IS_NOT_OBJ=9,
        ACTION_ERROR_READ_CONVERT_TABLE_ACT_ERROR=0xa,
        ACTION_ERROR_READ_PARAS_CONDITIONS_IS_OUT_OF_PARAS_SIZE=0xb,
        ACTION_ERROR_READ_PARAS_CONDITIONS_IS_NOT_NULL_BYTES_OR_BYTE_EXT=0xc,
        ACTION_ERROR_READ_PARAS_NODEIDS_IS_OUT_OF_PARAS_SIZE=0xd,
        ACTION_ERROR_READ_PARAS_NODEID_IS_NOT_NULL_BYTES_OR_BYTE_EXT=0xe,
        ACTION_ERROR_READ_PARAS_NODEID_IS_IS_NOT_FOUND_IN_DB = 0xf,
        ACTION_ERROR_READ_PARAS_CURSOR_IS_OUT_OF_PARAS_SIZE=0x10,
        ACTION_ERROR_READ_PARAS_CONVERT_IS_OUT_OF_PARAS_SIZE=0x11,
        ACTION_ERROR_READ_PARAS_BITMAP_CELL_IS_OUT_OF_CELL_TABLA_NUM = 0x12,
        ACTION_ERROR_READ_PARAS_NODEID_IS_NOT_BYTES_ARRAYS=0x13,
        ACTION_ERROR_READ_PARAS_CURSOR_IS_INCORRECT1 = 0x2b,
        
        ACTION_ERROR_READ_BUILDER_CAN_NOT_GET_TABLE=0x20,
        ACTION_ERROR_READ_BUILDER_CURSOR_MUST_6_BYTE_LEN=0x21,
        ACTION_ERROR_READ_BUILDER_GET_CONDITIONS_KEY_ERROR=0x22,
        ACTION_ERROR_READ_BUILDER_GET_TABLE_ITEM_ERROR_BY_CONDITIONS_KEY=0x23,
        ACTION_ERROR_READ_BUILDER_PUT_TABLE_ITEM_ERROR_BY_UPLOAD_FLAG=0x24,
        ACTION_ERROR_READ_BUILDER_READ_TABLE_BUFF_BY_NUM_ERROR=0x25,
        ACTION_ERROR_READ_BUILDER_ADD_NODEID_INDEX_ERROR=0x26,
        ACTION_ERROR_READ_BUILDER_BUILD_RETURN_TABLE_HEAD_ERROR=0x27,
        ACTION_ERROR_READ_BUILDER_NODEID_NOT_FOUND = 0x28,
        ACTION_ERROR_READ_BUILDER_NODEID_NO_AUTH = 0x29,
        ACTION_ERROR_READ_BUILDER_DESTOBJ_IS_NOT_TABLE = 0x2a,
        ACTION_ERROR_READ_BUILDER_FATLE = 0x2b,
        
        
        //ACTION_ERROR_WRITE_PARAS_BITMAP_ARRAY_CELL_IS_NOT_UINT8=0x2c,
        //ACTION_ERROR_WRITE_PARAS_WFORMATE_IS_NOT_BYTES_4=0x07,
        ACTION_ERROR_WRITE_PARAS_CONDITIONS_IS_NOT_NULL_BYTES_BYTE_EXT=0x08,
        ACTION_ERROR_WRITE_PARAS_CONDITIONS_IS_NOT_ARRAY_PARA_EXT=0x09,
        ACTION_ERROR_WRITE_PARAS3_DATA_IS_NOT_ARRAY_BYTE=0x0a,
        ACTION_ERROR_WRITE_PARAS3_DATA_IS_NOT_ARRAY_BYTES_BYTES_EXT=0x0b,
        ACTION_ERROR_WRITE_ACTION_BUFOFFSET_OUTOFF_RANGE=0x0d,
        ACTION_ERROR_WRITE_PARAS_IS_TOO_MANY=0x0e,
        ACTION_ERROR_WRITE_RETURN_TABLE_WFORMATE_IS_NOT_ARRAY=0x0f,
        ACTION_ERROR_WRITE_RETURN_TABLE_PARA_NUM_ERROR=0x10,
        ACTION_ERROR_WRITE_DATA_PARA_NUM_ERROR=0x11,
        ACTION_ERROR_WRITE_RETURN_TABLE_WFORMATE_IS_NOT_PACKET_IN_BYTES_EXT=0x12,
        ACTION_ERROR_WRITE_PARAS3_NODEID_IS_NOT_BB_BYTES=0x13,
        ACTION_ERROR_WRITE_PARAS4_DATA_IS_NOT_ARRAY=0x14,
        ACTION_ERROR_WRITE_BITIDS_PARA_NUM_ERROR = 0x15,
        ACTION_ERROR_WRITE_ACTION_ONE_NODE_ERROR=0x20,
        ACTION_ERROR_WRITE_ACTION_NODEID_IS_EMPTY=0x22,
        ACTION_ERROR_WRITE_ACTION_CYCLE_ADD_NOT_ADDINFO_DEFINED=0x23, //ly cycle; ly task
        ACTION_ERROR_WRITE_ACTION_CYCLE_ADD_NOT_INDEX_FIELD_DEFINED=0x24, //ly cycle; ly task
        ACTION_ERROR_WRITE_ACTION_CYCLE_ADD_NO_MAX_LEN_DEFINED=0x25,
        ACTION_ERROR_WRITE_ACTION_ERROR_BY_CONDITIONS_KEY=0x26,
        ACTION_ERROR_WRITE_ACTION_CHECK_CONDITIONS_KEY_FALSE=0x27,
        ACTION_ERROR_WRITE_ACTION_CONDITIONS_ONLY_FOR_UINT_INDEX=0x28,
        //inside write to func add.
        ACTION_ERROR_DELETE_PARAS1_IS_NOT_UINT_BYTES_ARRAY=7,
        ACTION_ERROR_DELETE_ID_NOT_FOUND=8,
        ACTION_ERROR_DELETE_IDS_NOT_FOUND=9,
        ACTION_ERROR_DELETE_IDS_INDEX_IS_NATURL_ADD = 10,
        ACTION_ERROR_DELETE_PARENT_OBJ_NOT_FOUND=11,
        ACTION_ERROR_CLONE_XML_SRC_PATH_NOT_EXIT=7,
        ACTION_ERROR_CLONE_XML_DEST_PARENT_NOT_EXIT=8,
        ACTION_ERROR_CLONE_XML_DEST_NODE_ALREADY_EXT=9,
        ACTION_ERROR_CLONE_XML_DEST_PARENT_INDEX_CFG_ERROR=10,
        ACTION_ERROR_DELETE_XML_CLUSTER_ARRAY_ID_NOT_FOUND=7,
        ACTION_ERROR_DELETE_XML_CLUSTERL_DEST_PARENT_NOT_EXIT=8,
        ACTION_ERROR_DELETE_XML_CLUSTERL_DEST_NODE_NOT_EXIT=9,
        ACTION_ERROR_DELETE_XML_CLUSTERL_DEL_XMLNODE_ERROR=10,
        ACTION_ERROR_DELETE_XML_DEST_PARENT_INDEX_CFG_ERROR=11,
        ACTION_ERROR_PACKET_PARAS_GET_OBJ_ERROR=0xF0,
        ACTION_ERROR_GET_CLUSTER_INFO_OBJECT_NOT_EXIT=0x20,
        ACTION_ERROR_GET_CLUSTER_INFO_ADD_INFO_NAME_NOT_EXIT=0x21,
        ACTION_ERROR_GET_CLUSTER_INFO_OBJECT_IS_NOT_ARRAY = 0x22,
        ACTION_ERROR_GET_CLUSTER_INFO_OBJECT_NO_AUTH = 0x23,
        ACTION_ERROR_EXT_WRITE_DEVICE_CAN_NOT_OPEN_DEST_ROUTER = 0x20,
        ACTION_ERROR_EXT_DEVICE_ADDINFO_SAVE_FILE_PATH_NOT_DEFINE = 0x21,
        ACTION_ERROR_EXT_DEVICE_EXT_FILE_NOT_FOUND = 0x22,
        ACTION_ERROR_EXT_DEVICE_CAN_NOT_OPEN_FILE = 0x23,
        ACTION_ERROR_EXT_DEVICE_IS_OUT_OFF_RANGE=0x24,
        ACTION_ERROR_EXT_METHOD_IS_NOT_OBJ_BYTES_8 =0x25,
        ACTION_ERROR_EXT_NODEID_IS_NOT_BB_BYTES=0x26,
        //
        ACTION_GET_NODE_PATH_OBJ_NAME=0x03
    };
	//AvartarDB_Action(string sensor_db_xml);
	/*AvartarDB_Action(string sensor_db_xml):AvartarDB_Base(sensor_db_xml) {
		// TODO Auto-generated constructor stub

	}
	AvartarDB_Action(char* xml, int filename_or_xml,string save_path):AvartarDB_Base(xml,filename_or_xml,save_path) {
		// TODO Auto-generated constructor stub

	}
	~AvartarDB_Action(){

	};*/
    
    //公共函数
    AvartarDB_Action(string db_filename);
    AvartarDB_Action(char *xml_content,int filename_xml,string save_path);
    bool AvartarDB_Init();
    ~AvartarDB_Action();
    //NewObj层次对象生成函数
    BB_OBJ *AvartarNewObj(string parse_path);
    BB_OBJ *AvartarNewObj(string parse_path,string &content);
    bool _AvartarNewObj_GetAttr(BB_OBJ **r_obj,ParseXml_Node &parse_node, string &parse_path, const char **check_point,const char **obj_type,BYTE &open_mode,BB_SIZE_T &max_len);
    bool AvartarNewObj_CheckType(const char *obj_type, BYTE &type);
    bool _AvartarNewObj_GetSonNodeObjs(ParseXml_Node &parse_node, string &parse_path,BYTE cell_type, string &data, BYTE method);
    bool _AvartarNewObj_GetIndexTableObjs(BB_OBJ_STRU &index_stru,string &data);
    BB_OBJ *AvartarNewObj_GetIndexTableObjs_AppendNode(BB_OBJ_STRU &index_stru,UInt64 num,string &index_table_dir_model,string &index_table_dir,bool xml_remain=false);
    string _AvartarNewObj_GetSonNodeObjs_GetSavePath(string src,int n);
    bool AvartarNewObj_SortObjFieldInit(BB_OBJ *table_obj,BYTE field_id); //ly sort;
    bool AvartarNewObj_LoadExtField(BB_OBJ *table_obj, string &save_ext_path);

    //系统信息队列
    BB_OBJ_STRU SysInfoTableStru;
    BB_OBJ *GetSysInfoTable();
    bool SysInfoTimeTrack(const char *name, BYTE method, UInt64 this_time);
    //系统队列
    BB_OBJ_STRU SysQueueTableStru;
    BB_OBJ_STRU *GetSysQueueTableStru();
    
    //DBTable读写处理函数
    //json 操作类函数
	BYTE ParseActionMessage(string &packet_head,string &input_str, PARSE_BUFF_STRU &input_stru,PV_AddParas_T &add_paras);
	void ParseActionMessage_Debug(PARSE_BUFF_STRU &obj);
	bool _ParseActionMessage_ActFlag(string &act_flag, BYTE *flag1, BYTE *flag2=NULL);
	bool _ParseActionMessage_Define(const char *obj_type, BYTE &flag);
	BYTE BuildResultMessage(PARSE_BUFF_STRU &act_paras,string &result_str, PV_AddParas_T &add_paras);//直接从act_paras.obj中创新返回数据包
	BYTE BuildResultMessageVector(PARSE_BUFF_STRU &act_paras, vector<string> &send_data, string &result_str, PV_AddParas_T &add_paras, bool is_add_paras = false, bool is_add_bytes_obj = true);//以send_data为数据，从act_paras.obj中创新返回数据包.
	BYTE BuildResultMessageArray(PARSE_BUFF_STRU &obj, string &result_str, string &send_msg,bool add_header,PV_AddParas_T &add_paras);//以send_data为数据，从act_paras.obj中创新返回数据包.
	//BYTE ParseActionMessage_Check(string input_str);
	BYTE DoActionMessage(string &packet_head,string &input_str, const char *add_flag,string &func_return,PV_AddParas_T &add_paras);
	BYTE DoActionParas(PARSE_BUFF_STRU &parse, string &func_return,PV_AddParas_T &add_paras);
	BYTE DoActionParas_GetBitmap(BB_OBJ *src_bitmap_obj, string &bitmap_str, BYTE &add_flag);
    BYTE DoActionParas_GetBitmapExt(BB_OBJ_STRU &cell_obj_table,BB_OBJ *src_bitmap_obj, string &bitmap_str, BYTE &add_flag);
    //int GetTableCellName2ID(BB_OBJ_STRU &cell_obj_stru,const char *field_str);
	BYTE DoActionParas_GetTableIndex(BB_OBJ *src_table_obj, string &tableindex);
    BYTE DoActionParas_CheckConditions(BB_OBJ_STRU *table_stru, BB_SIZE_T num, string &conditions);
    BYTE GetPathTableIndex(string &temp1, string &tableindex);
    BYTE CheckTableIndexRedir(BB_OBJ_STRU *table_stru, string &tableindex);
    BB_OBJ *CheckGetTableRedir(string &tableindex, BB_OBJ_STRU &table_stru);
    int CheckGetTableFilter(BB_OBJ_STRU &table_stru,string &conditions);
    BYTE GetPathAllIndex(string &temp1, string &tableindex);
    bool GetTableObjectByPath(string &path_str, BB_OBJ **obj, BB_SIZE_T &obj_size);
    BYTE GetAvartarObjectByNamePath(string &path_str, BB_OBJ **obj,BB_SIZE_T *obj_size);
    bool GetArrayObjectByName(BB_OBJ *obj, string &name,BB_OBJ **item_obj, BB_SIZE_T *item_obj_size);
	BYTE DoActionParas_GetCursor(BB_OBJ *src_sursor_obj, string &cursor_str);
	BYTE DoActionParas_SonAddInfo(string &buf_str, string &tableindex, string &bitmap_str, string &conditions, string cursor, AVARTDB_AUTH_MASK_TYPE access_mask, BYTE method, bool add_nodeid = true,bool group_result=true);
	BYTE DoActionParas_ACT_INFO(BB_OBJ_STRU &table_stru, string &bitmap_str, AVARTDB_AUTH_MASK_TYPE access_mask, string &func_return, int num=-1);
    BYTE DoActionParas_ACT_INFO_BY_NAME(BB_OBJ_STRU &table_stru, string &key_name, AVARTDB_AUTH_MASK_TYPE access_mask, string &func_return, int num=-1);
    
    //读写二维表格
    int WriteBufferToDB_OneNode(byte* buf, BB_SIZE_T &bufoffset,string &tableindex, string &nodeid,string bitmap, BYTE method);//is_offset:两种传参bitmap,offset_len模式
    void WriteBufferToDB_OneNode_SaveExtField(BB_OBJ_STRU &obj_stru,BB_SIZE_T num,string &index_str,string &ext_name,BYTE method);
    int WriteDataToDB_OneField(BYTE *data, BB_SIZE_T data_size, string &tableindex, string &nodeid, string &field_id);
    bool WriteBufferToDB_AllNodes(byte* buf, UInt32 len, string &tableindex, string bitmap, BYTE method,string conditions);
    BYTE BuildBufferFromDB_OneNode(string &buf_str, string &tableindex, string &nodeid_str, string bitmap_str, AVARTDB_AUTH_MASK_TYPE access_mask, BYTE method=0,bool add_nodeid=true);
    BYTE BuildBufferFromDB_AllNode(string &buf_str,string &tableindex, string &bitmap_str, string &condition, string sursor, AVARTDB_AUTH_MASK_TYPE access_mask,BYTE method=0,bool add_nodeid=true,bool group_result=false);
    bool BuildBuffer_TableCnvNumToNodeID(BB_OBJ_STRU table_obj_stru,BB_SIZE_T num,string &index_str,bool cnv_to_obj=false);
    BYTE BuildBufferFromDB_AddNodeID(string &index_str,BB_OBJ_STRU table_obj_stru, BB_SIZE_T num, BYTE method);
    int DeleteFromDB_OneNode(BB_OBJ_STRU &table_stru,string node_id);
    bool _BuildBufferFromDB_ReturnTableHead(BB_OBJ *table_obj,BB_OBJ *convert_table,string &bitmapstr,BB_OBJ **return_obj);
    string GetNodeParseValue(string tableindex,string nodeid_str, string &r_parse_value,const char *split_str);
    int CreateTableIndexStr(BB_OBJ_STRU *table_obj_stru, UInt64 index, string &rindex);
    
    //存盘处理函数
    int LoadExt(BB_OBJ_STRU &table_obj_stru, string &nodeid_str, string &ext_name,BYTE **buf, BB_SIZE_T &start, BB_SIZE_T &length, byte open_mode=0, bool from_start=true);
    string GetExtPathName(BB_OBJ *AddInfoObj,string &node_id,string ext_name);
    int SaveExt(BB_OBJ_STRU &table_obj_stru, string &nodeid_str, string &ext_name, BYTE *buf, BB_SIZE_T &start, BB_SIZE_T &length, byte open_mode=0);
    
    //其他
    typedef struct{
        //函数说明
        BYTE param;
        bool has_param_ext;
        BYTE param_ext;
        BYTE *data;
        BYTE *obj;
        BB_SIZE_T data_len;
        BB_SIZE_T max_num;
        bool is_changed=false;
        BB_SIZE_T bit_step;
        BB_SIZE_T byte_step;
        BYTE type;
    }BIT_SEARCH_STRU; //ly rec_chg_mark
    
    bool GetRecChgMarkObj(BB_OBJ *add_info_obj, const char *key,BIT_SEARCH_STRU *rec_chg_stru,BB_SIZE_T max_num=0); //ly rec_chg_mark
private:
    std::map<std::string, BYTE> DBTypes;
    string uploadflag_str={(const char)0x80,0x01,0x0C,0x00,0x4B,0x40,0x55,0x50,0x4C,0x4F,0x41,0x44,0x46,0x4C,0x41,0x47}; //["@UPLOADFLAG"]
    string rec_chg_mark_str={(const char)0x80,0x01,0x0C,0x00,0x4B,0x40,0x52,0x45,0x43,0x43,0x48,0x47,0x4D,0x41,0x52,0x4B}; //["@RECCHGMARK"]
    
};

#endif /* AVARTARDBACTION_H_ */
