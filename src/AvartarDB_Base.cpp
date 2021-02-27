 #include "stdafx.h"
#include "stdio.h"
#include <fstream>
#include <sstream>
#include "STRING_Helper.h"
#include "AvartarDB_Base.h"
#include "AvartarSortObj.h"
#include "ParseXml.h"
#include "string.h"
#include "Linux_Win.h"
#include <iostream>
//#include <WizBase.h>
#include "ByteBase.h"
#include "ParseBase.h"
//#include "FileQueue.h"
#include "math.h"
#include <time.h>
//#define AvartarDB_Base_SaveExt_CreateDir

BB_SIZE_T AvartarDB_Base::GetTableRecBySortKey(BB_OBJ *table_obj, BYTE field_id, BYTE * field_value, BB_SIZE_T field_value_len) {
	//ly sort
	//size_t num;
	return this->SortObj->SeekItem(table_obj, field_id, field_value, field_value_len);
}

BB_SIZE_T AvartarDB_Base::GetTableRecBySortKey(string &tableindex, BYTE field_id, const char * field_value, BB_SIZE_T field_value_len) {
	BB_SIZE_T num;
	BB_OBJ *obj;
	BB_SIZE_T obj_size;
	if (!this->GetTableObject(tableindex, &obj, obj_size)) {
		this->SHOW_ERROR((string)"GetTableRecBySortKey(),Load table error!");
		return BB_SIZE_T_ERROR;
	}
	if ((num = this->SortObj->SeekItem(obj, field_id, (BYTE *)field_value, field_value_len))!=BB_SIZE_T_ERROR)
		return num;
	else
		return BB_SIZE_T_ERROR;
}

string AvartarDB_Base::GetTableItemString(BB_OBJ_STRU *table_obj_stru,BB_SIZE_T rec,BYTE col){
	//取得item;
	BYTE *item;
	BB_SIZE_T item_size;
	if (BB_GET_TABLE_ITEMS_DATA_STRU(table_obj_stru,rec,col,&item,&item_size))
		return string((const char *)item,item_size);
	else
		return "";
}
string AvartarDB_Base::GetTableItemObj_String(BB_OBJ_STRU *table_obj_stru,BB_SIZE_T rec,BYTE col){
	//取得item;
	BYTE *item;
	BB_SIZE_T item_size;
	if (BB_GET_TABLE_ITEMS_DATA_STRU(table_obj_stru,rec,col,&item,&item_size))
		return string((const char *)item,item_size);
	else
		return "";
}
void *AvartarDB_Base::GetTableItemPoint(BB_OBJ_STRU *table_obj_stru,BB_SIZE_T rec,BYTE col){
	//取得item;
	BYTE *item;
	BB_SIZE_T item_size;
	if (BB_GET_TABLE_ITEMS_DATA_STRU(table_obj_stru,rec,col,&item,&item_size))
		return (void *)item;
	else
		return NULL;
}
UInt64 AvartarDB_Base::GetTableItemUINT(BB_OBJ_STRU *table_obj_stru,BB_SIZE_T rec,BYTE col){
	//取得item;
	BYTE *item;
	BB_SIZE_T item_size;
	
	if (BB_GET_TABLE_ITEMS_DATA_STRU(table_obj_stru,rec,col,&item,&item_size)){//取得对象		
		string tmp=string((const char *)item,item_size);
		return STRING_Helper::scanUINTStr(tmp);
	}
	return 0xFFFFFFFFFFFFFFFF;
}

bool AvartarDB_Base::SetTableItemUINT(BB_OBJ_STRU *table_obj_stru, BB_SIZE_T rec, BYTE col, UInt64 num) {
	//取得item;
	BYTE *item;
	BB_SIZE_T item_size;

	if (BB_GET_TABLE_ITEMS_DATA_STRU(table_obj_stru, rec, col, &item, &item_size)) {//取得对象		
		string tmp=STRING_Helper::putUINTToStr(num, item_size * 8);
		BB_PUT_TABLE_ITEMS_DATA_STRU(table_obj_stru, rec, col,(BYTE *)tmp.data(),tmp.size());
	}
	return false;
}

int AvartarDB_Base::GetTableCellName2ID(BB_OBJ_STRU &cell_obj_stru,const char *field_str){//ly cell_name;ly task此处可以提速
    BYTE *item;
    BB_SIZE_T item_size;
    
    for(BYTE i=0; i< cell_obj_stru.Num; i++){
        if (!BB_GET_TABLE_FIELD_NAME_STRU(&cell_obj_stru,i,&item,&item_size))
            return -1;
        BYTE t=strlen(field_str);
        if ((strncmp(field_str,(const char *)item,t>=item_size ? t : item_size)==0))
            return i;
    }
    return -1;
}

string AvartarDB_Base::ConvertBitmap_ids2Bitmap(string &bitmap_str_ids){
    BB_BITMAP_TYPE bitmap=0, bitmap_1=1;
    bitmap_1= bitmap_1 <<(sizeof(BB_BITMAP_TYPE)*8-1);
    BYTE bit_width=sizeof(BB_BITMAP_TYPE)*8;
    for (BB_SIZE_T n=0; n < bitmap_str_ids.size(); n++){//IDS改为32位Bitmap
        BYTE pos=(BYTE)(bitmap_str_ids[n]);
        if (pos < bit_width)
            bitmap|=(bitmap_1 >> pos); //位移指定位置
    }
    return STRING_Helper::putUINTToStr(bitmap,bit_width);
}


UInt64 AvartarDB_Base::GetTableItemObj_UINT(BB_OBJ_STRU *table_obj_stru,BB_SIZE_T rec,BYTE col){
	//取得item;
	BYTE *item;
	BB_SIZE_T item_size;
	
	if (BB_GET_TABLE_ITEMS_DATA_STRU(table_obj_stru,rec,col,&item,&item_size)){//取得对象		
		if (BB_IS_UINT(item)){
			return BB_GET_UINT_VALUE(item);
		}
	}
	return 0xFFFFFFFFFFFFFFFF;
}

bool AvartarDB_Base::GetAddInfoUINT(string &tableindex, const char *key, UInt64 &num){
	//ly sort
	BB_OBJ *item;
	BB_SIZE_T item_size;
	if (GetAddInfoObj(tableindex,key,&item,&item_size)){//取得对象
		if (BB_IS_UINT(item)){
			num = BB_GET_UINT_VALUE(item);
			return true;
		}
	}
	return false;
}

bool AvartarDB_Base::GetAddInfoUINT(BB_OBJ *add_info_obj, const char *key, UInt64 &num) {
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
		this->SHOW_ERROR("GetAddInfoUINT(key=" + (string)key + ") is not UINT!");
		return false;
	}
}

bool AvartarDB_Base::SetAddInfoUINT(string &tableindex, const char *key, UInt64 num){
	BB_OBJ *item;
	BB_SIZE_T item_size;
	if (GetAddInfoObj(tableindex,key,&item,&item_size)){//取得对象		
		if (BB_IS_UINT(item)){
			BB_SET_UINT_VALUE(item,num);
			return true;
		}
	}
	return false;
}

bool AvartarDB_Base::SetAddInfoUINT(BB_OBJ *add_info_obj, const char *key, UInt64 num) {
	BB_OBJ *item;
	BB_SIZE_T item_size;
	if (GetAddInfoObj(add_info_obj, key, &item, &item_size)) {//取得对象		
		if (BB_IS_UINT(item)) {
			BB_SET_UINT_VALUE(item, num);
			return true;
		}
	}
	return false;
}

bool AvartarDB_Base::GetAddInfoObj(BB_OBJ *add_info_obj, const char *key, BB_OBJ **item, BB_SIZE_T *item_size) {
	int rec;
	if ((rec = BB_GET_ARRAY_NUM_BY_INDEX(add_info_obj, (BYTE *)key, strlen(key))) > 0) {
		if (BB_GET_ARRAY_OBJ_BY_NUM(add_info_obj, rec, item, item_size))
			return true;
		else
			return false;
	}
	else
		return false;
}

bool AvartarDB_Base::GetAddInfoString(BB_OBJ *add_info_obj, const char *key,string &value_str) {
    if (add_info_obj != NULL) {
        BB_OBJ *item_obj;
        BB_SIZE_T item_size;
        if (!GetAddInfoObj(add_info_obj, key,&item_obj,&item_size)){
            return false;
        }else if (!BB_IS_OBJ_STRING(item_obj) && (BB_GET_OBJ_TYPE(item_obj) == BB_ARRAY || BB_GET_OBJ_TYPE(item_obj) == BB_TABLE) )
            value_str=string ((const char *)item_obj,item_size);
        else if (BB_IS_UINT(item_obj))
            value_str=STRING_Helper::uToStr(BB_GET_UINT_VALUE(item_obj));
        else
            value_str=string ((const char *)BB_GET_OBJ_DATA(item_obj),BB_GET_OBJ_DATA_LEN(item_obj));
    }
    else
        return false;
    return true;
}

bool AvartarDB_Base::GetAddInfoObj(string &tableindex, const char *key, BB_OBJ **item, BB_SIZE_T *item_size){
	//取得AddInfo对象 //ly sort
	BB_OBJ *table_obj;
	size_t table_obj_size;
	if (!this->GetTableObject(tableindex,&table_obj,table_obj_size)){
		this->SHOW_ERROR((string)"GetAddInfo() can't table obj ("+tableindex+")!");
		return false;
	}
	
	BB_OBJ_STRU table_obj_stru=BB_GET_OBJ_STRU(table_obj);
	if (table_obj_stru.AddInfoObj!=NULL) {
		return this->GetAddInfoObj(table_obj_stru.AddInfoObj,key,item,item_size);
	}else
		return false;
}

string AvartarDB_Base::GetAddInfo(string &tableindex, string key){
	string r_str;

	//取得AddInfo对象
	BB_OBJ *table_obj;
	size_t table_obj_size;
	if (!this->GetTableObject(tableindex,&table_obj,table_obj_size)){
		this->SHOW_ERROR((string)"GetAddInfo() can't table obj ("+tableindex+")!");
		return"";
	}
	BB_OBJ_STRU table_obj_stru=BB_GET_OBJ_STRU(table_obj);
	if (table_obj_stru.AddInfoObj==NULL)
		return "";
	else
		return this->GetAddInfo(table_obj_stru.AddInfoObj,key);
}

