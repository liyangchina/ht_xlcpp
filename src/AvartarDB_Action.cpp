/*
 * AvartarDBAction.cpp
 *
 *  Created on: Jul 26, 2017
 *      Author: root
 */
#include "STRING_Helper.h"
#if !defined(__ANDROID__) && !defined(__APPLE__)
#include "WizBase.h"
#endif
#include "AvartarDB_Action.h"
#include "stdio.h"
#include "string.h"
#include "WizBase.h"
#define AvartarDB_Base_SaveExt_CreateDir

//#include "WizBase.h"
/*#ifdef  __cplusplus
 extern "C" {
 #endif
	extern bool WIZBASE_pub_obj_send_msg(const char *msg, int msg_len,const char *dest_obj);
 #ifdef  __cplusplus
 }
 #endif*/

/*AvartarDB_Action::~AvartarDB_Action() {
	// TODO Auto-generated destructor stub
 }*/


bool AvartarDB_Action::AvartarDB_Init(){
    this->SortObj = new AvartarSortObj(); //ly sort;
    //初始化数据库结构
    DBTypes.insert(make_pair("NULL",BB_NULL));
    DBTypes.insert(make_pair("UINT8",BB_UINT8));
    DBTypes.insert(make_pair("UINT16",BB_UINT16));
    DBTypes.insert(make_pair("UINT32",BB_UINT32));
    DBTypes.insert(make_pair("UINT64",BB_UINT64));
    DBTypes.insert(make_pair("MALLOC",BB_MALLOC));
    DBTypes.insert(make_pair("REDIR",BB_REDIR));
    DBTypes.insert(make_pair("OBJ",BB_NULL));
    DBTypes.insert(make_pair("ARRAY",BB_ARRAY));
    DBTypes.insert(make_pair("TABLE",BB_TABLE));
    DBTypes.insert(make_pair("BYTES",BB_BYTES));
    DBTypes.insert(make_pair("BYTES_EXT",BB_BYTES_EXT));
    DBTypes.insert(make_pair("OBJECT",BB_NULL));
//    //
//    QFlags.insert(make_pair("READ", ACTION_READ));
//    QFlags.insert(make_pair("WRITE", ACTION_WRITE));
//    QFlags.insert(make_pair("LOGIN", ACTION_LOGIN));
//    QFlags.insert(make_pair("CHGPWD", ACTION_LOGIN));
//    QFlags.insert(make_pair("BITIDS", BB_TABLE_REC_WR_BITIDS));
//    QFlags.insert(make_pair("COLUMN_IDS", BB_TABLE_REC_WR_BITIDS));
//    QFlags.insert(make_pair("BITMAP", BB_TABLE_REC_WR_BITMAP));
//    QFlags.insert(make_pair("BITSTARTEND", BB_TABLE_REC_WR_BITSTARTEND));
//    QFlags.insert(make_pair("BITMAP_CONVERT", 0));
//    QFlags.insert(make_pair("CONDITIONS", BB_TABLE_REC_WR_QUERY_CONDITIONS));
//    QFlags.insert(make_pair("CURSOR", BB_TABLE_REC_WR_QUERY_CURSOR));
//    QFlags.insert(make_pair("CONVERT", BB_TABLE_REC_WR_BITIDS_CONVERT));
//    //"ACT|TID""READ",0x00000001],["PATH|BITMAP|CONDITIONS|CONVERT"
//    QFlags.insert(make_pair("PATH", 0));
//    QFlags.insert(make_pair("NODEID", 0));
//    QFlags.insert(make_pair("CONTENT", 0));
    //SystemInfo
    /*SysInfoFlags.insert(make_pair("HTTP_GET", SYS_INFO_NAME_HTTP_GET));
     SysInfoFlags.insert(make_pair("GET", SYS_INFO_NAME_GET));
     SysInfoFlags.insert(make_pair("SEARCH", SYS_INFO_NAME_SEARCH));
     SysInfoFlags.insert(make_pair("GET_CLUSTER_INFO", SYS_INFO_NAME_GET_CLUSTER_INFO));
     SysInfoFlags.insert(make_pair("SKHD", SYS_INFO_NAME_SKHD));
     SysInfoFlags.insert(make_pair("REGISTER", SYS_INFO_NAME_REGISTER));
     SysInfoFlags.insert(make_pair("RECEIVE_ALL", SYS_INFO_NAME_RECEIVE_ALL));
     SysInfoFlags.insert(make_pair("EVENT", SYS_INFO_NAME_EVENT));
     SysInfoFlags.insert(make_pair("HARTBIT", SYS_INFO_NAME_HARTBIT));*/
    
    
    this->SysInfoTableStru.ThisObj = NULL;    //标志未初始化
    this->SysQueueTableStru.ThisObj =NULL ;
    
    //取得数据库路径值
    this->ParseDeep=0;
    this->DebugType = DEBUG_NORMAL;
    
    this->CreateDir(this->BaseSavePath);
    
    this->rootObj=this->AvartarNewObj("/DB_Service");
//    this->DoRegTable_Rec = 0;
    
    return true;
}
AvartarDB_Action::AvartarDB_Action(string xml_filename){
    this->Loaded=false;
    this->ErrorStr="";
    
    //初始化解析文档对象
    this->_ActParseXml = new Http_ParseXML();
    int r=this->_ActParseXml->LoadFile(xml_filename.c_str());
    if (r==Http_ParseXML::LOAD_XML_ERROR){
        this->ErrorStr=(string)"AvartarDBManager(" +xml_filename +")";
        return;
    }
    //初始化数据库存储路径
    //size_t r1,r2;
    //if ((r1=xml_filename.find("/"))==0 && (r2=xml_filename.find("/"))==r1){
    //   this->BaseSavePath=xml_filename.substr(0,r2);
    //}else
    this->BaseSavePath="./AVT_DB";
    //初始化其他
    if (this->AvartarDB_Init())
        this->Loaded=true;
}

AvartarDB_Action::AvartarDB_Action(char *xml,int filename_xml, string db_save_path){
    this->Loaded=false;
    this->ErrorStr="";
    
    //初始化解析文档对象
    if (filename_xml==LOAD_XML_FILENAME){
        this->_ActParseXml = new Http_ParseXML();
        //cout << string("Loading:")+xml << endl;
        int r=this->_ActParseXml->LoadFile(xml);
        if (r==Http_ParseXML::LOAD_XML_ERROR){
            this->ErrorStr=(string)"AvartarDBManager("+xml+") file load error:"+this->_ActParseXml->ErrorStr;
            return;
        }
    } else{
        this->_ActParseXml = new Http_ParseXML();
        //cout << string("Loading: xml content...")<< endl;
        int r=this->_ActParseXml->Load(xml);  //ly mac 没有UTF8校验，需要今后补充
        if (r==Http_ParseXML::LOAD_XML_ERROR){
            this->ErrorStr=(string)"AvartarDBManager() load xml error:"+this->_ActParseXml->ErrorStr;
            return;
        }
    }
    
    //初始化数据库存储路径
    //if (db_save_path==NULL || db_save_path==0)
    if (db_save_path.size()==0)
        this->BaseSavePath="./AVT_DB";
    else
        this->BaseSavePath=db_save_path;
    
    //初始化其他
    if (this->AvartarDB_Init())
        this->Loaded = true;   //mark successed
}


AvartarDB_Action::~AvartarDB_Action(){
    if (this->_ActParseXml!=0)
        delete this->_ActParseXml;            //释放对象
    
    if (this->SortObj)
        delete this->SortObj;
    //Xml的malloc部分层层删除
}

BB_OBJ *AvartarDB_Action::AvartarNewObj(string parse_path){
    string content("");
    return this->AvartarNewObj(parse_path,content);
}

BB_OBJ *AvartarDB_Action::AvartarNewObj(string parse_path,string &content){
    string data="";
    BB_SIZE_T max_len=0;
    BYTE open_mode=0;
    size_t ipos1,ipos2;
    BYTE type;
    const char *obj_type,*check_point=NULL,*obj_value=NULL;
    bool is_attr_node=false;
    BYTE cell_type=BB_NULL;
    BB_OBJ_STRU cell_stru;
    BB_OBJ *cell,*index,*add_info;
    cell=index=add_info=NULL;
    PV_AddParas_T add_paras;//???parse_value change;
    string obj_value_str("");
    string sort_field_str("");
    string save_ext("");
    
    //查询节点是否存在
    ParseXml_Node parse_node = this->_ActParseXml->GetNodeByXpath(parse_path.c_str());
    if (!parse_node && ( (ipos1=parse_path.rfind("/_")) != string::npos) && ((ipos2=parse_path.rfind("/"))==ipos1)){
        //查询是否有属性节点
        ParseXml_Node tmp_parse_node=this->_ActParseXml->GetNodeByXpath(parse_path.substr(0,ipos1).c_str());
        obj_type=this->_ActParseXml->NodeAttr(tmp_parse_node,parse_path.substr(ipos1+2).c_str());
        if(obj_type!=0 && strlen(obj_type)>0){
            cell=index=add_info=BB_NULL;
            obj_value=NULL;
            is_attr_node=true;
        }
    }
    
    //解析节点
    if (parse_node || is_attr_node){//如果是正确的节点
        BB_OBJ * r_obj=NULL;
        if (!is_attr_node){//对于普通节点，读取节点信息
            this->DEBUGY(parse_node);
            this->ParseDeep++; //用于记录解析层次深度.tool = tool.next_sibling()
            if (!this->_AvartarNewObj_GetAttr(&r_obj,parse_node,parse_path,&check_point,&obj_type,open_mode,max_len) || !this->AvartarNewObj_CheckType(obj_type,type)){//取得当前节点的各个属性
                r_obj=NULL; goto end;
            }
            if (type==BB_ARRAY || type==BB_TABLE){//普通一维节点
                //取得cell
                cell = this->AvartarNewObj(parse_path+"/_Cell");//取得cell节点
                cell_type=BB_GET_OBJ_TYPE(cell);
                //ly table_dir
                if (cell_type==BB_MALLOC)
                    this->CreateDir(this->_AvartarNewObj_GetSonNodeObjs_GetSavePath(parse_path,0));//ly dir
                //取得index
                string son_node_array_objs;
                if ((type==BB_ARRAY && (cell_type==BB_BYTES || cell_type==BB_BYTES_EXT)) && this->_AvartarNewObj_GetSonNodeObjs(parse_node,parse_path,cell_type,son_node_array_objs,DBCONFIG_NODE_NAME)){
                    index = this->AvartarNewObj(parse_path+"/_Index",son_node_array_objs);//对于KeyValu类的Index初始化索引
                }else
                    index = this->AvartarNewObj(parse_path+"/_Index");//取得index节点
                //取得addinfo
                add_info = this->AvartarNewObj(parse_path+"/_AddInfo");//取得AddInfo节点
            }else{//零维节点
                cell=index=add_info=BB_NULL;
            }
            //取得内容
            obj_value=NULL;
            if (content.size()==0){
                ParseXml_Node value_node = this->_ActParseXml->GetNodeByXpath((parse_path+"/_Content").c_str());
                obj_value=this->_ActParseXml->NodeValue(value_node ? value_node :parse_node);//取得_Content字节点或者当前节点值.
            }
        }else{
            //对于属性节点,只允许0维cell节点。
            if (this->AvartarNewObj_CheckType(obj_type,type)){
                if (type==BB_ARRAY || type== BB_TABLE){
                    this->SHOW_ERROR((string)"cell 或index 属性节点不能定义数组(ARRAY)和表格(TABLE)");
                    r_obj=NULL; goto end;
                }
            }else {
                r_obj=NULL; goto end;
            }
        }
        
        //Debug
        if (check_point!=NULL && (strstr(check_point,"true")==check_point)){
            //如果没有定义type
            this->DEBUGY((string)"This is checkpoint!");
        }
        
        //Header 多层_Cell _Index This对象处理
        //如果是Cell(0Dim)
        UInt64 obj_value_int;
        if (obj_value && strlen(obj_value)!=0)
            obj_value_str=obj_value;
        
        switch(type){
            case BB_ARRAY:
                //如果是数组对象
                if (index==NULL){
                    this->SHOW_ERROR((string)"ARRAY对象需要定义cell, index属性！");
                    r_obj=NULL; break;
                }
                switch(cell_type){
                    case BB_ARRAY:
                        this->SHOW_ERROR((string)"ARRAY对象Cell属性，不能再是ARRAY");
                        r_obj=NULL;
                        break;
                    case BB_NULL:
                        //如果是普通对象数组
                        this->_AvartarNewObj_GetSonNodeObjs(parse_node,parse_path,cell_type,data,DBCONFIG_NODE_OBJECT_COPY);//创建对象，并拷贝到data中。
                        //创建MALLOC对象数组
                        if (cell == NULL) {
                            this->SHOW_ERROR((string)"ARRAY对象Cell属性，不能是BB_NULL");
                            r_obj = NULL;
                        }else
                            r_obj = BB_NEW_ARRAY(cell,index,BB_NULL,max_len,data.data(),data.size(),NULL);  //malloc模式 创建并返回对象指针
                        break;
                    case BB_MALLOC:
                        //此处加入目录创建
                        //this->CreateDir(this->_AvartarNewObj_GetSonNodeObjs_GetSavePath(parse_path,0));//ly dir //ly table_dir
                        //如果是MALLOC对象数组
                        if (BB_GET_OBJ_TYPE(index)==BB_TABLE){//ly table_dir
                            BB_OBJ_STRU index_stru=BB_GET_OBJ_STRU(index);
                            this->_AvartarNewObj_GetIndexTableObjs(index_stru,data);//创建对象，并拷贝地址，保留空间 
                        }
                        
                        this->_AvartarNewObj_GetSonNodeObjs(parse_node,parse_path,cell_type,data,DBCONFIG_NODE_OBJECT_MALLOC);//创建对象，并拷贝地址，保留空间
                        r_obj = BB_NEW_ARRAY(cell, index, add_info, max_len, data.data(), data.size(), NULL);  //malloc模式 创建并返回对象指针
                        //r_obj = BB_NEW_ARRAY(cell,index,BB_NULL,max_len,data.data(),data.size(),NULL);  //malloc模式 创建并返回对象指针
                        break;
                    case BB_UINT8:      case BB_UINT16:        case BB_UINT32:        case BB_UINT64:
                        //如果是普通数组ARRAY[UINT8-64|REDIR,?]
                        cell_stru=BB_GET_OBJ_STRU(cell);
                        if (strlen(obj_value)!=0 && (this->strArrayToBinary(data,obj_value,cell_stru,add_paras)==-1)){
                            this->SHOW_ERROR((string)"ARRAY's all item must be number! error string is:"+obj_value);
                            r_obj=NULL;            goto end;
                        }
                        r_obj = BB_NEW_ARRAY(cell,index,add_info,max_len,data.data(),data.size(),NULL);  //malloc模式 创建并返回对象
                        break;
                    case BB_BYTES:
                    case BB_BYTES_EXT:
                        //如果是短字符串数组
                        BYTE index_type=BB_GET_OBJ_TYPE(index);//BB_OBJ_STRU index_stru=BB_GET_OBJ_STRU(index);
                        cell_stru=BB_GET_OBJ_STRU(cell);
                        if (BB_IS_UINT(index)){//如果index type是cell_uint
                            //如果是普通字符串数组
                            //取得字符串值
                            if (obj_value && strlen(obj_value)){//如果存在在“\_CONTENT”节点预设（字串）;
                                int i;
                                if ((i=this->strArrayToBinary(data,obj_value,cell_stru,add_paras))==-1){
                                    //this->SHOW_ERROR((string)"NewObj->strArrayToBinary() error!"+obj_value);//内部已经有足够的提示
                                    r_obj=NULL;            goto end;
                                }
                                BB_SET_UINT_VALUE(index,i);
                            }else if (content.size()!=0){//如果是KeyValue模式（Key）
                                data.append(content);//如果存在上层节点预设（二进制），则预设
                            }
                            //创建BYTES_OBJ对象;
                            r_obj = BB_NEW_ARRAY(cell,index,BB_NULL,max_len,data.data(),data.size(),NULL); //创建并返回对象
                            break;
                        }else if(index_type==BB_ARRAY){
                            //如果是KeyValue模式(Value)
                            this->_AvartarNewObj_GetSonNodeObjs(parse_node,parse_path,cell_type,data,DBCONFIG_NODE_VALUE);//循环取得字节值点
                            r_obj = BB_NEW_ARRAY(cell,index,BB_NULL,max_len,data.data(),data.size(),NULL); //创建并返回对象
                            break;
                        }else{
                            this->SHOW_ERROR((string)"ARRAY_BTYES Cell Index 设置错误");
                            r_obj=NULL; break;
                        }
                }
                break;
            case BB_TABLE:
                //如果是表格对象
                //判断是否符合表格要素
                if (cell==NULL || index==NULL){
                    this->SHOW_ERROR((string)"TABLE对象需要定义Cell和Index不能为NULL");
                    r_obj=NULL; break;
                }
                cell_stru=BB_GET_OBJ_STRU(cell);
                if (BB_GET_OBJ_DIM(cell) != 1 || (cell_stru.Type==BB_ARRAY && !BB_IS_UINT(cell_stru.CellObj))) {
                    this->SHOW_ERROR((string)"TABLE对象需要定义Cell(必须是ARRAY_UINT) or TABLE！");
                    r_obj = NULL; break;
                }
                if    (!(BB_IS_UINT(index) || BB_GET_OBJ_TYPE(index) == BB_ARRAY)) {
                    this->SHOW_ERROR((string)"TABLE对象需要定义index(UINT*|ARRAY)属性！");
                    r_obj = NULL; break;
                }
                //创建表格对象
                if (obj_value!=0 && strlen(obj_value)!=0){
                    if (this->strTableToBinary(data,obj_value_str,cell_stru,add_paras)==-1){
                        //this->SHOW_ERROR((string)"TABLE对象需要定义Cell/Content初始化错误！");//内部已有足够的差错提示
                        r_obj=NULL; break;
                    };//如果有value
                }
                BYTE *obj_buff;
                BB_SIZE_T obj_buff_size;
                BB_OBJ_STRU r_obj_stru;
                if (add_info!=NULL && open_mode!=FileQueue::OPEN_MODE_TRUNC && this->LoadDB(add_info,&obj_buff, obj_buff_size,open_mode)){
                    r_obj = obj_buff;  //malloc模式 创建并返回对象
                    r_obj_stru=BB_GET_OBJ_STRU(r_obj);
                }else{
                    string test;
                    r_obj = BB_NEW_ARRAY(cell,index,add_info,max_len,(BYTE *)data.data(),data.size(),NULL);  //malloc模式 创建并返回对象
                    //存盘驱动
                    r_obj_stru=BB_GET_OBJ_STRU(r_obj);
                    if (r_obj_stru.AddInfoObj!=NULL && this->GetAddInfo(r_obj_stru.AddInfoObj,AVARTDB_SAVE_FILE_NAME).size()!=0 && AVARTDB_DO_INIT_SAVE_DB){//ly dir
                        this->SaveDB(r_obj,FileQueue::OPEN_MODE_TRUNC);
                    }
                }
                //创建索引 //ly sort
                if (r_obj_stru.AddInfoObj != NULL && (sort_field_str=this->GetAddInfo(r_obj_stru.AddInfoObj, AVARTDB_SORT_FIELDS_NAME)).size()!=0) {//ly sort
                    for(size_t i=0; i<sort_field_str.size(); i++)
                        this->AvartarNewObj_SortObjFieldInit(r_obj,sort_field_str.at(i));//一个BYTE 代表一个field
                }
                if (r_obj_stru.AddInfoObj != NULL && (save_ext=this->GetAddInfo(r_obj_stru.AddInfoObj, AVARTDB_SAVE_MALLOC_EXT)).size()!=0) {//ly sort
                    this->AvartarNewObj_LoadExtField(r_obj,save_ext);//ly malloc2.1
                }
                
                break;
            case BB_UINT8:            case BB_UINT16:            case BB_UINT32:case BB_UINT64:
                BB_SIZE_T tmp_size;
                BB_UINT_STRU r_stru;
                //临时创建数字对象
                obj_value_int=0;
                if (obj_value!=0 && strlen(obj_value)!=0){
                    //取得元对象值
                    STRING_Helper::strTrim(obj_value_str);
                    if (!STRING_Helper::CheckNum(obj_value_str)){
                        this->SHOW_ERROR((string)"节点内容必须是数字");
                        r_obj=NULL;
                        break;
                    }
                    obj_value_int=STRING_Helper::ParseInt(obj_value_str);
                }
                //r_obj = BB_NEW_CELL(type,(const void *)&obj_value_int,sizeof(obj_value_int),0);
                r_stru=BB_NEW_CELL_UINT(obj_value_int,type);//空值
                
                //开内存，拷贝创建具体对象
                tmp_size=BB_GET_OBJ_SIZE((BB_OBJ *)&r_stru);
                r_obj = (BB_OBJ *)malloc(tmp_size);
                memcpy((void*)r_obj,(const void *)&r_stru,tmp_size);
                break;
            case BB_MALLOC: {//ly malloc new; ly task
                if (obj_value != 0 && strlen(obj_value) != 0)
                    r_obj = BB_NEW_MALLOC(NULL, obj_value, strlen(obj_value), 0, NULL); //obj_value为,预设路径名成或类型
                else
                    r_obj = BB_NEW_MALLOC(NULL, NULL, 0, 0, NULL);//无路径malloc
                break;
            }
            case BB_BYTES:
            case BB_BYTES_EXT:
                if (obj_value!=0 && strlen(obj_value)!=0){
                    string tmp;
                    if (!this->strCellToBinary(tmp,obj_value_str,type,obj_value_str.size(),add_paras)){
                        this->SHOW_ERROR((string)"BB_BYTES|BB_BYTES_EXT 解析错误！");
                        r_obj=NULL; break;
                    }
                    r_obj=BB_NEW_CELL(type,tmp.data(),tmp.size(),NULL);
                    break;
                }else{
                    r_obj=BB_NEW_CELL(type,data.data(),data.size(),NULL);//空字串
                    break;
                }
            case BB_NULL:
            default:
                r_obj = (BB_OBJ *)malloc(1);
                (*r_obj)=BB_NULL; break;
        }
    end:
        if (cell!=NULL)
            free(cell);
        if (index!=NULL)
            free(index);
        if (add_info!=NULL)
            free(add_info);
        if (!is_attr_node)
            this->ParseDeep--; //用于记录解析层次深度.tool = tool.next_sibling()
        return r_obj;
    }else
        return NULL;
}

bool AvartarDB_Action::AvartarNewObj_CheckType(const char *obj_type,BYTE &type){
    if ((DBTypes.find(obj_type))!=DBTypes.end()){
        type = DBTypes.find(obj_type)->second;
        return true;
    }else{
        this->SHOW_ERROR((string)"数据定义节Type属性不可理解！");
        return false;
    }
}

bool AvartarDB_Action::_AvartarNewObj_GetAttr(BB_OBJ **r_obj,ParseXml_Node &parse_node, string &parse_path, const char **check_point,const char **obj_type,    BYTE &open_mode,BB_SIZE_T &max_len){
    //取得是否CheckPoint
    if (!parse_node)
        return false;
    *check_point=this->_ActParseXml->NodeAttr(parse_node,"CheckPoint");
    //if (*check_point!=0 && (strstr(*check_point,"true")==*check_point)){
    //    //如果没有定义type
    //    this->DEBUGY((string)"This is checkpoint!");
    //}
    
    //取得对象type配置属性
    *obj_type=this->_ActParseXml->NodeAttr(parse_node,"Type");
    if (*obj_type==0 || strlen(*obj_type)==0){
        //如果没有定义type
        this->SHOW_ERROR((string)"数据定义节点和'_Cell','_Index'节点必须定义Type属性！");
        return false;
    }
    
    //取得对象Open属性
    const char *open_mode_str =this->_ActParseXml->NodeAttr(parse_node,"OpenMode");
    if (open_mode_str!=0 && strlen(open_mode_str)!=0){
        if (strstr(open_mode_str,"TRUNC")==open_mode_str)
            open_mode=FileQueue::OPEN_MODE_TRUNC;
    }
    
    //创建 max_len_obj对象
    const char *max_len_s=this->_ActParseXml->NodeAttr(parse_node,"MaxLen");
    max_len=0;
    if (max_len_s!=0 && strlen(max_len_s)!=0){
        string max_len_str=max_len_s;
        STRING_Helper::strTrim(max_len_str);
        if (!STRING_Helper::CheckNum(max_len_str)){
            this->SHOW_ERROR((string)"Attribute MaxLen must be number！");
            return false;
        }
        max_len=STRING_Helper::ParseInt(max_len_str);
    }
    //BB_UINT_STRU max_len_obj=BB_NEW_CELL_UINT((UINT64)max_len,BB_GET_ARRAY_INDEX_WIDTH(index)*8);//今后改为直接读取IndexObj的cell_width
    
    return true;
}

bool AvartarDB_Action::_AvartarNewObj_GetSonNodeObjs(ParseXml_Node &parse_node,string &parse_path, BYTE cell_type,string &dest,BYTE method){
    //取得是否CheckPoint
    bool is_success=false;
    const char *value;
    BB_OBJ *son_obj_p=NULL;
    string son_obj_array("");
    BYTE type;
    if (!parse_node)
        return false;
    
    PV_AddParas_T add_paras;
    string obj_save_path = this->_AvartarNewObj_GetSonNodeObjs_GetSavePath(parse_path, 2);    //取得主对象存盘路径，addinfo节点需要上行两级
    add_paras.insert(ParseBase::PV_AddParas_T::value_type(string("{@_THIS_PATH}"), obj_save_path));
    string obj_path_xml = this->_AvartarNewObj_GetSonNodeObjs_GetSavePath(parse_path, ACTION_GET_NODE_PATH_OBJ_NAME);
    add_paras.insert(ParseBase::PV_AddParas_T::value_type(string("{@_THIS_OBJ_XML_PATH}"), obj_path_xml)); //ly table_dir
    string obj_path = this->_AvartarNewObj_GetSonNodeObjs_GetSavePath(parse_path, 1);
    string obj_name = obj_path.substr(obj_path.rfind("/") + 1); //取得主对象名称;
    add_paras.insert(ParseBase::PV_AddParas_T::value_type(string("{@_THIS_OBJ_NAME}"), obj_name));//ly tree
    for (ParseXml_Node tool = this->_ActParseXml->FirstChild(parse_node); tool; tool = this->_ActParseXml->NextChild(tool)){
        //只取得非属性节点。
        const char *name=this->_ActParseXml->NodeName(tool);
        if (strstr(name,"_")==name)
            continue;
        const char *type_str=this->_ActParseXml->NodeAttr(tool,"Type");
        
        switch (method){
            case DBCONFIG_NODE_VALUE://主要是AddInfo的子节点处理
                if ((value=this->_ActParseXml->NodeValue(tool))!=0 && strlen(value)!=0){
                    //ParseValue 补充处理,ly dir
                    string value_dest,value_str(value);
                    if (this->ParseValue(value_str,value_dest,add_paras).size()!=0){//注 AddInfo中可能有{@FUNC()},AvartarDB_Action->只是ParseBase对象，因此ParseValueFunc()->{@FUNC()}操作将保留。
                        this->SHOW_ERROR((string)"_AvartarNewObj_GetSonNodeObjs(), ParseValue Error!");
                        return false;
                    }
                    //ly ???test //ly table_filter
                    //if (STRING_Helper::PUB_FindStr(value_str,"{@_THIS_OBJ_XML_PATH}")>0)
                        //cout << parse_path << "@" << name << "/" <<"{@_THIS_OBJ_XML_PATH}" << value_dest << endl;
                    value=(char *)value_dest.data();
                    //
                    if ((type_str!=0 && strlen(type_str)!=0) && this->AvartarNewObj_CheckType(type_str,type)){
                        if (type==BB_BYTES_EXT && cell_type==BB_BYTES){//BB_BYTES_EXT不能在BB_BYTES中定义，把ARRAY 改为BB_BYTES_EXT ARRAY 并且注意设定全长度
                            this->SHOW_ERROR((string)"_AvartarNewObj_GetSonNodeObjs(), BB_BYTES_EXT object not allow in ARRAY_BYTES!");
                            return false;
                        }
                        if (type==BB_UINT8 ||type==BB_UINT16 ||type==BB_UINT32 ||type==BB_UINT64){
                            if (!STRING_Helper::CheckNum(value_str)){
                                this->SHOW_ERROR((string)"_AvartarNewObj_GetSonNodeObjs("+name+") is not number!");
                                return false;
                            }
                            UInt64 value_num=STRING_Helper::ParseULong(value_str);
                            BB_UINT_STRU obj_stru=BB_NEW_CELL_UINT(value_num,type);
                            son_obj_p = (BB_OBJ *)&obj_stru;
                            dest.append((const char *)son_obj_p,BB_GET_OBJ_SIZE(son_obj_p));
                            break;
                            //son_obj_p= BB_NEW_CELL(type,(const void *)&value_num,sizeof(value_num),NULL);//ly array_obj
                        }else if (type==BB_ARRAY){
                            string value_src = (string)"%B"+value_dest;
                            string value_arr;
                            PV_AddParas_T add_paras;
                            //转换成二进制流数组
                            if (this->JsonBArrayToBArray(value_src, value_arr, add_paras) < 0) {//ly err_block
                                this->SHOW_ERROR((string)"_AvartarNewObj_GetSonNodeObjs("+name+") load array error!");
                                return false;
                            }
                            son_obj_p= BB_NEW_CELL(cell_type,(const void *)(value_arr.data()),value_arr.size(),NULL);
                        }else
                            son_obj_p= BB_NEW_CELL(type,(const void *)value,value_dest.size(),NULL);//ly array_obj //ly rec_chg_mark BB_NEW_CELL(type,(const void *)value,strlen(value),NULL)
                    }else{
                        if (cell_type==BB_BYTES_EXT)
                            son_obj_p= BB_NEW_CELL(cell_type,(const void *)value,value_dest.size(),NULL);//注：不需要创建内存???如何处理BYTES_EXT,BB_NEW_CELL(data_len必须改进UInt32); //ly rec_chg_mark BB_NEW_CELL(type,(const void *)value,strlen(value),NULL)
                        else
                            son_obj_p= BB_NEW_CELL(cell_type,(const void *)value,value_dest.size(),NULL);//注：不需要创建内存???如何处理BYTES_EXT,BB_NEW_CELL(data_len必须改进UInt32);//ly rec_chg_mark BB_NEW_CELL(type,(const void *)value,strlen(value),NULL)
                    }
                    //直接对象实体拷贝
                    dest.append((const char *)son_obj_p,BB_GET_OBJ_SIZE(son_obj_p));
                    if (son_obj_p)
                        free(son_obj_p);//清除源实体对象；
                }
                break;
            case DBCONFIG_NODE_NAME:
                if ((name=this->_ActParseXml->NodeName(tool))!=0 && strlen(name)!=0){
                    son_obj_p = BB_NEW_CELL(BB_BYTES,(const void *)name,strlen(name),NULL);//注：不需要创建内存???如何处理BYTES_EXT,BB_NEW_CELL(data_len必须改进UInt32)
                    //直接对象实体拷贝
                    dest.append((const char *)son_obj_p,BB_GET_OBJ_SIZE(son_obj_p));
                    //dest.append(son_obj_array);
                    if (son_obj_p)
                        free(son_obj_p);//清除源实体对象；
                }
                break;
            case DBCONFIG_NODE_OBJECT_COPY:
                if ((son_obj_p = this->AvartarNewObj(parse_path+"/"+name))!=NULL){
                    dest.append((const char *)son_obj_p,BB_GET_OBJ_SIZE(son_obj_p));//直接对象实体拷贝
                    if (son_obj_p)
                        free(son_obj_p);//清除源实体对象；
                }
                break;
            case DBCONFIG_NODE_OBJECT_MALLOC:
                BB_OBJ *son_obj_p = this->AvartarNewObj(parse_path+"/"+name);  //??是否清楚内存空间　//ly malloc !!! 创建＋应该改为增加MALLOC_EXT处理
                
                //对象指针拷贝
                dest.append((const char *)&son_obj_p,sizeof(son_obj_p));//只拷贝指针，对象保留;!!!但也需要在退出时清除对象???
        }
        is_success=true;
    }
    add_paras.erase("{@_THIS_PATH}");
    add_paras.erase("{@_THIS_OBJ_NAME}");  //ly tree;
    add_paras.erase("{@_THIS_OBJ_XML_PATH}");  //ly tree;
    
    return is_success;
}

