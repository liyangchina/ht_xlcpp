
#include <iostream>
#include "STRING_Helper.h"
//#include "WizBase.h"
#include "ParseObj.h"
#include "ParseExternal.h"
#if !defined(__ANDROID__) && !defined(__APPLE__)
	#include "sha1.h"
	#include "cws.h"
	#include "base64.h"
#endif
#include "Linux_Win.h"
#define MSHINET3
//#include "CChineseCode.h"

string ParseExt::DECODE(const char *method, string &parse_buff, Http_ParseObj *pub_obj, string &to_obj){
	//vector<string> parse_field;
	//STRING_Helper::stringSplit(parse_buff,parse_field,"\r\n");
	if (strstr(method,"@_HTTP")==method){
		return ParseExt_HTTP::DECODE(method,parse_buff,pub_obj,to_obj);
	}
#if !defined(__ANDROID__) && !defined(__APPLE__)
	else if (strstr(method,"@_WEBSOCKET")==method){
		return ParseExt_WEBSOCKET::DECODE(method,parse_buff,pub_obj,to_obj);
	}
#endif
    else if (strstr(method, "@_MSHINET_CMD") == method) {
        return ParseExt_MSHINET_CMD::DECODE(method, parse_buff, pub_obj, to_obj);
    }
	else if (strstr(method,"@_MSHINET")==method){
		return ParseExt_MSHINET::DECODE(method,parse_buff,pub_obj,to_obj);
	}else if (strstr(method, "@_MSHINET_OLD") == method) {
		return ParseExt_MSHINET2::DECODE(method, parse_buff, pub_obj, to_obj);
    }
	else{
		return "Error(PARSE):parse name error! ";
	}
}

string ParseExt::ENCODE(vector<string> &paras, string &func_return){
	
	if (paras[0]=="@_MSHINET"){
		paras.erase(paras.begin());
		return ParseExt_MSHINET::ENCODE(paras,func_return);
	}else if (paras[0] == "@_MSHINET_OLD") {
		paras.erase(paras.begin());
		return ParseExt_MSHINET2::ENCODE(paras, func_return);
    }else if (paras[0] == "@_MSHINET_CMD") {
        paras.erase(paras.begin());
        return ParseExt_MSHINET_CMD::ENCODE(paras, func_return);
    }
#if !defined(__ANDROID__) && !defined(__APPLE__)
	else if (paras[0]=="@_WEBSOCKET"){
		paras.erase(paras.begin());
		return ParseExt_WEBSOCKET::ENCODE(paras,func_return);
	}
#endif
	else{
		return "Error(PARSE_ENCODE):parse name error! ";
	}
}

string ParseExt::LEN(vector<string> &paras, string &func_return){
	
	if (paras[0] == "@_HTTP") {
		return ParseExt_HTTP::LEN(paras, func_return);
	}else if (paras[0]=="@_MSHINET"){
		paras.erase(paras.begin());
		return ParseExt_MSHINET::LEN(paras,func_return);
	}else if (paras[0] == "@_MSHINET_OLD") {
		paras.erase(paras.begin());
		return ParseExt_MSHINET2::LEN(paras, func_return);
	}
#if !defined(__ANDROID__) && !defined(__APPLE__)
	else if (paras[0]=="@_WEBSOCKET"){
		paras.erase(paras.begin());
		return ParseExt_WEBSOCKET::LEN(paras,func_return);
	}
#endif
	else{
		return (string)"Error(PARSE_LEN):can't find parse class["+paras[0]+"]!";
	}
}

string ParseExt::FUNC(vector<string> &paras, string &func_return){
	
	if (paras[0]=="@_MSHINET"){
		paras.erase(paras.begin());
		return ParseExt_MSHINET::FUNC(paras,func_return);
	}
#if !defined(__ANDROID__) && !defined(__APPLE__)
	else if (paras[0]=="@_WEBSOCKET"){
		paras.erase(paras.begin());
		return ParseExt_WEBSOCKET::FUNC(paras,func_return);
	}
#endif
	else if (paras[0] == "@_HTTP") {
		paras.erase(paras.begin());
		return ParseExt_HTTP::FUNC(paras, func_return);
	}
	else if (paras[0] == "@_MSHINET_OLD") {
		paras.erase(paras.begin());
		return ParseExt_MSHINET2::FUNC(paras, func_return);
	}
	else{
		return (string)"Error(PARSE_FUNC):can't find parse class["+paras[0]+"]!";
	}
}

string ParseExt_HTTP::DECODE(const char *method, string &parse_buff, Http_ParseObj *pub_obj, string &to_obj){
	//取得属性字段
	string value_str;
	size_t blank_pos, rn_pos = parse_buff.find("\r\n");
	
	if (rn_pos== string::npos)
		return "Error(DECODE):" + parse_buff.substr(0, 300) + ".... is not Http packet(can't find \r\n)!";

	int in_method;
	if ((blank_pos =parse_buff.find(" "),rn_pos) != string::npos)
		value_str = parse_buff.substr(0, blank_pos);
	else 
		value_str = parse_buff.substr(0, rn_pos);
	if (value_str == "GET")
		in_method = GET;
	else if (value_str == "POST")
		in_method = POST;
	else
		return "Error(DECODE):" + parse_buff.substr(0, 300) + "....  is not Http packet(can't find POST/GET)!";

	//取得字段“/r/n”
	vector<string> parse_field;
	size_t tmp_find;
	if (in_method==GET)
		STRING_Helper::stringSplit(parse_buff,parse_field,"\r\n");
	else if ((tmp_find = parse_buff.find("\r\n\r\n")) != string::npos){
		string tmp_parse_buff = parse_buff.substr(0, tmp_find);
		STRING_Helper::stringSplit(tmp_parse_buff, parse_field, "\r\n");
		parse_field.push_back(parse_buff.substr(tmp_find + 4));
	}else
		return "Error(DECODE):" + parse_buff.substr(0, 300) + "....  is not Http packet(POST can't find \r\n\r\n)!";

	//循环操作处理
	string name_str=to_obj;
	name_str+=".";
	//this->_PubObj->Delete(to_obj.c_str(),true);
	for (size_t i=0; i < parse_field.size(); i++){
		if (i==0){
			//http 头
			string tmp=name_str+"@_METHOD";
			//if (parse_field[0].find(" ")!=string::npos)
				//value_str=parse_field[0].substr(0,parse_field[0].find(" "));
			//else
				//value_str=parse_field[0];
			pub_obj->SetNodeValue(tmp.c_str(),value_str.c_str(),Http_ParseObj::STR_COPY_OBJ);

			//URL
			tmp=name_str+"@_URL";
			string tmp2=name_str+"@_URI";
			if (parse_field[0].find(" ")!=string::npos){
				string tmp2=parse_field[0].substr(parse_field[0].find(" ")+1);
				if (tmp2.find(" HTTP")!=string::npos)
					value_str=tmp2.substr(0,tmp2.find(" HTTP"));
				else
					value_str=tmp2;
			}else
				value_str=parse_field[0];
			value_str=URL_DECODE(value_str);
			pub_obj->SetNodeValue(tmp.c_str(),value_str.c_str(),Http_ParseObj::STR_COPY_OBJ);
			pub_obj->SetNodeValue(tmp2.c_str(),value_str.c_str(),Http_ParseObj::STR_COPY_OBJ);

			//PATH
			tmp=name_str+"@_PATH";
			string uri_path;
			size_t p1;
			if ((p1=value_str.find("?"))!=string::npos){
				//SET Path
				uri_path=value_str.substr(0,p1);
				
				//Fields
				string tmp4=value_str.substr(p1+1);
				vector <string> value_s;
				STRING_Helper::stringSplit(tmp4,value_s,"&");
				size_t p2;
				for(size_t i=0; i< value_s.size();i++){
					if ((p2=value_s[i].find("="))!=string::npos){
						string field_name=name_str+value_s[i].substr(0,p2);
						string field_value=value_s[i].substr(p2+1);
						pub_obj->SetNodeValue(field_name.c_str(),field_value.c_str(),Http_ParseObj::STR_COPY_OBJ);
					}
					else {
						string field_name = name_str + "@_PARA_"+STRING_Helper::iToStr(i);
						pub_obj->SetNodeValue(field_name.c_str(), value_s[i].c_str(), Http_ParseObj::STR_COPY_OBJ);
					}
				}
			}else
				uri_path=value_str;
			pub_obj->SetNodeValue(tmp.c_str(),uri_path.c_str(),Http_ParseObj::STR_COPY_OBJ);

			//uri_file_ext
			tmp=name_str+"@_URI_FILE_TYPE";
			string uri_file_ext;
			if ((p1=uri_path.rfind("."))!=string::npos){
				uri_file_ext=uri_path.substr(p1+1);
				if((p1=uri_file_ext.rfind("/"))!=string::npos)
					uri_file_ext="";
			}else
				uri_file_ext="";
			
			pub_obj->SetNodeValue(tmp.c_str(),uri_file_ext.c_str(),Http_ParseObj::STR_COPY_OBJ);		
		//}else if ((i==parse_field.size()-1) && (parse_field[parse_field.size()-2].size()==0)){//内容字串
		}
		else if (in_method==POST && (i == parse_field.size() - 1)) {//内容字串
			string tmp = name_str+"_CONTENT_";
			pub_obj->SetNodeValue(tmp.c_str(),parse_field[i].data(),Http_ParseObj::STR_COPY_OBJ,parse_field[i].size());
		}
		else {//http field
			string tmp=name_str+"@"+parse_field[i].substr(0,parse_field[i].find(":"));
			//STRING_Helper::LowerCase(tmp);
			string tmp2=parse_field[i].substr(parse_field[i].find(":")+1);
			value_str=STRING_Helper::strTrim(tmp2);
			pub_obj->SetNodeValue(tmp.c_str(),value_str.c_str(),Http_ParseObj::STR_COPY_OBJ);
		};
	}
	return "";

}

string ParseExt_HTTP::URL_DECODE(string &url){
	string rs("");
#if WIN32
	char* uri_buf=_strdup ((char *)_strdup(url.data()));
#else
	char* uri_buf=strdup ((char *)strdup(url.data()));
#endif

	STRING_Helper::HTTP_decode_string(uri_buf);
	/*if (CChineseCode::IsTextUTF8(this->uri_,strlen(this->uri_))==TRUE){
		// 将UTF_8转化为GB2312
		string pOut;
		CChineseCode::UTF_8ToGB2312(pOut,uri_buf);
		free(uri_buf);
		uri_buf = strdup(pOut.c_str());
	}*/
	rs = uri_buf;
	free(uri_buf);
    return rs;
}

string ParseExt_HTTP::LEN(vector<string> &paras, string &func_return) {
	//取得content_len
    if (paras.size()==3 && paras[2]=="JAVA_LEN"){//ly_java_len
        cout << STRING_Helper::HBCStrToHBCStr(paras[1], "char", "hex", true)<< endl;
        size_t header_len;
        if ((header_len = paras[1].find("\r\n\r\n")) == string::npos)
            return "header is incomplate (no node: \r\n\r\n)!";
        string len_add_str=paras[1].substr(header_len+4);
        size_t len_end_pos;
        if ((len_end_pos=len_add_str.find("\r\n")) == string::npos)
            return "header is incomplate (java len end (\r\n) exited)!";
        string content_len_str = len_add_str.substr(0, len_end_pos);
        if (!STRING_Helper::CheckNum(content_len_str,"hex"))
            return "header error (java len is not hex)!";
        int content_len = STRING_Helper::ParseInt(content_len_str,16);
        cout << paras[1].substr(0,header_len+content_len+content_len_str.size()+3+7) << endl;
        func_return = STRING_Helper::uToStr(header_len+content_len+content_len_str.size()+3+7);
    }else{
        size_t len_pos,len_end_pos,split_pos;
        if ((len_pos=paras[1].find("Content-Length")) == string::npos)
            return "header is incomplate (Content-Length not exited)!";

        if ((len_end_pos=paras[1].find("\r\n",len_pos)) == string::npos)
            return "header is incomplate (\r\n not exited)!";

        if ((split_pos = paras[1].find(":", len_pos)) == string::npos)
            return "header is incomplate (**:** not exited)!";
        string content_len_str = paras[1].substr(split_pos+1, len_end_pos - split_pos-1);
        int content_len = STRING_Helper::ParseInt(content_len_str,10);

        //取得header_len
        size_t header_len;
        if ((header_len = paras[1].find("\r\n\r\n")) == string::npos)
            return "header is incomplate (no node: \r\n\r\n)!";

        func_return = STRING_Helper::uToStr(header_len+content_len+4);
    }
	return "";
}

string ParseExt_HTTP::FUNC(vector<string> &paras, string &func_return) {
	if (paras[0] == "_UPLOAD_FILE") {
		paras.erase(paras.begin());
		return ParseExt_HTTP::_UPLOAD_FILE(paras, func_return);
	}
	else {
		return (string)"Error(PARSE FUNC):Func[" + paras[0] + "]is not valid!";
	}
}

string ParseExt_HTTP::_UPLOAD_FILE(vector<string> &paras, string &func_return) {
	string file_split = ";";
	string file_name_list = ""; //用于存储创建的文件名。
	int result;

	//new websocket v13:
	string str_temp;
	string str_head_temp;
	string str_split_temp;
	string str_end_temp;
	//str_head_temp = paras[0];    //request.header_values(HTTP_Request::CONTENT_TYPE);
	STRING_Helper::strStringCut(paras[0], str_head_temp, "boundary=", " &\r\n");  
	str_split_temp = "--" + str_head_temp + "\r\n"; //原来是“--”现在“-”为了留一个“0”的字串结尾位置。
	str_end_temp = "--" + str_head_temp + "--";

	//取得文档内容和头信息
	char * content;
	char * content_end;
	char * head_1;
	char * head_1_end;
	char * head_2;
	char * head_2_end;
	char * file_content;
	int file_len;
	const char * src;
	char *tmp_src;
	src = paras[1].data();  //取得数据区
	int content_len = paras[1].size();
	tmp_src = (char *)src + 1;

	int cnt;
	for (cnt = 0;; cnt++) {
		//取得第一个文档块
		content = tmp_src + str_split_temp.size() - 1; //取得第一个文件的头位置
		if (!strcmp(content, "\r\n"))
			break;//已到结尾
		content_end = (char *)STRING_Helper::pMemStrPos(content, (char *)str_split_temp.c_str(), content_len - (content - src)); //取得第一个文件的尾位置
		if (content_end == 0)
			content_end = (char *)STRING_Helper::pMemStrPos(content, (char *)str_end_temp.c_str(), content_len - (content - src)); //取得最后一个文件的尾位置。
		if (content_end == 0) {
			return "-1";//this->resultSend(response, 1, "", false);
			break;
		}

		//取得文档块中的头信息
		head_1 = content;
		head_1_end = strstr(head_1, "\r\n");	//取得Content-Disposition:节
		head_2 = head_1_end + 2;
		head_2_end = strstr(head_2, "\r\n");	//取得Content-Type:节
		file_content = head_2_end + 4;
		file_len = content_end - file_content - 2;
		*head_1_end = 0;
		*head_2_end = 0;

		//文档内容处理操作
		cout << content << ";" << head_1 << ";" << head_2 << endl;		//取得第一个文件。
		string filename;
		result = ParseExt_HTTP::_UPLOAD_FILE_savefiles(paras[2].c_str(), file_content, file_len, head_1, head_2, filename);
		if (result == 1) {
			tmp_src = content_end + 1;	//循环第二个文件
			continue;
		}
			
		if (file_name_list == "")
			file_name_list = filename;
		else
			file_name_list += file_split + filename;

		if (result != 0) {
			return "-2";;    //save_file为iframe模式，不需要转换成utf8,所以为false;
			break;
		}
		//下一个文档
		tmp_src = content_end + 1;	//循环第二个文件
	}

	func_return = file_name_list;
	return "";
}

