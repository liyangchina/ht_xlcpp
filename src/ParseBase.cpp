#include "stdafx.h"
#include <iostream>
#include "ParseBase.h"
//#include "HTTP_Config.h"
#include "STRING_Helper.h"
#include "Linux_Win.h"
#include <string>
#include <stdio.h>
#include "string.h"
#include <fstream>
#include <sstream>
#include "pugixml.hpp"
#include <math.h>
#include "WizBase.h"
//#include "ParseExternal.h"
#include <fstream>
#include "ByteBase.h"

using namespace std;
using namespace pugi;


ParseBase::~ParseBase(void){
 
 }

//临时文件名存储操作今后应该改为Http_FileActionProcess::savefiles
int ParseBase::savefiles(const char * path,const char * in_data, size_t data_len, bool is_append,bool is_binary){
    //临时文件名存储操作
    string save_file_name = path;
    char data_len_str[30];
    itoa((int)data_len,data_len_str,10);
    
    std::ofstream stream;
    if (is_binary){
        if (is_append)
            stream.open(save_file_name.c_str(),ios::binary|std::ios::app);
        else
            stream.open(save_file_name.c_str(),ios::binary);
    }else{
        if (is_append)
            stream.open(save_file_name.c_str(),std::ios::app);
        else
            stream.open(save_file_name.c_str());
    }
    if (stream){
        stream.write(in_data,(std::streamsize)data_len);
        stream.close();
        return 0;
    }else
        return -1;
}

int ParseBase::savefiles(const char * path,string &file_data,bool is_append){
    
    //临时文件名存储操作
    if (is_append){
        std::ofstream stream(path,std::ios::app|ios::binary);
        stream.write(file_data.data(),(std::streamsize)file_data.size());
        stream.close();
    }else{
        std::ofstream stream(path,ios::out|ios::in|ios::binary);
        stream.write(file_data.data(),(std::streamsize)file_data.size());
        stream.close();
    }
    
    return 0;
}

int ParseBase::CreatDirExt(const char * path, string split_str){
    //生成目标目录
    vector<string> sub_path;
    vector<string>::iterator scit;
    vector<string>::iterator seit;
    string path_dir = "";
    STRING_Helper::stringSplit(path, sub_path, split_str);//把content按"|"拆分.
    
    seit = sub_path.end();
    seit--;
    for (scit = sub_path.begin(); scit != seit; ++scit)
    {
        path_dir = path_dir + *scit;
        //从缓存中获取
        if (LW_access(path_dir.c_str(), 0) <0)
            if (LW_mkdir(path_dir.c_str())<0)
                return -1;
        
        path_dir = path_dir + split_str;
    }
    return 0;
}

bool ParseBase::TestConditionStr(const char *test_str,PV_AddParas_T &add_paras){
    string err_str("");
    string test_strs(test_str);
    return this->TestConditionStr(test_strs,err_str,add_paras);
}
bool ParseBase::TestConditionStr(string &temp1,string &err_str){
    PV_AddParas_T add_paras;
    return this->TestConditionStr(temp1,err_str,add_paras);
}
bool ParseBase::TestConditionStr(string &temp1,string &err_str,PV_AddParas_T &add_paras){
    
    //内容换算
    string test_str("");
    err_str = this->ParseValue(temp1,test_str,add_paras);
    
    if (err_str.size()==0){
        // String 变换操作
        vector<string> or_str;
        vector<string> and_str;
        string left,right,condition;
        
        STRING_Helper::stringSplit(test_str,or_str," or ");
        int r_or = SWITCH_CASE_TEST_CONDITION_FALSE;
        
        for(size_t i=0; i<or_str.size(); i++){
            STRING_Helper::stringSplit(or_str[i],and_str," and ");
            int r_and = SWITCH_CASE_TEST_CONDITION_TRUE;
            
            for(size_t j=0; j<and_str.size(); j++){
                this->GetConditionPart(and_str[j],condition,left,right);
                r_and = this->TestCondition(condition,left,right);
                if (r_and==ACT_ERROR){
                    err_str="QueryCondition syntax error!";
                    break;
                }else if (r_and == SWITCH_CASE_TEST_CONDITION_FALSE)
                    break;
            }
            
            if (err_str.size()){
                //如果发生错误
                break;
            }else if (r_and==SWITCH_CASE_TEST_CONDITION_TRUE){
                r_or=SWITCH_CASE_TEST_CONDITION_TRUE;
                break;
            }
        }
        //返回赋值
        if (r_or==SWITCH_CASE_TEST_CONDITION_TRUE)
            return true;
        else
            return false;
    }
    return false;
}