string AvartarDB_Base::GetAddInfo(BB_OBJ *add_info_obj, string key){
	string r_str;

	//取得对象KeyValue
	BB_OBJ *item;
	BB_SIZE_T item_size;
	if (!BB_GET_ARRAY_REC_BY_INDEX(add_info_obj,(BYTE *)key.data(), key.size(),&item, &item_size)){
		//this->SHOW_ERROR((string)"GetAddInfo() can't find key("+key+")!");
		return "";
	}
	
	//回传结果
	r_str.append((const char *)item,item_size);//取得的也是对象string需提取内容
	return r_str;
}

bool AvartarDB_Base::GetArrayRecString(BB_OBJ_STRU &obj_stru, BB_SIZE_T num,string &r_str){

	//取得对象KeyValue
	BB_OBJ *item;
	BB_SIZE_T item_size;
	if (!BB_GET_ARRAY_REC_BY_NUM_STRU(&obj_stru,num,&item, &item_size)){
		return false;
	}

	r_str.append((char *)item,item_size);
	return true;
}

bool AvartarDB_Base::GetArrayRecsVector(BB_OBJ_STRU &obj_stru, vector<string> &vec){ //ly_ids
    //取得对象KeyValue
    if (!BB_IS_OBJ_ARRAY_STRU(&obj_stru)){
        return false;
    }
    BB_OBJ *bytes_obj_p; //form BB_BYTES_ARRAY:
    BB_SIZE_T i=0;
    BB_OBJ *item_obj;
    BB_SIZE_T item_obj_size;
    
    //非定长数据
    bytes_obj_p=obj_stru.Data; //取得元素宽度;
    for(;bytes_obj_p<(obj_stru.Data + obj_stru.DataLen); bytes_obj_p+=item_obj_size,i++){//ly max; ly obj
        item_obj=(BB_OBJ *)bytes_obj_p;
        item_obj_size=BB_GET_OBJ_SIZE(bytes_obj_p);
        if (!BB_IS_OBJ_STRING(item_obj) && (BB_GET_OBJ_TYPE(item_obj) == BB_ARRAY || BB_GET_OBJ_TYPE(item_obj) == BB_TABLE)){
            //多层数组对象转换
            string tmp((const char *)item_obj,BB_GET_OBJ_SIZE(item_obj));
            vec.push_back(tmp);
        }
        else {
            //BYTES对象转换,包括ARRAY_OBJ_STRING
            string tmp((const char *)BB_GET_OBJ_DATA(item_obj),BB_GET_OBJ_DATA_LEN(item_obj));
            vec.push_back(tmp);
        }
    }
    return true;
}
bool AvartarDB_Base::GetArrayRecsAddStr(BB_OBJ_STRU &obj_stru, string &data){ //ly_ids
    //取得对象KeyValue
    if (!BB_IS_OBJ_ARRAY_STRU(&obj_stru)){
        return false;
    }
    BB_OBJ *bytes_obj_p; //form BB_BYTES_ARRAY:
    BB_SIZE_T i=0;
    BB_OBJ *item_obj;
    BB_SIZE_T item_obj_size;
    data="";
    
    //非定长数据
    bytes_obj_p=obj_stru.Data; //取得元素宽度;
    for(;bytes_obj_p<(obj_stru.Data + obj_stru.DataLen); bytes_obj_p+=item_obj_size,i++){//ly max; ly obj
        item_obj=(BB_OBJ *)bytes_obj_p;
        item_obj_size=BB_GET_OBJ_SIZE(bytes_obj_p);
        if (!BB_IS_OBJ_STRING(item_obj) && (BB_GET_OBJ_TYPE(item_obj) == BB_ARRAY || BB_GET_OBJ_TYPE(item_obj) == BB_TABLE)){
            //多层数组对象转换
            string tmp((const char *)item_obj,BB_GET_OBJ_SIZE(item_obj));
            data+=tmp;
        }
        else {
            //BYTES对象转换,包括ARRAY_OBJ_STRING
            string tmp((const char *)BB_GET_OBJ_DATA(item_obj),BB_GET_OBJ_DATA_LEN(item_obj));
            data+=tmp;
        }
    }
    return true;
}


bool AvartarDB_Base::GetArrayRecUINT(BB_OBJ_STRU &obj_stru, BB_SIZE_T num, UInt64 &ret){
	BB_OBJ *item;
	BB_SIZE_T item_size;

	if (BB_GET_ARRAY_REC_BY_NUM_STRU(&obj_stru,num,&item, &item_size)){//取得对象		
		if (BB_IS_UINT(item)){
			ret = BB_GET_UINT_VALUE(item);
			return true;
		}
	}
	return false;
}

bool AvartarDB_Base::_GetConvertToRequertTable(string &src_table_path,BB_OBJ *convert_table_obj,string &bitmapstr,BB_OBJ **request_table_obj){
	//创建返回请求数据表
	BB_OBJ_STRU convert_table_obj_stru=BB_GET_OBJ_STRU(convert_table_obj);

	//ly test1
	//cout << "bitmapstr:" << bitmapstr << ";";
	//取得表格对象地址
	BB_OBJ *src_table_obj;
	BB_SIZE_T src_table_obj_size;
	if (!this->GetTableObject(src_table_path,&src_table_obj,src_table_obj_size)){
		this->SHOW_ERROR((string)"_GetConvertToRequertTable(),Load table error!");
		return false;
	}
	//BB_OBJ_STRU src_table_obj_stru=BB_GET_OBJ_STRU(src_table_obj);

	//生成数组型Index
	BB_OBJ *index_array=BB_NEW_ARRAY_CELL(BB_UINT8,BB_UINT8,BB_NULL,convert_table_obj_stru.Num,NULL,0,NULL);
	(*request_table_obj)= BB_NEW_ARRAY_INDEX_CELL(BB_UINT8,index_array,BB_NULL,convert_table_obj_stru.Num,NULL,0,NULL);//生成带上述索引的空数组
	if (index_array)
		free(index_array);
	BB_OBJ_STRU request_table_obj_stru=BB_GET_OBJ_STRU(*request_table_obj);

	//填写索引数组
	BB_SIZE_T  bitmap_size=bitmapstr.size();
	BB_BITMAP_TYPE bitmap=(bitmap_size==0) ? 0 : (BB_BITMAP_TYPE)STRING_Helper::scanUINTStr(bitmapstr);
	
	bitmap = sizeof(BB_BITMAP_TYPE) > bitmap_size ? bitmap <<((sizeof(BB_BITMAP_TYPE)-bitmap_size)*8) : bitmap;  //如果不是64位则相应错位，例如32位
	//ly test1
	//cout << "bitmap:" << bitmap << ";bitmap_size:" << bitmap_size << ";";
	if (convert_table_obj_stru.Num!=(bitmap_size*8)){
		this->SHOW_ERROR((string)"_GetConvertToRequertTable()error! cell table num("+STRING_Helper::iToStr(convert_table_obj_stru.Num)+") != bitmap_size("+STRING_Helper::iToStr(bitmap_size*8)+") !");
		return false;
	}

	// i = 0,
	
	//UInt64 field_id;
	BYTE *field_id_p;
	BYTE tmp_i = 0;
	BB_SIZE_T field_id_size;
	for(BYTE i=0; i<convert_table_obj_stru.Num; i++){//循环填写数组内容和索引
		if ( (bitmap_size==0) || ((((bitmap << (i + 1)) | (bitmap >> ((bitmap_size *8) - i))) & 1) > 0)){//转换bitmap to ids
			//if (!BB_GET_TABLE_ITEM_VALUE_STRU(&convert_table_obj_stru,i,BB_TABLE_DEFAULT_SYNCINDEX_FIELD,&field_id)){//取得目标字段id信息
			//	printf("%s %d\n","_GetConvertToRequertTable()error! sync_index_field=",BB_TABLE_DEFAULT_SYNCINDEX_FIELD);
			//	return false;//to_add
			//}
			
			if (!BB_GET_TABLE_ITEMS_DATA_STRU(&convert_table_obj_stru, i, BB_TABLE_DEFAULT_SYNCINDEX_FIELD, &field_id_p,&field_id_size)) {//取得目标字段id信息
				printf("%s %d\n","_GetConvertToRequertTable()error! sync_index_field=",BB_TABLE_DEFAULT_SYNCINDEX_FIELD);
				return false;//to_add
			}
			//BYTE field_id_tmp = field_id;
			string test((const char*)field_id_p, field_id_size);
			//ly test1
			//cout << "field_id:" << STRING_Helper::HBCStrToHBCStr(test,"char","hex",true) << ";i:" << (int)tmp_i << "_" << tmp_i<< ";i_size:" << sizeof(tmp_i) << ";";
			BB_APPEND_ARRAY_REC_BY_INDEX_STRU(&request_table_obj_stru,(BYTE *)&tmp_i,sizeof(tmp_i),field_id_p, field_id_size);//以目标字段为索引，以源字段为内容。
			//BB_APPEND_ARRAY_REC_BY_INDEX_STRU(&request_table_obj_stru, (BYTE *)&i, sizeof(i), (BYTE *)&field_id, sizeof(field_id));//以目标字段为索引，以源字段为内容。
			tmp_i++;
		}
	}
	
	//ly test1
	//string test2((const char*)(*request_table_obj), BB_GET_OBJ_SIZE(*request_table_obj));
	//cout << endl;
	//cout << STRING_Helper::HBCStrToHBCStr(test2, "char", "hex", true) << endl;
	//cout << endl;
	//bitmap.clear()//替换为新的bitmap，其中存放convert_cell_ids
	//bitmap.append()
	//if (request_table_obj)
	//	free(request_table_obj);
	return true;
}

bool AvartarDB_Base::CreateDir(string path){//应测试覆盖情况
	if (LW_mkdir(path.c_str())<0)
		return false;
	else
		return true;
}

