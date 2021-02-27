/*
 * This code originally came from here
 *
 * http://base64.sourceforge.net/bytebae.c
 *
 * with the following license:
 *
 * LICENCE:        Copyright (c) .
 *
 *                Permission is hereby granted, free of charge, to any person
 *                obtaining a copy of this software and associated
 *                documentation files (the "Software"), to deal in the
 *                Software without restriction, including without limitation
 *                the rights to use, copy, modify, merge, publish, distribute,
 *                sublicense, and/or sell copies of the Software, and to
 *                permit persons to whom the Software is furnished to do so,
 *                subject to the following conditions:
 *
 *                The above copyright notice and this permission notice shall
 *                be included in all copies or substantial portions of the
 *                Software.
 *
 *                THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 *                KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *                WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 *                PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 *                OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *                OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 *                OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *                SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * VERSION HISTORY:
 *               Bob Trower 08/04/01 -- Create Version 0.00.00B
 *
 */
//#include "stdafx.h"
#include "ByteBase.h"
//#include <string>
//#include <iostream>
#include "math.h"
#include "stdlib.h"
#include <string.h>

UInt16 BB_Error_Code;
BB_OBJ_STRU BB_OBJ_STRU_NULL;
BB_OBJ_STRU BB_OBJ_STRU_DEFAULT;
static BYTE BB_null_obj=BB_NULL;

//get bit value from object;
static int _get_mask_value(BB_OBJ *obj, BYTE mask);
//set bit value from object;
static void  _set_mask_value(BB_OBJ *obj, BYTE mask, BYTE val);
static BB_SIZE_T _get_max_value(BB_SIZE_T index_len,BB_SIZE_T max_len,BB_SIZE_T data_len);

/************************Tools*******************************/
static int _get_mask_value(BB_OBJ *obj, BYTE mask){//get bit value from object;
    return ((*obj) & mask);
}
/*static BYTE _get_cell_uint_mark_type(BYTE *obj){//取得一般Cell对象尾部长度信息，一般A4-1后改为A3-1;
	//return pow((double)2,_get_mask_value(obj,BB_CELL_UINT_WIDTH_MASK));
	switch(_get_mask_value(obj,BB_CELL_UINT_WIDTH_MASK)){
	case 0:return BB_UINT8;
	case 1:return BB_UINT16;
	case 2:return BB_UINT32;
	case 3:return BB_UINT64;
	}
 }*/
static BYTE _get_cell_uint_mark_width(BYTE *obj){//取得一般Cell对象尾部长度信息，一般A4-1后改为A3-1;
    //return pow((double)2,_get_mask_value(obj,BB_CELL_UINT_WIDTH_MASK));
    switch(_get_mask_value(obj,BB_CELL_UINT_WIDTH_MASK)){
        case 0:return 1;
        case 1:return 2;
        case 2:return 4;
        case 3:return 8;
        default:return 0;
    }
}

static BYTE _get_uint_type_value(BYTE uint_type){//取得一般Cell对象尾部长度信息，一般A4-1后改为A3-1;
    //return pow((double)2,_get_mask_value(obj,BB_CELL_UINT_WIDTH_MASK));
    switch(uint_type){
        case BB_UINT8:return 0;
        case BB_UINT16:return 1;
        case BB_UINT32:return 2;
        case BB_UINT64:return 3;
        default: return 0;
    }
}

static BYTE _get_uint_type_len(BYTE uint_type){//取得一般Cell对象尾部长度信息，一般A4-1后改为A3-1;
    //return pow((double)2,_get_mask_value(obj,BB_CELL_UINT_WIDTH_MASK));
    switch(uint_type){
        case BB_UINT8:return 1;
        case BB_UINT16:return 2;
        case BB_UINT32:return 4;
        case BB_UINT64:return 8;
        default:return 0;
    }
}
static void  _set_mask_value(BB_OBJ *obj, BYTE mask, BYTE val){//set bit value from object;
    //(*obj)=((*obj)&(mask))|((val)&(mask));
    (*obj)=(*obj)|((val)&(mask));
    return;
}
static BB_SIZE_T _get_max_value(BB_SIZE_T index_len,BB_SIZE_T max_len,BB_SIZE_T data_len){
    BB_SIZE_T r_len=index_len;
    if (r_len <max_len)
        r_len=max_len;
    if (r_len < data_len)
        r_len=data_len;
    return r_len;
}

static UInt16 _scanUINT16(BYTE *pBuf, size_t offset)
{
    return((((UINT)pBuf[offset]) << 8) + pBuf[offset+1]);
}
static UInt32 _scanUINT32(BYTE *pBuf, size_t offset)
{
    pBuf += offset;
    
    return (((ULONG)pBuf[0]) << 24)+(((ULONG)pBuf[1] ) << 16)+(((ULONG)pBuf[2] ) << 8) + pBuf[3];
}
static UInt64 _scanUINT64(BYTE *pBuf, size_t offset)
{
    pBuf += offset;
    
    return (((uint64)pBuf[0]) << 56)+(((uint64)pBuf[1]) << 48)+(((uint64)pBuf[2]) << 40)+(((uint64)pBuf[3]) << 32)+
    (((uint64)pBuf[4]) << 24)+(((uint64)pBuf[5]) << 16)+(((uint64)pBuf[6]) << 8) + pBuf[7];
    
}
static UInt64 _scanUINT(BYTE *pBuf,BYTE cell_width){
    
    switch (cell_width){
        case 1:
            return (*pBuf);
        case 2: case 3:
            return _scanUINT16(pBuf,0);
        case 4: case 5: case 6: case 7:
            return _scanUINT32(pBuf,0);
        case 8:
            return _scanUINT64(pBuf,0);
    }
    printf("%s\n","Error _scanUINT()输入必须是1-2-4-8");
    return 0;
}


static UInt64 _scanUINTWidth(BYTE *pBuf,BYTE cell_width){
    if (cell_width>8)
        printf("%s\n","Error _scanUINT()输入必须是1-8");
    uint64 r=0;
    for(int i=0; i<cell_width; i++){
        r+=((uint64)pBuf[i] << (cell_width-i-1)*8);
    }
    //uint64 tmp= (((uint64)pBuf[0]) << 56)+(((uint64)pBuf[1]) << 48)+(((uint64)pBuf[2]) << 40)+(((uint64)pBuf[3]) << 32)+	(((uint64)pBuf[4]) << 24)+(((uint64)pBuf[5]) << 16)+(((uint64)pBuf[6]) << 8) + pBuf[7];
    //(((ULONG)pBuf[0]) << 24)+(((ULONG)pBuf[1] ) << 16)+(((ULONG)pBuf[2] ) << 8) + pBuf[3];
    return r;
}
UInt64 BB_ScanUINTWidth(BYTE *pBuf,BYTE cell_width){
    return _scanUINTWidth(pBuf,cell_width);
}
int check_array(BYTE *arrays, BYTE array_field_size, int array_field_num,BYTE *cmp_value, BYTE cmp_value_size){
    UInt64 tmp,cmp=_scanUINT(cmp_value,cmp_value_size);int j;
    for (j=0; j<array_field_num; j++){
        tmp=_scanUINT(arrays+j,array_field_size);
        if (cmp==tmp){//发现为字段对象
            return j;
        }
    }
    return -1;
}
inline bool check_string(BYTE *value_str,BB_SIZE_T value_str_size,BYTE *cmp_value, BB_SIZE_T cmp_value_size, bool is_include){
    BB_SIZE_T i,size;
    BYTE *p1=value_str,*p2=cmp_value;
    
    if (is_include){//包含则只测试最小长度
        size=value_str_size < cmp_value_size ? value_str_size : cmp_value_size;
    }else{
        if (value_str_size!=cmp_value_size)
            return false;
        size=value_str_size;
    }
    //逐字比较
    for(i=0;i<size;i++){
        if (*p1!=*p2)
            return false;
        p1++;
        p2++;
    }
    return true;
}
static BYTE _pubUINT16(BYTE *pBuf, size_t offset, ULONG u16value)
{
    pBuf[offset++] = (BYTE)(u16value >> 8);
    pBuf[offset] = (BYTE)(u16value & 0xff);
    return(2);
}
static BYTE _pubUINT32(BYTE *pBuf, size_t offset, ULONG u32value)
{
    BYTE i;
    
    pBuf += (offset + 4);
    
    for (i = 0; i < 4; i++)
    {
        *(--pBuf) = (BYTE)(u32value & 0xff);
        u32value >>= 8;
    }
    
    return(4);
}
static BYTE _pubUINT64(BYTE *pBuf, size_t offset, uint64 u64value)
{
    BYTE i;
    
    pBuf += (offset + 8);
    
    for (i = 0; i < 8; i++)
    {
        *(--pBuf) = (BYTE)(u64value & 0xff);
        u64value >>= 8;
    }
    
    return i;
}
static BYTE _pubUINT(BYTE *pBuf, size_t offset, uint64 u64value)
{
    //BYTE *p_buf=pBuf+offset;
    
    if (u64value <= 0xFF){
        pBuf[0] = (BYTE)u64value;
        return 1;
    }else if (u64value <= 0xFFFF){
        return _pubUINT16(pBuf, 0, (UINT)u64value);
    }else if (u64value <= 0xFFFFFFFF){
        return _pubUINT32(pBuf, 0, (ULONG)u64value);
    }else{
        return _pubUINT64(pBuf, 0, u64value);
    }
}
static BYTE _sizeUINT(uint64 u64value)
{
    if (u64value <= 0xFF){
        return 1;
    }else if (u64value <= 0xFFFF){
        return 2;
    }else if (u64value <= 0xFFFFFFFF){
        return 4;
    }else{
        return 8;
    }
}
static BYTE _sizeUINTType(BYTE type)
{
    if (type <= 1){
        return 1;
    }else if (type <= 2){
        return 2;
    }else if (type <= 4){
        return 4;
    }else{
        return 8;
    }
}


/*bool BB_ADD_BUFF_BUFF(BYTE *src_buff, BYTE *src_buff_next, BYTE *dest_buff, BYTE cell_width) {
	//累加或写入操作
	UInt64 src = BB_ScanUINTWidth(src_buff, 8);
	UInt64 src2 = BB_ScanUINTWidth(src_buff_next, 8);
	//写入准备
	BB_PUT_UINT_TO_BUFF(dest_buff, src+src2, 8); //add SYS_INFO_ALL_TIME
 }*/

UInt64 BB_ADD_UINT_TO_BUFF(BYTE *dest_buff, BYTE *src_buff, uint64 u64value, BYTE cell_width) {
    //累加或写入操作
    UInt64 uadd = BB_ScanUINTWidth(src_buff, cell_width)+u64value;
    
    //写入准备
    BB_PUT_UINT_TO_BUFF(dest_buff, uadd, cell_width); //add SYS_INFO_ALL_TIME
    
    return uadd;
}
bool BB_PUT_UINT_TO_BUFF(BYTE *buff,uint64 u64value,BYTE cell_width){
    //////BYTE buff[16];
    //if (bit_num == 8){
    //	buff[0] = (BYTE)u64value;
    //}else if (bit_num == 16){
    //	_pubUINT16(buff, 0, (UINT)u64value);
    //}else if (bit_num == 32){
    //	_pubUINT32(buff, 0, (ULONG)u64value);
    //}else{
    //	_pubUINT64(buff, 0, u64value);
    //}
    switch(cell_width){
        case 0:
            return false;
        case 1:
            buff[0]=(BYTE)u64value;
            break;
        case 3:	case 2:
            _pubUINT16(buff, 0, (UINT)u64value);
            break;
        case 5:	case 6:	case 7:	case 4:
            _pubUINT32(buff, 0, (ULONG)u64value);
            break;
        case 8:
            _pubUINT64(buff, 0, u64value);
            break;
        default:
            _pubUINT64(buff, 0, u64value);
            break;
    }
    return true;
}
/*static void _pubUINTToBuf(BYTE *buff,uint64 u64value,BYTE bit_num){
	//BYTE buff[16];
	if (bit_num == 8){
 buff[0] = (BYTE)u64value;
	}else if (bit_num == 16){
 _pubUINT16(buff, 0, (UINT)u64value);
	}else if (bit_num == 32){
 _pubUINT32(buff, 0, (ULONG)u64value);
	}else{
 _pubUINT64(buff, 0, u64value);
	}
	return;
 }*/
/*函数定义*/
/************************对象基本读取操作**************************/
BYTE  BB_GET_OBJ_DIM(BB_OBJ *obj);//取得对象维度信息
BYTE  BB_GET_OBJ_TYPE(BB_OBJ *obj);//取得对象类型信息
BB_SIZE_T BB_GET_OBJ_DATA_LEN(BB_OBJ *obj);//取得对象数值区长度
BB_SIZE_T  BB_GET_OBJ_SIZE(BB_OBJ *obj);//取得对象总大小。
void *BB_GET_OBJ_DATA(BB_OBJ *obj);//取得对象数据区指针
BB_OBJ* BB_OBJ_COPY(BB_OBJ *dest, BB_OBJ *src);//拷贝整个对象
BB_OBJ_STRU BB_GET_OBJ_STRU(BB_OBJ *obj);//取得对象结构化信息

/************************0 维通用对象操作**************************/
//BB_OBJ *BB_NEW_CELL(BYTE db_type, void *data, BYTE data_len, BB_OBJ *p_buf);//创建0位对象:data 初始化值，初始化值长度data_len；p_buf:如果为NULL 创建内存，否则沿用p_buf与开内存
BYTE *BB_GET_CELL_DATA(BB_OBJ *obj);//取得0维对象数据区指针
BB_SIZE_T BB_GET_CELL_DATA_LEN(BB_OBJ *obj);//取得0维对象数据区长度
void BB_SET_CELL_DATA_LEN(BB_OBJ *obj,BB_SIZE_T data_len);//设置0维对象数据区长度
/************************0 维数值对象操作**************************/
BB_UINT_STRU BB_NEW_CELL_UINT(UInt64 value, BYTE db_type);//创建数值对象，包括：UINT/MALLOC
bool BB_IS_UINT(BB_OBJ *obj);//判断当前对象是否为数值对象
UInt64 BB_GET_UINT_VALUE(BB_OBJ *obj);//取得数值对象值
int BB_SET_UINT_VALUE(BB_OBJ *obj,UInt64 value);//设置数值对象值
//bool BB_PARA_CONDITION_TEST(BB_OBJ *obj, BYTE *item, BB_SIZE_T item_size, BYTE *field_type);

/************************1维通用数组操作**************************/
//BB_OBJ *BB_NEW_ARRAY(BB_OBJ *cell_obj, BB_OBJ *index_obj, BB_OBJ *add_info_obj, BB_SIZE_T input_max_len, const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf);//创建一维数组，包括普通一维数组和表格数组
BB_OBJ *BB_NEW_ARRAY_UINT(BYTE cell_type, BYTE index_type, BB_SIZE_T input_max_len, const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf);//创建一维数组，包括普通一维数组和表格数组
BB_SIZE_T BB_GET_ARRAY_NUM(BB_OBJ *obj);//取得数组元素个数
BB_SIZE_T BB_GET_ARRAY_INDEX_NUM(BB_OBJ *obj);//取得数组索引编号
BYTE BB_GET_ARRAY_INDEX_OBJ_WIDTH(BB_OBJ *obj);//取得数组索引字宽
//BB_SIZE_T BB_GET_ARRAY_UINT_SUM(BB_OBJ *obj);//取得数值数组值合计
bool BB_GET_ARRAY_UINT_VALUE(BB_OBJ *obj,UInt32 i,UInt64 *ret);//取得数值数组值合计
bool BB_GET_ARRAY_UINT_VALUE_STRU(BB_OBJ_STRU *obj_stru,BB_SIZE_T i,UInt64 *ret);//取得数值数组值合计

bool BB_GET_ARRAY_REC_BY_NUM_STRU(BB_OBJ_STRU *obj_stru,BB_SIZE_T num, BYTE **item, BB_SIZE_T *item_size);//取得数组记录（表格），元素（数组）的指针和长度
bool BB_GET_ARRAY_REC_BY_NUM(BB_OBJ *obj,BB_SIZE_T num, BYTE **item, BB_SIZE_T *item_size);//同上
int BB_GET_ARRAY_NUM_BY_INDEX(BB_OBJ *obj, BYTE *index_str, BB_SIZE_T index_str_size);//通过数组索引取得记录编号
int BB_APPEND_ARRAY_REC(BB_OBJ *obj, BYTE *buf, BB_SIZE_T buf_size);//数组对象添加一条记录，buf初始数据，buf_size初始数据长度
//int BB_APPEND_ARRAY_REC_BY_INDEX(BB_OBJ *obj, BYTE *buf, BB_SIZE_T buf_size, BYTE *index_str,BB_SIZE_T index_str_len);//数组(带索引)对象添加一条记录，buf初始数据，buf_size初始数据长度

/************************表格对象设定操作**************************/
//bool BB_GET_TABLE_ITEM_VALUE(BB_OBJ *obj, UInt32 row,UInt32 col,UInt64 *rt_value);
//bool BB_GET_TABLE_ITEM_VALUE_STRU(BB_OBJ_STRU *obj_stru, UInt32 row,UInt32 col,UInt64 *rt_value);
//bool BB_GET_TABLE_ITEMS_DATA_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T row,UInt32 col,BYTE **data,BB_SIZE_T *data_size);
BB_SIZE_T BB_GET_TABLE_CELL_WIDTH(BB_OBJ *obj);
//BB_SIZE_T BB_GET_TABLE_FIELD_WIDTH(BB_OBJ *obj,BB_SIZE_T field_num);
//BB_SIZE_T BB_GET_TABLE_FIELD_WIDTH_STRU(BB_OBJ_STRU *cell_obj_stru,BB_SIZE_T field_num);
// BB_GET_TABLE_FIELD_WIDTH_STRU(BB_OBJ_STRU *cell_obj_stru,UInt32 field_num);
int BB_WRITE_TABLE_BY_INDEX(BB_OBJ*obj, BYTE* buf, BB_SIZE_T *bufoffset,  BYTE *index_str, BYTE index_str_size, BYTE *bitmap_str, BYTE bitmap_size,BYTE method);//写入、添加表格（索引）记录，输入：buf+bufoffset-数据区指针,index_str-索引字串bitmap-field编号，method写入方式（0：普通bitmap; 1:bitmap区段）
int BB_WRITE_TABLE_BY_NUM(BB_OBJ*obj, BYTE* buf, BB_SIZE_T *bufoffset, BB_SIZE_T num,BYTE *bitmap_str, BYTE bitmap_size, BYTE method);//写入、添加表格记录，输入：buf+bufoffset-数据区指针,bitmap-field编号，method写入方式（0：普通bitmap; 1:bitmap区段）
bool BB_READ_TABLE_BY_NUM(BB_OBJ*obj, BB_SIZE_T num, BYTE* buf, BB_SIZE_T *bufoffset, BYTE *bitmap_str, BYTE bitmap_size,BYTE method);//读取表格记录，通过num记录编号，按照bitmap_str,method指定的格式
//bool BB_READ_TABLE_BY_INDEX(BB_OBJ*obj, BYTE* buf, BB_SIZE_T *bufoffset,  BYTE *index_str, BYTE index_str_size, BYTE *bitmap_str, BYTE bitmap_size,BYTE method, BB_OBJ *condition,bool add_index_header);//读取表格记录，通过index_str索引，按照bitmap_str,method指定的格式
//static bool get_table_obj_fields(BB_OBJ_STRU obj_stru,BYTE **obj_fields, BB_SIZE_T &obj_fields_num, BYTE &obj_fields_size);//取得表格中的对象字段列表
//static void set_table_obj_field(BYTE type, BYTE *buf, BB_SIZE_T buf_size, BB_SIZE_T field_size, BB_OBJ **new_obj);//设置表格中的对象字段列表
static BB_SIZE_T get_table_field_string(BYTE *src_field_pos,BB_SIZE_T src_field_size);
//static void add_bitmap_start_end(BB_BITMAP_TYPE *bitmap,BYTE bitmap_size);

/************************对象基本读取操作**************************/
BYTE  BB_GET_OBJ_DIM(BB_OBJ *obj){
    //get the object dimensions number
    if ((*obj)==BB_NULL)
        return 0xFF;
    else{
        BYTE type = BB_GET_OBJ_TYPE(obj);
        if (type == BB_ARRAY || type == BB_TABLE)
            return 1;
        else
            return 0;
        //table /array : 11000000/10000000
        /*if (((*obj)&BB_ARRAY)==BB_ARRAY)
         return 1;
         else
         return 0;*/
    }
}
BYTE  BB_GET_OBJ_TYPE(BB_OBJ *obj){
    //get the object type，0-1
    if (obj==NULL)
        return 0;
    if (((*obj)&BB_REDIR)==BB_REDIR)
        return BB_REDIR;//目前用A8=1;A7=1;A6=1代替重定向
    if (((*obj)&BB_ADDOBJ) == BB_ADDOBJ)
        return BB_ADDOBJ;//目前用A8=1;A7=0;A6=1外部对象
    if (((*obj)&BB_TABLE)==BB_TABLE) return BB_TABLE;
    if (((*obj)&BB_ARRAY)==BB_ARRAY) return BB_ARRAY;
    if (((*obj)&BB_BYTES)==BB_BYTES) return BB_BYTES;
    if (((*obj)&BB_BYTES_EXT)==BB_BYTES_EXT) return BB_BYTES_EXT;
    if (((*obj)&BB_PARA_EXT)==BB_PARA_EXT) return BB_PARA_EXT;//应该在BB_BYTES_EXT后面，30/20，20/30则100000/110000可能相互覆盖
    if (((*obj)&BB_MALLOC)==BB_MALLOC) return BB_MALLOC;	 //ly malloc
    //特殊BB_UINT没有专用头，为0，所以放在最后。
    if (((*obj)&BB_UINT64)==BB_UINT64) return BB_UINT64;
    if (((*obj)&BB_UINT32)==BB_UINT32) return BB_UINT32;
    if (((*obj)&BB_UINT16)==BB_UINT16) return BB_UINT16;
    if (((*obj)&BB_UINT8)==BB_UINT8) return BB_UINT8;
    if (((*obj)&BB_OBJ_NULL)==BB_OBJ_NULL) return BB_OBJ_NULL;
    return BB_NULL;
}
BB_SIZE_T BB_GET_OBJ_DATA_LEN(BB_OBJ *obj){//get object content lenght(content)
    switch(BB_GET_OBJ_DIM(obj)){
            //for cell;
        case 0xFF:
            return 0;
        case 0:
            return BB_GET_CELL_DATA_LEN(obj);
            //for array/table
        case 1:
        {BB_OBJ *lenObj = obj+1;
            return((BB_SIZE_T)BB_GET_UINT_VALUE(lenObj)); //需要初始化时和增删改时同步LenObj数据
        }
    }
    return 0;
}

BB_SIZE_T  BB_GET_OBJ_SIZE_CHECK(BB_OBJ *obj,BB_SIZE_T range) {//get object size(header+content)
    //确保每一个对象都有一个LenObj(UINT8-64)
    BB_SIZE_T header_len;
    BB_OBJ *pos;
    BB_SIZE_T max_len = 0, data_len;
    BYTE obj_type;
    
    switch (BB_GET_OBJ_DIM(obj)) {
            //0 dimension
        case 0xFF:
            return 1;
        case 0:
            obj_type = BB_GET_OBJ_TYPE(obj);
            if (obj_type == BB_BYTES_EXT)
                return BB_GET_OBJ_DATA_LEN(obj) + BB_OBJ_STX_EXT_LEN;
            else if (obj_type == BB_PARA_EXT) {
                int key_width = _get_cell_uint_mark_width(obj);
                int value_size = *(obj + BB_OBJ_STX_EXT_LEN - 1);
                return key_width + BB_OBJ_STX_EXT_LEN + value_size;
            }
            else if (obj_type == BB_MALLOC) {//ly malloc
                if (((*obj) & BB_MALLOC_SAVE_MASK) == BB_MALLOC_SAVE_MASK)
                    return (BB_OBJ_STX_EXT2_LEN + obj[BB_MALLOC_SAVE_FILE_BYTE]+ BB_GET_OBJ_DATA_LEN(obj));//如果存在存储路径字段
                else
                    return (BB_OBJ_STX_EXT_LEN+ BB_GET_OBJ_DATA_LEN(obj));
            }
            else
                return BB_GET_OBJ_DATA_LEN(obj) + BB_OBJ_STX_LEN;
        case 1:
            //get header length
            pos = obj;
            pos++;//Header Size
            pos += BB_GET_OBJ_SIZE(pos);//Len
            pos += BB_GET_OBJ_SIZE(pos);//Cell;
            if (((*obj)&BB_MASK_IndexObj) == BB_MASK_IndexObj)
                pos += BB_GET_OBJ_SIZE(pos);//Index; ly obj ly max
            if (((*obj)&BB_MASK_AddInfo) == BB_MASK_AddInfo)
                pos += BB_GET_OBJ_SIZE(pos);//AddInfo,这部分容易引起死循环！！！如果数字错的时候
            if (((*obj)&BB_MASK_MaxLenObj) == BB_MASK_MaxLenObj) {
                //MaxInfo
                max_len = (BB_SIZE_T)BB_GET_UINT_VALUE(pos);
                pos += BB_GET_OBJ_SIZE(pos);//递进。
            }
            data_len = BB_GET_OBJ_DATA_LEN(obj);
            header_len = pos - obj;
            //get data length
            if (max_len == 0)
                return header_len + data_len;
            else
                return header_len + max_len;
        default:
            printf("%s\n", "BB_GET_OBJ_SIZE() input *obj out 0-1-ff!");
            return 0;
    }
}
BB_SIZE_T  BB_GET_OBJ_SIZE(BB_OBJ *obj){//get object size(header+content)
    BB_SIZE_T range = 0xFFFFFFFF;
    return BB_GET_OBJ_SIZE_CHECK(obj, range);
}
void *BB_GET_OBJ_DATA(BB_OBJ *obj){//get object content lenght(content)
    //switch(BB_GET_OBJ_DIM(obj)){
    //	//0 dimension:
    //	case 0:
    //		return BB_GET_CELL_DATA(obj);
    //	default:
    //		return NULL;
    //}
    return BB_GET_OBJ_STRU(obj).Data;
}
BB_OBJ* BB_OBJ_COPY(BB_OBJ *dest, BB_OBJ *src){
    //拷贝,并返回拷贝后的尾指针,但是没有malloc检测。
    //get length
    BB_SIZE_T size = BB_GET_OBJ_SIZE(src);
    return (BB_OBJ *)memcpy(dest,src,size)+size;
}