int ParseBase::TestCondition(string &condition, string &left, string &right){
    int rt=SWITCH_CASE_TEST_CONDITION_FALSE;
    //各类Conditions判断操作
    if (condition=="=="){
        if (left==right){
            rt=SWITCH_CASE_TEST_CONDITION_TRUE;
        }
    }else if (condition=="="|| condition=="include"){
        if (left.find(right)!=string::npos){
            rt = SWITCH_CASE_TEST_CONDITION_TRUE;
        }
    }else if (condition=="=!"|| condition=="!="){
        if (left!=right){
            rt = SWITCH_CASE_TEST_CONDITION_TRUE;
        }
    }else if (condition=="==="){
        vector <string> right_s;
        STRING_Helper::stringSplit(right,right_s,"|");
        for(size_t i=0; i<right_s.size(); i++){
            if (left==right_s[i]){
                rt = SWITCH_CASE_TEST_CONDITION_TRUE;
                break;
            }
        }
    }else if (condition=="!=="){
        vector <string> right_s;
        STRING_Helper::stringSplit(right,right_s,"|");
        bool test=true;
        for(size_t i=0; i<right_s.size(); i++){
            if (left==right_s[i]){
                test=false;
                break;
            }
        }
        if (test)
            rt = SWITCH_CASE_TEST_CONDITION_TRUE;
    }else if (condition==">"|| condition=="!<"){//尚未提供此功能,在此处添加
        double d_left=STRING_Helper::StrTof(left);
        double d_right=STRING_Helper::StrTof(right);
        if (d_left >  d_right){
            rt = SWITCH_CASE_TEST_CONDITION_TRUE;
        }
    }else if (condition=="<" || condition=="!>"){//尚未提供此功能,在此处添加
        double d_left=STRING_Helper::StrTof(left);
        double d_right=STRING_Helper::StrTof(right);
        if (d_left <d_right){
            rt = SWITCH_CASE_TEST_CONDITION_TRUE;
        }
    }else{
        rt=ACT_ERROR;
    }
    return rt;
}

int ParseBase::GetConditionPart(string &src,string &condition,string &left,string &right){
    unsigned char a1,a2,a3;
    for(size_t i=0; i<src.size(); i++){
        a1=src.at(i);
        
        if (a1=='>'|| a1=='<'|| a1=='=' || a1=='!'){
            //取得下一个节点
            if ((i+1)<src.size())
                a2=src.at(i+1);
            else
                a2='\0';
            //取得左边
            left = src.substr(0,i);
            //继续
            if ((a1=='=' && a2=='=')||(a1=='!' && a2=='=')){
                //在取下一个节点
                if ((i+2)<src.size())
                    a3=src.at(i+2);
                else
                    a3='\0';
                //如果==...
                if (a3!='='){
                    condition=src.substr(i,2);
                    right=src.substr(i+2);
                    break;
                }else{//如果===...
                    condition=src.substr(i,3);
                    right=src.substr(i+3);
                    break;
                }
            }else{
                condition=src.substr(i,1);
                right=src.substr(i+1);
                break;
            }
        }
    }
    STRING_Helper::Trim(left);
    STRING_Helper::Trim(right);
    return 0;
}