int ParseExt_HTTP::_UPLOAD_FILE_creat_path_dir(const char * path, string split_str) {
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

int ParseExt_HTTP::_UPLOAD_FILE_savefiles(string path_str, char * file_data, int file_len, char* head1, char*head2, string &save_file_name)
{
	string default_path = "./";
	//取得http来源文件名
	string file_name;
	STRING_Helper::strStringCut(head1, file_name, "filename=\"", "\"");
	//ly new
	STRING_Helper::strReplace(file_name, '\\', '/');
	int i = 0; 
	if ((i =file_name.find_last_of("/"))!=file_name.npos){
		file_name=file_name.substr(i+1);
	}

	if (file_name == "")
		return 1;   //文件不存在

	//取得dest 目录+文件名
	string file_path;
	if (path_str.size() == 0) {
		//裸文件写入情况
		file_path = default_path + file_name;
	}else {
		STRING_Helper::strReplace(path_str, '\\', '/');
		file_path = path_str;   //缺省制定全名的情况
		if (path_str.at(path_str.size() - 1) == '\\' || path_str.at(path_str.size() - 1) == '/') {
			//给定目录的处理情况
			file_path = path_str + file_name;
		}
	}	

	//测试dest目录文件是否存在
	ParseExt_HTTP::_UPLOAD_FILE_creat_path_dir(file_path.c_str(), (string)"/");
	/*if (path_str.size() != 0) {
		
		string path = file_path.substr(0, file_path.rfind("/"));
		if (LW_access(path.c_str(), 0) < 0) {
			ParseExt_HTTP::_UPLOAD_FILE_creat_path_dir(path.c_str(), (string)"/");//注意这里只能对/888//
			//return -3;//文件目录不存在
		}	
	}*/

	//写入操作
	std::ofstream file(file_path.c_str(), ios::binary | ios::out);
	if (!file)
		return -1;
	file.write((const char *)file_data, file_len);
	file.close();

	//返回文件名																	 //移动到目标文件夹
	save_file_name = file_path;		//+ "*" + date + "*" + time + "*" + size; //返回文件名*年月日*时间*大小

	return 0;
}

/*
string urlDecode(string &SRC)
{
    string ret;
    char ch;
    unsigned int i, ii;
    for(i=0; i<SRC.length(); i++)
    {
        if (int(SRC[i])==37)
        {
            sscanf(SRC.substr(i+1,2).c_str(), "%x", &ii);
            ch=static_cast<char>(ii);
            ret+=ch;
            i=i+2;
        }
        else
        {
            ret+=SRC[i];
        }
    }
    return (ret);
}
/// ---------------------------------------------------------------
bool is_directory(string pathname)
{
    struct stat sb;

    if (stat(pathname.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        return true;
    }
    return false;
}

unsigned long get_file_size(const char *filepath)
{
    FILE * f = fopen(filepath, "r");
    if(f == NULL)
    {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(f);
    fclose(f);
    return len;
}

unsigned long get_file_time_last_modified(const char *filepath)
{
    struct stat b;
    if (!stat(filepath, &b))
    {
        return b.st_mtime;
    }
    return 0;
}

bool file_exists(string path)
{
    FILE *file = fopen(path.c_str(), "r");
    if(file != NULL)
    {
        fclose(file);
        return true;
    }
    return false;
}
*/
#if !defined(__ANDROID__) && !defined(__APPLE__)
/***************    WEBSOCKET    *********************************************************/
string ParseExt_WEBSOCKET::LEN(vector<string> &paras, string &func_return){
	if (paras[0].size() <2)
		return "frame is incomplate !";
	if (paras[0].data()[0]=='\0')
        return "frame is error !";

	size_t len=(size_t)ws_frame_len((unsigned char *)paras[0].data());

	func_return=STRING_Helper::iToStr(len);
	return "";
}

string ParseExt_WEBSOCKET::ENCODE(vector<string> &paras, string &func_return){

	ws_frame_type frame_type;
	if (paras[1]=="WS_TEXT_FRAME"){
		frame_type=WS_TEXT_FRAME;
	}else if (paras[1]=="WS_BINARY_FRAME") {
		frame_type=WS_BINARY_FRAME;
	}else if (paras[1]=="WS_PING_FRAME"){
		frame_type=WS_PING_FRAME;
	}else if (paras[1]=="WS_PONG_FRAME"){
		frame_type=WS_PONG_FRAME;
	}else if (paras[1]== "WS_CLOSING_FRAME"){
		frame_type=WS_CLOSING_FRAME;
	}else{
		func_return="";
		return "error set WS_FREAME TYPE !";
	};

	int frame_option;
	if (paras[2]== "WS_FINAL_FRAME" || paras[2]==""){
		frame_option=FINAL_FRAME;
	}else if (paras[2]== "WS_NEXT_FRAME"){
		frame_option=NEXT_FRAME;
	}else{
		func_return="";
		return "error set WS_FREAME OPTION !";
	};

	uint8_t *out_frame; 
    size_t out_frame_len;
    if (!(frame_type==WS_BINARY_FRAME || frame_type==WS_TEXT_FRAME) || paras[0].size()!=0){
		ws_make_frame((uint8_t *)paras[0].data(), paras[0].size(),&out_frame, &out_frame_len,frame_type, frame_option);
		func_return.append((const char*)out_frame,out_frame_len);
    }else{
    	func_return="";
    	return "WS_FRAME_ERROR: input value is null !";
    }
	return "";
}

string ParseExt_WEBSOCKET::FUNC(vector<string> &paras, string &func_return){
	if (paras[0]=="_ACCEPT_KEY"){
		paras.erase(paras.begin());
		return ParseExt_WEBSOCKET::_ACCEPT_KEY(paras,func_return);
	}else{
		return (string)"Error(PARSE FUNC):Func["+paras[0]+"]is not valid!";
	}
}

string ParseExt_WEBSOCKET::DECODE(const char *method, string &parse_buff, Http_ParseObj *pub_obj, string &to_obj){
	//初始化
	string field_value("");
	string name_str=to_obj;
	name_str+=".";
	//取得ws.type
	string field_name=name_str+"Type";
	if (ParseExt_WEBSOCKET::_FRAME_TYPE(parse_buff,field_value).size()==0)
		pub_obj->SetNodeValue(field_name.c_str(),field_value.c_str(),Http_ParseObj::STR_COPY_OBJ);

	//取得ws.payload
	field_name=name_str+"Payload";
	field_value="";
	if (ParseExt_WEBSOCKET::_FRAME_PAYLOAD(parse_buff,field_value).size()==0)
		pub_obj->SetNodeValue(field_name.c_str(),field_value.c_str(),Http_ParseObj::STR_COPY_OBJ);

	return "";

}

string ParseExt_WEBSOCKET::_FRAME_TYPE(string &frame_str,string &func_return){
	//new websocket v13:
	enum ws_frame_type type;
	type = ws_parse_input_frame((unsigned char *)frame_str.data(),frame_str.size());

	switch(type){
		case WS_TEXT_FRAME: func_return="WS_TEXT_FRAME";break;
		case WS_BINARY_FRAME: func_return="WS_BINARY_FRAME";break;
		case WS_PING_FRAME: func_return="WS_PING_FRAME";break;
		case WS_PONG_FRAME: func_return="WS_PONG_FRAME";break;
		case WS_CLOSING_FRAME: func_return="WS_CLOSING_FRAME";break;
		case WS_INCOMPLETE_FRAME: func_return="WS_INCOMPLETE_FRAME";break;
		case WS_ERROR_FRAME: func_return="WS_ERROR_FRAME";break;
		default:
			func_return="WS_ERROR_FRAME";
	}
	return "";
}

string ParseExt_WEBSOCKET::_FRAME_PAYLOAD(string &frame_str,string &func_return){
	uint64_t len=frame_str.size();
	unsigned char * r=extract_payload((unsigned char *)frame_str.data(), &len);
	if (r!=NULL){
		func_return.append((const char *)r,(size_t)len);
		return "";
	}else{
		func_return="";
		return "Can't extract payload.";
	}
}

string ParseExt_WEBSOCKET::_ACCEPT_KEY(vector<string> &paras, string &func_return){
    //new websocket v13:
	unsigned char sha[20]={0};
	string tmp_str=paras[0]+"258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

	//SHA1((const unsigned char *)tmp_str.c_str(),tmp_str.length(),sha);
	sha1((const unsigned char *)tmp_str.c_str(),tmp_str.length(),(unsigned char *)sha);
	string tmp_str3;
	string tmp_str2((const char *)sha,20);
	tmp_str3 = STRING_Helper::HBCStrToHBCStr(tmp_str2,LW_BINARY_STR,LW_HEX_STR,false);
	//parent->_PubPacket->BinaryToHBCStr((const char *)sha,20,tmp_str3,false);
	cout <<"sha1: "<< tmp_str3 << endl;
	func_return = base64_encode(sha,20);

	return "";
}
#endif
string ParseExt_MSHINET2::FUNC(vector<string> &paras, string &func_return){
	if (paras[0]=="_NLLL"){
		return (string)"Error(PARSE FUNC):Func["+paras[0]+"]is not valid!";
	}else if (paras[0]=="_BUILD_CRC"){
		paras.erase(paras.begin());
		return  ParseExt_MSHINET2::_BUILD_CRC(paras,func_return);
	}else{
		return (string)"Error(PARSE FUNC):Func["+paras[0]+"]is not valid!";
	}
}

string ParseExt_MSHINET2::DECODE(const char *method, string &parse_buff, Http_ParseObj *pub_obj, string &to_obj){//???应该加入substr长度安全判断，就是parse_buff太短，substr会出错。
	//解析操作
	tsHeaderParser shp = ParseHeader((BYTE *)parse_buff.data());
	//初始化
	string tmp_str;
	string field_value("");
	string name_str=to_obj;
	name_str+=".";
	string field_name;
	bool is_success=false;

	//设置成功
	/*if (shp.ContentLen==0)
		goto end;*/
	
	//取得STX
	field_name=name_str+"STX";
	field_value=STRING_Helper::iToStr(shp.HeaderStart,16);
	pub_obj->SetNodeValue(field_name.c_str(),field_value.c_str(),Http_ParseObj::STR_COPY_OBJ);
	//取得PacketType
	field_name=name_str+"PacketType";
	field_value=STRING_Helper::iToStr(shp.PacketType,16);
	pub_obj->SetNodeValue(field_name.c_str(),field_value.c_str(),Http_ParseObj::STR_COPY_OBJ);

	//取得PacketID
	field_name=name_str+"PacketID";
	tmp_str = STRING_Helper::iToStr(shp.TaskID,16);
	field_value = STRING_Helper::SetNumToID(tmp_str,4,0);
	pub_obj->SetNodeValue(field_name.c_str(),field_value.c_str(),Http_ParseObj::STR_COPY_OBJ);

	//取得DestID
	field_name=name_str+"DestID";
	if (shp.DestIDLen!=0 && parse_buff.size()>=shp.DestIDPos){
		tmp_str = parse_buff.substr(shp.DestIDPos,shp.DestIDLen);
		field_value=STRING_Helper::HBCStrToHBCStr(tmp_str,"char","hex",false);
		pub_obj->SetNodeValue(field_name.c_str(),field_value.c_str(),Http_ParseObj::STR_COPY_OBJ);
	}else{
		pub_obj->SetNodeValue(field_name.c_str(),"",Http_ParseObj::STR_COPY_OBJ);
	}

	//取得SourceID
	field_name=name_str+"SourceID";
	if (shp.SourceIDLen!=0 && parse_buff.size()>=shp.SourceIDPos){
		tmp_str = parse_buff.substr(shp.SourceIDPos,shp.SourceIDLen);
		field_value=STRING_Helper::HBCStrToHBCStr(tmp_str,"char","hex",false);
		pub_obj->SetNodeValue(field_name.c_str(),field_value.c_str(),Http_ParseObj::STR_COPY_OBJ);
	}else
		pub_obj->SetNodeValue(field_name.c_str(),"",Http_ParseObj::STR_COPY_OBJ);

	//取得TaskID
	field_name=name_str+"TaskID";
	if (shp.TaskIDLen!=0 && parse_buff.size()>=shp.TaskIDPos){
		tmp_str = parse_buff.substr(shp.TaskIDPos,shp.TaskIDLen);
		field_value=STRING_Helper::HBCStrToHBCStr(tmp_str,"char","hex",false);
		pub_obj->SetNodeValue(field_name.c_str(),field_value.c_str(),Http_ParseObj::STR_COPY_OBJ);
	}else
		pub_obj->SetNodeValue(field_name.c_str(),"0001",Http_ParseObj::STR_COPY_OBJ);

	//取得ContentLen
	field_name=name_str+"ContentLen"; 
	/*if (shp.ContentLen==0){
		cout << "Error: shp.ContentLen==0" << endl;
		goto end;
	}*/
	field_value = STRING_Helper::iToStr((int)shp.ContentLen);
	
	pub_obj->SetNodeValue(field_name.c_str(),field_value.c_str(),Http_ParseObj::STR_COPY_OBJ);

	//取得ContentLen
	field_name=name_str+"ContentStart"; 
	field_value = STRING_Helper::iToStr((int)shp.ContentStart);
	pub_obj->SetNodeValue(field_name.c_str(),field_value.c_str(),Http_ParseObj::STR_COPY_OBJ);

	//Get bExtContent
	field_name=name_str+"IsExtContent";
	if (shp.bExtContent)
		field_value = "true";
	else
		field_value = "false";
	pub_obj->SetNodeValue(field_name.c_str(),field_value.c_str(),Http_ParseObj::STR_COPY_OBJ);

	//Get Content
	field_name=name_str+"Content";
	if (shp.ContentLen!=0 && parse_buff.size()>=shp.ContentStart){
		field_value = parse_buff.substr(shp.ContentStart,(size_t)shp.ContentLen);//有字川为12但ContentStart=17的情况，会司机
		pub_obj->SetNodeValue(field_name.c_str(),field_value.data(),Http_ParseObj::STR_COPY_OBJ,field_value.size());
	}else
		pub_obj->SetNodeValue(field_name.c_str(),"",Http_ParseObj::STR_COPY_OBJ,field_value.size());
	
	//设置成功
	is_success=true;

	field_name=name_str+"IsSuccess";
	if (is_success==false){
		pub_obj->SetNodeValue(field_name.c_str(),"false",Http_ParseObj::STR_COPY_OBJ);
		return "ParseExt_MSHINET2::DECODE:ParseHeader Error-Content Len Is Empty!";
	}else{
		pub_obj->SetNodeValue(field_name.c_str(),"true",Http_ParseObj::STR_COPY_OBJ);
		return "";
	}
	
}

string ParseExt_MSHINET2::LEN(vector<string> &paras, string &func_return){
	if (paras.size()>=1){
		/*if (paras.size() == 3 && paras[2] == "Hex") {
			paras[0] = STRING_Helper::HBCStrToHBCStr(paras[0], "hex", "char", false);
		}*/
		if (paras[0].size() <2)
			return "frame is incomplate !";
		if (paras[0].data()[0]=='\0')
			return "frame is error !";

		if (paras.size()<2 || paras[1].size()==0){
			int len=(byte)paras[0].at(2);
			func_return=STRING_Helper::iToStr(len+2);
			return "";
		}else if (paras[1]=="EXT"){
			//取得头结构
			tsHeaderParser shp = ParseHeader((BYTE *)paras[0].data());
			func_return=STRING_Helper::iToStr((int)shp.PacketLen);
			return "";
		}else
			return "paras[1] must be EXT!";
	}else
		return "len func() paras should not be null !";
}

//ENCODE('@_MSHINET 参数：stx; packet_type; packet_flag(only 02); task_id, content_msg, 'true:长包;false:短包'
string ParseExt_MSHINET2::ENCODE(vector<string> &paras, string &func_return){
	tsHeaderBuilder shb;
	shb.HeaderStart=(BYTE)STRING_Helper::ParseInt(paras[0],16);
	shb.PacketType=(BYTE)STRING_Helper::ParseInt(paras[1],16);
	if (paras[0]=="02" && paras[2].size()!=0){//只有stx=02数据包才有PacketFlag
		shb.PacketFlags[0]=(BYTE)STRING_Helper::ParseInt(paras[2].substr(0,2),16);
		shb.PacketFlags[1]=(BYTE)STRING_Helper::ParseInt(paras[2].substr(2,2),16);
	}
	shb.SourceID=(uint64)STRING_Helper::ParseInt(paras[3],16);
	shb.DestID=(uint64)STRING_Helper::ParseInt(paras[4],16);
	shb.TaskID=(BYTE)STRING_Helper::ParseInt(paras[5],16);
	BYTE *pPacket=(BYTE *)malloc(paras[6].size()+256);
	BYTE *content=(BYTE *)malloc(paras[6].size()+10);
	memcpy((void *)content,(const void*)paras[6].data(),paras[6].size());
	size_t packet_len;
	shb.bExtContent=(paras[7]=="true" ? true : false);

    if (paras[0].size()!=0){
		//ParseExt_MSHINET2::BuildPacket(BYTE *pPacket, tsHeaderBuilder *shb, BYTE *content,uint64 contentlen)
		packet_len = BuildPacket(pPacket,&shb,content, (uint64)paras[6].size());
		func_return.append((const char*)pPacket,packet_len);
		free(pPacket);free(content);
    }else{
    	func_return="";
		free(pPacket);free(content);
    	return "WS_FRAME_ERROR: input value is null !";
    }
	return "";
} 

ParseExt_MSHINET2::tsHeaderParser ParseExt_MSHINET2::ParseHeader(BYTE *pPacket)
{
	tsHeaderParser shp;
	BYTE len = 0;
	// init all values
	shp.HeaderStart = pPacket[PCKT_STX];
	shp.PacketType = pPacket[PCKT_PACKTYPE];
	shp.ACKNeeded = shp.ACKRelated = shp.CRC = shp.bExtContent = FALSE;
	shp.BasicPacketLen = shp.PacketLen = 0;
	shp.DestID = shp.DestIDPos = shp.DestIDLen = 0;
	shp.SourceID = shp.SourceIDPos = shp.SourceIDLen = 0;
	shp.TaskID = shp.TaskIDPos = shp.TaskIDLen = 0;
	shp.ContentStart = shp.ContentLen = 0;
	shp.Pin = 0;
	shp.PacketFlags[0] = shp.PacketFlags[1] = 0;
	switch (pPacket[PCKT_STX])
	{
		case NW_PCKT_STX:
		{
			shp.PacketFlags[0] = pPacket[PCKT_PACKFLAG1];
			shp.PacketFlags[1] = pPacket[PCKT_PACKFLAG2];
			// if it's an ACK-related packet
			if (pPacket[PCKT_PACKFLAG1] & PACKFLAG1_ACKRELATED)
			{
				shp.ACKRelated = TRUE;
				shp.ACKNeeded = FALSE;
				shp.ContentLen = 2;	// fixed 2-byte ErrCode
				return (shp);
			}
			// if it's a normal packet
			else
			{
				shp.ACKRelated = FALSE;
				// whether ACK is needed
				if (pPacket[PCKT_PACKFLAG1] & PACKFLAG1_ACKNEEDED)
					shp.ACKNeeded = TRUE;
				else
					shp.ACKNeeded = FALSE;

				// free ASCII packet (reserved)
				if ((pPacket[PCKT_PACKTYPE] & 0xC0) == 0x00)
				{
					shp.DestIDLen = 0;
					shp.DestIDPos = 0;
					shp.SourceIDLen = 0;
					shp.SourceIDPos = 0;
					shp.TaskIDPos = 0;
					shp.TaskIDLen = 0;
					len = 5;
				}
				// normal CMD packet
				else if ((pPacket[PCKT_PACKTYPE] & 0xC0) == 0x40)
				{
					// init all default value
					shp.DestIDLen = 0;
					shp.DestIDPos = 0;
					shp.SourceIDLen = 0;
					shp.SourceIDPos = 0;
					shp.TaskIDPos = 0;
					shp.TaskIDLen = 0;
					len = 5;
					if ((pPacket[PCKT_PACKTYPE] & 0x30) == 0x00)
					{
						// just remains default
					}
					else if ((pPacket[PCKT_PACKTYPE] & 0x30) == 0x20)
					{
						// calculate SourceID length
						if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x00)
							shp.SourceIDLen = 1;
						else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x04)
							shp.SourceIDLen = 2;
						else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x08)
							shp.SourceIDLen = 4;
						else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x0C)
							shp.SourceIDLen = 8;

						shp.SourceIDPos = len;
						shp.DestIDLen = 0;
						shp.DestIDPos = 0;
						len += shp.SourceIDLen;
					}
					else if ((pPacket[PCKT_PACKTYPE] & 0x30) == 0x10)
					{
						// calculate DestID length
						if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x00)
							shp.DestIDLen = 1;
						else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x04)
							shp.DestIDLen = 2;
						else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x08)
							shp.DestIDLen = 4;
						else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x0C)
							shp.DestIDLen = 8;

						shp.DestIDPos = len;
						shp.SourceIDLen = 0;
						shp.SourceIDPos = 0;
						len += shp.DestIDLen;
					}
					else if ((pPacket[PCKT_PACKTYPE] & 0x30) == 0x30)
					{
						// calculate SourceID and DestID length
						if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x00)
							shp.SourceIDLen = shp.DestIDLen = 1;
						else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x04)
							shp.SourceIDLen = shp.DestIDLen = 2;
						else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x08)
							shp.SourceIDLen = shp.DestIDLen = 4;
						else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x0C)
							shp.SourceIDLen = shp.DestIDLen = 8;

						shp.SourceIDPos = len;
						shp.DestIDPos = len + shp.SourceIDLen;
						len += shp.SourceIDLen + shp.DestIDLen;
					}
				}
				// special CMD (range, bytestream) packet
				// Note here the SourceID and DestID will be different in meaning but the
				// position and length is still the same
				else if ((pPacket[PCKT_PACKTYPE] & 0xC0) == 0x80)
				{
					// init all default value
					shp.DestIDLen = 0;
					shp.DestIDPos = 0;
					shp.SourceIDLen = 0;
					shp.SourceIDPos = 0;
					shp.TaskIDPos = 0;
					shp.TaskIDLen = 0;
					len = 5;

					// calculate SourceID and DestID length
					if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x00)
						shp.SourceIDLen = shp.DestIDLen = 1;
					else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x04)
						shp.SourceIDLen = shp.DestIDLen = 2;
					else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x08)
						shp.SourceIDLen = shp.DestIDLen = 4;
					else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x0C)
						shp.SourceIDLen = shp.DestIDLen = 8;

					shp.SourceIDPos = len;
					shp.DestIDPos = len + shp.SourceIDLen;
					len += shp.SourceIDLen + shp.DestIDLen;
				}
				// report/event packet
				else if ((pPacket[PCKT_PACKTYPE] & 0xC0) == 0xC0)
				{
					// init all default value
					shp.DestIDLen = 0;
					shp.DestIDPos = 0;
					shp.SourceIDLen = 0;
					shp.SourceIDPos = 0;
					shp.TaskIDPos = 0;
					shp.TaskIDLen = 0;
					len = 5;

					// calculate SourceID and DestID length
					if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x00)
						shp.SourceIDLen = shp.DestIDLen = 1;
					else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x04)
						shp.SourceIDLen = shp.DestIDLen = 2;
					else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x08)
						shp.SourceIDLen = shp.DestIDLen = 4;
					else if ((pPacket[PCKT_PACKTYPE] & 0x0C) == 0x0C)
						shp.SourceIDLen = shp.DestIDLen = 8;

					shp.SourceIDPos = len;
					shp.DestIDPos = len + shp.SourceIDLen;
					len += shp.SourceIDLen + shp.DestIDLen;
				}

				// whether it has TaskID
				if (pPacket[PCKT_PACKTYPE] & 0x02)
				{
					shp.TaskIDPos = len;
					shp.TaskIDLen = 1;
					len += shp.TaskIDLen;
				}
			}
			break;
		}
		case NW_DATA_STX:
		{
			// if it's an ACK-related packet
			if (shp.PacketType & NW_DID_ACKRELATED)
			{
				shp.ACKRelated = TRUE;
				shp.ACKNeeded = FALSE;
				shp.ContentLen = 2;	// fixed 2-byte ErrCode
				return (shp);
			}
			// if it's a normal packet
			else
			{
				if (shp.PacketType & NW_DID_ACKNEEDED)
					shp.ACKNeeded = TRUE;
				else
					shp.ACKNeeded = FALSE;

				// init all default value
				shp.DestIDLen = 0;
				shp.DestIDPos = 0;
				shp.SourceIDLen = 0;
				shp.SourceIDPos = 0;
				shp.TaskIDPos = 0;
				shp.TaskIDLen = 0;
				len = 5;

				// init SourceID position
				shp.SourceIDPos = 3;
				// calculate length
				if ((pPacket[DATA_PACKTYPE] & 0x38) == 0x00)
					shp.DestIDLen = shp.SourceIDLen = 2;
				else if (((pPacket[DATA_PACKTYPE] & 0x38) == 0x08) ||
						((pPacket[DATA_PACKTYPE] & 0x38) == 0x10))
					shp.DestIDLen = shp.SourceIDLen = 4;
				// actually following will not be used in Jennet
				else if ((pPacket[DATA_PACKTYPE] & 0x38) == 0x18)
					shp.DestIDLen = shp.SourceIDLen = 6;
				else if ((pPacket[DATA_PACKTYPE] & 0x38) == 0x20)
					shp.DestIDLen = shp.SourceIDLen = 8;
				else if ((pPacket[DATA_PACKTYPE] & 0x38) == 0x28)
					shp.DestIDLen = shp.SourceIDLen = 16;
				else if ((pPacket[DATA_PACKTYPE] & 0x38) == 0x30)
					shp.DestIDLen = shp.SourceIDLen = 18;
				else if ((pPacket[DATA_PACKTYPE] & 0x38) == 0x38)
					shp.DestIDLen = shp.SourceIDLen = 32;
				// calculate DestID position and total header length
				shp.DestIDPos = shp.SourceIDPos + shp.SourceIDLen;
				len += shp.SourceIDLen + shp.DestIDLen;

				shp.TaskIDLen = 2;
				shp.TaskIDPos = len - shp.TaskIDLen;
			}
			break;
		}
	}

	// get DestID
	switch(shp.DestIDLen)
	{
		case 0:
			shp.DestID = 0; break;
		case 1:
			shp.DestID = pPacket[shp.DestIDPos]; break;
		case 2:
			shp.DestID = scanUINT(pPacket, shp.DestIDPos); break;
		case 4:
			shp.DestID = scanULONG(pPacket, shp.DestIDPos); break;
		case 8:
			shp.DestID = scanUINT64(pPacket, shp.DestIDPos); break;
	}
	// get SourceID
	switch(shp.SourceIDLen)
	{
		case 0:
			shp.SourceID = 0; break;
		case 1:
			shp.SourceID = pPacket[shp.SourceIDPos]; break;
		case 2:
			shp.SourceID = scanUINT(pPacket, shp.SourceIDPos); break;
		case 4:
			shp.SourceID = scanULONG(pPacket, shp.SourceIDPos); break;
		case 8:
			shp.SourceID = scanUINT64(pPacket, shp.SourceIDPos); break;
	}
	// get TaskID
	switch(shp.TaskIDLen)
	{
		case 0:
			shp.TaskID = 0; break;
		case 1:
			shp.TaskID = pPacket[shp.TaskIDPos]; break;
		case 2:
			shp.TaskID = scanUINT(pPacket, shp.TaskIDPos); break;
		case 4:
			shp.TaskID = scanULONG(pPacket, shp.TaskIDPos); break;
	}

	// check the last but 3 byte to see whether it has extended content
	// shp.BasicPacketLen is just a temp len variable here
	shp.BasicPacketLen = pPacket[PCKT_PACKLEN] - 3;
	// calculate content length
	if (pPacket[shp.BasicPacketLen] == 0)
	{
		// calculate in-basic-packet content length
		// additional 3 bytes are length (0x00) and CRC (2 bytes)
		shp.ContentLen = pPacket[PCKT_PACKLEN] - len - 3;
		// calculate packet total length
		// additional 2 bytes are 0x0D0A (2 bytes)
		shp.PacketLen = pPacket[PCKT_PACKLEN] + 2;
		// extended content is after TaskID field (or the last field exist)
		shp.ContentStart = len;
		shp.bExtContent = FALSE;
	}
	else
	{
		// extended content is after the CRC of header
		shp.ContentStart = shp.BasicPacketLen + 3;
		// calculate in-basic-packet content length
		if (pPacket[shp.BasicPacketLen] == 1)
			shp.ContentLen = pPacket[len] - 2;
		else if (pPacket[shp.BasicPacketLen] == 2)
			shp.ContentLen = scanUINT(pPacket, len) - 2;
		else if (pPacket[shp.BasicPacketLen] == 4)
			shp.ContentLen = scanULONG(pPacket, len) - 2;
		// calculate packet total length
		// additional 2 bytes are 0x0D0A (2 bytes) and missing 3 bytes from previous
		// calculation of "shp.BasicPacketLen = pPacket[PCKT_PACKLEN] - 3;"
		shp.PacketLen = shp.BasicPacketLen + shp.ContentLen + 7;
		shp.bExtContent = TRUE;
	}

	// assign the real shp.BasicPacketLen
	shp.BasicPacketLen = pPacket[PCKT_HDRLEN];

	// get PIN if there is one
	// only a STX packet has a PACKFLAG2 field and may have pin attached
	// actually the Pin is not counted in header but in content, but it's realated
	// to header and must be
	if (pPacket[PCKT_PACKFLAG2] & PACKFLAG2_PACKPINATTACHED)
		shp.Pin = scanULONG(pPacket, shp.ContentStart);

	return shp;
}