string AvartarDB_Base::JennicDataParseValue(string fmt_str, string data_str){
	//取得format值
	const char *format_offset_reportflag=fmt_str.c_str();
	UInt32 format=(*format_offset_reportflag)*0x01000000+(*(format_offset_reportflag+1))*0x00010000+(*(format_offset_reportflag+2))*0x00000100+(*(format_offset_reportflag+3))*0x00000001;
	//UInt16 format16=(*format_offset_reportflag)*0x0100+(*(format_offset_reportflag+1))*0x0001;
	//cout <<"[" <<(int) rfnumber <<"]: format(32): " << format<< ";(16):"<< format16 ;
	if (format==0xffffffff)
		return "";

	//解析处理
	float sign = 1;
    UInt64 data = 0;
	byte *rawarray=(byte *)data_str.c_str();
	UInt16 offset=0;
    byte b1 = (byte)(format / 0x01000000);	
	byte b2 = (byte)((format % 0x01000000) / 0x010000);
    byte b3 = (byte)((format % 0x010000) / 0x0100);
    byte b4 = (byte)(format % 0x0100);
	//b1取得长度
    if (b1 / 128 == 0){
        //byte len=1;
		return "";
    }else{
		//取得长度
        byte len = (byte)pow((double)2, (double)(b1 % 32) / 4);   
		//取得data和正负
        for (BB_SIZE_T i = 0; i < len; i++){
            if (i == 0)
            {
                if ((b1 % 64) / 32 == 0){
                    data += (UInt64)(rawarray[i + offset] * pow((double)256,(double)(pow((double)2, (double)(b1 % 32) / 4) - (1 + i))));
                    sign = 1;
                }else{
                    data += (UInt64)((rawarray[i + offset] % 128) * pow((double)256, (pow((double)2, (double)(b1 % 32) / 4) - (1 + i))));
                    if (rawarray[i + offset] / 128 == 0)
                        sign = 1;
                    else
                        sign = -1;
                }
            }
            else
                data += (UInt64)(rawarray[i+ offset] * pow((double)256, (pow((double)2, (double)(b1 % 32) / 4) - (1 + i))));
        }          
        offset += len;
		//取得各类字串
        if ((b1 % 128) / 64 == 1){
            if ((b2 % 8) / 2 == 0){         
                byte indi = (byte)(b4 % 8);
                if ((indi < 4)||(indi==5)){
                    byte io = 0;
                    if (indi == 1)
                        io = (byte)(data % 256);
                    else if (indi == 5)
                        io = (byte)data;
                    else
                        io = (byte)((data % 16777216) / 65536);
                    return (STRING_Helper::iToStr(io / 128) + STRING_Helper::iToStr((io % 128) / 64) + STRING_Helper::iToStr((io % 64) / 32) + STRING_Helper::iToStr((io % 32) / 16) + STRING_Helper::iToStr((io % 16) / 8) + STRING_Helper::iToStr((io % 8) / 4) + STRING_Helper::iToStr((io % 4) / 2) +STRING_Helper::iToStr (io % 2)); //reserved
                }
                else if (indi == 4){
                    if (data > 0)
                        return "on";
                    else
                        return "off";
                } else{
                    if (data > 0)
                        return "motion";
                    else
                        return "nobody";
                }
            }else if ((b2 % 8) / 2 == 1){
                int i = 0;
                string str = "";
                byte array[8];  //make up
                //UInt32 length =0;
                STRING_Helper::putUINT64(array, 0, data);
                if ((b4 % 4) / 2 == 1){
                    str += "Facing ";
                    if ((array[1] & 0x40) == 1){
                        if ((array[1] & 0x01) == 1)
                            str += "down";
                        else
                            str += "up";
                    }else{
                        if ((array[1] & 0x06) == 0)
                            str += "back";
                        else if ((array[1] & 0x06) == 0x02)
                            str += "front";
                        else if ((array[1] & 0x06) == 0x04)
                            str += "right";
                        else if ((array[1] & 0x06) == 0x06)
                            str += "left";
                    }
                    str += ";";
                    i += 1;
                }
                return str;
            }else if ((b2 % 8) / 2 == 2){
                UInt16 temp = (UInt16)(data % 0x10000);
                UInt16 humi = (UInt16)(data / 0x10000);
                if (temp > 4000)                     // if temperature is positive
                    temp = (UInt16)(temp - 4000);        // calculate temperature
                else                             // else (if temperature is negative)
                    temp = (UInt16)(4000 - temp);        // calculate temperature

                // Calculating humidity
                humi = (UInt16)(humi * 405 / 100 - (humi * humi * 28 / 100000) - 400);         // calculate humidity
                return STRING_Helper::iToStr(temp / 100) + "." + STRING_Helper::iToStr(temp % 100) + "C," + STRING_Helper::iToStr(humi / 100) + "." + STRING_Helper::iToStr(humi % 100) + "%";
            }
            else
                return "";
        }
        else
        {
            if ((b3 * 256 + b4) == 0)
                return STRING_Helper::fToStr(sign * data * pow((float)0.1, b1 % 4 * 256 + b2 / 16));
            else if ((b3 * 256 + b4) == 1)
                return STRING_Helper::fToStr(sign * data * pow((float)0.1, b1 % 4 * 256 + b2 / 16)) + " ℃";
            else if ((b3 * 256 + b4) == 2)
                return STRING_Helper::fToStr(sign * data * pow((float)0.1, b1 % 4 * 256 + b2 / 16)) + " ℉";
            else if ((b3 * 256 + b4) == 3)
                return STRING_Helper::fToStr(sign * data * pow((float)0.1, b1 % 4 * 256 + b2 / 16)) + " ％";
            else if ((b3 * 256 + b4) == 4)
                return STRING_Helper::fToStr(sign * data * pow((float)0.1, b1 % 4 * 256 + b2 / 16)) + " ‰";
            else if ((b3 * 256 + b4) == 5)
                return STRING_Helper::fToStr(sign * data * pow((float)0.1, b1 % 4 * 256 + b2 / 16)) + " Kg";
            else if ((b3 * 256 + b4) == 6)
                return STRING_Helper::fToStr(sign * data * pow((float)0.1, b1 % 4 * 256 + b2 / 16)) + " Lb";
            else if ((b3 * 256 + b4) == 7)
            {
                if (data == 0)
                    return ("Off");
                else if (data == 1)
                    return ("On");
                else
                    return "something else";
            }
            else if ((b3 * 256 + b4) == 8)
                return STRING_Helper::fToStr(sign * data * pow((float)0.1, b1 % 4 * 256 + b2 / 16)) + " V";
            else
                return "something else";
        }
    }
}

int AvartarDB_Base::JennicDataParseLen(byte *format_offset_reportflag){
	UInt32 format=(*format_offset_reportflag)*0x01000000+(*(format_offset_reportflag+1))*0x00010000+(*(format_offset_reportflag+2))*0x00000100+(*(format_offset_reportflag+3))*0x00000001;
	//UInt16 format16=(*format_offset_reportflag)*0x0100+(*(format_offset_reportflag+1))*0x0001;
	//cout <<"[" <<(int) rfnumber <<"]: format(32): " << format<< ";(16):"<< format16 ;
	if (format==0xffffffff)
		return -1;
	
    byte b1 = (byte)(format / 0x01000000);	
    if (b1 / 128 == 0){
        return 1;
    }else{
        byte len = (byte)pow((double)2, (double)(b1 % 32) / 4);                      
        return len;
	}
}


bool AvartarDB_Base::SaveDB(BB_OBJ *table_obj, byte open_mode) {//ly dir
	BB_OBJ_STRU table_obj_stru = BB_GET_OBJ_STRU(table_obj);

	return this->SaveDB(table_obj_stru,open_mode);
}

bool AvartarDB_Base::SaveDB(BB_OBJ_STRU &table_obj_stru,byte open_mode) {//ly dir
	//取得文件名
	string file_name;
	if (!this->DBGetSaveFileName(table_obj_stru, file_name)) {
		return false;
	}
		
	FileQueue::POINT point;
	if (this->FQueue.LoadFile(file_name, point, open_mode) < 0) {
		this->SHOW_ERROR((string)"SaveDB(" + file_name + ") error, can't open!");
		return false;
	}

	//写入全局
	BB_SIZE_T table_obj_size = BB_GET_OBJ_SIZE(table_obj_stru.ThisObj);
	if (this->FQueue.WriteFile(point, (const char*)table_obj_stru.ThisObj, table_obj_size, 0, FileQueue::SAVE_SYNC)< 0){
		this->SHOW_ERROR((string)"SaveDB(" + file_name + ") write error!");
		return false;
	}
	return true;
}

bool AvartarDB_Base::SaveDB(string &tableindex,byte open_mode)
{
	//取得表格对象地址
	BB_OBJ *table_obj;
	BB_SIZE_T table_obj_size;
	if (!this->GetTableObject(tableindex,&table_obj,table_obj_size)){
		this->SHOW_ERROR((string)"SaveDB(),Save table error!");
		return false;
	}

	//存盘处理
	BB_OBJ_STRU table_obj_stru=BB_GET_OBJ_STRU(table_obj);
	return this->SaveDB(table_obj_stru,open_mode);
}

bool AvartarDB_Base::LoadDB(BB_OBJ *add_info, BYTE **obj_load, BB_SIZE_T &length, byte open_mode)
{
	//加载并读取文件，要求必须文件存在，如果不存在也不需要TUNCK;
	string file_name;
	file_name = this->GetAddInfo(add_info, AVARTDB_SAVE_FILE_NAME);

	if (file_name.size() != 0) {
		//读取全部文件
		if (LW_access(file_name.c_str(), 0) < 0) {
			//this->SHOW_ERROR((string)"LoadDB(" + file_name + ") file not exit or open error!");
			goto error;//如果文件不存在！
		}
		if (open_mode == FileQueue::OPEN_MODE_TRUNC) {
			this->SHOW_ERROR((string)"LoadDB("+file_name+") not support OPEN_MODE_TRUNC!");
			goto error;//如果只是读取，则不要刷新文件
		}
			
		//加载文件
		FileQueue::POINT point;
		if (this->FQueue.LoadFile(file_name, point, open_mode) < 0) {
			this->SHOW_ERROR((string)"LoadDB(" + file_name + ") can't open file!");
			goto error;
		}
		if (this->FQueue.GetFileLength(point) == 0) {
			this->SHOW_ERROR((string)"LoadDB(" + file_name + ") file length is 0!");
			goto error;
		}	
		if (this->FQueue.ReadFile(point, (void **)obj_load, length,FileQueue::READ_ALL) == 0)
			return true;
	}
	
	//差错返回
error:
	length = 0;
	(*obj_load) = 0;

	return false;
}