//取得全局目录路径
string AvartarDB_Action::_AvartarNewObj_GetSonNodeObjs_GetSavePath(string parse_path,int n){//ly dir
    
    string tmp_path=parse_path.substr(strlen("/DB_Service"));    //this->BaseSavePath，初始化之后，可以根据不同的情况动态改变。
    string tmp_path2;
    string save_path;
    if (n==0)
        save_path=this->BaseSavePath+tmp_path;
    else if (n==1)
        save_path=this->BaseSavePath+tmp_path.substr(0,tmp_path.rfind("/"));
    else if (n==2){//n上行级数
        tmp_path2=tmp_path.substr(0,tmp_path.rfind("/"));
        save_path=this->BaseSavePath+tmp_path2.substr(0,tmp_path2.rfind("/"));
    }else if (n==ACTION_GET_NODE_PATH_OBJ_NAME){
        tmp_path2=parse_path;
        for (;;){
            string this_obj=tmp_path2.substr(tmp_path2.rfind("/") + 1);
            if (this_obj.at(0)!='_')
                break;
            tmp_path2=tmp_path2.substr(0,tmp_path2.rfind("/"));
            if (tmp_path2.size()==0)
                 break;
        }
        save_path=tmp_path2;
    }
    return save_path;
}


bool AvartarDB_Action::_AvartarNewObj_GetIndexTableObjs(BB_OBJ_STRU &index_stru,string &data){//ly table_dir
    //取得是否CheckPoint
    bool is_success=false;
    string son_obj_array("");
    if (index_stru.Type!=BB_TABLE){
        this->SHOW_ERROR((string)"_AvartarNewObj_GetIndexTableObjs()->IndexRow->only support index_table!");
        return false;
    }
    
    //克隆源节点
    string index_table_dir_model=this->GetAddInfo(index_stru.AddInfoObj, "INDEX_TABLE_DIR_MODEL");
    string index_table_dir=this->GetAddInfo(index_stru.AddInfoObj, "INDEX_TABLE_DIR");
    if (index_table_dir.size()==0 || index_table_dir_model.size()==0)
        return false;
    
    //循环添加子对象
    for (BB_SIZE_T num=0; num<index_stru.Num;num++){
        //对象指针拷贝
        BB_OBJ *son_obj_p=this->AvartarNewObj_GetIndexTableObjs_AppendNode(index_stru,num,index_table_dir_model,index_table_dir);
        if (son_obj_p!=NULL)
            data.append((const char *)&son_obj_p,sizeof(son_obj_p));//只拷贝指针，对象保留;!!!但也需要在退出时清除对象???
        else{
            this->SHOW_ERROR((string)"AvartarNewObj_GetIndexTableObjs()->AppendNode() Error!");
            break;
        }
        is_success=true;
    }
    //调试使用：this->_ActParseXml->SaveFile("UTF-8"); //待扩充。
    
    return is_success;
}

BB_OBJ *AvartarDB_Action::AvartarNewObj_GetIndexTableObjs_AppendNode(BB_OBJ_STRU &index_stru,UInt64 num,string &index_table_dir_model,string &index_table_dir,bool xml_remain){//ly table_dir
    //克隆源节点
    ParseXml_Node index_src_node=this->_ActParseXml->GetNodeByXpath(index_table_dir_model.c_str());
    ParseXml_Node index_dest_node=this->_ActParseXml->GetNodeByXpath(index_table_dir.c_str());
    if (!index_src_node ){
        this->SHOW_ERROR((string)"GetIndexTableObjs_AppendNode()->AddInfo->index_table_dir_model("+index_table_dir_model+") is not exited!");
        return NULL;
    }
    if (!index_dest_node ){
        this->SHOW_ERROR((string)"GetIndexTableObjs_AppendNode()->AddInfo->index_table_dir_model("+index_table_dir+") is not exited!");
        return NULL;
    }
        
    //克隆操作
    ParseXml_Node index_add_node=index_dest_node.append_copy(index_src_node);
    
    //取得name_field字段；
    UInt64 name_field=0,id_field=0;
    if (!this->GetAddInfoUINT(index_stru.AddInfoObj,"INDEX_FIELD",id_field))
        id_field=STRING_Helper::NaN;
    if (!this->GetAddInfoUINT(index_stru.AddInfoObj,"INDEX_TABLE_DIR_NAME_FIELD",name_field))
        name_field=id_field;
    
    //create add_name from table field; 多种目录名方式：字符串、hex、number、num
    BYTE index_cell_type=BB_GET_TABLE_FIELD_TYPE(index_stru.CellObj,name_field);

     string name(""),name_bin;
    if (name_field!=STRING_Helper::NaN){
        name_bin=this->GetTableItemString(&index_stru,num,name_field);
        //如果非字符串则采用0x0000n
        if (name_bin.size()==0){
            this->SHOW_ERROR((string)"AvartarNewObj_GetIndexTableObjs()->AppendNode->new node name is ''!");
            return NULL;
        }else if (index_cell_type==BB_TABLE_FIELD_TYPE_STRING){
            name=name_bin;
        }else if(index_cell_type==BB_TABLE_FIELD_TYPE_BINARY || index_cell_type==BB_TABLE_FIELD_TYPE_NUMBER){
            name=(string)AVARTDB_CREATE_OBJ_HEAD_STR+STRING_Helper::HBCStrToHBCStr(name_bin, "char", "hex", true);
        }else{
            this->SHOW_ERROR((string)"AvartarNewObj_GetIndexTableObjs()->AppendNode->NAME_FIELD_TYPE("+STRING_Helper::iToStr(index_cell_type)+"),new node name only support STRING|BINARY|NUMBER!");
            return NULL;
        }
    }else{
        name=AVARTDB_CREATE_OBJ_HEAD_STR+STRING_Helper::uToStr(num);
    }
    
    //DBCONFIG_NODE_OBJECT_MALLOC
    BB_OBJ *son_obj_p=NULL;
    index_add_node.set_name(name.c_str()); //更新模板节点为当前节点-名称
    son_obj_p = this->AvartarNewObj(index_table_dir+"/"+name);  //??是否清楚内存空间　//ly malloc !!! 创建＋应该改为增加MALLOC_EXT处理
    
    int r=0;
    if (!xml_remain)
        r=index_add_node.parent().remove_child(index_add_node);
    
    return son_obj_p;
}

//添加表格索引
bool AvartarDB_Action::AvartarNewObj_SortObjFieldInit(BB_OBJ *table_obj, BYTE field_id) {//ly sort
    BYTE *item;
    BB_SIZE_T item_size;
    BYTE field_type;
    //读取表格中的字段
    cout << "loading index ...." << endl;
    BB_OBJ_STRU table_obj_stru = BB_GET_OBJ_STRU(table_obj);
    for (size_t i = 0; i < table_obj_stru.Num; i++) {
        //取得item;
        if (BB_GET_TABLE_ITEMS_DATA_TYPE_VALUE(&table_obj_stru, i, field_id, &item, &item_size,&field_type)) {
            if (!this->SortObj->InsertItem(table_obj, field_id, item, item_size, i))
                return false;
        }
    }
    return true;
}

//加载全局内存
bool AvartarDB_Action::AvartarNewObj_LoadExtField(BB_OBJ *table_obj, string &save_ext) {
    //读取表格中的字段//ly malloc2.1
    cout << "loading ext fields ...." << endl;
    BB_OBJ_STRU table_obj_stru = BB_GET_OBJ_STRU(table_obj);
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU(table_obj_stru.CellObj);
    if (cell_obj_stru.Type != BB_TABLE)
        return false;
    
    for (size_t i = 0; i < table_obj_stru.Num; i++) {
        //取得LoadExtField 文件
        string index_str;
        if (!this->BuildBuffer_TableCnvNumToNodeID(table_obj_stru, i,index_str))
            continue;//新创建通过num读取index
        BYTE * buf = NULL;
        string ext_name=save_ext;
        BB_SIZE_T p_start=0,buf_len=0xffffffff;
        if (this->LoadExt(table_obj_stru, index_str,ext_name,&buf, p_start, buf_len, 0)!=0)
            continue;
        
        //循环读取文件para；写入记录数据
        BYTE *p=buf;
        for(BB_SIZE_T j=0;;j++){
            //顺序取得对象
            BB_OBJ *obj=(BB_OBJ *)p;
            BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(p);
            BB_SIZE_T obj_len=BB_GET_OBJ_SIZE(obj);
            
            UInt64 col=BB_GET_UINT_VALUE(obj_stru.AddInfoObj); //取得AddInfoObj=BYTE就是col名
            //写入操作???如果是旧内存MALLOC字段，free会出差错。
            BB_PUT_TABLE_ITEMS_DATA_STRU(&table_obj_stru, i, col, (BYTE *)obj_stru.Data, obj_stru.DataLen, BB_TABLE_PUT_OPERATE_INIT);
            //下一对象
            p+=obj_len;
            if (p>=buf+buf_len)
                break;
        }
        //清空缓存
        if (buf)
            free(buf);
    }
    return true;
}

//消息数据包处理
BYTE AvartarDB_Action::DoActionMessage(string &packet_head, string &input_str, const char *add_flag_str, string &func_return, PV_AddParas_T &add_paras) {
    PARSE_BUFF_STRU obj;
    
    func_return = "";
    //补充access-mark
    string head_str;
    if (packet_head.size() == 8) {//替换auth_mark
        obj.auth_mark = packet_head;
        head_str = "";
    }
    else {//同源
        obj.auth_mark = "";
        head_str = packet_head;
    }
    bool add_flag = strstr(add_flag_str, "no_header") == add_flag_str ? false : true;
    
    //参数解析
    BYTE err_char;
    if ((err_char = this->ParseActionMessage(head_str, input_str, obj, add_paras)) == 0) {
        //测试段1;
        err_char = this->DoActionParas(obj, func_return, add_paras);
    }else{
        obj.act = 0;//对于this->ParseActionMessage的差错，统一采用00错误
    }
    
    if (err_char != 0){
        obj.state = err_char;
        obj.paras.clear();
    }
    
    //NEW_DATAR 动作处理
    if ((obj.flag & BB_TABLE_REC_WR_BITIDS) == BB_TABLE_REC_WR_BITMAP){//A76=01:Bitmap,10:Offset/length,;11.（读取id数组、回传id数组、nodeid转换）
        //cout << STRING_Helper::iToStr(obj.flag) << "-" << BB_TABLE_REC_WR_BITMAP << "-" << (obj.flag & BB_TABLE_REC_WR_BITMAP) <<"-" << endl;
        func_return=this->BuildResultMessageArray(obj, func_return, func_return,false, add_paras);
    }else if (obj.packet_type==ACTION_PACKET_TYPE_DATAR_OBJECT){
        //打包回传
        if (err_char!=0){
            //如果返回差错则补充差错包;如果其他非返回包也采用此方法打包。
            this->BuildResultMessage(obj,func_return,add_paras);
        }
    }else if (obj.packet_type == ACTION_PACKET_TYPE_DATAR_ARRAY){
        if (err_char != 0) {
            string result_str("");
            this->BuildResultMessageArray(obj, result_str, func_return,add_flag, add_paras);
        }else
            this->BuildResultMessageArray(obj, func_return, func_return,add_flag, add_paras);
    }
    else{
        //普通回传
        func_return=string("")+STRING_Helper::iToStr(err_char)+func_return;
    }
    return err_char;
}

//消息头转标准obj参数结构
BYTE AvartarDB_Action::ParseActionMessage(string &packet_head,string &input_str, PARSE_BUFF_STRU &parse,PV_AddParas_T &add_paras){
    
    //return 0;
    //解析并验证输入对象??建议还是采用全面解析法：act,format,indextable_obj,buf,condition,buf_obj;
    parse.input_obj=(BB_OBJ *)input_str.data();
    BB_SIZE_T tmp_obj_size=BB_GET_OBJ_SIZE(parse.input_obj);
    
    if ( tmp_obj_size > input_str.size()){
        this->SHOW_ERROR((string)"InputString's Object Size("+STRING_Helper::iToStr(tmp_obj_size)+")by LenObj > Orignel size("+STRING_Helper::iToStr(input_str.size())+")!");
        return ACTION_ERROR_PACKET_PARSE_OBJ_LEN_LARGER_THEN_INPUT_STR_LEN;
    }
    //测试段0
    
    //校验参数数目
    BB_SIZE_T n=0;     //ly mac
    BB_OBJ_STRU obj_head;
    if (!BB_GET_OBJ_HEAD_STRU(parse.input_obj,&obj_head) || !BB_GET_ARRAY_OBJ_NUM_CHECK_STRU(&obj_head,&n,ACTION_MAX_PARAS_NUM)){
        this->SHOW_ERROR((string)"InputString's Object Num large than max num("+STRING_Helper::iToStr(n)+"),or is not defined obj type!");
        return ACTION_ERROR_PACKET_PARSE_OBJ_NUM_LARGER_THEN_MAX_NUM_OR_STRU_ERROR;
    }
    //测试段1
    //取得参数对象
    parse.input_stru=BB_GET_OBJ_STRU(parse.input_obj);
    if (parse.input_stru.LoadError || !BB_IS_OBJ_ARRAY_STRU(&(parse.input_stru))){//如果不是对象数组则返回错误
        this->SHOW_ERROR((string)"ParseActionMessage(),input string must be Object Array!");
        return ACTION_ERROR_PACKET_PARSE_OBJ_IS_NOT_ARRAY_OBJ;
    }
    
    //取得parse 头
    if (!(packet_head.size()==3 || packet_head.size()==0)){
        this->SHOW_ERROR((string)"InputString's packet_head size() not equal to 3");
        return ACTION_ERROR_PACKET_PARSE_HEAD_STR_IS_NOT_3_OR_0;
    }
    
    parse.task_id_obj=NULL; //空task_id;
    parse.session_obj = NULL;
    parse.flag=parse.add_flag = 0;
    //测试段2
    if (parse.input_stru.AddInfoObj) {
        if (packet_head.size()==3){
            //如果是传统DATAR 数据包
            parse.packet_type=ACTION_PACKET_TYPE_DATAR_PACKET;
            parse.state=packet_head.at(0);
            parse.flag=packet_head.at(1);
            parse.act=packet_head.at(2);
        }
        else if (packet_head.size()==0 && parse.input_stru.AddInfoObj){
            // 如果是Query Object 二进制流数据包
            parse.packet_type = ACTION_PACKET_TYPE_DATAR_OBJECT;
            if (parse.input_stru.Type!=BB_ARRAY){
                this->SHOW_ERROR((string)"Query Object is not BB_ARRAY!");
                return ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_IS_NOT_ARRAY;
            }
            BB_OBJ_STRU add_info_stru=BB_GET_OBJ_STRU(parse.input_stru.AddInfoObj);
            if (parse.input_stru.AddInfoObj==NULL || add_info_stru.LoadError){
                this->SHOW_ERROR((string)"Query Object's AddInfo is not BB_ARRAY!");
                return ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_IS_NOT_DEFINED;
            }
            if (add_info_stru.Type!=BB_ARRAY){
                this->SHOW_ERROR((string)"Query Object's AddInfo is not BB_ARRAY!");
                return ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_IS_NOT_ARRAY;
            }
            BB_OBJ *item;
            BB_SIZE_T item_size;
            if (!BB_GET_ARRAY_OBJ_BY_NUM_STRU(&add_info_stru,ACTION_PARAS_ARRAY_PACKET_HEAD_REC,&item, &item_size)){
                this->SHOW_ERROR((string)"Query Object's AddInfo(0) get record error!");
                return ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_GET_HEAD_FLAG_ERROR;
            }
            //parse.packet_head=(BB_OBJ *)item;
            if (BB_GET_OBJ_TYPE(item)!=BB_BYTES){
                this->SHOW_ERROR((string)"InputString's AddInfo(0) is not packet_head_bytes when packet_head is '' !");
                return ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_GET_HEAD_FLAG_IS_INCORRECT;
            }
            string packet_head_tmp((const char*)BB_GET_OBJ_DATA(item),BB_GET_OBJ_DATA_LEN(item));
            if (packet_head_tmp.size()!=3){
                this->SHOW_ERROR((string)"InputString's packet_head size(obj) not equal to 3");
                return ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_HEAD_FLAG_SIZE_IS_NOT_3;
            }
            parse.state=packet_head_tmp.at(0);
            parse.flag=packet_head_tmp.at(1);
            parse.act=packet_head_tmp.at(2);
            if (BB_GET_ARRAY_OBJ_BY_NUM_STRU(&add_info_stru,ACTION_PARAS_ARRAY_PACKET_TASKID_REC,&item, &item_size)){//取得task_id对象
                parse.task_id_obj=item;
            }
            if (BB_GET_ARRAY_OBJ_BY_NUM_STRU(&add_info_stru, ACTION_PARAS_ARRAY_PACKET_SESSION_REC, &item, &item_size)) {//取得session对象
                parse.session_obj = item;
            }
        }
        for (BB_SIZE_T i = 0; i<parse.input_stru.Num; i++) {//ly: 今后应改为递进查询法
            BB_OBJ *obj;
            BB_SIZE_T obj_size;
            if (!BB_GET_ARRAY_OBJ_BY_NUM_STRU(&(parse.input_stru), i, &obj, &obj_size)) {//必须拥有内容部分,
                this->SHOW_ERROR((string)"ParseActionMessage(),InputObj->Paras[" + STRING_Helper::iToStr(i) + "] must have set as 'buff_obj'!");
                return ACTION_ERROR_PACKET_PARSE_GET_PARAS_OBJ_PROBLEM;
            }
            //重定向处理
            string tmp_str;
            if (BB_GET_OBJ_TYPE(obj) == BB_REDIR) {
                string tmp_str1;
                if (!this->GetTableRedirValue(obj, obj_size, tmp_str1)) {//ly:BB_REDIR
                    this->SHOW_ERROR((string)"ParseActionMessage(),InputObj->Paras[" + STRING_Helper::iToStr(i) + "] Redir Obj Error!");
                    return ACTION_ERROR_PACKET_PARSE_GET_PARAS_OBJ_PROBLEM;
                }
                BB_OBJ *result_obj = (BB_OBJ *)tmp_str1.data();
                BB_SIZE_T obj_len = BB_GET_OBJ_SIZE(result_obj);
                if (obj_len > tmp_str1.size()) {
                    this->SHOW_ERROR((string)"ParseActionMessage(),InputObj->Paras[" + STRING_Helper::iToStr(i) + "] may be not objects!");
                    return ACTION_ERROR_PACKET_PARSE_GET_PARAS_OBJ_PROBLEM;
                }
                tmp_str = string((const char*)result_obj, obj_len);
            }
            else {
                tmp_str = string((const char *)obj, obj_size);
            }
            //增加字串内容
            parse.paras.push_back(tmp_str);
        }
    }
    else if (packet_head.size() == 0 && parse.input_stru.AddInfoObj==NULL){
        //如果是数组模型[[“Read”,”0x00001234”,”0x2016101023333333”],["PATH|COLUMN_IDS|CONDITIONS|CURSOR","0x0106","0x010203",[[“1”,”>”,”25”],[“4”,”==”,”Lee”]],"0x000000010002"]]
        //有两种携带access_mask的情况(1)packet_head=="", 缺省session_obj为==access_mask (2)packet_head==access_mack, 会替换session_obj;
        parse.packet_type = ACTION_PACKET_TYPE_DATAR_ARRAY;
        if (parse.input_stru.Type != BB_ARRAY) {
            this->SHOW_ERROR((string)"Query Object is not BB_ARRAY!");
            return ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_IS_NOT_ARRAY;
        }
        
        for (unsigned int i = 0; i<parse.input_stru.Num; i++) {//ly: 今后应改为递进查询法
            BB_OBJ *obj;
            BB_SIZE_T obj_size;
            if (!BB_GET_ARRAY_OBJ_BY_NUM_STRU(&(parse.input_stru), i, &obj, &obj_size)) {//必须拥有内容部分,
                this->SHOW_ERROR((string)"ParseActionMessage(),InputObj->Paras[" + STRING_Helper::iToStr(i) + "] must have set as 'buff_obj'!");
                return ACTION_ERROR_PACKET_PARSE_GET_PARAS_OBJ_PROBLEM;
            }
            parse.state = 0x00;
            
            BB_OBJ_STRU arr2 = BB_GET_OBJ_STRU(obj);
            if (arr2.LoadError || !BB_IS_OBJ_ARRAY_STRU(&arr2)) {//如果不是对象数组则返回错误
                this->SHOW_ERROR((string)"ParseActionMessage(),input[" + STRING_Helper::iToStr(i) + "] string must be Object Array!");
                return ACTION_ERROR_PACKET_PARSE_OBJ_IS_NOT_ARRAY_OBJ;
            }
            for (unsigned int j = 0; j < arr2.Num; j++) {
                BB_OBJ *item;
                BB_SIZE_T item_size; //ACTION_PARAS_ARRAY_PACKET_HEAD_REC
                if (!BB_GET_ARRAY_OBJ_BY_NUM_STRU(&arr2, j, &item, &item_size)) {
                    this->SHOW_ERROR((string)"ParseActionMessage(),get input[" + STRING_Helper::iToStr(i) + "] [" + STRING_Helper::iToStr(j) + "]error!");
                    return ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_GET_HEAD_FLAG_ERROR;
                }
                if ( i == 0 && j == 0) {
                    if (BB_GET_OBJ_TYPE(item) == BB_BYTES) {//普通的双字节定依法
                        if (BB_GET_OBJ_DATA_LEN(item) != 2) {
                            this->SHOW_ERROR((string)"InputString[" + STRING_Helper::iToStr(i) + "] [" + STRING_Helper::iToStr(j) + "]must be BYTES_LEN_2!");
                            return ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_GET_HEAD_FLAG_IS_INCORRECT;
                        }
                        
                        string act_flag = string((const char*)BB_GET_OBJ_DATA(item), BB_GET_OBJ_DATA_LEN(item));
                        parse.act = act_flag.at(0);
                        parse.flag = act_flag.at(1);
                    }
                    else if (BB_IS_OBJ_STRING(item)) {
                        string act_flag_str = string((const char*)BB_GET_OBJ_DATA(item), BB_GET_OBJ_DATA_LEN(item));
                        
                        if (act_flag_str.size() == 0) {
                            this->SHOW_ERROR((string)"InputString[" + STRING_Helper::iToStr(i) + "] [" + STRING_Helper::iToStr(j) + "]must be BYTES_LEN_2!");
                            return ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_GET_HEAD_FLAG_IS_INCORRECT;
                        }
                        
                        vector <string> act_flag_paras;
                        STRING_Helper::stringSplit(act_flag_str, act_flag_paras, ":");
                        if (act_flag_paras[0] == "SEARCH") {
                            parse.act = ACTION_READ;
                            parse.flag = BB_TABLE_REC_WR_BITIDS | BB_TABLE_REC_WR_QUERY_CONDITIONS | BB_TABLE_REC_WR_QUERY_CURSOR;
                        }
                        else if(act_flag_paras[0] == "GET") {
                            parse.act = ACTION_READ;
                            parse.flag = BB_TABLE_REC_WR_BITIDS;
                        }
                        else if (act_flag_paras[0] == "PUT") {
                            parse.act = ACTION_WRITE;
                            parse.flag = BB_TABLE_REC_WR_BITIDS;
                        }
                        else if (act_flag_paras[0] == "DELETE") {
                            parse.act = ACTION_DELETE;
                            //parse.flag = BB_TABLE_REC_WR_BITIDS;
                        }
                        else if (act_flag_paras[0] == "GET_CLUSTER_INFO") {
                            parse.act = ACTION_GET_CLUSTER_INFO;
                            parse.flag = BB_TABLE_REC_WR_BITIDS | BB_TABLE_REC_WR_QUERY_CURSOR;
                        }
                        else if (act_flag_paras[0] == "GET_TABLE_INFO") {
                            parse.act = ACTION_GET_TABLE_INFO;
                        }
                        else if (act_flag_paras[0] == "SEND") {//发送和转发操作
                            parse.act = ACTION_GET_TABLE_INFO; //???
                            //parse.flag = AVTA_PKG_ROUTING; //设置ROUTING;ACT;
                        }else if (act_flag_paras[0] == "PUT_EXT") {//发送和转发操作
                            parse.act = ACTION_WRITE_EXT;
                            parse.flag = BB_TABLE_REC_WR_BITSTARTEND; //设置ROUTING;ACT;
                        }else if (act_flag_paras[0] == "GET_EXT") {//发送和转发操作
                            parse.act = ACTION_READ_EXT;
                            parse.flag = BB_TABLE_REC_WR_BITSTARTEND; //设置ROUTING;ACT;
                        }
                        else {
                            //取得act
                            parse.act = 0;
                            if (!this->_ParseActionMessage_Define(act_flag_paras[0].c_str(), parse.act)) {
                                this->SHOW_ERROR((string)"InputString[" + STRING_Helper::iToStr(i) + "] [" + STRING_Helper::iToStr(j) + "]act flag is incorrect!");
                                return ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_GET_HEAD_FLAG_IS_INCORRECT;//to_add
                            }
                            
                        }
                        //取得flag
                        if (act_flag_paras.size()>=2 &&  !this->_ParseActionMessage_ActFlag(act_flag_paras[1], &(parse.flag), &(parse.add_flag))) {
                            this->SHOW_ERROR((string)"InputString[" + STRING_Helper::iToStr(i) + "] [" + STRING_Helper::iToStr(j) + "]act flag is incorrect!");
                            return ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_GET_HEAD_FLAG_IS_INCORRECT;//to_add
                        }
                    }
                    else{
                        this->SHOW_ERROR((string)"InputString[" + STRING_Helper::iToStr(i) + "] [" + STRING_Helper::iToStr(j) + "] is Incorrect!");
                        return ACTION_ERROR_PACKET_PARSE_QUERY_OBJ_ADDINFO_GET_HEAD_FLAG_IS_INCORRECT;
                    }
                    
                }
                
                if (i==0 && j==2)
                    parse.task_id_obj = item;
                if (i==0 && j==1)
                    parse.session_obj = item;
                //if (i == 1 && j > 0) {
                if (i == 1 || i==2) {
                    //重定向处理
                    string tmp_str;
                    if (BB_GET_OBJ_TYPE(obj) == BB_REDIR) {//精简Redir
                        BB_OBJ *tmp_obj=item;		//BB_OBJ *result_obj;
                        BB_SIZE_T tmp_obj_len=item_size;		//BB_SIZE_T result_obj_len;
                        //if (!this->GetTableRedirObj(item, item_size, &result_obj, result_obj_len)) {//ly:BB_REDIR
                        if (!this->GetTableRedirObj(tmp_obj,tmp_obj_len,&item, item_size)) {//ly:BB_REDIR
                            this->SHOW_ERROR((string)"ParseActionMessage(),InputObj->Paras[" + STRING_Helper::iToStr(i) + "] Redir Obj Error!");
                            return ACTION_ERROR_PACKET_PARSE_GET_PARAS_OBJ_PROBLEM;
                        }
                        //tmp_str = string((const char*)result_obj, result_obj_len);
                    }
                    //else {
                    //tmp_str = string((const char *)item, item_size);
                    //}
                    if (i==2)
                        parse.paras.push_back(string((const char *)obj, obj_size));
                    else
                        parse.paras.push_back(string((const char *)item, item_size));
                    //tmp_str = string((const char *)item, item_size);
                    //增加字串内容
                    //parse.paras.push_back(tmp_str);
                }
            }
        }
        
    }
    
    return 0;
}

bool AvartarDB_Action::_ParseActionMessage_ActFlag(string &act_flag, BYTE *flag1, BYTE *flag2) {
    vector <string> flags;
    STRING_Helper::stringSplit(act_flag, flags, "|");
    //(*flag1) = 0;
    for (BB_SIZE_T i = 0; i < flags.size(); i++) {
        if(flag2!=0 && flags[i]=="VERICAL"){
            (*flag2) = (*flag2) | ACTION_ADD_FLAG_VERICAL;
        }else if (!this->_ParseActionMessage_Define(flags[i].c_str(), (*flag1))) {
            this->SHOW_ERROR((string)"Array Cell Parse error! num=" + act_flag);
            return false;//to_add
        }
    }
    return true;
}