//功能:从属于_PARSE_NODE对 node 中的内容进行解析,简单就是{****} Object 替换为相应的 Object 值
//动作:（1）CASE:普通的{****}替换操作.先
//		（2）CASE:{@STR_LEN([_*****_])}数据 Length 函数.
//		（3）CASE:其它函数.
//返回:ParseValue的 String .
string ParseBase::ParseValue(ParseXml_Node parse_node,string &dest,PV_AddParas_T &add_paras){
    string is_debug,temp;
    string r_str="";
    string src_type= this->_ActParseXml->NodeAttr(parse_node,"SrcType");
    is_debug=this->_ActParseXml->NodeAttr(parse_node,"Debug");
    dest="";
    
    if (src_type.size()!=0 && src_type.find("BIN")!=string::npos){
        //如果是二进制流数组
        const char *src=this->_ActParseXml->NodeValue(parse_node);
        if (is_debug.find("PvIn")!=string::npos){
            string src_s(src);
            this->DEBUGY(STRING_Helper::strTrim(src_s));
        }
        this->ParseDeep++;
        dest="";
        //实际操作
        r_str=this->ParseVector(src,dest,add_paras);
        if (is_debug.find("PvOut") != string::npos) {
            this->DEBUGY(STRING_Helper::HBCStrToHBCStr(dest,"char","hex",true));
        }
    }else{
        //如果是普通字串解析
        string src=this->_ActParseXml->NodeValue(parse_node);
        if (is_debug.find("PvIn")!=string::npos){
            this->DEBUGY(STRING_Helper::strTrim(src));
        }
        this->ParseDeep++;
        //实际操作
        r_str=this->ParseValue(src,dest,add_paras);
        if (is_debug.find("PvOut")!=string::npos){
            this->DEBUGY(dest);
        }
    };
    this->ParseDeep--;
    
    return r_str;
}

string ParseBase::CallParseValue(string src){
    string dest;
    PV_AddParas_T add_paras;
    return this->ParseValue(src,dest,add_paras);
};

