#ifndef BYTE_BASE
#define BYTE_BASE

#include <stdio.h>
#include <string.h>
#include "Linux_Win.h"

	//#include <stdbool.h>
	#include <stdlib.h>
	#ifndef FALSE
	#define FALSE               0
	#endif

	#ifndef TRUE
	#define TRUE                1
	#endif

#ifndef __cplusplus
	double pow(double num,double value){
		int i;
		double r=1;
		for (i=0;i<num;i++){
			r=r*value;
		}
		return r;
	}
#endif
#ifdef  __cplusplus
extern "C" {
#endif

//typedef unsigned char   BYTE;
typedef BYTE BB_OBJ;
typedef size_t BB_SIZE_T;
#define BB_SIZE_T_ERROR 0xFFFFFFFF
#define BB_UINT64_F 0xffffffffffffffff
#define BB_OBJ_STX_LEN 0x1
#define BB_OBJ_STX_EXT_LEN 0x2
#define BB_OBJ_STX_EXT2_LEN 0x3
//#define BB_DIM_MASK 0x80

//0 dimensions

#define BB_NULL 0x00
#define BB_UINT8 0x01
#define BB_UINT16 0x02
#define BB_UINT32 0x04
#define BB_UINT64 0x08
#define BB_0DIM_UINT_LEN_MASK 0xF
#define BB_REDIR 0xe0 //#define BB_REDIR 0x60
#define BB_0DIM_REDIR_LEN_MASK 0x0F
#define BB_ADDOBJ 0xa0 //#define BB_REDIR 0x60
#define BB_0DIM_ADDOBJ_LEN_MASK 0x0F
#define BB_BYTES 0x40 
#define BB_0DIM_BYTES_LEN_MASK 0x3f
#define BB_0DIM_BYTES_MAX_LEN 64
#define BB_BYTES_EXT 0x30
#define BB_0DIM_BYTES_EXT_LEN_MASK 0xf
#define BB_0DIM_BYTES_EXT_MAX_LEN  256*16
#define BB_PARA_EXT 0x20
#define BB_0DIM_PARA_EXT_KEY_LEN_MASK 0xf
#define BB_PARA_CONDITION_COMP_MASK 0x0c
#define BB_PARA_CONDITION_GT 0x04
#define BB_PARA_CONDITION_LT 0x08
#define BB_PARA_CONDITION_NOT_EQUAL 0x0c
#define BB_PARA_CONDITION_STR_EQUAL 0x10
#define BB_PARA_CONDITION_STR_INCLUDE 0x14 //ly search2
#define BB_PARA_CONDITION_EQUAL 0x00
#define BB_CELL_UINT_WIDTH_MASK 0x03 //A4-1;
#define BB_MALLOC 0x10
#define BB_MALLOC_SAVE_MASK 0x04 //制定是否存储，如果有：则C字节存在，并指定存储路径信息长度   
#define BB_0DIM_MALLOC_SIZE_MASK 0x03    //ly malloc_obj 00:8位;01:16位;10:32位:11:64位， BB_SIZE_长度
#define BB_MALLOC_SAVE_FILE_BYTE 0x02 //malloc 文件长度定义字段,B
#define BB_MALLOC_BYTE_TYPE 0x01	 //malloc 类型定义字段
#define BB_MALLOC_TYPE_OBJ 0x02   //BB_OBJ 对象类型
#define BB_MALLOC_TYPE_QUEUE 0x1

typedef struct{
	BB_OBJ Header;
	BYTE data[8];//predefine 8 byte uint length;
}BB_UINT_STRU;

typedef struct {
	BB_OBJ Header;
	BYTE Type;
	BYTE data[8];//predefine 8 byte uint length;
}BB_MALLOC_STRU;

#define BB_OBJ_NULL 0x18
#define BB_ARRAY 0x80    //ARRAY
#define BB_TABLE 0xC0	 //TABL

//1 dimensions
typedef struct {
	BB_OBJ Header;
	BYTE Type;
	BB_OBJ *ThisObj;
	BB_OBJ *LenObj;
	BB_OBJ *CellObj;
	BB_OBJ *IndexObj;
	BB_OBJ *AddInfoObj;
	BB_OBJ *MaxLenObj;
	BYTE *Data;
	BB_SIZE_T DataLen;
	BB_SIZE_T HeaderLen;
	BB_SIZE_T Size;
	BB_SIZE_T CellWidth;
	BB_SIZE_T Num;
	BB_SIZE_T MaxLen;
	BB_SIZE_T TempLen;
	bool LoadError;
}BB_OBJ_STRU;
extern BB_OBJ_STRU BB_OBJ_STRU_NULL;
extern BB_OBJ_STRU BB_OBJ_STRU_DEFAULT;
//extern BYTE BB_FIELD_BITMAP_BASE = 0x80;  //字段位移

typedef struct {
	BB_OBJ Header;
	BYTE Type;
	BB_SIZE_T Size;
	BB_OBJ *LenObj;
	BB_OBJ *CellObj;
	BB_OBJ *IndexObj;
	BB_OBJ *AddInfoObj;
	BB_OBJ *MaxLenObj;
	void *Data;
	BB_SIZE_T DataLen;
	bool LoadError;
}BB_OBJ_STRU_ALL;

typedef struct{
	BB_OBJ Header;
	BYTE data[8];//predefine 8 byte uint length;
}BB_SEARCH_STRU;
#define BB_SEARCH_LEN_MASK 0x0f
#define BB_SEARCH_STX_LEN 0x01
#define BB_SEARCH_CONTENT_STX 0x00
#define BB_SEARCH_ADDINFO_STX 0xc0
#define BB_SEARCH_INDEX_STX 0x80
#define BB_SEARCH_CELL_STX 0x40
#define BB_SEARCH_BY_NUM 0x20 //A6=0则为Index;1:则为Num模式
#define BB_SEARCH_NULL 0x00

#define ADD_INFO_SAVE_PATH 0x00
#define BB_ARRAY_APPEND_DEFAULT_CHAR 0xFF

//TABLE_CELL Write Method记录及
#define BB_TABLE_DEFAULT_CELL_LENGTH_FIELD 1
#define BB_TABLE_DEFAULT_TYPE_FIELD 2
#define BB_TABLE_DEFAULT_SYNCINDEX_FIELD 3
#define BB_TABLE_CELL_NAME_FIELD 5
#define BB_TABLE_FIELD_TYPE_OBJECT 8
#define BB_TABLE_FIELD_TYPE_MALLOC 7
#define BB_TABLE_FIELD_TYPE_STRING 6
#define BB_TABLE_FIELD_TYPE_BINARY 2
#define BB_TABLE_FIELD_TYPE_NUMBER 1
#define BB_TABLE_FIELD_MAX_NUM 64
//WRITE_BUILDER method定义
//#define BB_TABLE_REC_WR_BITSTARTEND_COLUMN 128 //A8:横向传输/1:纵向传输：
//#define BB_TABLE_REC_WR_RETURN_OBJ 4
//#define BB_TABLE_REC_WR_RETURN_FILE 6 //A32=00:返回数据;01-返回数据表格; 10-返回path所指对象; 11—：返回path所指文件。
//-----------
#define BB_TABLE_REC_WR_QUERY_SORT 128 //A8=0:原始顺序，1：不排序
#define BB_TABLE_REC_WR_BITIDS_CONVERT 0x9F //A76=00 BITMAP加上CONVERT的处理处理形式
#define BB_TABLE_REC_WR_BITIDS 96 //A76=01:Bitmap,RMethod是bitmap的方式;10:Offset/length,RMethod是Offset/length的方式;11.按表格方式提供查询头信息（读取id数组、回传id数组、nodeid转换），同时bitmap为后续id数组宽度
#define BB_TABLE_REC_WR_BITSTARTEND  64
#define BB_TABLE_REC_WR_BITMARK  96 
#define BB_TABLE_REC_WR_BITMAP 32
#define BB_TABLE_REC_WR_QUERY_CONDITIONS  16 //A5=1:RCondition:索求符合条件的的节点(见RConditions)。
#define BB_TABLE_REC_WR_QUERY_UPLOADFLAG  8  //A4=1：只取得UpladeFlag的节点;
#define BB_TABLE_REC_WR_QUERY_CURSOR 4   //A3=0：返回所有记录；1：返回cursor节定义的纪录
#define BB_TABLE_REC_WR_RETURN_TABLE 2  //A2=0:返回数据块; 1-返回数据表
#define BB_TABLE_REC_WR_QUERY_UN_UPLOADFLAG  1  //A1=1：写入时不缺省写入UPLOADFLAG;//ly table_sync3; //ly rec_chg_mark
#define BB_TABLE_REC_WR_OBJ_DATA_VALUE 1 //A1=0：原版读取对象字段，1：读取对象值信息+长度。
#define BB_TABLE_REC_STATE_RETURN_INDEX 1 //B1=0: 返回的信息增加NodeID,只针对：
//PUT GET Method定义
#define BB_TABLE_PUT_OPERATE_OR 0x4 //A43=00 缺省替换；01:OR运算。10:AND运算, 11：XOR运算
#define BB_TABLE_PUT_OPERATE_AND 0x8
#define BB_TABLE_PUT_OPERATE_REPLACE 0x00
#define BB_TABLE_PUT_OPERATE_SET 0xc//A6=1
#define BB_TABLE_PUT_OPERATE_MARK 0x0c
#define BB_TABLE_PUT_OPERATE_INIT 0x10
#define BB_TABLE_REC_WR_CONFIRM_NODEID 128	//A8=0: 普通写入,不返回NodeID; A8=1：创建新节点，并返回新创建的NODEID
#define BB_ARRAY_NUM_WR 0
#define BB_ARRAY_NUM_WR_OBJ_WHOLE 0
#define BB_ARRAY_NUM_WR_OBJ_VALUE 1
typedef UInt32 BB_BITMAP_TYPE; //ly table_dir: typedef UInt64 BB_BITMAP_TYPE;

//--------ARRAY DEFINE---------
#define BB_MASK_IndexObj 0x10 //a5=1
#define BB_MASK_ExtArea 0x8		//a4=1
#define BB_MASK_Sort 0x4		//a3=1
#define BB_MASK_AddInfo 0x2		//a2=1
#define BB_MASK_MaxLenObj 0x1	//a1=1
#define BB_MASK_LenObj 0x0c

//error code
#define ARRAY_LEN_NUM_ERROR 1 

extern UInt16 BB_Error_Code;  //回传差错字串
extern UInt64 BB_ADD_UINT_TO_BUFF(BYTE *dest_buff, BYTE *src_buff, uint64 u64value,  BYTE cell_width);
extern bool BB_PUT_UINT_TO_BUFF(BYTE *buff,uint64 u64value,BYTE cell_width);
extern UInt64 BB_ScanUINTWidth(BYTE *pBuf,BYTE cell_width);
extern bool BB_IS_UINT(BB_OBJ *obj);
extern BYTE BB_GET_OBJ_DIM(BB_OBJ *obj);
extern BB_SIZE_T  BB_GET_OBJ_DATA_LEN(BB_OBJ *obj);
extern BYTE  BB_GET_OBJ_TYPE(BB_OBJ *obj);
extern BB_SIZE_T  BB_GET_OBJ_SIZE_CHECK(BB_OBJ *obj, BB_SIZE_T range);
extern BB_SIZE_T  BB_GET_OBJ_SIZE(BB_OBJ *obj);
extern BB_OBJ_STRU BB_GET_OBJ_STRU(BB_OBJ *obj);
extern bool BB_GET_OBJ_HEAD_STRU(BB_OBJ *obj, BB_OBJ_STRU *s_obj);
extern BYTE BB_GET_ARRAY_INDEX_OBJ_WIDTH(BB_OBJ *obj);
extern BB_SIZE_T BB_GET_ARRAY_NUM(BB_OBJ *obj);
//extern BB_SIZE_T BB_GET_ARRAY_UINT_SUM(BB_OBJ *obj);
extern bool BB_GET_ARRAY_UINT_MAX(BB_OBJ* obj,UInt64 *ret);
extern bool BB_GET_ARRAY_UINT_VALUE(BB_OBJ *obj,UInt32 i,UInt64 *ret);
extern bool BB_GET_ARRAY_UINT_VALUE_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T i, UInt64 *ret);
extern BB_SIZE_T BB_GET_OBJ_DATA_LEN(BB_OBJ *obj);
extern void BB_SET_CELL_DATA_LEN(BB_OBJ *obj,BB_SIZE_T data_len);
extern int	BB_SET_UINT_VALUE(BB_OBJ *obj,UInt64 value);
extern UInt64 BB_GET_UINT_VALUE(BB_OBJ *obj);
bool BB_GET_MALLOC_ADDRESS(BB_OBJ *obj, void **item, BYTE *type);
bool BB_SET_MALLOC_ADDRESS(BB_OBJ *obj, const void *address, BYTE address_size, BYTE type);