BYTE ParseExt_MSHINET2::BuildHeader(BYTE *pPacketHeader, tsHeaderBuilder *shb,uint64 contentlen)
{
	tsHeaderParser shp;

	// init PacketSTX and PacketType
	pPacketHeader[PCKT_STX] = shb->HeaderStart;
	pPacketHeader[PCKT_PACKTYPE] = shb->PacketType;

	// add PacketFlags if 0x02
	if (shb->HeaderStart == NW_PCKT_STX)
	{
		pPacketHeader[PCKT_PACKFLAG1] = shb->PacketFlags[0];
		pPacketHeader[PCKT_PACKFLAG2] = shb->PacketFlags[1];
	}

	BYTE len = GetPacketHeaderLen(shb->HeaderStart, shb->PacketType);
	if (shb->bExtContent)
	{
		contentlen -= 2;
		// 1 byte of extended content length
		if (contentlen <= 0xFF)
		{
			pPacketHeader[len + 1] = 1;
			pPacketHeader[len] = (BYTE)contentlen;
			// set basic packet length
			shp.BasicPacketLen = len + 1 + 3;
		}
		// 2 bytes of extended content length
		else if (contentlen <= 0xFFFF)
		{
			pPacketHeader[len + 2] = 2;
			putUINT16(pPacketHeader, len, (UINT)contentlen);
			// set basic packet length
			shp.BasicPacketLen = len + 2 + 3;
		}
		// 4 bytes of extended content length
		else if (contentlen <= 0xFFFFFFFF)
		{
			pPacketHeader[len + 4] = 4;
			putUINT32(pPacketHeader, len, (ULONG)contentlen);
			// set basic packet length
			shp.BasicPacketLen = len + 4 + 3;
		}
		shp.ContentStart = shp.BasicPacketLen;
	}
	else
	{
		pPacketHeader[len + contentlen] = 0;
		shp.ContentStart = len;
		// set basic packet length
		shp.BasicPacketLen = len + contentlen + 3;
	}
	pPacketHeader[PCKT_HDRLEN] = shp.BasicPacketLen;

	// after assign the above fields, we can have a valid tsHeaderParser
	shp = ParseHeader(pPacketHeader);

	// put SourceID, DestID and TaskID of corresponding length into position
	if (shp.SourceIDLen == 1)
		pPacketHeader[shp.SourceIDPos] = (BYTE)shb->SourceID;
	else if (shp.SourceIDLen == 2)
		putUINT16(pPacketHeader, shp.SourceIDPos, (UINT)shb->SourceID);
	else if (shp.SourceIDLen == 4)
		putUINT32(pPacketHeader, shp.SourceIDPos, (ULONG)shb->SourceID);
	else if (shp.SourceIDLen == 8)
		putUINT64(pPacketHeader, shp.SourceIDPos, shb->SourceID);

	if (shp.DestIDLen == 1)
		pPacketHeader[shp.DestIDPos] = (BYTE)shb->DestID;
	else if (shp.DestIDLen == 2)
		putUINT16(pPacketHeader, shp.DestIDPos, (UINT)shb->DestID);
	else if (shp.DestIDLen == 4)
		putUINT32(pPacketHeader, shp.DestIDPos, (ULONG)shb->DestID);
	else if (shp.DestIDLen == 8)
		putUINT64(pPacketHeader, shp.DestIDPos, shb->DestID);

	if (shp.TaskIDLen == 1)
		pPacketHeader[shp.TaskIDPos] = (BYTE)shb->TaskID;
	else if (shp.TaskIDLen == 2)
		putUINT16(pPacketHeader, shp.TaskIDPos, (UINT)shb->TaskID);
	else if (shp.TaskIDLen == 4)
		putUINT32(pPacketHeader, shp.TaskIDPos, (ULONG)shb->TaskID);

	// add CRC of basic packet if there is extended content
	if (shb->bExtContent)
	{
		// add CRC
		UINT CRC = u16CRC(pPacketHeader, pPacketHeader[PCKT_HDRLEN] - 2);
		putUINT16(pPacketHeader, pPacketHeader[PCKT_HDRLEN] - 2, CRC);
	}
	// add 0x0D0A if the content is inside basic packet, the CRC will be calculated later
	else
	{
		// add 0x0D0A
		putUINT16(pPacketHeader, pPacketHeader[PCKT_HDRLEN], 0x0D0A);
	}

	// return the actual content start point
	return (shp.ContentStart);

}

BYTE ParseExt_MSHINET2::GetPacketHeaderLen(BYTE STX, BYTE PID)
{
	if (STX == NW_PCKT_STX)
		return MessagePacketFixedPartLen(PID);
#ifndef ED_BUILD
	else if (STX == NW_DATA_STX)
		return DataPacketFixedPartLen(PID);
#endif
	else
		return 0;
}

BYTE ParseExt_MSHINET2::MessagePacketFixedPartLen(BYTE PID)
{
	BYTE PFixedLen = 0; 			// normally STX PID PLen Flag CRC1 CRC2 CR LF
	// calculate packet length
	if ((PID & 0xC0) == 0x00)
	{
		PFixedLen = 1;
	}
	else if ((PID & 0xC0) == 0x40)
	{
		if ((PID & 0x30) == 0x00)
		{
			PFixedLen = 5;
		}
		else if (((PID & 0x30) == 0x10) || ((PID & 0x30) == 0x20))
		{
			if ((PID & 0x0C) == 0x00)
				PFixedLen = 6;
			else if ((PID & 0x0C) == 0x04)
				PFixedLen = 7;
			else if ((PID & 0x0C) == 0x08)
				PFixedLen = 9;
			else if ((PID & 0x0C) == 0x0C)
				PFixedLen = 13;
		}
		else if ((PID & 0x30) == 0x30)
		{
			if ((PID & 0x0C) == 0x00)
				PFixedLen = 7;
			else if ((PID & 0x0C) == 0x04)
				PFixedLen = 9;
			else if ((PID & 0x0C) == 0x08)
				PFixedLen = 13;
			else if ((PID & 0x0C) == 0x0C)
				PFixedLen = 21;
		}
	}
	else if ((PID & 0xC0) == 0x80)
	{
		if ((PID & 0x0C) == 0x00)
			PFixedLen = 7;
		else if ((PID & 0x0C) == 0x04)
			PFixedLen = 9;
		else if ((PID & 0x0C) == 0x08)
			PFixedLen = 13;
		else if ((PID & 0x0C) == 0x0C)
			PFixedLen = 21;
	}
	else if ((PID & 0xC0) == 0xC0)
	{
		if ((PID & 0x0C) == 0x00)
			PFixedLen = 7;
		else if ((PID & 0x0C) == 0x04)
			PFixedLen = 9;
		else if ((PID & 0x0C) == 0x08)
			PFixedLen = 13;
		else if ((PID & 0x0C) == 0x0C)
			PFixedLen = 21;
	}

	if ((PID & 0x02) == 0x02)
		PFixedLen += 1;			

	return(PFixedLen);
}