bool AvartarDB_Base::DBGetSaveFileName(BB_OBJ_STRU &table_obj_stru,string &file_name) {
	//取得文件
	if (table_obj_stru.AddInfoObj == NULL) {
		this->SHOW_ERROR((string)"DBGetFileName() error, obj_stru.AddInfoObj is Null!");
		return false;
	}
	file_name = GetAddInfo(table_obj_stru.AddInfoObj, AVARTDB_SAVE_FILE_NAME);
	if (file_name.size() == 0) {//可能有不许要存盘的情况
		//this->SHOW_ERROR((string)"DBGetFileName() error, obj_stru.AddInfoObj->'"+AVARTDB_SAVE_FILE_NAME+"‘ is null !");
		return false;
	}
		
	return true;
}

bool AvartarDB_Base::DBCommitAll(bool debug) {//ly dir												//如果只是flush
	//取得文件名
	return this->FQueue.CommitAll(debug);
}

bool AvartarDB_Base::SaveDBNode(string &tableindex, string &node_id, byte save_flag) {//ly dir																			  //取得表格对象地址
	BB_OBJ *table_obj;
	BB_SIZE_T table_obj_size;
	if (!this->GetTableObject(tableindex, &table_obj, table_obj_size)) {
		this->SHOW_ERROR((string)"SaveDB(),Save table error!");
		return false;
	}

	//存盘处理
	BB_OBJ_STRU table_obj_stru = BB_GET_OBJ_STRU(table_obj);
	return this->SaveDBNode(table_obj_stru, node_id, save_flag);
}

bool AvartarDB_Base::SaveDBNode(BB_OBJ_STRU &table_obj_stru, string &node_id, byte save_flag) {//ly dir
	int r = 0;
	BB_SIZE_T start = 0,item_size = 0;	

	//取得文件名
	string file_name;
	if (!this->DBGetSaveFileName(table_obj_stru, file_name)) {
		return false;
	}

	//加载文件
	FileQueue::POINT point;
	if (this->FQueue.LoadFile(file_name, point) < 0)
		return false;

	//写入节点
	if (node_id.size() == 0) {
		this->SHOW_ERROR((string)"SaveDBNode(" + file_name + ") nodeid is null ");
		return false;
	}

	//更新AddInfo？？？
	start = 0;
	item_size = table_obj_stru.Data - table_obj_stru.ThisObj;
	if ((save_flag & FileQueue::SAVE_NO_HEAD) != FileQueue::SAVE_NO_HEAD) {
		if (this->FQueue.WriteFile(point, (const char*)table_obj_stru.ThisObj, item_size, start, FileQueue::SAVE_NO_SYNC) < 0) {
			this->SHOW_ERROR((string)"SaveDBNode(" + file_name + ") write add_info error! err_num(" + STRING_Helper::iToStr(r) + ")");
			return false;
		}
	}

	//写入node_id;
	BYTE *item;
	if (BB_IS_UINT(table_obj_stru.IndexObj)){//非索引写入
		BB_SIZE_T num = STRING_Helper::scanUINTStr(node_id);
		if (!BB_GET_ARRAY_REC_BY_NUM_STRU(&table_obj_stru, num,&item, &item_size)) {
			//取得表格行
			this->SHOW_ERROR((string)"SaveDBNode(" + file_name + ") get nodeid( "+node_id+") error!");
			return false;
		}
	}
	else {//索引写入
		if (!BB_GET_ARRAY_REC_BY_INDEX_STRU(&table_obj_stru, (BYTE *)node_id.data(), node_id.size(), &item, &item_size)) {
			//取得表格行
			this->SHOW_ERROR((string)"SaveDBNode(" + file_name + ") get nodeid( " + node_id + ") error!");
			return false;
		}
	}
	
	start = item - table_obj_stru.ThisObj;
	if (this->FQueue.WriteFile(point, (const char*)item, item_size,start, save_flag) < 0) {
		this->SHOW_ERROR((string)"SaveDBNode(" + file_name + ") write add_info error! err_num(" + STRING_Helper::iToStr(r) + ")");
		return false;
	}
	return true;
}

bool AvartarDB_Base::GetTableRedirValue(BB_OBJ *obj,BB_SIZE_T obj_size,string &ret_str){
	//取得重定向目录集
	BB_OBJ *value_obj;
	BB_SIZE_T value_obj_size;
	if (!this->GetTableRedirObj(obj,obj_size,&value_obj,value_obj_size))
		return false;
	ret_str.clear();
	ret_str.append((const char*)value_obj,value_obj_size);
	return true;
}

bool AvartarDB_Base::GetTableRedirObj(BB_OBJ *obj,BB_SIZE_T obj_size,BB_OBJ **value_obj, BB_SIZE_T &value_obj_size){
	//取得重定向目录集
	if (BB_GET_OBJ_TYPE(obj)!=BB_REDIR)
		return false;
	string search_str=string((const char *)BB_GET_OBJ_DATA(obj),BB_GET_OBJ_DATA_LEN(obj));

	if (!this->GetTableObject(search_str,value_obj,value_obj_size))//测试添加节点内容部分
		return false;
	return true;
}

bool AvartarDB_Base::GetTableObject(string &search_str, BB_OBJ **obj, BB_SIZE_T &obj_size) {//ly 910
	BB_OBJ *sobj = this->rootObj;
	int size_search_str = search_str.size();
    //cout <<"time_track0.2.2" << endl;
    if (!BB_GET_OBJ_BY_PATH(sobj, (BB_OBJ *)(search_str.data()), size_search_str, obj, &obj_size) || ((*obj) == NULL)) {
        //cout <<"time_track0.2.3" << endl;
		this->SHOW_ERROR((string)"GetTableObject(),BB_SEARCH_CELL_SIZE(sp)may be error-" + STRING_Helper::HBCStrToHBCStr(search_str, "char", "hex", true));
		return false;
	}
    else{
        //obj_size=obj_size_1;
		return true;
    }
}

bool AvartarDB_Base::CreateArray_H2C(string &header_search_str,const char *src_content,BB_SIZE_T src_content_size,BB_OBJ **dest_obj, BB_SIZE_T &dest_obj_size){//ly 910
    //取得对象
    BB_OBJ *sobj = this->rootObj;
    BB_SIZE_T size_search_str = header_search_str.size();
    BB_OBJ *obj;
    BB_SIZE_T obj_size;
    if (!BB_GET_OBJ_BY_PATH(sobj, (BB_OBJ *)(header_search_str.data()), (int)size_search_str, &obj, &obj_size) || (obj == NULL)) {
        return false;
    }
    
    //取得头信息
    BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
    (*dest_obj)=BB_NEW_ARRAY(obj_stru.CellObj,obj_stru.IndexObj,obj_stru.AddInfoObj,obj_stru.MaxLen,src_content,src_content_size,NULL);
    
    //创建新对象
    dest_obj_size=BB_GET_OBJ_SIZE(*dest_obj);
    return true;
}

bool AvartarDB_Base::GetHeaderStrObj(string &search_str, string &header){//ly 910
    BB_OBJ *sobj = this->rootObj;
    BB_SIZE_T size_search_str = search_str.size();
    BB_OBJ *obj;
    BB_SIZE_T obj_size;
    //取得对象
    if (!BB_GET_OBJ_BY_PATH(sobj, (BB_OBJ *)(search_str.data()), (int)size_search_str, &obj, &obj_size) || (obj == NULL)) {
        this->SHOW_ERROR((string)"GetTableObject(),BB_SEARCH_CELL_SIZE(sp)may be error-" + STRING_Helper::HBCStrToHBCStr(search_str, "char", "hex", true));
        return false;
    }
    
    //取得头信息
    BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
    header.clear();
    BB_SIZE_T size=obj_stru.Data-(BYTE *)obj-1;
    header.append((const char *)obj,size);
    
    return true;
}

int AvartarDB_Base::strTableToBinary(string & dest, string &src, BB_OBJ_STRU &cell_obj_stru, string split_row, PV_AddParas_T &add_paras) {
	  //输入：cell_obj表格头对象; col:字段分割符; row 行分割符
	  vector<string> row_array;

	  //切分行
	  STRING_Helper::stringSplit(src, row_array, split_row);

	  //循环行操作
	  dest = "";
	  for (BB_SIZE_T i = 0; i<row_array.size(); i++) {
		  STRING_Helper::strTrim(row_array[i]);
		  if (row_array[i].size() == 0)
			  continue;//过滤空格区域
		  if (this->strArrayToBinary(dest, row_array[i], cell_obj_stru, add_paras) == -1) {
			  this->SHOW_ERROR((string)"strTableToBinary()->strArrayToBinary() error !: row[" + STRING_Helper::iToStr(i) + "]:" + row_array[i]);
			  return -1;
		  }
	  }
	  return row_array.size();
}

int AvartarDB_Base::strTableToBinary(string & dest, string &src_str, BB_OBJ_STRU &cell_obj_stru, PV_AddParas_T &add_paras) {
	STRING_Helper::Trim(src_str);

	//拆分处理和循环解析
	const char *src = src_str.c_str();
	if ((*src) == '\0')
		return 0;

	char *next, *start = (char *)src; //*end = (char *)(src + src_str.size());;
	string tempContent;
	if (this->ParseToBinary_NextByPass(dest, tempContent, start, next, (char *)(src + src_str.size()), &cell_obj_stru, add_paras) == -2) {//取得下一格,,区域，或ByPass(EncloseFunc解析结果，并自动递进
		//差错处理
		this->SHOW_ERROR((string)"strTableToBinary() parse error!");
		return -1;//to_add
	}
	return 1;
}

int AvartarDB_Base::strArrayToBinary(string & dest, string &src, PV_AddParas_T &add_paras) {
	string split_col(",");
	vector<string> col_array;
	//切分列
	STRING_Helper::stringSplit(src, col_array, split_col);

	//循环列操作
	for (BB_SIZE_T j = 0; j<col_array.size(); j++) {
		STRING_Helper::strTrim(col_array[j]);//去除两边的空格，还包含OA,OD!!!
											 //取得cell预定义宽度
		string parse_field_value = STRING_Helper::HBCStrToHBCStr(col_array[j], LW_HEX_STR, LW_BINARY_STR, false);
		dest.append(parse_field_value);
	}
	return col_array.size();
}