//0 dimension object: Header(1Byte)+Content; No MaxLen,No AddInfo,No Sort, BaseCell
//extern BB_OBJ *BB_NEW_CELL(BYTE db_type, const void *content, UInt64 content_len, BB_OBJ *p_buf);
extern BB_UINT_STRU BB_NEW_CELL_UINT(UInt64 value, BYTE db_type);
//extern BB_MALLOC_STRU BB_NEW_CELL_MALLOC(UInt64 value, BYTE db_type);

//1 dimension object: Header(1Byte)+CellObj+IndexObj+Content; AllInfoDB:Len;MaxLen,AddInfo,Sort
extern BB_OBJ *BB_NEW_ARRAY(BB_OBJ *cell_obj, BB_OBJ *index_obj, BB_OBJ *add_info_obj, BB_SIZE_T max_len, const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf);
extern BB_OBJ *BB_NEW_ARRAY_CELL(BYTE cell_type, BYTE index_type, BB_OBJ *add_info_obj, BB_SIZE_T input_max_len, const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf);
extern BB_OBJ *BB_NEW_ARRAY_INDEX_CELL(BYTE cell_type, BB_OBJ *array_obj, BB_OBJ *add_info_obj, BB_SIZE_T input_max_len, const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf);
extern BB_OBJ *BB_NEW_OBJ_STRING(const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf);
extern bool BB_IS_OBJ_STRING(BB_OBJ *item_obj);
 extern bool BB_IS_BYTES_ALL(BB_OBJ *item_obj);