string ParseBase::ParseValue(string src,string &dest,PV_AddParas_T &add_paras){
    
    string debug_str;
    bool debug_dest=false,json_mark=false;
    string parse_field("");	//解析 Object
    string r_str("");
    
    //取得待解析 String 值
    STRING_Helper::strTrim(src);
    if (src!=""){
        //去除整体区域外部空格,并 Config debug信息.
        if(src.compare(0,5,"DEBUGY")==0){//判断是否需要DEBUGY,并去除该信息
            debug_dest=true;
            dest=src.substr(5);
        }else
            dest=src;
        
        //替换特殊函数的操作
        string func_str,func_return;
        size_t index,index_end,index_temp;
        
        //替换add_paras处理
        PV_AddParas_T::iterator it;
        if ((&add_paras)!=0) //ly mac
            for(it=add_paras.begin();it!=add_paras.end();++it){
                //对add_parasParam的替换处理.
                for(;(dest.find((*it).first)!=string::npos);)
                {
                    STRING_Helper::strReplace(dest,(*it).first,(*it).second);
                };
            };
        //wrap JSON MARK
        if (dest.find("\\{")!=string::npos){
            json_mark=true;
            STRING_Helper::strReplaceAll(dest,"\\{",JSON_CHG_MARK_BEGNG);
            STRING_Helper::strReplaceAll(dest,"\\}",JSON_CHG_MARK_END);
        }
        
        //替换{***} variable 操作.今后为了提高速度可以跟后面的合并.
        index =index_end=index_temp= 0;
        for(;;){
            index=dest.find("{",index);	index_end=dest.find("}",index);
            
            //函数 or  variable 处理
            if((index!=string::npos)&&(index_end!=string::npos)){
                //判断是否有多层函数处理
                size_t index_son=dest.find("{",index+1);
                if ( (index_son!=string::npos) && ( index_son < index_end)){//如果发现{1 {2  {3  3}  2}  1},其中index_son={2 index_end=2}  |index_son={2 index_end=3}=>{2  {3  3}
                    //再提取下一层；ly change 2014:今后扩充为自动找到对应index_end}
                    size_t index_son_son=dest.find("{",index_son+1);
                    if (index_son_son!=string::npos){
                        index_end=dest.find("}",index_end+1);
                    }
                    
                    //递归提取下一层
                    string son_old = dest.substr(index_son,index_end-index_son+1);//只是最底层处理
                    PV_AddParas_T add_paras_temp;
                    string son_new("");
                    this->ParseValue(son_old,son_new,add_paras_temp);//递归调用ParseValue;
                    STRING_Helper::strReplace(dest,son_old,son_new);
                    
                    //重置index_end
                    index_end=dest.find("}",index);     //此时index_end=1},如果没有应该报错.
                    if (index_end==string::npos){
                        r_str=(string)"{{***}}嵌套错误"+dest;  //这个差错不适合于递归中的嵌套（）
                        break;
                    }
                }
                //普通的 variable 处理和函数处理.
                if((dest.size()>(index+2)) && dest[index+1]=='@' && dest[index_end-1]==')'){
                    //函数处理
                    if(STRING_Helper::strStringCutFrom(dest,func_str,"{",")}",index)==STRING_Helper::FINDED){
                        //取得@***()函数 String ;
                        func_str+=")";
                        
                        //取得函数返回值,并进行替换操作.
                        if (this->ParseValueFunc(func_str,func_return,r_str,add_paras)==""){//ParseValueFunc:返回成功信息.
                            STRING_Helper::strReplace(dest,"{"+func_str+"}",func_return);//!!!今后可在此提高效率,只需在index相关位置替换.
                            index+=func_return.size();
                        }else{
                            break ;//不成功,见r_str返回值.
                        }
                    }
                }else{
                    //普通的 variable 处理
                    int r=STRING_Helper::strStringCutFrom(dest,parse_field,"{","}",index);
                    if (r==STRING_Helper::FINDED){
                        //普通的 String 替换.
                        //const char *p;
                        string s;
                        
                        if (parse_field.find("\\r\\n")!=string::npos){
                            //回车换行符替换.
                            STRING_Helper::strReplace(dest,"{\\r\\n}","\r\n");
                            index+=2;
                            continue;
                        }else if (parse_field.compare(0,2,"0x")==0 || parse_field.compare(0,2,"0X")==0){
                            //16进制转换符.
                            if (!STRING_Helper::CheckNum(parse_field)){
                                r_str=(string)"ParseValue ["+parse_field+"is not hex number!";
                                break;
                            }else{
                                string parse_field_value=STRING_Helper::HBCStrToHBCStr(parse_field,LW_HEX_STR,LW_BINARY_STR,false);
                                STRING_Helper::strReplace(dest,string("{")+parse_field+"}",parse_field_value);
                                index+=parse_field_value.size();
                                continue;
                            }
                        }else if (parse_field.compare(0,2,"0b")==0 || parse_field.compare(0,2,"0B")==0){
                            //2进制转换符.
                            if (!STRING_Helper::CheckNum(parse_field)){
                                r_str=(string)"ParseValue ["+parse_field+"is not hex number!";
                                break;
                            }else{
                                string parse_field_value=STRING_Helper::HBCStrToHBCStr(parse_field,LW_HEX_STR,LW_BIT_STR,false);
                                STRING_Helper::strReplace(dest,string("{")+parse_field+"}",parse_field_value);
                                index+=parse_field_value.size();
                                continue;
                            }
                        }else if (parse_field.compare(0,1,"#")==0 || parse_field.compare(0,3,"H%#")==0 || parse_field.compare(0,3,"h%#")==0){//ly task
                            if(this->_PubObj->TableObjStru!=NULL){//！！！ParseProcess的替换模式，如果替换后的内容和下一个字段一至，可能出现重复替换一处的情况。应该加入一定的偏移量的查找。
                                BYTE *data;
                                BB_SIZE_T data_len;
                                BB_SIZE_T pos=parse_field.compare(0,1,"#")==0 ? 1 : 3;
                                const char *parse_field_p=parse_field.c_str()+pos;
                                if (!BB_GET_TABLE_ITEMS_NAME_DATA_STRU(this->_PubObj->TableObjStru,this->_PubObj->Rec,parse_field_p,&data,&data_len)){
                                    r_str=(string)"ParseValue->BB_GET_TABLE_ITEMS_ID_DATA_STRU get field_value("+parse_field+") error!";
                                    break;
                                }
                                if (parse_field.compare(0,2,"h%")==0){
                                    string s((const char *)data,(UInt64)data_len);
                                    string s_value=STRING_Helper::HBCStrToHBCStr(s,LW_BINARY_STR,LW_HEX_STR,false);
                                    STRING_Helper::strReplace(dest,string("{")+parse_field+"}",s_value);
                                    index+=s_value.size();
                                }else if (parse_field.compare(0,2,"H%")==0){
                                    string s((const char *)data,(UInt64)data_len);
                                    string s_value=STRING_Helper::HBCStrToHBCStr(s,LW_BINARY_STR,LW_HEX_STR,true);
                                    STRING_Helper::strReplace(dest,string("{")+parse_field+"}",s_value);
                                    index+=s_value.size();
                                }else{
                                    STRING_Helper::strReplace(dest,string("{")+parse_field+"}",(const char *)data,(UInt64)data_len);
                                    index+=data_len;
                                }
                                continue;
                            }else{
                                r_str=(string)"variable not exist-"+"{"+parse_field+"}";
                                break;
                            }
                        }else if (parse_field.compare(0,2,"h%")==0){
                            //16进制转换符.
                            if(this->_PubObj->GetNodeValueStr(parse_field.c_str()+2,s)){
                                string s_value=STRING_Helper::HBCStrToHBCStr(s,LW_BINARY_STR,LW_HEX_STR,false);
                                STRING_Helper::strReplace(dest,string("{")+parse_field+"}",s_value);
                                index+=s_value.size();
                                continue;
                            }else{
                                r_str=(string)"variable not exist-"+"{"+parse_field+"}";
                                break;
                            }
                        }else if (parse_field.compare(0,2,"b%")==0 || parse_field.compare(0,2,"B%")==0){
                            //16进制转换符.
                            if(this->_PubObj->GetNodeValueStr(parse_field.c_str()+2,s)){
                                string s_value=STRING_Helper::HBCStrToHBCStr(s,LW_HEX_STR,LW_BINARY_STR,false);
                                STRING_Helper::strReplace(dest,string("{")+parse_field+"}",s_value);
                                index+=s_value.size();
                                continue;
                            }else{
                                r_str=(string)"variable not exist-"+"{"+parse_field+"}";
                                break;
                            }
                        }else if (parse_field.compare(0,2,"H%")==0){
                            //16进制转换符.
                            if(this->_PubObj->GetNodeValueStr(parse_field.c_str()+2,s)){
                                string s_value=STRING_Helper::HBCStrToHBCStr(s,LW_BINARY_STR,LW_HEX_STR,true);
                                STRING_Helper::strReplace(dest,string("{")+parse_field+"}",s_value);
                                index+=s_value.size();
                                continue;
                            }else{
                                r_str=(string)"variable not exist-"+"{"+parse_field+"}";
                                break;
                            }
                        }else if (parse_field.compare(0,2,"J%")==0){
                            //16进制转换符.
                            if(this->_PubObj->GetNodeValueStr(parse_field.c_str()+2,s)){
                                BB_OBJ * src_obj = (BB_OBJ *)s.data();
                                string s_value;
                                if (GB_AvartarDB->BArrayToJsonArray(src_obj, s_value, add_paras)<0){
                                    r_str=(string)"binary can't trans to json-"+"{"+parse_field+"}";
                                    break;
                                }
                                if(s_value.size()==0)
                                    s_value="[]";
                                else
                                    s_value=(string)"["+s_value+"]";
                                STRING_Helper::strReplace(dest,string("{")+parse_field+"}",s_value);
                                index+=s_value.size();
                                continue;
                            }else{
                                r_str=(string)"variable not exist-"+"{"+parse_field+"}";
                                break;
                            }
                        }else if(this->_PubObj->GetObjNodeType(parse_field.c_str())==Http_ParseObj::STR_POINT_OBJ){
                            r_str=(string)"STR_POINT_OBJ  variable can not be used here!";
                            break;
                        }else if(this->_PubObj->GetNodeValueStr(parse_field.c_str(),s)){//普通 node 解析
                            //进行 variable 字段替换操作.
                            STRING_Helper::strReplace(dest,(string)"{"+parse_field+"}",s);
                            index+=s.size();
                            continue;
                        }else{
                            r_str=(string)"variable not exist-"+"{"+parse_field+"}";
                            break;
                            //差错处理.
                        }
                    }
                }
            }else
                break;
        }
    }
    else
        dest="";
    
    //debug 显示
    if (r_str=="" && debug_dest)
        this->DEBUGY(dest);
    else if(r_str!=""){
        dest = "";
        this->SHOW_ERROR(r_str,true);
    }
    //wrap JSON MARK
    if (json_mark){
        STRING_Helper::strReplaceAll(dest,JSON_CHG_MARK_BEGNG,"{");
        STRING_Helper::strReplaceAll(dest,JSON_CHG_MARK_END,"}");
    }
    
    //用于记录解析层次深度.
    return r_str; //ly 0630
}