int AvartarDB_Base::strArrayToBinary(string & dest, const char *src_str, BB_OBJ_STRU &cel_obj_stru, PV_AddParas_T &add_paras) {
	string src(src_str);
	return this->strArrayToBinary(dest, src, cel_obj_stru, add_paras);
}
int AvartarDB_Base::strArrayToBinary(string & dest, string &src_str, BB_OBJ_STRU &cell_obj_stru, PV_AddParas_T &add_paras) {
	//清理
	if (cell_obj_stru.Type != BB_ARRAY && cell_obj_stru.Type != BB_TABLE) {
		//如果不是表格,一般为直接调用，这是必须有[]
		if (this->_ParseVector_TempStrFind(src_str.c_str(), "[]") == NULL) {
			this->SHOW_ERROR((string)"ARRAY must defined like [***,***,****]");
			return -1;//to_add
		}
		else
			STRING_Helper::strFastCut(src_str, "[", "]", 0);
	}

	if (this->TableCfgArrayToBinary(dest, src_str, &cell_obj_stru, add_paras) < 0)
		return -1;
	else
		return 0;
}

int AvartarDB_Base::ParseToBinary_NextByPass(string &dest, string & result, char* &start, char* &next, char *end, BB_OBJ_STRU *cell_obj_stru, PV_AddParas_T &add_paras,int rit,int deep,int rec) {
	//result = "";
	char *tmp1;
	int rt=0,start_pos = 0;
	int rit_begin = -1;
	 
	if (rit!=0 ||(rit_begin =STRING_Helper::C_CmpFirstStr(start, start_pos, "%Z,%B,%A,%T,[,\",%Q,", ",",(next - start),true))>=0) {
		if (rit_begin != -1)
			rit = rit_begin;
		string tempContent("");
		start = start+ start_pos;
		switch (rit) {
		case 0:
			return -2; //只是过滤

		case 1: //%B([***])
			int r;
			if (rit_begin != -1) {
				start += 2;
			}
			start_pos = 0;
			//if ((r = STRING_Helper::C_CmpFirstStr(start, start_pos, "[,\",{,0,1,2,3,4,5,6,7,8,9", ",", (next - start), true))<0){
			if ((r = STRING_Helper::C_CmpFirstStr(start, start_pos, "[,\",{", ",", (next - start), true))<0) {
				//第二轮才作此判断，第一轮为%B
				this->SHOW_ERROR((string)"ParseToBinary_NextByPass():is not Barray( [ or \" or { or num) not closed!");
                r = STRING_Helper::C_CmpFirstStr(start, start_pos, "[,\",{", ",", (next - start), true);
				return -2;
			}
			start = start + start_pos;
            if (r == 0) {
				if (this->_ParseVector_GetNext(start, tempContent, next, "[]") < 0) {
					this->SHOW_ERROR((string)"ParseToBinary_NextByPass():is not Barray %B(****)");
					return -2;
				}
				if (this->JsonBArrayToBArray(tempContent, dest, add_paras, 1, deep + 1,rec) < 0) //%B（{}）JsonQuery函数处理; r=1则rit=当前case n步骤
					return -2;
				rt = 1;
			}
			else if(r==1) {//如果发现\"
				if ((next = STRING_Helper::C_StrStrLen(start + 1, "\"", end - (start + 1))) == NULL) {
					this->SHOW_ERROR((string)"ParseToBinary_NextByPass():is not Barray \"****\"");
					return -2;
				}
				result = string(start, next - start + 1);//未结尾,转普通处理
				next++; //跳过"\""的下一个字
				rt = 0;
			}
			else if(r==2){//如果是大括号
				this->SHOW_ERROR((string)"ParseToBinary_NextByPass():is not support '{' or '}' in \"****\"!");
				return -2;
				break;
			}
			break;
		case 2: //%A(***);需要再补充测试
			if ((r=this->_ParseVector_GetNext(start, tempContent, next, "()")) < 0 ) {
				this->SHOW_ERROR((string)"FuncArrayToBArray():is not Barray %A(****)");
				return -2;
			}
			if (this->FuncArrayToBArray(tempContent, dest, add_paras) < 0) //%A（,,%A(,,),,）数组处理(r==0 ? 0 : rit)
				return -2;
			rt = 1;
			if (rit_begin != -1 && tempContent.substr(0,4)=="0x82") {//ly debug???只是用于跟踪结果
				string tmp1;
				this->FuncArrayToBArray(tempContent, tmp1, add_paras);
				this->DEBUGY(STRING_Helper::HBCStrToHBCStr(tmp1, "char", "hex", true));
			}
			break; 
		case 3: //%T(***);需要再补充测试
			if ((r = this->_ParseVector_GetNext(start, tempContent, next, "[]")) < 0) {
				this->SHOW_ERROR((string)"JsonArrayToBArray():is not Barray %T(****)");
				return -2;
			}
			if (this->JsonArrayToBArray(tempContent, dest, add_paras, r == 0 ? 0 : rit) < 0) //%A（,,%A(,,),,）数组处理(r==0 ? 0 : rit)
				return -2;
			rt = 1;
			break; 
		case 4: //[]
			if (this->_ParseVector_GetNext(start, tempContent, next, "[]") != 1) {
				this->SHOW_ERROR((string)"ParseToBinary_NextByPass():is not Barray [****]");
				return -2;
			}
			//cout << (string)"tempContent:"+tempContent << endl;
			if (this->TableCfgArrayToBinary(dest, tempContent, cell_obj_stru, add_paras) < 0) //表格配置[[***],[***]]函数
				return -2;
			rt = 1;
			break;
		case 5: //""???? 是否需要start=start+tmp_pos ?
			if ((next = STRING_Helper::C_StrStrLen(start + 1, "\"", end - (start + 1))) == NULL) {
				this->SHOW_ERROR((string)"ParseToBinary_NextByPass():is not Barray \"****\"");
				return -2;
			}
			int tmp_pos;
			if (STRING_Helper::C_CmpFirstStr(start + 1, tmp_pos, "{,}", ",", (next - start), true) >= 0) {
				this->SHOW_ERROR((string)"ParseToBinary_NextByPass():is not support '{' or '}' in \"****\"!");
				return -2;
			}
			result = string(start, next - start + 1);//未结尾,转普通处理
			next++; //跳过"\""的下一个字符
			//cout << (string)"result:"+result << endl;
			rt = 0;
			break;
		case 6: //%Q(***)
			if (this->_ParseVector_GetNext(start, tempContent, next, "()") != 1) {
				this->SHOW_ERROR((string)"ParseToBinary_NextByPass():is not Barray %Q(****)");
				return -2;
			}
			if (this->JsonQueryToBArray(tempContent, dest, add_paras) < 0) //%B（{}）JsonQuery函数处理
				return -2;
			rt = 1;
			break;
		}
		//if ((tmp1 = STRING_Helper::C_StrStrLen(next + 1, ",", end - (next + 1))) != NULL)
			//next = tmp1; //定位下一个",
	}
	else if ((next = strstr(start, ",")) != NULL) { //第一个条件递进,并判断；后一个条件可能是[[***]]的情况：
		result = string(start, next - start);//未结尾,转普通处理
		rt = 0;
	}
	else {
		result = string(start, end - start);//未结尾,转普通处理
		rt = 0;
	}
	//补充next定位下一个","
	if ((next!=NULL) && ((*next) != ',') && (next!=end)) {
		if ((tmp1 = STRING_Helper::C_StrStrLen(next + 1, ",", end - (next + 1))) != NULL)
			next = tmp1; //定位下一个",
	}
	return rt;
}

int AvartarDB_Base::TableCfgArrayToBinary(string & dest, string &src_str, BB_OBJ_STRU *cell_obj_stru, PV_AddParas_T &add_paras) {//[***]
	BB_SIZE_T field_size;

	//拆分处理和循环解析
	const char *src = src_str.c_str();
	char *next=NULL, *start = (char *)src, *end=(char *)(src+src_str.size());
	vector <string> part_array;   string tempContent;  //size_t bpos, apos = 0;	
	unsigned int j = 0;
	for (;;)
	{
		tempContent = "";
		int r;
		if ((r = this->ParseToBinary_NextByPass(dest,tempContent, start, next, (char *)(src + src_str.size()), cell_obj_stru,add_paras)) == -2) {//取得下一格,,区域，或ByPass(EncloseFunc解析结果，并自动递进
			//差错处理
			this->SHOW_ERROR((string)"Array Cell Parse error! num=" + STRING_Helper::iToStr(j));
			return -1;//to_add
		}
		else if (r == 1) {
			//ByPass处理操作，并取得下一个
			//dest.append(tempContent);
			//cout << "step1" << endl;
		}
		else {
			//普通处理操作,在此处
			STRING_Helper::strTrim(tempContent);//去除两边的空格，还包含OA,OD!!!	
			//cout << "step2" << endl;
            if (cell_obj_stru==NULL){
                this->SHOW_ERROR((string)"Array Cell Parse error2! num=" + STRING_Helper::iToStr(j));
                return -1; //ly change 2019-04-05
            }
			switch (cell_obj_stru->Type) {//取得cell预定义宽度
			case BB_ARRAY:
			case BB_TABLE:
				if ((j + 1) > cell_obj_stru->Num || ((next == NULL) && ((j + 1) != cell_obj_stru->Num))) {//源长度不符合cell头顶一;array.size==j+1
					this->SHOW_ERROR((string)"strArrayToBinary->BB_TABLE cell_number(" + STRING_Helper::iToStr(cell_obj_stru->Num) + ") != source string number(" + STRING_Helper::iToStr(j) + ")! may be ,,[] not close!");
					return -1;//to_add
				}
				if ((field_size = BB_GET_TABLE_FIELD_WIDTH_STRU(cell_obj_stru, j)) == BB_SIZE_T_ERROR) {
					this->SHOW_ERROR((string)"strArrayToBinary->BB_GET_TABLE_FIELD_WIDTH()error! field_num=" + STRING_Helper::iToStr(j));
					return -1;//to_add
				}
				break;
			case BB_UINT8:		case BB_UINT16:		case BB_UINT32:		case BB_UINT64:
				field_size = cell_obj_stru->DataLen;
				break;
			case BB_BYTES:
			case BB_BYTES_EXT:
				//创建指针对象 
				field_size = tempContent.size();
				break;
			case BB_PARA_EXT:
				this->SHOW_ERROR((string)"strArrayToBinary->not support BB_PARA_EXT now!");
				return -1;//to_add
			default://UINT类
				field_size = 0;
				break;
			}
			//读取cell
			if (cell_obj_stru->Type == BB_BYTES || cell_obj_stru->Type == BB_BYTES_EXT) {
				string tmp;
				if (!this->strCellToBinary(tmp, tempContent, cell_obj_stru->Type, field_size, add_paras)) {
					this->SHOW_ERROR((string)"strArrayToBinary->strCellToBinary error !: col_array[" + STRING_Helper::iToStr(j) + "]:" + tempContent);
					return -1;
				}
				BB_OBJ *son_obj_p = BB_NEW_CELL(cell_obj_stru->Type, (const void *)tmp.data(), tmp.size(), NULL);//注：不需要创建内存																							//添加对象
				dest.append((const char *)son_obj_p, BB_GET_OBJ_SIZE(son_obj_p));
				if (son_obj_p)
					free(son_obj_p);
			}
			else {
				//cout << (string)"step3:"+ tempContent << endl;
				if (!this->strCellToBinary(dest, tempContent, cell_obj_stru->Type, field_size, add_paras)) {
					this->SHOW_ERROR((string)"strArrayToBinary->strCellToBinary error !: col_array[" + STRING_Helper::iToStr(j) + "]:" + tempContent);
					return -1;
				}
			}
		}

		//跳出处理
		j++;
		if (next == NULL)
			break;
		if (next >= end)
			break;
		start = next + 1; //? for ,
		if (start >= end)
			break;
	}
	return 0;
}