bool AvartarDB_Action::_ParseActionMessage_Define(const char *obj_type, BYTE &flag) {
    if ((QFlags.find(obj_type)) != QFlags.end()) {
        if (strstr(obj_type, "CONVERT") == obj_type)
            flag = flag & QFlags.find(obj_type)->second;
        else
            flag = flag | QFlags.find(obj_type)->second;
        return true;
    }else {
        this->SHOW_ERROR((string)"Flag '" + obj_type + "'are not supported!");
        this->SET_LAST_ERROR_STR((string)"Flag '" + obj_type + "' are not support!");
        return false;
    }
}

void  AvartarDB_Action::ParseActionMessage_Debug(PARSE_BUFF_STRU &obj){
    stringstream s;
    s << "obj.state:"<< STRING_Helper::iToStr(obj.state) << endl;
    s << "obj.flag:"<< STRING_Helper::iToStr(obj.flag) << endl;
    s << "obj.act:"<< STRING_Helper::iToStr(obj.act) << endl;
    for (BB_SIZE_T i=0; i<obj.paras.size(); i++){
        s << "obj.paras[" <<STRING_Helper::iToStr(i)<< "]:"<< obj.paras[i] << endl;
    }
    //cout << s. << endl;
};

BYTE AvartarDB_Action::BuildResultMessageVector(PARSE_BUFF_STRU &parse,vector<string> &send_data, string &send_msg,PV_AddParas_T &add_paras,bool is_add_paras,bool is_add_bytes_obj){
    //生成parase_obj 数据集，（注：都改为对象）
    if (!is_add_paras)
        parse.paras.clear();//如果不是累加则清空
    BB_OBJ *data_obj=NULL;
    for (BB_SIZE_T i = 0; i < send_data.size(); i++) {
        if (is_add_bytes_obj) {
            if ((send_data[i].size() + 1) < BB_0DIM_BYTES_MAX_LEN)
                data_obj = BB_NEW_CELL(BB_BYTES, send_data[i].data(), send_data[i].size(), NULL);//短字串
            else if ((send_data[i].size() + 2) < BB_0DIM_BYTES_EXT_MAX_LEN)
                data_obj = BB_NEW_CELL(BB_BYTES_EXT, send_data[i].data(), send_data[i].size(), NULL);//长字串
            else if ((send_data[i].size() + 2) > BB_0DIM_BYTES_EXT_MAX_LEN) {
                BB_OBJ cell = BB_UINT8;
                data_obj = BB_NEW_ARRAY(&cell, NULL, NULL, 0, send_data[i].data(), send_data[i].size(), NULL);
            }
            parse.paras.push_back(string((const char*)data_obj, BB_GET_OBJ_SIZE(data_obj)));
            if (data_obj)
                free(data_obj);
        }
        else {
            parse.paras.push_back(send_data[i]);
        }
        
    }
    return this->BuildResultMessage(parse, send_msg, add_paras);
};

BYTE AvartarDB_Action::BuildResultMessage(PARSE_BUFF_STRU &parse, string &send_msg, PV_AddParas_T &add_paras) {
    
    //创建packet_head_obj;
    string add_info_data("");
    string packet_head("");
    packet_head += parse.state; //作为返回值,FF和0都代表成功，ff还代表当前包为返回包。
    packet_head += parse.flag; packet_head += parse.act;
    BB_OBJ *add_info_head_obj = BB_NEW_CELL(BB_BYTES, packet_head.data(), packet_head.size(), NULL);
    add_info_data.append((const char *)add_info_head_obj, BB_GET_OBJ_SIZE((BB_OBJ*)add_info_head_obj));
    //创建task_id对象
    if (parse.task_id_obj != NULL)
        add_info_data.append((const char *)parse.task_id_obj, BB_GET_OBJ_SIZE(parse.task_id_obj));
    //创建addinfo_array
    BB_OBJ cell = BB_NULL;
    BB_OBJ *add_info_obj = BB_NEW_ARRAY(&cell, NULL, NULL, 0, add_info_data.data(), add_info_data.size(), NULL);
    string test((const char *)add_info_obj, BB_GET_OBJ_SIZE(add_info_obj));
    //创建请求对象
    string data_obj_str("");
    for (BB_SIZE_T i = 0; i < parse.paras.size(); i++) {
        data_obj_str.append(parse.paras[i]);
    }
    BB_OBJ *r_obj = BB_NEW_ARRAY(&cell, NULL, add_info_obj, 0, (void *)data_obj_str.data(), data_obj_str.size(), NULL); //添加8002**00外部对象数组包
    send_msg = "";
    send_msg.append((const char *)r_obj, BB_GET_OBJ_SIZE(r_obj));
    if (add_info_head_obj)
        free(add_info_head_obj);
    if (add_info_obj)
        free(add_info_obj);
    if (r_obj)
        free(r_obj);
    
    return 0;
};

BYTE AvartarDB_Action::BuildResultMessageArray(PARSE_BUFF_STRU &obj, string &result_str, string &send_msg, bool add_header, PV_AddParas_T &add_paras) {
    BB_OBJ cell = BB_NULL;
    BB_OBJ *head_obj;
    //创建packet_head_obj;flag+state+act+taskid
    if (add_header){
        //创建头数组
        string packet_head(""); BB_UINT_STRU u_obj;
        u_obj = BB_NEW_CELL_UINT(obj.act, BB_UINT8);
        packet_head += string((const char*)&u_obj, BB_GET_OBJ_SIZE((BB_OBJ *)&u_obj));//add act
        u_obj = BB_NEW_CELL_UINT(obj.state, BB_UINT8);
        packet_head += string((const char*)&u_obj, BB_GET_OBJ_SIZE((BB_OBJ *)&u_obj)); //add state
        u_obj = BB_NEW_CELL_UINT(obj.flag, BB_UINT8);
        packet_head += string((const char*)&u_obj, BB_GET_OBJ_SIZE((BB_OBJ *)&u_obj));//add flag
        if (obj.task_id_obj != NULL)
            packet_head += string((const char *)obj.task_id_obj, BB_GET_OBJ_SIZE(obj.task_id_obj)); //add taskid
        head_obj = BB_NEW_ARRAY(&cell, NULL, NULL, 0, packet_head.data(), packet_head.size(), NULL);
        //创建内容数组
        BB_OBJ *content_obj = BB_NEW_ARRAY(&cell, NULL, NULL, 0, (void *)result_str.data(), result_str.size(), NULL); //添加8002**00外部对象数组包
        //创建外壳数组
        string result = string((const char*)head_obj, BB_GET_OBJ_SIZE(head_obj));
        result += string((const char*)content_obj, BB_GET_OBJ_SIZE(content_obj));
        BB_OBJ *r_obj = BB_NEW_ARRAY(&cell, NULL, NULL, 0, (void *)result.data(), result.size(), NULL);
        send_msg = "";
        send_msg.append((const char *)r_obj, BB_GET_OBJ_SIZE(r_obj));
        if (head_obj != NULL)
            free(head_obj);
        if (content_obj != NULL)
            free(content_obj);
        if (r_obj != NULL)
            free(r_obj);
    }
    else {
        BB_OBJ *content_obj = BB_NEW_ARRAY(&cell, NULL, NULL, 0, (void *)result_str.data(), result_str.size(), NULL); //添加8002**00外部对象数组包
        send_msg = "";
        send_msg.append((const char *)content_obj, BB_GET_OBJ_SIZE(content_obj));
        if(content_obj != NULL)
            free(content_obj);
    }
    
    return 0;
};