BYTE ParseExt_MSHINET2::DataPacketFixedPartLen(BYTE PID)
{
	BYTE PFixedLen = 5; 			// normally STX PID PLen PacketID(2 bytes)

	if ((PID & 0x38) == 0x00)
		PFixedLen += 4;
	else if (((PID & 0x38) == 0x08) || ((PID & 0x38) == 0x10))
		PFixedLen += 8;
	// actually following will not be used in Jennet
	else if ((PID & 0x38) == 0x18)
		PFixedLen += 12;
	else if ((PID & 0x38) == 0x20)
		PFixedLen += 16;
	else if ((PID & 0x38) == 0x28)
		PFixedLen += 32;
	else if ((PID & 0x38) == 0x30)
		PFixedLen += 36;
	else if ((PID & 0x38) == 0x38)
		PFixedLen += 64;

	return(PFixedLen);

}

size_t ParseExt_MSHINET2::BuildPacket(BYTE *pPacket, tsHeaderBuilder *shb, BYTE *content,
		uint64 contentlen)
{
	if (shb->bExtContent)
	{
		// calculate real content length and add CRC+0D0A
		uint64 clen = BuildPacketExtContent(content, contentlen);
		//string tmp((char *)content,clen);
		BYTE basicpacketlen = BuildHeader(pPacket, shb, clen);
#ifdef PROTOCOL_DEBUG
		uaprintf("%d\r\n", basicpacketlen);
#endif
		// copy packet extended content into buffer
		xcpy(&pPacket[basicpacketlen], content, clen);
		return (basicpacketlen + clen);
	}
	else
	{
		BYTE contentstart = BuildHeader(pPacket, shb, contentlen);
		// copy packet content into buffer
#ifdef PROTOCOL_DEBUG
		uaprintf("%d\r\n", contentstart);
#endif
		xcpy(&pPacket[contentstart], content, contentlen);
		// add CRC
		UINT CRC = u16CRC(pPacket, pPacket[PCKT_HDRLEN] - 2);
		//string test1=""
		//cout << "test crc:" <<  << endl;
		putUINT16(pPacket, pPacket[PCKT_HDRLEN] - 2, CRC);
		return (pPacket[PCKT_PACKLEN] + 2);
	}

}

uint64 ParseExt_MSHINET2::BuildPacketExtContent(BYTE *pPacketContent, uint64 len)
{
	putUINT16(pPacketContent, len, u16CRC(pPacketContent, len));
	putUINT16(pPacketContent, len + 2, 0x0D0A);
	return (len + 4);
}

string ParseExt_MSHINET2::_BUILD_CRC(vector<string> &paras, string &func_return){
	//new websocket v13:
	//UINT=ParseExt_MSHINET2::u16CRC((BYTE *)paras[0].data(),paras[0].size())
	//string tmp=paras[0];
	UInt32 i=ParseExt_MSHINET2::u16CRC((BYTE *)(paras[0].data()),paras[0].size());
	string tmp=STRING_Helper::UInt32ToBinaryStr(i);
	func_return = STRING_Helper::HBCStrToHBCStr(tmp,"char","hex",true);

	return "";
}

UINT ParseExt_MSHINET2::u16CRC(BYTE *pData, size_t len)
{
	// calculate 2 CRC bytes
	size_t i, crc = 0;
	UINT CRC;
	for (i = 0; i < len; i ++)
		crc = CRC_TABLE[crc^pData[i]];
	CRC = (((0x30 + ((crc >> 4) & 0x0F)) << 8) & 0xFF00) + (0x30 + (crc & 0x0f));
	return CRC;

}

/*
//ly change 所有BYTE offset改为：size_t offset
UINT ParseExt_MSHINET2::scanUINT(BYTE *pBuf, size_t offset)
{
	return((((UINT)pBuf[offset]) << 8) + pBuf[offset+1]);
}

ULONG ParseExt_MSHINET2::scanULONG(BYTE *pBuf, size_t offset)
{
	pBuf += offset;

	return (((ULONG)pBuf[0]) << 24)+(((ULONG)pBuf[1] ) << 16)+(((ULONG)pBuf[2] ) << 8) + pBuf[3];
}

uint64 ParseExt_MSHINET2::scanUINT64(BYTE *pBuf, size_t offset)
{
	pBuf += offset;

	return (((uint64)pBuf[0]) << 56)+(((uint64)pBuf[1]) << 48)+(((uint64)pBuf[2]) << 40)+(((uint64)pBuf[3]) << 32)+
	(((uint64)pBuf[4]) << 24)+(((uint64)pBuf[5]) << 16)+(((uint64)pBuf[6]) << 8) + pBuf[7];

}


BYTE ParseExt_MSHINET2::putUINT16(BYTE *pBuf, size_t offset, ULONG u16value)
{
	pBuf[offset++] = (BYTE)(u16value >> 8);
	pBuf[offset] = (BYTE)(u16value & 0xff);
	return(2);
}


BYTE ParseExt_MSHINET2::putUINT32(BYTE *pBuf, size_t offset, ULONG u32value)
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


BYTE ParseExt_MSHINET2::putUINT64(BYTE *pBuf, size_t offset, uint64 u64value)
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


void ParseExt_MSHINET2::xcpy(void *dest, void *src, UINT nBytes)
{
	BYTE *p;
	BYTE *s;

	p = (BYTE *)dest;
	s = (BYTE *)src;
	while (nBytes--) *(p++) = *(s++);

}*/

string ParseExt_MSHINET::FUNC(vector<string> &paras, string &func_return) {
	if (paras[0] == "_NULL") {
		return (string)"Error(PARSE FUNC):Func[" + paras[0] + "]is not valid!";
	}
	else if (paras[0] == "_BUILD_CRC") {
		paras.erase(paras.begin());
		return  ParseExt_MSHINET::_BUILD_CRC(paras, func_return);
	}
	else if (paras[0] == "STREAM_DOWNLOAD") {
		paras.erase(paras.begin());
		return  ParseExt_MSHINET::STREAM_DOWNLOAD(paras, func_return);
	}
	else {
		return (string)"Error(PARSE FUNC):Func[" + paras[0] + "]is not valid!";
	}
}

string ParseExt_MSHINET::DECODE(const char *method, string &parse_buff, Http_ParseObj *pub_obj, string &to_obj) {
	//???应该加入substr长度安全判断，就是parse_buff太短，substr会出错。
	//解析操作
	tsHeaderParser shp = ParseHeader((BYTE *)parse_buff.data());
    if (shp.parse_err!=0){
        return (string)"parse packet("+STRING_Helper::HBCStrToHBCStr(parse_buff, "char", "hex",true)+") is err packet:"+STRING_Helper::iToStr(shp.parse_err)+"!";
    }
	//初始化
	string tmp_str;
	string field_value("");
	string name_str = to_obj;
	name_str += ".";
	string field_name;
	bool is_success = false;

	//设置成功
	/*if (shp.ContentLen==0)
	goto end;*/

	//取得STX
	field_name = name_str + "STX";
	field_value = STRING_Helper::iToStr(shp.HeaderStart, 16);
	pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
	//取得PacketType
	field_name = name_str + "PacketType";
	field_value = STRING_Helper::iToStr(shp.PacketType, 16);
	pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);

#ifdef MSHI3
    //取得CloudProcessID
    field_name = name_str + "CloudProcessID";
    if (shp.ProcessIDLen[MSHI_PID_CPIDINDEX] != 0) {
        string tmp=STRING_Helper::putUINTToStr(shp.CloudProcessID, shp.ProcessIDLen[MSHI_PID_CPIDINDEX]*8);
        field_value=STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", false);
        pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
    }
    else {
        pub_obj->SetNodeValue(field_name.c_str(), "", Http_ParseObj::STR_COPY_OBJ);
    }
    //取得ProcessID
    field_name = name_str + "ProcessID";
    if (shp.ProcessIDLen[MSHI_PID_PIDINDEX] != 0) {
        string tmp=STRING_Helper::putUINTToStr(shp.ProcessID, shp.ProcessIDLen[MSHI_PID_PIDINDEX]*8);
        field_value=STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", false);
        pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
    }
    else {
        pub_obj->SetNodeValue(field_name.c_str(), "", Http_ParseObj::STR_COPY_OBJ);
    }
    //取得CloudNextProcessID
    field_name = name_str + "CloudNextProcessID";
    if (shp.ProcessIDLen[MSHI_PID_CNPIDINDEX] != 0) {
        string tmp = STRING_Helper::putUINTToStr(shp.CloudNextProcessID, shp.ProcessIDLen[MSHI_PID_CNPIDINDEX] * 8);
        field_value = STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", false);
        pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
    }
    else {
        pub_obj->SetNodeValue(field_name.c_str(), "", Http_ParseObj::STR_COPY_OBJ);
    }

    //取得NextProcessID
    field_name = name_str + "NextProcessID";
    if (shp.ProcessIDLen[MSHI_PID_NPIDINDEX	] != 0) {
        string tmp = STRING_Helper::putUINTToStr(shp.NextProcessID, shp.ProcessIDLen[MSHI_PID_NPIDINDEX	] * 8);
        field_value = STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", false);
        pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
    }
    else {
        pub_obj->SetNodeValue(field_name.c_str(), "", Http_ParseObj::STR_COPY_OBJ);
    }

#else
	//取得ProcessID
	field_name = name_str + "ProcessID";
	if (shp.ProcessIDLen[0] != 0) {
		//STRING_Helper::putUINTToStr(field_value, shp.ProcessID);
		string tmp=STRING_Helper::putUINTToStr(shp.ProcessID, shp.ProcessIDLen[0]*8);
		field_value=STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", false);
		pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
	}
	else {
		pub_obj->SetNodeValue(field_name.c_str(), "", Http_ParseObj::STR_COPY_OBJ);
	}
	//取得ProcessID
	field_name = name_str + "NextProcessID";
	if (shp.ProcessIDLen[1] != 0) {
		//STRING_Helper::putUINTToStr(field_value, shp.NextProcessID);
		string tmp = STRING_Helper::putUINTToStr(shp.NextProcessID, shp.ProcessIDLen[1] * 8);
		field_value = STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", false);
		pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
	} 
	else {
		pub_obj->SetNodeValue(field_name.c_str(), "", Http_ParseObj::STR_COPY_OBJ);
	}
#endif

	//取得OtherID
	for (int i = 0; i < 4; i++) {
		field_name = name_str + "ID"+STRING_Helper::iToStr(i);
		if (shp.OtherIDLen[i] != 0) {
			string tmp = STRING_Helper::putUINTToStr(shp.OtherID[i], shp.OtherIDLen[i] * 8);
			string field_value_tmp = STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", false);
			field_value=STRING_Helper::SetNumToID(field_value_tmp, shp.OtherIDLen[i]*2,"0");
			pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
		}else {
			pub_obj->SetNodeValue(field_name.c_str(), "", Http_ParseObj::STR_COPY_OBJ);
		}
	}

	//取得PacketFlag
	field_name = name_str + "PacketNwkFlag";
	pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(shp.nwkflags.nwkflags,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
	field_name = name_str + "PacketFlag1";
	pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(shp.pcktflags1.pcktflags1,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
	field_name = name_str + "PacketFlag2";
	pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(shp.pcktflags2.pcktflags2,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
	field_name = name_str + "Pin";
	if (shp.PinLen==0)
		pub_obj->SetNodeValue(field_name.c_str(), "", Http_ParseObj::STR_COPY_OBJ);
	else {
		string pin_str = string((const char*)shp.Pin, shp.PinLen);
		field_value = STRING_Helper::HBCStrToHBCStr(pin_str, "char", "hex", false);
		pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
	}
	
	if (shp.ackflags2.pcktflags2 == 0) {
		field_name = name_str + "AckRelated";
		pub_obj->SetNodeValue(field_name.c_str(), "1", Http_ParseObj::STR_COPY_OBJ);
		field_name = name_str + "Ack";
		if (shp.pcktflags2.pcktflags2 & MSHI_PKF2_ACKNEEDED) 
			pub_obj->SetNodeValue(field_name.c_str(), "1", Http_ParseObj::STR_COPY_OBJ);
		else	
			pub_obj->SetNodeValue(field_name.c_str(), "0", Http_ParseObj::STR_COPY_OBJ);
	}	
	else {
		field_name = name_str + "AckRelated";
		pub_obj->SetNodeValue(field_name.c_str(), "0", Http_ParseObj::STR_COPY_OBJ);
		field_name = name_str + "Ack";
		if (shp.ackflags2.pcktflags2 & MSHI_PKF2_ACK)
			pub_obj->SetNodeValue(field_name.c_str(), "1", Http_ParseObj::STR_COPY_OBJ);
		else
			pub_obj->SetNodeValue(field_name.c_str(), "0", Http_ParseObj::STR_COPY_OBJ);
	}

	//取得ContentLen
	field_name = name_str + "ContentLen";
	field_value = STRING_Helper::iToStr((int)shp.ContentLen);
	pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);

	//取得ContentStart
	field_name = name_str + "ContentStart";
	field_value = STRING_Helper::iToStr((int)shp.ContentStart);
	pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);

	//取得ContentFlagsLen
	field_name = name_str + "ContentFlagsLen";
	field_value = STRING_Helper::iToStr((int)shp.ContentFlagsLen);
	pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);

	//取得ContentFlags
	field_name = name_str + "ContentFlags";
	if (shp.ContentFlagsLen != 0) {
		string tmp((char *)shp.ContentFlags, shp.ContentFlagsLen);
		field_value = STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", false);
		pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
	}else
		pub_obj->SetNodeValue(field_name.c_str(), "", Http_ParseObj::STR_COPY_OBJ);

	//Get Content
	field_name = name_str + "_Content";
	if (shp.ContentLen != 0 && parse_buff.size() >= shp.ContentStart) {
		field_value = parse_buff.substr(shp.ContentStart, (size_t)shp.ContentLen);//有字川为12但ContentStart=17的情况，会司机
		pub_obj->SetNodeValue(field_name.c_str(), field_value.data(), Http_ParseObj::STR_COPY_OBJ, field_value.size());
	}
	else
		pub_obj->SetNodeValue(field_name.c_str(), "", Http_ParseObj::STR_COPY_OBJ, 0);

	//设置成功
	is_success = true;

	field_name = name_str + "IsSuccess";
	if (is_success == false) {
		pub_obj->SetNodeValue(field_name.c_str(), "false", Http_ParseObj::STR_COPY_OBJ);
		return "ParseExt_MSHINET::DECODE:ParseHeader Error-Content Len Is Empty!";
	}
	else {
		pub_obj->SetNodeValue(field_name.c_str(), "true", Http_ParseObj::STR_COPY_OBJ);
		return "";
	}

}

string ParseExt_MSHINET::LEN(vector<string> &paras, string &func_return) {
	if (paras.size() >= 1) {
		if (paras[0].size() <2)
			return "frame is incomplate !";
		if (paras[0].data()[0] == '\0')
			return "frame is error !";
		if (paras.size()<2 || paras[1].size() == 0) {
			int len = (byte)paras[0].at(2);
			func_return = STRING_Helper::iToStr(len + 2);
			return "";
		}
		else if (paras[1] == "EXT") {
			BYTE pos = 0;
			UInt64 packet_len;
			BYTE *pPacket;
			string tmp("");
			if ((paras.size() > 2) && (paras[2] == "hex")){
				tmp = STRING_Helper::HBCStrToHBCStr(paras[0], "hex", "char", false);
				pPacket = (BYTE *)tmp.data();
			}else{
				pPacket = (BYTE *)paras[0].data();
			}
			packet_len=pPacket[MSHI_PCKT_PCKTLEN];
			pos += MSHI_PCKT_STARTLEN + MSHI_PCKT_PCKTLENLEN;
			if (packet_len <= MSHI_HDR_MAXPCKTLENLEN){// parse packet length if extra length field is used
				BYTE packetlenlen = packet_len;
				packet_len = scanUINT64(pPacket, pos);
				packet_len >>= 64 - packetlenlen * 8;
			}
			if (tmp.size()!=0)
				func_return = STRING_Helper::iToStr((int)packet_len * 2);
			else
				func_return = STRING_Helper::iToStr((int)packet_len);
			return "";
			/*if ((paras.size() > 2) && (paras[2] == "hex")) {
				//取得头结构
				string tmp = STRING_Helper::HBCStrToHBCStr(paras[0], "hex", "char", false);
				//tsHeaderParser shp = ParseHeader((BYTE *)tmp.data());
				//func_return = STRING_Helper::iToStr((int)shp.PacketLen*2);
				BYTE pos = 0;
				UInt64 packet_len;
				BYTE *pPacket = (BYTE *)tmp.data();
				packet_len=pPacket[MSHI_PCKT_PCKTLEN];
				pos += MSHI_PCKT_STARTLEN + MSHI_PCKT_PCKTLENLEN;
				if (packet_len <= MSHI_HDR_MAXPCKTLENLEN){// parse packet length if extra length field is used
					BYTE packetlenlen = packet_len;
					packet_len = scanUINT64(pPacket, pos);
					packet_len >>= 64 - packetlenlen * 8;
				}
				func_return = STRING_Helper::iToStr((int)packet_len * 2);
				return "";
			}
			else {
				//取得头结构
				tsHeaderParser shp = ParseHeader((BYTE *)paras[0].data());
				func_return = STRING_Helper::iToStr((int)shp.PacketLen);
				return "";
			}*/
			
		}
		else
			return "paras[1] must be EXT!";
	}
	else
		return "len func() paras should not be null !";
}