extern bool BB_IS_OBJ_ARRAY_STRU(BB_OBJ_STRU *item_obj_stru);
extern BB_OBJ *BB_NEW_CELL(BYTE db_type, const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf);
extern BB_OBJ *BB_NEW_BYTES_ALL(const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf);
extern BB_OBJ *BB_NEW_PARA(BYTE key_type, UInt64 key, const void *data, BYTE data_len, BB_OBJ *p_buf);
extern BB_OBJ *BB_NEW_MALLOC(const void *address, const char* save_path, BYTE save_path_len, BYTE ext_obj_type, BB_OBJ *p_buf);
extern BB_SIZE_T BB_GET_ARRAY_NUM_STRU(BB_OBJ_STRU *obj_stru);
extern bool BB_GET_ARRAY_OBJ_NUM_CHECK_STRU(BB_OBJ_STRU *obj_stru,BB_SIZE_T *num, BB_SIZE_T max);
extern bool BB_GET_OBJ_BY_PATH(BB_OBJ *sobj, BB_OBJ *search_str_data, int size_search_str, BYTE **obj, BB_SIZE_T *obj_size);
extern bool BB_GET_OBJ_BY_PATH_ALL(BB_OBJ *sobj, BB_OBJ *search_str_data, int size_search_str, BYTE **obj, BB_SIZE_T *obj_size);
extern bool BB_GET_ARRAY_REC_BY_NUM(BB_OBJ *obj,BB_SIZE_T num, BYTE **item, BB_SIZE_T *item_size);
extern bool BB_GET_ARRAY_REC_BY_NUM_STRU(BB_OBJ_STRU *obj_stru,BB_SIZE_T num, BYTE **item, BB_SIZE_T *item_size);
extern bool BB_GET_ARRAY_BY_NUM_NOREDIR_STRU(BB_OBJ_STRU *obj_stru,BB_SIZE_T num, BYTE **item, BB_SIZE_T *item_size);
extern bool BB_GET_ARRAY_OBJ_BY_NUM_STRU(BB_OBJ_STRU *obj_stru,BB_SIZE_T num, BB_OBJ **item, BB_SIZE_T *item_size);//ly table_dir
extern bool BB_GET_ARRAY_OBJ_BY_NUM(BB_OBJ *obj,BB_SIZE_T num, BB_OBJ **item, BB_SIZE_T *item_size);
extern int BB_GET_ARRAY_NUM_BY_INDEX(BB_OBJ *obj, BYTE *index_str, BB_SIZE_T index_str_size);
extern int BB_GET_ARRAY_NUM_BY_INDEX_STRU(BB_OBJ_STRU *obj_stru, BYTE *index_str, BB_SIZE_T index_str_size);
    