//NEW_DATAR 动作处理，on obj参数结构
BYTE AvartarDB_Action::DoActionParas(PARSE_BUFF_STRU &obj, string &func_return, PV_AddParas_T &add_paras) {
    string err_str, tmp1("");
    int r;
    string bitmap_str("");
    
    //取得table_obj;
    if (obj.paras.size() == 0) {
        this->SHOW_ERROR((string)"DoActionParas() input paras[0] must be tableindex!");
        return ACTION_ERROR_PACKET_PARAS_NUM_ERROR;
    }
    //取得tableindex信息
    
    string tableindex("");
    BYTE table_r;
    if ((table_r=this->DoActionParas_GetTableIndex((BB_OBJ *)(obj.paras[0].data()), tableindex)) != 0)
        return table_r;
    
    //
    BB_OBJ *table_obj;
    BB_SIZE_T table_obj_size;
    if (!this->GetTableObject(tableindex, &table_obj, table_obj_size)) {
        //this->GetTableObject(tableindex, &table_obj, table_obj_size);
        this->SHOW_ERROR((string)"DoActionParas() can't tableobj by index(" + tableindex + ")!");
        return ACTION_ERROR_PACKET_PARAS0_TABLE_NOT_FOUND;
    }
    BB_OBJ_STRU table_stru = BB_GET_OBJ_STRU(table_obj);
    
    //obj.add_flag = 0;   //各个参数解析返回的，@补充参数
    
    switch (obj.act) {
        case ACTION_READ:
        {
            //测试参数数量;
            BYTE type;
            unsigned int now_para_num = 0;
            if (obj.paras.size() < 3) {
                this->SHOW_ERROR((string)"DoActionParas can't Read Rec error-input paras must have 4 item(0:tableindex,1:wformate,2:condition/nodeids,*3:page_cursor,*4:convert_table)!");
                return ACTION_ERROR_PACKET_PARAS_NUM_ERROR;
            }
            
            //取得conditions
            string nodeid_str(""),conditions_str("");
            vector<string> nodeid_vec;
            now_para_num++;
            if ((obj.flag&BB_TABLE_REC_WR_QUERY_CONDITIONS)==BB_TABLE_REC_WR_QUERY_CONDITIONS){
                //如果有conditions
                if (now_para_num>=obj.paras.size()){
                    this->SHOW_ERROR((string )"DoActionParas() Error Read input paras["+STRING_Helper::iToStr(now_para_num)+"](conditions) is out of obj.paras.size()="+STRING_Helper::iToStr(obj.paras.size())+")!");
                    return ACTION_ERROR_READ_PARAS_CONDITIONS_IS_OUT_OF_PARAS_SIZE;
                }
                BB_OBJ_STRU conditions_stru;
                BB_OBJ *conditions_obj=(BB_OBJ *)(obj.paras[now_para_num].data());
                type=BB_GET_OBJ_TYPE(conditions_obj);
                
                //if (!(type==BB_NULL || type==BB_BYTES || type==BB_BYTES_EXT )){//wformate test
                if (!(type==BB_ARRAY || type == BB_NULL || type == BB_BYTES || type == BB_BYTES_EXT)) {//ly 910;增加BB_ARRAY for [["****",="","???"]]
                    this->SHOW_ERROR((string )"DoActionParas() Error Read input paras["+STRING_Helper::iToStr(now_para_num)+"](conditions) is not BB_NULL or BB_ARRAY_OBJ)!");
                    return ACTION_ERROR_READ_PARAS_CONDITIONS_IS_NOT_NULL_BYTES_OR_BYTE_EXT;
                };
                conditions_stru=BB_GET_OBJ_STRU(conditions_obj);
                conditions_str=(type==BB_NULL) ? "" : string((const char *)conditions_stru.Data,conditions_stru.DataLen);//如果有conditions ly add2
            }
            else{
                //如果有nodeids
                BB_OBJ *nodeid_obj=(BB_OBJ *)(obj.paras[now_para_num].data());
                BYTE type=BB_GET_OBJ_TYPE(nodeid_obj);
                BB_OBJ_STRU nodeid_obj_stru=BB_GET_OBJ_STRU(nodeid_obj);
                if (BB_IS_OBJ_ARRAY_STRU(&nodeid_obj_stru)){//多维数组模式 ly_ids
                    if (!this->GetArrayRecsVector(nodeid_obj_stru,nodeid_vec))
                        return ACTION_ERROR_READ_PARAS_NODEID_IS_NOT_BYTES_ARRAYS;
                }else if (!(type==BB_NULL || type==BB_BYTES || type==BB_BYTES_EXT || BB_IS_OBJ_STRING(nodeid_obj))){//wformate test
                    this->SHOW_ERROR((string )"DoActionParas() Error Read input paras[2](nodeid) is not BB_NULL or BB_ARRAY_OBJ)!");
                    return ACTION_ERROR_READ_PARAS_NODEID_IS_NOT_NULL_BYTES_OR_BYTE_EXT;
                }else{
                    nodeid_str=(type==BB_NULL) ? "" : string((const char *)BB_GET_OBJ_DATA(nodeid_obj),BB_GET_OBJ_DATA_LEN(nodeid_obj));//如果无nodeids则参数固定为nodeid
                }
            }
            
            //取得bitmap
            now_para_num++;
            //转换如果是"1,2,3,4,5",转换成"0x0102030405"
            BYTE bitmap_r; string bitmap_str;
            BB_OBJ_STRU cell_obj_table = BB_GET_OBJ_STRU(table_stru.CellObj);
            if ((bitmap_r = this->DoActionParas_GetBitmapExt(cell_obj_table,(BB_OBJ *)obj.paras[now_para_num].data(), bitmap_str, obj.add_flag)) != 0) { //ly cell_name; ly task
                return bitmap_r;
            }
            /*if ((obj.flag & BB_TABLE_REC_WR_BITIDS) == BB_TABLE_REC_WR_BITIDS) {//测试bitmap_str[n]是否超出field范围 ly cell_name; ly task
                //测试每一个字段号是否超出
                BB_OBJ_STRU cell_obj_table = BB_GET_OBJ_STRU(table_stru.CellObj);
                for (BB_SIZE_T i = 0; i < bitmap_str.size(); i++) {
                    if (i >(cell_obj_table.Num - 1)) {
                        this->SHOW_ERROR((string)"DoActionParas() Error Read input bitmap[" + STRING_Helper::iToStr(i) + "](conditions) is out of range cell_table_stru.Num(" + STRING_Helper::iToStr(cell_obj_table.Num) + ")!");
                        return ACTION_ERROR_READ_PARAS_BITMAP_CELL_IS_OUT_OF_CELL_TABLA_NUM;
                    }
                }
            }*/
            
            //取得page/cursor; 页码
            string cursor_str("");
            if ((obj.flag&BB_TABLE_REC_WR_QUERY_CURSOR)==BB_TABLE_REC_WR_QUERY_CURSOR ){
                if ((++now_para_num)>=obj.paras.size()){
                    this->SHOW_ERROR((string )"DoActionParas() Error Read input paras["+STRING_Helper::iToStr(now_para_num)+"](cursor) is out of obj.paras.size()="+STRING_Helper::iToStr(obj.paras.size())+")!");
                    return ACTION_ERROR_READ_PARAS_CURSOR_IS_OUT_OF_PARAS_SIZE;
                }
                BYTE cur_r;
                if ((cur_r = this->DoActionParas_GetCursor((BB_OBJ *)obj.paras[now_para_num].data(), cursor_str)) != 0)
                    return cur_r;
            }
            
            //取得转换模式ConvertTable/ConvertStr;
            string convert_str("");
            if ((obj.flag | BB_TABLE_REC_WR_BITIDS_CONVERT)==BB_TABLE_REC_WR_BITIDS_CONVERT){
                if ((++now_para_num)>=obj.paras.size()){
                    this->SHOW_ERROR((string )"DoActionParas() Error Read input paras["+STRING_Helper::iToStr(now_para_num)+"](convert) is out of obj.paras.size()="+STRING_Helper::iToStr(obj.paras.size())+")!");
                    return ACTION_ERROR_READ_PARAS_CONVERT_IS_OUT_OF_PARAS_SIZE;
                }
                BB_OBJ *convert_obj=(BB_OBJ *)obj.paras[now_para_num].data();
                string convert_table_path((const char*)BB_GET_OBJ_DATA(convert_obj), BB_GET_OBJ_DATA_LEN(convert_obj));
                BB_OBJ *convert_table_obj;
                BB_SIZE_T convert_table_obj_size;
                if (!this->GetTableObject(convert_table_path, &convert_table_obj, convert_table_obj_size)) {
                    this->SHOW_ERROR((string)"DoActionParas can't find convert_table(" + STRING_Helper::BinaryToHBCStr(obj.paras[now_para_num].c_str(), obj.paras[now_para_num].size(), "hex", true) + ")!");
                    return ACTION_ERROR_READ_CONVERT_TABLE_NOT_FOUND;
                }
                //建立请求数组
                BB_OBJ *request_table_obj;
                if (!this->_GetConvertToRequertTable(tableindex, convert_table_obj, bitmap_str, &request_table_obj)) {
                    this->SHOW_ERROR((string)"_GetConvertToRequertTable Error !");
                    return ACTION_ERROR_READ_CONVERT_TABLE_ACT_ERROR;		//bitmap_str返回值不一样。
                }
                bitmap_str.clear();
                bitmap_str.append((const char *)request_table_obj, BB_GET_OBJ_SIZE(request_table_obj)); //替换为新的bitmap，其中存放convert_cell_ids
            }
            //取得session_obj=>access_mask
            AVARTDB_AUTH_MASK_TYPE access_mask;
            if (obj.auth_mark.size() != 0)
                access_mask = obj.auth_mark;
            else {
                if (obj.session_obj != NULL)
                    access_mask = string((const char *)BB_GET_OBJ_DATA(obj.session_obj), BB_GET_OBJ_DATA_LEN(obj.session_obj));
                else
                    access_mask = AVARTDB_AUTH_MASK_NULL;
            }
            
            bool is_add_index = (obj.add_flag&ACTION_ADD_FLAG_ADD_INDEX) == ACTION_ADD_FLAG_ADD_INDEX;
            bool is_group = (obj.add_flag&ACTION_ADD_FLAG_VERICAL) == ACTION_ADD_FLAG_VERICAL ? false : true;
            //测试段3
            if ((nodeid_vec.size()==0 && TestFFFF(nodeid_str))|| nodeid_str == "@ALL_NODE" || conditions_str.size() != 0) {
                if (obj.packet_type == ACTION_PACKET_TYPE_DATAR_ARRAY)
                    obj.state = this->BuildBufferFromDB_AllNode(func_return, tableindex, bitmap_str, conditions_str, cursor_str, access_mask, obj.flag, is_add_index, is_group);//新模式
                else
                    obj.state = this->BuildBufferFromDB_AllNode(func_return, tableindex, bitmap_str, conditions_str, cursor_str, access_mask, obj.flag, is_add_index);//按conditins读取,旧模式
            }
            else{
                if (nodeid_vec.size()!=0){//多重ID的情况 ly_ids
                    string result; //ly_ids
                    BB_OBJ cell = BB_NULL;
                    for(BB_SIZE_T i=0; i<nodeid_vec.size(); i++){
                        result.clear();
                        obj.state = this->BuildBufferFromDB_OneNode(result, tableindex, nodeid_vec[i], bitmap_str, access_mask,obj.flag, is_add_index);//按指定nodeid读取,--不返回nodeid
                        if (obj.state!=0)
                            break;
                        BB_OBJ *content_obj = BB_NEW_ARRAY(&cell, NULL, NULL, 0, (void *)result.data(), result.size(), NULL);
                        func_return.append((const char *)content_obj, BB_GET_OBJ_SIZE(content_obj));
                        if(content_obj != NULL)
                            free(content_obj);
                    }
                }else
                    obj.state = this->BuildBufferFromDB_OneNode(func_return, tableindex, nodeid_str, bitmap_str, access_mask,obj.flag, is_add_index);//按指定nodeid读取,--不返回nodeid
            }
            if (obj.state != 0)
                return obj.state; //返回差错信息
        }
            break;
            
        case ACTION_WRITE:
        {
            //测试参数数量;
            BYTE type;
            unsigned int now_para_num = 0;
            if ((obj.paras.size()>5) && (obj.paras.size()<4)) {
                this->SHOW_ERROR((string)"DoActionMessage Write Rec Error-input paras must have 4 or 5 item(0:tablepath,*1:wformate(tableid), *2:formate(redturtableid) 3:data 4:conditions)!");
                return ACTION_ERROR_WRITE_DATA_PARA_NUM_ERROR;
            }
            
            string index_cut_str("");
            if ((obj.flag & BB_TABLE_REC_WR_BITIDS) == BB_TABLE_REC_WR_BITIDS) {//针对于nodeid单一情况
                
                /*if (obj.paras.size() != (now_para_num + 3)) {
                 this->SHOW_ERROR((string)"DoActionMessage Write Rec Error-input paras must have 4 item(0:tablepath,*1:wformate(tableid), *2:formate(redturtableid) 3:data)");
                 return ACTION_ERROR_WRITE_DATA_PARA_NUM_ERROR;
                 }*/
                //取得nodeid
                now_para_num++;
                BB_OBJ * index_str_obj = (BB_OBJ *)obj.paras[now_para_num].data();
                if (!BB_IS_BYTES_ALL(index_str_obj)) {
                    this->SHOW_ERROR((string)"DoActionMessage Write Rec Error-input paras[3]nodeid is not BB_STRING!");
                    return ACTION_ERROR_WRITE_PARAS3_NODEID_IS_NOT_BB_BYTES;
                }
                index_cut_str = string((const char*)BB_GET_OBJ_DATA(index_str_obj), BB_GET_OBJ_DATA_LEN(index_str_obj)); //取得nodeid
                bool is_return_index = TestFFFF(index_cut_str);
                if (index_cut_str.size() == 0) {
                    this->SHOW_ERROR((string)"DoActionMessage Write Rec ACTION_ERROR_WRITE_ACTION_NODEID_IS_EMPTY!");
                    return ACTION_ERROR_WRITE_ACTION_NODEID_IS_EMPTY;
                }
                //ly cycle;ly task;
                bool index_cycle_add=false;
                if (index_cut_str=="@CYCLE_ADD"){//取得AddInfo的累加记录 ly cycle;ly task;
                    //取得AddInfo->CYCLE_ADD
                    index_cycle_add=is_return_index=true;
                    UInt64 index_next;
                    if (!this->GetAddInfoUINT(tableindex,AVARTDB_ADDINFO_CYCLE_ADD_FIELD,index_next)){
                        this->SHOW_ERROR((string)"DoActionMessage Write Rec ACTION_ERROR_WRITE_ACTION_CYCLE_ADD_NOT_ADDINFO_DEFINED!");
                        return ACTION_ERROR_WRITE_ACTION_CYCLE_ADD_NOT_ADDINFO_DEFINED;
                    }
                    //递进index,如果超出则循环;
                    if (table_stru.Num!=0)
                        index_next++;//累加
                    BB_SIZE_T max_num_len=BB_GET_TABLE_MAX_NUM_LEN(&table_stru);
                    if (max_num_len==0){
                        this->SHOW_ERROR((string)"DoActionMessage Write Rec CYCLE ADD should have MaxLen attribute defined!");
                        return ACTION_ERROR_WRITE_ACTION_CYCLE_ADD_NO_MAX_LEN_DEFINED;//调到Index的第一条记录
                    }
                    if(index_next>=max_num_len){//循环处理
                        if (!BB_IS_UINT(table_stru.IndexObj)){//今后再次改进
                            this->SHOW_ERROR((string)"DoActionMessage Write Rec CYCLE ADD only support indexObj is uint!");
                            return ACTION_ERROR_WRITE_ACTION_CYCLE_ADD_NOT_ADDINFO_DEFINED;//调到Index的第一条记录
                        }
                            index_next=0; //针对没有索引的情况
                    }
                    if (index_next >=table_stru.Num){
                        index_next=0xffffffffffffffff; //提示添加
                    }
                    //生成新的Index_str 今后函数化
                    int index_r;
                    if ((index_r=CreateTableIndexStr(&table_stru,index_next,index_cut_str)) !=0){
                        this->SHOW_ERROR((string)"DoActionMessage Write Rec ACTION_ERROR_WRITE_ACTION_CYCLE_ADD_NOT_ADDINFO_DEFINED!");
                        return index_r;
                    }
                }
                
                //取得bitmap
                now_para_num++;
                BYTE bitmap_r; string bitmap_str;
                if (!((obj.flag&BB_TABLE_REC_WR_RETURN_TABLE) == BB_TABLE_REC_WR_RETURN_TABLE)) {//转换如果是"1,2,3,4,5",转换成"0x0102030405"
                    BB_OBJ_STRU cell_obj_table = BB_GET_OBJ_STRU(table_stru.CellObj);
                    //if ((bitmap_r = this->DoActionParas_GetBitmap((BB_OBJ *)obj.paras[now_para_num].data(), bitmap_str, obj.add_flag)) != 0) {
                    if ((bitmap_r = this->DoActionParas_GetBitmapExt(cell_obj_table,(BB_OBJ *)obj.paras[now_para_num].data(), bitmap_str, obj.add_flag)) != 0) {//ly cycle;ly
                        return bitmap_r;
                    }
                }
                //取得数据区
                now_para_num++;
                // = 0;
                BB_SIZE_T bufoffset = 0;
                BB_OBJ *data_obj = (BB_OBJ *)(obj.paras[now_para_num].data());//取得数据区
                if (BB_IS_OBJ_STRING(data_obj) || BB_GET_OBJ_TYPE(data_obj) != BB_ARRAY ) {//wformate test //ly change 2018
                    this->SHOW_ERROR((string)"DoActionMessage can't Write Rec error-input paras[4](data is not BB_ARRAY)!");
                    return ACTION_ERROR_WRITE_PARAS4_DATA_IS_NOT_ARRAY;
                }
                //取得conditions
                if (obj.paras.size()>(now_para_num+1)) {//ly task
                    if (!BB_IS_UINT(table_stru.IndexObj)){//今后再次改进
                        this->SHOW_ERROR((string)"DoActionMessage Write Rec CYCLE ADD only support indexObj is uint!");
                        return ACTION_ERROR_WRITE_ACTION_CONDITIONS_ONLY_FOR_UINT_INDEX;//调到Index的第一条记录
                    }
                    string condition("");
                    now_para_num++;
                    BB_OBJ_STRU conditions_stru;
                    BB_OBJ *conditions_obj = (BB_OBJ *)(obj.paras[now_para_num].data());
                    type = BB_GET_OBJ_TYPE(conditions_obj);
                    if (!(type==BB_ARRAY || type == BB_NULL || type == BB_BYTES || type == BB_BYTES_EXT)) {//wformate test
                        this->SHOW_ERROR((string)"DoActionMessage can't Write Rec error-input paras[2](conditions is not BB_NULL or BB_ARRAY_OBJ)!");
                        return ACTION_ERROR_WRITE_PARAS_CONDITIONS_IS_NOT_NULL_BYTES_BYTE_EXT;
                    };
                    conditions_stru = BB_GET_OBJ_STRU(conditions_obj);
                    string conditions = (type == BB_NULL) ? "" : string((const char *)conditions_stru.Data, conditions_stru.DataLen);
                    BB_SIZE_T input_num=STRING_Helper::scanUINTStr(index_cut_str);
                    BYTE conditions_rb=DoActionParas_CheckConditions(&table_stru,input_num,conditions);
                    if (conditions_rb!=0)
                        return conditions_rb;
                }
                //cout << STRING_Helper::HBCStrToHBCStr(bitmap_str, "char", "hex", true);
                int rw;
                if ((rw=this->WriteBufferToDB_OneNode((BYTE *)data_obj, bufoffset, tableindex, index_cut_str, bitmap_str, obj.flag))<0 ) {
                    //可能有nodeid后数据区为空的情况。
                    this->SHOW_ERROR((string)"ACTION_ERROR_WRITE_ACTION_ONE_NODE_ERROR! at nodeid:" + index_cut_str);
                    return ACTION_ERROR_WRITE_ACTION_ONE_NODE_ERROR-rw;
                };
                //ly cycle;ly task;
                if (index_cycle_add && rw==0){//如果已经超出,则循环从头
                    UInt64 index_num=STRING_Helper::scanUINTStr(index_cut_str);
                    this->SetAddInfoUINT(table_stru.AddInfoObj,AVARTDB_ADDINFO_CYCLE_ADD_FIELD,index_num);
                }
                
                if (is_return_index || ((obj.flag & BB_TABLE_REC_WR_CONFIRM_NODEID) == BB_TABLE_REC_WR_CONFIRM_NODEID)) {
                    BB_OBJ nodeid_obj[66];
                    BB_NEW_CELL(BB_BYTES, (const void *)index_cut_str.data(), index_cut_str.size(), nodeid_obj);//注：不需要创建内存,??超出size的处理
                    func_return.append((const char *)nodeid_obj, BB_GET_OBJ_SIZE(nodeid_obj));
                }
                this->SaveDBNode(tableindex, index_cut_str, FileQueue::SAVE_SYNC);  //存盘写入, 注：采用tableindex确保添加操作；
            }
            else {//1-9048870096
                //取得bitmap
                now_para_num++;
                BYTE bitmap_r; string bitmap_str;
                if (!((obj.flag&BB_TABLE_REC_WR_RETURN_TABLE) == BB_TABLE_REC_WR_RETURN_TABLE)) {//转换如果是"1,2,3,4,5",转换成"0x0102030405"
                    if ((bitmap_r = this->DoActionParas_GetBitmap((BB_OBJ *)obj.paras[now_para_num].data(), bitmap_str, obj.add_flag)) != 0) {
                        return bitmap_r;
                    }
                }
                
                //取得conditions
                string condition("");
                if ((obj.flag&BB_TABLE_REC_WR_QUERY_CONDITIONS) == BB_TABLE_REC_WR_QUERY_CONDITIONS) {
                    now_para_num++;
                    BB_OBJ_STRU conditions_stru;
                    BB_OBJ *conditions_obj = (BB_OBJ *)(obj.paras[now_para_num].data());
                    type = BB_GET_OBJ_TYPE(conditions_obj);
                    if (!(type == BB_NULL || type == BB_BYTES || type == BB_BYTES_EXT)) {//wformate test
                        this->SHOW_ERROR((string)"DoActionMessage can't Write Rec error-input paras[2](conditions is not BB_NULL or BB_ARRAY_OBJ)!");
                        return ACTION_ERROR_WRITE_PARAS_CONDITIONS_IS_NOT_NULL_BYTES_BYTE_EXT;
                    };
                    conditions_stru = BB_GET_OBJ_STRU(conditions_obj);
                    string conditions = (type == BB_NULL) ? "" : string((const char *)conditions_stru.Data, conditions_stru.DataLen);
                }
                
                //取得可能的增补formate
                if ((obj.flag&BB_TABLE_REC_WR_RETURN_TABLE) == BB_TABLE_REC_WR_RETURN_TABLE) {//特殊的补充bitmap;针对于table返回需要有bitmap有表格头内容，同时前面关闭bitmap的写入模式采用此处写入模式
                    now_para_num++;
                    if (obj.paras.size() < (now_para_num + 1)) {//wformate test
                        this->SHOW_ERROR((string)"DoActionMessage Write Rec Error-input paras must have 2 item(0:tableindex,*1:wformate(tableid),*2:conditions, *3:formate(redturtableid) 4:data)!");
                        return ACTION_ERROR_WRITE_RETURN_TABLE_PARA_NUM_ERROR;
                    }
                    BB_OBJ *bitmap_obj = (BB_OBJ *)(obj.paras[now_para_num].data());
                    if (BB_GET_OBJ_TYPE(bitmap_obj) != BB_BYTES_EXT) {//wformate test
                        this->SHOW_ERROR((string)"DoActionMessage can't Write Rec error-input paras[1](wformate is note BYTES[4])!");
                        return ACTION_ERROR_WRITE_RETURN_TABLE_WFORMATE_IS_NOT_PACKET_IN_BYTES_EXT;
                    }
                    bitmap_str = string((const char*)BB_GET_OBJ_DATA(bitmap_obj), BB_GET_OBJ_DATA_LEN(bitmap_obj));
                    if (BB_GET_OBJ_TYPE((BB_OBJ *)bitmap_str.data()) != BB_ARRAY) {//wformate test
                        this->SHOW_ERROR((string)"DoActionMessage can't Write Rec error-input paras[1](wformate is note Array Object)!");
                        return ACTION_ERROR_WRITE_RETURN_TABLE_WFORMATE_IS_NOT_ARRAY;
                    }
                }
                
                now_para_num++;
                //取得数据区
                BB_OBJ_STRU data_stru;
                if (obj.paras.size() != (now_para_num + 1)) {
                    this->SHOW_ERROR((string)"DoActionMessage Write Rec Error-input paras must have data item(0:tableindex,*1:wformate(tableid),*2:conditions, *3:formate(redturtableid) 4:data)!");
                    return ACTION_ERROR_WRITE_RETURN_TABLE_PARA_NUM_ERROR;
                }
                BB_OBJ *data_obj = (BB_OBJ *)(obj.paras[now_para_num].data());
                data_stru = BB_GET_OBJ_STRU(data_obj);
                type = BB_GET_OBJ_TYPE(data_obj);
                if (type == BB_NULL) {
                    this->SHOW_ERROR((string)"DoActionMessage can't Write Rec error-input paras[3] Is Null!");
                    return 0;
                }
                else if (type == BB_ARRAY && BB_GET_OBJ_TYPE(data_stru.CellObj) != BB_UINT8) {//wformate test
                    this->SHOW_ERROR((string)"DoActionMessage can't Write Rec error-input paras[3](data is not BB_ARRAY)!");
                    return ACTION_ERROR_WRITE_PARAS3_DATA_IS_NOT_ARRAY_BYTE;
                }
                else if (!(type == BB_BYTES || type == BB_BYTES_EXT || type==BB_ARRAY)) {
                    this->SHOW_ERROR((string)"DoActionMessage can't Write Rec error-input paras[3](data is not BB_ARRAY_BYTE)!");
                    return ACTION_ERROR_WRITE_PARAS3_NODEID_IS_NOT_BB_BYTES;
                }
                //写入操作
                BB_SIZE_T bufoffset = 0;
                BB_OBJ nodeid_obj[66];
                // ly test1
                //cout << "bitmap:" << STRING_Helper::HBCStrToHBCStr(bitmap_str, "char", "hex", true) << endl;
                for (BB_SIZE_T num = 0; bufoffset < data_stru.DataLen; num++)
                {
                    //取得nodeid
                    index_cut_str = "";
                    bufoffset += this->CutTableNodeidStr(index_cut_str, data_stru.Data + bufoffset, tableindex);//截取index_str；并递进;可以和index库中的CellWidth共享。
                    bool is_return_index = TestFFFF(index_cut_str);
                    int rw=0;
                    if (bufoffset<data_stru.DataLen && (rw=this->WriteBufferToDB_OneNode(data_stru.Data, bufoffset, tableindex, index_cut_str, bitmap_str, obj.flag))<0 ) {
                        //可能有nodeid后数据区为空的情况。
                        this->SHOW_ERROR((string)"ACTION_ERROR_WRITE_ACTION_ONE_NODE_ERROR! at num:" + STRING_Helper::iToStr(num));
                        return ACTION_ERROR_WRITE_ACTION_ONE_NODE_ERROR-rw;
                    }
                    
                    if (bufoffset > data_stru.DataLen) {
                        this->SHOW_ERROR((string)"DoActionMessage can't Write Rec error-input paras[3](data is not BB_ARRAY_BYTE)!");
                        return ACTION_ERROR_WRITE_ACTION_BUFOFFSET_OUTOFF_RANGE;
                    };
                    //添加index_str;index_cut_str可以用于返回nodeid
                    if ( is_return_index || ((obj.flag & BB_TABLE_REC_WR_CONFIRM_NODEID) == BB_TABLE_REC_WR_CONFIRM_NODEID)) {
                        BB_NEW_CELL(BB_BYTES, (const void *)index_cut_str.data(), index_cut_str.size(), nodeid_obj);//注：不需要创建内存,??超出size的处理
                        func_return.append((const char *)nodeid_obj, BB_GET_OBJ_SIZE(nodeid_obj));
                    }
                };
                this->SaveDB(tableindex);  //全局写入并存盘，注：采用tableindex确保添加操作；
            }
        }
            break;
        case ACTION_DELETE:
        {
            //测试参数数量;???尚未深入测试（table等有许多数据的情况，主要是从中间移动，所剩的内容最好重新用000替代才可以跟踪???）
            if (obj.paras.size()!=2){
                this->SHOW_ERROR((string )"DoActionMessage can't Delete Rec error-input paras must have 2 item(0:tableindex,2:id/id_array)!");
                return ACTION_ERROR_PACKET_PARAS_NUM_ERROR;
            }
            
            //取得id集
            BB_OBJ_STRU buff_stru =BB_GET_OBJ_STRU((BB_OBJ *)obj.paras[1].data());
            if (BB_IS_UINT(table_stru.IndexObj)) {
                this->SHOW_ERROR((string)"Can't Delete Rec[" + string((char *)buff_stru.Data, buff_stru.DataLen) + "] because IndexObj is Naturel Add");
                return ACTION_ERROR_DELETE_IDS_INDEX_IS_NATURL_ADD;
            }
            else {
                if (BB_IS_UINT(buff_stru.ThisObj) || buff_stru.Type == BB_BYTES || buff_stru.Type == BB_BYTES_EXT) {
                    //单节点删除 //ly table_dir
                    int r=this->DeleteFromDB_OneNode(table_stru,string((char *)buff_stru.Data, buff_stru.DataLen));
                    if (r!=0){
                        return r;
                    }
//                    if ((r = BB_DELETE_ARRAY_REC_BY_INDEX_STRU(&table_stru, buff_stru.Data, buff_stru.DataLen)) <0) {
//                        this->SHOW_ERROR((string)"Can't Delete Rec[" + string((char *)buff_stru.Data, buff_stru.DataLen) + "]" + " error(" + STRING_Helper::iToStr(r) + ")!");
//                        return ACTION_ERROR_DELETE_ID_NOT_FOUND;
//                    }
//                    this->SortObj->DeleteItem(table_stru.ThisObj, r); //ly sort
                }
                else if (buff_stru.Type == BB_ARRAY && BB_IS_UINT(buff_stru.CellObj)) {
                    //批量删除操作
                    BYTE *item;
                    BB_SIZE_T item_size;
                    for (unsigned int i = 0; i<buff_stru.Num; i++) {
                        BB_GET_ARRAY_REC_BY_NUM_STRU(&(buff_stru), i, &item, &item_size);
                        //ly table_dir
                        int r=this->DeleteFromDB_OneNode(table_stru,string((char *)buff_stru.Data, buff_stru.DataLen));
                        if (r<0){
                            return r;
                        }
                    }
                }
                else {
                    this->SHOW_ERROR((string)"DoActionMessage(),InputObj->paras[1] id/ids_array must be UINT|OBJECT_ARRAY!");
                    return ACTION_ERROR_DELETE_PARAS1_IS_NOT_UINT_BYTES_ARRAY;
                }
            }
        }
            this->SaveDB(tableindex); //注：采用tableindex确保删除操作；
            break;
        case ACTION_WRITE_EXT:
        {
            //测试参数数量;
            size_t now_para_num = 0;
            if (obj.paras.size() < 5) {
                this->SHOW_ERROR((string)"DoActionMessage Write Ext Error-input paras must have 4 item(0:tableindex,1:method(column[0,1]),2:NodeID, 3:extName, 4:data, 5:DestRouter/DestBinary)!");
                return ACTION_ERROR_PACKET_PARAS_NUM_ERROR;
            }
            
            //取得NodeID(MAC)
            string index_cut_str("");
            now_para_num++;
            BB_OBJ * index_str_obj = (BB_OBJ *)obj.paras[now_para_num].data();
            if (BB_GET_OBJ_TYPE(index_str_obj) != BB_BYTES) {
                this->SHOW_ERROR((string)"DoActionMessage Write Rec Error-input paras[3]nodeid is not BB_BYTES!");
                return ACTION_ERROR_EXT_NODEID_IS_NOT_BB_BYTES;
            }
            index_cut_str = string((const char*)BB_GET_OBJ_DATA(index_str_obj), BB_GET_OBJ_DATA_LEN(index_str_obj)); //取得nodeid
            
            //读取ext_name
            now_para_num++;
            BB_OBJ *ext_name_obj = (BB_OBJ *)(obj.paras[now_para_num].data());
            string ext_name = string((const char*)BB_GET_OBJ_DATA(ext_name_obj), BB_GET_OBJ_DATA_LEN(ext_name_obj));
            
            //取得method p_start,p_offset
            BB_SIZE_T p_start=0, p_offset=0;  //ly mac
            if ((obj.flag & BB_TABLE_REC_WR_BITSTARTEND) == BB_TABLE_REC_WR_BITSTARTEND) {//特殊的补充bitmap;针对于table返回需要有bitmap有表格头内容，同时前面关闭bitmap的写入模式采用此处写入模式
                now_para_num++;
                if (obj.paras.size()<=now_para_num){
                    this->SHOW_ERROR((string)"DoActionMessage Read paras[3] not set(paras[3] is start_end)!");
                    return ACTION_ERROR_EXT_METHOD_IS_NOT_OBJ_BYTES_8;
                }
                
                BB_OBJ *method_obj = (BB_OBJ *)(obj.paras[now_para_num].data());
                if ((BB_GET_OBJ_TYPE(method_obj) != BB_BYTES) && (BB_GET_OBJ_SIZE(method_obj) != 8)) {//wformate test
                    this->SHOW_ERROR((string)"DoActionMessage can't Write Rec error-input paras[1](wformate is note BYTES[4])!");
                    return ACTION_ERROR_EXT_METHOD_IS_NOT_OBJ_BYTES_8;
                }
                string method_str = string((const char*)BB_GET_OBJ_DATA(method_obj), BB_GET_OBJ_DATA_LEN(method_obj));
                //cout << STRING_Helper::HBCStrToHBCStr(method_str,"char","hex",true) << endl;
                string tmp = method_str.substr(0, 4);
                p_start = STRING_Helper::scanUINTStr(tmp);
                tmp = method_str.substr(4, 4);
                p_offset = STRING_Helper::scanUINTStr(tmp);//!!in linux is bighead should be error; pstart/p_offset exchang side; ??? no soluthion yet!
            }
            //预读取内容处理
            now_para_num++;
            BB_OBJ *data_obj = (BB_OBJ *)(obj.paras[now_para_num].data());//取得数据区

            //路由发送处理
#if !defined(__ANDROID__) && !defined(__APPLE__)
            now_para_num++;
            if (obj.paras.size() == (size_t)(now_para_num + 2)) {
                //路由发送处理
                if (!WIZBASE_create_routing_packet(func_return, WIZBASE_PATH_TYPE_URL_PATH, obj.paras[now_para_num], obj.paras[now_para_num + 1], 1))
                    return ACTION_ERROR_EXT_WRITE_DEVICE_CAN_NOT_OPEN_DEST_ROUTER;
                //在此添加发送处理...
            }
#endif
            //文件offset写入处理
            int rt;
            BB_SIZE_T tmp_size = BB_GET_OBJ_DATA_LEN(data_obj);
            BB_SIZE_T length = tmp_size < p_offset ? tmp_size : p_offset;
            BYTE open_mode=FileQueue::OPEN_MODE_TRUNC;
            if (p_offset==0xffffffff)
                open_mode=FileQueue::OPEN_MODE_TRUNC;

            if ((rt=this->SaveExt(table_stru, index_cut_str,ext_name,(BYTE *)BB_GET_OBJ_DATA(data_obj), p_start, length, open_mode))!=0) {
                return rt;
            }
        }
            break;
        case ACTION_READ_EXT:
        {
            //测试参数数量;
            //BYTE type;
            size_t now_para_num = 0;
            if (obj.paras.size() < 3) {
                this->SHOW_ERROR((string)"DoActionMessage Write Ext Error-input paras must have 4 item(0:tableindex,1:method(column[0,1]),2:NodeID!");
                return ACTION_ERROR_PACKET_PARAS_NUM_ERROR;
            }
            
            //取得NodeID(MAC)
            string index_cut_str("");
            now_para_num++;
            BB_OBJ * index_str_obj = (BB_OBJ *)obj.paras[now_para_num].data();
            if (BB_GET_OBJ_TYPE(index_str_obj) != BB_BYTES) {
                this->SHOW_ERROR((string)"DoActionMessage Write Rec Error-input paras[3]nodeid is not BB_BYTES!");
                return ACTION_ERROR_EXT_NODEID_IS_NOT_BB_BYTES;
            }
            index_cut_str = string((const char*)BB_GET_OBJ_DATA(index_str_obj), BB_GET_OBJ_DATA_LEN(index_str_obj)); //取得nodeid
            
            //读取ext_name
            now_para_num++;
            BB_OBJ *ext_name_obj = (BB_OBJ *)(obj.paras[now_para_num].data());
            string ext_name= string((const char*)BB_GET_OBJ_DATA(ext_name_obj), BB_GET_OBJ_DATA_LEN(ext_name_obj));
            
            //取得可能的增补formate
            BB_SIZE_T p_start, p_offset;
            if ((obj.flag & BB_TABLE_REC_WR_BITSTARTEND) == BB_TABLE_REC_WR_BITSTARTEND && (obj.paras.size()!=5)) {//特殊的补充bitmap;针对于table返回需要有bitmap有表格头内容，同时前面关闭bitmap的写入模式采用此处写入模式
                now_para_num++;
                if (obj.paras.size()<=now_para_num){
                    this->SHOW_ERROR((string)"DoActionMessage Read paras[3] not set(paras[3] is start_end)!");
                    return ACTION_ERROR_EXT_METHOD_IS_NOT_OBJ_BYTES_8;
                }

                BB_OBJ *method_obj = (BB_OBJ *)(obj.paras[now_para_num].data());
                if ((BB_GET_OBJ_TYPE(method_obj) != BB_BYTES) && (BB_GET_OBJ_SIZE(method_obj) != 8)) {//wformate test
                    this->SHOW_ERROR((string)"DoActionMessage can't Read Rec error-input paras[3](wformate is not BYTES[8])!");
                    return ACTION_ERROR_EXT_METHOD_IS_NOT_OBJ_BYTES_8;
                }
                string method_str = string((const char*)BB_GET_OBJ_DATA(method_obj), BB_GET_OBJ_DATA_LEN(method_obj));
                string tmp = method_str.substr(0, 4);
                p_start = STRING_Helper::scanUINTStr(tmp);
                tmp = method_str.substr(4, 4);
                p_offset = STRING_Helper::scanUINTStr(tmp);
            }else if( (obj.paras.size()==5) || (obj.flag & BB_TABLE_REC_WR_QUERY_CURSOR) == BB_TABLE_REC_WR_QUERY_CURSOR){
                now_para_num++;
                if (obj.paras.size()<=now_para_num){
                    this->SHOW_ERROR((string )"DoActionParas() Error Read input paras["+STRING_Helper::uToStr(now_para_num)+"](cursor) is out of obj.paras.size()="+STRING_Helper::uToStr(obj.paras.size())+")!");
                    return ACTION_ERROR_READ_PARAS_CURSOR_IS_OUT_OF_PARAS_SIZE;
                }
                BB_OBJ *cursor_obj = (BB_OBJ *)(obj.paras[now_para_num].data());
                string cur_str = string((const char*)BB_GET_OBJ_DATA(cursor_obj), BB_GET_OBJ_DATA_LEN(cursor_obj));
                vector <string> tmp;
                STRING_Helper::stringSplit(cur_str, tmp, ",");
                if (tmp.size() != 2 || !STRING_Helper::CheckNum(tmp[0]) || !STRING_Helper::CheckNum(tmp[1])) {
                    this->SHOW_ERROR((string)"DoActionParas() Error Read input paras[" + cur_str + "] is error!");
                    return ACTION_ERROR_READ_PARAS_CURSOR_IS_INCORRECT1;
                }
                p_start = STRING_Helper::ParseInt(tmp[0]);
                p_offset= STRING_Helper::ParseInt(tmp[1]);
            }
            //是否从末尾读取
            bool from_start=true;
            now_para_num++;
            if (now_para_num < obj.paras.size()){
                BB_OBJ *direct_obj = (BB_OBJ *)(obj.paras[now_para_num].data());
                const char *direct_p =(const char*)BB_GET_OBJ_DATA(direct_obj);
                if (strncmp(direct_p,"END",BB_GET_OBJ_DATA_LEN(direct_obj))==0)
                    from_start=false;
            }

            //文件offset读处理
            BYTE * buf = NULL;
            int rt;
            if ((rt = this->LoadExt(table_stru, index_cut_str,ext_name,&buf, p_start, p_offset, 0, from_start))!=0) {
                return rt;
            }
            
            //清空缓存
            BB_OBJ *result_obj=BB_NEW_OBJ_STRING((BYTE *)buf,p_offset, NULL);
            func_return = string((const char *)result_obj,BB_GET_OBJ_SIZE(result_obj));
            if (result_obj)
            	free(result_obj);
            //func_return=string((const char*)buf,p_offset);
            if (buf)
                free(buf);
        }
            break;
        case ACTION_CREATE_OBJ_BY_XML_MODEL:
            //创建xml新目录模板
        {
            //测试参数数量;
            if (obj.paras.size()!=4){//ly dir
                this->SHOW_ERROR((string )"buff_obj's number must be 4: 0:tableindex,1:source path; 2:dest parent path; 3:dest node_name !");
                return ACTION_ERROR_PACKET_PARAS_NUM_ERROR;
            }
            //处理操作
            
            ParseXml_Node node1=this->_ActParseXml->GetNodeByXpath(this->GetObjDataStr(obj.paras[1]).c_str());//源节点
            ParseXml_Node node2=this->_ActParseXml->GetNodeByXpath(this->GetObjDataStr(obj.paras[2]).c_str());//目标父
            BB_OBJ * new_node_id=(BB_OBJ *)(obj.paras[3].data());//目标id名
            string new_name_id=(string)AVARTDB_CREATE_OBJ_HEAD_STR+STRING_Helper::BinaryToHBCStr((const char*)BB_GET_OBJ_DATA(new_node_id),BB_GET_OBJ_DATA_LEN(new_node_id),"hex",false);
            tmp1=this->GetObjDataStr(obj.paras[2])+(string)"/"+new_name_id;
            ParseXml_Node node3=this->_ActParseXml->GetNodeByXpath(tmp1.c_str());
            ParseXml_Node node4=this->_ActParseXml->GetNodeByXpath((this->GetObjDataStr(obj.paras[2])+"/_Index/_Content").c_str());//取得index索引内容
            
            if (!node1){//源节点不存在
                this->SHOW_ERROR((string)"DoActionMessage(),InputObj->data buffer object xml source path"+obj.paras[1]+"not exit !");
                return ACTION_ERROR_CLONE_XML_SRC_PATH_NOT_EXIT;
            }
            if (!node2){//目标父节点不存在
                this->SHOW_ERROR((string)"DoActionMessage(),InputObj->data buffer object xml dest parent path"+obj.paras[2]+"not exit!!");
                return ACTION_ERROR_CLONE_XML_DEST_PARENT_NOT_EXIT;
            }
            if (node3){//目标节点已存在
                this->SHOW_ERROR((string)"DoActionMessage(),InputObj->data buffer object xml dest node"+tmp1+"already exit!!");
                return ACTION_ERROR_CLONE_XML_DEST_NODE_ALREADY_EXT;
            }
            
            //尝试添加Index索引
            if (node4){
                string tmp4=new_name_id.substr(strlen(AVARTDB_CREATE_OBJ_HEAD_STR)),tmp3=node4.text().get();
                if (tmp3.size()==0){
                    tmp4="[{0x"+tmp4+"}]";
                    node4.text().set(tmp4.c_str());
                }else{
                    size_t n=tmp3.rfind("]");
                    if (n!=string::npos){
                        tmp4=tmp3.substr(0,n)+",{0x"+tmp4+"}]";
                        node4.text().set(tmp4.c_str());
                    }else{
                        this->SHOW_ERROR((string)"DoActionMessage(),InputObj->data buffer object xml dest parent path index must '[***,****]'!");
                        return ACTION_ERROR_CLONE_XML_DEST_PARENT_INDEX_CFG_ERROR;
                    }
                }
            }else{
                this->SHOW_ERROR((string)"DoActionMessage(),InputObj->data buffer object xml dest parent path index must '[***,****]'!");
                return ACTION_ERROR_CLONE_XML_DEST_PARENT_INDEX_CFG_ERROR;
            }
            
            //克隆操作
            ParseXml_Node clone_node=node2.append_copy(node1);
            //更名操作
            clone_node.set_name(new_name_id.c_str());
            //永久化存储
            this->_ActParseXml->SaveFile("UTF-8"); //待扩充。
            
            //创建对象空间
            BB_OBJ *tmp_obj = this->AvartarNewObj(tmp1);  //??是否清楚内存空间
            
            //对象指针拷贝
            //BB_SET_UINT_VALUE(obj.input_obj,(UInt64)tmp_obj);
            BB_APPEND_ARRAY_REC_BY_INDEX_STRU(&table_stru,(BYTE *)&tmp_obj,sizeof(tmp_obj),(BYTE *)BB_GET_OBJ_DATA(new_node_id),BB_GET_OBJ_DATA_LEN(new_node_id));
            //this->SaveDB(ta);
        }
            break;
        case ACTION_DELETE_OBJ_XML_CLUSTER:
            //创建xml新目录模板
        {
            //测试参数数量;
            if (obj.paras.size()!=3){//ly dir
                this->SHOW_ERROR((string )"buff_obj's number must be 3: 0:tableindex,1:del parent path; 2:dest node_name !");
                return ACTION_ERROR_PACKET_PARAS_NUM_ERROR;
            }
            //处理操作
            ParseXml_Node node1=this->_ActParseXml->GetNodeByXpath(this->GetObjDataStr(obj.paras[1]).c_str());//目标父
            BB_OBJ * new_node_id=(BB_OBJ *)(obj.paras[2].data());//目标id名
            string new_name_id=(string)AVARTDB_CREATE_OBJ_HEAD_STR+STRING_Helper::BinaryToHBCStr((const char*)BB_GET_OBJ_DATA(new_node_id),BB_GET_OBJ_DATA_LEN(new_node_id),"hex",false);
            tmp1=this->GetObjDataStr(obj.paras[1])+(string)"/"+new_name_id;
            ParseXml_Node node2=this->_ActParseXml->GetNodeByXpath(tmp1.c_str());//取得目标节点
            ParseXml_Node node3=this->_ActParseXml->GetNodeByXpath((this->GetObjDataStr(obj.paras[1])+"/_Index/_Content").c_str());//取得目标父index索引内容
            
            if (!node1){//目标父节点不存在
                this->SHOW_ERROR((string)"DoActionMessage(),InputObj->data buffer object xml dest parent path"+obj.paras[2]+"not exit!!");
                return ACTION_ERROR_DELETE_XML_CLUSTERL_DEST_PARENT_NOT_EXIT;
            }
            if (!node2){//目标节点不存在
                this->SHOW_ERROR((string)"DoActionMessage(),InputObj->data buffer object xml dest node"+tmp1+"already exit!!");
                return ACTION_ERROR_DELETE_XML_CLUSTERL_DEST_NODE_NOT_EXIT;
            }
            //尝试删除index
            if (node3){
                string tmp4=new_name_id.substr(strlen(AVARTDB_CREATE_OBJ_HEAD_STR));
                string tmp3=node3.text().get(),tmp5=(string)"{0x"+tmp4+"}";
                if (tmp3.find(tmp5)!=string::npos){
                    STRING_Helper::strReplace(tmp3,(string)","+tmp5,"");
                    STRING_Helper::strReplace(tmp3,tmp5,"");
                    node3.text().set(tmp3.c_str());
                }else{
                    this->SHOW_ERROR((string)"DoActionMessage(),InputObj->data buffer object xml dest parent path index must be '[***,****]'");
                    return ACTION_ERROR_DELETE_XML_DEST_PARENT_INDEX_CFG_ERROR;
                }
            }
            
            //删除操作
            if (!node1.remove_child(node2)){
                this->SHOW_ERROR((string)"DoActionMessage(),InputObj->data buffer object xml dest node"+tmp1+"already exit!!");
                return ACTION_ERROR_DELETE_XML_CLUSTERL_DEL_XMLNODE_ERROR;
            }
            //永久化存储
            this->_ActParseXml->SaveFile("UTF-8"); //待扩充。
            
            //对象指针删除
            if ((r=BB_DELETE_ARRAY_REC_BY_INDEX_STRU(&table_stru,(BYTE *)BB_GET_OBJ_DATA(new_node_id),BB_GET_OBJ_DATA_LEN(new_node_id))) <0 ){
                this->SHOW_ERROR((string )"Can't Delete Rec["+new_name_id+"]"+" error("+STRING_Helper::iToStr(r)+")!");
                return ACTION_ERROR_DELETE_XML_CLUSTER_ARRAY_ID_NOT_FOUND;
            }
            //this->SaveDB(table_stru.ThisObj);
        }
            break;
        case ACTION_GET_TABLE_INFO:
            //创建xml新目录模板
        {
            int now_para_num = 0;
            
            //测试参数数量;
            if (obj.paras.size() != 2) {//ly dir
                this->SHOW_ERROR((string)"buff_obj's number must be 32: 0:tableindex,1:taskid;!");
                return ACTION_ERROR_PACKET_PARAS_NUM_ERROR;
            }
            
            //取得session_obj=>access_mask
            AVARTDB_AUTH_MASK_TYPE access_mask;
            if (obj.auth_mark.size() != 0)
                access_mask = obj.auth_mark;
            else {
                if (obj.session_obj != NULL)
                    access_mask = string((const char *)BB_GET_OBJ_DATA(obj.session_obj), BB_GET_OBJ_DATA_LEN(obj.session_obj));
                else
                    access_mask = AVARTDB_AUTH_MASK_NULL;
            }
            
            //取得bitmap//转换如果是"1,2,3,4,5",转换成"0x0102030405"
            now_para_num++;
            BYTE bitmap_r; string bitmap_str;
            if ((bitmap_r = this->DoActionParas_GetBitmap((BB_OBJ *)obj.paras[now_para_num].data(), bitmap_str, obj.add_flag)) != 0) {
                if (bitmap_r==ACTION_ERROR_PACKET_PARAS_BITMAP_IS_NO_UINT8_STRING){
                    obj.state = this->DoActionParas_ACT_INFO_BY_NAME(table_stru, bitmap_str, access_mask, func_return);
                }else{
                    return bitmap_r;
                }
            }else{
                obj.state = this->DoActionParas_ACT_INFO(table_stru, bitmap_str, access_mask, func_return);
            }
        }
            break;
        case ACTION_GET_CLUSTER_INFO:
        {
            //参数拆解
            
            //判断是否为ARRAY(cluster)ACTION_GET_CLUSTER_INFO_TYPE
            //循环当前cluster对象的子tableindex
            //递归调用当前函数
            //测试参数数量;
            BYTE type;
            unsigned int now_para_num = 0;
            if (obj.paras.size()<3) {
                this->SHOW_ERROR((string)"DoActionParas can't Read Rec error-input paras must have 4 item(0:tableindex,1:wformate,2:condition/nodeids,*3:page_cursor,*4:convert_table)!");
                return ACTION_ERROR_PACKET_PARAS_NUM_ERROR;
            }
            
            //取得conditions
            string nodeid_str(""), conditions_str("");
            now_para_num++;
            if ((obj.flag&BB_TABLE_REC_WR_QUERY_CONDITIONS) == BB_TABLE_REC_WR_QUERY_CONDITIONS) {
                //如果有conditions
                if (now_para_num >= obj.paras.size()) {
                    this->SHOW_ERROR((string)"DoActionParas() Error Read input paras[" + STRING_Helper::iToStr(now_para_num) + "](conditions) is out of obj.paras.size()=" + STRING_Helper::iToStr(obj.paras.size()) + ")!");
                    return ACTION_ERROR_READ_PARAS_CONDITIONS_IS_OUT_OF_PARAS_SIZE;
                }
                BB_OBJ_STRU conditions_stru;
                BB_OBJ *conditions_obj = (BB_OBJ *)(obj.paras[now_para_num].data());
                type = BB_GET_OBJ_TYPE(conditions_obj);
                if (!(type == BB_NULL || type == BB_BYTES || type == BB_BYTES_EXT)) {//wformate test
                    this->SHOW_ERROR((string)"DoActionParas() Error Read input paras[" + STRING_Helper::iToStr(now_para_num) + "](conditions) is not BB_NULL or BB_ARRAY_OBJ)!");
                    return ACTION_ERROR_READ_PARAS_CONDITIONS_IS_NOT_NULL_BYTES_OR_BYTE_EXT;
                };
                conditions_stru = BB_GET_OBJ_STRU(conditions_obj);
                conditions_str = (type == BB_NULL) ? "" : string((const char *)conditions_stru.Data, conditions_stru.DataLen);//如果有conditions ly add2
            }
            else {
                //如果有nodeids
                BB_OBJ *nodeid_obj = (BB_OBJ *)(obj.paras[now_para_num].data());
                BYTE type = BB_GET_OBJ_TYPE(nodeid_obj);
                if (!(type == BB_NULL || type == BB_BYTES || type == BB_BYTES_EXT || BB_IS_OBJ_STRING(nodeid_obj))) {//wformate test
                    this->SHOW_ERROR((string)"DoActionParas() Error Read input paras[2](nodeid) is not BB_NULL or BB_ARRAY_OBJ)!");
                    return ACTION_ERROR_READ_PARAS_NODEID_IS_NOT_NULL_BYTES_OR_BYTE_EXT;
                };
                nodeid_str = (type == BB_NULL) ? "" : string((const char *)BB_GET_OBJ_DATA(nodeid_obj), BB_GET_OBJ_DATA_LEN(nodeid_obj));//如果无nodeids则参数固定为nodeid
            }
            
            //取得bitmap
            //转换如果是"1,2,3,4,5",转换成"0x0102030405"
            now_para_num++;
            BYTE bitmap_r; string bitmap_str;
            if ((bitmap_r = this->DoActionParas_GetBitmap((BB_OBJ *)obj.paras[now_para_num].data(), bitmap_str, obj.add_flag)) != 0) {
                return bitmap_r;
            }
            
            //取得page/cursor; 页码
            string cursor_str("");
            if ((obj.flag&BB_TABLE_REC_WR_QUERY_CURSOR) == BB_TABLE_REC_WR_QUERY_CURSOR) {
                if ((now_para_num++) >= obj.paras.size()) {
                    this->SHOW_ERROR((string)"DoActionParas() Error Read input paras[" + STRING_Helper::iToStr(now_para_num) + "](cursor) is out of obj.paras.size()=" + STRING_Helper::iToStr(obj.paras.size()) + ")!");
                    return ACTION_ERROR_READ_PARAS_CURSOR_IS_OUT_OF_PARAS_SIZE;
                }
                //BB_OBJ *cursor_obj = (BB_OBJ *)obj.paras[now_para_num].data();
                //cursor_str.append((const char*)BB_GET_OBJ_DATA(cursor_obj), BB_GET_OBJ_DATA_LEN(cursor_obj));
                BYTE cur_r;
                if ((cur_r = this->DoActionParas_GetCursor((BB_OBJ *)obj.paras[now_para_num].data(), cursor_str)) != 0)
                    return cur_r;
            }
            
            //取得session_obj=>access_mask
            AVARTDB_AUTH_MASK_TYPE access_mask;
            if (obj.auth_mark.size() != 0)
                access_mask = obj.auth_mark;
            else {
                if (obj.session_obj != NULL)
                    access_mask = string((const char *)BB_GET_OBJ_DATA(obj.session_obj), BB_GET_OBJ_DATA_LEN(obj.session_obj));
                else
                    access_mask = AVARTDB_AUTH_MASK_NULL;
            }
            
            //读取操作
            bool is_add_index = (obj.add_flag&ACTION_ADD_FLAG_ADD_INDEX) == ACTION_ADD_FLAG_ADD_INDEX;
            if (TestFFFF(nodeid_str) || nodeid_str=="@ALL_NODE" || conditions_str.size() != 0)
                obj.state = this->DoActionParas_SonAddInfo(func_return, tableindex, bitmap_str, conditions_str, cursor_str, access_mask,obj.flag, is_add_index);//按conditins读取
            else {
                BB_SIZE_T node_num=0;
                if (table_stru.IndexObj != NULL) {
                    int node_num_int;  //ly mac
                    if ((node_num_int = BB_GET_ARRAY_NUM_BY_INDEX_STRU(&table_stru, (BYTE *)nodeid_str.data(), nodeid_str.size())) < 0) {
                        this->SHOW_ERROR((string)"DoActionParas() Error Read NodeIds[" + STRING_Helper::iToStr(node_num) + "!");
                        return ACTION_ERROR_READ_PARAS_NODEID_IS_IS_NOT_FOUND_IN_DB;
                    };
                    node_num = node_num_int;
                }else {
                    node_num = STRING_Helper::scanUINTStr(nodeid_str);
                }
                obj.state = this->DoActionParas_ACT_INFO(table_stru, bitmap_str, access_mask,func_return, node_num);//按指定nodeid读取,--不返回nodeid
            }
            
            break;
        }
            
        default:
            this->SHOW_ERROR((string)"InputObj->act("+STRING_Helper::iToStr((int)obj.act)+") not be supported!");
            return ACTION_ERROR_PACKET_HEAD_ACT_NOT_SUPPORT;
    }
    //生成返回对象
    if (obj.packet_type == ACTION_PACKET_TYPE_DATAR_OBJECT && ((obj.flag & BB_TABLE_REC_WR_BITIDS) == BB_TABLE_REC_WR_BITIDS)) {
        //只是针对BITIDS的数组结果，其他都用末尾的BuildResultMessage();
        string return_tmp_str = func_return;
        func_return = "";
        vector <string> send_data;
        //bitmap对象
        send_data.push_back(obj.paras[1]);
        //生成结果对象
        BYTE cell_null = 0x0;
        BB_OBJ *r_obj = BB_NEW_ARRAY(&cell_null, NULL, NULL, 0, (void *)return_tmp_str.data(), return_tmp_str.size(), NULL); //添加8002**00外部对象数组包
        send_data.push_back(string((const char*)r_obj, BB_GET_OBJ_SIZE(r_obj))); //ly :提速
        if (r_obj != NULL)
            free(r_obj);
        obj.state = (obj.state == 0 ? 0xFF : obj.state);
        this->BuildResultMessageVector(obj, send_data, func_return, add_paras, false, false);
        return 0;
    }else if (obj.packet_type==ACTION_PACKET_TYPE_DATAR_OBJECT){//打包回传，包括后半段差错信息
        string return_tmp_str=func_return;
        func_return="";
        vector <string> send_data;
        //obj.paras.clear();//清空原始请求参数，返回用户参数|
        if ((obj.act==ACTION_READ)&&((obj.flag | BB_TABLE_REC_WR_BITIDS_CONVERT)==BB_TABLE_REC_WR_BITIDS_CONVERT))
            send_data.push_back(bitmap_str);
        send_data.push_back(return_tmp_str); //ly :提速
        obj.state=(obj.state==0 ? 0xFF: obj.state);
        this->BuildResultMessageVector(obj,send_data,func_return,add_paras);
    }
    else if (obj.packet_type == ACTION_PACKET_TYPE_DATAR_ARRAY) {
        return 0;//this->BuildResultMessageArray(obj, func_return, func_return, add_paras);
    }
    
    return	0;
}