//ENCODE('@_MSHINET 参数：stx; packet_type; packet_flag(only 02); task_id, content_msg, 'true:长包;false:短包'
string ParseExt_MSHINET::ENCODE(vector<string> &paras, string &func_return) {
	func_return = "";
	if (paras[0].size() == 0) {
		return "ENCODE(): input value is null !";
	}
	if (paras.size() < 6)
		return "ENCODE(): Paras num should >=6 !";

	tsHeaderParser shb;
	//Head Set
	shb.HeaderStart = (BYTE)STRING_Helper::ParseInt(paras[0], 16);
	shb.PacketType = (BYTE)STRING_Helper::ParseInt(paras[1], 16);

	//Flag Set
	if (!STRING_Helper::CheckNum(paras[2],"hex"))
		return (string)"ParseExt_MSHINET::ENCODE(): paras[2] is not num!";
	if (paras[2].size() >= 2)
		shb.nwkflags.nwkflags = (BYTE)STRING_Helper::ParseInt(paras[2].substr(0, 2), 16);
	else
		shb.nwkflags.nwkflags = 0;
	if (paras[2].size() >= 4)
		shb.pcktflags1.pcktflags1 = (BYTE)STRING_Helper::ParseInt(paras[2].substr(2, 2), 16);
	else
		shb.pcktflags1.pcktflags1 = 0;

	shb.pcktflags2.pcktflags2 = MSHI_PKF2_NONACK;//这里加入if (bACKNeeded) shb.pcktflags2.pcktflags2 = MSHI_PKF2_NONACK | MSHI_PKF2_ACKNEEDED;

	shb.ackflags2.pcktflags2 = 0;
	if (paras[2].size() >= 6) {
		BYTE b = (BYTE)STRING_Helper::ParseInt(paras[2].substr(4, 2), 16);
		if (b & MSHI_PKF2_ACKRELATED) {
			shb.pcktflags2.pcktflags2 = 0;
			shb.ackflags2.pcktflags2 = b;
		}
		else {
			shb.pcktflags2.pcktflags2 = b;
			shb.ackflags2.pcktflags2 = 0;
		}
	}

#ifdef MSHI3
    //
    shb.ProcessIDLen[MSHI_PID_CPIDINDEX] = 0;
    shb.ProcessIDLen[MSHI_PID_PIDINDEX] = 0;
    shb.ProcessIDLen[MSHI_PID_CNPIDINDEX] = 0;
    shb.ProcessIDLen[MSHI_PID_NPIDINDEX] = 0;
    vector <string > process_ids;
    STRING_Helper::stringSplit(paras[3], process_ids, "|",false);
    if ((process_ids.size() >= MSHI_PID_CPIDINDEX + 1) && (process_ids[MSHI_PID_CPIDINDEX].size() != 0)) {
        if (!STRING_Helper::CheckNum(process_ids[MSHI_PID_CPIDINDEX],"hex"))
            return "ENCODE(): paras[3][MSHI_PID_CPIDINDEX 0]: " + process_ids[MSHI_PID_CPIDINDEX] + " is not hex num ! !";
        ParseExt_MSHINET::_ENCODE_IDS64(process_ids[MSHI_PID_CPIDINDEX], shb.ProcessIDLen[MSHI_PID_CPIDINDEX], shb.CloudProcessID);
    }
    if ((process_ids.size() >= MSHI_PID_PIDINDEX + 1) && (process_ids[MSHI_PID_PIDINDEX].size() != 0)) {
        if (ParseExt_MSHINET::_ENCODE_IDS(process_ids[MSHI_PID_PIDINDEX], shb.ProcessIDLen[MSHI_PID_PIDINDEX], shb.ProcessID) < 0)
            return "ENCODE(): paras[3][MSHI_PID_PIDINDEX 0]: " + process_ids[MSHI_PID_PIDINDEX] + " is not hex num ! !";
    }
    if ((process_ids.size() >= MSHI_PID_CNPIDINDEX + 1) && (process_ids[MSHI_PID_CNPIDINDEX].size() != 0)) {
//        if (ParseExt_MSHINET::_ENCODE_IDS(process_ids[MSHI_PID_CNPIDINDEX], shb.ProcessIDLen[MSHI_PID_CNPIDINDEX], shb.CloudNextProcessID) < 0)
//            return "ENCODE(): paras[3][MSHI_PID_CNPIDINDEX 1]: " + process_ids[MSHI_PID_CNPIDINDEX] + " is not hex num ! !";
        //ly table_sync
        ParseExt_MSHINET::_ENCODE_IDS64(process_ids[MSHI_PID_CNPIDINDEX], shb.ProcessIDLen[MSHI_PID_CNPIDINDEX], shb.CloudNextProcessID);
    }
    
    if ((process_ids.size() >= MSHI_PID_NPIDINDEX + 1) && (process_ids[MSHI_PID_NPIDINDEX].size() != 0)) {
        if (ParseExt_MSHINET::_ENCODE_IDS(process_ids[MSHI_PID_NPIDINDEX], shb.ProcessIDLen[MSHI_PID_NPIDINDEX], shb.NextProcessID) < 0)
            return "ENCODE(): paras[3][MSHI_PID_NPIDINDEX 1]: " + process_ids[MSHI_PID_NPIDINDEX] + " is not hex num ! !";
    }
#else
    //ProcesID/NextProcessID Set
	shb.ProcessIDLen[MSHI_PID_PIDINDEX] = 0;
	shb.ProcessIDLen[MSHI_PID_NPIDINDEX] = 0;
	vector <string > process_ids;
	STRING_Helper::stringSplit(paras[3].c_str(), process_ids, "|");
	if ((process_ids.size() >= MSHI_PID_PIDINDEX + 1) && (process_ids[MSHI_PID_PIDINDEX].size() != 0)) {
		if (ParseExt_MSHINET::_ENCODE_IDS(process_ids[0], shb.ProcessIDLen[MSHI_PID_PIDINDEX], shb.ProcessID) < 0)
			return "ENCODE(): paras[3][MSHI_PID_PIDINDEX 0]: " + process_ids[MSHI_PID_PIDINDEX] + " is not hex num ! !";
	}
	if ((process_ids.size() >= MSHI_PID_NPIDINDEX + 1) && (process_ids[MSHI_PID_NPIDINDEX].size() != 0)) {
		if (ParseExt_MSHINET::_ENCODE_IDS(process_ids[1], shb.ProcessIDLen[MSHI_PID_NPIDINDEX], shb.NextProcessID) < 0)
			return "ENCODE(): paras[3][MSHI_PID_NPIDINDEX 1]: " + process_ids[MSHI_PID_NPIDINDEX] + " is not hex num ! !";
	}
#endif

	//OtherID Set
	for (int i = 0; i < 4; i++)
		shb.OtherIDLen[i] = 0;		//初始化
	if (paras.size()>4 && paras[4].size() != 0) {
		vector <string > other_ids;
		STRING_Helper::stringSplit(paras[4].c_str(), other_ids, "|");
		for (unsigned int i = 0; i < other_ids.size() && i < 4; i++) {
			if (other_ids[i].size() != 0) {
				if (ParseExt_MSHINET::_ENCODE_IDS(other_ids[i], shb.OtherIDLen[i], shb.OtherID[i]) < 0) {
					ParseExt_MSHINET::_ENCODE_IDS(other_ids[i], shb.OtherIDLen[i], shb.OtherID[i]);
					return (string)"ENCODE(): paras[4]: " + STRING_Helper::iToStr(i) + " is not num!";
				}
					
			}
		}
	}
    shb.pcktflags1.flags.headercrc = 0;
    
    //ContentFlags
    if (paras.size() > 6 && paras[6].size() != 0) {
//        shb.ContentFlagsLen = paras[6].size()/2;
        string content_flags= STRING_Helper::HBCStrToHBCStr(paras[6], "hex", "char", false);
        shb.ContentFlagsLen = content_flags.size(); //ly table_dir
        xcpy(shb.ContentFlags, (BYTE *)content_flags.data(), shb.ContentFlagsLen);
    }
    else
        shb.ContentFlagsLen = 0;
    
	//PinSet
	if (paras.size() > 7 && paras[7].size() != 0) {
		shb.PinLen = paras[7].size()/2;
		string pin_str= STRING_Helper::HBCStrToHBCStr(paras[7], "hex", "char", false);
		xcpy(shb.Pin, (BYTE *)pin_str.data(), shb.PinLen);
	}
	else
        shb.PinLen = 0;
    
    //Content Set
    bool data_hex=true;
    if (paras.size() > 8 && paras[8].size() != 0 && paras[8]=="bin") {
        data_hex=false;
    }
    BYTE *pPacket;  size_t packet_len;
    if (data_hex){
        string content_str = STRING_Helper::HBCStrToHBCStr(paras[5], "hex", "char", false);
        shb.ContentLen = (uint64)content_str.size();
        pPacket = (BYTE *)malloc(content_str.size() + 256);
        packet_len = BuildPacket(pPacket, &shb, (BYTE *)content_str.data());
    }else{
        shb.ContentLen = (uint64)paras[5].size();
        pPacket = (BYTE *)malloc(paras[5].size() + 256);
        packet_len = BuildPacket(pPacket, &shb, (BYTE *)paras[5].data());
    }
    func_return.append((const char*)pPacket, packet_len);
    if (pPacket)
        free(pPacket);

	return "";
}

string ParseExt_MSHINET::STREAM_DOWNLOAD(vector<string> &paras, string &func_return) {
	func_return = "";
	if (paras[0].size() == 0) {
		return "ENCODE(): input value is null !";
	}
	if (paras.size() < 6)
		return "ENCODE(): Paras num should >=6 !";

	tsHeaderParser shb;
	//Head Set
	shb.HeaderStart = MSHI_PCKT_STX;
	shb.PacketType = MSHI_PCKTTYPE_UNICASTBS;
	shb.nwkflags.nwkflags = 0;
	shb.pcktflags1.pcktflags1 = 0;

	//Flag Set
	bool bACKNeeded = (paras[0] == "ACK");
	if (bACKNeeded)
		shb.pcktflags2.pcktflags2 = MSHI_PKF2_NONACK | MSHI_PKF2_ACKNEEDED;
	else
		shb.pcktflags2.pcktflags2 = MSHI_PKF2_NONACK;
	shb.ackflags2.pcktflags2 = 0;

	//ProcesID/NextProcessID Set
	if (bACKNeeded)
	{
		shb.ProcessIDLen[MSHI_PID_PIDINDEX] = 4;
		shb.ProcessID = MSHI_PID_BYTESTREAM;//shb.ProcessID = ACK_GetUniquePID(MSHI_PID_BYTESTREAM);
	}
	else
	{
		shb.ProcessIDLen[MSHI_PID_PIDINDEX] = 2;
		shb.ProcessID = MSHI_PID_BYTESTREAM;
	}
	shb.ProcessIDLen[MSHI_PID_NPIDINDEX] = 0;
	shb.NextProcessID = 0;
#ifdef MSHI3
    shb.ProcessIDLen[MSHI_PID_CPIDINDEX] = 0;
    shb.CloudProcessID = 0;
    shb.ProcessIDLen[MSHI_PID_CNPIDINDEX] = 0;;
    shb.CloudNextProcessID = 0;
#endif

	//OtherID Set
	shb.OtherIDLen[MSHI_OID_DESTIDINDEX] = 2;
	if (!STRING_Helper::CheckNum(paras[0]))
		return (string)"ParseExt_MSHINET::ENCODE(): paras[0]=DestNodeID is not num!";
	shb.OtherID[MSHI_OID_DESTIDINDEX] = STRING_Helper::ParseInt(paras[0]);//DestNodeID
	shb.OtherIDLen[MSHI_OID_SRCIDINDEX] = 2;
	if (!STRING_Helper::CheckNum(paras[1]))
		return (string)"ParseExt_MSHINET::ENCODE(): paras[1]=srcNodeID is not num!";
	shb.OtherID[MSHI_OID_SRCIDINDEX] = STRING_Helper::ParseInt(paras[1]);//Local_NodeTable.NodeID;
	shb.OtherIDLen[MSHI_OID_OID2INDEX] = 2;
	if (!STRING_Helper::CheckNum(paras[2]))
		return (string)"ParseExt_MSHINET::ENCODE(): paras[2]=TID is not num!";
	shb.OtherID[MSHI_OID_OID2INDEX] = STRING_Helper::ParseInt(paras[2]);//TID;
	shb.OtherIDLen[MSHI_OID_OID3INDEX] = 2;
	if (!STRING_Helper::CheckNum(paras[3]))
		return (string)"ParseExt_MSHINET::ENCODE(): paras[3]=address is not num!";
	shb.OtherID[MSHI_OID_OID3INDEX] = STRING_Helper::ParseInt(paras[3]);//address;

	//Content Set
	shb.pcktflags1.flags.headercrc = 0;
	shb.PinLen = 0;
	shb.ContentFlagsLen = 0;

	BYTE *pPacket = (BYTE *)malloc(paras[4].size() + 256);
	string temp = paras[4];
	temp += '\0';  //发送字符串必须补充后面的\0
	shb.ContentLen = (uint64)paras[4].size()+1;

	UINT packet_len = ParseExt_MSHINET::BuildPacket(pPacket, &shb, (BYTE *)(temp.data()));
	func_return.append((const char*)pPacket, packet_len);
	free(pPacket);

	return "";
}



int ParseExt_MSHINET::_ENCODE_IDS(string &src, BYTE &id_len, uint64 &id) {
	if (!STRING_Helper::CheckNum(src, LW_HEX_STR))
		return -1;
	string tmp_src=src;
	string head = src.substr(0,2);
	if (head == "0x" || head == "0X")
		tmp_src = src.substr(2);
	id_len = tmp_src.size() / 2;
    id = (uint64)STRING_Helper::ParseInt(tmp_src, 16);
	return 0;
}

void ParseExt_MSHINET::_ENCODE_IDS64(string &src, BYTE &id_len, uint64 &id) {
    string tmp_src=src;
    string head = src.substr(0,2);
    if (head == "0x" || head == "0X")
        tmp_src = src.substr(2);
    id_len = tmp_src.size() / 2;
    //id = (uint64)STRING_Helper::ParseInt(tmp_src, 16);
    string tmp_id=STRING_Helper::HBCStrToHBCStr(src, "hex", "char", false);
    id=STRING_Helper::scanUINT64((BYTE *)tmp_id.c_str(),0);
}

