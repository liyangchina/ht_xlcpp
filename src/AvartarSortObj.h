/*
 * AvartarSortObj.h
 *
 *  Created on: Jul 26, 2017
 *      Author: root
 */

#ifndef AVARTARSORTOBJ_H_
#define AVARTARSORTOBJ_H_

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "ByteBase.h"
#include "STRING_Helper.h"

//ly sort
typedef std::map<std::string, BB_SIZE_T > AvartarSortObj_Value_Index_T; //针对于单一字段的索引
typedef std::map<BB_SIZE_T, std::string > AvartarSortObj_Num_Index_T; //针对于单一字段的索引
typedef struct{
	AvartarSortObj_Value_Index_T ValueIndex;
	AvartarSortObj_Num_Index_T NumIndex;
}AvartarSortObjVN_T;
typedef std::map<BYTE, AvartarSortObjVN_T> AvartarSortObj_FieldIndex_T; //字段索引数组
typedef std::map<BB_SIZE_T, AvartarSortObj_FieldIndex_T> AvartarSortObj_TableIndex_T; //表格索引组

class AvartarSortObj {
public:
	AvartarSortObj();
	~AvartarSortObj();
	bool InsertItem(BB_OBJ *table_obj, BYTE field_id, BYTE *field_value, BB_SIZE_T field_len, BB_SIZE_T num);
	BB_SIZE_T SeekItem(BB_OBJ *table_obj, BYTE field_id, BYTE *field_value, BB_SIZE_T field_len);
	bool DeleteItem(BB_OBJ *table_obj, BB_SIZE_T num);
	AvartarSortObj_TableIndex_T *TableSortIndex;    //表格索引数组
private:

};

#endif /* AVARTARSORTOBJ_H_ */
