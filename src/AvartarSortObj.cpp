/*
 * AvartarSortObj.cpp
 *
 *  Created on: Jul 26, 2017
 *      Author: root
 */
#include <vector>
#include <map>
#include <string>
#include "AvartarSortObj.h"
#include "ByteBase.h"
#include <stdio.h>


AvartarSortObj::AvartarSortObj() {
	this->TableSortIndex = new AvartarSortObj_TableIndex_T();
};
AvartarSortObj::~AvartarSortObj() {
	if (this->TableSortIndex)
		delete this->TableSortIndex;
};

bool AvartarSortObj::InsertItem(BB_OBJ *table_obj, BYTE field_id, BYTE *field_value, BB_SIZE_T field_len, BB_SIZE_T num) {//ly sort
	//查询table_obj 是否存在,如果不存在则插入
	BB_SIZE_T table_obj_sz = (BB_SIZE_T)table_obj; //??? ly test 应该在路由中测试。
	AvartarSortObj_TableIndex_T::iterator p;
	if ((p = this->TableSortIndex->find(table_obj_sz)) == this->TableSortIndex->end()) {
		//创建空白数组
		AvartarSortObj_FieldIndex_T field_map;
		pair< AvartarSortObj_TableIndex_T::iterator, bool > ret=this->TableSortIndex->insert(make_pair(table_obj_sz, field_map));
		if (!ret.second) {
			//GB_AvartarDB->SHOW_ERROR((string)"AvartarSortObj::InsertItem(table_obj) error!");
			cout << "Error: " <<(string)"AvartarSortObj::InsertItem(table_obj) error!" << endl;
			return false;
		}
		p = ret.first;
	}

	//判断字段是否存在，如果不存在则插入
	AvartarSortObj_FieldIndex_T::iterator f;
	AvartarSortObjVN_T value_num_map;
	if ((f = p->second.find(field_id)) == p->second.end()) {
		//创建空白数组
		std::pair< AvartarSortObj_FieldIndex_T::iterator, bool > ret=p->second.insert(make_pair(field_id, value_num_map));
		if (!ret.second) {
			//GB_AvartarDB->SHOW_ERROR((string)"AvartarSortObj::InsertItem(field_id) error! field_id="+STRING_Helper::iToStr(field_id));
			cout << "Error: " << (string)"AvartarSortObj::InsertItem(field_id) error! field_id="+STRING_Helper::iToStr(field_id) << endl;
			return false;
		}
		f = ret.first;
	}

	//内容插入操作
	string field_value_str((const char*)field_value, field_len);
	f->second.ValueIndex.insert(make_pair(field_value_str, num)); //直接替换即可
	f->second.NumIndex.insert(make_pair(num, field_value_str)); //直接替换即可

	return true;
};

BB_SIZE_T AvartarSortObj::SeekItem(BB_OBJ *table_obj, BYTE field_id, BYTE *field_value, BB_SIZE_T field_len) {
	//查询
	BB_SIZE_T table_obj_sz = (BB_SIZE_T)table_obj;
	AvartarSortObj_TableIndex_T::iterator p;
	if ((p = this->TableSortIndex->find(table_obj_sz)) == this->TableSortIndex->end())
		return BB_SIZE_T_ERROR;

	AvartarSortObj_FieldIndex_T::iterator f;
	if ((f = p->second.find(field_id)) == p->second.end())
		return BB_SIZE_T_ERROR;

	AvartarSortObj_Value_Index_T::iterator v;
	string field_value_str((const char*)field_value, field_len);
	if ((v = f->second.ValueIndex.find(field_value_str)) == f->second.ValueIndex.end())
		return BB_SIZE_T_ERROR;
    //cout << "dddddd:" << f->second.ValueIndex.size() << endl;

	//返回num
	return v->second;
};

bool AvartarSortObj::DeleteItem(BB_OBJ *table_obj, BB_SIZE_T num) {
	AvartarSortObj_Value_Index_T::iterator v;
	//查询表格
	BB_SIZE_T table_obj_sz = (BB_SIZE_T)table_obj;
	AvartarSortObj_TableIndex_T::iterator p;
	if ((p = this->TableSortIndex->find(table_obj_sz)) == this->TableSortIndex->end())
		return false;

	//循环字段数组
	for (AvartarSortObj_FieldIndex_T::iterator f = p->second.begin(); f != p->second.end(); ++f) {
		AvartarSortObj_Num_Index_T::iterator n;
		if ((n = f->second.NumIndex.find(num)) == f->second.NumIndex.end())
			return false;
		//查询num并删除
		string field_value_str=n->second;
		f->second.NumIndex.erase(n);
		//删除field
		AvartarSortObj_Value_Index_T::iterator v;
		if ((v = f->second.ValueIndex.find(field_value_str)) == f->second.ValueIndex.end()) {
			//GB_AvartarDB->SHOW_ERROR((string)"AvartarSortObj::DeleteItem() error !  num(" + STRING_Helper::iToStr(num) + ") not in field(" + field_value_str);
			cout << "Error:" << (string)"AvartarSortObj::DeleteItem() error !  num(" + STRING_Helper::iToStr(num) + ") not in field(" + field_value_str << endl;
			return false;
		}else
			f->second.ValueIndex.erase(v);  //???
	}
	return true;
};