ParseExt_MSHINET::tsHeaderParser ParseExt_MSHINET::ParseHeader(BYTE *pPacket)
{
	tsHeaderParser shp;
	BYTE pos = 0, i;
    shp.parse_err=0;

	shp.HeaderStart = pPacket[MSHI_PCKT_START];
	shp.PacketLen = pPacket[MSHI_PCKT_PCKTLEN];
	pos += MSHI_PCKT_STARTLEN + MSHI_PCKT_PCKTLENLEN;

	// parse packet length if extra length field is used
	if (shp.PacketLen <= MSHI_HDR_MAXPCKTLENLEN)
	{
		BYTE packetlenlen = shp.PacketLen;
		shp.PacketLen = scanUINT64(pPacket, pos);
		shp.PacketLen >>= 64 - packetlenlen * 8;
		pos += packetlenlen;
	}
    
    if (pPacket[shp.PacketLen-1]!=0x0a && pPacket[shp.PacketLen-1]!=0x0d){
        //ly 差错处理
        shp.parse_err=1;
        return shp;
    }

	shp.PacketType = pPacket[pos];
	pos += MSHI_PCKT_PCKTTYPELEN;

	shp.nwkflags.nwkflags = pPacket[pos];
	shp.nwkflagspos = pos;
	pos += MSHI_PCKT_NWKFLAGSLEN;

	shp.pcktflags1.pcktflags1 = pPacket[pos];
	pos += MSHI_PCKT_PCKTFLAGS1LEN;

	// check to see the PacketFlags2 is ACK related or not
	if (pPacket[pos] & MSHI_PKF2_ACKRELATED)
	{
		shp.pcktflags2.pcktflags2 = 0;
		shp.ackflags2.pcktflags2 = pPacket[pos];
	}
	else
	{
		shp.pcktflags2.pcktflags2 = pPacket[pos];
		shp.ackflags2.pcktflags2 = 0;
	}
	pos += MSHI_PCKT_PCKTFLAGS2LEN;

	BYTE idflags = pPacket[pos];
	pos += MSHI_PCKT_PIDFLAGSLEN;
    
#ifdef MSHI3
    // extract Cloud ProcessID if there is one
    if ((idflags & MSHI_PIDF_CPIDLEN) == MSHI_PIDF_NOCPID)
    {
        shp.ProcessIDLen[MSHI_PID_CPIDINDEX] = 0;
        shp.CloudProcessID = 0;
    }
    else if ((idflags & MSHI_PIDF_CPIDLEN) == MSHI_PIDF_CPIDLEN2)
    {
        shp.ProcessIDLen[MSHI_PID_CPIDINDEX] = 2;
        shp.CloudProcessID = scanUINT(pPacket, pos);
    }
    else if ((idflags & MSHI_PIDF_CPIDLEN) == MSHI_PIDF_CPIDLEN4)
    {
        shp.ProcessIDLen[MSHI_PID_CPIDINDEX] = 4;
        shp.CloudProcessID = scanULONG(pPacket, pos);
    }
    else if ((idflags & MSHI_PIDF_CPIDLEN) == MSHI_PIDF_CPIDLEN8)
    {
        shp.ProcessIDLen[MSHI_PID_CPIDINDEX] = 8;
        shp.CloudProcessID = scanUINT64(pPacket, pos);
    }
    pos += shp.ProcessIDLen[MSHI_PID_CPIDINDEX];
#endif

	// extract ProcessID if there is one
	if ((idflags & MSHI_PIDF_PIDLEN) == MSHI_PIDF_NOPID)
	{
		shp.ProcessIDLen[MSHI_PID_PIDINDEX] = 0;
		shp.ProcessID = 0;
	}
	else if ((idflags & MSHI_PIDF_PIDLEN) == MSHI_PIDF_PIDLEN2)
	{
		shp.ProcessIDLen[MSHI_PID_PIDINDEX] = 2;
		shp.ProcessID = scanUINT(pPacket, pos);
	}
	else if ((idflags & MSHI_PIDF_PIDLEN) == MSHI_PIDF_PIDLEN4)
	{
		shp.ProcessIDLen[MSHI_PID_PIDINDEX] = 4;
		shp.ProcessID = scanULONG(pPacket, pos);
	}
	else if ((idflags & MSHI_PIDF_PIDLEN) == MSHI_PIDF_PIDLEN8)
	{
		shp.ProcessIDLen[MSHI_PID_PIDINDEX] = 8;
		shp.ProcessID = scanUINT64(pPacket, pos);
	}
	pos += shp.ProcessIDLen[MSHI_PID_PIDINDEX];
    
#ifdef MSHI3
    // extract Cloud NextProcessID if there is one
    if ((idflags & MSHI_PIDF_CNPIDLEN) == MSHI_PIDF_NOCNPID)
    {
        shp.ProcessIDLen[MSHI_PID_CNPIDINDEX] = 0;
        shp.CloudNextProcessID = 0;
    }
    else if ((idflags & MSHI_PIDF_CNPIDLEN) == MSHI_PIDF_CNPIDLEN2)
    {
        shp.ProcessIDLen[MSHI_PID_CNPIDINDEX] = 2;
        shp.CloudNextProcessID = scanUINT(pPacket, pos);
    }
    else if ((idflags & MSHI_PIDF_CNPIDLEN) == MSHI_PIDF_CNPIDLEN4)
    {
        shp.ProcessIDLen[MSHI_PID_CNPIDINDEX] = 4;
        shp.CloudNextProcessID = scanULONG(pPacket, pos);
    }
    else if ((idflags & MSHI_PIDF_CNPIDLEN) == MSHI_PIDF_CNPIDLEN8)
    {
        shp.ProcessIDLen[MSHI_PID_CNPIDINDEX] = 8;
        shp.CloudNextProcessID = scanUINT64(pPacket, pos);
    }
    pos += shp.ProcessIDLen[MSHI_PID_CNPIDINDEX];
#endif

	// extract NextProcessID if there is one
	if ((idflags & MSHI_PIDF_NPIDLEN) == MSHI_PIDF_NONPID)
	{
		shp.ProcessIDLen[MSHI_PID_NPIDINDEX] = 0;
		shp.NextProcessID = 0;
	}
	else if ((idflags & MSHI_PIDF_NPIDLEN) == MSHI_PIDF_NPIDLEN2)
	{
		shp.ProcessIDLen[MSHI_PID_NPIDINDEX] = 2;
		shp.NextProcessID = scanUINT(pPacket, pos);
	}
	else if ((idflags & MSHI_PIDF_NPIDLEN) == MSHI_PIDF_NPIDLEN4)
	{
		shp.ProcessIDLen[MSHI_PID_NPIDINDEX] = 4;
		shp.NextProcessID = scanULONG(pPacket, pos);
	}
	else if ((idflags & MSHI_PIDF_NPIDLEN) == MSHI_PIDF_NPIDLEN8)
	{
		shp.ProcessIDLen[MSHI_PID_NPIDINDEX] = 8;
		shp.NextProcessID = scanUINT64(pPacket, pos);
	}
	pos += shp.ProcessIDLen[MSHI_PID_NPIDINDEX];

	idflags = pPacket[pos];
	pos += MSHI_PCKT_OIDFLAGSLEN;

	// extract OtherIDs if there is at least one
	if (idflags)
	{
		for (i = 0; i < MSHI_PCKT_HDRMAXOID; i++)
		{
			if ((idflags & (MSHI_OIDF_OIDLEN >> (2 * i))) == (MSHI_OIDF_NOOID >> (2 * i)))
			{
				shp.OtherID[i] = 0;
				shp.OtherIDLen[i] = 0;
			}
			else if ((idflags & (MSHI_OIDF_OIDLEN >> (2 * i))) == (MSHI_OIDF_OIDLEN2 >> (2 * i)))
			{
				shp.OtherID[i] = scanUINT(pPacket, pos);
				shp.OtherIDLen[i] = 2;
			}
			else if ((idflags & (MSHI_OIDF_OIDLEN >> (2 * i))) == (MSHI_OIDF_OIDLEN4 >> (2 * i)))
			{
				shp.OtherID[i] = scanULONG(pPacket, pos);
				shp.OtherIDLen[i] = 4;
			}
			else if ((idflags & (MSHI_OIDF_OIDLEN >> (2 * i))) == (MSHI_OIDF_OIDLEN8 >> (2 * i)))
			{
				shp.OtherID[i] = scanUINT64(pPacket, pos);
				shp.OtherIDLen[i] = 8;
			}

			// note DestID pos
			if (i == MSHI_OID_DESTIDINDEX)
				shp.DestIDStart = pos;

			pos += shp.OtherIDLen[i];
		}
	}
	else
		for (i = 0; i < MSHI_PCKT_HDRMAXOID; i++)
			shp.OtherIDLen[i] = 0;

	if (shp.pcktflags1.flags.headercrc)
		pos += MSHI_PCKT_HDRCRCLEN;

	shp.HeaderLen = pos - MSHI_PCKT_STARTLEN;

	if (shp.pcktflags1.flags.pin == MSHI_PKF1_LEN0)
		shp.PinLen = 0;
	else if (shp.pcktflags1.flags.pin == MSHI_PKF1_LEN2)
		shp.PinLen = 2;
	else if (shp.pcktflags1.flags.pin == MSHI_PKF1_LEN4)
		shp.PinLen = 4;
	else if (shp.pcktflags1.flags.pin == MSHI_PKF1_LENBUF)
	{
		shp.PinLen = pPacket[pos] & 0x3F;
		pos += 1;
	}
    if (shp.PinLen > shp.PacketLen-pos){
        //ly 差错处理
        shp.parse_err=5;
        return shp;
    }
	xcpy(shp.Pin, &pPacket[pos], shp.PinLen);
	pos += shp.PinLen;

	if (shp.pcktflags1.flags.contentflags == MSHI_PKF1_LEN0)
		shp.ContentFlagsLen = 0;
	else if (shp.pcktflags1.flags.contentflags == MSHI_PKF1_LEN2)
		shp.ContentFlagsLen = 2;
	else if (shp.pcktflags1.flags.contentflags == MSHI_PKF1_LEN4)
		shp.ContentFlagsLen = 4;
	else if (shp.pcktflags1.flags.contentflags == MSHI_PKF1_LENBUF)
	{
		shp.ContentFlagsLen = pPacket[pos] & 0x3F;
		pos += 1;
	}
    if (shp.ContentFlagsLen > shp.PacketLen-pos){
        //ly 差错处理
        shp.parse_err=6;
        return shp;
    }
        
	xcpy(shp.ContentFlags, &pPacket[pos], shp.ContentFlagsLen);
	pos += shp.ContentFlagsLen;

	shp.ContentStart = pos;
	shp.ContentLen = shp.PacketLen - pos - MSHI_PCKT_CRCLEN - MSHI_PCKT_0D0ALEN;

	// this will be added later
	shp.SrcAddr = 0;

	return shp;
}
UINT ParseExt_MSHINET::BuildPacket(BYTE *pPacket, tsHeaderParser *shp, BYTE *content)
{
	// calculate the whole packet length first
	BYTE i;
    
#ifdef MSHI3
    // initial with some fixed length in header
    shp->PacketLen = MSHI_PCKT_HDRFIXEDLEN;
    for (i = 0; i < MSHI_PCKT_HDRMAXPID; i ++)
        shp->PacketLen += shp->ProcessIDLen[i];
#else
    //initial with some fixed length in header
	shp->PacketLen = MSHI_PCKT_HDRFIXEDLEN + shp->ProcessIDLen[MSHI_PID_PIDINDEX] + shp->ProcessIDLen[MSHI_PID_NPIDINDEX];
#endif

	for (i = 0; i < MSHI_PCKT_HDRMAXOID; i++)
		shp->PacketLen += shp->OtherIDLen[i];

	if (shp->pcktflags1.flags.headercrc)
		shp->PacketLen += MSHI_PCKT_HDRCRCLEN;

	// add content length
	if (shp->PinLen)
	{
		if ((shp->PinLen != 4) && (shp->PinLen != 8))
			shp->PacketLen += 1;
		shp->PacketLen += shp->PinLen;
	}
	if (shp->ContentFlagsLen)
	{
		if ((shp->ContentFlagsLen != 2) && (shp->ContentFlagsLen != 4))
			shp->PacketLen += 1;
		shp->PacketLen += shp->ContentFlagsLen;
	}

	shp->PacketLen += shp->ContentLen + MSHI_PCKT_CRCLEN + MSHI_PCKT_0D0ALEN;

	// add packet length length if needed
	if ((shp->PacketLen > 0xFF) && (shp->PacketLen <= 0xFFFF - MSHI_HDR_PCKTLENLEN2))
		shp->PacketLen += MSHI_HDR_PCKTLENLEN2;
	else if ((shp->PacketLen > 0xFFFF - MSHI_HDR_PCKTLENLEN2) &&
		(shp->PacketLen <= 0xFFFFFFFF - MSHI_HDR_PCKTLENLEN4))
		shp->PacketLen += MSHI_HDR_PCKTLENLEN4;
	else if ((shp->PacketLen > 0xFFFFFFFF - MSHI_HDR_PCKTLENLEN4) &&
		(shp->PacketLen <= 0xFFFFFFFFFFFFFFFF - MSHI_HDR_PCKTLENLEN8))
		shp->PacketLen += MSHI_HDR_PCKTLENLEN8;

	// copy packet content
	xcpy(&pPacket[BuildHeader(pPacket, shp)], content, shp->ContentLen);

	// Calculate CRC and add 0x0D0A
	putUINT(pPacket, shp->PacketLen - MSHI_PCKT_CRCLEN - MSHI_PCKT_0D0ALEN,
		u16CRC(&pPacket[MSHI_PCKT_STARTLEN], shp->PacketLen - MSHI_PCKT_CRCLEN - MSHI_PCKT_0D0ALEN -
			MSHI_PCKT_STARTLEN));
	putUINT(pPacket, shp->PacketLen - MSHI_PCKT_0D0ALEN, MSHI_PCKT_0D0A);

	return shp->PacketLen;
}