/*
BYTE AvartarDB_Action::DoActionParas_GetTableIndex(BB_OBJ *tableindex_obj, string &tableindex) {
    
    if (BB_IS_OBJ_STRING(tableindex_obj)) {
        string temp1 = string((const char*)BB_GET_OBJ_DATA(tableindex_obj), BB_GET_OBJ_DATA_LEN(tableindex_obj));
        vector <string> tmp;
        STRING_Helper::stringSplit(temp1, tmp, "/");
        string tmp1;
        for (unsigned int i = 0; i < tmp.size(); i++) {
            if (tmp[i].size() == 0)
                continue;
            if (tmp[i] == "@CELL_INFO") {
                //tmp1 = BB_SEARCH_CELL_STX;
                tableindex += BB_SEARCH_CELL_STX;
            }else if (tmp[i] == "@ADD_INFO") {
                //tmp1 = BB_SEARCH_CELL_STX;
                tableindex += (char)BB_SEARCH_ADDINFO_STX;
            }else {
                if	(!STRING_Helper::CheckNum(tmp[i])) {
                    this->SHOW_ERROR((string)"DoActionParas() Error Read input paras[0] is not '0xNumber')!");
                    return ACTION_ERROR_PACKET_PARAS0_TABLE_PATH_ERROR;
                }
                tmp1 = STRING_Helper::HBCStrToHBCStr(tmp[i], "hex", "char", false);
                if (!this->AddSearchCell(tableindex, 0, tmp1)) {
                    this->SHOW_ERROR((string)"DoActionParas() Error Read input paras[0] index path error!");
                    return ACTION_ERROR_PACKET_PARAS0_TABLE_PATH_ERROR;
                }
            }
        }
        if (tableindex.size() == 0)
            tableindex = STRING_Helper::HBCStrToHBCStr("0x00", "hex", "char", false);//对于根目录的处理
    }
    else {
        if (BB_GET_OBJ_TYPE(tableindex_obj) != BB_BYTES && BB_GET_OBJ_TYPE(tableindex_obj) != BB_BYTES_EXT) {//wformate test
            this->SHOW_ERROR((string)"DoActionParas() can't Write Rec error-input paras[0](tableindex is not BYTES)!");
            return ACTION_ERROR_PACKET_PARAS0_TABLE_IS_NOT_HEX_BYTES;
        }
        tableindex = string((const char*)BB_GET_OBJ_DATA(tableindex_obj), BB_GET_OBJ_DATA_LEN(tableindex_obj));
    }
    return 0;
}*/



BYTE AvartarDB_Action::DoActionParas_GetTableIndex(BB_OBJ *tableindex_obj, string &tableindex) {
    if (BB_IS_OBJ_STRING(tableindex_obj)) {
        string temp1 = string((const char*)BB_GET_OBJ_DATA(tableindex_obj), BB_GET_OBJ_DATA_LEN(tableindex_obj));
        return this->GetPathTableIndex(temp1,tableindex);
    }
    else {
        if (BB_GET_OBJ_TYPE(tableindex_obj) != BB_BYTES && BB_GET_OBJ_TYPE(tableindex_obj) != BB_BYTES_EXT) {//wformate test
            this->SHOW_ERROR((string)"DoActionParas() can't Write Rec error-input paras[0](tableindex is not BYTES)!");
            return ACTION_ERROR_PACKET_PARAS0_TABLE_IS_NOT_HEX_BYTES;
        }
        tableindex = string((const char*)BB_GET_OBJ_DATA(tableindex_obj), BB_GET_OBJ_DATA_LEN(tableindex_obj));
        return 0;
    }
}

BYTE AvartarDB_Action::DoActionParas_CheckConditions(BB_OBJ_STRU *table_stru, BB_SIZE_T num, string &conditions){
    //条件判断
    if (conditions.size()!=0){//读取符合条件记录情况rec_access_mask
        BB_OBJ *p_obj;
        int i,err_code;
        bool result=true;
        //循环判断所有的条件。
        for(p_obj=(BB_OBJ *)conditions.data(), i=0; (const char *)p_obj<(conditions.c_str() + conditions.size()); p_obj+=BB_GET_OBJ_SIZE(p_obj),i++){
            bool chk_result=BB_TABLE_PARA_CONDITION_TEST(table_stru,num,p_obj,err_code);
            if (err_code<0){
                this->SHOW_ERROR((string)"DoActionParas_CheckConditions(): DB load err->rec="+STRING_Helper::iToStr(num)+"or ("+conditions+") error->"+STRING_Helper::iToStr(i));
                return ACTION_ERROR_WRITE_ACTION_ERROR_BY_CONDITIONS_KEY;
            }
            if(!chk_result){//只适合于小范围内比较。大范围比较，需要把内部的取值还是搬出。
                result=false;
                break;//对比key_value和value内容。
            }
        }
        if (!result){
            return ACTION_ERROR_WRITE_ACTION_CHECK_CONDITIONS_KEY_FALSE;
        }
    }
    return 0;
}

BYTE AvartarDB_Action::GetPathTableIndex(string &temp1, string &tableindex) {//ly task
    vector <string> tmp;
    STRING_Helper::stringSplit(temp1, tmp, "/");
    string tmp1;
    for (unsigned int i = 0; i < tmp.size(); i++) {
        if (tmp[i].size() == 0)
        continue;
        if (tmp[i] == "@CELL_INFO") {
            //tmp1 = BB_SEARCH_CELL_STX;
            tableindex += BB_SEARCH_CELL_STX;
        }else if (tmp[i] == "@ADD_INFO") {
            //tmp1 = BB_SEARCH_CELL_STX;
            tableindex += (char)BB_SEARCH_ADDINFO_STX;
        }else if (tmp[i] == "@INDEX_INFO") {
            //tmp1 = BB_SEARCH_CELL_STX;
            tableindex += (char)BB_SEARCH_INDEX_STX; //ly table_dir
        }else {
            if    (!STRING_Helper::CheckNum(tmp[i])) {
                this->SHOW_ERROR((string)"DoActionParas() Error Read input paras[0] is not '0xNumber')!");
                return ACTION_ERROR_PACKET_PARAS0_TABLE_PATH_ERROR;
            }
            tmp1 = STRING_Helper::HBCStrToHBCStr(tmp[i], "hex", "char", false);
            if (!this->AddSearchCell(tableindex, 0, tmp1)) {
                this->SHOW_ERROR((string)"DoActionParas() Error Read input paras[0] index path error!");
                return ACTION_ERROR_PACKET_PARAS0_TABLE_PATH_ERROR;
            }
        }
    }
    if (tableindex.size() == 0)
    tableindex = STRING_Helper::HBCStrToHBCStr("0x00", "hex", "char", false);//对于根目录的处理
    return 0;
}

//ly table_redir
BYTE AvartarDB_Action::CheckTableIndexRedir(BB_OBJ_STRU *table_stru, string &tableindex) {//ly task
    string redir=this->GetAddInfo(table_stru->AddInfoObj,"REDIR");
    if (redir.size()!=0){
        tableindex=redir;
        return true;
    }else
        return false;
}
//ly table_redir
int AvartarDB_Action::CheckGetTableFilter(BB_OBJ_STRU &table_stru,string &conditions) {//ly task
    string filter=this->GetAddInfo(table_stru.AddInfoObj,"FILTER");
    if (filter.size()==0)
        return 0;
   
    BB_OBJ_STRU filter_arr_stru=BB_GET_OBJ_STRU((BYTE *)(filter.data()));
    if(filter_arr_stru.LoadError){
        this->SHOW_ERROR((string)"BuildBufferFromDB(), Filter Load table error!");
        return -1;
    }
    //读取和转换
    string tmp_filter_item2,filter_item2;
    PV_AddParas_T add_paras;
    if (!this->GetArrayRecString(filter_arr_stru, 2, filter_item2)){
        this->SHOW_ERROR((string)"BuildBufferFromDB(), Filter Load table error!");
        return -1;
    }
    string r=GB_Gateway->ParseValue(filter_item2,tmp_filter_item2,add_paras);
    if (r.size()!=0){
        this->SHOW_ERROR((string)"BuildBufferFromDB(), Filter Load table error("+r+")!");
        return -1;
    }        
    
    //重新生成
    if(tmp_filter_item2!=filter_item2){
        BB_OBJ *filter_obj=BB_NEW_BYTES_ALL(tmp_filter_item2.data(), tmp_filter_item2.size(), NULL);
        BB_OBJ *tmp_obj=BB_NEW_REPLACE_ARRAY_REC_STRU(&filter_arr_stru,2,filter_obj,BB_GET_OBJ_SIZE(filter_obj),NULL);
        if (tmp_obj!=NULL){
            conditions+=string((const char *)tmp_obj,BB_GET_OBJ_SIZE(tmp_obj));
            free(tmp_obj);
        }
        if (filter_obj!=NULL)
            free(filter_obj);
    }
    return 1;
}

BB_OBJ *AvartarDB_Action::CheckGetTableRedir(string &tableindex,BB_OBJ_STRU &table_stru) {//ly task //ly table_redir
    BB_OBJ *table_obj;
    BB_SIZE_T table_obj_size;
    string redir;
    
    if (tableindex.size()==0)
        return NULL;
    
    //对@*的预处理;
    BYTE end_char=tableindex.at(tableindex.size()-1);
    if ((end_char & 0xc0)!=0){//如果是：@INDEX,@ADDINFO,@CELL
        string tableindex_tmp=tableindex.substr(0,tableindex.size()-1);
        if (!this->GetTableObject(tableindex_tmp,&table_obj,table_obj_size)){
            this->SHOW_ERROR((string)"CheckGetTableRedir(),Load table @* error!");
            return NULL;
        }
        BB_OBJ_STRU table_stru_tmp=BB_GET_OBJ_STRU(table_obj);
        redir=this->GetAddInfo(table_stru_tmp.AddInfoObj,"REDIR");
    }else
        redir=this->GetAddInfo(table_stru.AddInfoObj,"REDIR");
    
    //redir
    if (redir.size()!=0){
        tableindex="";
        if (this->GetPathAllIndex(redir,tableindex)) {
            this->SHOW_ERROR((string)"//*/*/*语法错误:" + redir);  //这个差错不适合于递归中的嵌套
            return NULL;
        }
        if ((end_char & 0xc0)!=0)
            tableindex+=end_char; //如果是@*则反补。
        
        //取得新定位
        if (!this->GetTableObject(tableindex,&table_obj,table_obj_size)){
            this->SHOW_ERROR((string)"BuildBufferFromDB(), Redir Load table error!");
            return NULL;
        }
        //重置重定向表格
        return table_obj;
    }else
        return table_stru.ThisObj;
}

//ly table_dir
int AvartarDB_Action::DeleteFromDB_OneNode(BB_OBJ_STRU &table_stru,string node_id){
    //delete this->table rec;
    int rec=0;
    if ((rec = BB_DELETE_ARRAY_REC_BY_INDEX_STRU(&table_stru, (BYTE *)(node_id.data()), node_id.size())) <0) {
        if (rec==-2 )
            this->SHOW_ERROR((string)"Can't Delete Rec[" + node_id + "]" + " id not found!");
        else
            this->SHOW_ERROR((string)"Can't Delete Rec[" + node_id + "]" + " error(" + STRING_Helper::iToStr(rec) + ")!");
        return ACTION_ERROR_DELETE_ID_NOT_FOUND;
    }
    
    //delete this->table sort
    this->SortObj->DeleteItem(table_stru.ThisObj, rec); //ly sort
    
    //如果有.ext文件则删除
    string ext_file=this->GetExtPathName(table_stru.AddInfoObj, node_id, "");
    if (ext_file.size()!=0)
        LW_del(ext_file.c_str());
    
    //delete table_dir; //ly table_dir
    string index_table_dir_model=this->GetAddInfo(table_stru.AddInfoObj, "INDEX_TABLE_DIR_MODEL");
    string index_table_dir=this->GetAddInfo(table_stru.AddInfoObj, "INDEX_TABLE_DIR");
    if (index_table_dir.size()!=0 && index_table_dir_model.size()!=0){ //删除当前对象记录
        //取得父对象
        BB_OBJ *parent_obj;
        BB_SIZE_T parent_obj_size;
        if (this->GetAvartarObjectByNamePath(index_table_dir,&parent_obj,&parent_obj_size)<0){
            this->SHOW_ERROR((string)"Can't Delete Rec error parent obj("+index_table_dir+") is not exited");
            return ACTION_ERROR_DELETE_PARENT_OBJ_NOT_FOUND;
        }
        
        //删除孙对象
        BB_OBJ *son_obj;
        BB_SIZE_T son_obj_size;
        if (BB_GET_ARRAY_REC_BY_NUM(parent_obj,(BB_SIZE_T)rec,&son_obj,&son_obj_size)){
            BB_OBJ_STRU son_stru=BB_GET_OBJ_STRU(son_obj);
            //删除子对象的下级（孙对象）的MALLOC和目录内容。
            if (son_stru.Type==BB_ARRAY && BB_GET_OBJ_TYPE(son_stru.CellObj)==BB_MALLOC){//循环子节点删除孙节点
                BYTE *item;
                BB_SIZE_T item_size;
                for (BB_SIZE_T i = 0; i<son_stru.Num; i++) {
                    BB_GET_ARRAY_REC_BY_NUM_STRU(&(son_stru), i, &item, &item_size);
                    BB_OBJ_STRU item_stru=BB_GET_OBJ_STRU(item);
                    //del file
                    string file_name=this->GetAddInfo(item_stru.AddInfoObj,"SAVE_PATH");
                    if (file_name.size()!=0 && file_name.find(".adb")>0){
                        LW_del(file_name.c_str());
                        //???加入删除子目录
                    }
                    //ly table_dir
                    if (!BB_DELETE_ARRAY_REC_STRU(&(son_stru), i))
                    {
                        this->SHOW_ERROR((string)"Can't Delete Rec[" + STRING_Helper::uToStr(i) + "]");
                        return ACTION_ERROR_DELETE_IDS_NOT_FOUND;
                    }
                    //del sort
                    this->SortObj->DeleteItem(son_obj, i); //ly sort del
                }
            }
            //删除管理目录
            string dir_name=this->GetAddInfo(son_stru.AddInfoObj,"SAVE_PATH");
            if (dir_name.size()!=0 && dir_name.find("N_")!=string::npos)
                LW_del(dir_name.c_str());
            //删除当前子对象
            BB_DELETE_ARRAY_REC(parent_obj,(BB_SIZE_T)rec);
        }
        //释放子对象内存
//        if (parent_stru.Type==BB_MALLOC){
//            if (son_stru.DataLen>0)
//                free(son_stru.Data);
//        };
        
    }
    return 0;
}

BYTE AvartarDB_Action::GetPathAllIndex(string &temp1, string &tableindex) {//ly task
    vector <string> tmp;
    STRING_Helper::stringSplit(temp1, tmp, "/");
    string tmp1;
    for (unsigned int i = 0; i < tmp.size(); i++) {
        if (tmp[i].size() == 0)
            continue;
        if (tmp[i] == "@CELL_INFO") {
            //tmp1 = BB_SEARCH_CELL_STX;
            tableindex += BB_SEARCH_CELL_STX;
        }else if (tmp[i] == "@ADD_INFO") {
            //tmp1 = BB_SEARCH_CELL_STX;
            tableindex += (char)BB_SEARCH_ADDINFO_STX;
        }else if (tmp[i] == "@INDEX_INFO") {
            //tmp1 = BB_SEARCH_CELL_STX;
            tableindex += (char)BB_SEARCH_INDEX_STX; //ly table_dir
        }else {
            if    (!STRING_Helper::CheckNum(tmp[i])) {
                this->SHOW_ERROR((string)"DoActionParas() Error Read input paras[0] is not '0xNumber')!");
                return ACTION_ERROR_PACKET_PARAS0_TABLE_PATH_ERROR;
            }
            tmp1 = STRING_Helper::HBCStrToHBCStr(tmp[i], "hex", "char", false);
            if (!this->AddSearchCell(tableindex, 0, tmp1)) {
                this->SHOW_ERROR((string)"DoActionParas() Error Read input paras[0] index path error!");
                return ACTION_ERROR_PACKET_PARAS0_TABLE_PATH_ERROR;
            }
        }
    }
    if (tableindex.size() == 0)
        tableindex = STRING_Helper::HBCStrToHBCStr("0x00", "hex", "char", false);//对于根目录的处理
    return 0;
}

bool AvartarDB_Action::GetTableObjectByPath(string &path_str, BB_OBJ **obj, BB_SIZE_T &obj_size) {//ly table_dir; 应转为AvartarDB_Base
    string tableindex("");
    
    if (this->GetPathAllIndex(path_str,tableindex)) {
        this->SHOW_ERROR((string)"//*/*/*语法错误:" + path_str);  //这个差错不适合于递归中的嵌套
        return false;
    }
    
    return this->GetTableObject(tableindex, obj, obj_size);
}

BYTE AvartarDB_Action::GetAvartarObjectByNamePath(string &path_name_str, BB_OBJ **item_obj, BB_SIZE_T *item_obj_size) {//ly table_dir; 应转为AvartarDB_Base
    vector <string> tmp;
    STRING_Helper::stringSplit(path_name_str, tmp, "/");
    BB_OBJ *sobj=this->rootObj;
    for (unsigned int i = 1; i < tmp.size(); i++) {
        if (tmp[i].size() == 0)
            continue;
        if (tmp[i] == "@CELL_INFO" ||tmp[i] == "@ADD_INFO" || tmp[i] == "@INDEX_INFO") {
            BB_OBJ_STRU s_stru=BB_GET_OBJ_STRU(sobj);
            (*item_obj)=tmp[i] == "@CELL_INFO" ? s_stru.CellObj : (tmp[i] == "@ADD_INFO" ? s_stru.AddInfoObj :s_stru.IndexObj);
        }else if (!GetArrayObjectByName(sobj,tmp[i],item_obj,item_obj_size)) {
            return ACTION_ERROR_PACKET_PARAS0_TABLE_PATH_ERROR;
        }
        if (i!=(tmp.size()-1))
            sobj=(*item_obj);
    }
    return 0;
}


bool AvartarDB_Action::GetArrayObjectByName(BB_OBJ *obj, string &cmp_name,BB_OBJ **item_obj, BB_SIZE_T *item_obj_size) {//ly table_dir; 应转为AvartarDB_Base
    //取得根对象 //ly table_dir
    BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
    
    BB_OBJ_STRU cell_stru = BB_GET_OBJ_STRU(obj_stru.CellObj);
    if (!(obj_stru.Type==BB_ARRAY && (cell_stru.Type==BB_MALLOC))){ //ly obj
        this->SHOW_ERROR((string)"GetArrayObjectByName('"+cmp_name+"'): is not BB_MALLOC");  //这个差错不适合于递归中的嵌套
        return false;
    }
    
    if (obj_stru.Type==BB_ARRAY && (cell_stru.Type==BB_MALLOC)){ //ly obj
        for(BB_SIZE_T i=0; i<obj_stru.Num; i++){
            if (!BB_GET_ARRAY_REC_BY_NUM_STRU(&obj_stru,i,item_obj,item_obj_size)){
                this->SHOW_ERROR((string)"GetArrayObjectByName(): get num("+STRING_Helper::uToStr(i)+") error!");  //这个差错不适合于递归中的嵌套
                return false;
            }
            BB_OBJ_STRU item_stru=BB_GET_OBJ_STRU(*item_obj);
            string src_name=this->GetAddInfo(item_stru.AddInfoObj,"NAME");
            if (src_name==cmp_name)
                return true;
        }
        return false;  //结果超出范围。
    }
    return false;
}

BYTE AvartarDB_Action::DoActionParas_GetBitmapExt(BB_OBJ_STRU &cell_obj_table,BB_OBJ *src_bitmap_obj, string &bitmap_str, BYTE &add_flag) {
    //ly cell_name
    add_flag = add_flag & ACTION_ADD_FLAG_NO_ADD_INDEX;
    //转换如果是"1,2,3,4,5",转换成"0x0102030405"
    BB_OBJ *bitmap_obj = (BB_OBJ *)(src_bitmap_obj);
    if (BB_IS_OBJ_STRING(bitmap_obj)) {
        string temp1 = string((const char*)BB_GET_OBJ_DATA(bitmap_obj), BB_GET_OBJ_DATA_LEN(bitmap_obj));
        //if (temp1.find("@VERICAL:"))
        vector <string> tmp; string temp2("");
        STRING_Helper::stringSplit(temp1, tmp, ",");
        for (unsigned int i = 0; i < tmp.size(); i++) {
            if (tmp[i] == "@INDEX"){
                if (i == 0) {
                    add_flag = add_flag | ACTION_ADD_FLAG_ADD_INDEX;
                    continue;
                }
                else {
                    this->SHOW_ERROR((string)"Array Cell Parse[1] bitmap @INDEX must set as first! ");
                    return ACTION_ERROR_PACKET_PARAS_BITMAP_INDEX_MUST_SET_AS_FIRST_CELL;//to_add
                }
            }
            //字符串转换
            int n;
            if (tmp[i].at(0)=='#'){//ly cell_name
                //cell 字段转换
                const char *p=(char *)(tmp[i].c_str());
                if ((n=BB_GET_TABLE_FIELD_NAME_ID_STRU(&cell_obj_table,(const char *)(p+1)))<0){//!!!此处可以提速：改为一次性取出CellObjectArray;
                    this->SHOW_ERROR((string)"Array Cell Parse[1] bitmap "+tmp[i]+" is not field name! ");
                    return ACTION_ERROR_PACKET_PARAS_BITMAP_IS_NO_JFIELD_STRING;
                }
            }else if ((n = STRING_Helper::ParseInt(tmp[i])) > 0xff || n == STRING_Helper::NaN) {
                //数字转换
                if (i==0 && tmp.size()==1){
                    //普通字符串的情况
                    bitmap_str = string((const char*)BB_GET_OBJ_DATA(bitmap_obj), BB_GET_OBJ_DATA_LEN(bitmap_obj));
                    return ACTION_ERROR_PACKET_PARAS_BITMAP_IS_NO_UINT8_STRING;
                }
                else{
                    this->SHOW_ERROR((string)"Array Cell Parse[1] bitmap is not number or large than 0xff! ");
                    return ACTION_ERROR_PACKET_PARAS_BITMAP_ARRAY_CELL_IS_NOT_UINT8;//to_add
                }
            }
            //超出判断 ly cell_name; ly task
            if (((add_flag & BB_TABLE_REC_WR_BITIDS) == BB_TABLE_REC_WR_BITIDS) &&  (n >(cell_obj_table.Num - 1))){//测试bitmap_str[n]是否超出field范围
                this->SHOW_ERROR((string)"DoActionParas() Error Read input bitmap[" + STRING_Helper::iToStr(i) + "](conditions) is out of range cell_table_stru.Num(" + STRING_Helper::iToStr(cell_obj_table.Num) + ")!");
                return ACTION_ERROR_READ_PARAS_BITMAP_CELL_IS_OUT_OF_CELL_TABLA_NUM;
            }
            BYTE b = n;
            bitmap_str += b;
        }
    }
    else {
        //普通bitmap_obj处理
        if (BB_GET_OBJ_TYPE(bitmap_obj) != BB_BYTES && BB_GET_OBJ_SIZE(bitmap_obj) != 4) {//wformate test
            this->SHOW_ERROR((string)"DoActionParas can't Write Rec error-input paras[1](wformate is note BYTES[4])!");
            return ACTION_ERROR_PACKET_PARAS_WFORMATE_IS_NOT_BYTES_4;
        }
        bitmap_str = string((const char*)BB_GET_OBJ_DATA(bitmap_obj),
            BB_GET_OBJ_DATA_LEN(bitmap_obj));
    }
    return 0;
}