bool BB_GET_OBJ_HEAD_STRU(BB_OBJ *obj, BB_OBJ_STRU *s_obj){
    BB_OBJ *pos;
    BB_SIZE_T len_size;
    
    (*s_obj).LoadError=0;
    (*s_obj).Header=(*obj);
    (*s_obj).ThisObj=obj;
    
    switch(BB_GET_OBJ_DIM(obj)){
            //0 dimension
        case 0xFF:
            (*s_obj).Type=BB_NULL;
            return true;
        case 0:
            (*s_obj).Type=BB_GET_OBJ_TYPE(obj);
            (*s_obj).CellObj=(*s_obj).IndexObj=(*s_obj).AddInfoObj=(*s_obj).MaxLenObj=NULL;
            (*s_obj).Data=BB_GET_CELL_DATA(obj);
            break;
        case 1:
            (*s_obj).Type=BB_GET_OBJ_TYPE(obj);
            (*s_obj).LenObj = obj+1;//DataLen
            len_size=BB_GET_OBJ_SIZE((*s_obj).LenObj);
            pos=(*s_obj).CellObj=(*s_obj).LenObj+len_size;  //Cell;
            //cell_size=BB_GET_OBJ_SIZE((*s_obj).CellObj);//to_test
            if (((*obj) & BB_MASK_IndexObj)==BB_MASK_IndexObj)
                pos=(*s_obj).IndexObj=pos+BB_GET_OBJ_SIZE(pos);//Index;
            else
                (*s_obj).IndexObj=NULL;
            //AddInfo
            if (((*obj) & BB_MASK_AddInfo)==BB_MASK_AddInfo)
                pos=(*s_obj).AddInfoObj=pos+BB_GET_OBJ_SIZE(pos);
            else
                (*s_obj).AddInfoObj=NULL;
            //MaxLenObj
            if (((*obj) & BB_MASK_MaxLenObj)==BB_MASK_MaxLenObj)
                pos=(*s_obj).MaxLenObj=pos+BB_GET_OBJ_SIZE(pos);// ly max
            else
                (*s_obj).MaxLenObj=NULL;
            (*s_obj).Data=pos+BB_GET_OBJ_SIZE(pos);
            break;
        default:
            (*s_obj).LoadError=1;
            printf("%s\n","Error:Load Error In BB_GET_OBJ_STRU()!");
            return false;
    }
    (*s_obj).HeaderLen=(*s_obj).Data - obj;
    (*s_obj).DataLen=BB_GET_OBJ_DATA_LEN(obj);
    
    return true;
}

BB_OBJ_STRU BB_GET_OBJ_STRU(BB_OBJ *obj){
    BB_OBJ_STRU s_obj;
    BB_OBJ *pos;
    BB_SIZE_T len_size;
    if (obj==NULL){
        s_obj.LoadError=true;
        return s_obj;
    }
    s_obj.LoadError=false;
    s_obj.Header=(*obj);
    s_obj.ThisObj=obj;
    
    switch(BB_GET_OBJ_DIM(obj)){
            //0 dimension
        case 0xFF:
            s_obj.Type=BB_NULL;
            s_obj.Data=obj;     //ly task
            s_obj.DataLen=0;
            s_obj.Size=1;
            return s_obj;
        case 0:
            s_obj.Type=BB_GET_OBJ_TYPE(obj);
            s_obj.CellObj=s_obj.IndexObj=s_obj.AddInfoObj=s_obj.MaxLenObj=NULL;
            s_obj.Data=BB_GET_CELL_DATA(obj);
            s_obj.Size = BB_GET_OBJ_SIZE(obj);
            break;
        case 1:
            s_obj.Type=BB_GET_OBJ_TYPE(obj);
            s_obj.LenObj = obj+1;//DataLen
            len_size=BB_GET_OBJ_SIZE(s_obj.LenObj);
            pos=s_obj.CellObj=s_obj.LenObj+len_size;  //Cell;
            //cell_size=BB_GET_OBJ_SIZE(s_obj.CellObj);//to_test
            if (((*obj) & BB_MASK_IndexObj)==BB_MASK_IndexObj)
                pos=s_obj.IndexObj=pos+BB_GET_OBJ_SIZE(pos);//Index;
            else
                s_obj.IndexObj=NULL;
            //AddInfo
            if (((*obj) & BB_MASK_AddInfo)==BB_MASK_AddInfo)
                pos=s_obj.AddInfoObj=pos+BB_GET_OBJ_SIZE(pos);
            else
                s_obj.AddInfoObj=NULL;
            //MaxLenObj
            if (((*obj) & BB_MASK_MaxLenObj)==BB_MASK_MaxLenObj)
                pos=s_obj.MaxLenObj=pos+BB_GET_OBJ_SIZE(pos);// ly max
            else
                s_obj.MaxLenObj=NULL;
            s_obj.Data=pos+BB_GET_OBJ_SIZE(pos);
            break;
        default:
            s_obj.LoadError=true;
            printf("%s\n","Error:Load Error In BB_GET_OBJ_STRU()!");
            return s_obj;
    }
    s_obj.HeaderLen=s_obj.Data - obj;
    s_obj.DataLen=BB_GET_OBJ_DATA_LEN(obj);
    
    switch (s_obj.Type){
        case BB_ARRAY:
            //！！？？目前只支持定长数组
            switch (BB_GET_OBJ_TYPE(s_obj.CellObj)){
                case BB_UINT8:		case BB_UINT16:		case BB_UINT32:		case BB_UINT64:
                    s_obj.CellWidth=BB_GET_OBJ_DATA_LEN(s_obj.CellObj); //取得元素宽度
                    s_obj.Num=(BB_SIZE_T)BB_GET_UINT_VALUE(s_obj.LenObj)/s_obj.CellWidth;
                    break;
                case BB_MALLOC:
                    s_obj.CellWidth = BB_GET_OBJ_DATA_LEN(s_obj.CellObj); //取得元素宽度	//ly malloc 数组和table元素中只存放指针
                    s_obj.Num = (BB_SIZE_T)BB_GET_UINT_VALUE(s_obj.LenObj) / s_obj.CellWidth;
                    break;
                default://witch cell_obj==BB_NULL is object_array
                    s_obj.CellWidth=0;
                    if (s_obj.IndexObj!=NULL)
                        s_obj.Num=(BB_SIZE_T)BB_GET_ARRAY_INDEX_NUM(s_obj.IndexObj);
                    else{
                        s_obj.Num=BB_GET_ARRAY_NUM_STRU(&s_obj);
                    }
                    break;
            }
            break;
        case BB_TABLE:
            s_obj.CellWidth=BB_GET_TABLE_CELL_WIDTH(s_obj.CellObj); //取得元素宽度;to_add; 内部ly malloc 数组和table元素中只存放指针
            if (s_obj.CellWidth==0) {
                s_obj.LoadError = true;
                printf("%s\n", "Error:Load Error In BB_GET_OBJ_STRU():s_obj.CellWidth is 0 !");
                return s_obj;
            }
            s_obj.Num=(BB_SIZE_T)BB_GET_UINT_VALUE(s_obj.LenObj)/s_obj.CellWidth;
            break;
        default://普通零维节点
            s_obj.CellWidth=1;
            return s_obj;
    }
    //s_obj.Num=BB_GET_ARRAY_NUM_STRU(&s_obj);
    
    //以下为ARRAY/TABLE专用
    //测试
    //const char * data1=(const char *)(s_obj.Data)+1;
    //const char * data2=(const char *)(s_obj.Data)+2;
    if (s_obj.MaxLenObj!=NULL)
        s_obj.MaxLen = (BB_SIZE_T)BB_GET_UINT_VALUE(s_obj.MaxLenObj);
    else
        s_obj.MaxLen = s_obj.DataLen;//ly max
    
    s_obj.Size=s_obj.HeaderLen+s_obj.MaxLen;
    
    return s_obj;
}

/************************0 维通用对象操作**************************/
BB_OBJ *BB_NEW_CELL(BYTE db_type, const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf){
    /*0 dimension object: Header(1Byte)+Content; No MaxLen,No AddInfo,No Sort, BaseCell*/
    //p_buff:if malloc memory space;
    BB_OBJ *obj;
    if (db_type==BB_BYTES_EXT){//如果EXT则多一个BYTE ly ext
        obj= (p_buf==NULL) ? (BB_OBJ *)malloc(BB_OBJ_STX_EXT_LEN+data_len) : p_buf;//
        (*obj)=db_type;//set header db_type
        BB_SET_CELL_DATA_LEN(obj,data_len);//set obj data_len
        if (data!=NULL && data_len!=0)
            memcpy((void*)(obj+BB_OBJ_STX_EXT_LEN),data,data_len > BB_0DIM_BYTES_EXT_MAX_LEN ? BB_0DIM_BYTES_EXT_MAX_LEN: data_len);//copy data to data if has;
    }else{
        switch(db_type){
            case BB_UINT8: 		case BB_UINT16:			case BB_UINT32:			case BB_UINT64:	case BB_MALLOC:
                printf("ERROR: BB_NEW_CELL() not support UINT please using BB_NEW_CELL_UINT\n");
                return NULL;
                break;
            default:
                //如果是其他则直接采用data_len开去内存
                obj= (p_buf==NULL) ? (BB_OBJ *)malloc(BB_OBJ_STX_LEN+data_len) : p_buf;
                (*obj)=db_type;//set header db_type
                BB_SET_CELL_DATA_LEN(obj,data_len);//set obj data_len
                if (data!=NULL && data_len!=0)
                    memcpy((void*)(obj+BB_OBJ_STX_LEN),data, data_len > BB_0DIM_BYTES_MAX_LEN ? BB_0DIM_BYTES_MAX_LEN: data_len);//copy data to data if has; ly tree
                break;
        }
    }
    return obj;
}
BB_OBJ *BB_NEW_BYTES_ALL(const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf) {
    //p_buff:if malloc memory space;
    BB_OBJ *data_obj=NULL; 	//ly mac
    if ((data_len + 1) < BB_0DIM_BYTES_MAX_LEN)
        data_obj = BB_NEW_CELL(BB_BYTES, data, data_len, p_buf);//短字串
    else if ((data_len + 2) < BB_0DIM_BYTES_EXT_MAX_LEN)
        data_obj = BB_NEW_CELL(BB_BYTES_EXT, data, data_len, p_buf);//长字串
    else if ((data_len + 2) > BB_0DIM_BYTES_EXT_MAX_LEN) {
        BB_OBJ cell = BB_UINT8;
        data_obj = BB_NEW_ARRAY(&cell, NULL, NULL, 0, data, data_len, p_buf);
    }
    return data_obj;
}

BB_OBJ *BB_NEW_PARA(BYTE key_type,UInt64 key, const void *data, BYTE data_len,BB_OBJ *p_buf){
    //创建PARA 并开启空间。
    BB_OBJ *obj;
    
    BYTE key_len=_get_uint_type_len(key_type);//???A4-1;如BB_UINT64为8位;
    obj= (p_buf==NULL) ? (BB_OBJ *)malloc(BB_OBJ_STX_EXT_LEN+key_len+data_len) : p_buf;//
    (*obj)=BB_PARA_EXT & _get_uint_type_value(key_type);//set header's key type
    *(obj+1)=data_len;//设置value扩展字节
    if (data!=NULL && data_len!=0)
        memcpy((void*)(obj+BB_OBJ_STX_EXT_LEN+key_len),data,data_len);//copy data to data if has;
    
    return obj;
}

BB_OBJ *BB_NEW_OBJ_STRING(const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf) {
    BB_OBJ cell = BB_UINT8;
    BB_UINT_STRU add_info_tmp = BB_NEW_CELL_UINT(BB_TABLE_FIELD_TYPE_STRING, BB_UINT8);
    return  BB_NEW_ARRAY(&cell, NULL, (BB_OBJ *)&add_info_tmp, 0, data, data_len, NULL);
}

bool BB_IS_OBJ_STRING(BB_OBJ *item_obj) {
    if (BB_GET_OBJ_TYPE(item_obj) != BB_ARRAY)
        return false;
    
    BB_OBJ_STRU item_obj_stru = BB_GET_OBJ_STRU(item_obj);
    if (BB_GET_OBJ_TYPE(item_obj_stru.AddInfoObj) == BB_UINT8 && BB_GET_UINT_VALUE(item_obj_stru.AddInfoObj) == BB_TABLE_FIELD_TYPE_STRING) {
        return true;
    }
    else
        return false;
}

bool BB_IS_BYTES_ALL(BB_OBJ *item_obj) {
    if (BB_GET_OBJ_TYPE(item_obj)==BB_BYTES || BB_GET_OBJ_TYPE(item_obj)==BB_BYTES_EXT)
        return true;
    
    if (BB_GET_OBJ_TYPE(item_obj) != BB_ARRAY)
        return false;
    
    BB_OBJ_STRU item_obj_stru = BB_GET_OBJ_STRU(item_obj);
    if (BB_GET_OBJ_TYPE(item_obj_stru.CellObj) == BB_UINT8)
        return true;
    else
        return false;
}


bool BB_IS_OBJ_ARRAY_STRU(BB_OBJ_STRU *item_obj_stru) {
    if (item_obj_stru->Type != BB_ARRAY)
        return false;
    
    if (BB_GET_OBJ_TYPE(item_obj_stru->CellObj) == BB_NULL) {
        return true;
    }
    else
        return false;
}

BYTE *BB_GET_CELL_DATA(BB_OBJ *obj){//get object content lenght(content)
    
    switch (BB_GET_OBJ_TYPE(obj)) {
        case BB_BYTES_EXT:
        case BB_PARA_EXT:
            return (obj + BB_OBJ_STX_EXT_LEN);
        case BB_MALLOC://ly malloc 增加：扩展长度BB_GET_OBJ_TYPE(obj)==malloc
            if (((*obj) & BB_MALLOC_SAVE_MASK) == BB_MALLOC_SAVE_MASK)
                return (obj + BB_OBJ_STX_EXT2_LEN + obj[BB_MALLOC_SAVE_FILE_BYTE]);//如果存在存储路径字段
            else
                return (obj + BB_OBJ_STX_EXT_LEN);
        default:
            return (obj + BB_OBJ_STX_LEN);
    }
    /*
     if (BB_GET_OBJ_TYPE(obj)==BB_BYTES_EXT || BB_GET_OBJ_TYPE(obj)==BB_PARA_EXT)
     return (obj+BB_OBJ_STX_EXT_LEN);
     else
     return (obj+BB_OBJ_STX_LEN);*/
}
BB_SIZE_T BB_GET_CELL_DATA_LEN(BB_OBJ *obj)
{
    switch(BB_GET_OBJ_TYPE(obj)){//0 dimension:
        case BB_UINT8:
        case BB_UINT16:
        case BB_UINT32:
        case BB_UINT64:
            return _get_mask_value(obj, BB_0DIM_UINT_LEN_MASK);
        case BB_MALLOC:
            return _get_cell_uint_mark_width(obj); //ly malloc 增加-00:8位;01:16位;10:32位:11:64位， BB_SIZE_长度
            break;
        case BB_BYTES:
            return _get_mask_value(obj,BB_0DIM_BYTES_LEN_MASK);
        case BB_BYTES_EXT:
            return _get_mask_value(obj,BB_0DIM_BYTES_EXT_LEN_MASK)*256 + *(obj+1);
        case BB_PARA_EXT:
            return _get_mask_value(obj,BB_0DIM_PARA_EXT_KEY_LEN_MASK) + *(obj+1);//key+len
        case BB_REDIR:
            return _get_mask_value(obj,BB_0DIM_REDIR_LEN_MASK);
        case BB_ADDOBJ:
            return _get_mask_value(obj, BB_0DIM_ADDOBJ_LEN_MASK);
        default:
            return 0;
    }
}
void BB_SET_CELL_DATA_LEN(BB_OBJ *obj,BB_SIZE_T data_len){// ly ext
    switch(BB_GET_OBJ_TYPE(obj))
    {//0 dimension:
        case BB_UINT8:
        case BB_UINT16:
        case BB_UINT32:
        case BB_UINT64:
            return _set_mask_value(obj, BB_0DIM_UINT_LEN_MASK, (BYTE)data_len);
        case BB_MALLOC:
            return _set_mask_value(obj, BB_CELL_UINT_WIDTH_MASK,(BYTE)_get_uint_type_value(data_len)); //ly malloc;增加A2-1: 设定BB_SIZE长度、其中data_len只能是1,2,4,8代表8-64位
        case BB_BYTES:
            return _set_mask_value(obj,BB_0DIM_BYTES_LEN_MASK,(BYTE)data_len);
        case BB_BYTES_EXT:
            _set_mask_value(obj,BB_0DIM_BYTES_EXT_LEN_MASK,(BYTE)(data_len/256));
            *(obj+BB_OBJ_STX_LEN)=data_len%256;
            return ;
        case BB_PARA_EXT:
            printf("Error: %s\n","BB_SET_CELL_DATA_LEN() can't support BB_BYTES_EXT!");
            return;
        case BB_REDIR:
            return _set_mask_value(obj,BB_0DIM_REDIR_LEN_MASK,(BYTE)data_len);
        case BB_ADDOBJ:
            return _set_mask_value(obj, BB_0DIM_ADDOBJ_LEN_MASK, (BYTE)data_len);
        default:
            return ;
    }
}

/************************0 维数值对象操作**************************/
BB_UINT_STRU BB_NEW_CELL_UINT(UInt64 value, BYTE db_type){//set value to result data;
    //init null result
    BB_UINT_STRU result;
    memset(result.data,0,8);
    BYTE data_len;
    //unsigned int a;
    //BYTE *tmp_malloc=NULL; //ly mac
    
    /*Set Header*/
    if (db_type!=0)
        result.Header=db_type;
    //else
    //printf("%s\n","BB_NEW_CELL_UINT() not support db_type==0!");
    
    switch(db_type){
        case 0:
            data_len=_pubUINT(result.data,0,value);//设置长度信息，?可以用这个来写入，测试！
            switch (data_len) {
                case 1: result.Header = BB_UINT8; break;
                case 2: result.Header = BB_UINT16; break;
                case 4: result.Header = BB_UINT32; break;
                case 8: result.Header = BB_UINT64; break;
                default:
                    result.Header = BB_UINT64;
                    printf("BB_NEW_CELL_UINT() not support datalen(%u)!\n", data_len);
            }
            BB_SET_CELL_DATA_LEN((BB_OBJ *)(&result),data_len); //已经设置了长度,也同时设定了type;
            break;
        case BB_UINT8:
            result.data[0]=(BYTE)value;
            break;
        case BB_UINT16:
            data_len=_pubUINT16(result.data,0,(UINT)value);
            break;
        case BB_UINT32:
            data_len=_pubUINT32(result.data,0,(ULONG)value);
            break;
        case BB_UINT64:
            data_len=_pubUINT64(result.data,0,value);
            break;
        case BB_MALLOC:
            printf("BB_NEW_CELL_UINT() not support MALLOC please use BB_NEW_CELL_MALLOC()!"); //ly malloc del 应该建立一个新new Malloc函数
            break;
            /*a=sizeof(tmp_malloc);
             BB_SET_CELL_DATA_LEN((BB_OBJ *)(&result),a);
             switch(a){
             case 1:
             result.data[0]=(BYTE)value;
             break;
             case 2:
             data_len=_pubUINT16(result.data,0,(UINT)value);
             break;
             case 4:
             data_len=_pubUINT32(result.data,0,(ULONG)value);
             break;
             case 8:
             data_len=_pubUINT64(result.data,0,value);
             break;
             }
             break;*/
        default:
            result.Header=BB_NULL;
    }
    
    return result;
}

BB_OBJ *BB_NEW_MALLOC(const void *address,  const char* save_path, BYTE save_path_len, BYTE ext_obj_type, BB_OBJ *p_buf) {//set value to result data;																					   //创建PARA 并开启空间。
    BB_OBJ *obj;
    BYTE address_size = sizeof(address);
    
    //create mem size
    if (p_buf == NULL) {
        if (save_path ==NULL)
            obj = (BB_OBJ *)malloc(BB_OBJ_STX_EXT_LEN + address_size);
        else
            obj = (BB_OBJ *)malloc(BB_OBJ_STX_EXT2_LEN + address_size + save_path_len);
    }
    else
        obj = p_buf;
    //set obj header
    (*obj) = BB_MALLOC;
    
    //set malloc point address_size;
    BB_SET_CELL_DATA_LEN(obj, address_size);
    
    //set ext object type
    (*(obj + BB_MALLOC_BYTE_TYPE)) = ext_obj_type;
    
    if (save_path != NULL) {
        //set save_path & save_path lenlen;
        (*(obj + BB_MALLOC_SAVE_FILE_BYTE)) = save_path_len;
        memcpy((void*)(obj + BB_OBJ_STX_EXT2_LEN), save_path, save_path_len);
        //set malloc point address;
        if (address == NULL)
            memset((void*)(obj + BB_OBJ_STX_EXT_LEN), 0, address_size);
        else
            memcpy((void*)(obj + BB_OBJ_STX_EXT2_LEN + save_path_len), address, address_size);
    }else{
        //set malloc point address;
        if (address==NULL)
            memset((void*)(obj + BB_OBJ_STX_EXT_LEN), 0, address_size);
        else
            memcpy((void*)(obj + BB_OBJ_STX_EXT_LEN), address, address_size);
    }
    
    return obj;
}

bool BB_IS_UINT(BB_OBJ *obj){
    switch(BB_GET_OBJ_TYPE(obj))
    {//0 dimension:
        case BB_UINT8:
        case BB_UINT16:
        case BB_UINT32:
        case BB_UINT64:
            return true;
        default:
            return false;
    }
    return false;
}

UInt64 BB_GET_UINT_VALUE(BB_OBJ *obj){ //ly malloc no changed
    switch(BB_GET_OBJ_TYPE(obj))
    {//0 dimension:
        case BB_UINT8:
        case BB_UINT16:
        case BB_UINT32:
        case BB_UINT64:
        {//Cell Obj
            BB_SIZE_T value_len = BB_GET_CELL_DATA_LEN(obj);
            BYTE *value = (BYTE *)BB_GET_CELL_DATA(obj);
            switch (value_len) {
                case 1://BYTE
                    return (*value);
                case 2://UINT16
                    return _scanUINT16(value, 0);
                case 4://UINT32
                    return _scanUINT32(value, 0);
                case 8://UINT64
                    return _scanUINT64(value, 0);
            }
        }
            break;
        case BB_MALLOC:
            /*{//Cell Obj
             BB_SIZE_T value_len = BB_GET_CELL_DATA_LEN(obj);
             BYTE *value=(BYTE *)BB_GET_CELL_DATA(obj);
             switch(value_len){
             case 1://BYTE
             return (*value);
             case 2://UINT16
             return _scanUINT16(value,0);
             case 4://UINT32
             return _scanUINT32(value,0);
             case 8://UINT64
             return _scanUINT64(value,0);
             }
             }*/
            printf("BB_GET_UINT_VALUE() not support MALLOC, please use BB_GET_MALLOC_ADDRESS(BB_OBJ *obj, void *address)"); //ly malloc
            break;
    }
    return 0;
}

int BB_SET_UINT_VALUE(BB_OBJ *obj,UInt64 value){
    BYTE data_len=0;	//ly mac
    //BB_SIZE_T value_len;
    //BYTE *data;
    
    switch(BB_GET_OBJ_TYPE(obj))
    {//0 dimension:
        case BB_UINT8:
            *(obj+1)=(BYTE)value;
            data_len=1;
            break;
        case BB_UINT16:
            data_len=_pubUINT16(obj,1,(UINT)value);
            break;
        case BB_UINT32:
            data_len=_pubUINT32(obj,1,(ULONG)value);
            break;
        case BB_UINT64:
            data_len=_pubUINT64(obj,1,value);
            break;
            /*case BB_MALLOC:
             value_len = BB_GET_CELL_DATA_LEN(obj);
             switch(value_len){
             case 1://BYTE
             *(obj+1)=(BYTE)value;
             break;
             case 2://UINT16
             data_len=_pubUINT16(obj,1,(UINT)value);
             break;
             case 4://UINT32
             data_len=_pubUINT32(obj,1,(ULONG)value);
             break;
             case 8://UINT64
             data_len=_pubUINT64(obj,1,value);
             break;
             }
             break;*/
        case BB_MALLOC: //ly malloc ？？
            /*value_len = BB_GET_CELL_DATA_LEN(obj);
             data = (BYTE *)BB_GET_CELL_DATA(obj);
             switch (value_len) {
             case 1://BYTE
             (*data) = (BYTE)value;
             break;
             case 2://UINT16
             data_len = _pubUINT16(data, 0, (UINT)value);
             break;
             case 4://UINT32
             data_len = _pubUINT32(data, 0, (ULONG)value);
             break;
             case 8://UINT64
             data_len = _pubUINT64(data, 0, value);
             break;
             }*/
            printf("BB_SET_UINT_VALUE() not support MALLOC, please use BB_SET_MALLOC_ADDRESS(BB_OBJ *obj, void *address)"); //ly malloc
            break;
        default:
            return 0;
    };
    return data_len;
}
/*
BYTE BB_CONDITIONS_GET_KEYS(BYTE *conditions, BB_SIZE_T conditions_size, UInt64 *key, BYTE key_max_size){//头（a4-1包含key_width+扩展字段宽+key(UINTn)+value_point）
    BYTE i,*p_obj,*value,condition;
    UInt64 *p_key;
    BB_SIZE_T value_size;
    
    for(p_obj=conditions, p_key=key,i=0; (((size_t)(p_obj-conditions))<conditions_size) && (((size_t)(p_key-key))<=key_max_size); p_obj+=BB_GET_OBJ_SIZE(p_obj),p_key++,i++){
        //循环读取conditions的组合
        if (p_obj==NULL || !BB_PARA_GET_KEY_VALUE((BB_OBJ *)p_obj,p_key,&value,&value_size,&condition))
            break;
    }
    return i;
}


bool BB_PARA_GET_KEY_VALUE(BB_OBJ *para_obj, UInt64 *key, BYTE **value, BB_SIZE_T *value_size, BYTE *condition) {//头（a4-1包含key_width+扩展字段宽+key(UINTn)+value_point）
    BYTE type = BB_GET_OBJ_TYPE(para_obj);
    if (type == BB_PARA_EXT) {
        BYTE key_width = _get_cell_uint_mark_width(para_obj);
        (*key) = _scanUINT((para_obj + BB_OBJ_STX_EXT_LEN), key_width);
        (*value) = (para_obj + BB_OBJ_STX_EXT_LEN + key_width);
        (*value_size) = *(para_obj + BB_OBJ_STX_EXT_LEN - 1);
        (*condition) = (*para_obj) & BB_PARA_CONDITION_COMP_MASK;
        return true;
    }
    else if (type == BB_ARRAY) {//ly 910
        BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(para_obj);
        if (obj_stru.Num != 3) {
            printf("%s", "BB_PARA_GET_KEY_VALUE()'s input para_obj_type must be BB_PARA_EXT or BB_ARRAY[***,=,***]");
            return false;
        }
        //取得key
        BYTE *key_p;
        BB_SIZE_T key_size;
        BB_GET_ARRAY_REC_BY_NUM_STRU(&obj_stru, 0, &key_p, &key_size);
        if (key_size==1)
            (*key) = _scanUINT(key_p, key_size);
        else {//如果是"1之类"，今后改为直接数字
            char tmp[21];
            memcpy((void *)tmp, (const void *)key_p, key_size > 20 ? 20 : key_size);
            tmp[key_size] = 0;
            (*key) = atoi(tmp);
        }
        
        //取得conditions
        BYTE *cd;
        BB_SIZE_T cd_size;
        BB_GET_ARRAY_REC_BY_NUM_STRU(&obj_stru, 1, &cd, &cd_size);
        for (unsigned int i = 0; i < cd_size; i++)
            if ((*cd) == ' ')cd++;//去除空格
        if ((*cd) == '=') {
            if(*(cd+1)=='=')
                (*condition) = BB_PARA_CONDITION_EQUAL;
            else
                (*condition) = BB_PARA_CONDITION_STR_EQUAL;
        }
        else if ((*cd) == '>')
            (*condition) = BB_PARA_CONDITION_GT;
        else if ((*cd) == '<')
            (*condition) = BB_PARA_CONDITION_LT;
        else if ((*cd) == '!' && (*cd+1)=='=')
            (*condition) = BB_PARA_CONDITION_NOT_EQUAL;
        else {
            printf("%s", "BB_PARA_GET_KEY_VALUE()'s input BB_ARRAY[***,=,***] should be one of  = > < !=");
            return false;
        }
        //取得value
        BB_GET_ARRAY_REC_BY_NUM_STRU(&obj_stru, 2, value, value_size);
    }
    else {
        printf("%s", "BB_PARA_GET_KEY_VALUE()'s input para_obj_type must be BB_PARA_EXT or BB_ARRAY[***,=,***]");
        return false;
    }
    return true;
}*/