BYTE ParseExt_MSHINET::BuildHeader(BYTE *pPacket, tsHeaderParser *shp)
{
	BYTE pos = 0, i, temppos, pktflag1pos;

	pPacket[MSHI_PCKT_START] = shp->HeaderStart;

	if (shp->PacketLen <= 0xFF)
		pPacket[MSHI_PCKT_PCKTLEN] = shp->PacketLen;
	else if (shp->PacketLen <= 0xFFFF)
	{
		pPacket[MSHI_PCKT_PCKTLEN] = MSHI_HDR_PCKTLENLEN2;
		putUINT(pPacket, MSHI_PCKT_STARTLEN + MSHI_PCKT_PCKTLENLEN, shp->PacketLen);
		pos += MSHI_HDR_PCKTLENLEN2;
	}
	// normally, the following won't happen
	else if (shp->PacketLen <= 0xFFFFFFFF)
	{
		pPacket[MSHI_PCKT_PCKTLEN] = MSHI_HDR_PCKTLENLEN4;
		putULONG(pPacket, MSHI_PCKT_STARTLEN + MSHI_PCKT_PCKTLENLEN, shp->PacketLen);
		pos += MSHI_HDR_PCKTLENLEN4;
	}
	else if (shp->PacketLen <= 0xFFFFFFFFFFFFFFFF)
	{
		pPacket[MSHI_PCKT_PCKTLEN] = MSHI_HDR_PCKTLENLEN8;
		putUINT64(pPacket, MSHI_PCKT_STARTLEN + MSHI_PCKT_PCKTLENLEN, shp->PacketLen);
		pos += MSHI_HDR_PCKTLENLEN8;
	}

	pos += MSHI_PCKT_STARTLEN + MSHI_PCKT_PCKTLENLEN;

	pPacket[pos] = shp->PacketType;
	pos += MSHI_PCKT_PCKTTYPELEN;

	pPacket[pos] = shp->nwkflags.nwkflags;
	pos += MSHI_PCKT_NWKFLAGSLEN;

	pPacket[pos] = shp->pcktflags1.pcktflags1;
	pktflag1pos = pos;	// used later in this function
	pos += MSHI_PCKT_PCKTFLAGS1LEN;

	// check to see the PacketFlags2 is ACK related or not
	if (shp->ackflags2.pcktflags2)
		pPacket[pos] = shp->ackflags2.pcktflags2;
	else
		pPacket[pos] = shp->pcktflags2.pcktflags2;
	pos += MSHI_PCKT_PCKTFLAGS2LEN;
    
#ifdef MSHI3
    // put Cloud ProcessID if any
    temppos = pos;
    pPacket[temppos] = 0;
    pos += MSHI_PCKT_PIDFLAGSLEN;
    if (shp->ProcessIDLen[MSHI_PID_CPIDINDEX])
    {
        if (shp->ProcessIDLen[MSHI_PID_CPIDINDEX] == 2)
        {
            pPacket[temppos] |= MSHI_PIDF_CPIDLEN2;
            putUINT(pPacket, pos, shp->CloudProcessID);
        }
        else if (shp->ProcessIDLen[MSHI_PID_CPIDINDEX] == 4)
        {
            pPacket[temppos] |= MSHI_PIDF_CPIDLEN4;
            putULONG(pPacket, pos, shp->CloudProcessID);
        }
        else if (shp->ProcessIDLen[MSHI_PID_CPIDINDEX] == 8)
        {
            pPacket[temppos] |= MSHI_PIDF_CPIDLEN8;
            putUINT64(pPacket, pos, shp->CloudProcessID);
        }
        pos += shp->ProcessIDLen[MSHI_PID_CPIDINDEX];
    }
    // put ProcessID if any
    if (shp->ProcessIDLen[MSHI_PID_PIDINDEX])
    {
        if (shp->ProcessIDLen[MSHI_PID_PIDINDEX] == 2)
        {
            pPacket[temppos] |= MSHI_PIDF_PIDLEN2;
            putUINT(pPacket, pos, shp->ProcessID);
        }
        else if (shp->ProcessIDLen[MSHI_PID_PIDINDEX] == 4)
        {
            pPacket[temppos] |= MSHI_PIDF_PIDLEN4;
            putULONG(pPacket, pos, shp->ProcessID);
        }
        else if (shp->ProcessIDLen[MSHI_PID_PIDINDEX] == 8)
        {
            pPacket[temppos] |= MSHI_PIDF_PIDLEN8;
            putUINT64(pPacket, pos, shp->ProcessID);
        }
        pos += shp->ProcessIDLen[MSHI_PID_PIDINDEX];
    }
    // put Cloud NextProcessID if any
    if (shp->ProcessIDLen[MSHI_PID_CNPIDINDEX])
    {
        if (shp->ProcessIDLen[MSHI_PID_CNPIDINDEX] == 2)
        {
            pPacket[temppos] |= MSHI_PIDF_CNPIDLEN2;
            putUINT(pPacket, pos, shp->CloudNextProcessID);
        }
        else if (shp->ProcessIDLen[MSHI_PID_CNPIDINDEX] == 4)
        {
            pPacket[temppos] |= MSHI_PIDF_CNPIDLEN4;
            putULONG(pPacket, pos, shp->CloudNextProcessID);
        }
        else if (shp->ProcessIDLen[MSHI_PID_CNPIDINDEX] == 8)
        {
            pPacket[temppos] |= MSHI_PIDF_CNPIDLEN8;
            putUINT64(pPacket, pos, shp->CloudNextProcessID);
        }
        pos += shp->ProcessIDLen[MSHI_PID_CNPIDINDEX];
    }
    // put NextProcessID if any
    if (shp->ProcessIDLen[MSHI_PID_NPIDINDEX])
    {
        if (shp->ProcessIDLen[MSHI_PID_NPIDINDEX] == 2)
        {
            pPacket[temppos] |= MSHI_PIDF_NPIDLEN2;
            putUINT(pPacket, pos, shp->NextProcessID);
        }
        else if (shp->ProcessIDLen[MSHI_PID_NPIDINDEX] == 4)
        {
            pPacket[temppos] |= MSHI_PIDF_NPIDLEN4;
            putULONG(pPacket, pos, shp->NextProcessID);
        }
        else if (shp->ProcessIDLen[MSHI_PID_NPIDINDEX] == 8)
        {
            pPacket[temppos] |= MSHI_PIDF_NPIDLEN8;
            putUINT64(pPacket, pos, shp->NextProcessID);
        }
        pos += shp->ProcessIDLen[MSHI_PID_NPIDINDEX];
    }
#else
	// put ProcessID if any
	pPacket[pos] = 0;  //mshinet3 ???
	if (shp->ProcessIDLen[MSHI_PID_PIDINDEX])
	{
		if (shp->ProcessIDLen[MSHI_PID_PIDINDEX] == 2)
		{
			pPacket[pos] |= MSHI_PIDF_PIDLEN2;
			putUINT(pPacket, pos + MSHI_PCKT_PIDFLAGSLEN, shp->ProcessID);
		}
		else if (shp->ProcessIDLen[MSHI_PID_PIDINDEX] == 4)
		{
			pPacket[pos] |= MSHI_PIDF_PIDLEN4;
			putULONG(pPacket, pos + MSHI_PCKT_PIDFLAGSLEN, shp->ProcessID);
		}
		else if (shp->ProcessIDLen[MSHI_PID_PIDINDEX] == 8)
		{
			pPacket[pos] |= MSHI_PIDF_PIDLEN8;
			putUINT64(pPacket, pos + MSHI_PCKT_PIDFLAGSLEN, shp->ProcessID);
		}
	}

	// put NextProcessID if any
	if (shp->ProcessIDLen[MSHI_PID_NPIDINDEX])
	{
		if (shp->ProcessIDLen[MSHI_PID_NPIDINDEX] == 2)
		{
			pPacket[pos] |= MSHI_PIDF_NPIDLEN2;
			putUINT(pPacket, pos + shp->ProcessIDLen[MSHI_PID_PIDINDEX] + MSHI_PCKT_PIDFLAGSLEN,
				shp->NextProcessID);
		}
		else if (shp->ProcessIDLen[MSHI_PID_NPIDINDEX] == 4)
		{
			pPacket[pos] |= MSHI_PIDF_NPIDLEN4;
			putULONG(pPacket, pos + shp->ProcessIDLen[MSHI_PID_PIDINDEX] + MSHI_PCKT_PIDFLAGSLEN,
				shp->NextProcessID);
		}
		else if (shp->ProcessIDLen[MSHI_PID_NPIDINDEX] == 8)
		{
			pPacket[pos] |= MSHI_PIDF_NPIDLEN8;
			putUINT64(pPacket, pos + shp->ProcessIDLen[MSHI_PID_PIDINDEX] + MSHI_PCKT_PIDFLAGSLEN,
				shp->NextProcessID);
		}
	}
	pos += shp->ProcessIDLen[MSHI_PID_PIDINDEX] + shp->ProcessIDLen[MSHI_PID_NPIDINDEX] +
		MSHI_PCKT_PIDFLAGSLEN;
#endif

	// put OtherID if any
	pPacket[pos] = 0;
	for (i = 0, temppos = MSHI_PCKT_OIDFLAGSLEN; i < MSHI_PCKT_HDRMAXOID; i++)
	{
		if (shp->OtherIDLen[i])
		{
			if (shp->OtherIDLen[i] == 2)
			{
				pPacket[pos] |= MSHI_OIDF_OIDLEN2 >> i * 2;
				putUINT(pPacket, pos + temppos, shp->OtherID[i]);
			}
			else if (shp->OtherIDLen[i] == 4)
			{
				pPacket[pos] |= MSHI_OIDF_OIDLEN4 >> i * 2;
				putULONG(pPacket, pos + temppos, shp->OtherID[i]);
			}
			else if (shp->OtherIDLen[i] == 8)
			{
				pPacket[pos] |= MSHI_OIDF_OIDLEN8 >> i * 2;
				putUINT64(pPacket, pos + temppos, shp->OtherID[i]);
			}
			temppos += shp->OtherIDLen[i];
		}
	}
	pos += temppos;

	if (shp->pcktflags1.flags.headercrc)
	{
		putUINT(pPacket, pos, u16CRC(&pPacket[MSHI_PCKT_PCKTLEN], pos - MSHI_PCKT_STARTLEN));
		pos += MSHI_PCKT_HDRCRCLEN;
	}

	shp->HeaderLen = pos - MSHI_PCKT_STARTLEN;

	if (!shp->PinLen)
		pPacket[pktflag1pos] |= MSHI_PKF1_NOPING;
	else if (shp->PinLen == 2)
		pPacket[pktflag1pos] |= MSHI_PKF1_PINGLEN2;
	else if (shp->PinLen == 4)
		pPacket[pktflag1pos] |= MSHI_PKF1_PINGLEN4;
	else
	{
		pPacket[pktflag1pos] |= MSHI_PKF1_PINGBUF;
		pPacket[pos] = shp->PinLen | 0x40;
		pos += 1;
	}
	xcpy(&pPacket[pos], shp->Pin, shp->PinLen);
	pos += shp->PinLen;

	if (!shp->ContentFlagsLen)
		pPacket[pktflag1pos] |= MSHI_PKF1_NOCNTF;
	else if (shp->ContentFlagsLen == 2)
		pPacket[pktflag1pos] |= MSHI_PKF1_CNTFLEN2;
	else if (shp->ContentFlagsLen == 4)
		pPacket[pktflag1pos] |= MSHI_PKF1_CNTFLEN4;
	else
	{
		pPacket[pktflag1pos] |= MSHI_PKF1_CNTFBUF;
		pPacket[pos] = shp->ContentFlagsLen | 0x40;
		pos += 1;
	}
	xcpy(&pPacket[pos], shp->ContentFlags, shp->ContentFlagsLen);
	pos += shp->ContentFlagsLen;

	return pos;
}
/*=============================================================================
 ; Function:	DuplicateHeader(*destshp, *srcshp)
 ; Abstract:	Duplicate a header
 ; Returns:	None
 ; Inputs:	*destshp - dest structure
 ;			*srcshp - source structure
 ==============================================================================*/

void ParseExt_MSHINET::DuplicateHeader(tsHeaderParser *destshp, tsHeaderParser *srcshp)
{
    BYTE i;
    destshp->HeaderStart = srcshp->HeaderStart;
    destshp->PacketLen = srcshp->PacketLen;
    destshp->PacketType = srcshp->PacketType;
    destshp->nwkflags.nwkflags = srcshp->nwkflags.nwkflags;
    destshp->nwkflagspos = srcshp->nwkflagspos;
    destshp->pcktflags1.pcktflags1 = srcshp->pcktflags1.pcktflags1;
    destshp->pcktflags2.pcktflags2 = srcshp->pcktflags2.pcktflags2;
    destshp->ackflags2.pcktflags2 = srcshp->ackflags2.pcktflags2;
    for (i = 0; i < MSHI_PID_NUMPID; i ++)
        destshp->ProcessIDLen[i] = srcshp->ProcessIDLen[i];
    destshp->CloudProcessID = srcshp->CloudProcessID;
    destshp->ProcessID = srcshp->ProcessID;
    destshp->CloudNextProcessID = srcshp->CloudNextProcessID;
    destshp->NextProcessID = srcshp->NextProcessID;
    for (i = 0; i < MSHI_OID_NUMOID; i ++)
    {
        destshp->OtherIDLen[i] = srcshp->OtherIDLen[i];
        destshp->OtherID[i] = srcshp->OtherID[i];
    }
    destshp->DestIDStart = srcshp->DestIDStart;
    destshp->HeaderLen = srcshp->HeaderLen;
    destshp->PinLen = srcshp->PinLen;
    xcpy(destshp->Pin, srcshp->Pin, srcshp->PinLen);
    destshp->ContentFlagsLen = srcshp->ContentFlagsLen;
    xcpy(destshp->ContentFlags, srcshp->ContentFlags, srcshp->ContentFlagsLen);
    destshp->ContentStart = srcshp->ContentStart;
    destshp->ContentLen = srcshp->ContentLen;
    destshp->SrcAddr = srcshp->SrcAddr;
}


string ParseExt_MSHINET::_BUILD_CRC(vector<string> &paras, string &func_return) {
	//new websocket v13:
	//UINT=ParseExt_MSHINET::u16CRC((BYTE *)paras[0].data(),paras[0].size())
	//string tmp=paras[0];
	UInt32 i = ParseExt_MSHINET::u16CRC((BYTE *)(paras[0].data()), paras[0].size());
	string tmp = STRING_Helper::UInt32ToBinaryStr(i);
	func_return = STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", true);

	return "";
}

UINT ParseExt_MSHINET::u16CRC(BYTE *pData, size_t len)
{
	// calculate 2 CRC bytes
	size_t i, crc = 0;
	UINT CRC;
	for (i = 0; i < len; i++)
		crc = CRC_TABLE[crc^pData[i]];
	CRC = (((0x30 + ((crc >> 4) & 0x0F)) << 8) & 0xFF00) + (0x30 + (crc & 0x0f));
	return CRC;

}

//ly change 所有BYTE offset改为：size_t offset
UINT ParseExt::scanUINT(BYTE *pBuf, size_t offset)
{
	return((((UINT)pBuf[offset]) << 8) + pBuf[offset + 1]);
}

ULONG ParseExt::scanULONG(BYTE *pBuf, size_t offset)
{
	pBuf += offset;

	return (((ULONG)pBuf[0]) << 24) + (((ULONG)pBuf[1]) << 16) + (((ULONG)pBuf[2]) << 8) + pBuf[3];
}

uint64 ParseExt::scanUINT64(BYTE *pBuf, size_t offset)
{
	pBuf += offset;

	return (((uint64)pBuf[0]) << 56) + (((uint64)pBuf[1]) << 48) + (((uint64)pBuf[2]) << 40) + (((uint64)pBuf[3]) << 32) +
		(((uint64)pBuf[4]) << 24) + (((uint64)pBuf[5]) << 16) + (((uint64)pBuf[6]) << 8) + pBuf[7];

}

BYTE ParseExt::putUINT(BYTE *pBuf, size_t offset, ULONG u16value)
{
	pBuf[offset++] = (BYTE)(u16value >> 8);
	pBuf[offset] = (BYTE)(u16value & 0xff);
	return(2);
}


BYTE ParseExt::putULONG(BYTE *pBuf, size_t offset, ULONG u32value)
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

BYTE ParseExt::putUINT16(BYTE *pBuf, size_t offset, ULONG u16value)
{
	pBuf[offset++] = (BYTE)(u16value >> 8);
	pBuf[offset] = (BYTE)(u16value & 0xff);
	return(2);
}


BYTE ParseExt::putUINT32(BYTE *pBuf, size_t offset, ULONG u32value)
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


BYTE ParseExt::putUINT64(BYTE *pBuf, size_t offset, uint64 u64value)
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


void ParseExt::xcpy(void *dest, void *src, UINT nBytes)
{
	BYTE *p;
	BYTE *s;

	p = (BYTE *)dest;
	s = (BYTE *)src;
	while (nBytes--) *(p++) = *(s++);

}


/*=============================================================================
 ; Function:	ParseCommand(*CmdStr, CmdLen)
 ; Abstract:	Parse command buffer to a command structure
 ; Returns:	tsCommand - a structure contains all information needed for command
 ;			processing
 ; Inputs:	*CmdBuf - command string buffer
 ;			CmdLen - buffer length
 ==============================================================================*/

ParseExt_MSHINET_CMD::tsCommand ParseExt_MSHINET_CMD::ParseCommand(BYTE *CmdBuf){
    BYTE i, pos;
    tsCommand cmd;
    
    cmd.CmdType = CmdBuf[CMD_CMDTYPE];
    cmd.CmdCode = CmdBuf[CMD_CMDCODE];
    cmd.CmdFlag = CmdBuf[CMD_CMDFLAG];
    cmd.NumParams = cmd.ParamLen = 0;
    cmd.DestID = cmd.DestIDLen = cmd.DestIDType = 0;
    pos = 3;
    
    // whether it's a cmd to be sent out
    if (cmd.CmdType & CMD_CMDTYPE_SENDPACKETMASK)
    {
        cmd.bSendPacket = TRUE;
        cmd.CmdType &= ~CMD_CMDTYPE_SENDPACKETMASK;
    }
    else
        cmd.bSendPacket = FALSE;
    
    // whether DestID is attached
    if (cmd.bSendPacket && (cmd.CmdFlag & CMD_CMDFLAG_DESTIDATTACHED))
    {
        cmd.DestIDType = CmdBuf[pos ++];
        
        // get DestID according to DestID type and length
        // if an invalid type and length combination is set, DestID length will be considered as 0 and
        // the DestID field will not be usable
        // in this case, the length of the DestID, if there is any, will seem to be "removed" from the
        // whole command in the next processes
        switch (cmd.DestIDType)
        {
            case CMD_DIDTYPE_INTNTNODEID:
            case CMD_DIDTYPE_EXTNTNODEID:
            case CMD_DIDTYPE_LNODEID:
                if ((cmd.CmdFlag & CMD_CMDFLAG_DESTIDSRCIDLEN) == CMD_CMDFLAG_DESTIDSRCIDLEN2)
                {
                    cmd.DestIDLen = 2;
                    cmd.DestID = scanUINT(CmdBuf, pos);
                }
                else if ((cmd.CmdFlag & CMD_CMDFLAG_DESTIDSRCIDLEN) == CMD_CMDFLAG_DESTIDSRCIDLEN4)
                {
                    cmd.DestIDLen = 4;
                    cmd.DestID = scanULONG(CmdBuf, pos);
                }
                else if ((cmd.CmdFlag & CMD_CMDFLAG_DESTIDSRCIDLEN) == CMD_CMDFLAG_DESTIDSRCIDLEN8)
                {
                    cmd.DestIDLen = 8;
                    cmd.DestID = scanUINT64(CmdBuf, pos);
                }
                else
                {
                    cmd.DestIDLen = 0;
                }
                break;
            case CMD_DIDTYPE_NODEMAC:
                if ((cmd.CmdFlag & CMD_CMDFLAG_DESTIDSRCIDLEN) == CMD_CMDFLAG_DESTIDSRCIDLEN8)
                {
                    cmd.DestIDLen = 8;
                    cmd.DestID = scanUINT64(CmdBuf, pos);
                }
                else
                {
                    cmd.DestIDLen = 0;
                }
                break;
            case CMD_DIDTYPE_NODEIDRNG:
            case CMD_DIDTYPE_LNODEIDRNG:
                if ((cmd.CmdFlag & CMD_CMDFLAG_DESTIDSRCIDLEN) == CMD_CMDFLAG_DESTIDSRCIDLEN4)
                {
                    cmd.DestIDLen = 4;
                    cmd.DestID = scanULONG(CmdBuf, pos);
                }
                else if ((cmd.CmdFlag & CMD_CMDFLAG_DESTIDSRCIDLEN) == CMD_CMDFLAG_DESTIDSRCIDLEN8)
                {
                    cmd.DestIDLen = 8;
                    cmd.DestID = scanUINT64(CmdBuf, pos);
                }
                else
                {
                    cmd.DestIDLen = 0;
                }
                break;
            case CMD_DIDTYPE_ZONEGROUP:
                if ((cmd.CmdFlag & CMD_CMDFLAG_DESTIDSRCIDLEN) == CMD_CMDFLAG_DESTIDSRCIDLEN2)
                {
                    cmd.DestIDLen = 2;
                    cmd.DestID = scanUINT(CmdBuf, pos);
                }
                else
                {
                    cmd.DestIDLen = 0;
                }
                break;
            case CMD_DIDTYPE_DUTYID:
                if ((cmd.CmdFlag & CMD_CMDFLAG_DESTIDSRCIDLEN) == CMD_CMDFLAG_DESTIDSRCIDLEN2)
                {
                    cmd.DestIDLen = 2;
                    cmd.DestID = scanUINT(CmdBuf, pos);
                }
                else
                {
                    cmd.DestIDLen = 0;
                }
                break;
            default:
                cmd.DestIDLen = 0;
                break;
        }
        if (cmd.DestIDLen)
            pos += cmd.DestIDLen;
        else
        {
            if ((cmd.CmdFlag & CMD_CMDFLAG_DESTIDSRCIDLEN) == CMD_CMDFLAG_DESTIDSRCIDLEN2)
                pos += 2;
            else if ((cmd.CmdFlag & CMD_CMDFLAG_DESTIDSRCIDLEN) == CMD_CMDFLAG_DESTIDSRCIDLEN4)
                pos += 4;
            else if ((cmd.CmdFlag & CMD_CMDFLAG_DESTIDSRCIDLEN) == CMD_CMDFLAG_DESTIDSRCIDLEN8)
                pos += 8;
        }
    }
    else
        cmd.DestIDLen = 0;
    
    // only parse the parameters unless it's not string cmd or passthru cmd
    if ((cmd.CmdType != CMD_CMDTYPE_PASSTHRU) &&
        (cmd.CmdType != CMD_CMDTYPE_STRINGCMD))
    {
        // read command parameters if there are any
        if (cmd.CmdFlag & CMD_CMDFLAG_PARAMATTACHED)
        {
            // calculate number of parameters and length of each parameter
            cmd.NumParams = (CmdBuf[pos] >> 2) & 0x3F;
            if ((CmdBuf[pos] & 0x03) == 0x00)
                cmd.ParamLen = 1;
            else if ((CmdBuf[pos] & 0x03) == 0x01)
                cmd.ParamLen = 2;
            else if ((CmdBuf[pos] & 0x03) == 0x02)
                cmd.ParamLen = 4;
            else
                cmd.ParamLen = 8;
            pos ++;
            
            for (i = 0; i < cmd.NumParams; i ++)
            {
                if (cmd.ParamLen == 1)
                    cmd.Params[i] = CmdBuf[pos];
                else if (cmd.ParamLen == 2)
                    cmd.Params[i] = scanUINT(CmdBuf, pos);
                else if (cmd.ParamLen == 4)
                    cmd.Params[i] = scanULONG(CmdBuf, pos);
                else if (cmd.ParamLen == 8)
                    cmd.Params[i] = scanUINT64(CmdBuf, pos);
                else
                    cmd.Params[i] = 0;
                pos += cmd.ParamLen;
            }
        }
        else
            cmd.NumParams = 0;
    }
    
    cmd.CmdLen = pos;
    // if it's a passthru or string cmd to be sent out, calculate the total length
    // now cmd.CmdCode is the length of the passthru packet or string command following
    if ((cmd.bSendPacket) || ((cmd.CmdType == CMD_CMDTYPE_PASSTHRU) ||
                              (cmd.CmdType == CMD_CMDTYPE_STRINGCMD)))
    {
        if (cmd.CmdType == CMD_CMDTYPE_STRINGCMD)
            cmd.CmdStrStart = cmd.CmdLen;
        cmd.CmdLen = pos + cmd.CmdCode;
    }
    
    //	UART_Printf(DBG_UART, "%d %x\r\n", cmd.NumParams, cmd.CmdStr);
    return cmd;

}