BYTE AvartarDB_Action::DoActionParas_GetBitmap(BB_OBJ *src_bitmap_obj, string &bitmap_str, BYTE &add_flag) {
    add_flag = add_flag & ACTION_ADD_FLAG_NO_ADD_INDEX;
    //转换如果是"1,2,3,4,5",转换成"0x0102030405"
    BB_OBJ *bitmap_obj = (BB_OBJ *)(src_bitmap_obj);
    if (BB_IS_OBJ_STRING(bitmap_obj)) {
        string temp1 = string((const char*)BB_GET_OBJ_DATA(bitmap_obj), BB_GET_OBJ_DATA_LEN(bitmap_obj));
        //if (temp1.find("@VERICAL:"))
        vector <string> tmp; string temp2("");
        int n;
        STRING_Helper::stringSplit(temp1, tmp, ",");
        for (unsigned int i = 0; i < tmp.size(); i++) {
            if (tmp[i] == "@INDEX"){
                if (i == 0) {
                    add_flag = add_flag | ACTION_ADD_FLAG_ADD_INDEX;
                    continue;
                }
                else {
                    this->SHOW_ERROR((string)"Array Cell Parse[1] bitmap @INDEX must set as first! ");
                    return ACTION_ERROR_PACKET_PARAS_BITMAP_INDEX_MUST_SET_AS_FIRST_CELL;//to_add
                }
            }
            if ((n = STRING_Helper::ParseInt(tmp[i])) > 0xff || n == STRING_Helper::NaN) {
                if (i==0 && tmp.size()==1){
                    //普通字符串的情况
                    bitmap_str = string((const char*)BB_GET_OBJ_DATA(bitmap_obj), BB_GET_OBJ_DATA_LEN(bitmap_obj));
                    return ACTION_ERROR_PACKET_PARAS_BITMAP_IS_NO_UINT8_STRING;
                }
                else{
                    this->SHOW_ERROR((string)"Array Cell Parse[1] bitmap is not number or large than 0xff! ");
                    return ACTION_ERROR_PACKET_PARAS_BITMAP_ARRAY_CELL_IS_NOT_UINT8;//to_add
                }
            }
            BYTE b = n;
            bitmap_str += b;
        }
    }
    else {
        //普通bitmap_obj处理
        if (BB_GET_OBJ_TYPE(bitmap_obj) != BB_BYTES && BB_GET_OBJ_SIZE(bitmap_obj) != 4) {//wformate test
            this->SHOW_ERROR((string)"DoActionParas can't Write Rec error-input paras[1](wformate is note BYTES[4])!");
            return ACTION_ERROR_PACKET_PARAS_WFORMATE_IS_NOT_BYTES_4;
        }
        bitmap_str = string((const char*)BB_GET_OBJ_DATA(bitmap_obj), BB_GET_OBJ_DATA_LEN(bitmap_obj));
    }
    return 0;
}

BYTE AvartarDB_Action::DoActionParas_GetCursor(BB_OBJ *cursor_obj, string &cursor_str) {
    if (BB_IS_OBJ_STRING(cursor_obj)) {
        //for "1,n"
        string cur_str = string((const char*)BB_GET_OBJ_DATA(cursor_obj), BB_GET_OBJ_DATA_LEN(cursor_obj));
        vector <string> tmp;
        STRING_Helper::stringSplit(cur_str, tmp, ",");
        if (tmp.size() != 2 || !STRING_Helper::CheckNum(tmp[0]) || !STRING_Helper::CheckNum(tmp[1])) {
            this->SHOW_ERROR((string)"DoActionParas() Error Read input paras[" + cur_str + "] is error!");
            return ACTION_ERROR_READ_PARAS_CURSOR_IS_INCORRECT1;
        }
        BB_UINT_STRU obj1 = BB_NEW_CELL_UINT(STRING_Helper::ParseInt(tmp[0]), BB_UINT32);
        BB_UINT_STRU obj2 = BB_NEW_CELL_UINT(STRING_Helper::ParseInt(tmp[1]), BB_UINT16);
        cursor_str = string((const char *)BB_GET_OBJ_DATA((BB_OBJ *)&obj1), BB_GET_OBJ_DATA_LEN((BB_OBJ *)&obj1)) + string((const char *)BB_GET_OBJ_DATA((BB_OBJ *)&obj2), BB_GET_OBJ_DATA_LEN((BB_OBJ *)&obj2));
    }
    else
        cursor_str.append((const char*)BB_GET_OBJ_DATA(cursor_obj), BB_GET_OBJ_DATA_LEN(cursor_obj));
    return 0;
}

BYTE AvartarDB_Action::DoActionParas_SonAddInfo(string &buf_str, string &tableindex, string &bitmap_str, string &conditions, string cursor, AVARTDB_AUTH_MASK_TYPE access_mask,BYTE method, bool add_nodeid,bool group_result) {
    //取得源表格对象地址
    BB_OBJ *table_obj;
    BB_SIZE_T table_obj_size;
    BYTE rt = 0;
    string clear_bitmap = STRING_Helper::putUINTToStr(0, 32);
    UInt64 rec_num = 0;
    
    if (!this->GetTableObject(tableindex, &table_obj, table_obj_size)) {
        this->SHOW_ERROR((string)"DoActionParas_SonAddInfo(),Load table error!");
        return ACTION_ERROR_READ_BUILDER_CAN_NOT_GET_TABLE;
    }
    BB_OBJ_STRU table_obj_stru = BB_GET_OBJ_STRU(table_obj);
    
    if (table_obj_stru.Type != BB_ARRAY) {
        this->SHOW_ERROR((string)"DoActionParas_SonAddInfo(),Load table is not BB_ARRAY!");
        return ACTION_ERROR_GET_CLUSTER_INFO_OBJECT_IS_NOT_ARRAY;
    }
    
    //取得起止页码
    UInt32 page_start=0, page_count=0;
    if (cursor.size() != 0) {
        if (cursor.size() == QUERY_CURSOR_STR_LEN) {
            string tmp = cursor.substr(0, 4);
            page_start = STRING_Helper::scanUINTStr(tmp);
            tmp = cursor.substr(4, 2);
            page_count = STRING_Helper::scanUINTStr(tmp);
        }
        else {
            this->SHOW_ERROR((string)"BuildBufferFromDB(),cursor must be 6 bytes len!");
            rt = ACTION_ERROR_READ_BUILDER_CURSOR_MUST_6_BYTE_LEN;
            return rt;
        }
    }
    
    for (BB_SIZE_T num = 0; num<table_obj_stru.Num; num++) {//循环所有表内记录
        //察看page_star,page_count
        rec_num++;	//符合条件的记录计数。
        if (cursor.size() != 0) {
            if (rec_num < page_start)
                continue;
            if (rec_num >= (page_start + page_count))
                break;
        }
        
        //通过索引，取得指定记录信息
        string buf_tmp;
        if ((rt = this->DoActionParas_ACT_INFO(table_obj_stru,bitmap_str, access_mask,buf_tmp, num)) != 0){
            if (rt == ACTION_ERROR_GET_CLUSTER_INFO_OBJECT_NO_AUTH)
                continue; //权限测试未通过
            else
                return rt;
        }
        
        if (add_nodeid) {
            BYTE index_type = BB_GET_OBJ_TYPE(table_obj_stru.IndexObj);
            if ((index_type == BB_ARRAY || index_type == BB_TABLE)) {
                //string header;
                string index_str;
                if ((rt = this->BuildBufferFromDB_AddNodeID(index_str, table_obj_stru, num, method)) != 0)
                    return rt;
                buf_tmp.insert(0,index_str);
            }
        }
        
        if (group_result) {
            //如果需要添加group
            BB_OBJ cell = BB_NULL;;
            BB_OBJ *group_obj = BB_NEW_ARRAY(&cell, NULL, NULL, 0, buf_tmp.data(), buf_tmp.size(), NULL);
            buf_str.append(string((const char*)group_obj, BB_GET_OBJ_SIZE(group_obj)));
            if (group_obj)
                free(group_obj);
        }
        else
            buf_str.append(buf_tmp);
    }
    return 0;
}

BYTE AvartarDB_Action::DoActionParas_ACT_INFO(BB_OBJ_STRU &table_stru,string &bitmap_str, AVARTDB_AUTH_MASK_TYPE access_mask, string &func_return, int num) {
    BB_OBJ *ret_obj, *table_son_obj;
    BB_SIZE_T table_son_obj_size;
    BB_BITMAP_TYPE bitmap = STRING_Helper::scanUINTIDStr(bitmap_str,32);
    
    //取得子表格对象
    BB_OBJ_STRU table_son_obj_stru;
    BB_OBJ_STRU *p_table_son_obj_stru;
    if (num == -1) {
        p_table_son_obj_stru = &table_stru;
    }else{
        if (!BB_GET_ARRAY_REC_BY_NUM_STRU(&table_stru, num, &table_son_obj, &table_son_obj_size)) {
            this->SHOW_ERROR((string)"DoActionParas_ACT_INFO(),Load son table error!");
            return ACTION_ERROR_GET_CLUSTER_INFO_OBJECT_NOT_EXIT;
        }
        table_son_obj_stru = BB_GET_OBJ_STRU(table_son_obj);
        p_table_son_obj_stru = &table_son_obj_stru;
    }
    
    //读取操作
    //测试access_mask是否符合条件
    AVARTDB_AUTH_MASK_TYPE	addinfo_access_mask;
    if (access_mask != AVARTDB_AUTH_MASK_FULL) {
        addinfo_access_mask = this->GetAddInfo(p_table_son_obj_stru->AddInfoObj, AVARTDB_ADDINFO_ACCESS_MASK);
        if ((addinfo_access_mask.size() != 0) && !this->TestMask(addinfo_access_mask, access_mask))	// {if ((rec_access_mask & access_mask) == 0)
            return ACTION_ERROR_READ_BUILDER_NODEID_NO_AUTH;
    }
    
    if (this->TestFFFF(bitmap_str)) {
        func_return.append((const char *)p_table_son_obj_stru->AddInfoObj, BB_GET_OBJ_SIZE(p_table_son_obj_stru->AddInfoObj));
        return 0;
    }
    
    //测试权限操作
    
    if ((bitmap & ACTION_GET_CLUSTER_INFO_NAME) == ACTION_GET_CLUSTER_INFO_NAME) {
        //取得对象AddInfo结构
        string name;
        if (p_table_son_obj_stru->AddInfoObj != NULL ) {
            name = GetAddInfo(p_table_son_obj_stru->AddInfoObj, AVARTDB_ADDINFO_OBJ_NAME);
            if (name.size()==0)
                name = "!!AddInfo_NoName";
        }
        else
            name = "!!NoAddInfo";
        
        BB_OBJ *name_obj=BB_NEW_OBJ_STRING((const void *)name.data(), name.size(), NULL);//注：不需要创建内存,??超出size的处理,BB_NEW_CELL已能够截取
        func_return.append((const char *)name_obj, BB_GET_OBJ_SIZE(name_obj));
        if (name_obj!=NULL)
            free(name_obj);
    }
    if ((bitmap & ACTION_GET_CLUSTER_INFO_TYPE) == ACTION_GET_CLUSTER_INFO_TYPE) {
        //取得对象AddInfo结构
        BB_UINT_STRU tmp_uint_stru;
        tmp_uint_stru = BB_NEW_CELL_UINT(p_table_son_obj_stru->Type, BB_UINT8);
        ret_obj = (BB_OBJ *)&tmp_uint_stru;
        func_return.append((const char *)ret_obj, BB_GET_OBJ_SIZE(ret_obj));
    }
    if ((bitmap & ACTION_GET_CLUSTER_INFO_COUNT) == ACTION_GET_CLUSTER_INFO_COUNT) {//ly: tree																							  //取得对象记录数（）
        BB_UINT_STRU tmp_uint_stru;
        tmp_uint_stru = BB_NEW_CELL_UINT(p_table_son_obj_stru->Num, BB_UINT64);
        ret_obj = (BB_OBJ *)&tmp_uint_stru;
        func_return.append((const char *)ret_obj, BB_GET_OBJ_SIZE(ret_obj));
    }
    if ((bitmap & ACTION_GET_CLUSTER_MODEL_TYPE) == ACTION_GET_CLUSTER_MODEL_TYPE) {
        //取得对象AddInfo结构
        string name;
        if (p_table_son_obj_stru->AddInfoObj != NULL) {
            name = GetAddInfo(p_table_son_obj_stru->AddInfoObj, AVARTDB_ADDINFO_MODEL_TYPE);
            if (name.size() == 0)
                name = "NULL";
        }
        else
            name = "!!NoAddInfo";
        
        BB_OBJ *name_obj = BB_NEW_OBJ_STRING((const void *)name.data(), name.size(), NULL);//注：不需要创建内存,??超出size的处理,BB_NEW_CELL已能够截取
        func_return.append((const char *)name_obj, BB_GET_OBJ_SIZE(name_obj));
        if (name_obj != NULL)
            free(name_obj);
    }
    if ((bitmap & ACTION_GET_CLUSTER_SAVE_PATH) == ACTION_GET_CLUSTER_SAVE_PATH) {
        //取得对象AddInfo结构
            string name;
        if (p_table_son_obj_stru->AddInfoObj != NULL) {
            name = GetAddInfo(p_table_son_obj_stru->AddInfoObj, AVARTDB_SAVE_FILE_NAME);
            if (name.size() == 0)
                name = "NULL";
        }
        else
            name = "!!NoAddInfo";
        
        BB_OBJ *name_obj = BB_NEW_OBJ_STRING((const void *)name.data(), name.size(), NULL);//注：不需要创建内存,??超出size的处理,BB_NEW_CELL已能够截取
        func_return.append((const char *)name_obj, BB_GET_OBJ_SIZE(name_obj));
        if (name_obj != NULL)
            free(name_obj);
    }
    if ((bitmap & ACTION_GET_CLUSTER_INFO_ADDINFO) == ACTION_GET_CLUSTER_INFO_ADDINFO) {//ly: tree																							  //取得对象记录数（）
        func_return.append((const char *)p_table_son_obj_stru->AddInfoObj, BB_GET_OBJ_SIZE(p_table_son_obj_stru->AddInfoObj));
    }
    if ((bitmap & ACTION_GET_CLUSTER_INFO_INDEX_TYPE) == ACTION_GET_CLUSTER_INFO_INDEX_TYPE) {//ly table_dir
        //取得对象AddInfo结构
        string index_type_str;
        if (p_table_son_obj_stru->AddInfoObj != NULL) {
            switch(BB_GET_OBJ_TYPE(p_table_son_obj_stru->IndexObj)){
                case BB_TABLE:
                    index_type_str="BB_TABLE";
                    break;
                case BB_ARRAY:
                    index_type_str="BB_ARRAY";
                    break;
                default:
                    index_type_str="BB_UINT";
                    break;
            }
        }
        else
            index_type_str = "!!NoAddInfo";
        
        BB_OBJ *table_dir_obj = BB_NEW_OBJ_STRING((const void *)index_type_str.data(), index_type_str.size(), NULL);//注：不需要创建内存,??超出size的处理,BB_NEW_CELL已能够截取
        func_return.append((const char *)table_dir_obj, BB_GET_OBJ_SIZE(table_dir_obj));
        if (table_dir_obj != NULL)
            free(table_dir_obj);
    }
    
    return 0;
}

BYTE AvartarDB_Action::DoActionParas_ACT_INFO_BY_NAME(BB_OBJ_STRU &table_stru,string &key, AVARTDB_AUTH_MASK_TYPE access_mask, string &func_return, int num) {
    BB_OBJ *table_son_obj;
    BB_SIZE_T table_son_obj_size;
    
    //取得子表格对象
    BB_OBJ_STRU table_son_obj_stru;
    BB_OBJ_STRU *p_table_son_obj_stru;
    if (num == -1) {
        p_table_son_obj_stru = &table_stru;
    }else{
        if (!BB_GET_ARRAY_REC_BY_NUM_STRU(&table_stru, num, &table_son_obj, &table_son_obj_size)) {
            this->SHOW_ERROR((string)"DoActionParas_ACT_INFO(),Load son table error!");
            return ACTION_ERROR_GET_CLUSTER_INFO_OBJECT_NOT_EXIT;
        }
        table_son_obj_stru = BB_GET_OBJ_STRU(table_son_obj);
        p_table_son_obj_stru = &table_son_obj_stru;
    }
    
    //读取操作
    //测试access_mask是否符合条件
    AVARTDB_AUTH_MASK_TYPE	addinfo_access_mask;
    if (access_mask != AVARTDB_AUTH_MASK_FULL) {
        addinfo_access_mask = this->GetAddInfo(p_table_son_obj_stru->AddInfoObj, AVARTDB_ADDINFO_ACCESS_MASK);
        if ((addinfo_access_mask.size() != 0) && !this->TestMask(addinfo_access_mask, access_mask))	// {if ((rec_access_mask & access_mask) == 0)
            return ACTION_ERROR_READ_BUILDER_NODEID_NO_AUTH;
    }

    //取得对象AddInfo结构
    string value_obj_str,value_str;
    if (p_table_son_obj_stru->AddInfoObj != NULL) {
        BB_OBJ *item_obj;
        BB_SIZE_T item_size;
        if (!GetAddInfoObj(p_table_son_obj_stru->AddInfoObj, key.c_str(),&item_obj,&item_size)){
            value_str = "NULL"; //ly table_sync old no else!
        }else if (!BB_IS_OBJ_STRING(item_obj) && (BB_GET_OBJ_TYPE(item_obj) == BB_ARRAY || BB_GET_OBJ_TYPE(item_obj) == BB_TABLE) )
            value_str=string ((const char *)item_obj,item_size);
        else if (BB_IS_UINT(item_obj))
            value_str=STRING_Helper::uToStr(BB_GET_UINT_VALUE(item_obj));
        else
            value_str=string ((const char *)BB_GET_OBJ_DATA(item_obj),BB_GET_OBJ_DATA_LEN(item_obj));
    }
    else
        value_str = "!!NoAddInfo";
    
    BB_OBJ *name_obj = BB_NEW_OBJ_STRING((const void *)value_str.data(), value_str.size(), NULL);//注：不需要创建内存,??超出size的处理,BB_NEW_CELL已能够截取
    func_return.append((const char *)name_obj, BB_GET_OBJ_SIZE(name_obj));
    if (name_obj != NULL)
        free(name_obj);
    
//    if ((bitmap & ACTION_GET_CLUSTER_INFO_ADDINFO) == ACTION_GET_CLUSTER_INFO_ADDINFO) {//ly: tree																							  //取得对象记录数（）
//        func_return.append((const char *)p_table_son_obj_stru->AddInfoObj, BB_GET_OBJ_SIZE(p_table_son_obj_stru->AddInfoObj));
//    }
    
    return 0;
}


BB_OBJ * AvartarDB_Action::GetSysInfoTable() {
    BYTE *table_obj;
    BB_SIZE_T table_obj_size;
    if (this->SysInfoTableStru.ThisObj == NULL) {
        //初始化加载系统库
        string sys_info_table(AVARTDB_SYS_INFO_TABLE0_PATH);
        if (!this->GetTableObject(sys_info_table, &table_obj, table_obj_size)) {
            //???今后TABLE目录应可以配置
            this->SHOW_ERROR((string)"INIT():SYS_INFO_TABLE0(),Load table error!");
            return NULL;
        }
        this->SysInfoTableStru = BB_GET_OBJ_STRU(table_obj);
        BB_OBJ_STRU index_obj_stru = BB_GET_OBJ_STRU(this->SysInfoTableStru.IndexObj);
        if (BB_GET_OBJ_TYPE(index_obj_stru.CellObj) != BB_BYTES) {
            this->SHOW_ERROR((string)"INIT():SYS_INFO_TABLE0(),Index should be BB_BYTES_ARRAY!");
            return NULL;
        }
        BB_SIZE_T i;
        if ((i=BB_GET_ARRAY_NUM(this->SysInfoTableStru.CellObj)) < SYS_INFO_TABLE_MAX_CELL_NUM) {
            this->SHOW_ERROR((string)"INIT():SYS_INFO_TABLE0(),Index should be large than "+STRING_Helper::iToStr(i)+"!");
            return NULL;
        }
        return table_obj;
    }else
        return this->SysInfoTableStru.ThisObj;
}