int BB_GET_TABLE_FIELD_NAME_ID(BB_OBJ *cell_obj,const char *field_str){//ly cell_name;ly task此处可以提速
    BB_OBJ_STRU cell_stru=BB_GET_OBJ_STRU(cell_obj);
    return BB_GET_TABLE_FIELD_NAME_ID_STRU(&cell_stru,field_str);
}

int BB_GET_TABLE_FIELD_NAME_ID_STRU(BB_OBJ_STRU *cell_obj_stru,const char *field_str){//ly cell_name;ly task此处可以提速
    BYTE *item;
    BB_SIZE_T item_size;
    
    for(BYTE i=0; i< cell_obj_stru->Num; i++){
        if (!BB_GET_TABLE_FIELD_NAME_STRU(cell_obj_stru,i,&item,&item_size))
            return -1;
        BYTE t=strlen(field_str);
        if ((strncmp(field_str,(const char *)item,t<=item_size ? t : item_size)==0))
            return i;
    }
    return -1;
}

int BB_GET_TABLE_FIELD_NAME_ID_LEN(BB_OBJ *cell_obj,const char *field_str,BB_SIZE_T field_len){//ly cell_name;ly task此处可以提速
    BB_OBJ_STRU cell_stru=BB_GET_OBJ_STRU(cell_obj);
    return BB_GET_TABLE_FIELD_NAME_ID_STRU_LEN(&cell_stru,field_str,field_len);
}

int BB_GET_TABLE_FIELD_NAME_ID_STRU_LEN(BB_OBJ_STRU *cell_obj_stru,const char *field_str,BB_SIZE_T field_len){//ly cell_name;ly task此处可以提速
    BYTE *item;
    BB_SIZE_T item_size;
    
    BYTE t;
    for(BYTE i=0; i< cell_obj_stru->Num; i++){
        if (!BB_GET_TABLE_FIELD_NAME_STRU(cell_obj_stru,i,&item,&item_size))
            return -1;
        t=field_len<=item_size ? field_len : item_size;
        if ((strncmp(field_str,(const char *)item,t)==0))
            return i;
    }
    return -1;
}

BYTE BB_CONDITIONS_GET_KEYS(BYTE *conditions, BB_SIZE_T conditions_size, BB_SIZE_T *key, BYTE key_max_size,BB_OBJ_STRU *table_stru){//头（a4-1包含key_width+扩展字段宽+key(UINTn)+value_point）//ly task change
    BYTE i,*p_obj,*value,condition;
    BB_SIZE_T *p_key;
    BB_SIZE_T value_size;
    
    for(p_obj=conditions, p_key=key,i=0; (((size_t)(p_obj-conditions))<conditions_size) && (((size_t)(p_key-key))<=key_max_size); p_obj+=BB_GET_OBJ_SIZE(p_obj),p_key++,i++){
        //循环读取conditions的组合
        if (p_obj==NULL || !BB_PARA_GET_KEY_VALUE((BB_OBJ *)p_obj,p_key,&value,&value_size,&condition,table_stru))
        break;
    }
    return i;
}

bool BB_PARA_GET_KEY_VALUE(BB_OBJ *para_obj, BB_SIZE_T *key, BYTE **value, BB_SIZE_T *value_size, BYTE *condition,BB_OBJ_STRU *table_stru) {//头（a4-1包含key_width+扩展字段宽+key(UINTn)+value_point）//ly task change;
    BYTE type = BB_GET_OBJ_TYPE(para_obj);
    if (type == BB_PARA_EXT) {
        BYTE key_width = _get_cell_uint_mark_width(para_obj);
        (*key) = _scanUINT((para_obj + BB_OBJ_STX_EXT_LEN), key_width);
        (*value) = (para_obj + BB_OBJ_STX_EXT_LEN + key_width);
        (*value_size) = *(para_obj + BB_OBJ_STX_EXT_LEN - 1);
        (*condition) = (*para_obj) & BB_PARA_CONDITION_COMP_MASK;
        return true;
    }
    else if (type == BB_ARRAY) {//ly 910
        BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(para_obj);
        if (obj_stru.Num != 3) {
            printf("%s", "BB_PARA_GET_KEY_VALUE()'s input para_obj_type must be BB_PARA_EXT or BB_ARRAY[***,=,***]");
            return false;
        }
        //取得key
        BYTE *key_p;
        BB_SIZE_T key_size;
        BB_GET_ARRAY_REC_BY_NUM_STRU(&obj_stru, 0, &key_p, &key_size);
        if (key_size==1)
            (*key) = _scanUINT(key_p, key_size); //0x12==***类的处理。
        else if ((*key_p)=='#' && (table_stru!=NULL)){//ly cell_name ly task
            //cell 字段转换
            int n;
            if ((n=BB_GET_TABLE_FIELD_NAME_ID_LEN(table_stru->CellObj,(const char *)(key_p+1),key_size-1))<0){//!!!此处可以提速：改为一次性取出CellObjectArray;
                return false;
            }
            (*key)=n;
        }else {//如果是"1之类"，今后改为直接数字
            char tmp[21];
            memcpy((void *)tmp, (const void *)key_p, key_size > 20 ? 20 : key_size);
            tmp[key_size] = 0;
            (*key) = atoi(tmp);
        }
        
        //取得conditions
        BYTE *cd;
        BB_SIZE_T cd_size;
        BB_GET_ARRAY_REC_BY_NUM_STRU(&obj_stru, 1, &cd, &cd_size);
        for (unsigned int i = 0; i < cd_size; i++)
            if ((*cd) == ' ')cd++;//去除空格
        if ((*cd) == '=') {
            if(*(cd+1)=='='){
                if(*(cd+2)=='=')
                    (*condition) = BB_PARA_CONDITION_STR_INCLUDE; //ly search2
                else
                    (*condition) = BB_PARA_CONDITION_EQUAL;
            }else
                (*condition) = BB_PARA_CONDITION_STR_EQUAL;
        }
        else if ((*cd) == '>')
            (*condition) = BB_PARA_CONDITION_GT;
        else if ((*cd) == '~')
            (*condition) = BB_PARA_CONDITION_STR_INCLUDE; //ly search2
        else if ((*cd) == '<')
            (*condition) = BB_PARA_CONDITION_LT;
        else if ((*cd) == '!' && (*(cd+1))=='=')
            (*condition) = BB_PARA_CONDITION_NOT_EQUAL;
        else {
            printf("%s", "BB_PARA_GET_KEY_VALUE()'s input BB_ARRAY[***,=,***] should be one of  = > < !=");
            return false;
        }
        //取得value
        BB_GET_ARRAY_REC_BY_NUM_STRU(&obj_stru, 2, value, value_size);
    }
    else {
        printf("%s", "BB_PARA_GET_KEY_VALUE()'s input para_obj_type must be BB_PARA_EXT or BB_ARRAY[***,=,***]");
        return false;
    }
    return true;
}


bool BB_PARA_CONDITION_TEST(BB_OBJ *para_obj, BYTE *item, BB_SIZE_T item_size, BYTE *field_type,BB_OBJ_STRU *table_stru) {//0:Absolutely equal,1:include
    //BYTE type;
    //if ((type=BB_GET_OBJ_TYPE(para_obj)) != BB_PARA_EXT && (type !=BB_ARRAY)) {
    //	printf("%s", "Error:BB_PARA_CONDITION_TEST()input obj type must be BB_PARA_EXT!");
    //	return false;
    //};
    //取得PARA值
    BB_SIZE_T key;
    BYTE *value_str,condition=0;
    BB_SIZE_T value_size;
    BB_PARA_GET_KEY_VALUE(para_obj, &key, &value_str, &value_size,&condition,table_stru);//ly 910 add:condition
    
    //key_value 和item比较,method：0绝对等于或1包含
    
    //逐字段进行比较
    UInt64 src_num, cmp_num;
    BB_SIZE_T i;
    //???应增加字串和数值比较的区分???
    switch (condition) {
        case BB_PARA_CONDITION_EQUAL:
            if (value_size != item_size)
                return false;
            for (i = 0; i<value_size; i++) {
                if (value_str[i] != item[i])
                    return false;
            }
            break;
        case BB_PARA_CONDITION_STR_EQUAL:
            if ((*field_type) == BB_TABLE_FIELD_TYPE_STRING) {
                for (i= 0; (i<value_size) && (i<item_size) ; i++) {
                    if (value_str[i] != item[i])
                        return false;
                    if ((value_str[i] == 0) && (item[i] == 0))
                        break;
                }
            }/*else if ((*field_type) == BB_TABLE_FIELD_TYPE_BINARY){
              if (value_size > item_size)
              return false;
              for (i = 0; i<value_size; i++) {
              if (value_str[i] != item[i])
              return false;
              }
              if (value_size < item_size)
              for (; i < item_size; i++) {//对于二者不相等但是后续为空的情况，还是可以相等的
              if (item[i] != 0)
              return false;
              }
              }*/
            else {
                if (value_size != item_size)
                    return false;
                for (i = 0; i<value_size; i++) {
                    if (value_str[i] != item[i])
                        return false;
                }
            }
            break;
        case BB_PARA_CONDITION_STR_INCLUDE: //ly search2
            if ((*field_type) == BB_TABLE_FIELD_TYPE_STRING) {//ly ??? 需要添加
                for (i= 0; (i<value_size) && (i<item_size) ; i++) {
                    if (value_str[i] != item[i])
                        return false;
                    if ((value_str[i] == 0) && (item[i] == 0))
                        break;
                }
            }else {//先做这个
                //针对BB_PARA_GET_KEY_VALUE->BB_GET_ARRAY_REC_BY_NUM_STRU->if (!BB_IS_OBJ_STRING(item_obj) && (BB_GET_OBJ_TYPE(item_obj) == BB_ARRAY || BB_GET_OBJ_TYPE(item_obj) == BB_TABLE))的补充处理，应该只是BB_IS_BYTES_ALL才能检出第三层JsonBArrayToBArray->SERACH打包
                if ((value_size + 2) > BB_0DIM_BYTES_EXT_MAX_LEN && ((*value_str)==0x80)) {//递进超长字串
                    value_size= BB_GET_OBJ_DATA_LEN(value_str);
                    value_str=(BYTE *)BB_GET_OBJ_DATA(value_str);
                }
                if ((value_size % item_size)!=0)
                    return false;
                unsigned long step=value_size/item_size;
                for(unsigned long j=0; j<value_size; j+=item_size){
                    bool is_equal=true;
                    for (i = 0; i<item_size; i++) {
                        if (value_str[j+i] != item[i]){
                            is_equal=false;
                            break;
                        }
                    }
                    if (is_equal)
                        return true;
                }
                return false;
            }
            break;
        case BB_PARA_CONDITION_GT://大于
            src_num = _scanUINT(item, item_size);
            cmp_num = _scanUINT(value_str, value_size);
            if (src_num>cmp_num)
                break;
            else
                return false;
        case BB_PARA_CONDITION_LT://小于
            src_num = _scanUINT(item, item_size);
            cmp_num = _scanUINT(value_str, value_size);
            if (src_num<cmp_num)
                break;
            else
                return false;
        case BB_PARA_CONDITION_NOT_EQUAL://不等于
            src_num = _scanUINT(item, item_size);
            cmp_num = _scanUINT(value_str, value_size);
            if (src_num != cmp_num)
                break;
            else
                return false;
            //break;
            break;
    };
    
    return true;
}

bool BB_TABLE_PARA_CONDITION_TEST(BB_OBJ_STRU *table_stru,BB_SIZE_T num,BB_OBJ *para_obj,int &err_code) {//0:Absolutely equal,1:include
    //取得PARA值
    err_code=0;
    BB_SIZE_T key;
    BYTE *value_str,condition=0;
    BB_SIZE_T value_size;
    BB_PARA_GET_KEY_VALUE(para_obj, &key, &value_str, &value_size,&condition,table_stru);//ly 910 add:condition
    
    //取得table对应字段的值
    BYTE *item,field_type;
    BB_SIZE_T item_size;
    if (!BB_GET_TABLE_ITEMS_DATA_TYPE_VALUE(table_stru, num,key,&item,&item_size,&field_type)){
        err_code=-1;
        return false;
    }
    //逐字段进行比较
    UInt64 src_num, cmp_num;
    BB_SIZE_T i;
    //???应增加字串和数值比较的区分???
    switch (condition) {
        case BB_PARA_CONDITION_EQUAL:
            if (value_size != item_size)
                return false;
            for (i = 0; i<value_size; i++) {
                if (value_str[i] != item[i])
                    return false;
            }
            break;
        case BB_PARA_CONDITION_STR_EQUAL:
            if (field_type == BB_TABLE_FIELD_TYPE_STRING) {
                for (i= 0; (i<value_size) && (i<item_size) ; i++) {
                    if (value_str[i] != item[i])
                        return false;
                    if ((value_str[i] == 0) && (item[i] == 0))
                        break;
                }
            }/*else if ((*field_type) == BB_TABLE_FIELD_TYPE_BINARY){
              if (value_size > item_size)
              return false;
              for (i = 0; i<value_size; i++) {
              if (value_str[i] != item[i])
              return false;
              }
              if (value_size < item_size)
              for (; i < item_size; i++) {//对于二者不相等但是后续为空的情况，还是可以相等的
              if (item[i] != 0)
              return false;
              }
              }*/
            else {
                if (value_size != item_size)
                    return false;
                for (i = 0; i<value_size; i++) {
                    if (value_str[i] != item[i])
                        return false;
                }
            }
            break            ;
        case BB_PARA_CONDITION_GT://大于
            src_num = _scanUINT(item, item_size);
            cmp_num = _scanUINT(value_str, value_size);
            if (src_num>cmp_num)
                break;
            else
                return false;
        case BB_PARA_CONDITION_LT://小于
            src_num = _scanUINT(item, item_size);
            cmp_num = _scanUINT(value_str, value_size);
            if (src_num<cmp_num)
                break;
            else
                return false;
        case BB_PARA_CONDITION_NOT_EQUAL://不等于
            src_num = _scanUINT(item, item_size);
            cmp_num = _scanUINT(value_str, value_size);
            if (src_num != cmp_num)
                break;
            else
                return false;
            //break;
            break;
    };
    
    return true;
}


/*
 bool BB_PARA_GET_KEY_VALUE(BB_OBJ *para_obj, UInt64 *key, BYTE **value, BB_SIZE_T *value_size){//头（a4-1包含key_width+扩展字段宽+key(UINTn)+value_point）
	if (BB_GET_OBJ_TYPE(para_obj)!=BB_PARA_EXT){
 printf("%s","BB_PARA_GET_KEY_VALUE()'s input para_obj_type must be BB_PARA_EXT");
 return false;
	}
	BYTE key_width=_get_cell_uint_mark_width(para_obj);
	(*key)= _scanUINT((para_obj+BB_OBJ_STX_EXT_LEN),key_width);
	(*value)=(para_obj+BB_OBJ_STX_EXT_LEN+key_width);
	(*value_size)=*(para_obj+BB_OBJ_STX_EXT_LEN-1);
	return true;
 }
 
 bool BB_PARA_CONDITION_TEST(BB_OBJ *para_obj, BYTE *item, BB_SIZE_T item_size){//0:Absolutely equal,1:include
	if (BB_GET_OBJ_TYPE(para_obj)!=BB_PARA_EXT){
 printf("%s","Error:BB_PARA_CONDITION_TEST()input obj type must be BB_PARA_EXT!");
 return false;
	};
	//取得PARA值
	UInt64 key;
	BYTE *condition_str;
	BB_SIZE_T condition_size;
	BB_PARA_GET_KEY_VALUE(para_obj,&key,&condition_str,&condition_size);
 
	//key_value 和item比较,method：0绝对等于或1包含
	
	//逐字段进行比较
	UInt64 src_num,cmp_num;
	//???应增加字串和数值比较的区分???
	switch((*para_obj) & BB_PARA_CONDITION_COMP_MASK){
 case BB_PARA_CONDITION_EQUAL:
 {
 if (condition_size!=item_size)
 return false;
 BB_SIZE_T i;
 for(i=0; i<condition_size; i++){
 if(condition_str[i]!=item[i])
 return false;
 }
 }
 break;
 case BB_PARA_CONDITION_GT://大于
 src_num=_scanUINT(item,item_size);
 cmp_num=_scanUINT(condition_str,condition_size);
 if (src_num>cmp_num)
 break;
 else
 return false;
 case BB_PARA_CONDITION_LT://小于
 src_num=_scanUINT(item,item_size);
 cmp_num=_scanUINT(condition_str,condition_size);
 if (src_num<cmp_num)
 break;
 else
 return false;
 case BB_PARA_CONDITION_NOT_EQUAL://不等于
 src_num=_scanUINT(item,item_size);
 cmp_num=_scanUINT(condition_str,condition_size);
 if (src_num!=cmp_num)
 break;
 else
 return false;
 break;
 break;
	};
 
	return true;
 }
 */

/************************1维通用数组操作**************************/
BB_OBJ *BB_NEW_ARRAY(BB_OBJ *cell_obj, BB_OBJ *index_obj, BB_OBJ *add_info_obj, BB_SIZE_T in_max_len, const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf){
    //1 dimension object: Header(1Byte)+CellObj+IndexObj+Content; AllInfoDB:Len;MaxLen,AddInfo,Sort
    BB_SIZE_T max_len=0,index_max_len=0;
    BB_OBJ *obj;
    BB_OBJ header=0;
    
    //设置数组类型，取得max_len;()
    BB_OBJ_STRU cell_stru=BB_GET_OBJ_STRU(cell_obj);
    BYTE len_obj_uint_type=8;
    UInt64 max_index_num_len;
    BB_SIZE_T cell_width=0,input_max_len=0;
    if ((index_obj!=NULL)&& (*index_obj)!=BB_NULL)
        max_index_num_len=(UInt64)pow((double)256,BB_GET_ARRAY_INDEX_OBJ_WIDTH(index_obj));//用于计算len_obj_uint_type的字宽
    else
        max_index_num_len=(UInt64)pow((double)256,4);
    
    switch(cell_stru.Type){
        case BB_BYTES_EXT:
            header=BB_ARRAY;
            input_max_len=in_max_len;
            if (in_max_len==0)
                len_obj_uint_type=_sizeUINT(data_len);//对于没有max_len的变长数组,紧凑存放len_obj。
            //printf("Note: BB_BYTES_EXT ARRAY MAX_LEN is the orign data length! and not indeep test");
            break;
        case BB_BYTES:
        case BB_NULL:
            //如果是对象数组：ARRAY[OBJ|BYTES]; 0x00==BB_NULL;
            header=BB_ARRAY;
            input_max_len=in_max_len*BB_0DIM_BYTES_MAX_LEN;
            if (in_max_len==0)
                len_obj_uint_type=_sizeUINT(data_len);//对于没有max_len的变长数组紧凑存放len_obj,。
            break; 
        case BB_UINT8:
            //如果是普通数组：ARRAY[UINT|MALLOC|REDIR]
            header = BB_ARRAY;
            if (index_obj == NULL && in_max_len == 0) {
                input_max_len = data_len;
                index_max_len = 0;
                len_obj_uint_type = _sizeUINT(data_len);//对于没有max_len的变长数组紧凑存放len_obj,。
            }
            else {
                input_max_len = cell_stru.DataLen *in_max_len; //ly max //我们可以从max_len 或index的max_len中取得maxlen; 但建议都是max_len
                index_max_len = cell_stru.DataLen * BB_GET_ARRAY_INDEX_NUM(index_obj);
                //len_obj的uint type
                len_obj_uint_type = _sizeUINTType(_sizeUINT(max_index_num_len) + cell_stru.Type);
            }
            break;
        case BB_UINT16:		case BB_UINT32:		case BB_UINT64:		case BB_MALLOC:		case BB_REDIR:
            //如果是普通数组：ARRAY[UINT|MALLOC|REDIR]
            header=BB_ARRAY;
            input_max_len=cell_stru.DataLen *in_max_len; //ly max //我们可以从max_len 或index的max_len中取得maxlen; 但建议都是max_len
            index_max_len=cell_stru.DataLen * BB_GET_ARRAY_INDEX_NUM(index_obj);   //ly malloc test
            //len_obj的uint type
            len_obj_uint_type=_sizeUINTType(_sizeUINT(max_index_num_len)+cell_stru.Type);
            break;
        case BB_ARRAY:
        case BB_TABLE:
            //如果是表格：ARRAY_TABLE!!!!!
            header=BB_TABLE;
            cell_width=BB_GET_TABLE_CELL_WIDTH(cell_obj);//to_add
            input_max_len=cell_width *in_max_len;
            index_max_len=cell_width * BB_GET_ARRAY_INDEX_NUM(index_obj);//我们可以从max_len 或index的max_len中取得maxlen
            //len_obj的uint type
            
            len_obj_uint_type=_sizeUINTType(_sizeUINT(max_index_num_len)+_sizeUINT(cell_width));
            break;
    }
    
    
    //创建len_obj对象。
    BB_UINT_STRU len_obj_stru=BB_NEW_CELL_UINT((UInt64)data_len,len_obj_uint_type);
    BB_OBJ *len_obj=(BB_OBJ *)&len_obj_stru;
    
    //创建max_len_obj对象
    BB_UINT_STRU max_len_obj_stru;
    BB_OBJ *max_len_obj=NULL;
    max_len = _get_max_value(index_max_len,input_max_len,data_len);
    if (max_len>data_len){//ly max
        max_len_obj_stru=BB_NEW_CELL_UINT((UInt64)max_len,len_obj_uint_type);
        max_len_obj=(BB_OBJ *)&max_len_obj_stru;
    }
    
    //创建对象空间（header+content），：creat object space
    BB_SIZE_T len_size=BB_GET_OBJ_SIZE(len_obj);
    BB_SIZE_T cell_size=BB_GET_OBJ_SIZE(cell_obj);//to_test
    BB_SIZE_T header_len = BB_OBJ_STX_LEN+len_size+cell_size;
    BB_SIZE_T index_size=0;
    if (index_obj!=NULL){
        index_size=BB_GET_OBJ_SIZE(index_obj);
        header_len += index_size;
        header=header|BB_MASK_IndexObj;
    }
    if (add_info_obj!=NULL){
        header_len+=BB_GET_OBJ_SIZE(add_info_obj);//对应长度
        header=header|BB_MASK_AddInfo;//添加头标记
    }
    if (max_len>data_len){
        header_len+=BB_GET_OBJ_SIZE(max_len_obj);
        header=header|BB_MASK_MaxLenObj;
    }
    obj= (p_buf==NULL) ? (BB_OBJ *)malloc(header_len+max_len) :p_buf;
    
    //拷贝头文件：copy header;
    (*obj)=header;
    BB_OBJ *add_pos=obj+BB_OBJ_STX_LEN;
    add_pos=BB_OBJ_COPY(add_pos, len_obj);
    add_pos=BB_OBJ_COPY(add_pos, cell_obj);//to_test
    if (index_obj!=NULL)
        add_pos=BB_OBJ_COPY(add_pos, index_obj);
    if (add_info_obj!=NULL)
        add_pos=BB_OBJ_COPY(add_pos, add_info_obj);
    if (max_len>data_len)
        add_pos=BB_OBJ_COPY(add_pos, max_len_obj);
    
    //拷贝数据内容
    if (data!=NULL)
        memcpy((void*)(obj+header_len),data,data_len);//??是否需要清理空间?
    
    //测试
    if (max_len>data_len){//清空空余，方便调试,应采用memset
        memset(obj+header_len+data_len, 0,max_len-data_len);
    }
    //只是测试 ly:效率删除
    //const char * new_data=(const char *)obj+header_len;
    //const char * new_data2=new_data+1;
    //const char * new_data3=new_data+2;
    
    //BB_OBJ_STRU temp=BB_GET_OBJ_STRU(obj);
    
    return obj;
}
BB_OBJ *BB_NEW_ARRAY_CELL(BYTE cell_type, BYTE index_type, BB_OBJ *add_info_obj, BB_SIZE_T input_max_len, const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf){
    BB_UINT_STRU cell=BB_NEW_CELL_UINT(0,cell_type);
    BB_UINT_STRU index=BB_NEW_CELL_UINT(0,index_type);
    return BB_NEW_ARRAY((BB_OBJ *)&cell,(BB_OBJ *)&index,add_info_obj,input_max_len,data,data_len,p_buf);
};
BB_OBJ *BB_NEW_ARRAY_INDEX_CELL(BYTE cell_type, BB_OBJ *array_obj, BB_OBJ *add_info_obj, BB_SIZE_T input_max_len, const void *data, BB_SIZE_T data_len, BB_OBJ *p_buf){
    BB_UINT_STRU cell=BB_NEW_CELL_UINT(0,cell_type);
    return BB_NEW_ARRAY((BB_OBJ *)&cell,array_obj,add_info_obj,input_max_len,data,data_len,p_buf);
}
BB_SIZE_T BB_GET_ARRAY_NUM_STRU(BB_OBJ_STRU *obj_stru){//ly max; 本函数当前只用于BB_GET_OBJ_STRU().
    
    BB_SIZE_T cell_width=0;
    if ((*obj_stru).Type==BB_ARRAY)
        cell_width = BB_GET_OBJ_DATA_LEN((*obj_stru).CellObj);
    else if ((*obj_stru).Type==BB_TABLE)
        cell_width = BB_GET_TABLE_CELL_WIDTH((*obj_stru).CellObj);
    else {
        printf("Error: BB_GET_ARRAY_NUM_STRU() input is not Array (%d)!",(int) obj_stru->Type);
    }
    
    
    BYTE cell_type=BB_GET_OBJ_TYPE((*obj_stru).CellObj);
    
    //如果是定长则直接计算；如果是Obj数组则轮巡或者从INdex中读取。
    if ((BB_IS_UINT((*obj_stru).CellObj) || cell_type==BB_MALLOC || (*obj_stru).Type==BB_TABLE) && cell_width!=0){
        //定长记录直接计算
        return (BB_SIZE_T)BB_GET_UINT_VALUE((*obj_stru).LenObj)/cell_width;//只对定长表格有作用 ly malloc_test
    }else if ((*obj_stru).IndexObj!=NULL){
        //从indexobj取得
        return (BB_SIZE_T)BB_GET_ARRAY_INDEX_NUM((*obj_stru).IndexObj);//ly max ly obj
    }else if (cell_type==BB_NULL || cell_type==BB_BYTES || cell_type==BB_BYTES_EXT){
        //ly obj 则顺序计算
        BB_OBJ *bytes_obj_p=(*obj_stru).Data; //form BB_BYTES_ARRAY:
        BB_SIZE_T num=0;
        for(;bytes_obj_p<((*obj_stru).Data + (*obj_stru).DataLen) ;bytes_obj_p+=BB_GET_OBJ_SIZE(bytes_obj_p)){//注意没有边界测试，如果需要见下
            num++;
        }
        //ly ?? 建议在此处增加IndexObj的补充。
        return num;
    }else
        return 0;
}