/*=============================================================================
 ; Function:	BuildCommand(*CmdBuf, *CmdBuilder)
 ; Abstract:	Build command buffer from a command builder structure
 ; Returns:	Length of the command buffer
 ; Inputs:	*CmdBuf - command buffer
 ;			*CmdBuilder - command builder structure
 ==============================================================================*/

BYTE ParseExt_MSHINET_CMD::BuildCommand(BYTE *CmdBuf, ParseExt_MSHINET_CMD::tsCommand *CmdBuilder)
{
    // this function cannot be used to generate passthru cmd or string cmd
    if ((CmdBuilder->CmdType == CMD_CMDTYPE_PASSTHRU) ||
        (CmdBuilder->CmdType == CMD_CMDTYPE_STRINGCMD))
        return 0;
    else
    {
        BYTE pos = CMD_EXTCONTENTSTART, i;
        // set cmd header
        CmdBuf[CMD_CMDTYPE] = CmdBuilder->CmdType;
        CmdBuf[CMD_CMDCODE] = CmdBuilder->CmdCode;
        CmdBuf[CMD_CMDFLAG] = 0x00;
        
        if (CmdBuilder->NumParams)
        {
            // update CmdFlag if there are CmdParameter
            CmdBuf[CMD_CMDFLAG] |= CMD_CMDFLAG_PARAMATTACHED;
            // calculate CmdParameterFlags
            CmdBuf[pos] = 0x00 | ((CmdBuilder->NumParams << 2) & 0xFC);
            if (CmdBuilder->ParamLen == 1)
                CmdBuf[pos ++] |= 0x00;
            else if (CmdBuilder->ParamLen == 2)
                CmdBuf[pos ++] |= 0x01;
            else if (CmdBuilder->ParamLen == 4)
                CmdBuf[pos ++] |= 0x02;
            else if (CmdBuilder->ParamLen == 8)
                CmdBuf[pos ++] |= 0x03;
            // add extended parameter
            for (i = 0; i < CmdBuilder->NumParams; i ++)
            {
                if (CmdBuilder->ParamLen == 1)
                    CmdBuf[pos] = CmdBuilder->Params[i];
                else if (CmdBuilder->ParamLen == 2)
                    putUINT(CmdBuf, pos, CmdBuilder->Params[i]);
                else if (CmdBuilder->ParamLen == 4)
                    putULONG(CmdBuf, pos, CmdBuilder->Params[i]);
                else if (CmdBuilder->ParamLen == 8)
                    putUINT64(CmdBuf, pos, CmdBuilder->Params[i]);
                pos += CmdBuilder->ParamLen;
            }
        }
        return pos;
    }
}

/*=============================================================================
 ; Function:	BuildStringCommand(*CmdBuf, *CmdStrBuf)
 ; Abstract:	Build command buffer from a command builder structure
 ; Returns:	Length of the command buffer
 ; Inputs:	*CmdBuf - command structure buffer
 ;			*CmdStrBuf - command string
 ==============================================================================*/

BYTE ParseExt_MSHINET_CMD::BuildStringCommand(BYTE *CmdBuf, char *CmdStrBuf)
{
    // set cmd header
    UINT l = strlen(CmdStrBuf);
    if (l <= 0xFF)
    {
        CmdBuf[CMD_CMDTYPE] = CMD_CMDTYPE_STRINGCMD;
        CmdBuf[CMD_CMDCODE] = l;
        CmdBuf[CMD_CMDFLAG] = 0x00;
        xcpy(&CmdBuf[CMD_CMDFLAG + 1], CmdStrBuf, l);
        return l + 3;
    }
    else
        return 0;
}

BYTE ParseExt_MSHINET::SendStorageReadPacket(UINT DestNodeID,UINT StorageID,UINT address,BYTE readlen)
{
    //buildheader
    tsHeaderParser shb;
    shb.HeaderStart=MSHI_PCKT_STX;
    shb.PacketType=MSHI_PCKTTYPE_UNICASTCMD;
    shb.nwkflags.nwkflags=0x00;
    shb.pcktflags1.pcktflags1=0;
    shb.pcktflags2.pcktflags2=MSHI_PKF2_STATUSNEEDED;
    shb.ackflags2.pcktflags2=0;
    shb.ProcessIDLen[MSHI_PID_PIDINDEX]=4;
    //shb.ProcessID=ACK_GetUniquePID(MSHI_PID_COMMAND);
    shb.ProcessIDLen[MSHI_PID_NPIDINDEX]=0;
    shb.NextProcessID=0;
    shb.ProcessIDLen[MSHI_PID_CPIDINDEX]=0;
    shb.CloudProcessID=0;
    shb.ProcessIDLen[MSHI_PID_CNPIDINDEX]=0;
    shb.CloudNextProcessID=0;
    shb.OtherIDLen[MSHI_OID_DESTIDINDEX]=2;
    shb.OtherID[MSHI_OID_DESTIDINDEX]=DestNodeID;
    shb.OtherIDLen[MSHI_OID_SRCIDINDEX]=2;
    //shb.OtherID[MSHI_OID_SRCIDINDEX]=Local_NodeTable.NodeID;
    shb.OtherIDLen[MSHI_OID_OID2INDEX]=0;
    shb.OtherIDLen[MSHI_OID_OID3INDEX]=0;
    shb.PinLen=0;
    shb.ContentFlagsLen=0;
    
    //buildcontent
    ParseExt_MSHINET_CMD::tsCommandBuilder scmd;
    scmd.CmdType=CMD_CMDTYPE_STORAGE;
    scmd.CmdCode=CMD_CMDTYPE_STORAGE_CMDCODE_READ;
    scmd.NumParams=3;
    scmd.ParamLen=4;
    scmd.Params[0]=address;
    scmd.Params[1]=readlen;
    scmd.Params[2]=StorageID;
    //shb.ContentLen=ParseExt_MSHINET_CMD::BuildCommand(ContentBuf,&scmd);
    
    //buildpacket
    //BuildPacket(OutPcktBuf,&shb,ContentBuf);
    return 0;
}


BYTE  ParseExt_MSHINET::SendByteStreamDownloadPacket(UINT DestNodeID,UINT TID,UINT address,BYTE* bstream,BYTE bslen,BOOL bACKNeeded)
{
    //buildheader
    tsHeaderParser shb;
    shb.HeaderStart=MSHI_PCKT_STX;
    shb.PacketType=MSHI_PCKTTYPE_UNICASTBS;
    shb.nwkflags.nwkflags=0x00;
    shb.pcktflags1.pcktflags1=0;
    if(bACKNeeded)
        shb.pcktflags2.pcktflags2=MSHI_PKF2_NONACK|MSHI_PKF2_ACKNEEDED;
    else
        shb.pcktflags2.pcktflags2=MSHI_PKF2_NONACK;
    shb.ackflags2.pcktflags2=0;
    if(bACKNeeded)
    {
        shb.ProcessIDLen[MSHI_PID_PIDINDEX]=4;
        //shb.ProcessID=ACK_GetUniquePID(MSHI_PID_BYTESTREAM);
    }
    else
    {
        shb.ProcessIDLen[MSHI_PID_PIDINDEX]=2;
        shb.ProcessID=MSHI_PID_BYTESTREAM;
    }
    shb.ProcessIDLen[MSHI_PID_NPIDINDEX]=0;
    shb.NextProcessID=0;
    shb.ProcessIDLen[MSHI_PID_CPIDINDEX]=0;
    shb.CloudProcessID=0;
    shb.ProcessIDLen[MSHI_PID_CNPIDINDEX]=0;
    shb.CloudNextProcessID=0;
    shb.OtherIDLen[MSHI_OID_DESTIDINDEX]=2;
    shb.OtherID[MSHI_OID_DESTIDINDEX]=DestNodeID;
    shb.OtherIDLen[MSHI_OID_SRCIDINDEX]=2;
    //shb.OtherID[MSHI_OID_SRCIDINDEX]=Local_NodeTable.NodeID;
    shb.OtherIDLen[MSHI_OID_OID2INDEX]=2;
    shb.OtherID[MSHI_OID_OID2INDEX]=TID;
    shb.OtherIDLen[MSHI_OID_OID3INDEX]=2;
    shb.OtherID[MSHI_OID_OID3INDEX]=address;
    shb.PinLen=0;
    shb.ContentFlagsLen=0;
    
    //buildcontent
    shb.ContentLen=bslen;
    //xcpy(ContentBuf,bstream,bslen);
    
    //buildpacket
    //BuildPacket(OutPcktBuf,&shb,ContentBuf);
    return 0;
}


/*
size_t ParseExt_MSHINET_CMD::ParseStringCommand(BYTE *CmdBuf, char *CmdStrBuf)
{
    // set cmd header
    UINT l = strlen(CmdStrBuf);
    if (l <= 0xFF)
    {
        CmdBuf[CMD_CMDTYPE] = CMD_CMDTYPE_STRINGCMD;
        CmdBuf[CMD_CMDCODE] = l;
        CmdBuf[CMD_CMDFLAG] = 0x00;
        xcpy(&CmdBuf[CMD_CMDFLAG + 1], CmdStrBuf, l);
        return l + 3;
    }
    else
        return 0;
}*/


string ParseExt_MSHINET_CMD::DECODE(const char *method, string &parse_buff, Http_ParseObj *pub_obj, string &to_obj) {
    //???应该加入substr长度安全判断，就是parse_buff太短，substr会出错。
    //初始化
    string tmp_str;
    string field_value("");
    string name_str = to_obj;
    name_str += ".";
    string field_name;
    bool is_success = false;
    
    //解析操作
    BYTE *cmd_buf=(BYTE *)parse_buff.c_str();
    BYTE cmd_type = cmd_buf[CMD_CMDTYPE];
    BYTE cmd_code = cmd_buf[CMD_CMDCODE];
    BYTE cmd_flag = cmd_buf[CMD_CMDFLAG];
    //取得PacketFlag
    field_name = name_str + "CmdType";
    pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(cmd_type,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
    field_name = name_str + "CmdCode";
    pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(cmd_code,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
    field_name = name_str + "CmdFlag";
    pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(cmd_flag,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
    
    //取得命令内容
    if(cmd_type==CMD_CMDTYPE_STRINGCMD){
        field_name = name_str + "_Content";
        string tmp_cmd=string((const char *)(cmd_buf+3),cmd_code);
        pub_obj->SetNodeValue(field_name.c_str(), tmp_cmd.c_str(),Http_ParseObj::STR_COPY_OBJ);
    }else{
        //取得STX
        tsCommand  shp = ParseCommand(cmd_buf);
        field_name = name_str + "SendPacket";
        field_value = STRING_Helper::iToStr(shp.bSendPacket, 16);
        pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
        
        //取得PacketFlag
        field_name = name_str + "DestIDType";
        pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(shp.DestIDType,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
        field_name = name_str + "DestIDLen";
        pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(shp.DestIDLen,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
        field_name = name_str + "DestID";
        if (shp.DestIDLen != 0) {
            string tmp=STRING_Helper::putUINTToStr(shp.DestID, shp.DestIDLen*8);
            field_value=STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", false);
            pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
        }
        else {
            pub_obj->SetNodeValue(field_name.c_str(), "", Http_ParseObj::STR_COPY_OBJ);
        }
        //pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(shp.DestID,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
        //取得PacketFlag
        field_name = name_str + "NumParams";
        pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(shp.NumParams,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
        field_name = name_str + "ParamLen";
        pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(shp.ParamLen,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
        field_name = name_str + "Params";
        for (int i = 0; i < shp.NumParams; i++) {
            field_name = name_str + "Param_"+STRING_Helper::iToStr(i);
            string tmp=STRING_Helper::putUINTToStr(shp.Params[i], shp.ParamLen*8);
            field_value=STRING_Helper::HBCStrToHBCStr(tmp, "char", "hex", false);
            pub_obj->SetNodeValue(field_name.c_str(), field_value.c_str(), Http_ParseObj::STR_COPY_OBJ);
        }
        
        field_name = name_str + "CmdLen";
        pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(shp.CmdLen,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
        //取得PacketFlag
        field_name = name_str + "CmdRealCode";
        pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(shp.CmdRealCode,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
        field_name = name_str + "CmdStrStart";
        pub_obj->SetNodeValue(field_name.c_str(), STRING_Helper::iToStr(shp.CmdStrStart,16).c_str(), Http_ParseObj::STR_COPY_OBJ);
        //取得命令内容
//        field_name = name_str + "CmdString";
//        if(shp.CmdType==CMD_CMDTYPE_STRINGCMD){
//            string tmp_cmd=string((const char *)(&shp+shp.CmdStrStart),shp.CmdCode);
//            pub_obj->SetNodeValue(field_name.c_str(), tmp_cmd.c_str(),Http_ParseObj::STR_COPY_OBJ);
//        }
    }
   
    //设置成功
    is_success = true;
    
    field_name = name_str + "IsSuccess";
    if (is_success == false) {
        pub_obj->SetNodeValue(field_name.c_str(), "false", Http_ParseObj::STR_COPY_OBJ);
        return "ParseExt_MSHINET::DECODE:ParseHeader Error-Content Len Is Empty!";
    }
    else {
        pub_obj->SetNodeValue(field_name.c_str(), "true", Http_ParseObj::STR_COPY_OBJ);
        return "";
    }
    
}

string ParseExt_MSHINET_CMD::ENCODE(vector<string> &paras, string &func_return) {
    func_return = "";
    if (paras[0].size() == 0) {
        return "ENCODE(): input value is null !";
    }
    if (paras.size() < 5)
        return "ENCODE(): Paras num should >=5 !";
    
    ParseExt_MSHINET_CMD::tsCommand shb;
    if (paras[1]=="CMD_STR"){
        BYTE CmdBuf[256+10];
        size_t msg_len=ParseExt_MSHINET_CMD::BuildStringCommand(CmdBuf, (char *)paras[0].c_str());
        if (msg_len==0)
            return "ENCODE(): paras[0] add cmd error!";
        func_return=string((const char *)CmdBuf,msg_len);
        
    }else if(paras.size()>=5){//参数指令
        //Head Set
        shb.CmdType = (BYTE)STRING_Helper::ParseInt(paras[0], 16);
        shb.CmdCode = (BYTE)STRING_Helper::ParseInt(paras[1], 16);
        shb.NumParams = (BYTE)STRING_Helper::ParseInt(paras[2], 16);
        shb.ParamLen=(BYTE)STRING_Helper::ParseInt(paras[3]);
        //ProcesID/NextProcessID Set
        //vector <string > paras_arr;
        //STRING_Helper::stringSplit(paras[4].c_str(), paras_arr, "|");
        for(size_t i=0; i<shb.NumParams && i<4; i++){
            string tmp=STRING_Helper::HBCStrToHBCStr(paras[4+i], "hex", "char", false);
            shb.Params[i] = STRING_Helper::scanUINTStr(tmp);
        }
        
        BYTE *pPacket = (BYTE *)malloc(8*MAX_PARAMS_IN_CMD+ 256);
        size_t packet_len = BuildCommand(pPacket, &shb);
        func_return.append((const char*)pPacket, packet_len);
        free(pPacket);
    }else
        return "ENCODE(): Paras num should be 5 !";
    
    return "";
}