int AvartarDB_Base::BArrayToJsonArray(BB_OBJ *src_obj, string & dest, PV_AddParas_T &add_paras) {
	BB_OBJ_STRU src_stru = BB_GET_OBJ_STRU(src_obj);
	if (src_stru.Type != BB_ARRAY) {
		this->SHOW_ERROR((string)"BArrayToJsonArray()->only support BB_ARRAY!");
		return -1;
	}

	string tmp("");
	BB_OBJ_STRU cell_obj_stru = BB_GET_OBJ_STRU(src_stru.CellObj);
	switch (cell_obj_stru.Type) {
	case BB_NULL: {
		BB_OBJ *item_obj, *bytes_obj_p = (BB_OBJ *)src_stru.Data, *start_p = bytes_obj_p;
		BB_SIZE_T item_obj_size;
		BB_SIZE_T rec = 0, objs_data_size = src_stru.DataLen;
		//循环子记录
		for (; bytes_obj_p<(start_p + objs_data_size); bytes_obj_p += item_obj_size, rec++,tmp="") {
			//通用循环
			if (bytes_obj_p > src_stru.Data + src_stru.DataLen) {
				this->SHOW_ERROR((string)"BArrayToJsonArray()->CellObj Type error!");
				return -1;
			}
			item_obj = (BB_OBJ *)bytes_obj_p;
			item_obj_size = BB_GET_OBJ_SIZE(bytes_obj_p);
			//专用处理
			if (dest.size() != 0)
				dest += ",";
			switch (BB_GET_OBJ_TYPE(item_obj)) {
			case BB_ARRAY: {
					if (BB_IS_OBJ_STRING(item_obj)){//针对自定义字符串对象
						tmp = string((const char*)BB_GET_OBJ_DATA(item_obj), BB_GET_OBJ_DATA_LEN(item_obj));
						dest.append((string)"\"" + tmp + "\"");
                    }else if (BB_IS_BYTES_ALL(item_obj)){//针无限而今直流对象
                        //BB_OBJ *data = (BB_OBJ *)src_stru.Data;
                        //BB_SIZE_T data_len = src_stru.DataLen;
                        BB_OBJ *data = (BB_OBJ *)BB_GET_OBJ_DATA(item_obj); //ly malloc2.2 差错更改
                        BB_SIZE_T data_len = BB_GET_OBJ_DATA_LEN(item_obj);
                        tmp = string((const char*)data, data_len);
                        dest.append((string)"\"" + STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", true)+ "\"");
                    }else {
						//普通ARRAY
						if (this->BArrayToJsonArray(item_obj, tmp, add_paras) < 0) {
							string err1 = string((const char*)BB_GET_OBJ_DATA(src_obj), BB_GET_OBJ_DATA_LEN(src_obj));
							string err2=STRING_Helper::HBCStrToHBCStr(err1, "char", "hex",true);
							this->SHOW_ERROR((string)"BArrayToJsonArray()->BArrayToJsonArray()error: " + err2,true);
							return -1;
						}
						dest.append((string)"[" + tmp + "]");
					}
					//cout << dest << endl;
				}
				break;
			case BB_BYTES:case BB_BYTES_EXT:
				tmp = string((const char*)BB_GET_OBJ_DATA(item_obj), BB_GET_OBJ_DATA_LEN(item_obj));
				dest.append((string)"\""+STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", true)+"\"");
				//cout << dest << endl;
				break;
			case BB_UINT8: case BB_UINT16: case BB_UINT32: case BB_UINT64:
				tmp = string((const char*)BB_GET_OBJ_DATA(item_obj), BB_GET_OBJ_DATA_LEN(item_obj));
				dest.append((string)"\"" + STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", true) + "\"");
				break;
			case BB_NULL:
				tmp = string((const char*)BB_GET_OBJ_DATA(item_obj), BB_GET_OBJ_DATA_LEN(item_obj));
				dest.append((string)"\"\"");
				break;
			default:
				//this->SHOW_ERROR((string)"BArrayToJsonArray()->only support BB_ARRAY, BB_BYTES, BB_BYTES_EXT!");
				//return -1;
				dest.append((string)"\"NaN\"");
				break;
			}
		}
		break;
	}
    default:
		this->SHOW_ERROR((string)"BArrayToJsonArray()->BB_ARRAY only support obj array now!");
		break;
	};
		
	return 0;
}

int AvartarDB_Base::FuncArrayToBArray(string &src_str, string & dest, PV_AddParas_T &add_paras, int rit) {
	//拆分处理和循环解析
	const char *src = src_str.c_str();
	char *next, *start = (char *)src, *end = (char *)(src + src_str.size());;
	vector <string> part_array;
	string tempContent, tmp_result, tmp_dest;  //size_t bpos, apos = 0;
	int j = 0;

	while (1)
	{
		tempContent = tmp_result = tmp_dest = "";
		int r;
		if ((r = this->ParseToBinary_NextByPass(tmp_dest, tempContent, start, next, (char *)(src + src_str.size()), NULL, add_paras, rit)) == -2) {//取得下一格,,区域，或ByPass(EncloseFunc解析结果，并自动递进
																																				   //差错处理
			this->SHOW_ERROR((string)"FuncArrayToBArray(): Array Cell Parse error! num=" + STRING_Helper::iToStr(j));
			return -1;//to_add
		}
		else if (r == 1) {
			//ByPass处理操作，并取得下一个
			part_array.push_back(tmp_dest);
		}
		else {
			//普通处理操作
			if (this->JsonCellToObjStr(tempContent, tmp_result, add_paras))
				part_array.push_back(tmp_result);
			else
				return -1;
		}
		//跳出处理
		if (next == NULL)
			break;
		if (next >= end)
			break;
		start = next + 1; //? for ,
		j++;
	}

	//头参数处理
	BB_OBJ *dest_obj, *cell_obj, *index_obj, *add_info_obj;  dest_obj = cell_obj = index_obj = add_info_obj = BB_NULL;
	BYTE pos = 1;
	if (part_array.size() < 1) {
		this->SHOW_ERROR((string)"FuncArrayToBArray() part_array.size()==0!");
		return -1;//to_add
	}
	else if (part_array[pos].size() == 0) {
		return 0;
	}
	else {
		cell_obj = (BB_OBJ *)part_array[pos].data();
		if ((part_array[0].at(0)&BB_MASK_IndexObj) == BB_MASK_IndexObj && ((size_t)(pos + 1)) <= part_array.size()) {
			pos++;
			index_obj = (BB_OBJ *)part_array[pos].data();
		}
		if ((part_array[0].at(0)&BB_MASK_AddInfo) == BB_MASK_AddInfo && ((size_t)(pos + 1)) <= part_array.size()) {
			pos++;
			add_info_obj = (BB_OBJ *)part_array[pos].data();
		}
		//内容对象组织
		string part_data = "";
		pos++;
		for (; pos < part_array.size(); pos++)
			part_data.append(part_array[pos]);  //ly json 可以加入BBYTE 也可以手工直接加入(如4400000001，需要有头)。
												//创建数组
		dest_obj = BB_NEW_ARRAY(cell_obj, index_obj, add_info_obj, 0, part_data.data(), part_data.size(), NULL);//生成带上述索引的空数组
		string test = string((const char *)dest_obj, BB_GET_OBJ_SIZE(dest_obj));
		dest.append(test);
	}

	//清理内存
	if (dest_obj != NULL)
		free(dest_obj);

	return 0;
}

int AvartarDB_Base::JsonArrayToBArray(string &src_str, string & dest, PV_AddParas_T &add_paras,int rit) {
//拆分处理和循环解析
	const char *src = src_str.c_str();
	char *next, *start = (char *)src, *end = (char *)(src + src_str.size());;
	vector <string> part_array;   
	string tempContent,tmp_result,tmp_dest;  //size_t bpos, apos = 0;
	int j = 0;

	while (1)
	{
		tempContent=tmp_result =tmp_dest= "";
		int r;
		if ((r = this->ParseToBinary_NextByPass(tmp_dest,tempContent, start, next, (char *)(src + src_str.size()), NULL, add_paras,rit)) == -2) {//取得下一格,,区域，或ByPass(EncloseFunc解析结果，并自动递进
			//差错处理
			this->SHOW_ERROR((string)"JsonArrayToBArray(): Array Cell Parse error! num=" + STRING_Helper::iToStr(j));
			return -1;//to_add
		}
		else if (r == 1) {
			//ByPass处理操作，并取得下一个
			part_array.push_back(tmp_dest);
		}
		else {
			//普通处理操作
			if (this->JsonCellToObjStr(tempContent, tmp_result, add_paras))
				part_array.push_back(tmp_result);
			else
				return -1;
		}
		//跳出处理
		if (next == NULL)
			break;
		if (next >= end)
			break;
		start = next + 1; //? for ,
		j++;
	}

	if (part_array.size() != 0) {
		string part_data;
		for (unsigned int i = 0; i < part_array.size(); i++) {
			part_data.append(part_array[i]);
		}
		BYTE cell_obj = 0;
		BB_OBJ *dest_obj, *index_obj, *add_info_obj;  dest_obj = index_obj = add_info_obj = BB_NULL;
		dest_obj = BB_NEW_ARRAY((BB_OBJ *)&cell_obj, index_obj, add_info_obj, 0, part_data.data(), part_data.size(), NULL);//生成带上述索引的空数组
		string test = string((const char *)dest_obj, BB_GET_OBJ_SIZE(dest_obj));
		dest.append(test);

		//清理内存
		if (dest_obj != NULL)
			free(dest_obj);
	}
	return 0;
}