//src:binary 输入处理
string ParseBase::ParseVector(const char *src,string &dest,PV_AddParas_T &add_paras){
    string debug_str;
    string r_str("");
    bool debug_dest=false;
    //vector<string > dest_arr; //目标string 数组；
    
    //取得待解析 String 值
    if (*src!='\0'){
        string part_str("");
        char *next=0,*start=(char *)src;
        //读取所有[],[]数组内容；
        int r;
        for(;;){
            if (*start=='\0')
                break;
            r=this->_ParseVector_GetNext(start,part_str,next,"[]");
            if ((part_str.size() < 6) && STRING_Helper::CheckNum(part_str)) {
                //补充：过滤数组ARRAY[10]等模式,已经算是最低层级了//ly 910
                string dest_tmp;
                this->ParseValue(src, dest_tmp, add_paras);
                dest.append(dest_tmp);//!!!you can turn fast this place;
                break;
            }
            if (r==1){
                //取得子字串；
                this->ParseVector(part_str.c_str(),dest,add_paras);//如果还有自层次则递归。
            }else if (r==0){
                //如果没有[]
                string dest_tmp;
                this->ParseValue(part_str,dest_tmp,add_paras);//如果末端则解析
                //cout << "ParseVector->ParseValue()"<<dest_tmp.size() << endl;
                dest.append(dest_tmp);//!!!you can turn fast this place;
                break;
            }else if (r==-1){
                //如果语法错误
                r_str=(string)"[[***],[***]]嵌套错误:"+src;  //这个差错不适合于递归中的嵌套（）
                break;
            };
            start=next;
        };
    }
    
    //debug 显示
    if (r_str=="" && debug_dest)
        this->DEBUGY(dest);
    else if(r_str!=""){
        dest = "";
        this->SHOW_ERROR(r_str);
    }
    
    //用于记录解析层次深度.
    return r_str; //ly 0630
}