bool BB_GET_ARRAY_OBJ_NUM_CHECK_STRU(BB_OBJ_STRU *obj_stru,BB_SIZE_T *num, BB_SIZE_T max){//ly max; 本函数当前只用于BB_GET_OBJ_STRU().
    
    //BB_SIZE_T cell_width=0;
    
    //if ((*obj_stru).Type==BB_ARRAY)
    //	cell_width = BB_GET_OBJ_DATA_LEN((*obj_stru).CellObj);
    //else
    //return false;
    if ((*obj_stru).Type!=BB_ARRAY)
        return false;
    
    BYTE cell_type=BB_GET_OBJ_TYPE((*obj_stru).CellObj);
    
    //如果是定长则直接计算；如果是Obj数组则轮巡或者从INdex中读取。
    if (!(cell_type==BB_NULL || cell_type==BB_BYTES || cell_type==BB_BYTES_EXT))
        return false;//定长记录直接计算
    if ((*obj_stru).IndexObj!=NULL){
        //从indexobj取得
        (*num)=(BB_SIZE_T)BB_GET_ARRAY_INDEX_NUM((*obj_stru).IndexObj);//ly max ly obj
    }else{
        //ly obj 则顺序计算
        BB_OBJ *bytes_obj_p=(*obj_stru).Data; //form BB_BYTES_ARRAY:
        (*num)=0;
        for(;bytes_obj_p<((*obj_stru).Data + (*obj_stru).DataLen) ;bytes_obj_p+=BB_GET_OBJ_SIZE(bytes_obj_p)){
            (*num)++;
            //测试合法性
            //if ((BYTE)(*bytes_obj_p)>0x9f)
            //	return false;
            if ((*num) > max)
                return false;
            //增加操作
        }
        //ly ?? 建议在此处增加IndexObj的补充。
    }
    return true;
}

BB_SIZE_T BB_GET_ARRAY_NUM(BB_OBJ *obj){//ly max
    BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
    
    return BB_GET_ARRAY_NUM_STRU(&obj_stru);
}
BB_SIZE_T BB_GET_ARRAY_INDEX_NUM(BB_OBJ *obj){
    //using for comput width length of Dimensions
    //get char* content;
    if (obj==NULL)
        return 0;
    
    BB_SIZE_T num=0;
    switch(BB_GET_OBJ_DIM(obj))
    {
        case 0xFF:
            num=0;
            break;
        case 0:
            //0 dimension:
            num = (BB_SIZE_T)BB_GET_UINT_VALUE(obj);
            break;
        case 1:
            //在此添加对ARRAY
            return BB_GET_ARRAY_INDEX_NUM(BB_GET_OBJ_STRU(obj).IndexObj);
            break;
    }
    return num;
}
BYTE BB_GET_ARRAY_INDEX_OBJ_WIDTH(BB_OBJ *obj){//get the index width of array, just for search/get
    //循环取得index节点，直到发现CELL类节点。
    BYTE *pos;
    
    //if (BB_GET_OBJ_TYPE(obj)!=BB_ARRAY)
    //return 0;
    if (BB_GET_OBJ_TYPE(obj)==BB_NULL)
        return 0;
    
    if (BB_GET_OBJ_DIM(obj)==0)
        if (BB_IS_UINT(obj))
            return BB_GET_CELL_DATA_LEN(obj);
        else
            return 0;  //出错不应该出现非UINT内容。
        else{
            pos=obj+1;//取得LenObj信息
            pos+=BB_GET_OBJ_SIZE(pos); //取得//取得CellObj(?? indexObj)的起始信息的起始信息
            return BB_GET_ARRAY_INDEX_OBJ_WIDTH(pos);//循环测试indexObj)(CellObj ??)的信息
        }
};

//计算数值数组的和。
bool BB_GET_ARRAY_UINT_VALUE(BB_OBJ* obj,UInt32 i,UInt64 *ret){
    BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
    return BB_GET_ARRAY_UINT_VALUE_STRU(&obj_stru,i,ret);
}
bool BB_GET_ARRAY_UINT_VALUE_STRU(BB_OBJ_STRU *obj_stru,BB_SIZE_T i,UInt64 *ret){//今后应添加更好的差错提示
    BYTE cell_width;
    //BB_SIZE_T sum=0;
    
    //必须是数组
    if ((*obj_stru).Type!=BB_ARRAY)
        return false;
    //必须是整数数组
    switch(BB_GET_OBJ_TYPE((*obj_stru).CellObj)){
        case BB_UINT8:
        case BB_UINT16:
        case BB_UINT32:
        case BB_UINT64:
            cell_width = BB_GET_CELL_DATA_LEN((*obj_stru).CellObj);
            (*ret)=_scanUINT((*obj_stru).Data+i*cell_width,cell_width);
            //printf("|%d*%d,", cell_width, (*ret));
            return true;
        default:
            return false;
    };
}

bool BB_GET_ARRAY_UINT_VALUE_STRU_DEBUG(BB_OBJ_STRU *obj_stru, UInt32 i, UInt64 *ret) {//今后应添加更好的差错提示
    BYTE cell_width;
    //BB_SIZE_T sum = 0;
    BYTE *tmp;
    
    //必须是数组
    if ((*obj_stru).Type != BB_ARRAY)
        return false;
    //必须是整数数组
    switch (BB_GET_OBJ_TYPE((*obj_stru).CellObj)) {
        case BB_UINT8:
        case BB_UINT16:
        case BB_UINT32:
        case BB_UINT64:
            cell_width = BB_GET_CELL_DATA_LEN((*obj_stru).CellObj);
            //(*ret) = _scanUINT((*obj_stru).Data + i*cell_width, cell_width);
            tmp = (*obj_stru).Data + i*cell_width;
            (*ret) = (UInt64)(*tmp);
            printf("(%d*%d*%d*%d)", (int)cell_width, (int)(*tmp),(int)(*ret),(int)i);
            return true;
        default:
            return false;
    };
    return true;
}
bool BB_GET_ARRAY_UINT_MAX_STRU(BB_OBJ_STRU *obj_stru,UInt64 *ret){//今后应添加更好的差错提示
    BYTE cell_width;
    //BB_SIZE_T sum=0;
    int i;
    UInt64 value;
    
    (*ret)=0;
    //必须是数组
    if ((*obj_stru).Type!=BB_ARRAY)
        return false;
    int len=BB_GET_ARRAY_NUM(obj_stru->ThisObj);
    //必须是整数数组
    switch(BB_GET_OBJ_TYPE((*obj_stru).CellObj)){
        case BB_UINT8:
        case BB_UINT16:
        case BB_UINT32:
        case BB_UINT64:
            cell_width = BB_GET_CELL_DATA_LEN((*obj_stru).CellObj);
            for(i=0; i<len; i++){
                if ((value=(BB_SIZE_T)_scanUINT((*obj_stru).Data+i*cell_width,cell_width)) > (*ret))
                    (*ret)=value;
            }
            return true;
        default:
            return false;
    };
    return true;
}
//计算数值数组的和。
bool BB_GET_ARRAY_UINT_MAX(BB_OBJ* obj,UInt64 *ret){
    BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
    return BB_GET_ARRAY_UINT_MAX_STRU(&obj_stru,ret);
}
bool BB_GET_ARRAY_REC_BY_NUM_STRU(BB_OBJ_STRU *obj_stru,BB_SIZE_T num, BYTE **item, BB_SIZE_T *item_size){//locate itme inside obj
    BB_OBJ_STRU cell_stru = BB_GET_OBJ_STRU((*obj_stru).CellObj);
    BB_OBJ *bytes_obj_p; //form BB_BYTES_ARRAY:
    BB_SIZE_T i=0;
    BB_OBJ *item_obj;
    BB_SIZE_T item_obj_size;
    
    switch ((*obj_stru).Type){
        case BB_ARRAY:
            //！！？？目前只支持定长数组
            switch (cell_stru.Type){
                case BB_UINT8:		case BB_UINT16:		case BB_UINT32:		case BB_UINT64:		case BB_MALLOC:
                    break;
                case BB_BYTES:
                case BB_BYTES_EXT:
                case BB_NULL:
                    //非定长数据
                    bytes_obj_p=(*obj_stru).Data; //取得元素宽度;
                    for(;bytes_obj_p<((*obj_stru).Data + (*obj_stru).DataLen); bytes_obj_p+=item_obj_size,i++){//ly max; ly obj
                        item_obj=(BB_OBJ *)bytes_obj_p;
                        item_obj_size=BB_GET_OBJ_SIZE(bytes_obj_p);
                        if (num==i){
                            if (!BB_IS_OBJ_STRING(item_obj) && (BB_GET_OBJ_TYPE(item_obj) == BB_ARRAY || BB_GET_OBJ_TYPE(item_obj) == BB_TABLE)) {
                                //多层数组对象转换
                                (*item) = (BYTE *)item_obj;
                                (*item_size) = BB_GET_OBJ_SIZE(item_obj);
                            }/*else if (BB_IS_UINT(item_obj)){
                                (*item) = (BYTE *)item_obj;
                                (*item_size) = BB_GET_OBJ_SIZE(item_obj);
                            }*/else {
                                //BYTES对象转换,包括ARRAY_OBJ_STRING
                                (*item)=(BYTE *)BB_GET_OBJ_DATA(item_obj);
                                (*item_size)= BB_GET_OBJ_DATA_LEN(item_obj);
                            }
                            return true;
                        }
                    }
                    //printf("BB_GET_ARRAY_REC_BY_NUM_STRU()->req_num!=src_num out of range! req_num=%zu,src_num=%zu",num,i);
                    return false;  //结果超出范围。
                default:
                    return false;
            };
            break;
        case BB_TABLE:
            break;
        default:
            return false;
    }
    
    //定长数据
    BB_SIZE_T pos = num *(*obj_stru).CellWidth;
    if ((pos+(*obj_stru).CellWidth) >(*obj_stru).DataLen){
        printf("%s,%zu,%zu","BB_GET_ARRAY_REC_BY_NUM_STRU()->pos+(*obj_stru).CellWidth out of range! ",(pos+(*obj_stru).CellWidth),(*obj_stru).DataLen);
        return false;  //结果超出范围。
    };
    if (cell_stru.Type!=BB_MALLOC){
        (*item) = (*obj_stru).Data+pos; //取得指针
        (*item_size) = (*obj_stru).CellWidth;//取得结果
    }else{
        //取得malloc跳转表格对象 ly malloc !!!应加入类型的判断，或者直接取得STL对象化内容
        BYTE *table_malloc=(*obj_stru).Data+pos;
        BB_SIZE_T tmp1=*((BB_SIZE_T*)table_malloc);
        (*item) = (BYTE *)(tmp1);
        if ((*item)!=0){
            (*item_size)=BB_GET_OBJ_SIZE((*item));
        }else{
            (*item_size)=0;
            return false;
        }
    }
    return true;
};

bool BB_GET_MALLOC_ADDRESS(BB_OBJ *obj, void **item, BYTE *type) { //ly malloc new
    if (BB_GET_OBJ_TYPE(obj) != BB_MALLOC) {
        printf("BB_GET_MALLOC_ADDRESS(obj)=%d obj is not BB_MALLOC! ",BB_GET_OBJ_TYPE(obj));
        return false;
    }
    //取得malloc跳转表格对象 ly malloc !!!应加入类型的判断，或者直接取得STL对象化内容
    //BB_SIZE_T value_len = BB_GET_CELL_DATA_LEN(obj); //ly
    BYTE *value = (BYTE *)BB_GET_CELL_DATA(obj);
    BB_SIZE_T tmp1 = *((BB_SIZE_T*)value);
    if (tmp1 == 0)
        return false;
    
    (*item) = (void *)(tmp1);
    (*type) = (*(obj + 1));
    return true;
}

bool BB_SET_MALLOC_ADDRESS(BB_OBJ *obj, const void *address, BYTE address_size, BYTE type) { //ly malloc new
    //test object type
    if (BB_GET_OBJ_TYPE(obj) != BB_MALLOC) {
        printf("BB_SET_MALLOC_ADDRESS(obj, address, address_size) obj is not BB_MALLOC! ");
        return false;
    }
    
    //取得malloc跳转表格对象 ly malloc !!!应加入类型的判断，或者直接取得STL对象化内容
    BB_SIZE_T value_len = BB_GET_CELL_DATA_LEN(obj);
    BYTE *value = (BYTE *)BB_GET_CELL_DATA(obj);
    if (value_len != address_size) {
        printf("BB_SET_MALLOC_ADDRESS(obj, address, address_size) address_size != BB_GET_CELL_DATA_LEN(obj)! ");
        return false;
    }
    
    //设置类型 set ext object type
    if (type!=0xFF)
        (*(obj + 1)) = type;
    
    //设置操作
    memcpy((void*)(value), address, address_size);
    return true;
}

bool BB_GET_ARRAY_BY_NUM_NOREDIR_STRU(BB_OBJ_STRU *obj_stru,BB_SIZE_T num, BYTE **item, BB_SIZE_T *item_size){//locate itme inside obj
    BB_OBJ_STRU cell_stru = BB_GET_OBJ_STRU((*obj_stru).CellObj);
    BB_OBJ *bytes_obj_p; //form BB_BYTES_ARRAY:
    BB_SIZE_T i=0;
    BB_OBJ *item_obj;
    BB_SIZE_T item_obj_size;
    
    switch ((*obj_stru).Type){
        case BB_ARRAY:
            //！！？？目前只支持定长数组
            switch (cell_stru.Type){
                case BB_UINT8:		case BB_UINT16:		case BB_UINT32:		case BB_UINT64:		case BB_MALLOC: //ly malloc_test
                    break;
                case BB_BYTES:
                case BB_BYTES_EXT:
                case BB_NULL:
                    //非定长数据
                    bytes_obj_p=(*obj_stru).Data; //取得元素宽度;
                    for(;bytes_obj_p<((*obj_stru).Data + (*obj_stru).DataLen); bytes_obj_p+=item_obj_size,i++){//ly max; ly obj
                        item_obj=(BB_OBJ *)bytes_obj_p;
                        item_obj_size=BB_GET_OBJ_SIZE(bytes_obj_p);
                        if (num==i){
                            //BYTES对象转换
                            (*item)=(BYTE *)BB_GET_OBJ_DATA(item_obj);
                            //(*item_size)=BB_GET_CELL_DATA_LEN(item_obj);
                            (*item_size)=BB_GET_OBJ_DATA_LEN(item_obj);//ly table_dir
                            return true;
                        }
                    }
                    printf("%s,req_num=%zu,src_num=%zu","BB_GET_ARRAY_BY_NUM_NOREDIR_STRU()->req_num!=src_num out of range! ",num,i);
                    return false;  //结果超出范围。
                default:
                    return false;
            };
            break;
        case BB_TABLE:
            break;
        default:
            return false;
    }
    
    //定长数据
    BB_SIZE_T pos = num *(*obj_stru).CellWidth;
    if ((pos+(*obj_stru).CellWidth) >(*obj_stru).DataLen){
        printf("%s,%zu,%zu","BB_GET_ARRAY_BY_NUM_NOREDIR_STRU()->pos+(*obj_stru).CellWidth out of range! ",(pos+(*obj_stru).CellWidth),(*obj_stru).DataLen);
        return false;  //结果超出范围。
    };
    (*item) = (*obj_stru).Data+pos; //取得指针
    (*item_size) = (*obj_stru).CellWidth;//取得结果
    return true;
};

bool BB_GET_ARRAY_OBJ_BY_NUM_STRU(BB_OBJ_STRU *obj_stru,BB_SIZE_T num, BB_OBJ **item, BB_SIZE_T *item_size){//locate itme inside obj
    BB_OBJ_STRU cell_stru = BB_GET_OBJ_STRU((*obj_stru).CellObj);
    BB_OBJ *bytes_obj_p; //form BB_BYTES_ARRAY:
    BB_SIZE_T i=0;
    BB_OBJ *item_obj;
    BB_SIZE_T item_obj_size;
    
    if ((*obj_stru).Type==BB_ARRAY && (cell_stru.Type==BB_BYTES || cell_stru.Type==BB_BYTES_EXT || cell_stru.Type==BB_NULL)){ //ly obj
        bytes_obj_p=(*obj_stru).Data; //取得元素宽度;
        for(;bytes_obj_p<((*obj_stru).Data + (*obj_stru).DataLen) ;bytes_obj_p+=item_obj_size,i++){
            item_obj=(BB_OBJ *)bytes_obj_p;
            item_obj_size=BB_GET_OBJ_SIZE(bytes_obj_p);
            if (num==i){
                //BYTES对象转换
                (*item)=item_obj;
                (*item_size)=BB_GET_OBJ_SIZE(item_obj);
                return true;
            }
        }
        //printf("%s,req_num=%d,src_num=%d","BB_GET_ARRAY_OBJ_BY_NUM()->req_num!=src_num out of range! ",num,i);
        return false;  //结果超出范围。
    }else
        return false;
};

bool BB_GET_ARRAY_OBJ_BY_NUM(BB_OBJ *obj,BB_SIZE_T num, BB_OBJ **item, BB_SIZE_T *item_size){//locate itme inside obj //ly table_dir:BYTE **item -> BB_OBJ **item;
    BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
    return BB_GET_ARRAY_OBJ_BY_NUM_STRU(&obj_stru,num,item,item_size);
};

bool BB_GET_ARRAY_ADD_INFO_UINT(BB_OBJ *add_info_obj, const char *key, UInt64 &num) {//ly table_dir
    string r_str;
    
    //取得对象KeyValue
    BB_OBJ *item;
    BB_SIZE_T item_size;
    //BB_SIZE_T rec;
    int rec=-1;  //ly mac
    if (add_info_obj == NULL)
        return false;
    if ((rec = BB_GET_ARRAY_NUM_BY_INDEX(add_info_obj, (BYTE *)key, strlen(key)))<0 ||!BB_GET_ARRAY_OBJ_BY_NUM(add_info_obj, rec, &item, &item_size)) {
        return false;
    }
    
    //取得num
    if (BB_IS_UINT(item)) {
        num = BB_GET_UINT_VALUE(item);
        return true;
    }else{
        return false;
    }
}

/*
 bool BB_GET_OBJ_BY_PATH(BB_OBJ *sobj, BB_OBJ *search_str_data, int size_search_str, BYTE **obj, BB_SIZE_T *obj_size) {//ly 910
	int num;
 
	//循环读取search_str内容，递进查询定位
	int sp_size = 0;
	(*obj) = NULL;
 
	BB_OBJ *sp0;
	sp0 = (BB_OBJ *)search_str_data;
 
	if ((*sp0) == BB_SEARCH_NULL) {
 (*obj) = sobj;
 (*obj_size) = BB_GET_OBJ_SIZE(sobj);
	}
 
	BB_OBJ *sp;
	for (sp = sp0; (sp - sp0) <= size_search_str && (*sp) != BB_SEARCH_NULL; sp += sp_size) {
 sp_size = BB_SEARCH_CELL_SIZE(sp);
 if (((sp - sp0) + sp_size) > size_search_str)
 break; //超出对象处理
 
 //如果是Content则按原方式查询，如果是分支则sobj为分支
 if ((*sp) >= 40) {
 BB_OBJ_STRU sobj_stru = BB_GET_OBJ_STRU(sobj);
 //如果是分支
 switch ((*sp) & 0xC0) {//11000000只流出a8,a7
 case BB_SEARCH_CELL_STX:
 if ((sobj = sobj_stru.CellObj) == NULL) {
 printf("BB_GET_OBJ_BY_PATH(),can't find obj(BB_SEARCH_CELL_STX)!");
 return false;
 }
 break;
 case BB_SEARCH_INDEX_STX:
 if ((sobj = sobj_stru.IndexObj) == NULL) {
 printf("BB_GET_OBJ_BY_PATH(),can't find obj(BB_SEARCH_INDEX_STX)!");
 return false;
 }
 break;
 case BB_SEARCH_ADDINFO_STX:
 if ((sobj = sobj_stru.AddInfoObj) == NULL) {
 printf("BB_GET_OBJ_BY_PATH(),can't find obj(BB_SEARCH_ADDINFO_STX)!");
 return false;
 }
 break;
 }
 if (sp_size == 1) {//如果直接取得整个表格
 (*obj) = sobj;
 (*obj_size) = BB_GET_OBJ_SIZE(sobj);
 return true;
 }
 sobj_stru = BB_GET_OBJ_STRU(sobj);
 }
 
 //针对目标元做查询
 if ((*sp)&BB_SEARCH_BY_NUM) {
 //如果是Num查询
 //string tmp; tmp.append((const char*)sp + 1, sp_size - 1);
 //BB_SIZE_T num1 = (BB_SIZE_T)STRING_Helper::scanUINTStr(tmp);
 BB_SIZE_T num1 = _scanUINTWidth((BYTE *)sp, sp_size - 1);
 BYTE sobj_type = BB_GET_OBJ_TYPE(sobj);
 if (sobj_type == BB_TABLE) {
 printf("BB_GET_OBJ_BY_PATH(),can't find obj(BB_SEARCH_BY_NUM)!");
 return false;
 }
 if (!BB_GET_ARRAY_REC_BY_NUM(sobj, num1, obj, obj_size)) {
 printf("BB_GET_OBJ_BY_PATH(),can't num rec by search_str!");
 return false;
 }
 }
 else {
 //如果是Index
 BYTE sobj_type = BB_GET_OBJ_TYPE(sobj);
 if (sobj_type == BB_TABLE) {
 //如果是Table，终点重定向
 //取得index
 BB_OBJ_STRU sobj_stru = BB_GET_OBJ_STRU(sobj);
 BYTE index_width = BB_GET_ARRAY_INDEX_OBJ_WIDTH(sobj_stru.IndexObj);
 if ((num = BB_GET_ARRAY_NUM_BY_INDEX(sobj, (BYTE *)sp + 1, index_width))<0) {
 printf("BB_GET_OBJ_BY_PATH(TABLE)-> indexObj search error!");
 return false;
 }
 //取得元素
 BYTE col = BB_ScanUINTWidth((BYTE *)sp + (1 + index_width), sp_size - (1 + index_width));
 if (!BB_GET_TABLE_ITEMS_DATA(sobj, num, col, obj, obj_size)) {
 printf("BB_GET_OBJ_BY_PATH(TABLE)->ItemData(Num) search error!");
 return false;
 }
 return true;
 }
 else if (sobj_type == BB_ARRAY) {
 //如果是数组
 if ((num = BB_GET_ARRAY_NUM_BY_INDEX(sobj, (BYTE *)sp + 1, sp_size - 1))<0) {
 printf("BB_GET_OBJ_BY_PATH(ARRAY)-> indexObj search error!");
 return false;
 }
 if (!BB_GET_ARRAY_REC_BY_NUM(sobj, num, obj, obj_size)) {
 printf("BB_GET_OBJ_BY_PATH(ARRAY)-> itemData(Num) search error!");
 return false;
 }
 }
 else {
 printf("BB_GET_OBJ_BY_PATH(),search_str-obj only be ARRAY|TABLE");
 return false;
 }
 }
 
 sobj = (*obj);
	};
	return true;
 }*/

bool BB_GET_OBJ_BY_PATH(BB_OBJ *sobj, BB_OBJ *search_str_data, int size_search_str, BYTE **obj, BB_SIZE_T *obj_size) {//ly 910
    int num;
    
    //循环读取search_str内容，递进查询定位
    int sp_size = 0;
    (*obj) = NULL;
    
    BB_OBJ *sp0;
    sp0 = (BB_OBJ *)search_str_data;
    
    if ((*sp0) == BB_SEARCH_NULL) {
        (*obj) = sobj;
        (*obj_size) = BB_GET_OBJ_SIZE(sobj);
    }
    
    BB_OBJ *sp;
    for (sp = sp0; (sp - sp0) <= size_search_str && (*sp) != BB_SEARCH_NULL; sp += sp_size) {
        sp_size = BB_SEARCH_CELL_SIZE(sp);
        if (((sp - sp0) + sp_size) > size_search_str)
            break; //超出对象处理
        
        //如果是Content则按原方式查询，如果是分支则sobj为分支
        BB_OBJ_STRU sobj_stru= BB_GET_OBJ_STRU(sobj);
        if ((*sp) >= 40) {
            //如果是分支
            switch ((*sp) & 0xC0) {//11000000只流出a8,a7
                case BB_SEARCH_CELL_STX:
                    if ((sobj = sobj_stru.CellObj) == NULL) {
                        printf("BB_GET_OBJ_BY_PATH(),can't find obj(BB_SEARCH_CELL_STX)!");
                        return false;
                    }
                    break;
                case BB_SEARCH_INDEX_STX:
                    if ((sobj = sobj_stru.IndexObj) == NULL) {
                        printf("BB_GET_OBJ_BY_PATH(),can't find obj(BB_SEARCH_INDEX_STX)!");
                        return false;
                    }
                    break;
                case BB_SEARCH_ADDINFO_STX:
                    if ((sobj = sobj_stru.AddInfoObj) == NULL) {
                        printf("BB_GET_OBJ_BY_PATH(),can't find obj(BB_SEARCH_ADDINFO_STX)!");
                        return false;
                    }
                    break;
            }
            //if (sp_size == 1) {//如果直接取得整个表格
            if (sp_size==1){
                if((sp - sp0)==(size_search_str-1)){
                    (*obj) = sobj;
                    (*obj_size) = BB_GET_OBJ_SIZE(sobj);
                    return true;
                }else
                    continue;//ly table_dir;
            }
            sobj_stru = BB_GET_OBJ_STRU(sobj);
        }
        
        //针对目标元做查询
        if ( (sobj_stru.IndexObj==NULL)||((*sp)&BB_SEARCH_BY_NUM) ) {
            //如果是Num查询
            //string tmp; tmp.append((const char*)sp + 1, sp_size - 1);
            //BB_SIZE_T num1 = (BB_SIZE_T)STRING_Helper::scanUINTStr(tmp);
            BB_SIZE_T num1 = _scanUINTWidth((BYTE *)sp+1, sp_size - 1);
            //BYTE sobj_type = BB_GET_OBJ_TYPE(sobj);
            if (sobj_stru.Type == BB_TABLE) {
                printf("BB_GET_OBJ_BY_PATH(),can't find obj(BB_SEARCH_BY_NUM)!");
                return false;
            }
            if (!BB_GET_ARRAY_REC_BY_NUM_STRU(&sobj_stru, num1, obj, obj_size)) {
                //printf("BB_GET_OBJ_BY_PATH(),can't num rec by search_str!");
                return false;
            }
        }
        else {
            //如果是Index
            //BYTE sobj_type = BB_GET_OBJ_TYPE(sobj);
            if (sobj_stru.Type == BB_TABLE) {
                //如果是Table，终点重定向
                //取得index
                //BB_OBJ_STRU sobj_stru = BB_GET_OBJ_STRU(sobj);例如:/0x000119:0x0001记录号，19字段号
                BYTE index_width = BB_GET_ARRAY_INDEX_OBJ_WIDTH(sobj_stru.IndexObj);
                if ((num = BB_GET_ARRAY_NUM_BY_INDEX_STRU(&sobj_stru, (BYTE *)sp + 1, index_width))<0) {
                    printf("BB_GET_OBJ_BY_PATH(TABLE)-> indexObj search error!");
                    return false;
                }
                //取得元素
                BYTE col = BB_ScanUINTWidth((BYTE *)sp + (1 + index_width), sp_size - (1 + index_width));
                if (!BB_GET_TABLE_ITEMS_DATA_STRU(&sobj_stru, num, col, obj, obj_size)) {
                    printf("BB_GET_OBJ_BY_PATH(TABLE)->ItemData(Num) search error!");
                    return false;
                }
                return true;
            }
            else if (sobj_stru.Type == BB_ARRAY) {
                //如果是数组
                if ((num = BB_GET_ARRAY_NUM_BY_INDEX_STRU(&sobj_stru, (BYTE *)sp + 1, sp_size - 1))<0) {
                    printf("BB_GET_OBJ_BY_PATH(ARRAY)-> indexObj search error!");
                    return false;
                }
                //printf("index_width0.4%d\n",num);
                if (!BB_GET_ARRAY_REC_BY_NUM_STRU(&sobj_stru, num, obj, obj_size)) {//ly malloc test
                    printf("BB_GET_OBJ_BY_PATH(ARRAY)-> itemData(Num) search error!");
                    return false;
                }
                //printf("index_width0.5\n");
            }
            else {
                printf("BB_GET_OBJ_BY_PATH(),search_str-obj only be ARRAY|TABLE");
                return false;
            }
        }
        
        sobj = (*obj);
    };
    return true;
}