//%B([[****]])
int AvartarDB_Base::JsonBArrayToBArray(string &src_str, string & dest, PV_AddParas_T &add_paras,int rit,int deep,int rec) {
	//拆分处理和循环解析
	const char *src = src_str.c_str();
	char *next=NULL, *start = (char *)src, *end = (char *)(src + src_str.size());;
	vector <string> part_array;
	string tempContent, tmp_result, tmp_dest;  //size_t bpos, apos = 0;
	unsigned int j = 0;
	string part_data = "";
	
	for(int rec=0;; rec++)
	{
		tempContent = tmp_result = tmp_dest = "" ;
		int r;
		if ((r = this->ParseToBinary_NextByPass(tmp_dest, tempContent, start, next, (char *)(src + src_str.size()), NULL, add_paras, rit, deep,rec)) == -2) {//取得下一格,,区域，或ByPass(EncloseFunc解析结果，并自动递进
			//差错处理
			this->SHOW_ERROR((string)"JsonBArrayToBArray():Array Cell Parse error! num=" + STRING_Helper::iToStr(j));
			return -1;//to_add
		}
		else if (r == 1) {
			if (deep==0)
				dest.append(tmp_dest); //如果是第一层，则直接返回（已经累加数组了）
			else
				part_data.append(tmp_dest);//如果不是倒数第一次则累加成数组--
		}
		else {
			//普通处理操作
			BB_OBJ *obj;
			STRING_Helper::Trim(tempContent);
			if (tempContent[0] == '\"') {//取得"****"
				//cout << tempContent << endl;
				if (tempContent[tempContent.size()-1]!= '\"') { 
					this->SHOW_ERROR((string)"Array Cell Parse error! num=" + STRING_Helper::iToStr(j));
					return -1;//to_add
				} 
				string temp1 = tempContent.substr(1, tempContent.size() - 2);
				STRING_Helper::Trim(temp1);
				if (temp1.compare(0, 2, "0x") == 0) {//"0x二进制流"
					string temp2 = STRING_Helper::HBCStrToHBCStr(temp1, "hex", "char", false);
					//obj = BB_NEW_CELL(BB_BYTES, (BYTE *)temp2.data(), temp2.size(), NULL);
                    obj = BB_NEW_BYTES_ALL((BYTE *)temp2.data(), temp2.size(), NULL);
				}else if (deep < 3) {//第一个内容；自描述字串：往往是各种字符串命令
					obj = BB_NEW_OBJ_STRING((BYTE *)temp1.data(), temp1.size(), NULL);
				}else{//"普通字符串"：无限二进制流字符串。可以作为Serach条件，例如["0x06","0x3d3d3d","0x000000020000000200000...]超长，查询内容为二进制流。
					obj = BB_NEW_BYTES_ALL((BYTE *)temp1.data(), temp1.size(), NULL);
				}
				part_data.append(string((const char*)obj, BB_GET_OBJ_SIZE(obj)));
				if (obj != NULL)
					free(obj);
			}
			else if (STRING_Helper::CheckNum(tempContent)) {
				BB_UINT_STRU tmp = BB_NEW_CELL_UINT(STRING_Helper::ParseInt(tempContent),0);
				obj = (BB_OBJ *)&tmp;
				part_data.append(string((const char*)obj, BB_GET_OBJ_SIZE(obj)));
			}
			else {
				this->SHOW_ERROR((string)"JsonBArrayToBArray():Array Cell Parse error! num=" + STRING_Helper::iToStr(j)+"(not '***' or number)");
				return -1;//to_add
			}	
		}
		//跳出处理
		if (next == NULL)
			break;
		if (next >= end)
			break;
		start = next + 1; //? for ,
		if (start >= end)
			break;
		j++;
	}

	//创建数组
	if (part_data.size()!=0) {
		BYTE cell_obj = 0;
		BB_OBJ *dest_obj, *index_obj, *add_info_obj;  dest_obj = index_obj = add_info_obj = BB_NULL;
		dest_obj = BB_NEW_ARRAY((BB_OBJ *)&cell_obj, index_obj, add_info_obj, 0, part_data.data(), part_data.size(), NULL);//生成带上述索引的空数组
		string test = string((const char *)dest_obj, BB_GET_OBJ_SIZE(dest_obj));
		dest.append(test);
		
		//清理内存
		if (dest_obj != NULL)
			free(dest_obj);
	}
	return 0;
}