//取得字符串中第一个[]对应组合,
//src 取得src 以\0结尾
int ParseBase::_ParseVector_GetNext(const char *src, string &dest_str, char* &next,string para){
    char *find, *first_find=NULL,*start=(char *)src;
    dest_str="";
    bool begin=true;
    int i=0;
    
    for(;;){
        //递进[]查找指针；
        if ((find=this->_ParseVector_TempStrFind(start,para.data()))!=0){
            if (*find==para.at(0)){
                i++;//如果发现[
                if (begin)
                    first_find = find;
            }else
                i--;//如果发现]
        }else if (begin){
            //没有发现[]，原版字串内容
            dest_str.append(src);
            next=find;
            return 0;
        }else
            return -2;//ly change???
        begin = false;
        //完成[]截取
        if (i==0){//完整截取[]	
            //发现了子字串。
            dest_str.append(first_find+1,find-first_find-1);
            next = find+1;
            return 1;
        }
        if (i<0){
            //差错情况
            return -1;
        }
        //递进查询
        start=find+1;
    };
}

//如果在src中发现find中的任意字符，则返回最近的字符位置。
char *ParseBase::_ParseVector_TempStrFind(const char *src, const char *find){
    size_t len=strlen(find);
    char *dest=0;
    char *tmp;
    char find_str[2];find_str[1]=0;
    
    if (*src=='\0') 
        return 0;
    for(size_t i=0; i<len; i++){
        find_str[0]=find[i];
        if ((tmp=(char *)strstr(src,(const char *)find_str))!=0){
            if (dest==0 || tmp<dest)
                dest=tmp;
        };
    }
    return dest;
}