BB_OBJ_STRU * AvartarDB_Action::GetSysQueueTableStru() {
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

bool AvartarDB_Action::SysInfoTimeTrack(const char *name, BYTE method, UInt64 this_time) {
    BB_OBJ *table_obj = this->GetSysInfoTable();
    if (table_obj == NULL)
        return false;
    
    int rec;
    bool is_init = false;
    //取得name所对应的记录
    
    if ((rec = BB_GET_ARRAY_NUM_BY_INDEX_STRU(&(this->SysInfoTableStru), (BYTE *)name, strlen(name))) < 0) {
        BB_SIZE_T len = strlen(name);
        if (rec != -1) {
            this->SHOW_ERROR((string)"TimeTrack->GET_SYS_INFO_TABLE error0 [" + name + "] can't create!");
            return false;
        }
        //添加索引、记录
        if ((rec = BB_APPEND_ARRAY_REC_BY_INDEX_STRU(&(this->SysInfoTableStru), NULL, 0, (BYTE *)name, len))<0) {
            this->SHOW_ERROR((string)"TimeTrack->GET_SYS_INFO_TABLE error1 [" + name + "] not found!");
            return false;
        }
        //写入name
        //BB_OBJ *buf_obj = BB_NEW_CELL(BB_BYTES, (const void *)name, len, NULL);
        //BB_SIZE_T buf_obj_size = BB_GET_OBJ_SIZE(buf_obj);
        //BB_PUT_TABLE_ITEMS_DATA_STRU(&(this->SysInfoTableStru), rec, SYS_INFO_NAME, (BYTE *)buf_obj, buf_obj_size, BB_TABLE_PUT_OPERATE_REPLACE);
        //if (buf_obj != NULL)
        //free(buf_obj);
        BB_PUT_TABLE_ITEMS_DATA_STRU(&(this->SysInfoTableStru), rec, SYS_INFO_NAME, (BYTE *)name, len, BB_TABLE_PUT_OPERATE_REPLACE);
        
        is_init = true;
    }
    if (method > AvartarDB_Base::SYS_INFO_TABLE_MAX_CELL_NUM) {
        this->SHOW_ERROR((string)"TimeTrack->GET_SYS_INFO_TABLE error [" + STRING_Helper::iToStr(method) + "] is tool large!");
        return false;
    }
    
    BYTE bitmap_base = 0x80;
    switch (method) {
        case SYS_INFO_START_TIME:{
            //起始设置
            BYTE time_buff[8];
            BB_PUT_UINT_TO_BUFF(time_buff, this_time, 8);
            if (is_init) {
                BYTE count_buff[8] = { 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0 };
                BYTE all_time_buff[8] = { 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0 };
                BB_PUT_TABLE_ITEMS_DATA_STRU(&(this->SysInfoTableStru), rec, SYS_INFO_COUNT, (BYTE *)&count_buff, 8, BB_TABLE_PUT_OPERATE_REPLACE);
                BB_PUT_TABLE_ITEMS_DATA_STRU(&(this->SysInfoTableStru), rec, SYS_INFO_ALL_TIME, (BYTE *)&all_time_buff, 8, BB_TABLE_PUT_OPERATE_REPLACE);
            }
            BB_PUT_TABLE_ITEMS_DATA_STRU(&(this->SysInfoTableStru), rec, SYS_INFO_START_TIME, (BYTE *)&time_buff, 8, BB_TABLE_PUT_OPERATE_REPLACE);
        }
            break;
        case SYS_INFO_END_TIME:{
            BYTE bitmap_read,read_uint64[3][8], bitmap_write,write_uint64[3][8];
            //读取操作
            bitmap_read = (bitmap_base >> SYS_INFO_COUNT) | (bitmap_base >> SYS_INFO_START_TIME) | (bitmap_base >> SYS_INFO_ALL_TIME)  ;
            BB_SIZE_T bufoffset=0;
            BB_READ_TABLE_BY_NUM_STRU(&(this->SysInfoTableStru), rec, (BYTE *)read_uint64, &bufoffset, &bitmap_read, 1, BB_TABLE_REC_WR_BITMAP);
            
            //累加或写入操作
            
            BB_ADD_UINT_TO_BUFF(write_uint64[2], read_uint64[2], this_time - BB_ScanUINTWidth(read_uint64[1], 8), 8);
            BB_ADD_UINT_TO_BUFF(write_uint64[0], read_uint64[0], 1, 8);
            BB_PUT_UINT_TO_BUFF(write_uint64[1], this_time, 8); //add SYS_INFO_ALL_TIME
            bitmap_write = (bitmap_base >> SYS_INFO_COUNT) | (bitmap_base >> SYS_INFO_END_TIME) | (bitmap_base >> SYS_INFO_ALL_TIME) ;
            //写入操作
            bufoffset = 0;
            if ((rec = BB_WRITE_TABLE_BY_NUM_STRU(&(this->SysInfoTableStru), (BYTE *)&write_uint64, &bufoffset, rec, &bitmap_write, 1, BB_TABLE_REC_WR_BITMAP)) < 0) {
                this->SHOW_ERROR((string)"TimeTrack->BB_WRITE_TABLE_BY_NUM_STRU,Write table[" + name + "]error!");
                return false;
            }
        }
            break;
        default:
            return false;
            break;
    }
    return true;
}
int AvartarDB_Action::WriteDataToDB_OneField(BYTE *data,BB_SIZE_T data_size,string &tableindex, string &nodeid,string &field_id){
    int rw;
    BB_SIZE_T offset=0;
    BB_OBJ cell=BB_NULL;
    
    BB_OBJ *item_obj= BB_NEW_BYTES_ALL(data,(UInt32)data_size,NULL);
    BB_OBJ *data_obj = BB_NEW_ARRAY(&cell, NULL, NULL, 0, item_obj, BB_GET_OBJ_SIZE(item_obj), NULL);
    rw=this->WriteBufferToDB_OneNode(data_obj, offset, tableindex, nodeid,field_id,(BYTE)0x60);
    if (data_obj!=NULL)
        free(data_obj);
    if (item_obj==NULL)
        free(item_obj);
    if (rw<0)
        return rw;
    else
        return 0;
}

int AvartarDB_Action::WriteBufferToDB_OneNode(byte* buf, BB_SIZE_T &bufoffset,string &tableindex, string &index_str,string bitmap_str, BYTE method){//is_offset:两种传参bitmap,offset_len模式
    
    BYTE *obj;
    BB_SIZE_T obj_size;
    int num;
    UInt64 max_num=0;
    int max_err = -7;
    bool has_addinfo_max_num=false;
    bool is_append=false;
    
    if (!this->GetTableObject(tableindex,&obj,obj_size)){
        this->SHOW_ERROR((string)"WriteBufferToDB_OneNode(),Load table error!");
        return -1;
    }
    BB_OBJ_STRU obj_stru=BB_GET_OBJ_STRU(obj);
    
    //重定向测试 //ly table_filter //ly table_redir
    string fdefault1=this->GetAddInfo(obj_stru.AddInfoObj,"FILTER");
    string fdefault;
    if (this->CheckGetTableFilter(obj_stru,fdefault)<0){
        this->SHOW_ERROR((string)"BuildBufferFromDB(), Redir Load table error!");
        return ACTION_ERROR_READ_BUILDER_CAN_NOT_GET_TABLE;
    };
    
    //table_redir
    int old_rec_num=obj_stru.Num;
    bool is_redir=false;
    BB_OBJ *redir_obj=this->CheckGetTableRedir(tableindex,obj_stru);
    if (redir_obj==NULL){//如果是重定向table，则tableindex\table_stru改为新table
        this->SHOW_ERROR((string)"BuildBufferFromDB(), Redir Load table error!");
        return ACTION_ERROR_READ_BUILDER_CAN_NOT_GET_TABLE;
    }else if (obj!=redir_obj){
        obj_stru=BB_GET_OBJ_STRU(redir_obj);
        obj=redir_obj;
        is_redir=true;
    }
    
    //如果是ffff则创建新的index_str;
    //ly test
    //cout << "i:" << STRING_Helper::HBCStrToHBCStr(index_str, "char", "hex", false) << ";b:" << bufoffset << ";m" << method <<"index_str1:" << STRING_Helper::HBCStrToHBCStr(index_str,"char","hex",true);
    if (index_str.size() == 0) {
        this->SHOW_ERROR((string)"WriteBufferToDB_OneNode(),index string is empty!");
        return -2;
    }
    
    if (BB_GET_OBJ_DIM(obj_stru.IndexObj) == 1) {
        //有IndexArray写入操作
        if (this->TestFFFF(index_str)) {//自动创建NodeID模式，如果是fff则创建新的NodeID;
            is_append=true; //ly table_dir
            if (!(has_addinfo_max_num = this->GetAddInfoUINT(obj_stru.AddInfoObj, AVARTDB_ADDINFO_INDEX_MAX_NUM, max_num)) || max_num<2) {//ly sort
                //小于2则，全面对比
                if (!BB_GET_ARRAY_UINT_MAX(obj_stru.IndexObj, &max_num)) {//取得最大的NodeID+1;从index_str开始替换；test?
                    this->SHOW_ERROR((string)"WriteBufferToDB_OneNode(),can't get new index!");
                    return -3;
                }
            }
            BB_OBJ_STRU index_stru = BB_GET_OBJ_STRU(obj_stru.IndexObj);
            BB_SIZE_T size = BB_GET_OBJ_DATA_LEN(index_stru.CellObj);//取得Index字段宽度
            //cout << "max_num:" << max_num << "index_str" << index_str;
            if (this->TestFFFF(max_num++, size)) {//如果累加计数为ffff*代表已满
                this->SHOW_ERROR((string)"WriteBufferToDB_OneNode(),index is full !");
                return -4;
            }
            //创建新的index_str;，可替换新函数ly task
            BYTE index_str_buf[8];
            BB_PUT_UINT_TO_BUFF(index_str_buf, max_num, size);
            index_str = string((char *)index_str_buf, size);
        }
        //写入操作
        if ((num = BB_WRITE_TABLE_BY_INDEX(obj, buf, &bufoffset, (BYTE *)index_str.data(), index_str.size(), (BYTE *)bitmap_str.data(), bitmap_str.size(), method)) < 0) {
            this->SHOW_ERROR((string)"WriteBufferToDB_OneNode(),Write table error!");
            return (max_err + num);
        }
    }else if (BB_IS_UINT(obj_stru.IndexObj)){
        //IndexUint写入操作
        if (this->TestFFFF(index_str)) {
            is_append=true; //ly table_dir
            has_addinfo_max_num = this->GetAddInfoUINT(obj_stru.AddInfoObj, AVARTDB_ADDINFO_INDEX_MAX_NUM, max_num); //ly sort
            //自动创建NodeID模式，如果是fff则创建新的NodeID;
            BB_SIZE_T size = BB_GET_OBJ_DATA_LEN(obj_stru.IndexObj);//取得Index字段宽度
            if (this->TestFFFF(obj_stru.Num, size)) {//如果累加计数为ffff*代表已满
                this->SHOW_ERROR((string)"WriteBufferToDB_OneNode(),index is full!");
                return -4;
            }
            //创建新的index_str;
            if ((num = BB_APPEND_ARRAY_REC_STRU(&obj_stru, NULL ,0)) < 0) {
                this->SHOW_ERROR((string)"WriteBufferToDB_OneNode(),Write table error!");
                return (max_err + num);
            }
            BYTE index_str_buf[8];
            BB_PUT_UINT_TO_BUFF(index_str_buf, num, size);
            max_num = num;
            index_str = string((char *)index_str_buf, size);
        }
        else {
            //旧记录写入模式,不支持大于当前节点
            num = (int)STRING_Helper::scanUINTStr(index_str);
            if (num >= (int)obj_stru.Num) {
                this->SHOW_ERROR((string)"WriteBufferToDB_OneNode(),index("+STRING_Helper::iToStr(num)+")is out of table size(" + STRING_Helper::iToStr(num) + ")!");
                return -3;
            }
        }
        //写入操作
        int r;
        if ((r = BB_WRITE_TABLE_BY_NUM(obj, buf, &bufoffset, num, (BYTE *)bitmap_str.data(), bitmap_str.size(), method)) < 0) {
            this->SHOW_ERROR((string)"WriteBufferToDB_OneNode(),Write table error!");
            return (max_err + r);
        }
    }else{
        this->SHOW_ERROR((string)"WriteBufferToDB_OneNode(),can't index config should be UINT|ARRAY|TABLE!");
        return -3;
    }
    obj_stru=BB_GET_OBJ_STRU(obj); //ly table_sync3; //ly rec_chg_mark
    
    //obj_stru=BB_GET_OBJ_STRU(obj); //!!!obj 添加记录后未同步到obj_stru,后续操作如果用到obj_stru.Num 故需要重新加载。
    //补充设置上传字段(可以把整个bitmap拷入即可) //ly ???这里有问题 //ly table_sync
    UInt64 col;
    //if ((BB_TABLE_REC_WR_QUERY_UPLOADFLAG & method)==BB_TABLE_REC_WR_QUERY_UPLOADFLAG && this->GetAddInfoUINT(obj_stru.AddInfoObj,AVARTDB_ADDINFO_UPLOADFLAG_FIELD,col)) {
    if (!((BB_TABLE_REC_WR_QUERY_UN_UPLOADFLAG & method)==BB_TABLE_REC_WR_QUERY_UN_UPLOADFLAG) && (this->GetAddInfoUINT(obj_stru.AddInfoObj,AVARTDB_ADDINFO_UPLOADFLAG_FIELD,col))) {//ly table_sync3; //ly rec_chg_mark 需要测试添加 AND?OR
        //设置记录的bitmp field
        BYTE method_tmp = is_append ? BB_TABLE_PUT_OPERATE_AND : BB_TABLE_PUT_OPERATE_OR;
        string bitmap_uint_str= ((method & BB_TABLE_REC_WR_BITIDS) == BB_TABLE_REC_WR_BITIDS) ? this->ConvertBitmap_ids2Bitmap(bitmap_str) : bitmap_str;
        if (!BB_PUT_TABLE_ITEMS_DATA_STRU(&obj_stru,num,col,(BYTE *)bitmap_uint_str.data(),bitmap_uint_str.size(),method_tmp)){
            this->SHOW_ERROR((string)"WriteBufferToDB_OneNode()->GetAddInfoUINT->AVARTDB_ADDINFO_UPLOADFLAG_FIELD2,can't set upload flag field! num="+STRING_Helper::iToStr(num)+"; col="+STRING_Helper::iToStr(col));
            return -5;
        }
    }
    //ly rec_chg_mark
    bool has_rec_chg_mark=false;
    BIT_SEARCH_STRU chg_mark_obj;
    if (!((BB_TABLE_REC_WR_QUERY_UN_UPLOADFLAG & method)==BB_TABLE_REC_WR_QUERY_UN_UPLOADFLAG) &&
        (has_rec_chg_mark=this->GetRecChgMarkObj(obj_stru.AddInfoObj,"REC_CHG_MARK",&chg_mark_obj,obj_stru.Num)) ){//设计AddInfo->rec_chg_mark
        BB_SIZE_T data_pos=num/chg_mark_obj.byte_step;
        BYTE data_bit=num % chg_mark_obj.byte_step;
        BYTE bit_pos=data_bit/chg_mark_obj.bit_step;
        chg_mark_obj.data[data_pos] |= (0x80 >> bit_pos);
        chg_mark_obj.obj[0]|=0x80;
    }
    
    //补充设置NodeID索引字串
    if (this->GetAddInfoUINT(obj_stru.AddInfoObj,AVARTDB_ADDINFO_INDEX_FIELD,col) && !BB_PUT_TABLE_ITEMS_DATA(obj,num,col,(BYTE *)index_str.data(),index_str.size(),BB_TABLE_PUT_OPERATE_REPLACE)){
        this->SHOW_ERROR((string)"WriteBufferToDB_OneNode()->GetAddInfoUINT->AVARTDB_ADDINFO_INDEX_FIELD(bitmap),can't set default index field! num="+STRING_Helper::iToStr(num)+"; col="+STRING_Helper::iToStr(col));
        return -6;
    }
    
    //如果是重定向模式，补充写入字段。
    if (is_redir && fdefault.size()!=0){//ly table_redir //ly table_filter 只针对于单一字段条件的情况。
        string col_str(""), df_value(""),equal_str;
        BB_OBJ_STRU fd_stru=BB_GET_OBJ_STRU((BB_OBJ *)fdefault.data());
        if (fd_stru.LoadError || !this->GetArrayRecString(fd_stru, 0, col_str)|| !this->GetArrayRecString(fd_stru, 1, equal_str) || !this->GetArrayRecString(fd_stru, 2, df_value)){
            this->SHOW_ERROR((string)"WriteBufferToDB_OneNode()->GetAddInfoUINT->fdefault(['id','=','value'] ) defined error !");
            return -6; //??return 需要和sord对应
        }
        if (col_str.size()==1 && equal_str=="=" && !BB_PUT_TABLE_ITEMS_DATA(obj,num,STRING_Helper::scanUINTStr(col_str),(BYTE *)(df_value.data()),df_value.size(),BB_TABLE_PUT_OPERATE_REPLACE)){
            this->SHOW_ERROR((string)"WriteBufferToDB_OneNode()->GetAddInfoUINT->AVARTDB_ADDINFO_INDEX_FIELD(bitmap),can't set default index field! num="+STRING_Helper::iToStr(num)+"; col="+col_str);
            return -6; //??return 需要和sord对应
        }
    }
    
    //补充排序字段;ly sort
    string sort_field_str;
    if ((sort_field_str = this->GetAddInfo(obj_stru.AddInfoObj, AVARTDB_SORT_FIELDS_NAME)).size() != 0){//ly sort ??return 需要和sord对应
        BYTE *field_value;
        BB_SIZE_T field_value_size;
        for (size_t i = 0; i < sort_field_str.size(); i++) {
            if (!BB_GET_TABLE_ITEMS_DATA_STRU(&obj_stru, num, sort_field_str.at(i), &field_value, &field_value_size)) {
                this->SHOW_ERROR((string)"WriteBufferToDB_OneNode()->BB_GET_TABLE_ITEMS_DATA_STRU->AVARTDB_SORT_FIELDS_NAME,can't set default index field! field_id=" + STRING_Helper::iToStr(sort_field_str.at(i)));
                return -6;
            }
            string test((const char*)field_value, field_value_size);
            this->SortObj->InsertItem(obj_stru.ThisObj, sort_field_str.at(i), field_value, field_value_size,num);
        }
    }
    
    //AddInfo补充设置AddInfoMaxNum
    if (has_addinfo_max_num && !this->SetAddInfoUINT(obj_stru.AddInfoObj,AVARTDB_ADDINFO_INDEX_MAX_NUM,max_num)){
        this->SHOW_ERROR((string)"WriteBufferToDB_OneNode()->SetAddInfoUINT->AVARTDB_ADDINFO_INDEX_MAX_NUM can't set addinfo max_num! num="+STRING_Helper::iToStr(num)+"; col="+STRING_Helper::uToStr(col));
        return -7;
    }
    
    //AddInfo补充设置AddInfoChgTime //ly table_dir; ly table_sync
    UInt64 last_chg_time=0;
    bool has_chg_time = this->GetAddInfoUINT(obj_stru.AddInfoObj, AVARTDB_ADDINFO_LAST_CHG_TIME, last_chg_time);
    if (has_chg_time && !this->SetAddInfoUINT(obj_stru.AddInfoObj,AVARTDB_ADDINFO_LAST_CHG_TIME,time(NULL))){
        this->SHOW_ERROR((string)"WriteBufferToDB_OneNode()->SetAddInfoUINT->AVARTDB_ADDINFO_LAST_CHG_TIME can't set addinfo max_num! num="+STRING_Helper::iToStr(num)+"; col="+STRING_Helper::uToStr(col));
        return -7;
    }
    
    //注index_str为回传字串。
    if (index_str.size() == 0) {
        this->SHOW_ERROR((string)"Return Index is NULL:num=" + STRING_Helper::iToStr(num) + "; col=" + STRING_Helper::iToStr(col));
        return -8;
    }
    
    //ly table_dir write_one
//    obj_stru=BB_GET_OBJ_STRU(obj);
    //if (is_append ){//如果是新节点
    if (obj_stru.Num>old_rec_num){//如果是新节点
        string index_table_dir_model=this->GetAddInfo(obj_stru.AddInfoObj, "INDEX_TABLE_DIR_MODEL");
        string index_table_dir=this->GetAddInfo(obj_stru.AddInfoObj, "INDEX_TABLE_DIR");
        if (index_table_dir.size()!=0 && index_table_dir_model.size()!=0){
            //创建新对象
            obj_stru=BB_GET_OBJ_STRU(obj); //!!!obj 添加记录后未同步到obj_stru 故需要重新加载。
            BB_OBJ *tmp_obj=this->AvartarNewObj_GetIndexTableObjs_AppendNode(obj_stru,num,index_table_dir_model,index_table_dir);
            if (tmp_obj!=NULL){
                //取得父对象
                BB_OBJ *parent_obj; BB_SIZE_T parent_obj_size;
                if (this->GetAvartarObjectByNamePath(index_table_dir,&parent_obj,&parent_obj_size)<0){
                    this->SHOW_ERROR((string)"WriteBufferToDB_OneNode()->Index_Table_dir->GetAvartarObjectByNamePath("+index_table_dir+") error!");
                    return -9;
                }
                BB_OBJ_STRU parent_stru=BB_GET_OBJ_STRU(parent_obj);
                //添加父对象，子节点
                BB_APPEND_ARRAY_REC_STRU(&parent_stru,(BYTE *)&tmp_obj,sizeof(tmp_obj));
                //调试使用：this->_ActParseXml->SaveFile("UTF-8"); //待扩充。
            }
        }
    }
    
    //写入后回调处理 //ly rec_chg_mark //ly call_func
    string eval_after_write;
    if ((eval_after_write = this->GetAddInfo(obj_stru.AddInfoObj, AVARTDB_EVAL_AFTER_WRITE)).size() != 0){//ly sort ??return 需要和sord对应
        PV_AddParas_T add_paras;
        string func_return;
        
        add_paras.insert(ParseProcess::PV_AddParas_T::value_type(string((string)"{@__NODEID}"), STRING_Helper::HBCStrToHBCStr(index_str,"char","hex",false)));
        string r=GB_Gateway->ParseValue(eval_after_write,func_return,add_paras);
        if (r.size()!=0){
            this->SHOW_ERROR((string)"WriteBufferToDB_OneNode()->AVARTDB_EVAL_AFTER_WRITE->ERROR("+r+")!");
            return -10;
        }
    }

    //判断是否存储变长字段,扩展文件        //ly malloc2.1
    //cout <<"index_str"  << index_str <<" :"<< STRING_Helper::iToStr(BB_GET_ARRAY_NUM_STRU(&obj_stru)) << endl;
    string save_ext("");
    if (obj_stru.AddInfoObj != NULL && (save_ext=this->GetAddInfo(obj_stru.AddInfoObj, AVARTDB_SAVE_MALLOC_EXT)).size()!=0){
        this->WriteBufferToDB_OneNode_SaveExtField(obj_stru,num,index_str,save_ext,method);
    }
    
    
    return 0;
}


bool AvartarDB_Action::GetRecChgMarkObj(BB_OBJ *add_info_obj, const char *key, BIT_SEARCH_STRU *bit_search_stru,BB_SIZE_T max_num) {
    int rec;
    BB_OBJ *obj=NULL;
    BB_SIZE_T obj_size=0;
    if ((rec = BB_GET_ARRAY_NUM_BY_INDEX(add_info_obj, (BYTE *)key, strlen(key))) > 0) {
        if (BB_GET_ARRAY_OBJ_BY_NUM(add_info_obj, rec, &obj, &obj_size)){
            BYTE * item=(BYTE *)BB_GET_OBJ_DATA(obj);
            BB_SIZE_T item_size=BB_GET_OBJ_DATA_LEN(obj);
            if (item_size<2)
                return false;
            //ly rec_chg_mark
            bit_search_stru->param=item[0];
            bit_search_stru->is_changed=((bit_search_stru->param & 0x80)==0x80);//8位，是否刷新
            bit_search_stru->has_param_ext=((bit_search_stru->param & 0x40)==0x40); //7位, 是否有扩展字段
            bit_search_stru->param_ext=bit_search_stru->has_param_ext ? item[1] : 0;
            bit_search_stru->data=bit_search_stru->has_param_ext ? item+2 : item+1;
            bit_search_stru->obj=item;
            bit_search_stru->data_len=bit_search_stru->has_param_ext ? item_size-2 : item_size-1;
            bit_search_stru->type=(bit_search_stru->param & 0x30)>>4;//6-5位 查询类型，现在缺省为0,bit_step查询法;
            switch(bit_search_stru->param & 0x0f){
                case 0: bit_search_stru->max_num=256; break;
                case 1: bit_search_stru->max_num=256*4;break;
                case 2: bit_search_stru->max_num=256*16;break;
                case 3: bit_search_stru->max_num=256*64;break;
                case 4: bit_search_stru->max_num=256*256;break;
                case 5: bit_search_stru->max_num=256*256*4;break;
                case 6: bit_search_stru->max_num=256*256*16;break;
                case 7: bit_search_stru->max_num=256*256*64;break;
                case 8: bit_search_stru->max_num=256*256*256;break;
            };//4-1位
            bit_search_stru->bit_step=bit_search_stru->max_num/(8*bit_search_stru->data_len);
            bit_search_stru->byte_step=bit_search_stru->bit_step*8;
            return true;
        }else
            return false;
    }
    else
        return false;
    //
//    string value_str;
//    if (!this->GetAddInfoString(add_info_obj,key,value_str) || value_str.size()<3)
//        return false;
//
//    BYTE * item=(BYTE *)(value_str.data());
//    BB_SIZE_T item_size=value_str.size();
}


void AvartarDB_Action::WriteBufferToDB_OneNode_SaveExtField(BB_OBJ_STRU &obj_stru,BB_SIZE_T num,string &index_str,string &ext_name,BYTE method){
    //补充写入非定长字段，ly malloc2.1
    //string ext_name("");
    //条件进入
    //string ext_name1("");
//    if (obj_stru.AddInfoObj == NULL)
//        return;
//    string ext_name=this->GetAddInfo(obj_stru.AddInfoObj, AVARTDB_SAVE_MALLOC_EXT);
//    if (ext_name.size()==0)
//        return;
    if ((method & BB_TABLE_REC_WR_BITIDS) != BB_TABLE_REC_WR_BITIDS)
        return;
    BB_OBJ_STRU cell_obj_stru=BB_GET_OBJ_STRU(obj_stru.CellObj);
    if (cell_obj_stru.Type != BB_TABLE)
        return;
    
    //查找ext字段
    string ext_data_str="";
    BB_SIZE_T all_col_num=BB_GET_ARRAY_NUM(obj_stru.CellObj);
    for (BB_SIZE_T i = 0; i < all_col_num; i++)
    {//所有的malloc重创建外部文件。
        if(BB_GET_TABLE_FIELD_TYPE_STRU(&cell_obj_stru,i) == BB_TABLE_FIELD_TYPE_MALLOC ) {//ly malloc2;ly task
            //取得malloc数据区域
            BYTE *obj;
            BB_SIZE_T obj_size;
            BB_GET_TABLE_ITEMS_ID_OBJ(obj_stru.ThisObj, num,(BYTE)i,&obj,&obj_size); //malloc存储为对象，读取为对象内容。且可以实现内部重新读取
            
            //创建数据节
            BB_UINT_STRU add_info_obj=BB_NEW_CELL_UINT((UInt64)i,BB_UINT8);
            BB_OBJ cell = BB_NULL;
            BB_OBJ *obj_p = BB_NEW_ARRAY(&cell, NULL, (BB_OBJ *)&add_info_obj, 0, (const void *)obj, obj_size, NULL);
            //写入字串
            ext_data_str.append((const char *)obj_p,BB_GET_OBJ_SIZE(obj_p));
        }
    }
    
    //写入统一文件
    if (ext_data_str.size()!=0){
        //string ext_name1="ext";
        BYTE *ext_data=(BYTE *)(ext_data_str.data());
        BB_SIZE_T start=0,ext_data_size=ext_data_str.size();
        int r;
        if ((r=this->SaveExt(obj_stru, index_str,ext_name,ext_data, start, ext_data_size, FileQueue::OPEN_MODE_TRUNC))!=0) {
            this->SHOW_ERROR((string)"Save Ext Field Error("+STRING_Helper::iToStr(r)+"):" + index_str + "." +ext_name);
            return;
        }
    }
}

bool AvartarDB_Action::WriteBufferToDB_AllNodes(byte* buf, UInt32 len, string &tableindex, string bitmap_str, BYTE method,string conditions){
    BB_SIZE_T bufoffset = 0;
    
    //表格
    for (BB_SIZE_T num=0; bufoffset < len; num++)
    {
        string index_cut_str("");
        //取得nodeid
        bufoffset+=this->CutTableNodeidStr(index_cut_str,buf+bufoffset,tableindex);//截取index_str；并递进;可以和index库中的CellWidth共享。
        
        if (bufoffset < len && (this->WriteBufferToDB_OneNode(buf, bufoffset, tableindex, index_cut_str, bitmap_str, method)<0))
            return false;//可能有nodeid后数据区为空的情况。
        
        //if (((method & BB_TABLE_REC_WR_BITIDS)==BB_TABLE_REC_WR_BITIDS) && ((method & BB_TABLE_REC_WR_RETURN_TABLE)==BB_TABLE_REC_WR_RETURN_TABLE))
        //break; //只是测试使用
        
        if (bufoffset > len)
            return false;
    }
    return true;
};

BYTE AvartarDB_Action::BuildBufferFromDB_OneNode(string &buf_str,string &tableindex_in, string &nodeid_str,string bitmap_str, AVARTDB_AUTH_MASK_TYPE access_mask,BYTE method,bool add_nodeid){
    //取得表格对象地址
    string tableindex=tableindex_in;
    BB_OBJ *table_obj;
    BB_SIZE_T table_obj_size;
    UInt64 upload_flag_col=0xffff;
    bool marked_save=false;
    
    if (!this->GetTableObject(tableindex,&table_obj,table_obj_size)){
        this->SHOW_ERROR((string)"BuildBufferFromDB_OneNode(),Load table error!");
        return ACTION_ERROR_READ_BUILDER_CAN_NOT_GET_TABLE;
    }
    
    ////取得表格对象
    BB_OBJ_STRU table_obj_stru=BB_GET_OBJ_STRU(table_obj);
    
    //重定向测试 //ly table_redir
    BB_OBJ *redir_obj=this->CheckGetTableRedir(tableindex,table_obj_stru);
    if (redir_obj==NULL){//如果是重定向table，则tableindex\table_stru改为新table
        this->SHOW_ERROR((string)"BuildBufferFromDB(), Redir Load table error!");
        return ACTION_ERROR_READ_BUILDER_CAN_NOT_GET_TABLE;
    }else if (table_obj!=redir_obj){
        table_obj_stru=BB_GET_OBJ_STRU(redir_obj);
        table_obj=redir_obj;
    }
    
    //测试access_mask是否符合条件
    int num;
    if (BB_GET_OBJ_DIM(table_obj_stru.IndexObj) == 1) {
        if ((num = BB_GET_ARRAY_NUM_BY_INDEX_STRU(&table_obj_stru, (BYTE *)nodeid_str.data(), nodeid_str.size())) && (num < 0)) {//未查到信息
            return ACTION_ERROR_READ_BUILDER_NODEID_NOT_FOUND;
        }
    }else{
        num = STRING_Helper::scanUINTStr(nodeid_str);
    }
    
    UInt64 col=0;
    if ((access_mask != AVARTDB_AUTH_MASK_FULL) && (this->GetAddInfoUINT(table_obj_stru.AddInfoObj, AVARTDB_ADDINFO_ACCESS_MASK_FIELD, col))) {
        string rec_access_mask=this->GetTableItemString(&table_obj_stru,num,col);
        if ((rec_access_mask.size()!=0) && !this->TestMask(rec_access_mask,access_mask))    // {if ((rec_access_mask & access_mask) == 0)
            return ACTION_ERROR_READ_BUILDER_NODEID_NO_AUTH;
    }
    
    //预开记录空间
    BB_SIZE_T bufoffset=0,cell_width_ext;
    if (BB_CHECK_METHOD(method) == BB_TABLE_REC_WR_BITIDS)
        cell_width_ext=BB_GET_TABLE_ITEMS_IDS_WIDTH_EXT(&table_obj_stru,num,(BYTE *)(bitmap_str.data()),bitmap_str.size())+bitmap_str.size()*8; //+CELLWIDTH_BUF_ADD_MAX; //对于ids的缩减内存
    else
        cell_width_ext=BB_GET_TABLE_CELL_WIDTH_EXT(&table_obj_stru,num)+CELLWIDTH_BUF_ADD_MAX; // ;//普通则直接采用全集。
    BYTE *buf=(BYTE *)malloc(cell_width_ext);
    //BYTE *buf=(BYTE *)malloc(table_obj_stru.CellWidth+ CELLWIDTH_BUF_ADD_MAX); //CELLWIDTH_BUF_ADD_MAX足够IDS的字串对象的空间空间，???但是尚未解决字段重复可能超长问题。
    
    //get one record;
    BYTE rt = 0;
    //通过记录数，取得指定记录信息
    if (BB_GET_OBJ_DIM(table_obj_stru.IndexObj) == 1) {//!!!上面num多查了一次 //ly 提速。
        //索引读取
        BB_READ_TABLE_BY_INDEX(table_obj, buf, &bufoffset, (BYTE *)nodeid_str.data(), nodeid_str.size(), (BYTE *)bitmap_str.data(), bitmap_str.size(), method, add_nodeid);
    }
    else {
        //原始读取
        if (!BB_READ_TABLE_BY_NUM(table_obj, num, buf, &bufoffset, (BYTE *)bitmap_str.data(), bitmap_str.size(), method)) {
            this->SHOW_ERROR((string)"BuildBufferFromDB()->BB_READ_TABLE_BY_NUM,Load key error! num=" + STRING_Helper::iToStr(num));
            rt = ACTION_ERROR_READ_BUILDER_READ_TABLE_BUFF_BY_NUM_ERROR;
            goto end;
        }
        //添加索引信息
        if (add_nodeid) {
            if ((method & BB_TABLE_REC_WR_BITIDS) == BB_TABLE_REC_WR_BITIDS) {
                BB_OBJ *index_obj = BB_NEW_BYTES_ALL(nodeid_str.data(), nodeid_str.size(), NULL);
                buf_str.append((const char *)index_obj, BB_GET_OBJ_SIZE(index_obj));
                if (index_obj != NULL)
                    free(index_obj);
            }else
                buf_str.append(nodeid_str.data(), nodeid_str.size()); //ly table_dir error
        }
    }
    if ( cell_width_ext >= bufoffset)
        buf_str.append((const char *)buf, bufoffset);
    else{
        this->SHOW_ERROR((string)"BuildBufferFromDB()->BB_READ_TABLE_BY_NUM,fatal memcpy ! bufoffset=" + STRING_Helper::iToStr(bufoffset) +" cell_width_ext="+ STRING_Helper::iToStr(cell_width_ext));
        rt=ACTION_ERROR_READ_BUILDER_FATLE;
    }
    
    //查询是否有updataflag标记，并清除updataflag已读标记
    if ((BB_TABLE_REC_WR_QUERY_UPLOADFLAG & method)==BB_TABLE_REC_WR_QUERY_UPLOADFLAG){ //今后这部分可以清除，完全用StoreProcedure替代 ly:add
        //清除上传字串
        //if (this->GetAddInfoUINT(tableindex, AVARTDB_ADDINFO_UPLOADFLAG_FIELD,col)){
        if (upload_flag_col!=0xffff || this->GetAddInfoUINT(table_obj_stru.AddInfoObj, AVARTDB_ADDINFO_UPLOADFLAG_FIELD,upload_flag_col)){//ly upload
            //判断UPLOADFLAG是否为空
            UInt64 upload_field;
            if (BB_GET_TABLE_ITEM_VALUE(table_obj,num,upload_flag_col,&upload_field) && upload_field==0){
                //continue;
            }else{
                UInt64 clear_bitmap=0;
                BYTE clear_field_width = BB_GET_TABLE_FIELD_WIDTH(table_obj_stru.CellObj, upload_flag_col);  //可提速
                //设置Uploadfield为空;
                if (!BB_PUT_TABLE_ITEMS_DATA(table_obj,num,upload_flag_col,(BYTE*)&clear_bitmap, clear_field_width,BB_TABLE_PUT_OPERATE_SET)){
                    this->SHOW_ERROR((string)"BuildBufferFromDB()->add_nodeid->BB_PUT_TABLE_ITEMS_DATA(),IndexObj error! num="+STRING_Helper::iToStr(num)+"; upload_flag_col="+STRING_Helper::iToStr(upload_flag_col));
                    rt=ACTION_ERROR_READ_BUILDER_PUT_TABLE_ITEM_ERROR_BY_UPLOAD_FLAG;
                    goto end;
                }
                marked_save=true;
            }
        }
    }
    
    if (marked_save)
        this->SaveDB(tableindex);//ly table_sync
    
end:
    if (buf!=NULL)
        free(buf);
    return rt;
}

BYTE AvartarDB_Action::BuildBufferFromDB_AllNode(string &buf_str,string &tableindex_in, string &bitmap_str, string &conditions, string cursor,AVARTDB_AUTH_MASK_TYPE access_mask,BYTE method,bool add_nodeid,bool group_result){
    //ly table_filter 原conditions 改为conditions_in; //ly table_redir
    //取得源表格对象地址
    BB_OBJ *table_obj;
    BB_SIZE_T table_obj_size,table_cell_width=0;
    BYTE rt=0;
    string clear_bitmap=STRING_Helper::putUINTToStr(0,32);
    UInt64 rec_num=0;
    BYTE key_num1 = 0;
    string buf_tmp("");
    string tableindex(tableindex_in); //ly table_filter //ly table_redir
    //string conditions(conditions_in);
    
    
    //ly table_sync3; //ly uploadflags
    UInt64 upload_flag_col=0xffff;
    BB_SIZE_T upload_flag_col_width=0;
    BYTE * upload_flag_offset=NULL;
    bool marked_save=false; //uploadflage 设定后需要存盘
    //ly rec_chg_mark
    bool do_rec_chg_mark=false;
    bool condition_upload_flag=false;
    BB_SIZE_T bit_num=0;
    BB_SIZE_T search_cnt=0;
    
    //正常
    if (!this->GetTableObject(tableindex,&table_obj,table_obj_size)){
        this->SHOW_ERROR((string)"BuildBufferFromDB(),Load table error!");
        return ACTION_ERROR_READ_BUILDER_CAN_NOT_GET_TABLE;
    }
    BB_OBJ_STRU table_obj_stru=BB_GET_OBJ_STRU(table_obj);
    
    //重定向测试 //ly table_filter //ly table_redir
    if (this->CheckGetTableFilter(table_obj_stru,conditions)<0){
        return ACTION_ERROR_READ_BUILDER_CAN_NOT_GET_TABLE;
    }
    
    //ly table_redir
    BB_OBJ *redir_obj=this->CheckGetTableRedir(tableindex,table_obj_stru);
    if (redir_obj==NULL){//如果是重定向table，则tableindex\table_stru改为新table
        this->SHOW_ERROR((string)"BuildBufferFromDB(), Redir Load table error!");
        return ACTION_ERROR_READ_BUILDER_CAN_NOT_GET_TABLE;
    }else if (table_obj!=redir_obj){
        table_obj_stru=BB_GET_OBJ_STRU(redir_obj);
        table_obj=redir_obj;
    }
    
    //原
    if (table_obj_stru.Type != BB_TABLE) {
        this->SHOW_ERROR((string)"BuildBufferFromDB(),Load table error: obj.type!= BB_TABLE !");
        return ACTION_ERROR_READ_BUILDER_DESTOBJ_IS_NOT_TABLE;
    }
    
    //取得RecChgMark //ly table_sync3 //ly rec_chg_mark
    BB_SIZE_T size_old=conditions.size();
    STRING_Helper::strReplace(conditions, this->uploadflag_str, "");
    if (size_old!=conditions.size()){
        condition_upload_flag=true;
        size_old=conditions.size();
    };
    BIT_SEARCH_STRU chg_mark_obj;
    STRING_Helper::strReplace(conditions, this->rec_chg_mark_str, "");
    if (size_old!=conditions.size())
        do_rec_chg_mark=this->GetRecChgMarkObj(table_obj_stru.AddInfoObj,"REC_CHG_MARK",&chg_mark_obj);
    
    //预开记录空间
    BB_SIZE_T bufoffset=0;
    BYTE *buf=NULL;
    
    //取得起止页码
    UInt32 page_start=0,page_count=DEFAULT_PAGE_COUNT;
    if (cursor.size()!=0){
        if (cursor.size()==QUERY_CURSOR_STR_LEN){
            string tmp=cursor.substr(0,4);
            page_start=(UInt32)STRING_Helper::scanUINTStr(tmp);
            tmp=cursor.substr(4,2);
            page_count=(UInt32)STRING_Helper::scanUINTStr(tmp);
        }else{
            this->SHOW_ERROR((string)"BuildBufferFromDB(),cursor must be 6 bytes len!");
            rt=ACTION_ERROR_READ_BUILDER_CURSOR_MUST_6_BYTE_LEN;
            goto end;
        }
    }
    //测试段0
    
    //UPLOADFLAG判断，和取得相关参数。
    if (table_obj_stru.Num!=0 && this->GetAddInfoUINT(table_obj_stru.AddInfoObj, AVARTDB_ADDINFO_UPLOADFLAG_FIELD,upload_flag_col)){
        //为了快速递进需要取得:table_cell_width,upload_flag_col_width,upload_flag_offset
        table_cell_width=BB_GET_TABLE_CELL_WIDTH(table_obj_stru.CellObj);
        upload_flag_col_width=BB_GET_TABLE_FIELD_WIDTH(table_obj_stru.CellObj,upload_flag_col);
        BB_SIZE_T item_size=0;
        if (!BB_GET_TABLE_ITEMS_DATA_STRU(&table_obj_stru,0,upload_flag_col,&upload_flag_offset,&item_size)){//取得第一条字段值，后面循环：直接通过tablecellwidth累加即可。
            this->SHOW_ERROR((string)"BuildBufferFromDB(),BB_TABLE_REC_WR_QUERY_UPLOADFLAG not found!");
            rt=ACTION_ERROR_READ_BUILDER_CURSOR_MUST_6_BYTE_LEN;
            goto end;
        }
    }
    
    //循环获取conditions的各级key;
    BB_SIZE_T key[QUERY_MAX_CONDITIONS_SIZE];
    //ly index 如果发现conditions第一个为#? 且AddInfo存在该，
    //则conditions后移，把前面部分作为index_start起点。num起点为第一个Index
    //否：conditions不变，Index发现后移。
    if (!(conditions.size()==0) && (key_num1=BB_CONDITIONS_GET_KEYS(( BYTE *)(conditions.data()),conditions.size(),key,QUERY_MAX_CONDITIONS_SIZE,&table_obj_stru))==0){
        this->SHOW_ERROR((string)"BuildBufferFromDB()->BB_PARA_GET_KEY_VALUE,QUERY_CONDITIONS Load key error!");
        rt=ACTION_ERROR_READ_BUILDER_GET_CONDITIONS_KEY_ERROR;
        goto end;
    }
    
    //测试段1
    
    //BYTE UInt64(this->GetAddInfoUINT(tableindex, AVARTDB_ACCESS_MASK_FIELD, col);
    for(BB_SIZE_T num=0; num<table_obj_stru.Num; num++){//循环所有表内记录
        //ly table_sync3; //ly rec_chg_mark
        if (do_rec_chg_mark && chg_mark_obj.is_changed && (num < chg_mark_obj.max_num)){//如果有rec_chg_mark; 且在范围内;chg_mark_obj.obj A8为1
            if (bit_num==0 || (bit_num >= chg_mark_obj.bit_step)){
                bit_num=0;
                BB_SIZE_T data_pos=num/chg_mark_obj.byte_step;
                BYTE data_bit=num % chg_mark_obj.byte_step;
                if((data_bit==0) && (chg_mark_obj.data[data_pos]==0) ){//如果为节起点，且为0，则BYTE跃进
                    num+=(chg_mark_obj.byte_step-1);
                    continue;
                }else{//否则bit跃进
                    BYTE bit_pos=data_bit/chg_mark_obj.bit_step;
                    BYTE bit_cmp=0x80 >> bit_pos;
                    if ((chg_mark_obj.data[data_pos] & bit_cmp)==0){//如果bit为0;则bit跃进
                        num+=(chg_mark_obj.bit_step-1);
                        continue;
                    }
                }
            }
            bit_num++;
        };
        
        //test uploadflag
        if (condition_upload_flag && upload_flag_col_width!=0){//ly upload //ly table_sync3; 此处可提速UINT32;UINT64直接分析
            //判断UPLOADFLAG变化; //判断是否为0000//为了快速递进需要取得 table_cell_width,upload_flag_col_width,upload_flag_offset
            if (this->TestByteAll(upload_flag_offset+(num*table_cell_width),upload_flag_col_width,0))
                continue;
        }
        
        //测试conditions,是否符合条件
        if (conditions.size()!=0){//读取符合条件记录情况rec_access_mask
            BYTE *item;BB_SIZE_T item_size;
            BYTE field_type;
            BB_OBJ *p_obj;
            int i;    bool result=true;
            
            for(p_obj=(BB_OBJ *)conditions.data(), i=0; i<key_num1 ;p_obj+=BB_GET_OBJ_SIZE(p_obj),i++){
                if (!BB_GET_TABLE_ITEMS_DATA_TYPE_VALUE(&table_obj_stru, (UInt32)num,(UInt32)key[i],&item,&item_size,&field_type)){
                    this->SHOW_ERROR((string)"BuildBufferFromDB_AllNode()->BB_GET_TABLE_ITEMS_DATA_STRU,Load key error! num="+STRING_Helper::iToStr(num)+"; key="+STRING_Helper::uToStr(key[i]));
                    rt=ACTION_ERROR_READ_BUILDER_GET_TABLE_ITEM_ERROR_BY_CONDITIONS_KEY;
                    goto end;
                }
                //string test((const char *)item,item_size);
                //取得请求表，中index为key的字段。
                if(!BB_PARA_CONDITION_TEST(p_obj,item,item_size,&field_type,&table_obj_stru)){
                    result=false;
                    break;//对比key_value和value内容。
                }
            }
            if (!result)
                continue;
        }
        //测试段3
        //测试access_mask是否符合条件
        UInt64 col;
        if (!this->TestFFFF(access_mask) && this->GetAddInfoUINT(table_obj_stru.AddInfoObj, AVARTDB_ADDINFO_ACCESS_MASK_FIELD, col)) {
            //ly err_block test? for Builder_one next func;
            BYTE *item;
            BB_SIZE_T item_size;
            if (BB_GET_TABLE_ITEMS_DATA_STRU(&table_obj_stru, num, col, &item, &item_size) &&
                !this->TestMask((const char*)item, item_size, access_mask.c_str(), access_mask.size())) {
                return ACTION_ERROR_READ_BUILDER_NODEID_NO_AUTH; //ly ???:应该是continue；这应才能起到过滤作用。
            };
        }
        
        if ((BB_TABLE_REC_WR_QUERY_UPLOADFLAG & method)==BB_TABLE_REC_WR_QUERY_UPLOADFLAG && upload_flag_col_width!=0){//ly upload //ly table_sync3;
            //清空字段
            UInt64 clear_bitmap=0;
            BYTE clear_field_width = BB_GET_TABLE_FIELD_WIDTH(table_obj_stru.CellObj, upload_flag_col);  //可提速
            //设置Uploadfield为空;
            if (!BB_PUT_TABLE_ITEMS_DATA(table_obj,num,upload_flag_col,(BYTE*)&clear_bitmap, clear_field_width,BB_TABLE_PUT_OPERATE_SET)){
                this->SHOW_ERROR((string)"BuildBufferFromDB()->add_nodeid->BB_PUT_TABLE_ITEMS_DATA(),IndexObj error! num="+STRING_Helper::iToStr(num)+"; upload_flag_col="+STRING_Helper::iToStr(upload_flag_col));
                rt=ACTION_ERROR_READ_BUILDER_PUT_TABLE_ITEM_ERROR_BY_UPLOAD_FLAG;
                goto end;
            }
            marked_save=true; //ly table_sync3 //今后可以提速
        }
        
        //测试段3.5
        //continue;
        //察看page_star,page_count
        rec_num++;    //符合条件的记录计数。
        if (cursor.size()!=0){
            if (rec_num < page_start)
                continue;
            if (rec_num >= (page_start+page_count))
                break;
        }
        
        //测试段4
        buf_tmp.clear();
        //预开记录空间buf; //ly malloc2.1
        BB_SIZE_T cell_width_ext;
        if (BB_CHECK_METHOD(method) == BB_TABLE_REC_WR_BITIDS)
            cell_width_ext=BB_GET_TABLE_ITEMS_IDS_WIDTH_EXT(&table_obj_stru,num,(BYTE *)(bitmap_str.data()),bitmap_str.size())+bitmap_str.size()*8; //+CELLWIDTH_BUF_ADD_MAX; //对于ids的缩减内存
        else
            cell_width_ext=BB_GET_TABLE_CELL_WIDTH_EXT(&table_obj_stru,num)+CELLWIDTH_BUF_ADD_MAX; // ;//普通则直接采用全集。
        BYTE *buf=(BYTE *)malloc(cell_width_ext);
        
        //通过索引，取得指定记录信息
        if (!BB_READ_TABLE_BY_NUM(table_obj,num,buf, &bufoffset,(BYTE *)bitmap_str.data(),bitmap_str.size(),method)){
            this->SHOW_ERROR((string)"BuildBufferFromDB()->BB_READ_TABLE_BY_NUM,Load key error! num="+STRING_Helper::iToStr(num));
            rt=ACTION_ERROR_READ_BUILDER_READ_TABLE_BUFF_BY_NUM_ERROR;
            goto end;
        }
        buf_tmp.append((const char *)buf,(size_t)bufoffset);
        bufoffset=0;    //???test 不是累加么？
        //测试段5
        
        //补充添加index_str(nodeid);
        BYTE index_type= BB_GET_OBJ_TYPE(table_obj_stru.IndexObj);
        if (add_nodeid && (index_type==BB_ARRAY ||index_type==BB_TABLE || BB_IS_UINT(table_obj_stru.IndexObj))){
            //string header;
            string index_str(""),index_tmp("");
            BYTE *index_str_32; BB_SIZE_T index_size_32;
            if (index_type == BB_ARRAY || index_type == BB_TABLE){
                //如果有索引
                if (!BB_GET_ARRAY_REC_BY_NUM(table_obj_stru.IndexObj,num,&index_str_32,&index_size_32)){
                    this->SHOW_ERROR((string)"BuildBufferFromDB()->add_nodeid->BB_GET_ARRAY_REC_BY_NUM(),IndexObj error! num="+STRING_Helper::iToStr(num));
                    rt=ACTION_ERROR_READ_BUILDER_ADD_NODEID_INDEX_ERROR;
                    goto end;
                }//ly index_size:
            }
            else if (BB_IS_UINT(table_obj_stru.IndexObj)) {//以位置为索引，IndexObj为宽度
                index_tmp = STRING_Helper::putUINTToStr(num, 8 * BB_GET_OBJ_DATA_LEN(table_obj_stru.IndexObj));
                index_str_32 = (BYTE *)index_tmp.data();
                index_size_32 = index_tmp.size();
            }else {
                //如果没有索引则以第一条记录为索引,好像找不到
                BB_GET_TABLE_ITEMS_DATA_STRU(&table_obj_stru,num,0,&index_str_32,&index_size_32);
            }
            
            if ((method & BB_TABLE_REC_WR_BITIDS) == BB_TABLE_REC_WR_BITIDS) {//对象化index_str;ly ids
                BYTE *data_obj = BB_NEW_BYTES_ALL(index_str_32, index_size_32, NULL);//短字串
                index_str = string((const char *)data_obj, BB_GET_OBJ_SIZE(data_obj));
                if (data_obj!=NULL)
                    free(data_obj);
            }else
                index_str=string((const char *)index_str_32,index_size_32);
            buf_tmp.insert(0,index_str);
        }
        if (group_result) {
            //如果需要添加group
            BB_OBJ cell = BB_NULL;;
            BB_OBJ *group_obj = BB_NEW_ARRAY(&cell, NULL, NULL, 0, buf_tmp.data(), buf_tmp.size(), NULL);
            buf_str.append(string((const char*)group_obj, BB_GET_OBJ_SIZE(group_obj)));
            if (group_obj != NULL)
                free(group_obj);
        }else
            buf_str.append(buf_tmp);
        /**/
        if (buf)
            free(buf);
        buf=NULL;
    }
    //cout << search_cnt << endl
    if (do_rec_chg_mark && !this->TestByteAll(chg_mark_obj.data,chg_mark_obj.data_len,0)){
        memset(chg_mark_obj.data,0,chg_mark_obj.data_len); //ly rec_chg_mark
        //marked_save=true;
        chg_mark_obj.obj[0]&=0x7f;
    }
    
    //如果为表格结果集,则生成表格头bitmap,
    if ((method & BB_TABLE_REC_WR_RETURN_TABLE)==BB_TABLE_REC_WR_RETURN_TABLE){
        BB_OBJ *buf_table_cell;
        //取得cell表格头
        BB_OBJ *convert_cell_ids=(BB_OBJ *)bitmap_str.data();
        if (convert_cell_ids!=NULL){
            if (!this->_BuildBufferFromDB_ReturnTableHead(table_obj,convert_cell_ids,bitmap_str,&buf_table_cell)){//采用转换表格头
                this->SHOW_ERROR((string)"BuildBufferFromDB()->_BuildBufferFromDB_ReturnTableHead(),IndexObj error! num=");
                rt=ACTION_ERROR_READ_BUILDER_BUILD_RETURN_TABLE_HEAD_ERROR;
                goto end;
            }
        }else{
            buf_table_cell=table_obj_stru.CellObj;//采用自身表格头.??? ly:应该加入bitmap表格过滤
        }
        bitmap_str.clear();
        bitmap_str.append((const char*)buf_table_cell,BB_GET_OBJ_SIZE(buf_table_cell));
        if (convert_cell_ids!=NULL && buf_table_cell!=NULL){
            free(buf_table_cell); //只删除convert_ids动态生成的表格内存。
        }
    }
    if (marked_save)
        this->SaveDB(tableindex); //ly table_sync3
    
    //rt=buf_str.size();
end:
    if (buf)
        free(buf);
    return rt;
}

bool AvartarDB_Action::BuildBuffer_TableCnvNumToNodeID(BB_OBJ_STRU table_obj_stru,BB_SIZE_T num,string &index_str,bool cnv_to_obj){
    //补充添加index_str(nodeid); //ly malloc2.1
    BYTE index_type= BB_GET_OBJ_TYPE(table_obj_stru.IndexObj);
    int rt;
    if (index_type==BB_ARRAY ||index_type==BB_TABLE || BB_IS_UINT(table_obj_stru.IndexObj)){
        //string header;
        string index_tmp("");
        BYTE *index_str_32; BB_SIZE_T index_size_32;
        if (index_type == BB_ARRAY || index_type == BB_TABLE){
            //如果有索引
            if (!BB_GET_ARRAY_REC_BY_NUM(table_obj_stru.IndexObj,num,&index_str_32,&index_size_32)){
                this->SHOW_ERROR((string)"BuildBuffer_TableCnvNumToNodeID()->add_nodeid->BB_GET_ARRAY_REC_BY_NUM(),IndexObj error! num="+STRING_Helper::iToStr(num));
                rt=ACTION_ERROR_READ_BUILDER_ADD_NODEID_INDEX_ERROR;
                return false;
            }//ly index_size:
        }
        else if (BB_IS_UINT(table_obj_stru.IndexObj)) {//以位置为索引，IndexObj为宽度
            index_tmp = STRING_Helper::putUINTToStr(num, 8 * BB_GET_OBJ_DATA_LEN(table_obj_stru.IndexObj));
            index_str_32 = (BYTE *)index_tmp.data();
            index_size_32 = index_tmp.size();
        }else {
            //如果没有索引则以第一条记录为索引,好像找不到
            BB_GET_TABLE_ITEMS_DATA_STRU(&table_obj_stru,num,0,&index_str_32,&index_size_32);
        }
        
        if (cnv_to_obj) {//对象化index_str;ly ids
            BYTE *data_obj = BB_NEW_BYTES_ALL(index_str_32, index_size_32, NULL);//短字串
            index_str = string((const char *)data_obj, BB_GET_OBJ_SIZE(data_obj));
            if (data_obj!=NULL)
                free(data_obj);
        }else
            index_str=string((const char *)index_str_32,index_size_32);
        return true;
    }
    return false;
}

bool AvartarDB_Action::_BuildBufferFromDB_ReturnTableHead(BB_OBJ *table_obj,BB_OBJ *convert_table_obj,string &bitmapstr,BB_OBJ **convert_obj){
    //创建返回表格头数组：(1)ARRAY_INDEX模式，Content-源field宽度，Index-目标数据库id(需要根据传入的convert_obj数组结构)
    //取得表格对象地址
    BB_SIZE_T field_size;
    BB_OBJ_STRU convert_table_obj_stru=BB_GET_OBJ_STRU(convert_table_obj);
    BB_OBJ_STRU table_obj_stru=BB_GET_OBJ_STRU(table_obj);
    
    //生成数组型Index
    BB_OBJ *index_array=BB_NEW_ARRAY_CELL(BB_UINT8,BB_UINT8,BB_NULL,convert_table_obj_stru.Num,0,0,NULL); //??? 今后也许不是BB_UINT8
    //创建索引数组
    (*convert_obj) = BB_NEW_ARRAY_INDEX_CELL(BB_UINT8,index_array,BB_NULL,convert_table_obj_stru.Num,0,0,NULL);//生成带上述索引的空数组
    if (index_array)
        free(index_array);     //ly err_block test?
    //填写索引数组
    UInt64 src_id,field_id;
    BYTE field_id_byte,field_size_byte;
    for(size_t i=0; i<convert_table_obj_stru.Num; i++){//循环填写数组内容和索引
        if (!BB_GET_ARRAY_UINT_VALUE(convert_table_obj_stru.IndexObj,i,&field_id)){//取得字段id信息
            printf("%s %zu\n","_DoRegTableTask_BuildBufferFromDB_ReturnTableHead()error! sync_index_field=",i);
            return false;//to_add
        }
        if (!BB_GET_ARRAY_UINT_VALUE_STRU(&convert_table_obj_stru,i,&src_id)){//取得目标id
            printf("%s i=%zu \n","BB_READ_TABLE_BY_NUM()->BB_GET_ARRAY_UINT_VALUE: can't read ids index !",i);
            return false;//to_add
        }
        if ((field_size=BB_GET_TABLE_FIELD_WIDTH(table_obj_stru.CellObj,src_id))==BB_SIZE_T_ERROR){//取得宽度信息
            printf("%s %d\n","_DoRegTableTask_BuildBufferFromDB_ReturnTableHead()error! cell_width_field=",BB_TABLE_DEFAULT_CELL_LENGTH_FIELD);
            return false;//to_add
        }
        field_id_byte=field_id;
        field_size_byte = field_size;// ly !!! 主要用于9331/pc,BB_SIZE_T 到byte的转换不行
        //cout << "(" << field_id << "*" << src_id << "*" << field_size << "*" <<(int) field_id_byte << ")";
        //BB_APPEND_ARRAY_REC_BY_INDEX((*convert_obj), (BYTE *)&field_size, sizeof(field_size), (BYTE *)&field_id, sizeof(field_id));
        BB_APPEND_ARRAY_REC_BY_INDEX((*convert_obj),(BYTE *)&field_size_byte,sizeof(field_size_byte),(BYTE *)&field_id_byte,sizeof(field_id_byte));//???这其中似乎有数字顺序转换问题
    }
    //string test3((const char*)(*convert_obj), BB_GET_OBJ_SIZE((*convert_obj)));
    //cout << STRING_Helper::HBCStrToHBCStr(test3,"char","hex",true) <<endl;
    return true;
}

BYTE AvartarDB_Action::BuildBufferFromDB_AddNodeID(string &index_str, BB_OBJ_STRU table_obj_stru, BB_SIZE_T num, BYTE method) {
    BYTE *index_str_32; BB_SIZE_T index_size_32;
    if (table_obj_stru.IndexObj != BB_NULL) {
        BB_OBJ_STRU index_stru=BB_GET_OBJ_STRU(table_obj_stru.IndexObj);
        //如果有索引
        if (index_stru.Type==BB_TABLE){//ly table_dir
            UInt64 id_field=0;
            if (this->GetAddInfoUINT(index_stru.AddInfoObj,"INDEX_FIELD",id_field))
                BB_GET_TABLE_ITEMS_DATA_STRU(&index_stru, num, id_field, &index_str_32, &index_size_32);
            else
                BB_GET_TABLE_ITEMS_DATA_STRU(&index_stru, num, 0, &index_str_32, &index_size_32);
        }else{
            if (!BB_GET_ARRAY_REC_BY_NUM(table_obj_stru.IndexObj, num, &index_str_32, &index_size_32)) {
                this->SHOW_ERROR((string)"BuildBufferFromDB()->add_nodeid->BB_GET_ARRAY_REC_BY_NUM(),IndexObj error! num=" + STRING_Helper::iToStr(num));
                return ACTION_ERROR_READ_BUILDER_ADD_NODEID_INDEX_ERROR;
            }//ly index_size:
        }
    }
    else {
        //如果没有索引则以第一条记录为索引
        BB_GET_TABLE_ITEMS_DATA_STRU(&table_obj_stru, num, 0, &index_str_32, &index_size_32);
    }
    
    if ((method & BB_TABLE_REC_WR_BITIDS) == BB_TABLE_REC_WR_BITIDS) {//对象化index_str;ly ids
        BYTE *data_obj = BB_NEW_CELL(BB_BYTES, index_str_32, ((index_size_32 > BB_0DIM_BYTES_MAX_LEN) ? BB_0DIM_BYTES_MAX_LEN : index_size_32), NULL);//短字串
        index_str = string((const char *)data_obj, BB_GET_OBJ_SIZE(data_obj));
        if (data_obj != NULL)
            free(data_obj);
    }
    else
        index_str = string((const char *)index_str_32, index_size_32);
    return 0;
}


int AvartarDB_Action::LoadExt(BB_OBJ_STRU &table_obj_stru, string &nodeid_str, string &ext_name,BYTE **buf, BB_SIZE_T &start, BB_SIZE_T &length, byte open_mode, bool from_start)
{
    string file_name=this->GetExtPathName(table_obj_stru.AddInfoObj,nodeid_str,ext_name);
    
    if (file_name.size() != 0) {
        if (LW_access(file_name.c_str(), 0) <0)
            return ACTION_ERROR_EXT_DEVICE_EXT_FILE_NOT_FOUND;//判断文件是否已经存在
        
        std::fstream f(file_name.c_str(), ios::in | ios::binary);
        if (f.good()){
            //读取全部文档
            f.seekg(0, std::ios::end);
            BB_SIZE_T fl = f.tellg();
            if (start > fl)
                return ACTION_ERROR_EXT_DEVICE_IS_OUT_OFF_RANGE;  //参数超出范围
            if ((start + length) > fl)
                length = (fl - start);//调整length；
            
            //定位启迪指针
            if (from_start)
                f.seekg(start);
            else
                f.seekg(start+length, std::ios::end);
            //开内存,读取信息
            char *buffer = (char *)malloc(length + 1);
            memset(buffer, 0, length + 1);
            f.read(buffer, length);
            //赋值
            (*buf) = (BYTE *)buffer;
            
            f.close();
        }else{
            f.close();
            return ACTION_ERROR_EXT_DEVICE_CAN_NOT_OPEN_FILE;
        }
    }else
        return ACTION_ERROR_EXT_DEVICE_ADDINFO_SAVE_FILE_PATH_NOT_DEFINE;
    return 0;
}

string AvartarDB_Action::GetExtPathName(BB_OBJ *add_info,string &nodeid_str,string ext_name){
    //取得文件名称
    string ext_str=ext_name.size()==0 ? this->GetAddInfo(add_info, AVARTDB_SAVE_MALLOC_EXT) : ext_name;
    string addinfo_name = this->GetAddInfo(add_info, AVARTDB_SAVE_FILE_NAME);
    if (addinfo_name.size() == 0)
        return "";
    
    //拼凑新的file_name
    string file_name;
    if (nodeid_str.size()==0)
        file_name = ext_str; //取得主对象名称;
    else{
#ifdef AvartarDB_Base_SaveExt_CreateDir
        string path = addinfo_name.substr(0, addinfo_name.rfind("."));
        if (path.size()!=0){
            this->CreateDir(path);
            file_name = path+"/"+ STRING_Helper::HBCStrToHBCStr(nodeid_str,"char","hex",false) + "."+ext_str; //取得主对象名称;
        }else{
            file_name = "./"+ STRING_Helper::HBCStrToHBCStr(nodeid_str,"char","hex",false) + ext_str; //取得主对象名称;
        }
#else
        string path = addinfo_name.substr(0, addinfo_name.rfind("/"));
        file_name = (path.size() == 0 ? "." : path) + "/" + STRING_Helper::HBCStrToHBCStr(nodeid_str, "char", "hex", false) + ext_str; //取得主对象名称;
#endif
    }
    return file_name;
}

int AvartarDB_Action::SaveExt(BB_OBJ_STRU &table_obj_stru, string &nodeid_str, string &ext_name, BYTE *buf, BB_SIZE_T &start, BB_SIZE_T &length, byte open_mode)
{
    string file_name=this->GetExtPathName(table_obj_stru.AddInfoObj,nodeid_str,ext_name);
    
    if (file_name.size() != 0) {
        //if (LW_access(file_name.c_str(), 0) <0)
        //return ACTION_ERROR_EXT_DEVICE_EXT_FILE_NOT_FOUND;//判断文件是否已经存在
        if (LW_access(file_name.c_str(), 0) < 0) {
            std::ofstream f(file_name.c_str(), ios::binary);
            f.close();
        };
        
        std::fstream f(file_name.c_str(),open_mode == FileQueue::OPEN_MODE_TRUNC ? (ios::in | ios::out | ios::binary|ios::trunc) : (ios::in | ios::out | ios::binary));
        if (f.good()) {
            //读取全部文档
            f.seekg(0, std::ios::end);
            BB_SIZE_T fl = f.tellg();
            if (start > fl) {
                //return ACTION_ERROR_EXT_DEVICE_IS_OUT_OFF_RANGE;  //参数超出范围
                for (BB_SIZE_T i = fl; i <=start; i++)
                    f.put(0);//补充0
            }
            
            //开内存,读取信息
            f.seekg(start);
            f.write((char *)buf, length);
            //删除缓存
            f.close();
            return 0;
        }else
            return ACTION_ERROR_EXT_DEVICE_CAN_NOT_OPEN_FILE;
    }else
        return ACTION_ERROR_EXT_DEVICE_ADDINFO_SAVE_FILE_PATH_NOT_DEFINE;
}

string AvartarDB_Action::GetNodeParseValue(string tableindex,string nodeid_str, string &r_parse_value,const char *split_str){
    //读取nodeid的format_flag,和value
    string fmt_data_str(""),fmt_flag_str("");
    
    //递进处理法
    if (this->BuildBufferFromDB_OneNode(fmt_flag_str,tableindex,nodeid_str,STRING_Helper::putUINTToStr(JENNIC_BITMAP_FMT_FLAG,32),AVARTDB_AUTH_MASK_FULL,BB_TABLE_REC_WR_BITMAP,false)==0){    //???FMT_FLAG
        if (this->BuildBufferFromDB_OneNode(fmt_data_str,tableindex,nodeid_str,STRING_Helper::putUINTToStr(JENNIC_BITMAP_FMT_DATA,32), AVARTDB_AUTH_MASK_FULL,BB_TABLE_REC_WR_BITMAP,false)!=0)
            return (string)"Error:can't get nodeid("+nodeid_str+") data step2!";    //???FMT_DATA
    }else
        return (string)"Error:can't get nodeid("+nodeid_str+") data step1! ";
    
    r_parse_value="";
    for(unsigned int i=0,data_p=0,fmt_p=0; i<6; i++){
        //取得长度fmt,data值
        string fmt_str=fmt_flag_str.substr(fmt_p,4);
        byte data_len=this->JennicDataParseLen((byte *)fmt_str.c_str());
        if (data_p+data_len > fmt_data_str.size()){
            cout <<  (string)"Error: nodeid("+STRING_Helper::HBCStrToHBCStr(nodeid_str,"char","hex",false)+") data ["+STRING_Helper::iToStr(i)+"]out of size! " << endl;
            return "";
        }
        string fmt_data=fmt_data_str.substr(data_p,data_len);
        //计算字串
        r_parse_value+=this->JennicDataParseValue(fmt_str,fmt_data);//计算fmt长度
        if (i!=0)
            r_parse_value+=split_str;
        //下一个
        fmt_p+=4; data_p+=data_len;//已初始化，正常处理。
    }
    return "";
}


int AvartarDB_Action::CreateTableIndexStr(BB_OBJ_STRU *table_stru, UInt64 index_num, string &rindex){//ly task
    //取得index field
    UInt64 index_field;
    if (!this->GetAddInfoUINT(table_stru->AddInfoObj,AVARTDB_ADDINFO_INDEX_FIELD,index_field)){
        return ACTION_ERROR_WRITE_ACTION_CYCLE_ADD_NOT_INDEX_FIELD_DEFINED;
    }
    //根据宽度生成rindex
    if (BB_GET_OBJ_TYPE(table_stru->IndexObj)==BB_ARRAY){////如果有INDEX数组，则直接去的字段值
        rindex=this->GetTableItemString(table_stru, index_num, (BYTE)index_field);
        if (rindex=="")
            return -1;
        else
            return 0;
    }else{//如果无INDEX数组，则直接吧index_num变为index
        BYTE index_str_buf[16];
        BYTE size=BB_GET_TABLE_FIELD_WIDTH(table_stru->CellObj,(BYTE)index_field);
        BB_PUT_UINT_TO_BUFF(index_str_buf, index_num, size);
        rindex=string((char *)index_str_buf, size);
        return 0;
    }
}