bool BB_GET_OBJ_BY_PATH_ALL(BB_OBJ *sobj, BB_OBJ *search_str_data, int size_search_str, BYTE **obj, BB_SIZE_T *obj_size) {//ly 910
    int num;
    
    //循环读取search_str内容，递进查询定位
    int sp_size = 0;
    (*obj) = NULL;
    
    BB_OBJ *sp0;
    sp0 = (BB_OBJ *)search_str_data;
    
    if ((*sp0) == BB_SEARCH_NULL) {
        (*obj) = sobj;
        (*obj_size) = BB_GET_OBJ_SIZE(sobj);
    }
    
    BB_OBJ *sp;
    for (sp = sp0; (sp - sp0) <= size_search_str && (*sp) != BB_SEARCH_NULL; sp += sp_size) {
        sp_size = BB_SEARCH_CELL_SIZE(sp);
        if (((sp - sp0) + sp_size) > size_search_str)
            break; //超出对象处理
        
        //如果是Content则按原方式查询，如果是分支则sobj为分支
        BB_OBJ_STRU sobj_stru= BB_GET_OBJ_STRU(sobj);
        if ((*sp) >= 40) {
            //如果是分支
            switch ((*sp) & 0xC0) {//11000000只流出a8,a7
                case BB_SEARCH_CELL_STX:
                    if ((sobj = sobj_stru.CellObj) == NULL) {
                        printf("BB_GET_OBJ_BY_PATH(),can't find obj(BB_SEARCH_CELL_STX)!");
                        return false;
                    }
                    break;
                case BB_SEARCH_INDEX_STX:
                    if ((sobj = sobj_stru.IndexObj) == NULL) {
                        printf("BB_GET_OBJ_BY_PATH(),can't find obj(BB_SEARCH_INDEX_STX)!");
                        return false;
                    }
                    break;
                case BB_SEARCH_ADDINFO_STX:
                    if ((sobj = sobj_stru.AddInfoObj) == NULL) {
                        printf("BB_GET_OBJ_BY_PATH(),can't find obj(BB_SEARCH_ADDINFO_STX)!");
                        return false;
                    }
                    break;
            }
            if (sp_size == 1) {//如果直接取得整个表格
                if (sp_size==1){
                    if((sp - sp0)==(size_search_str-1)){
                        (*obj) = sobj;
                        (*obj_size) = BB_GET_OBJ_SIZE(sobj);
                        return true;
                    }else
                        continue;//ly table_dir; //ly table_sync3 //ly rec_chg_mark
                }
//                (*obj) = sobj;
//                (*obj_size) = BB_GET_OBJ_SIZE(sobj);
//                return true;
            }
            sobj_stru = BB_GET_OBJ_STRU(sobj);
        }
        
        //针对目标元做查询
        if ( (sobj_stru.IndexObj==NULL)||((*sp)&BB_SEARCH_BY_NUM) ) {
            //如果是Num查询
            //string tmp; tmp.append((const char*)sp + 1, sp_size - 1);
            //BB_SIZE_T num1 = (BB_SIZE_T)STRING_Helper::scanUINTStr(tmp);
            BB_SIZE_T num1 = _scanUINTWidth((BYTE *)sp+1, sp_size - 1);
            //BYTE sobj_type = BB_GET_OBJ_TYPE(sobj);
            if (sobj_stru.Type == BB_TABLE) {
                printf("BB_GET_OBJ_BY_PATH(),can't find obj(BB_SEARCH_BY_NUM)!");
                return false;
            }
            if (!BB_GET_ARRAY_REC_BY_NUM_STRU(&sobj_stru, num1, obj, obj_size)) {
                printf("BB_GET_OBJ_BY_PATH(),can't num rec by search_str!");
                return false;
            }
        }
        else {
            //如果是Index
            //BYTE sobj_type = BB_GET_OBJ_TYPE(sobj);
            if (sobj_stru.Type == BB_TABLE) {
                //如果是Table，终点重定向
                //取得index
                //BB_OBJ_STRU sobj_stru = BB_GET_OBJ_STRU(sobj);
                BYTE index_width = BB_GET_ARRAY_INDEX_OBJ_WIDTH(sobj_stru.IndexObj);
                if ((num = BB_GET_ARRAY_NUM_BY_INDEX_STRU(&sobj_stru, (BYTE *)sp + 1, index_width))<0) {
                    printf("BB_GET_OBJ_BY_PATH(TABLE)-> indexObj search error!");
                    return false;
                }
                //取得元素
                sp=(BYTE *)sp + (1 + index_width);
                if((sp - sp0) > size_search_str){
                    //如果没有下级字段元素则错误。
                    printf("BB_GET_OBJ_BY_PATH(TABLE)->ItemData search error (no field sp part!!!) !");
                    return false;
                }
                sp_size = BB_SEARCH_CELL_SIZE(sp);
                BYTE col = BB_ScanUINTWidth(sp+1,sp_size-1);
                if (!BB_GET_TABLE_ITEMS_DATA_STRU(&sobj_stru, num, col, obj, obj_size)) {
                    printf("BB_GET_OBJ_BY_PATH(TABLE)->ItemData(Num) search error!");
                    return false;
                }
            }
            else if (sobj_stru.Type == BB_ARRAY) {
                //如果是数组
                if ((num = BB_GET_ARRAY_NUM_BY_INDEX_STRU(&sobj_stru, (BYTE *)sp + 1, sp_size - 1))<0) {
                    printf("BB_GET_OBJ_BY_PATH(ARRAY)-> indexObj search error!");
                    return false;
                }
                //printf("index_width0.4%d\n",num);
                if (!BB_GET_ARRAY_REC_BY_NUM_STRU(&sobj_stru, num, obj, obj_size)) {//ly malloc test
                    printf("BB_GET_OBJ_BY_PATH(ARRAY)-> itemData(Num) search error!");
                    return false;
                }
                //printf("index_width0.5\n");
            }
            else {
                printf("BB_GET_OBJ_BY_PATH(),search_str-obj only be ARRAY|TABLE");
                return false;
            }
        }
        
        sobj = (*obj);
    };
    return true;
}

bool BB_GET_ARRAY_REC_BY_NUM(BB_OBJ *obj,BB_SIZE_T num, BYTE **item, BB_SIZE_T *item_size){//locate itme inside obj
    BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
    return BB_GET_ARRAY_REC_BY_NUM_STRU(&obj_stru,num,item,item_size);
};
bool BB_GET_ARRAY_REC_BY_INDEX_STRU(BB_OBJ_STRU *obj_stru, BYTE *index_str, BB_SIZE_T index_str_size, BYTE **item, BB_SIZE_T *item_size) {
    return BB_GET_ARRAY_REC_BY_INDEX(obj_stru->ThisObj, index_str, index_str_size, item, item_size);
}
bool BB_GET_ARRAY_REC_BY_INDEX(BB_OBJ *obj,BYTE *index_str, BB_SIZE_T index_str_size,BYTE **item, BB_SIZE_T *item_size){
    int rec;
    if ((rec = BB_GET_ARRAY_NUM_BY_INDEX(obj,index_str,index_str_size))<0){//通过index查找是否存在ids索引；
        return false;
    }else if (!BB_GET_ARRAY_REC_BY_NUM(obj,rec,item,item_size)){
        return false;
    };
    return true;
}
/*bool BB_GET_ARRAY_KEY_VALUE(BB_OBJ *obj,BYTE *index_str, BB_SIZE_T index_str_size,BYTE **item, BB_SIZE_T *item_size){
	int rec;
	BYTE **item_obj;
	BB_SIZE_T item_obj_size;//返回为字串对象，需要去除头。
	//测试index和内容都必须是BYTES
	BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
	if (!(obj_stru.Type==BB_BYTES ||  obj_stru.Type==BB_BYTES_EXT)){
 printf("%s\n","BB_GET_ARRAY_KEY_VALUE()'s input type must be BYTES or BYTES_EXT !");
 return false;
	}
	BYTE index_type=BB_GET_OBJ_TYPE(obj_stru.IndexObj);
	if (!(obj_stru.Type==BB_BYTES ||  obj_stru.Type==BB_BYTES_EXT)){
 printf("%s\n","BB_GET_ARRAY_KEY_VALUE()'s index type must be BYTES or BYTES_EXT !");
 return false;
	}
 
	if ((rec = BB_GET_ARRAY_NUM_BY_INDEX(obj,index_str,index_str_size))<0){//通过index查找是否存在ids索引；
 return false;
	}else if (!BB_GET_ARRAY_REC_BY_NUM(obj,rec,item_obj,&item_obj_size)){
 return false;
	};
 
	//BYTES对象转换
	(*item_obj)=BB_GET_OBJ_DATA(*item_obj);
	(*item_size)=BB_GET_CELL_DATA_LEN(item_obj);
	return true;
 }*/
bool BB_GET_ARRAY_UINT_VALUE_BY_INDEX(BB_OBJ *obj,BYTE *index_str, BB_SIZE_T index_str_size,UInt64 *ret_value){
    int rec;
    if ((rec = BB_GET_ARRAY_NUM_BY_INDEX(obj,index_str,index_str_size))<0){//通过index查找是否存在ids索引；
        return false;
    }else if (!BB_GET_ARRAY_UINT_VALUE(obj,rec,ret_value)){
        return false;
    }
    return true;
}
int BB_GET_ARRAY_NUM_BY_INDEX(BB_OBJ *obj, BYTE *index_str, BB_SIZE_T index_str_size){
    //取得obj详细信息
    BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
    return BB_GET_ARRAY_NUM_BY_INDEX_STRU(&obj_stru,index_str,index_str_size);
}
int BB_GET_ARRAY_NUM_BY_INDEX_STRU(BB_OBJ_STRU *obj_stru, BYTE *index_str, BB_SIZE_T index_str_size){//ly obj
    //根据index 值查询相关记录，并返回item指针和item_len和记录号。
    BB_SIZE_T test_item_size;
    UInt64 test_item_num,test_index;
    int num=0;
    BYTE *test_item;
    string test_str;
    
    if ((*obj_stru).LoadError || !((*obj_stru).Type==BB_ARRAY || (*obj_stru).Type==BB_TABLE) || (*obj_stru).IndexObj==NULL){//ly obj
        //printf("%s\n","BB_GET_ARRAY_NUM_BY_INDEX()'s input must be ARRAY/TABLE and must have IndexObj !");
        return -5;
    }
    BB_OBJ_STRU index_stru=BB_GET_OBJ_STRU((*obj_stru).IndexObj);
    BB_OBJ *bytes_obj_p; //form BB_BYTES_ARRAY:
    BB_OBJ_STRU index_cell_stru;
    //UInt64 index_cell_num;
    UInt64 index_table_idx_field;
    
    switch (index_stru.Type){
        case BB_ARRAY:
        case BB_TABLE:
            //在此处ly RedirTable 添加：index_stru.AddInfo=TABLE_INDEX_TYPE_FILTER,则直接取得Array值为num //ly table_redir; //ly table_filter
            switch (BB_GET_OBJ_TYPE(index_stru.CellObj)){
                case BB_UINT8:		case BB_UINT16:		case BB_UINT32:		case BB_UINT64:		case BB_MALLOC:
                    //判断是否超出max_len
                    test_index=_scanUINT(index_str,index_str_size);
                    test_item_size=BB_GET_OBJ_DATA_LEN(index_stru.CellObj); //取得元素宽度
                    if (test_item_size!=index_str_size){//ly index_size:
                        printf("Error:BB_GET_ARRAY_NUM_BY_INDEX()'s index_str_size != CellObj's width !\n");
                        //return -4;
                    }
                    for(test_item=index_stru.Data;(test_item+test_item_size)<=(index_stru.Data + index_stru.DataLen) ;test_item+=test_item_size,num++){
                        test_item_num=_scanUINT(test_item,test_item_size);
                        if (test_index==test_item_num){
                            return num;
                        }
                    }
                    return -1;
                case BB_BYTES:
                case BB_BYTES_EXT:
                case BB_NULL:	//ly max ly obj
                    bytes_obj_p=index_stru.Data; //取得元素宽度;
                    for(;bytes_obj_p<(index_stru.Data + index_stru.DataLen) ;bytes_obj_p+=test_item_size,num++){
                        test_item=(BYTE *)BB_GET_OBJ_DATA(bytes_obj_p);
                        test_item_size=BB_GET_OBJ_SIZE(bytes_obj_p);
                        if (check_string(index_str,index_str_size,test_item,BB_GET_OBJ_DATA_LEN(bytes_obj_p),false))
                            return num;
                    }
                    return -1;
                case BB_TABLE://ly table_dir 
                    index_cell_stru=BB_GET_OBJ_STRU(index_stru.CellObj);
                    if (BB_GET_ARRAY_ADD_INFO_UINT(index_stru.AddInfoObj,"INDEX_FIELD",index_table_idx_field)){
                        for(int num=0; num<index_stru.Num; num++){
                            //循环取得cell_table 字段值
                            BYTE *cmp_field;
                            BB_SIZE_T cmp_field_size;
                            BB_GET_TABLE_ITEMS_DATA_STRU(&index_stru, num, index_table_idx_field, &cmp_field, &cmp_field_size);
                            //对比
                            if (cmp_field_size==index_str_size){
                                if (memcmp((const void *)cmp_field,(const void *)index_str,index_str_size)==0)
                                    return num;
                                //找到
                                continue;
                            }else{
                                printf("BB_GET_ARRAY_NUM_BY_INDEX()'s obj.Index.CellObj cmp width error!\n");
                                return -1;
                            }
                        }
                        printf("BB_GET_ARRAY_NUM_BY_INDEX()'s index is not found!\n");
                        return -3;
                    }else
                        printf("BB_GET_ARRAY_NUM_BY_INDEX()'s obj.Index.CellObj Only Support UINT now!\n");
                    return -1;
                default:
                    //针对对象的查询
                    //return BB_GET_ARRAY_NUM_BY_INDEX((*obj_stru).IndexObj, index_str, index_str_size);
                    printf("BB_GET_ARRAY_NUM_BY_INDEX()'s obj.Index.CellObj Only Support UINT now!\n");
                    return -2;
            }
            break;
        case BB_UINT8:        case BB_UINT16:        case BB_UINT32:        case BB_UINT64:
            //ly malloc 2.1
            test_index=_scanUINT(index_str,index_str_size);
            return (int)test_index;
            break;
        default:
            printf("BB_GET_ARRAY_NUM_BY_INDEX()'s Index must be ARRAY or BB_TABLE!\n");
            return -3;
    }
}


int BB_APPEND_ARRAY_REC_STRU(BB_OBJ_STRU *obj_stru, BYTE *buf, BB_SIZE_T buf_size){//ly obj
    //在表格和数组中插入空记录
    if ((*obj_stru).Type!=BB_ARRAY && (*obj_stru).Type!=BB_TABLE){
        printf("BB_APPEND_ARRAY_REC() must input TABLE or ARRAY;\n");
        return -1;
    }
    
    //取得新内容起点
    BYTE *item = (*obj_stru).Data+(*obj_stru).DataLen;
    
    //如果内容为空则清空
    BB_OBJ_STRU cell_stru=BB_GET_OBJ_STRU((*obj_stru).CellObj);
    if (cell_stru.Type==BB_BYTES || cell_stru.Type==BB_BYTES_EXT){//ly obj_new
        //创建对象(字串改为长度对象)
        BB_OBJ *buf_obj = BB_NEW_CELL(cell_stru.Type, (const void *)buf, buf_size, NULL);
        BB_SIZE_T buf_obj_size = BB_GET_OBJ_SIZE(buf_obj);
        //对象数组添加
        if (((*obj_stru).DataLen+buf_obj_size)>(*obj_stru).MaxLen){
            free(buf_obj);
            printf("BB_APPEND_ARRAY_REC() input data is too large;MaxLen is %zu; DataLen is %zu, AppendObj Size is %zu\n",(*obj_stru).MaxLen,(*obj_stru).DataLen,buf_obj_size);
            return -1;  //结果超出范围。
        }
        if (buf_obj!=NULL)
            memcpy (item,buf_obj,buf_obj_size);
        free(buf_obj);
        (*obj_stru).DataLen+=buf_obj_size; //应该有此赋值动作!!!刷新struc缓存
    }
    else if (cell_stru.Type == BB_NULL) {//ly obj
        //对象数组添加
        if (((*obj_stru).DataLen + buf_size)>(*obj_stru).MaxLen) {
            printf("BB_APPEND_ARRAY_REC() input data is too large;MaxLen is %zu; DataLen is %zu, AppendObj Size is %zu\n", (*obj_stru).MaxLen, (*obj_stru).DataLen, buf_size);
            return -1;  //结果超出范围。
        }
        if (buf != NULL)
            memcpy(item, buf, buf_size);
        (*obj_stru).DataLen += buf_size; //应该有此赋值动作!!!刷新struc缓存
    }
    else{
        //定长数组添加
        if (((*obj_stru).DataLen+(*obj_stru).CellWidth)>(*obj_stru).MaxLen){
            printf("BB_APPEND_ARRAY_REC() input data is too large;MaxLen is %zu; DataLen is %zu, CellWidth is %zu\n",(*obj_stru).MaxLen,(*obj_stru).DataLen,(*obj_stru).CellWidth);
            return -1;  //结果超出范围。
        }
        if (buf_size>=(*obj_stru).CellWidth){
            if (buf!=NULL)
                memcpy (item,buf,(*obj_stru).CellWidth);
            buf_size=(*obj_stru).CellWidth;
        }else{
            if (buf!=NULL)
                memcpy (item,buf,buf_size);
            memset(item+buf_size,BB_ARRAY_APPEND_DEFAULT_CHAR,((*obj_stru).CellWidth-buf_size));
        }
        (*obj_stru).DataLen+=(*obj_stru).CellWidth; //应该有此赋值动作!!!刷新struc缓存
    }
    (*obj_stru).Size=(*obj_stru).HeaderLen+(*obj_stru).DataLen;
    (*obj_stru).Num++;
    
    //创建新的LenObj 和 IndexObj
    BB_SET_UINT_VALUE((*obj_stru).LenObj,(*obj_stru).DataLen);
    //如果是Index端节点则设置Index长度，作用于GET_ARRAY_INDEX_NUM();
    if ((*obj_stru).IndexObj!=NULL && BB_IS_UINT((*obj_stru).IndexObj))
        BB_SET_UINT_VALUE((*obj_stru).IndexObj,(*obj_stru).Num);//这个市除了Len/CellWidth外另一个记录长度的方法;??好像没有产生作用
    
    return (*obj_stru).Num-1;
}
int BB_APPEND_ARRAY_REC(BB_OBJ *obj, BYTE *buf, BB_SIZE_T buf_size){
    //在表格和数组中插入空记录
    BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
    
    return BB_APPEND_ARRAY_REC_STRU(&obj_stru,buf,buf_size);
}
int BB_APPEND_ARRAY_REC_BY_INDEX_STRU(BB_OBJ_STRU *obj_stru, BYTE *buf, BB_SIZE_T buf_size, BYTE *index_str,BB_SIZE_T index_str_len){
    //在表格和数组中插入空记录
    int num;
    //入口判断
    if ((*obj_stru).Type!=BB_ARRAY && (*obj_stru).Type!=BB_TABLE){
        printf("Error:BB_APPEND_ARRAY_REC_BY_INDEX() input must be ARRAY  or TABLE !\n");
        return -1;
    }
    
    //添加记录信息
    if ((num=BB_APPEND_ARRAY_REC_STRU(obj_stru,buf,buf_size))<0){
        printf("Error:BB_APPEND_ARRAY_REC_BY_INDEX()->BB_APPEND_ARRAY_REC(data)error !\n");
        return -1;
    }
    
    //添加索引信息，如果有索引
    if (index_str_len!=0 ){
        //判断索引索引字串大小，切割ly index_size:
        //BB_OBJ_STRU obj_index_stru=BB_GET_OBJ_STRU((*obj_stru).IndexObj); //ly mac

    	//ly RedirTable 在此处添加Index.AddInfo=FILTER,则添加num，并添加Index；解决他人的跳跃问题。
        
        //添加索引记录
        if (BB_APPEND_ARRAY_REC((*obj_stru).IndexObj,index_str,index_str_len)<0){
            printf("Error:BB_APPEND_ARRAY_REC_BY_INDEX()->BB_APPEND_ARRAY_REC(Index)error !\n");
            return -1;
        }
    }
    
    return num;
}
int BB_APPEND_ARRAY_REC_BY_INDEX(BB_OBJ *obj, BYTE *buf, BB_SIZE_T buf_size, BYTE *index_str,BB_SIZE_T index_str_len){
    BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
    return BB_APPEND_ARRAY_REC_BY_INDEX_STRU(&obj_stru,buf,buf_size,index_str,index_str_len);
}

int BB_REPLACE_ARRAY_REC_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T num,BYTE *buf, BB_SIZE_T buf_size){//现在还不支持！
    //在表格和数组中插入空记录
    printf("BB_REPLACE_ARRAY_REC_STRU() not avalible\n");
    return -1;
    
    if ((*obj_stru).Type!=BB_ARRAY && (*obj_stru).Type!=BB_TABLE){
        printf("BB_APPEND_ARRAY_REC() must input TABLE or ARRAY;\n");
        return -1;
    }
    
    //取得旧记录
    BB_OBJ *old_item;
    BB_SIZE_T old_size;
    if (!BB_GET_ARRAY_BY_NUM_NOREDIR_STRU(obj_stru,num,&old_item,&old_size)){
        printf("BB_RELACE_ARRAY_REC() num %zu not exist\n",num);
        return false;
    }
    
    //准备CELL
    BB_OBJ *item_obj;
    BB_SIZE_T item_obj_size;
    //取得不同宽度
    BB_OBJ_STRU cell_stru=BB_GET_OBJ_STRU((*obj_stru).CellObj);
    if (cell_stru.Type==BB_BYTES || cell_stru.Type==BB_BYTES_EXT){//ly obj_new
        //创建对象(字串改为长度对象)
        item_obj = BB_NEW_CELL(cell_stru.Type, (const void *)buf, buf_size, NULL);
        if (item_obj==NULL){
            printf("BB_REPLACE_ARRAY_REC()->bb_new_cell error");
            return -1;
        }
        item_obj_size = BB_GET_OBJ_SIZE(item_obj);
    }
    else if (cell_stru.Type == BB_NULL) {//ly obj
        //对象数组添加
        item_obj=buf;
        item_obj_size=buf_size;
    }
    else{
        //定长数组添加
        printf("BB_APPEND_ARRAY_REC() only support unstatic length array");
        return -1;  //结果超出范围。
    }
    
    //在此处添加大小，并判断是否超出。
}

BB_OBJ *BB_NEW_REPLACE_ARRAY_REC_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T num,BYTE *buf, BB_SIZE_T buf_size, BB_OBJ *p_buf){//ly table_filter:今后可在此提速。
    //在表格和数组中插入空记录
    if ((*obj_stru).Type!=BB_ARRAY && (*obj_stru).Type!=BB_TABLE){
        printf("BB_NEW_REPLACE_ARRAY_REC_STRU() must input TABLE or ARRAY;\n");
        return NULL;
    }
    
    //准备CELL
    BB_OBJ *item_obj=NULL;
    BB_SIZE_T item_obj_size;
    //取得不同宽度
    BB_OBJ_STRU cell_stru=BB_GET_OBJ_STRU((*obj_stru).CellObj);
    if (cell_stru.Type==BB_BYTES || cell_stru.Type==BB_BYTES_EXT){//ly obj_new
        //创建对象(字串改为长度对象)
        item_obj = BB_NEW_CELL(cell_stru.Type, (const void *)buf, buf_size, NULL);
        item_obj_size=BB_GET_OBJ_SIZE(item_obj);
    }
    else if (cell_stru.Type == BB_NULL) {//ly obj
        //对象数组添加
        item_obj= buf;
        item_obj_size=buf_size;
    }
    else{
        //定长数组添加
        printf("BB_NEW_REPLACE_ARRAY_REC_STRU() only support unstatic length array");
        return NULL;  //结果超出范围。
    }
    if (item_obj==NULL){
        printf("BB_NEW_REPLACE_ARRAY_REC() new cell error;\n");
        return NULL;
    }
    
    //创建新数组
    /*BYTE tmp;
    BB_OBJ *new_arr = BB_NEW_ARRAY((*obj_stru).CellObj, (*obj_stru).AddInfoObj, NULL, (item_obj_size+obj_stru->MaxLen)/BB_0DIM_BYTES_MAX_LEN+1, &tmp, 0, p_buf);
    //其中/BB_0DIM_BYTES_MAX_LEN和内部max_len处理;而且必须在：cell_stru.Type ==BB_NULL|BB_BYTES|BB_BYTES_EXT下使用。
    BB_OBJ_STRU new_stru=BB_GET_OBJ_STRU(new_arr);
    //循环再插入
    for(BB_SIZE_T i=0; i<obj_stru->Num; i++){
        if (i==num){//插入点
            BB_APPEND_ARRAY_REC_STRU(&new_stru, item_obj, item_obj_size);
        }else{//复制旧
            BB_OBJ *old_item; BB_SIZE_T old_item_size;
            if (!BB_GET_ARRAY_OBJ_BY_NUM_STRU(obj_stru, i, &old_item, &old_item_size)){
                printf("BB_NEW_REPLACE_ARRAY_REC()->rec num %d not exist;\n",(int)num);
                return NULL;
            }
            BB_APPEND_ARRAY_REC_STRU(&new_stru, old_item, old_item_size);
        }
    }*/
    
    //创建缓存
    BB_OBJ *new_arr=(BB_OBJ *)malloc((*obj_stru).Size+item_obj_size);//
    memcpy(new_arr,(*obj_stru).ThisObj,(*obj_stru).Size);
    BB_OBJ_STRU new_stru=BB_GET_OBJ_STRU(new_arr);
    
    //读取被替换记录
    BB_OBJ *old_item_p;
    BB_SIZE_T old_item_size;
    if (!BB_GET_ARRAY_OBJ_BY_NUM_STRU(obj_stru, num, &old_item_p, &old_item_size)){
        printf("BB_NEW_REPLACE_ARRAY_REC()->rec num %d not exist;\n",(int)num);
        return NULL;
    }
    //拷贝后部分
    BB_SIZE_T len=old_item_p-obj_stru->Data;
    void *src=new_stru.Data+len+old_item_size;
    void *dest=new_stru.Data+len+item_obj_size;
    BB_SIZE_T data_len=obj_stru->DataLen-len-old_item_size;
    memcpy(dest,src,data_len);
    //拷贝新数据
    dest=new_stru.Data+len;
    memcpy(dest,item_obj,item_obj_size);
    new_stru.DataLen+=item_obj_size-old_item_size;
    BB_SET_UINT_VALUE(new_stru.LenObj,new_stru.DataLen);
    
    //释放新item
    if (cell_stru.Type==BB_BYTES || cell_stru.Type==BB_BYTES_EXT)
        free(item_obj);
    BB_OBJ_STRU test=BB_GET_OBJ_STRU(new_arr);
    return new_arr;
}