string& ParseBase::ParseValueFunc(string &func_str, string &func_return, string &r_str,PV_AddParas_T &add_paras){
    r_str="";
    func_return=(string)"{"+func_str+"}";//ly table_redir;
    return r_str;
}

void ParseBase::DEBUGY(ParseXml_Node this_node){
    this->DEBUGY(this_node,"");
}

void ParseBase::DEBUGY(ParseXml_Node this_node,string act_stat){
    this->DEBUGY(this_node,act_stat.c_str());
}

void ParseBase::DEBUGY(const char *node_value,string act_stat){
    string deep_str="";
    //取得层次深度
    for(int i=0; i< this->ParseDeep; i++)
        deep_str+="  ";
    
    //显示前部信息
    cout << deep_str << node_value <<endl;
    
    //显示后部信息
    if (act_stat.size()!=0)
        WIZBASE_DEBUG((deep_str+"<!--"+act_stat+"-->").c_str());
        //cout<< deep_str << "<!--"<< act_stat << "-->" << endl;
}


void ParseBase::DEBUGY(string act_stat) {
    this->DEBUGY(act_stat.c_str());
}

void ParseBase::DEBUGY(const char *act_stat) {
    if (this->DebugType == ParseBase::DEBUG_NO)
        return;
    string deep_str = "";
    //取得层次深度
    for (int i = 0; i< this->ParseDeep; i++)
        deep_str += "  ";
    
    //显示后部信息
    if (strlen(act_stat) != 0)
        WIZBASE_DEBUG((deep_str+act_stat).c_str());
        //cout << deep_str << act_stat << endl;
}


//用于跟踪解释程序,针对Active.xml的解析过程.
void ParseBase::DEBUGY(ParseXml_Node this_node, const char *act_stat) {
    if (this->DebugType == ParseBase::DEBUG_NO)
        return;
    string deep_str = "";
    for (int i = 0; i<this->ParseDeep; i++) {
        deep_str += "  ";
    }//可以用string(num,c)解决；
    
    if (strlen(this_node.name()) != 0) {
        string node_line;
        WIZBASE_DEBUG((deep_str+this->_ActParseXml->GetNodeLine(this_node, node_line)).c_str());
        //cout << deep_str << this->_ActParseXml->GetNodeLine(this_node, node_line) << endl;
    }
    else {
        string temp = this_node.text().get();
        WIZBASE_DEBUG((deep_str+STRING_Helper::strTrim(temp)).c_str());
        //cout << deep_str << STRING_Helper::strTrim(temp) << endl;
    }
    if (strlen(act_stat) != 0)
        WIZBASE_DEBUG((deep_str+act_stat).c_str());
    //cout << deep_str << "<!--" << act_stat << "-->" << endl;
}

void ParseBase::DEBUGY() {//空跟踪；用于定点调试
    int i=0;
    i++;
}

void ParseBase::SHOW_ERROR(string act_stat, bool set_last_err){
    this->SHOW_ERROR(act_stat.c_str(),set_last_err);
}

void ParseBase::SHOW_ERROR(const char *act_stat, bool set_last_err){
    string deep_str="";
    //取得层次深度
    for(int i=0; i< this->ParseDeep; i++)
        deep_str+="  ";
    
    //显示后部信息
    if (strlen(act_stat)!=0)
        WIZBASE_SHOW_ERROR((deep_str+"<!--" +"E:"+act_stat+"-->").c_str());
    //cout<< deep_str << "<!--" <<"E:" << act_stat << "-->" << endl;
    if (set_last_err)
        this->ErrorStr = act_stat;
}

void ParseBase::CLEAR_LAST_ERROR() {
    string tmp = this->ErrorStr;
    this->ErrorStr = "";
}

string ParseBase::GET_LAST_ERROR() {
    string tmp = this->ErrorStr;
    this->ErrorStr = "";
    return tmp;
}

bool ParseBase::GetLastError(string &err_str) {
    if (this->ErrorStr.size() == 0)
        return false;
    else{
        err_str = this->ErrorStr;
        this->ErrorStr = "";
        return true;
    }
}

void ParseBase::SET_LAST_ERROR_STR(string str) {
    this->ErrorStr = str;
}