int AvartarDB_Base::JsonQueryToBArray(string &src_str, string & dest, PV_AddParas_T &add_paras) {
	//清理
	STRING_Helper::strFastCut(src_str, "{", "}", 0);  //?? 是否有必要

	vector <string> part_array;
	STRING_Helper::stringSplit(src_str, part_array, ",");
	vector <string> json_para;
	//解析任务头
	BYTE act = 0, state = 0, flag = 0;
	BYTE *tmp_p;
	string session = AVARTDB_AUTH_MASK_FULL;
	string tmp_str,method_obj = "",node_id_obj="",content_obj="",index_table_obj="",condition_obj="",cursor_obj="",task_id_obj= "",convert_obj="";
	for (unsigned int i = 0; i < part_array.size(); i++) {
		//拆分出json的act:""的类似元素。
		STRING_Helper::stringSplit(part_array[i], json_para, ":");
		if (json_para.size() == 1) {
			this->SHOW_ERROR((string)"JsonQueryToBArray->input is not json");
			return -1;
		}
		STRING_Helper::Trim(json_para[0]);
		STRING_Helper::Trim(json_para[1]);
		//任务说明判断
		if (json_para[0].find("\"")==0)
			STRING_Helper::strFastCut(json_para[0], "\"", "\"", 0);
		if (json_para[1].find("\"") == 0)
			STRING_Helper::strFastCut(json_para[1], "\"", "\"", 0);
		if (json_para[0] == "Act" || json_para[0] == "A") {
			if ((act = this->_JsonQueryToBArray_Define(json_para[1].c_str())) == 0xFF) {
				this->SHOW_ERROR((string)"JsonQueryToBArray->"+ part_array[i] +"is not defined");
				return -1;
			}
			continue;
		}else if (json_para[0] == "Flag" || json_para[0] == "F") {
			if (STRING_Helper::CheckNum(json_para[1]))
				flag = STRING_Helper::HBCStrToHBCStr(json_para[1], "hex", "char", false)[0];//例如0x1
			else if ((flag = this->_JsonQueryToBArray_Define(json_para[1].c_str())) == 0xFF) {//例如READ|BITMAP_CONVERT|CONDITIONS ?需要在后续组合
				this->SHOW_ERROR((string)"JsonQueryToBArray->" + part_array[i] + "is not defined");
				return -1;
			}
			continue;
		}
		//后续字串对象判断
		if (!STRING_Helper::CheckNum(json_para[1])) {
			this->SHOW_ERROR((string)"JsonQueryToBArray()->input:"+part_array[i]+" is not hex number");
			return -1;
		}
		tmp_str = STRING_Helper::HBCStrToHBCStr(json_para[1], "hex", "char", false);
		tmp_p = BB_NEW_CELL(BB_BYTES, (const void *)tmp_str.data(), tmp_str.size(), NULL);//注：不需要创建内存

		if (json_para[0] == "TaskID" || json_para[0] == "ID") {
			task_id_obj = string((const char *)tmp_p, BB_GET_OBJ_SIZE(tmp_p));
		}
		else if (json_para[0] == "ConvertTable" || json_para[0] == "CT") {
			convert_obj = string((const char *)tmp_p, BB_GET_OBJ_SIZE(tmp_p));
		}else if (json_para[0] == "Column" || json_para[0] == "Method" ||  json_para[0] == "M") {
			method_obj = string((const char *)tmp_p, BB_GET_OBJ_SIZE(tmp_p));
		}else if (json_para[0] == "NodeID" || json_para[0] == "N") {
			node_id_obj = string((const char *)tmp_p, BB_GET_OBJ_SIZE(tmp_p));
		}else if (json_para[0] == "Data" || json_para[0] == "D") {
			content_obj = string((const char *)tmp_p, BB_GET_OBJ_SIZE(tmp_p));
		}else if (json_para[0] == "IndexTable" || json_para[0] == "T") {
			index_table_obj = string((const char *)tmp_p, BB_GET_OBJ_SIZE(tmp_p));
		}else if (json_para[0] == "Conditions"|| json_para[0] == "C" || json_para[0] == "Search" || json_para[0] == "S") {
			condition_obj = string((const char *)tmp_p, BB_GET_OBJ_SIZE(tmp_p));
		}else if (json_para[0] == "Page" || json_para[0] == "P") {
			cursor_obj = string((const char *)tmp_p, BB_GET_OBJ_SIZE(tmp_p));
		}else {
			this->SHOW_ERROR((string)"JsonQueryToBArray()->input:" + part_array[i] + "synctactic error!");
			return -1;
		}
		if	(tmp_p)
			free(tmp_p);
	}
	//组头操作
	string add_info_data("");
	if (act != 0 && flag != 0) {	
		string packet_head("");
		packet_head += state; //作为返回值,FF和0都代表成功，ff还代表当前包为返回包。
		packet_head += flag; packet_head += act;
		BB_OBJ *add_info_head_obj = BB_NEW_CELL(BB_BYTES, packet_head.data(), packet_head.size(), NULL);
		add_info_data.append((const char *)add_info_head_obj, BB_GET_OBJ_SIZE((BB_OBJ*)add_info_head_obj));
		add_info_data.append(task_id_obj);
		if (add_info_head_obj) 
			free(add_info_head_obj);
	}
	else {
		this->SHOW_ERROR((string)"JsonQueryToBArray->input is not json");
		return -1;
	}

	//内容操作
	string data="";
	data.append(index_table_obj);
	data.append(method_obj);
	switch (act) {
	case ACTION_WRITE:
		//添加NodeID
		if (node_id_obj.size()!=0)
			data.append(node_id_obj);
		break;
	case ACTION_READ:
		//生成内容数组
		if (condition_obj.size()!=0) {
			data.append(condition_obj);
		}else if (node_id_obj.size()!=0) {
			data.append(node_id_obj);
		}else {
			data.append(STRING_Helper::HBCStrToHBCStr("0x44ffffffff", "hex", "char", false)); //NodeID=0xFFFFFFF for all records
		}
		//
		if (cursor_obj.size()!=0) 
			data.append(cursor_obj);

		if (convert_obj.size() != 0)
			data.append(convert_obj);

		break;
	default:
		this->SHOW_ERROR((string)"JsonQueryToBArray->input is not json");
		return -1;
	}
	
	//建立add_info_obj数组
	BB_OBJ cell = BB_NULL;
	BB_OBJ *add_info_obj = BB_NEW_ARRAY(&cell, NULL, NULL, 0, add_info_data.data(), add_info_data.size(), NULL);
	//string test((const char *)add_info_obj, BB_GET_OBJ_SIZE(add_info_obj));
	//建立data数组
	BB_OBJ *r_obj = BB_NEW_ARRAY(&cell, NULL, add_info_obj, 0, (void *)data.data(), data.size(), NULL); //添加8002**00外部对象数组包
	string test((const char *)r_obj, BB_GET_OBJ_SIZE(r_obj));
	dest.append((const char *)r_obj, BB_GET_OBJ_SIZE(r_obj));
	if (add_info_obj)
		free(add_info_obj);
	if (r_obj)
		free(r_obj);
	return 0;
}
BYTE AvartarDB_Base::_JsonQueryToBArray_Define(const char *obj_type) {
	if ((QFlags.find(obj_type)) != QFlags.end()) {
		return QFlags.find(obj_type)->second;
	}
	else {
		this->SHOW_ERROR((string)"数据定义节Type属性不可理解！");
		return 0xFF;
	}
}
bool AvartarDB_Base::JsonCellToObjStr(string &src_str, string &dest,PV_AddParas_T &add_paras) {
	if (src_str.size() == 0) {
		dest = "";
		return false;
	}
		
	STRING_Helper::strFastCut(src_str, "{", "}", 0);  //?? 是否有必要
	if (src_str.compare(0, 2, "\"0x")==0 || src_str.compare(0, 2, "\"0x") == 0) {
		STRING_Helper::strFastCut(src_str, "\"", "\"", 0);  //?? 是否有必要
		dest.append(STRING_Helper::HBCStrToHBCStr(src_str, "hex", "char", false));
	}else{
		return this->strCellToBinary(dest, src_str, BB_NULL, src_str.size(), add_paras);
	}
	return true;
}
bool AvartarDB_Base::strCellToBinary(string & dest, string &src_str, BYTE field_type, BB_SIZE_T field_size, PV_AddParas_T &add_paras) {
	BYTE a[8];
	//解析{***}宏
	string dest_cell;
	string src;

	//解析"***",number数字

	/*if (src_str.compare(0, 3, "%A(") == 0) {
	//[%A,***,***]数组处理
	string tmp("");
	if (this->JsonArrayToBArray(src_str, tmp, add_paras) < 0)
	return -1; //ly json :for BARRAY
	dest.append(tmp);  //顺序结果累计
	}
	else */
    if (src_str.size()==0)
        return false; //ly table_dir
	if (src_str.at(0) == '"') {
		STRING_Helper::strFastCut(src_str, "\"", "\"", 0);
		if (this->ParseValue(src_str, src, add_paras).size() != 0)
			return false;
		if (src.size()<field_size) {
			if (BB_ARRAY_APPEND_DEFAULT_CHAR != 0) {
				src.append(1, 0x00);
			}
			src.append(field_size - src.size(), (char)BB_ARRAY_APPEND_DEFAULT_CHAR);//补齐空位
		}
		dest.append(src.substr(0, field_size));
	}
	else if (src_str.at(0) == '{' || field_type == BB_BYTES || field_type == BB_BYTES_EXT) {
		//如果是字符串
		STRING_Helper::strFastCut(src_str, "\"", "\"", 0);
		//if (src_str.substr(0,4)=="{IP:" ||src_str.substr(0,4)=="{ip:" ){
		//	//计算ip值
		//	STRING_Helper::strFastCut(src_str,"{IP:","}");
		//	STRING_Helper::strFastCut(src_str,"{IP:","}");
		//	int n=src_str.find(":");
		//	if (n==string::npos){
		//		this->SHOW_ERROR((string)"strCellToBinary()input ip has not :port!"+src_str);
		//		return false;
		//	}
		//	string ip_str=src_str.substr(0,n);
		//	string port=src_str.substr(n,src_str.size());
		//	vector<string> ip_array=STRING_Helper::stringSplit(ip_str,".");
		//	for(size_t i=0; i<ip.size(); i++){
		//		if (!STRING_Helper::CheckNum(ip_array.at(i))){
		//			this->SHOW_ERROR((string)"strCellToBinary()input ip address is not number!"+src_str);
		//			return false;
		//		}
		//		src[i]=STRING_Helper::StrToi(ip_array.at(i));
		//	}
		//}else 
		/*if (src_str.compare(0, 5, "{Act:") == 0 || src_str.compare(0, 5, "{A:") == 0) {
			this->JsonQueryToBArray(src, src_str, add_paras); //ly json
		}
		else */
		if (this->ParseValue(src_str, src, add_paras).size() != 0)
			return false;
		if (src.size()<field_size) {
			src.append(field_size - src.size(), (char)BB_ARRAY_APPEND_DEFAULT_CHAR);//补齐空位
		}
		dest.append(src.substr(0, field_size));
	}
	else {
		//测试是否为数字
		if (!STRING_Helper::CheckNum(src_str)) {
			this->SHOW_ERROR((string)"strCellToBinary()input data must be int!:" + src_str);
			return false;
		}
		//如果是数字
		uint64 c = STRING_Helper::ParseULong(src_str);
		string tmp_c=STRING_Helper::iToStr(c,16);
		if (tmp_c.size()>(field_size*2)){
			this->SHOW_ERROR((string)"strCellToBinary()input data is too large!:" + src_str);
			return false;
		}
		if (!BB_PUT_UINT_TO_BUFF(a, c, field_size)) {
			this->SHOW_ERROR((string)"strCellToBinary()field_size not be 0!:" + src_str);
			return false;
		}
		dest.append((char *)a, field_size);
	}
	return true;
}

bool AvartarDB_Base::AddSearchCell(string &search_str, BYTE search_type, const char *new_cell,BYTE new_cell_size){
	BB_SEARCH_STRU add_info_s=BB_NEW_SEARCH_CELL(search_type,new_cell,new_cell_size);
	search_str.append((const char*)&add_info_s,BB_SEARCH_CELL_SIZE((BB_OBJ *)&add_info_s)); //1BYTE Len
	return true;
}
bool AvartarDB_Base::AddSearchCell(string &search_str, BYTE search_type, string new_cell){
	//设置AddInfo查询
	return this->AddSearchCell(search_str,search_type,new_cell.data(),new_cell.size());
}
BB_SIZE_T AvartarDB_Base::CutTableNodeidStr(string &index_cut_str,byte* buf, string &tableindex){

	//取得Index Cell宽度
	BYTE *obj;
	BB_SIZE_T obj_size;
	if (!this->GetTableObject(tableindex,&obj,obj_size)){
		this->SHOW_ERROR((string)"WriteBufferToDB_OneNode(),Load table error!");
		return false;
	}
	
	BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
	if (obj_stru.IndexObj==NULL){
		this->SHOW_ERROR((string)"WriteBufferToDB_OneNode(),CutTabelNodeidStr TableObj must have IndexObj!");
		return false;
	}

	BB_OBJ_STRU obj_index_stru=BB_GET_OBJ_STRU(obj_stru.IndexObj);
	BB_SIZE_T index_width=obj_index_stru.CellWidth;

	//截取缓存中的nodeid;
	index_cut_str.append((const char *)buf,index_width);//截取源数据的nodeid 区域，根据index cell width对应
	
	return index_width;
}

bool AvartarDB_Base::TestFFFF(string &test_str){
	unsigned char len=test_str.size();
	BYTE *p=(BYTE *)test_str.data();
	
	//cout << "TestFFFF:(" <<(int)len<< ")";
	for(unsigned char i=0; i<len; i++,p++){
		if ((*p)!=0xFF)
			return false;
		//cout <<(int)(*p);
	}
	//cout << endl;
	return true;
}

bool AvartarDB_Base::TestByteAll(BYTE *p, BB_SIZE_T len, BYTE cmp_chr){
    for(unsigned char i=0; i<len; i++,p++){
        if ((*p)!=cmp_chr)
            return false;
    }
    return true;
}

bool AvartarDB_Base::TestFFFF(UInt64 test_num, BB_SIZE_T len){
	if (len==1){
		return test_num==0xFF ? true: false;
	}else if (len == 2 || len==3){
		return test_num==0xFFFF ? true: false;
	}else if (len == 4 || len==5 || len==6 || len==7){
		return test_num==0xFFFFFFFF ? true: false;
	}else{
		return test_num==0xFFFFFFFFFFFFFFFF ? true: false;
	}
}

bool AvartarDB_Base::TestMask(string & s1, string s2) {
	for (unsigned int i = 0; i < s1.size() && i < s2.size(); i++) {
		if ((s1[i] & s2[i]) > 0)
			return true;
	}
	return false;
}

bool AvartarDB_Base::TestMask(const char *s1, BB_SIZE_T s1_len, const char *s2, BB_SIZE_T s2_len) {
	for (unsigned int i = 0; i < s1_len && i < s2_len; i++) {
		if (((*(s1+i)) & (*(s2+i))) > 0)
			return true;
	}
	return false;
}