bool BB_DELETE_ARRAY_REC_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T del_num){//ly obj
    //在表格和数组中删除记录
    if ((*obj_stru).Type!=BB_ARRAY && (*obj_stru).Type!=BB_TABLE){
        printf("%s\n","BB_APPEND_ARRAY_REC() must input TABLE or ARRAY;");
        return false;
    }
    //取得对应的数据区
    BYTE *item;
    BB_SIZE_T item_size;
    if (!BB_GET_ARRAY_BY_NUM_NOREDIR_STRU(obj_stru,del_num,&item,&item_size)){
        printf("BB_DELETE_ARRAY_REC() num %zu not exist\n",del_num);
        return false;
    }
    
    //清空MALLOC空间
    if (BB_GET_OBJ_TYPE(obj_stru->CellObj)==BB_MALLOC){//ly table_dir
        //BYTE *table_malloc=(*obj_stru).Data+pos;
        //BB_SIZE_T tmp1=*((BB_SIZE_T*)table_malloc);
        //(*item) = (BYTE *)(tmp1);
        if (item!=0 && item_size==sizeof(BB_SIZE_T)){
            BB_SIZE_T tmp1=*((BB_SIZE_T*)item);
            free((void *)tmp1);  //ly malloc new; 找寻malloc指针地址方法。
        }
        
    };
    
    //内存移动处理
    BB_SIZE_T trail_len=((*obj_stru).Data+(*obj_stru).DataLen)-(item+item_size);
    if (trail_len>0)
        memmove((void *)item,item+item_size,trail_len);
    
    //重新计算长度
    (*obj_stru).DataLen-=item_size; //应该有此赋值动作!!!刷新struc缓存
    (*obj_stru).Size=(*obj_stru).HeaderLen+(*obj_stru).DataLen;
    (*obj_stru).Num--;
    
    //创建新的LenObj 和 IndexObj
    BB_SET_UINT_VALUE((*obj_stru).LenObj,(*obj_stru).DataLen);
    //如果是Index端节点则设置Index长度，作用于GET_ARRAY_INDEX_NUM();
    if (BB_IS_UINT((*obj_stru).IndexObj))
        BB_SET_UINT_VALUE((*obj_stru).IndexObj,(*obj_stru).Num);
    
    return true;
}
bool BB_DELETE_ARRAY_REC(BB_OBJ *obj, BB_SIZE_T del_num){
    //在表格和数组中删除记录
    BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
    
    return BB_DELETE_ARRAY_REC_STRU(&obj_stru, del_num);
}
int BB_DELETE_ARRAY_REC_BY_INDEX_STRU(BB_OBJ_STRU *obj_stru,BYTE *index_str,BB_SIZE_T index_str_len){
    //在表格和数组中插入空记录
    //入口判断
    if ((*obj_stru).Type!=BB_ARRAY && (*obj_stru).Type!=BB_TABLE){
        printf("Error:BB_DELETE_ARRAY_REC_BY_INDEX() input must be ARRAY  or TABLE !\n");
        return -1;
    }
    UInt64 del_rec=_scanUINT((BYTE *)index_str,(BYTE)index_str_len);
    int rec=(int)del_rec;
    //判断索引索引字串大小，切割ly index_size:
    if ((*obj_stru).IndexObj!=NULL){
        BB_OBJ_STRU obj_index_stru=BB_GET_OBJ_STRU((*obj_stru).IndexObj);
        //查找记录
        if ((rec=BB_GET_ARRAY_NUM_BY_INDEX((*obj_stru).ThisObj,index_str,index_str_len))<0){//？？cell_width??参考
            //printf("Error:BB_DELETE_ARRAY_REC() num %d not exist\n",rec);
            return -2;
        }
        if (obj_index_stru.Type==BB_TABLE || obj_index_stru.Type==BB_ARRAY){
            if (!BB_DELETE_ARRAY_REC_STRU(&obj_index_stru,rec)){
                printf("Error:BB_DELETE_ARRAY_REC_BY_INDEX()->BB_DELETE_ARRAY_REC(data)error !\n");
                return -3;
            }
        }
    }
    
    //添加记录信息
    if (!BB_DELETE_ARRAY_REC_STRU(obj_stru,rec)){
        printf("%s\n","Error:BB_DELETE_ARRAY_REC_BY_INDEX()->BB_DELETE_ARRAY_REC(data)error may be out off range num!");
        return -4;
    }
    
    return rec;
}
int BB_DELETE_ARRAY_REC_BY_INDEX(BB_OBJ *obj,BYTE *index_str,BB_SIZE_T index_str_len){
    BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
    return BB_DELETE_ARRAY_REC_BY_INDEX_STRU(&obj_stru,index_str,index_str_len);
}
/************************表格对象设定操作**************************/
bool BB_GET_TABLE_ITEM_VALUE(BB_OBJ *obj, UInt32 row,UInt32 col,UInt64 *rt_value){//取得表格元素的值，如果row为FFFFFFFF
    //取得表格行
    BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
    BYTE* item_offset;
    BB_SIZE_T item_size=0;
    if (BB_GET_TABLE_ITEMS_DATA_STRU(&obj_stru,row,col,&item_offset,&item_size)){
        (*rt_value)=_scanUINTWidth(item_offset,item_size);
        return true;
    }else
        return false;
}

//bool BB_GET_TABLE_INDEX_ITEM_VALUE(BB_OBJ *obj, BYTE *index_str, BB_SIZE_T index_str_size,BYTE **rt_item_offset,BB_SIZE_T *rt_item_size){
//    BB_OBJ_STRU index_stru=BB_GET_OBJ_STRU(obj);
//    //如果有索引
//    if (index_stru.Type==BB_TABLE){//ly table_dir
//        UInt64 id_field=0;
//        if (this->GetAddInfoUINT(index_stru.AddInfoObj,"INDEX_FIELD",id_field))
//            BB_GET_TABLE_ITEMS_DATA_STRU(&index_stru, num, id_field, &index_str_32, &index_size_32);
//        else
//            BB_GET_TABLE_ITEMS_DATA_STRU(&index_stru, num, 0, &index_str_32, &index_size_32);
//    }else{
//        if (!BB_GET_ARRAY_REC_BY_NUM(table_obj_stru.IndexObj, num, &index_str_32, &index_size_32)) {
//            this->SHOW_ERROR((string)"BuildBufferFromDB()->add_nodeid->BB_GET_ARRAY_REC_BY_NUM(),IndexObj error! num=" + STRING_Helper::iToStr(num));
//            return ACTION_ERROR_READ_BUILDER_ADD_NODEID_INDEX_ERROR;
//        }//ly index_size:
//    }
//}

bool BB_GET_TABLE_ITEM_VALUE_STRU(BB_OBJ_STRU *obj_stru, UInt32 row,UInt32 col,UInt64 *rt_value){//取得表格元素的值，如果row为FFFFFFFF
    //取得表格行
    BYTE* item_offset;
    BB_SIZE_T item_size=0;
    if (BB_GET_TABLE_ITEMS_DATA_STRU(obj_stru,row,col,&item_offset,&item_size)){
        (*rt_value)=_scanUINTWidth(item_offset,item_size);
        return true;
    }else
        return false;
}

bool BB_GET_TABLE_ITEMS_DATA_TYPE_VALUE(BB_OBJ_STRU *obj_stru, BB_SIZE_T row,BB_SIZE_T col,BYTE **rt_item_offset,BB_SIZE_T *rt_item_size,BYTE *rt_field_type){//取得表格元素的值，如果row为FFFFFFFF则取得行列，否则取得综列
    BB_SIZE_T item_size;
    BB_SIZE_T field_size=0;
    BYTE *item,*item_offset;
    
    //取得表格行
    if (!BB_GET_ARRAY_REC_BY_NUM_STRU(obj_stru,row,&item,&item_size)){
        return false;
    }
    item_offset=item;
    
    //循环表格列
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU((*obj_stru).CellObj);
    BB_SIZE_T i;
    for (i = 0; i < col; i++)
    {
        //取得头字段大小
        if ((field_size=BB_GET_TABLE_FIELD_WIDTH_STRU(&cell_obj_stru,i))==BB_SIZE_T_ERROR){
            printf("%s %u\n","BB_GET_TABLE_ITEMS_DATA_STRU()->BB_GET_TABLE_FIELD_WIDTH()error ! cell field_num is ", i);
            return false;//to_add
        }
        
        item_offset+=field_size;
        //边界
        if ((item_offset-item)>((int)item_size)){
            printf("%s\n","BB_GET_TABLE_ITEMS_DATA_STRU() out of range !");
            return false;
        }
    }
    *rt_item_offset=item_offset;
    
    //判断是否为对象:有三种写入形式：（1）普通field返回内容长度 （2）对象字段，返回对象值; (3)字串返回字串长度
    //取得field_type
    UInt64 field_type=BB_TABLE_FIELD_TYPE_BINARY;
    if (cell_obj_stru.Type==BB_TABLE)
        BB_GET_TABLE_ITEM_VALUE_STRU(&cell_obj_stru,i,BB_TABLE_DEFAULT_TYPE_FIELD,&field_type); //只针对cell 表格, 否则缺省为BINARY
    //取得field_size
    if ((field_size=BB_GET_TABLE_FIELD_WIDTH_STRU(&cell_obj_stru,i))==BB_SIZE_T_ERROR){
        printf("%s %u\n","BB_GET_TABLE_ITEMS_DATA_STRU()->BB_GET_TABLE_FIELD_WIDTH()error ! cell field_num is ",i);
        return false;//to_add
    }
    if (field_type==BB_TABLE_FIELD_TYPE_STRING){//对于字符长度的处理
        /*int tmp_field_len=strlen((char *)item_offset);
         if (tmp_field_len<field_size)
         field_size=tmp_field_len;*/
        unsigned int j;
        for (j = 0; (j < field_size); j++) {
            if ((item_offset[j] == BB_ARRAY_APPEND_DEFAULT_CHAR)|| item_offset[j]==0)
                break;
        }
        field_size = j;
    }
    
    (*rt_item_size)=field_size;
    (*rt_field_type) = field_type;
    return true;
}
bool BB_GET_TABLE_ITEMS_DATA(BB_OBJ *obj, UInt32 row,UInt32 col,BYTE **rt_item_offset,BB_SIZE_T *rt_item_size){
    BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
    return BB_GET_TABLE_ITEMS_DATA_STRU(&obj_stru,row,col,rt_item_offset,rt_item_size);
}
bool BB_GET_TABLE_ITEMS_DATA_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T row,BB_SIZE_T col,BYTE **rt_item_offset,BB_SIZE_T *rt_item_size){//取得表格元素的值，如果row为FFFFFFFF则取得行列，否则取得综列
    BB_SIZE_T item_size;
    BB_SIZE_T field_size=0;
    BYTE *item,*item_offset;
    
    //取得表格行
    if (!BB_GET_ARRAY_REC_BY_NUM_STRU(obj_stru,row,&item,&item_size)){
        return false;
    }
    item_offset=item;
    
    //循环表格列
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU((*obj_stru).CellObj);
    if (col > cell_obj_stru.Num-1) {
        printf("%s %u\n", "BB_GET_TABLE_ITEMS_DATA_STRU()->paras->col is larger than cell_obj_stru.Num! col=", col);
        return false;//to_add
    }
    BB_SIZE_T i;
    for (i = 0; i < col; i++)
    {
        //取得头字段大小
        if ((field_size=BB_GET_TABLE_FIELD_WIDTH_STRU(&cell_obj_stru,i))==BB_SIZE_T_ERROR){
            printf("%s %u\n","BB_GET_TABLE_ITEMS_DATA_STRU()->BB_GET_TABLE_FIELD_WIDTH()error ! cell field_num is ", i);
            return false;//to_add
        }
        
        item_offset+=field_size;
        //边界
        if ((item_offset-item)>((int)item_size)){
            printf("%s\n","BB_GET_TABLE_ITEMS_DATA_STRU() out of range !");
            return false;
        }
    }
    *rt_item_offset=item_offset;
    //取得field_size
    if ((field_size=BB_GET_TABLE_FIELD_WIDTH_STRU(&cell_obj_stru,i))==BB_SIZE_T_ERROR){
        printf("%s %u\n","BB_GET_TABLE_ITEMS_DATA_STRU()->BB_GET_TABLE_FIELD_WIDTH()error ! cell field_num is ", i);
        return false;//to_add
    }
    (*rt_item_size)=field_size;
    return true;
}

bool BB_GET_TABLE_FIELD_OFFSET_SIZE_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T col, BB_SIZE_T *offset, BB_SIZE_T *field_size){//取得表格元素的值，如果row为FFFFFFFF则取得行列，否则取得综列 //ly task
    (*offset)=0;
    
    //循环表格列
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU((*obj_stru).CellObj);
    if (col > cell_obj_stru.Num-1) {
        printf("%s %u\n", "BB_GET_TABLE_ITEMS_DATA_STRU()->paras->col is larger than cell_obj_stru.Num! col=", col);
        return false;//to_add
    }
    UInt32 i;
    for (i = 0; i < col; i++)
    {
        //取得头字段大小
        if (((*field_size)=BB_GET_TABLE_FIELD_WIDTH_STRU(&cell_obj_stru,i))==BB_SIZE_T_ERROR){
            printf("%s %u\n","BB_GET_TABLE_ITEMS_DATA_STRU()->BB_GET_TABLE_FIELD_WIDTH()error ! cell field_num is ", i);
            return false;//to_add
        }
        (*offset)+=(*field_size);
        //边界
    }
    (*offset)=(*offset)-(*field_size);
    return true;
}

bool BB_GET_TABLE_FIELD_NAME_OFFSET_SIZE_STRU(BB_OBJ_STRU *obj_stru, const char *field_name, BB_SIZE_T *offset, BB_SIZE_T *field_size){//取得表格元素的值，如果row为FFFFFFFF则取得行列，否则取得综列 //ly task
    (*offset)=0;
    BYTE *item;
    BB_SIZE_T item_size;
    
    //循环表格列
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU((*obj_stru).CellObj);
    UInt32 i;
    BYTE field_name_len=strlen(field_name);
    for (i = 0; i < cell_obj_stru.Num; i++)
    {
        //取得头字段大小
        if (((*field_size)=BB_GET_TABLE_FIELD_WIDTH_STRU(&cell_obj_stru,i))==BB_SIZE_T_ERROR){
            printf("%s %u\n","BB_GET_TABLE_ITEMS_DATA_STRU()->BB_GET_TABLE_FIELD_WIDTH()error ! cell field_num is ", i);
            return false;//to_add
        }
        BB_GET_TABLE_FIELD_NAME_STRU(&cell_obj_stru,i,&item,&item_size);
        BB_SIZE_T len=field_name_len >=item_size ? item_size : field_name_len;
        if (strncmp(field_name,(const char *)item,len)==0)
            break;
        (*offset)+=(*field_size);
    }
    //如果未找到
    if (i==cell_obj_stru.Num)
        return false;
    //如果找到
    return true;
}
BB_SIZE_T BB_GET_TABLE_MAX_NUM_LEN(BB_OBJ_STRU *obj_stru){
    if (obj_stru->MaxLenObj==NULL)
        return 0;
    else{
        UInt64 data_len=BB_GET_UINT_VALUE(obj_stru->MaxLenObj);
        BB_SIZE_T cell_width=BB_GET_TABLE_CELL_WIDTH(obj_stru->CellObj);
        return data_len/cell_width;
    }
}

bool BB_PUT_TABLE_ITEMS_DATA_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T row,BB_SIZE_T col,BYTE *buf,BB_SIZE_T buf_size,BYTE method){//取得表格元素的值，如果row为FFFFFFFF则取得行列，否则取得综列
    BB_SIZE_T item_size;
    BB_SIZE_T field_size,item_offset=0;
    BYTE *item;
    UInt64 field_type=0;
    field_size = 0;
    
    //取得表格行
    if (!BB_GET_ARRAY_REC_BY_NUM_STRU(obj_stru,row,&item,&item_size)){
        printf("%s %d\n","BB_PUT_TABLE_ITEMS_DATA_STRU()->BB_GET_ARRAY_REC_BY_NUM_STRU()error ! row is ", row);
        return false;
    }
    
    //循环表格列
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU((*obj_stru).CellObj);
    UInt32 i;
    for (i = 0; i < col; i++)
    {
        //取得头字段大小
        if ((field_size=BB_GET_TABLE_FIELD_WIDTH_STRU(&cell_obj_stru,i))==BB_SIZE_T_ERROR){
            printf("%s %u\n","BB_PUT_TABLE_ITEMS_DATA_STRU()->BB_GET_TABLE_FIELD_WIDTH()error ! cell field_num is ", i);
            return false;//to_add
        }
        
        item_offset+=field_size;
        //边界
        if (item_offset>item_size){
            printf("%s\n","BB_PUT_TABLE_ITEMS_DATA_STRU() out of range !");
            return false;
        }
    }
    
    //找到后的处理
    //取得field_size
    if ((field_size=BB_GET_TABLE_FIELD_WIDTH_STRU(&cell_obj_stru,col))==BB_SIZE_T_ERROR){
        printf("%s %u\n","BB_PUT_TABLE_ITEMS_DATA_STRU()->BB_GET_TABLE_FIELD_WIDTH()error ! cell field_num is ", col);
        return false;//to_add
    }
    //写入操作
    BB_GET_TABLE_ITEM_VALUE_STRU(&cell_obj_stru,col,BB_TABLE_DEFAULT_TYPE_FIELD,&field_type);
    if (field_type == BB_TABLE_FIELD_TYPE_OBJECT) {
        //数值写入或全对象写入
        BB_SIZE_T obj_buf_size = BB_GET_OBJ_SIZE((BB_OBJ *)buf);
        memcpy((void *)(item + item_offset), (const void*)buf, obj_buf_size > field_size ? field_size : obj_buf_size); //ly mac
    }else if(field_type == BB_TABLE_FIELD_TYPE_MALLOC) {//ly malloc2;ly task
        //如果有旧的MALLOC则删除
        BB_OBJ *new_obj;
        BYTE type;
        BYTE *item_pos=item + item_offset;
        if ((*item_pos) != BB_ARRAY_APPEND_DEFAULT_CHAR && BB_GET_MALLOC_ADDRESS(item_pos, (void **)&new_obj, &type)){
            if ((method & BB_TABLE_PUT_OPERATE_INIT)== BB_TABLE_PUT_OPERATE_INIT){
                //printf(">>SET");//ly malloc2.1 需要详细测试，包括任务循环
            }else
                free(new_obj);
        }

        //拷贝对象
        void * new_buf=malloc(buf_size);
        memset(new_buf, 0, buf_size);
        memcpy(new_buf,buf,buf_size); //在这里不free，需要在记录替换的时候再释放。今后应该在malloc中添加长度计算标记
        //数值写入空间
        new_obj = BB_NEW_MALLOC((void*)&new_buf, NULL, 0, BB_MALLOC_TYPE_OBJ, NULL);
        if (new_obj == NULL) {
            printf("%s \n","BB_PUT_TABLE_ITEMS_DATA_STRU()->BB_TABLE_FIELD_TYPE_MALLOC: BB_NEW_MALLOC() error!");
            return false;//to_add
        }
        BB_SIZE_T obj_buf_size = BB_GET_OBJ_SIZE((BB_OBJ *)new_obj);
        if (obj_buf_size >= field_size) {
            printf("%s \n","BB_PUT_TABLE_ITEMS_DATA_STRU()->BB_TABLE_FIELD_TYPE_MALLOC: field_size is too shot can't put NEW_MALLOC() obj!");
            return false;//to_add
        }
        memcpy((void *)(item + item_offset), (const void*)new_obj, obj_buf_size); //ly mac
        if (new_obj!=NULL)
            free(new_obj);
    }else{
        if ((method & BB_TABLE_PUT_OPERATE_SET)== BB_TABLE_PUT_OPERATE_SET){
            //memset((void *)(item + item_offset), ARRAY_APPEND_DEFAULT_CHAR, field_size);//清空原字串。 ly: string;
            for(unsigned int i=0; (i<field_size) &&(i<buf_size); i++){
                if (i<buf_size)
                    (*(item+item_offset+i))=(*(buf+i));
            }
        }else if ((method & BB_TABLE_PUT_OPERATE_OR)== BB_TABLE_PUT_OPERATE_OR || (method & BB_TABLE_PUT_OPERATE_AND)== BB_TABLE_PUT_OPERATE_AND){//按位与操作
            for(unsigned int i=0; (i<field_size) &&(i<buf_size) ; i++){
                if ((method & BB_TABLE_PUT_OPERATE_OR)== BB_TABLE_PUT_OPERATE_OR)
                    (*(item+item_offset+i))=(*(item+item_offset+i))|(*(buf+i));
                else
                    (*(item+item_offset+i))=(*(item+item_offset+i))&(*(buf+i));
            }
        }else{
            memset((void *)(item + item_offset), BB_ARRAY_APPEND_DEFAULT_CHAR, field_size);//清空原字串。 ly: string;
            memcpy((void *)(item+item_offset),(const void*)buf, buf_size > field_size ? field_size : buf_size);  //ly mac
        }
    }
    /*if (!(cell_obj_stru.Type==BB_TABLE && field_type==BB_TABLE_FIELD_TYPE_OBJECT) || (method & BB_TABLE_REC_WR_OBJ_WHOLE)){
     //数值写入或全对象写入
     if (method & BB_TABLE_PUT_OPERATE_OR || method & BB_TABLE_PUT_OPERATE_AND ){//按位与操作
     int i;
     for(i=0; (i<field_size) &&(i<buf_size) ; i++){
     if (method & BB_TABLE_PUT_OPERATE_OR)
					(*(item+item_offset+i))=(*(item+item_offset+i))|(*(buf+i));
     else
					(*(item+item_offset+i))=(*(item+item_offset+i))&(*(buf+i));
     }
     }else if (method & BB_TABLE_PUT_OPERATE_SET){
     memset((void *)(item + item_offset), ARRAY_APPEND_DEFAULT_CHAR, field_size);//清空原字串。 ly: string;
     for(i=0; (i<field_size) &&(i<buf_size); i++){
     if (i<buf_size)
					(*(item+item_offset+i))=(*(buf+i));
     }
     }else{
     memset((void *)(item + item_offset), ARRAY_APPEND_DEFAULT_CHAR, field_size);//清空原字串。 ly: string;
     BYTE *dest=(BYTE *)memcpy((void *)(item+item_offset),(const void*)buf, buf_size > field_size ? field_size : buf_size);
     }
     //bufoffset += field_size;
     }else{
     //变长字段写入	??? 此处尚不能运行???确实没有测试过
     BB_OBJ *new_obj;
     set_table_obj_field(0,buf, buf_size, field_size, &new_obj);
     BB_SIZE_T obj_size=BB_GET_OBJ_SIZE(new_obj);
     BYTE *dest=(BYTE *)memcpy((void *)(item+item_offset),(const void*)new_obj, obj_size);
     free (new_obj);
     //??? 难道不需要：bufoffset += field_size;
     }*/
    
    return true;
}
bool BB_PUT_TABLE_ITEMS_DATA(BB_OBJ *obj, BB_SIZE_T row,BB_SIZE_T col,BYTE *buf,BB_SIZE_T buf_size,BYTE method){
    BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
    
    return BB_PUT_TABLE_ITEMS_DATA_STRU(&obj_stru,row,col,buf,buf_size,method);
}
//bool BB_WRITE_ARRAY_FIELD_BY_NUM(BB_OBJ *obj, BYTE* buf, BB_SIZE_T buf_size, BB_SIZE_T rec_num,UInt32 field_num){
//	BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
//	BB_SIZE_T obj_fields_num,item_size=0,cell_item_size=0,item_offset=0;
//	BYTE *item,*cell_item,*obj_fields,obj_fields_size;
//
//	//必须是数组
//	if (obj_stru.Type!=BB_ARRAY  && obj_stru.Type!=BB_TABLE){
//		printf("%s\n","Error: BB_WRITE_ARRAY_REC_BY_INDEX() input must be ARRAY or TABLE");
//		return false;
//	}
//	//取得记录块
//	if (!BB_GET_ARRAY_REC_BY_NUM_STRU(obj_stru,rec_num,&item,item_size)){
//		printf("%s\n","BB_WRITE_ARRAY_REC_BY_INDEX()-> BB_GET_ARRAY_REC_BY_NUM() error!");
//		return false;
//	}
//
//	//取得对象描述数组
//	bool has_obj_fields=BB_GET_ARRAY_OBJ_FIELDS(obj_stru,&obj_fields,obj_fields_num,obj_fields_size);
//
//	//循环写入处理
//	BB_OBJ_STRU cell_obj_stru = BB_GET_OBJ_STRU(obj_stru.CellObj);
//		for (BYTE i = 0; i < 32; i++)
//		{
//			//取得头字段大小
//			if (!BB_GET_ARRAY_REC_BY_NUM_STRU(cell_obj_stru,i,&cell_item,cell_item_size)){
//				printf("%s\n","BB_WRITE_ARRAY_REC_BY_INDEX() Bitmap->Get record number error !");
//				return false;
//			}
//			BB_SIZE_T field_size = (BB_SIZE_T)_scanUINT(cell_item,cell_item_size);
//
//			//遇到对应的bitmap为1则写入操作
//    		if (i==field_num){
//				//拷贝字段内容
//				if (BB_ARRAY_OBJ_RESIZE && has_obj_fields && check_array(obj_fields,obj_fields_size,obj_fields_num,&i,1)>=0){
//					//对象拷贝操作
//					BB_OBJ *new_obj;
//					BB_SET_ARRAY_OBJ_FIELD(NULL,buf, buf_size, field_size, &new_obj);
//					BYTE *dest=(BYTE *)memcpy((void *)(item+item_offset),(const void*)new_obj, BB_GET_OBJ_SIZE(new_obj));
//					free (new_obj);
//				}else{
//					if (buf_size < field_size)
//						BYTE *dest=(BYTE *)memcpy((void *)(item+item_offset),(const void*)(buf), buf_size);
//					else
//						BYTE *dest=(BYTE *)memcpy((void *)(item+item_offset),(const void*)(buf), field_size);
//				}
//			}
//			//数据基址item顺序累加
//			item_offset+=field_size;
//		}//only testing
//		//string test1;
//		//test1.append((const char *)item,item_offset);
//    return true;
//}

BB_SIZE_T BB_GET_TABLE_CELL_WIDTH(BB_OBJ *obj){//obj为表格table头信息对象-可能是数组或者表格
    //今后应添加更好的差错提示,
    UInt64 sum_width=0;
    UInt64 item_value=0;
    
    //必须是数组
    BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
    if (obj_stru.Type!=BB_ARRAY && obj_stru.Type!=BB_TABLE){
        return 0;
    }
    BB_SIZE_T j;
    if (obj_stru.Type==BB_ARRAY){
        for(j=0; j<obj_stru.Num; j++){//obj 为cell_width
            if (!BB_GET_ARRAY_UINT_VALUE_STRU(&obj_stru,j,&item_value))
                printf("%s,j=%zu","Error: BB_GET_TABLE_CELL_WIDTH()->BB_GET_ARRAY_UINT_VALUE_STRU() return error!",j);
            sum_width+=item_value;//缺省为第一个字段为length字段。
        }
    }else if (obj_stru.Type==BB_TABLE){
        for(j=0; j<obj_stru.Num; j++){
            if (!BB_GET_TABLE_ITEM_VALUE_STRU(&obj_stru,j,BB_TABLE_DEFAULT_CELL_LENGTH_FIELD,&item_value))
                printf("%s,j=%zu","Error: BB_GET_TABLE_CELL_WIDTH()->BB_GET_TABLE_ITEM_VALUE_STRU() return error!",j);
            sum_width+=item_value;//缺省为第一个字段为length字段。
        }
    }
    return (BB_SIZE_T)sum_width;
}