extern bool BB_GET_ARRAY_UINT_VALUE_BY_INDEX(BB_OBJ *obj,BYTE *index_str, BB_SIZE_T index_str_size,UInt64 *ret_value);
extern bool BB_GET_ARRAY_REC_BY_INDEX_STRU(BB_OBJ_STRU *obj_stru, BYTE *index_str, BB_SIZE_T index_str_size, BYTE **item, BB_SIZE_T *item_size);
extern bool BB_GET_ARRAY_REC_BY_INDEX(BB_OBJ *obj,BYTE *index_str, BB_SIZE_T index_str_size,BYTE **item, BB_SIZE_T *item_size);
//extern bool BB_GET_ARRAY_KEY_VALUE(BB_OBJ *obj,BYTE *index_str, BB_SIZE_T index_str_size,BYTE **item, BB_SIZE_T *item_size);
extern int BB_GET_TABLE_FIELD_NAME_ID(BB_OBJ *cell_obj,const char *field_str);
extern int BB_GET_TABLE_FIELD_NAME_ID_STRU(BB_OBJ_STRU *cell_obj_stru,const char *field_str);
extern int BB_GET_TABLE_FIELD_NAME_ID_LEN(BB_OBJ *cell_obj,const char *field_str,BB_SIZE_T len);
extern int BB_GET_TABLE_FIELD_NAME_ID_STRU_LEN(BB_OBJ_STRU *cell_obj_stru,const char *field_str,BB_SIZE_T len);
extern bool BB_PARA_CONDITION_TEST(BB_OBJ *para_obj, BYTE *item, BB_SIZE_T item_size,BYTE *field_type,BB_OBJ_STRU *table_stru);
extern bool BB_TABLE_PARA_CONDITION_TEST(BB_OBJ_STRU *table_stru,BB_SIZE_T num,BB_OBJ *para_obj,int &err_code);
extern bool BB_PARA_GET_KEY_VALUE(BB_OBJ *obj, BB_SIZE_T *key, BYTE **value, BB_SIZE_T *value_size, BYTE *condition,BB_OBJ_STRU *table_stru);
extern BYTE BB_CONDITIONS_GET_KEYS(BYTE *conditions, BB_SIZE_T conditions_size, BB_SIZE_T *key, BYTE key_max_num,BB_OBJ_STRU *table_stru);
extern int BB_APPEND_ARRAY_REC(BB_OBJ *obj, BYTE *buf, BB_SIZE_T buf_size);
extern int BB_APPEND_ARRAY_REC_STRU(BB_OBJ_STRU *obj_stru, BYTE *buf, BB_SIZE_T buf_size);
extern BB_OBJ *BB_NEW_REPLACE_ARRAY_REC_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T num,BYTE *buf, BB_SIZE_T buf_size,BB_OBJ *p_buf);
extern int BB_APPEND_ARRAY_REC_BY_INDEX(BB_OBJ *obj, BYTE *buf, BB_SIZE_T buf_size, BYTE *index_str,BB_SIZE_T index_str_len);
extern int BB_APPEND_ARRAY_REC_BY_INDEX_STRU(BB_OBJ_STRU *obj_stru, BYTE *buf, BB_SIZE_T buf_size, BYTE *index_str,BB_SIZE_T index_str_len);
extern bool BB_DELETE_ARRAY_REC(BB_OBJ *obj, BB_SIZE_T num);
extern bool BB_DELETE_ARRAY_REC_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T del_num);
extern int BB_DELETE_ARRAY_REC_BY_INDEX_STRU(BB_OBJ_STRU *obj_stru,BYTE *index_str, BB_SIZE_T index_str_size);
extern int BB_DELETE_ARRAY_REC_BY_INDEX(BB_OBJ *obj,BYTE *index_str, BB_SIZE_T index_str_size);
//extern bool BB_GET_TABLE_ITEM_VALUE(BB_OBJ *obj, UInt32 row,UInt32 col);
//extern bool BB_GET_TABLE_ITEM_BY_INDEX(BB_OBJ *obj, BYTE *index_str, BB_SIZE_T index_str_size,BYTE **rt_item_offset,BB_SIZE_T *rt_item_size);
extern bool BB_GET_TABLE_ITEM_VALUE(BB_OBJ *obj, UInt32 row,UInt32 col,UInt64 *rt_value);
extern bool BB_GET_TABLE_ITEM_VALUE_STRU(BB_OBJ_STRU *obj_stru, UInt32 row,UInt32 col,UInt64 *rt_value);
extern bool BB_GET_TABLE_ITEMS_DATA_TYPE_VALUE(BB_OBJ_STRU *obj_stru, BB_SIZE_T row,BB_SIZE_T col,BYTE **rt_item_offset,BB_SIZE_T *rt_item_size, BYTE *rt_field_type);
extern bool BB_GET_TABLE_ITEMS_DATA(BB_OBJ *obj, UInt32 row,UInt32 col,BYTE **rt_item_offset,BB_SIZE_T *rt_item_size);
extern bool BB_GET_TABLE_ITEMS_DATA_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T row,BB_SIZE_T col,BYTE **rt_item_offset,BB_SIZE_T *rt_item_size);
extern bool BB_PUT_TABLE_ITEMS_DATA(BB_OBJ *obj, BB_SIZE_T row,BB_SIZE_T col,BYTE *buf,BB_SIZE_T buf_size,BYTE method);
extern bool BB_PUT_TABLE_ITEMS_DATA_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T row,BB_SIZE_T col,BYTE *buf,BB_SIZE_T buf_size,BYTE method=0);
extern bool BB_GET_TABLE_FIELD_OFFSET_SIZE_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T col,BB_SIZE_T *offset, BB_SIZE_T *size);
extern bool BB_GET_TABLE_FIELD_NAME_OFFSET_SIZE_STRU(BB_OBJ_STRU *obj_stru, const char * field_name,BB_SIZE_T *offset, BB_SIZE_T *size);
//extern int BB_GET_TABLE_FIELD_NAME_ID(BB_OBJ_STRU &cell_obj_stru,const char *field_str); //ly task
extern BB_SIZE_T BB_GET_TABLE_MAX_NUM_LEN(BB_OBJ_STRU *obj_stru);
extern void *BB_GET_OBJ_DATA(BB_OBJ *obj);
extern BB_SIZE_T BB_GET_TABLE_CELL_WIDTH(BB_OBJ *obj);
//extern BB_SIZE_T BB_GET_TABLE_CELL_WIDTH_STRU(BB_OBJ_STRU *obj_stru);
extern BB_SIZE_T BB_GET_TABLE_CELL_WIDTH_EXT(BB_OBJ_STRU *table_stru,BB_SIZE_T num);
extern BB_SIZE_T BB_GET_TABLE_FIELD_WIDTH(BB_OBJ *obj,BB_SIZE_T j);
extern BB_SIZE_T BB_GET_TABLE_FIELD_WIDTH_STRU(BB_OBJ_STRU *obj_stru,BB_SIZE_T j);
extern BB_SIZE_T BB_GET_TABLE_FIELD_TYPE(BB_OBJ *obj,BB_SIZE_T j);
extern BB_SIZE_T BB_GET_TABLE_FIELD_TYPE_STRU(BB_OBJ_STRU *obj_stru,BB_SIZE_T j);
//extern BB_SIZE_T BB_GET_TABLE_FIELD_WIDTH_STRU_EXT(BB_OBJ_STRU *obj_stru,BB_SIZE_T j);
extern bool BB_GET_TABLE_FIELD_NAME_STRU(BB_OBJ_STRU *cell_obj_stru,BYTE field_num,BYTE** item_offset,BB_SIZE_T *item_size);
extern int BB_WRITE_TABLE_BY_NUM(BB_OBJ*obj, BYTE* buf, BB_SIZE_T *bufoffset, BB_SIZE_T num,BYTE *bitmap_str, BYTE bitmap_size, BYTE method);
extern int BB_WRITE_TABLE_BY_NUM_STRU(BB_OBJ_STRU *obj_stru, BYTE* buf, BB_SIZE_T *bufoffset, BB_SIZE_T num, BYTE *bitmap_str, BYTE bitmap_size, BYTE method);
extern int BB_WRITE_TABLE_BY_INDEX(BB_OBJ*obj, BYTE* buf, BB_SIZE_T *bufofset,  BYTE *node_str, BYTE node_str_size, BYTE *bitmap, BYTE bitmap_size,BYTE method);
extern bool BB_READ_TABLE_BY_INDEX(BB_OBJ*obj, BYTE* buf, BB_SIZE_T *bufoffset,  BYTE *index_str, BYTE index_str_size, BYTE *bitmap_str, BYTE bitmap_size,BYTE method,bool add_index_heade);
//extern bool BB_READ_TABLE_BY_SORTID(BB_OBJ*obj, BYTE* buf, BB_SIZE_T *bufoffset, BYTE *sort_id_str, BYTE sort_str_size, BYTE *bitmap_str, BYTE bitmap_size, BYTE method, bool add_index_heade);
extern bool BB_READ_TABLE_BY_NUM(BB_OBJ*obj, BB_SIZE_T num, BYTE* buf, BB_SIZE_T *bufoffset, BYTE *bitmap_str, BYTE bitmap_size,BYTE method);
extern bool BB_READ_TABLE_BY_NUM_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T num, BYTE* buf, BB_SIZE_T *bufoffset, BYTE *bitmap_str, BYTE bitmap_size, BYTE method);
extern bool BB_GET_TABLE_ITEMS_ID_DATA_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T row,BYTE col,BYTE **data,BB_SIZE_T *data_size);
extern bool BB_GET_TABLE_ITEMS_ID_DATA(BB_OBJ *obj, BB_SIZE_T row,BYTE col,BYTE **data,BB_SIZE_T *data_size);
extern bool BB_GET_TABLE_ITEMS_ID_OBJ_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T row,BYTE col,BYTE **data,BB_SIZE_T *data_size);
extern bool BB_GET_TABLE_ITEMS_ID_OBJ(BB_OBJ *obj, BB_SIZE_T row,BYTE col,BYTE **data,BB_SIZE_T *data_size);
extern BB_SIZE_T BB_GET_TABLE_ITEMS_IDS_WIDTH_EXT(BB_OBJ_STRU *table_stru,BB_SIZE_T row, BYTE *bitmap_str, BB_SIZE_T bitmap_size);
extern BYTE BB_CHECK_METHOD(BYTE method);
extern bool BB_GET_TABLE_ITEMS_NAME_DATA_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T row,const char *name,BYTE **data,BB_SIZE_T *data_size);
//extern bool BB_DELETE_TABLE_BY_INDEX(BB_OBJ*obj, BYTE* buf, BB_SIZE_T *bufoffset,  BYTE *index_str, BYTE index_str_size, BYTE *bitmap_str, BYTE bitmap_size,BYTE method,bool add_index_heade);
//extern bool BB_DELETE_TABLE_BY_NUM(BB_OBJ*obj, BB_SIZE_T num, BYTE* buf, BB_SIZE_T *bufoffset, BYTE *bitmap_str, BYTE bitmap_size,BYTE method);

BB_SEARCH_STRU BB_NEW_SEARCH_CELL(BYTE search_type, const void *data, BYTE data_len);
BYTE BB_SEARCH_CELL_SIZE(BB_OBJ *sobj);

//2 dimension object: Header(1Byte)+CIndexObj+RIndexObj;  AllInfoDB:Len;MaxLen,AddInfo,Sort
//extern BB_OBJ *BB_NEW_TABLE(BYTE db_type, BB_OBJ *cell_obj,BB_OBJ *RIndexObj,BB_OBJ *CIndexObj,const void *content, UInt64 content_len);

#ifdef  __cplusplus
}
#endif

#endif