BB_SIZE_T BB_GET_TABLE_ITEMS_IDS_WIDTH_EXT(BB_OBJ_STRU *table_stru,BB_SIZE_T rec, BYTE *bitmap_str, BB_SIZE_T bitmap_size){
    UInt64 sum_width=0;
    UInt64 item_value=0;
    //必须是数组
    BB_OBJ_STRU cell_stru=BB_GET_OBJ_STRU(table_stru->CellObj);
    if (cell_stru.Type!=BB_ARRAY && cell_stru.Type!=BB_TABLE){
        return 0;
    }
    if (cell_stru.Type==BB_ARRAY){
        for(BB_SIZE_T j=0; j<bitmap_size; j++){//obj 为cell_width
            if (!BB_GET_ARRAY_UINT_VALUE_STRU(&cell_stru,*(bitmap_str+j),&item_value))
                printf("%s,j=%zu","Error: BB_GET_TABLE_CELL_WIDTH()->BB_GET_ARRAY_UINT_VALUE_STRU() return error!",j);
            sum_width+=item_value;//缺省为第一个字段为length字段。
        }
    }else if (cell_stru.Type==BB_TABLE){
        UInt64 field_type;
        BYTE *item_offset,type;
        BB_SIZE_T item_size;
        BB_OBJ *malloc_obj;
        for(BB_SIZE_T j=0; j<bitmap_size; j++){
            //取得头字段大小
            if (!BB_GET_TABLE_ITEM_VALUE_STRU(&cell_stru,*(bitmap_str+j),BB_TABLE_DEFAULT_CELL_LENGTH_FIELD,&item_value))
                printf("%s,j=%zu","Error: BB_GET_TABLE_CELL_WIDTH_EXT()->BB_GET_TABLE_ITEM_VALUE_STRU() return error!",j);
            if (!BB_GET_TABLE_ITEM_VALUE_STRU(&cell_stru,*(bitmap_str+j), BB_TABLE_DEFAULT_TYPE_FIELD, &field_type))
                printf("%s,j=%zu","Error: BB_GET_TABLE_CELL_WIDTH_EXT()->BB_GET_TABLE_ITEM_VALUE_STRU()->CELL_TYPE_FIELD return error!",j);
            
            if (field_type==BB_TABLE_FIELD_TYPE_MALLOC){//针对外部对象的扩展宽度处理。
                if (!BB_GET_TABLE_ITEMS_DATA_STRU(table_stru, rec,*(bitmap_str+j), &item_offset, &item_size))
                    printf("%s\n", "BB_WRITE_TABLE_BY_NUM()-> BB_GET_ARRAY_REC_BY_NUM() error!");
                if ((*(item_offset)) == BB_ARRAY_APPEND_DEFAULT_CHAR){
                    sum_width+=item_value;//缺省为第一个字段为length字段。
                }else {
                    if (!BB_GET_MALLOC_ADDRESS((BB_OBJ *)item_offset, (void **)&malloc_obj, &type))
                        printf("%s \n","BB_READ_TABLE_BY_NUM_STRU()->BB_TABLE_FIELD_TYPE_MALLOC: BB_NEW_MALLOC() error!");
                    sum_width+= BB_GET_OBJ_SIZE(malloc_obj);
                }
            }else{
                sum_width+=item_value;//缺省为第一个字段为length字段。
            }
        }
    }
    
    return (BB_SIZE_T)sum_width;
}
BYTE BB_CHECK_METHOD(BYTE method){
    if ((method | BB_TABLE_REC_WR_BITIDS_CONVERT)==BB_TABLE_REC_WR_BITIDS_CONVERT){
        return BB_TABLE_REC_WR_BITIDS_CONVERT;
    }else if ((method & BB_TABLE_REC_WR_BITIDS) == BB_TABLE_REC_WR_BITIDS) {
        return BB_TABLE_REC_WR_BITIDS;
    }else {
        return BB_TABLE_REC_WR_BITMAP;
    }
}

BB_SIZE_T BB_GET_TABLE_CELL_WIDTH_EXT(BB_OBJ_STRU *table_stru,BB_SIZE_T num){
    UInt64 sum_width=0;
    UInt64 item_value=0;
    //必须是数组
    BB_OBJ_STRU cell_stru=BB_GET_OBJ_STRU(table_stru->CellObj);
    if (cell_stru.Type!=BB_ARRAY && cell_stru.Type!=BB_TABLE){
        return 0;
    }
    if (cell_stru.Type==BB_ARRAY){
        for(BB_SIZE_T j=0; j<cell_stru.Num; j++){//obj 为cell_width
            if (!BB_GET_ARRAY_UINT_VALUE_STRU(&cell_stru,j,&item_value))
            printf("%s,j=%zu","Error: BB_GET_TABLE_CELL_WIDTH()->BB_GET_ARRAY_UINT_VALUE_STRU() return error!",j);
            sum_width+=item_value;//缺省为第一个字段为length字段。
        }
    }else if (cell_stru.Type==BB_TABLE){
        UInt64 field_type;
        BYTE *item_offset,type;
        BB_SIZE_T item_size;
        BB_OBJ *malloc_obj;
        for(BB_SIZE_T j=0; j<cell_stru.Num; j++){
            //取得头字段大小
            if (!BB_GET_TABLE_ITEM_VALUE_STRU(&cell_stru,(UInt32)j,BB_TABLE_DEFAULT_CELL_LENGTH_FIELD,&item_value))
            printf("%s,j=%zu","Error: BB_GET_TABLE_CELL_WIDTH_EXT()->BB_GET_TABLE_ITEM_VALUE_STRU() return error!",j);
            if (!BB_GET_TABLE_ITEM_VALUE_STRU(&cell_stru,(UInt32)j, BB_TABLE_DEFAULT_TYPE_FIELD, &field_type))
            printf("%s,j=%zu","Error: BB_GET_TABLE_CELL_WIDTH_EXT()->BB_GET_TABLE_ITEM_VALUE_STRU()->CELL_TYPE_FIELD return error!",j);
            
            if (field_type==BB_TABLE_FIELD_TYPE_MALLOC){//针对外部对象的扩展宽度处理。
                if (!BB_GET_TABLE_ITEMS_DATA_STRU(table_stru, num, j, &item_offset, &item_size))
                printf("%s\n", "BB_WRITE_TABLE_BY_NUM()-> BB_GET_ARRAY_REC_BY_NUM() error!");
                if (!BB_GET_MALLOC_ADDRESS((BB_OBJ *)item_offset, (void **)&malloc_obj, &type))
                printf("%s \n","BB_READ_TABLE_BY_NUM_STRU()->BB_TABLE_FIELD_TYPE_MALLOC: BB_NEW_MALLOC() error!");
                sum_width+= BB_GET_OBJ_SIZE(malloc_obj);
            }else{
                sum_width+=item_value;//缺省为第一个字段为length字段。
            }
        }
    }
    return (BB_SIZE_T)sum_width;
}

BB_SIZE_T BB_GET_TABLE_FIELD_WIDTH(BB_OBJ *cell_obj,BB_SIZE_T field_num){//obj:为表格头对象,
    //今后应添加更好的差错提示,
    //必须是数组
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU(cell_obj);
    return BB_GET_TABLE_FIELD_WIDTH_STRU(&cell_obj_stru,field_num);
}

BB_SIZE_T BB_GET_TABLE_FIELD_WIDTH_STRU(BB_OBJ_STRU *cell_obj_stru,BB_SIZE_T field_num){//取得表格字段的列宽度
    BYTE* item_offset;
    BB_SIZE_T item_size=0;
    UInt64 field_value;
    
    if ((*cell_obj_stru).Type!=BB_ARRAY && (*cell_obj_stru).Type!=BB_TABLE){
        printf("%s\n","Error: BB_GET_TABLE_FIELD_WIDTH() cell_obj's type must be ARRAY or TABLE");
        return BB_SIZE_T_ERROR;
    }
    
    if ((*cell_obj_stru).Type==BB_ARRAY){
        if (!BB_GET_ARRAY_UINT_VALUE_STRU(cell_obj_stru,field_num,&field_value)){
            printf("%s,field_num=%u","Error: BB_GET_TABLE_FIELD_WIDTH_STRU()->BB_GET_ARRAY_UINT_VALUE_STRU() return error!",field_num);
            return BB_SIZE_T_ERROR;
        }else
            return (BB_SIZE_T)field_value;//缺省为第一个字段为length字段。
    }else if ((*cell_obj_stru).Type==BB_TABLE){
        if (BB_GET_TABLE_ITEMS_DATA_STRU(cell_obj_stru,field_num,BB_TABLE_DEFAULT_CELL_LENGTH_FIELD,&item_offset,&item_size))
            return (BB_SIZE_T)_scanUINT(item_offset,item_size);
    }
    printf("%s %u\n","Error: BB_GET_TABLE_FIELD_WIDTH() error field_num is",field_num);
    return BB_SIZE_T_ERROR;
}

BB_SIZE_T BB_GET_TABLE_FIELD_TYPE(BB_OBJ *cell_obj,BB_SIZE_T field_num){//obj:为表格头对象,
    //今后应添加更好的差错提示,
    //必须是数组
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU(cell_obj);
    return BB_GET_TABLE_FIELD_TYPE_STRU(&cell_obj_stru,field_num);
}

BB_SIZE_T BB_GET_TABLE_FIELD_TYPE_STRU(BB_OBJ_STRU *cell_obj_stru,BB_SIZE_T field_num){//取得表格字段的列宽度
    BYTE* item_offset;
    BB_SIZE_T item_size=0;
    //UInt64 field_value;
    
    if ((*cell_obj_stru).Type!=BB_ARRAY && (*cell_obj_stru).Type!=BB_TABLE){
        printf("%s\n","Error: BB_GET_TABLE_FIELD_YPTE() cell_obj's type must be ARRAY or TABLE");
        return BB_SIZE_T_ERROR;
    }
    
    if ((*cell_obj_stru).Type==BB_ARRAY){
        printf("%s,field_num=%u","Error: BB_GET_TABLE_FIELD_TYPE_STRU()->BB_GET_ARRAY_UINT_VALUE_STRU() return error!",field_num);
        return BB_SIZE_T_ERROR;
    }else if ((*cell_obj_stru).Type==BB_TABLE){
        if (BB_GET_TABLE_ITEMS_DATA_STRU(cell_obj_stru,field_num,BB_TABLE_DEFAULT_TYPE_FIELD,&item_offset,&item_size))
            return (BB_SIZE_T)_scanUINT(item_offset,item_size);
    }
    printf("%s %u\n","Error: BB_GET_TABLE_FIELD_TYPE() error field_num is",field_num);
    return BB_SIZE_T_ERROR;
}


bool BB_GET_TABLE_FIELD_NAME_STRU(BB_OBJ_STRU *cell_obj_stru,BYTE field_num,BYTE** item_offset,BB_SIZE_T *item_size){//取得表格字段的列宽度 //ly cell_name; ly task
    if ((*cell_obj_stru).Type!=BB_TABLE){
        printf("%s\n","Error: BB_GET_TABLE_FIELD_WIDTH() cell_obj's type must be ARRAY or TABLE");
        return false;
    }
    
    if (BB_GET_TABLE_ITEMS_DATA_STRU(cell_obj_stru,field_num,BB_TABLE_CELL_NAME_FIELD,item_offset,item_size))
        return true;
    
    return false;
}

int BB_WRITE_TABLE_BY_INDEX(BB_OBJ*obj, BYTE* buf, BB_SIZE_T *bufoffset,  BYTE *index_str, BYTE index_str_size, BYTE *bitmap_str, BYTE bitmap_size,BYTE method){
    BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
    
    //BB_SIZE_T item_offset=0;
    int num;
    
    //必须是数组
    if (obj_stru.Type!=BB_TABLE){
        printf("%s\n","Error: BB_WRITE_TABLE_BY_INDEX() write in object must be TABLE");
        return -1;
    }
    
    //根据node索引查找相关记录，如果没有则插入
    if ((num=BB_GET_ARRAY_NUM_BY_INDEX(obj,index_str,index_str_size))<0){
        if (num!=-1){
            printf("BB_WRITE_TABLE_BY_INDEX()'s Index config error!\n");
            return -2;//配置化错误
        }
        //添加索引、记录
        num=BB_APPEND_ARRAY_REC_BY_INDEX(obj,NULL,0,index_str,index_str_size); //双向及指针，其中包含清ff操作ly add: 需要db->InDEX的配置处理。
        if (num<0){
            printf("BB_WRITE_TABLE_BY_INDEX() add Index process error!\n");
            return -3;
        }
    }
    
    int r;
    if ((r = BB_WRITE_TABLE_BY_NUM(obj, buf, bufoffset, num, bitmap_str, bitmap_size, method)) < 0) {
        printf("BB_WRITE_TABLE_BY_INDEX() ->BB_WRITE_TABLE_BY_NUM()write in error!\n");
        return -4+r;
    }
    else
        return num;
}
int BB_WRITE_TABLE_BY_NUM(BB_OBJ*obj, BYTE* buf, BB_SIZE_T *bufoffset, BB_SIZE_T num, BYTE *bitmap_str, BYTE bitmap_size, BYTE method) {
    BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
    
    if (obj_stru.LoadError) {
        printf("%s\n", "Error:BB_READ_TABLE_BY_NUM() load table error!");
        return -1;
    }
    
    return BB_WRITE_TABLE_BY_NUM_STRU(&obj_stru, buf, bufoffset, num, bitmap_str, bitmap_size, method);
}
int BB_WRITE_TABLE_BY_NUM_STRU(BB_OBJ_STRU *obj_stru, BYTE* buf, BB_SIZE_T *bufoffset, BB_SIZE_T num, BYTE *bitmap_str, BYTE bitmap_size, BYTE method) {
    //说明：method支持三种写入内容方式：1: bitmap模式 2:bit_start_end模式(连续字段写入)
    BB_OBJ *obj = obj_stru->ThisObj;
    BB_SIZE_T field_size = 0;
    BYTE *item;
    UInt64 field_type = 0;
    
    //必须是数组
    if (obj_stru->Type != BB_TABLE) {
        printf("%s\n", "Error: BB_WRITE_TABLE_BY_NUM() input must be TABLE");
        return -1;
    }
    
    BB_OBJ_STRU cell_obj_stru = BB_GET_OBJ_STRU(obj_stru->CellObj);
    if (obj_stru->Type != BB_TABLE && obj_stru->Type != BB_ARRAY) {
        printf("%s\n", "Error: BB_WRITE_TABLE_BY_NUM() cell_type must be ARRAY or TABLE");
        return -2;
    }
    BB_SIZE_T i;
    if ((method & BB_TABLE_REC_WR_RETURN_TABLE) == BB_TABLE_REC_WR_RETURN_TABLE) {
        UInt64 dest_id=0, buf_field_size;
        BB_OBJ *convert_obj = (BB_OBJ*)bitmap_str;
        BB_OBJ_STRU convert_obj_stru = BB_GET_OBJ_STRU(convert_obj);
        
        //for ( i= 0; i < 5; i++)
        for (i = 0; i < convert_obj_stru.Num; i++)
        {
            //换算目标数据库id
            BB_GET_ARRAY_UINT_VALUE(convert_obj_stru.IndexObj, i, &dest_id);//取得目标id
            if (!BB_GET_ARRAY_UINT_VALUE_STRU(&convert_obj_stru, i, &buf_field_size)) {//取得源字段宽度
                printf("%s %u\n", "BB_WRITE_TABLE_BY_NUM()->BB_GET_ARRAY_UINT_VALUE:get source id field width error ! num=", i);
                return -3;//to_add
            }
            
            if (!BB_PUT_TABLE_ITEMS_DATA(obj, (BB_SIZE_T)num, (BB_SIZE_T)dest_id, buf + (*bufoffset), (BB_SIZE_T)buf_field_size, 0)) {//写入操作，需内部解决字段长度删减 ???此处效率太低今后可以考虑format再排序模式
                printf("%s %u\n", "BB_WRITE_TABLE_BY_NUM()->BB_PUT_TABLE_ITEMS_DATA:can't wirte to dest db ! num=", i);
                return -4;//to_add
            }
            //printf("%s","BB_TABLE_REC_WR_RETURN_TABLE ");
            //数据基址item顺序累加
            (*bufoffset) += (BB_SIZE_T)buf_field_size;
            //printf("%d-%d;", buf_field_size, dest_id);
        }
    }
    else if ((method & BB_TABLE_REC_WR_BITIDS) == BB_TABLE_REC_WR_BITIDS) {//对于BitIDS采用对象回传模式, ly:ids
        BB_OBJ_STRU data_stru = BB_GET_OBJ_STRU(buf);
        if (data_stru.LoadError || data_stru.Type!=BB_ARRAY ||(bitmap_size > data_stru.Num)) {
            printf("BB_WRITE_TABLE_BY_NUM()-> ColumnID.Num(%u)!=Data.Num(%lu)\n", bitmap_size, (unsigned long)data_stru.Num);
            return -5;//to_add
        }
        
        //char test[1000];//ly test;
        for (i = 0; i < bitmap_size; i++)
        {
            if (*(bitmap_str + i) >= cell_obj_stru.Num) {
                printf("BB_WRITE_TABLE_BY_NUM()->BB_PUT_TABLE_ITEMS_DATA:col num(%u)is out of cell.Num(%zu) ", *(bitmap_str + i), cell_obj_stru.Num);
                return -6;//to_add
            }
            //根据不同的针对table判断field_type
            if (cell_obj_stru.Type == BB_TABLE)
                BB_GET_TABLE_ITEM_VALUE_STRU(&cell_obj_stru, *(bitmap_str + i), BB_TABLE_DEFAULT_TYPE_FIELD, &field_type); //ly mac
            else
                field_type = BB_TABLE_FIELD_TYPE_BINARY;
            
            //读出操作
            //BYTE *field_pos = buf + (*bufoffset);	//取得当前对象
            BYTE *field_pos = data_stru.Data + (*bufoffset);	//取得当前对象
            if ((BB_GET_OBJ_TYPE((BB_OBJ *)field_pos) != BB_BYTES) && (BB_GET_OBJ_TYPE((BB_OBJ *)field_pos) != BB_BYTES_EXT) && !BB_IS_OBJ_STRING((BB_OBJ *)field_pos)) {
                if (!BB_IS_BYTES_ALL((BB_OBJ *)field_pos)){//ly malloc2.2 注：BB_IS_BYTES_ALL是针对0x无限二进制流；OBJ_STRING是针对自描述字符串
                    printf("%s %u\n", "BB_WRITE_TABLE_BY_NUM()->BB_PUT_TABLE_ITEMS_DATA:src obj is not BB_BYTES or BB_BYTES_EXT ! field_num=", i);
                    return -7;//to_add
                }
            }
            //memcpy(test, field_pos, BB_GET_OBJ_SIZE(field_pos));	//ly test
            
            if (field_type == BB_TABLE_FIELD_TYPE_OBJECT ) {
                //写入字串对象
                if (!BB_PUT_TABLE_ITEMS_DATA(obj, (BB_SIZE_T)num, *(bitmap_str + i), field_pos, BB_GET_OBJ_SIZE(field_pos), method & BB_TABLE_PUT_OPERATE_MARK)) {//写入操作，需内部解决字段长度删减 ???此处效率太低今后可以考虑format再排序模式
                    printf("%s %u\n", "BB_WRITE_TABLE_BY_NUM()->BB_PUT_TABLE_ITEMS_DATA:can't wirte to dest db ! num=", i);
                    return -8;//to_add
                }
            }else if(field_type == BB_TABLE_FIELD_TYPE_MALLOC ) {//ly malloc2;ly task
                //写入字串对象
                if (!BB_PUT_TABLE_ITEMS_DATA(obj, (BB_SIZE_T)num, *(bitmap_str + i), field_pos, BB_GET_OBJ_SIZE(field_pos), method & BB_TABLE_PUT_OPERATE_MARK)) {//写入操作，需内部解决字段长度删减 ???此处效率太低今后可以考虑format再排序模式
                    printf("%s %u\n", "BB_WRITE_TABLE_BY_NUM()->BB_PUT_TABLE_ITEMS_DATA:can't wirte to dest db ! num=", i);
                    return -8;//to_add
                }
            }
            else {
                //写入源字串内容
                if (!BB_PUT_TABLE_ITEMS_DATA(obj, (BB_SIZE_T)num, *(bitmap_str + i), (BYTE *)BB_GET_OBJ_DATA((BYTE *)field_pos), BB_GET_OBJ_DATA_LEN(field_pos), method & BB_TABLE_PUT_OPERATE_MARK)) {//写入操作，需内部解决字段长度删减 ???此处效率太低今后可以考虑format再排序模式
                    printf("%s %u\n", "BB_WRITE_TABLE_BY_NUM()->BB_PUT_TABLE_ITEMS_DATA:can't wirte to dest db ! num=", i);
                    return -9;//to_add
                }
            }
            
            //数据基址item顺序累加
            (*bufoffset) += (BB_SIZE_T)BB_GET_OBJ_SIZE(field_pos);
        }
    }
    else if ((method & BB_TABLE_REC_WR_BITSTARTEND) == BB_TABLE_REC_WR_BITSTARTEND) {
        //bitmap记录写入 ???今后改为自适应长度UINT8/16/32/64
        BB_BITMAP_TYPE bitmap;
        BB_SIZE_T item_size = 0, item_offset = 0;
        bitmap = (bitmap_size == 0) ? 0 : (BB_BITMAP_TYPE)_scanUINT(bitmap_str, bitmap_size);
        
        //取得记录块
        if (!BB_GET_ARRAY_REC_BY_NUM_STRU(obj_stru, num, &item, &item_size)) {//???
            printf("%s\n", "BB_WRITE_TABLE_BY_NUM()-> BB_GET_ARRAY_REC_BY_NUM() error!");
            return -10;
        }
        
        //特殊情况以(*bufoffset)记载记录长度,确保不能过大
        if ((*bufoffset) > obj_stru->CellWidth) {
            printf("BB_WRITE_TABLE_BY_NUM()-> buff_len(%zu) is large than cell_width(%zu)!", (*bufoffset), obj_stru->CellWidth);
            return -11;
        }
        
        for (i = 0; i < cell_obj_stru.Num; i++)
        {
            if ((field_size = BB_GET_TABLE_FIELD_WIDTH_STRU(&cell_obj_stru, i)) == BB_SIZE_T_ERROR) {
                printf("BB_WRITE_TABLE_BY_NUM()->BB_GET_TABLE_FIELD_WIDTH()error ! cell field_num is %u", i);
                return -23;//to_add
            }
            if ((((bitmap << (i + 1)) | (bitmap >> ((bitmap_size * 8 - 1) - i))) & 1) > 0) {
                //数值写入
                memset((void *)(item + item_offset), BB_ARRAY_APPEND_DEFAULT_CHAR, (*bufoffset));//清空原字串。 ly: string;
                memcpy((void *)(item + item_offset), (const void*)buf, (*bufoffset));
                return 0;
            }
            //数据基址item顺序累加
            item_offset += field_size;
        }
    }
    else {
        //if BB_TABLE_REC_WR_BITMAP
        //bitmap记录写入 ???今后改为自适应长度UINT8/16/32/64
        BB_BITMAP_TYPE bitmap;
        BB_SIZE_T item_size = 0, item_offset = 0;
        bitmap = (bitmap_size == 0) ? 0 : (BB_BITMAP_TYPE)_scanUINT(bitmap_str, bitmap_size);
        
        //取得记录块
        if (!BB_GET_ARRAY_REC_BY_NUM_STRU(obj_stru, num, &item, &item_size)) {//???
            printf("%s\n", "BB_WRITE_TABLE_BY_NUM()-> BB_GET_ARRAY_REC_BY_NUM() error!");
            return -12;
        }
        
        for (i = 0; i < cell_obj_stru.Num; i++)
        {
            if ((field_size = BB_GET_TABLE_FIELD_WIDTH_STRU(&cell_obj_stru, i)) == BB_SIZE_T_ERROR) {
                printf("%s %u\n", "BB_WRITE_TABLE_BY_NUM()->BB_GET_TABLE_FIELD_WIDTH()error ! cell field_num is ", i);
                return -13;//to_add
            }
            if ((((bitmap << (i + 1)) | (bitmap >> ((bitmap_size * 8 - 1) - i))) & 1) > 0) {
                //写入操作	
                //BB_GET_TABLE_ITEM_VALUE_STRU(&cell_obj_stru,i,BB_TABLE_DEFAULT_TYPE_FIELD,&field_type);
                if (cell_obj_stru.Type == BB_TABLE)
                    BB_GET_TABLE_ITEM_VALUE_STRU(&cell_obj_stru, i, BB_TABLE_DEFAULT_TYPE_FIELD, &field_type);
                else
                    field_type = BB_TABLE_FIELD_TYPE_BINARY;
                if (field_type == BB_TABLE_FIELD_TYPE_OBJECT) {
                    //全对象写入
                    //BYTE type = BB_GET_OBJ_TYPE((BB_OBJ *)(buf + (*bufoffset)));	//ly mac
                    /*if (type != BB_BYTES && type != BB_BYTES_EXT) {
                     printf("%s %d\n", "BB_WRITE_TABLE_BY_NUM()->BB_TABLE_FIELD_TYPE_OBJECT:input object field is not object! field num=", i);
                     return false;//to_add
                     }*/
                    BB_SIZE_T obj_buf_size = BB_GET_OBJ_SIZE((BB_OBJ *)(buf + (*bufoffset)));
                    memcpy((void *)(item + item_offset), (const void*)(buf + (*bufoffset)), obj_buf_size > field_size ? field_size : obj_buf_size);
                    //std::string test_str((const char*)(buf+(*bufoffset)),obj_buf_size > field_size ? field_size : obj_buf_size);
                    (*bufoffset) += obj_buf_size;
                }
                else {
                    //数值写入
                    memset((void *)(item + item_offset), BB_ARRAY_APPEND_DEFAULT_CHAR, field_size);//清空原字串。 ly: string;
                    memcpy((void *)(item + item_offset), (const void*)(buf + (*bufoffset)), field_size);
                    //std::string test_str((const char*)(buf+(*bufoffset)),field_size);
                    (*bufoffset) += field_size;
                }
            }
            //数据基址item顺序累加
            item_offset += field_size;
        }
    }
    
    return 0;
}
bool BB_READ_TABLE_BY_NUM(BB_OBJ*obj, BB_SIZE_T num, BYTE* buf, BB_SIZE_T *bufoffset, BYTE *bitmap_str, BYTE bitmap_size, BYTE method) {
    BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
    
    //必须是数组
    if (obj_stru.LoadError) {
        printf("%s\n", "Error:BB_READ_TABLE_BY_NUM() load table error!");
        return false;
    }
    
    return BB_READ_TABLE_BY_NUM_STRU(&obj_stru, num, buf, bufoffset, bitmap_str, bitmap_size, method);
}
bool BB_READ_TABLE_BY_NUM_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T num, BYTE* buf, BB_SIZE_T *bufoffset, BYTE *bitmap_str, BYTE bitmap_size,BYTE method){
    //BB_OBJ *obj = obj_stru->ThisObj; //ly mac
    BYTE *item;
    BB_SIZE_T item_size=0,item_offset=0,resize_field_size,field_size;
    //BYTE obj_fields_size=0;
    //BYTE null_obj=BB_NULL;
    UInt64 field_type = 0;
    
    //必须是数组
    if (obj_stru->Type!=BB_TABLE){
        printf("%s\n","Error:BB_READ_TABLE_BY_NUM() input must be BB_TABLE");
        return false;
    }
    
    //根据node索引查找相关记录，如果没有则插入
    if (!BB_GET_ARRAY_REC_BY_NUM_STRU(obj_stru,num,&item,&item_size))
        return false;
    
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU(obj_stru->CellObj);
    if (obj_stru->Type!=BB_TABLE && obj_stru->Type!=BB_ARRAY ){
        printf("%s\n","Error: BB_READ_TABLE_BY_NUM() cell_type must be ARRAY or TABLE");
        return false;
    }
    
    //取得对象描述数组
    BB_SIZE_T i ;
    //if ((method | BB_TABLE_REC_WR_BITIDS_CONVERT)==BB_TABLE_REC_WR_BITIDS_CONVERT){
    if ((method & BB_TABLE_REC_WR_BITIDS_CONVERT)==BB_TABLE_REC_WR_BITIDS_CONVERT){//ly table_dir error?
        UInt64 src_id;
        BB_OBJ *req_ids_array = (BB_OBJ *)bitmap_str;//bitmap为ids_array;
        BB_OBJ_STRU req_ids_array_stru=BB_GET_OBJ_STRU(req_ids_array);
        for (i = 0; i < req_ids_array_stru.Num; i++)
        {
            //换算目标数据库id
            if (!BB_GET_ARRAY_UINT_VALUE_STRU(&req_ids_array_stru,i,&src_id)){//转换取得目标id
                printf("%s i=%d \n","BB_READ_TABLE_BY_NUM()->BB_GET_ARRAY_UINT_VALUE: can't read ids index !",i);
                return false;//to_add
            }
            
            //读取数据库内容
            BYTE *field_pos;
            if (!BB_GET_TABLE_ITEMS_DATA_STRU(obj_stru,(BB_SIZE_T)num,(BB_SIZE_T)src_id,&field_pos,&field_size)){//reading操作
                printf("%s num=%zu,src_id=%llu \n","BB_READ_TABLE_BY_NUM()->BB_GET_TABLE_ITEMS_DATA_STRU: can't read db field !", num, src_id);
                return false;//to_add
            }
            //拷贝动作
            memcpy((void *)(buf + (*bufoffset)), (const void*)field_pos, field_size);//value拷贝操作
            //数据基址item顺序累加
            (*bufoffset)+=field_size;	
        }
    }else if ((method & BB_TABLE_REC_WR_BITIDS) == BB_TABLE_REC_WR_BITIDS) {
        for (i = 0; i < bitmap_size; i++)
        {
            //读取数据库内容
            BYTE *field_pos;
            if (!BB_GET_TABLE_ITEMS_DATA_STRU(obj_stru, (BB_SIZE_T)num, *(bitmap_str+i), &field_pos, &field_size)) {//reading操作
                printf("%s num=%zu,src_id=%d \n", "BB_READ_TABLE_BY_NUM()->BB_GET_TABLE_ITEMS_DATA_STRU: can't read db field !", num, *(bitmap_str + i));
                return false;//to_add
            }
            //根据不同的针对table判断field_type
            if (cell_obj_stru.Type == BB_TABLE)
                BB_GET_TABLE_ITEM_VALUE_STRU(&cell_obj_stru, *(bitmap_str + i), BB_TABLE_DEFAULT_TYPE_FIELD, &field_type); //ly mac
            else
                field_type = BB_TABLE_FIELD_TYPE_BINARY;
            
            //读出操作
            if (field_type == BB_TABLE_FIELD_TYPE_OBJECT) {
                //对象值返回,则直接取得对象
                if (*(field_pos) == BB_ARRAY_APPEND_DEFAULT_CHAR) {//对于没有初始化值的情况，创建一个空值，???测试两种返回;
                    field_pos = &BB_null_obj;
                }
                field_size = BB_GET_OBJ_SIZE(field_pos);
                memcpy((void *)(buf + (*bufoffset)), (const void*)field_pos, field_size);//value拷贝操作
                (*bufoffset) += field_size;
            }else if (field_type == BB_TABLE_FIELD_TYPE_MALLOC) {//ly malloc2; ly task; 如果STRING FIELD则只取得ARRAY_APPEND_DEFAULT_CHAR（0xff）前面的内容ly: string
                //
                if (*(field_pos) == BB_ARRAY_APPEND_DEFAULT_CHAR) {//对于没有初始化值的情况，创建一个空值，???测试两种返回;
                    field_pos = &BB_null_obj;
                    field_size = BB_GET_OBJ_SIZE(field_pos);
                    memcpy((void *)(buf + (*bufoffset)), (const void*)field_pos, field_size);
                    (*bufoffset) += field_size;
                }else{
                    field_size = BB_GET_OBJ_SIZE(field_pos);
                    //读取对象
                    BB_OBJ *malloc_obj;
                    BYTE type;
                    if (!BB_GET_MALLOC_ADDRESS(field_pos, (void **)&malloc_obj, &type)) {
                        printf("%s \n","BB_READ_TABLE_BY_NUM_STRU()->BB_TABLE_FIELD_TYPE_MALLOC: BB_NEW_MALLOC() error!");
                        return false;//to_add
                    }
                    //拷贝对象
                    memcpy((void *)(buf + (*bufoffset)), (const void*)malloc_obj, BB_GET_OBJ_SIZE(malloc_obj));
                    (*bufoffset) += BB_GET_OBJ_SIZE(malloc_obj);
                }
            }else {
                //普通则需创建对象
                BB_OBJ	*new_obj;
                if (field_type == BB_TABLE_FIELD_TYPE_NUMBER) {
                    uint64 num = BB_ScanUINTWidth(field_pos, field_size);
                    //BB_UINT_STRU new_obj_uint=BB_NEW_CELL_UINT(num,BB_UINT64);
                    BB_UINT_STRU new_obj_uint=BB_NEW_CELL_UINT(num,field_size);
                    new_obj = (BB_OBJ *)&new_obj_uint;
                    memcpy((void *)(buf + (*bufoffset)), (const void*)new_obj, BB_GET_OBJ_SIZE(new_obj));
                    (*bufoffset) += BB_GET_OBJ_SIZE(new_obj);
                }else if (field_type == BB_TABLE_FIELD_TYPE_STRING) {//如果STRING FIELD则只取得ARRAY_APPEND_DEFAULT_CHAR（0xff）前面的内容ly: string
                    field_size = get_table_field_string(field_pos, field_size); 
                    new_obj = BB_NEW_OBJ_STRING(field_pos, field_size, NULL);
                    memcpy((void *)(buf + (*bufoffset)), (const void*)new_obj, BB_GET_OBJ_SIZE(new_obj));
                    (*bufoffset) += BB_GET_OBJ_SIZE(new_obj);
                    if(new_obj!=NULL)
                        free(new_obj);
                }
                else {
                    /*if (field_size < (BB_0DIM_BYTES_MAX_LEN - BB_OBJ_STX_LEN))
                     new_obj = BB_NEW_CELL(BB_BYTES, field_pos, field_size, NULL);//暂时用BB_BYTES今后可以直接从外部设定对象内容
                     else
                     new_obj = BB_NEW_CELL(BB_BYTES_EXT, field_pos, field_size >(BB_0DIM_BYTES_EXT_MAX_LEN - BB_OBJ_STX_EXT_LEN) ? (BB_0DIM_BYTES_EXT_MAX_LEN - BB_OBJ_STX_EXT_LEN) : field_size, NULL);//暂时用BB_BYTES今后可以直接从外部设定对象内容*/
                    new_obj = BB_NEW_BYTES_ALL(field_pos, field_size, NULL);//自适应BYTE,BYTE_EXT,BB_ARRAY
                    memcpy((void *)(buf + (*bufoffset)), (const void*)new_obj, BB_GET_OBJ_SIZE(new_obj));
                    (*bufoffset) += BB_GET_OBJ_SIZE(new_obj);
                    if (new_obj!=NULL)
                        free(new_obj);
                }
                
            }
        }
    }else {
        //if BB_TABLE_REC_WR_BITMAP
        BB_BITMAP_TYPE bitmap;
        if (bitmap_size > 64){
            printf("Error:BB_READ_TABLE_BY_NUM()bitmap is not 32 or 64!");
            return false;
        }
        bitmap=(bitmap_size==0) ? 0:(BB_BITMAP_TYPE) _scanUINT(bitmap_str,bitmap_size);
        
        //循环字段列读取列字段
        for (i = 0; i < cell_obj_stru.Num; i++)
        {
            //取得自身字段宽
            if ((field_size=BB_GET_TABLE_FIELD_WIDTH_STRU(&cell_obj_stru,i))==BB_SIZE_T_ERROR){
                printf("%s %u\n","BB_READ_TABLE_BY_NUM()->BB_GET_TABLE_FIELD_WIDTH2()error ! cell field_num is ", i);
                return false;//to_add
            }
            if ((((bitmap << (i + 1)) | (bitmap >> ((bitmap_size*8-1) - i))) & 1) > 0){
                BYTE *field_pos=item+item_offset;
                if (cell_obj_stru.Type==BB_TABLE)
                    BB_GET_TABLE_ITEM_VALUE_STRU(&cell_obj_stru,i,BB_TABLE_DEFAULT_TYPE_FIELD,&field_type);
                else
                    field_type=BB_TABLE_FIELD_TYPE_BINARY;
                
                //判断是否为对象:有三种写入形式：（1）普通field返回内容长度 （2）对象字段，返回对象值
                if (field_type == BB_TABLE_FIELD_TYPE_OBJECT) {
                    //对象返回
                    if (*(field_pos) == BB_ARRAY_APPEND_DEFAULT_CHAR)
                        field_pos = &BB_null_obj;//对于没有初始化值的情况，创建一个空值，???测试两种返回;
                    if ((method & BB_TABLE_REC_WR_OBJ_DATA_VALUE) == BB_TABLE_REC_WR_OBJ_DATA_VALUE) {
                        //只取得对象值
                        resize_field_size = BB_GET_CELL_DATA_LEN(field_pos);
                        BYTE *resize_value = BB_GET_CELL_DATA(field_pos); //BB_GET_CELL_DATA(item+item_offset);
                        memcpy((void *)(buf + (*bufoffset)), resize_value, resize_field_size);
                        (*bufoffset) += resize_field_size;
                    }else {
                        //取得全对象，今后测试对象跳转的情况
                        resize_field_size = BB_GET_OBJ_SIZE(field_pos);
                        memcpy((void *)(buf + (*bufoffset)), field_pos, resize_field_size);
                        (*bufoffset) += resize_field_size;				
                    }
                }else{
                    //非对象数值返回//普通field返回	
                    memcpy((void *)(buf + (*bufoffset)), (const void*)(field_pos), field_size);//value拷贝操作
                    (*bufoffset) += field_size;
                }
            }
            //数据基址item顺序累加
            item_offset+=field_size;
        }
    }
    return true;
}

bool BB_GET_TABLE_ITEMS_NAME_DATA_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T row,const char *name,BYTE **data,BB_SIZE_T *data_size){
    BYTE col;
    if ((col= BB_GET_TABLE_FIELD_NAME_ID(obj_stru->CellObj,name))<0)
        return false;   
    return BB_GET_TABLE_ITEMS_ID_DATA_STRU(obj_stru,row,col,data,data_size);
}

bool BB_GET_TABLE_ITEMS_ID_OBJ(BB_OBJ *obj, BB_SIZE_T row,BYTE col,BYTE **data,BB_SIZE_T *data_size){//malloc2.1
    BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
    
    if (obj_stru.LoadError) {
        printf("%s\n", "Error:BB_GET_TABLE_ITEMS_ID_OBJ() load table error!");
        return false;
    }
    return BB_GET_TABLE_ITEMS_ID_OBJ_STRU(&obj_stru, row,col,data,data_size);
}

bool BB_GET_TABLE_ITEMS_ID_OBJ_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T row,BYTE col,BYTE **data,BB_SIZE_T *data_size){
    //和BB_GET_TABLE_ITEMS_ID_OBJ_STRU的区别在于BB_TABLE_FIELD_TYPE_OBJECT；BB_TABLE_FIELD_TYPE_MALLOC；返回的都是原始对象。
    BYTE *item;
    BB_SIZE_T item_size=0,field_size;
    //BYTE null_obj=BB_NULL;
    UInt64 field_type = 0;
    
    //必须是数组
    if (obj_stru->Type!=BB_TABLE){
        printf("%s\n","Error:BB_GET_TABLE_ITEMS_ID_OBJ_STRU() input must be BB_TABLE");
        return false;
    }
    //根据node索引查找相关记录，如果没有则插入
    if (!BB_GET_ARRAY_REC_BY_NUM_STRU(obj_stru,row,&item,&item_size))
        return false;
    
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU(obj_stru->CellObj);
    if (obj_stru->Type!=BB_TABLE && obj_stru->Type!=BB_ARRAY ){
        printf("%s\n","Error: BB_GET_TABLE_ITEMS_ID_OBJ_STRU() cell_type must be ARRAY or TABLE");
        return false;
    }
    
    //取得对象描述数组
    BYTE *field_pos;
    if (!BB_GET_TABLE_ITEMS_DATA_STRU(obj_stru, row, col, &field_pos, &field_size)) {//reading操作
        printf("%s num=%zu,src_id=%d \n", "BB_GET_TABLE_ITEMS_ID_DATA_STRU()->BB_GET_TABLE_ITEMS_DATA_STRU: can't read db field !", row, col);
        return false;//to_add
    }
    //根据不同的针对table判断field_type
    if (cell_obj_stru.Type == BB_TABLE)
        BB_GET_TABLE_ITEM_VALUE_STRU(&cell_obj_stru, col, BB_TABLE_DEFAULT_TYPE_FIELD, &field_type); //ly mac
    else
        field_type = BB_TABLE_FIELD_TYPE_BINARY;
    
    //读出操作
    if (field_type == BB_TABLE_FIELD_TYPE_OBJECT) {
        //对象值返回,则直接取得对象
        if (*(field_pos) == BB_ARRAY_APPEND_DEFAULT_CHAR) {//对于没有初始化值的情况，创建一个空值，???测试两种返回;
            field_pos = &BB_null_obj;
        }
        (*data_size) = BB_GET_OBJ_SIZE((BB_OBJ *)field_pos);
        (*data)=(BYTE *)field_pos;
    }else if (field_type == BB_TABLE_FIELD_TYPE_MALLOC) {//ly malloc2; ly task; 如果STRING FIELD则只取得ARRAY_APPEND_DEFAULT_CHAR（0xff）前面的内容ly: string
        //
        if (*(field_pos) == BB_ARRAY_APPEND_DEFAULT_CHAR) {//对于没有初始化值的情况，创建一个空值，???测试两种返回;
            field_pos = &BB_null_obj;
            (*data_size) = BB_GET_OBJ_SIZE((BB_OBJ *)field_pos);
            (*data)=(BYTE *)field_pos;
        }else{
            field_size = BB_GET_OBJ_SIZE(field_pos);
            //读取对象
            BB_OBJ *malloc_obj;
            BYTE type;
            if (!BB_GET_MALLOC_ADDRESS(field_pos, (void **)&malloc_obj, &type)) {
                printf("%s \n","BB_READ_TABLE_BY_NUM_STRU()->BB_TABLE_FIELD_TYPE_MALLOC: BB_NEW_MALLOC() error!");
                return false;//to_add
            }
            //结果对象
            (*data_size) = BB_GET_OBJ_SIZE(malloc_obj);
            (*data)=(BYTE *)malloc_obj;
        }
    }else {
        //普通则需创建对象
        if (field_type == BB_TABLE_FIELD_TYPE_NUMBER) {
            (*data_size) = field_size;
            (*data)=field_pos;
        }else if (field_type == BB_TABLE_FIELD_TYPE_STRING) {//如果STRING FIELD则只取得ARRAY_APPEND_DEFAULT_CHAR（0xff）前面的内容ly: string
            field_size = get_table_field_string(field_pos, field_size);
            (*data_size) = field_size;
            (*data)=field_pos;
        }
        else {
            (*data_size) = field_size;
            (*data)=field_pos;
        }
        
    }
    return true;
}

bool BB_GET_TABLE_ITEMS_ID_DATA(BB_OBJ *obj, BB_SIZE_T row,BYTE col,BYTE **data,BB_SIZE_T *data_size){//malloc2.1
    BB_OBJ_STRU obj_stru = BB_GET_OBJ_STRU(obj);
    
    if (obj_stru.LoadError) {
        printf("%s\n", "Error:BB_READ_TABLE_BY_NUM() load table error!");
        return false;
    }
    
    return BB_GET_TABLE_ITEMS_ID_DATA_STRU(&obj_stru, row,col,data,data_size);
}

bool BB_GET_TABLE_ITEMS_ID_DATA_STRU(BB_OBJ_STRU *obj_stru, BB_SIZE_T row,BYTE col,BYTE **data,BB_SIZE_T *data_size){
    BYTE *item;
    BB_SIZE_T item_size=0,field_size;
    //BYTE null_obj=BB_NULL;
    UInt64 field_type = 0;
    
    //必须是数组
    if (obj_stru->Type!=BB_TABLE){
        printf("%s\n","Error:BB_READ_TABLE_BY_NUM() input must be BB_TABLE");
        return false;
    }
    //根据node索引查找相关记录，如果没有则插入
    if (!BB_GET_ARRAY_REC_BY_NUM_STRU(obj_stru,row,&item,&item_size))
        return false;
    
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU(obj_stru->CellObj);
    if (obj_stru->Type!=BB_TABLE && obj_stru->Type!=BB_ARRAY ){
        printf("%s\n","Error: BB_READ_TABLE_BY_NUM() cell_type must be ARRAY or TABLE");
        return false;
    }
    
    //取得对象描述数组
    BYTE *field_pos;
    if (!BB_GET_TABLE_ITEMS_DATA_STRU(obj_stru, row, col, &field_pos, &field_size)) {//reading操作
        printf("%s num=%zu,src_id=%d \n", "BB_GET_TABLE_ITEMS_ID_DATA_STRU()->BB_GET_TABLE_ITEMS_DATA_STRU: can't read db field !", row, col);
        return false;//to_add
    }
    //根据不同的针对table判断field_type
    if (cell_obj_stru.Type == BB_TABLE)
        BB_GET_TABLE_ITEM_VALUE_STRU(&cell_obj_stru, col, BB_TABLE_DEFAULT_TYPE_FIELD, &field_type); //ly mac
    else
        field_type = BB_TABLE_FIELD_TYPE_BINARY;
        
    //读出操作
    if (field_type == BB_TABLE_FIELD_TYPE_OBJECT) {
        //对象值返回,则直接取得对象
        if (*(field_pos) == BB_ARRAY_APPEND_DEFAULT_CHAR) {//对于没有初始化值的情况，创建一个空值，???测试两种返回;
            field_pos = &BB_null_obj;
        }
        (*data_size) = BB_GET_OBJ_DATA_LEN((BB_OBJ *)field_pos);
        (*data)=(BYTE *)BB_GET_OBJ_DATA((BB_OBJ *)field_pos);
    }else if (field_type == BB_TABLE_FIELD_TYPE_MALLOC) {//ly malloc2; ly task; 如果STRING FIELD则只取得ARRAY_APPEND_DEFAULT_CHAR（0xff）前面的内容ly: string
        //
        if (*(field_pos) == BB_ARRAY_APPEND_DEFAULT_CHAR) {//对于没有初始化值的情况，创建一个空值，???测试两种返回;
            field_pos = &BB_null_obj;
            (*data_size) = BB_GET_OBJ_DATA_LEN((BB_OBJ *)field_pos);
            (*data)=(BYTE *)BB_GET_OBJ_DATA(field_pos);
        }else{
            field_size = BB_GET_OBJ_SIZE(field_pos);
            //读取对象
            BB_OBJ *malloc_obj;
            BYTE type;
            if (!BB_GET_MALLOC_ADDRESS(field_pos, (void **)&malloc_obj, &type)) {
                printf("%s \n","BB_READ_TABLE_BY_NUM_STRU()->BB_TABLE_FIELD_TYPE_MALLOC: BB_NEW_MALLOC() error!");
                return false;//to_add
            }
            //结果对象
            (*data_size) = BB_GET_OBJ_DATA_LEN(malloc_obj);
            (*data)=(BYTE *)BB_GET_OBJ_DATA(malloc_obj);
        }
    }else {
        //普通则需创建对象
        if (field_type == BB_TABLE_FIELD_TYPE_NUMBER) {
            /*uint64 num = BB_ScanUINTWidth(field_pos, field_size);
            BB_UINT_STRU new_obj_uint=BB_NEW_CELL_UINT(num,BB_UINT64);
            new_obj = (BB_OBJ *)&new_obj_uint;
            memcpy((void *)(buf + (*bufoffset)), (const void*)new_obj, BB_GET_OBJ_SIZE(new_obj));
            (*bufoffset) += BB_GET_OBJ_SIZE(new_obj);*/
            (*data_size) = field_size;
            (*data)=field_pos;
        }else if (field_type == BB_TABLE_FIELD_TYPE_STRING) {//如果STRING FIELD则只取得ARRAY_APPEND_DEFAULT_CHAR（0xff）前面的内容ly: string
            field_size = get_table_field_string(field_pos, field_size);
            /*new_obj = BB_NEW_OBJ_STRING(field_pos, field_size, NULL);
            memcpy((void *)(buf + (*bufoffset)), (const void*)new_obj, BB_GET_OBJ_SIZE(new_obj));
            (*bufoffset) += BB_GET_OBJ_SIZE(new_obj);
            if(new_obj!=NULL)
            free(new_obj);*/
            (*data_size) = field_size;
            (*data)=field_pos;
        }
        else {
            /*new_obj = BB_NEW_BYTES_ALL(field_pos, field_size, NULL);//自适应BYTE,BYTE_EXT,BB_ARRAY
            memcpy((void *)(buf + (*bufoffset)), (const void*)new_obj, BB_GET_OBJ_SIZE(new_obj));
            (*bufoffset) += BB_GET_OBJ_SIZE(new_obj);
            if (new_obj!=NULL)
            free(new_obj);*/
            (*data_size) = field_size;
            (*data)=field_pos;
        }
        
    }
    return true;
}

bool BB_READ_TABLE_BY_INDEX(BB_OBJ*obj, BYTE* buf, BB_SIZE_T *bufoffset,  BYTE *index_str, BYTE index_str_size, BYTE *bitmap_str, BYTE bitmap_size,BYTE method,bool add_index_header){
    BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
    
    //BB_SIZE_T item_size=0;
    int num;
    
    //必须是数组
    if (obj_stru.Type!=BB_ARRAY  && obj_stru.Type!=BB_TABLE){
        printf("%s\n","Error:BB_READ_TABLE_BY_INDEX() input must be BB_ARRAY/BB_TABLE");
        return false;
    }
    
    //根据node索引查找相关记录，如果没有则插入
    if ((num=BB_GET_ARRAY_NUM_BY_INDEX(obj,index_str,index_str_size))<0){
        printf("%s\n","Error:BB_READ_TABLE_BY_INDEX() can't find record!");
        return false;
    }
    
    //增加header_index处理
    if (add_index_header){
        //取得准确的index,index_size,从库中取得ly index_size;
        BYTE *index_item_str;
        BB_SIZE_T index_item_size;
        BB_GET_ARRAY_REC_BY_NUM(obj_stru.IndexObj,num,&index_item_str,&index_item_size);
        if ((method & BB_TABLE_REC_WR_BITIDS) == BB_TABLE_REC_WR_BITIDS) {
            BB_OBJ *index_obj = BB_NEW_BYTES_ALL(index_item_str, index_item_size, NULL);
            index_item_size= BB_GET_OBJ_SIZE(index_obj);
            memcpy(buf, (const void *)index_obj, index_item_size);
            if (index_obj!=NULL)
                free(index_obj);
        }
        else 
            memcpy(buf, index_item_str, index_item_size);
        (*bufoffset)+=index_item_size;	
    }
    
    return BB_READ_TABLE_BY_NUM(obj,num,buf,bufoffset,bitmap_str,bitmap_size,method);
}

/*static bool get_table_obj_fields(BB_OBJ_STRU obj_stru,BYTE **obj_fields, BB_SIZE_T &obj_fields_num, BYTE &obj_fields_size){
	BB_OBJ_STRU cell_obj_stru = BB_GET_OBJ_STRU(obj_stru.CellObj);
	if (cell_obj_stru.AddInfoObj!=NULL){
 (*obj_fields)=BB_GET_CELL_DATA(cell_obj_stru.AddInfoObj);
 obj_fields_num=BB_GET_CELL_DATA_LEN(cell_obj_stru.AddInfoObj);
 obj_fields_size=1;
 return true;
	}else
 return false;
 }*/
/*static void set_table_obj_field(BYTE type, BYTE *buf, BB_SIZE_T buf_size, BB_SIZE_T field_size, BB_OBJ **new_obj){
	if (buf_size < BB_0DIM_BYTES_MAX_LEN){//不能==64
 if (buf_size >(field_size-BB_OBJ_STX_LEN))//超出字段范围则截断
 buf_size = field_size-BB_OBJ_STX_LEN;
 (*new_obj)=BB_NEW_CELL(BB_BYTES,(const void*)(buf), buf_size,NULL);//暂时用BB_BYTES今后可以直接从外部设定对象内容
	}else{
 if (buf_size >(field_size-BB_OBJ_STX_EXT_LEN))//超出字段范围则截断
 buf_size = field_size-BB_OBJ_STX_EXT_LEN;
 (*new_obj)=BB_NEW_CELL(BB_BYTES_EXT,(const void*)(buf), buf_size,NULL);//暂时用BB_BYTES今后可以直接从外部设定对象内容
	}
 }*/

static BB_SIZE_T get_table_field_string(BYTE *src_field_pos, BB_SIZE_T src_field_size) {
    BB_SIZE_T i;
    for (i = 0; i < src_field_size; i++) {
        if (src_field_pos[i]==0 ||src_field_pos[i] == BB_ARRAY_APPEND_DEFAULT_CHAR)
            return i;
    }
    return src_field_size;
}

/*static void add_bitmap_start_end(BB_BITMAP_TYPE &bitmap,BYTE bitmap_size){
	//起始结束bitmap模式
	bool begin=false,end=false;
	for (BYTE i = 0; i < (bitmap_size *8); i++){
 if ((((bitmap << (i + 1)) | (bitmap >> ((bitmap_size*8-1) - i))) & 1) > 0){
 if (!begin){//查询起始
 begin=true;
 }
 if (begin && !end){//查询结束
 end=true;
 break;
 }
 }
 BB_BITMAP_TYPE tmp=1<<i; 
 bitmap=bitmap|tmp;//填写其中空位。
	}
 }*/
/************************查询对象创建操作**************************/
BB_SEARCH_STRU BB_NEW_SEARCH_CELL(BYTE search_type, const void *data, BYTE data_len){
    BB_SEARCH_STRU sobj;
    
    //set header db_type
    sobj.Header=search_type;
    
    //set obj data_len
    sobj.Header=(sobj.Header)|((data_len)&(BB_SEARCH_LEN_MASK));
    
    //copy data to data if has;
    if (data!=NULL && data_len!=0)
        memcpy((void*)sobj.data,data,data_len);
    
    return sobj;
}
BYTE BB_SEARCH_CELL_SIZE(BB_OBJ *sobj){
    return _get_mask_value(sobj,BB_SEARCH_LEN_MASK)+1;
}


