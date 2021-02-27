#ifndef PARSE_EXT_H_
#define PARSE_EXT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <map>
#include "ParseObj.h"
#include "Linux_Win.h"
#define MSHI3

using namespace std;

class ParseExt
{
private:

public:
	  static string DECODE(const char *method, string &parse_buff, Http_ParseObj *pub_obj, string &to_obj);
	  static string LEN(vector<string> &paras, string &func_return); 
	  static string ENCODE(vector<string> &paras, string &func_return);
	  static string FUNC(vector<string> &paras, string &func_return);
	  enum{
		  PACKET_CUT_LEN=1,
		  PACKET_BEGIN_END=2
	  };
    
    /*=============================================================================
     ; Function:	scanUINT(*pBuf, offset)
     ; Purpose:	scan 16 bit unsigned integer from indicated buffer at offset
     ; Returns:	16 bit value
     ; Inputs:	*pBuf:	-> Buffer
     ;			offset:	starting offset (0 = first byte)
     ==============================================================================*/
    static UINT scanUINT(BYTE *pBuf, size_t offset);
    static ULONG scanULONG(BYTE *pBuf, size_t offset);
    static uint64 scanUINT64(BYTE *pBuf, size_t offset);
    
    /*=============================================================================
     ; Function:	putUINT32(pBuf, offset, u32value)
     ; Abstract:	insert 32-bit unsigned integet into the indicated buffer @ offset
     ; Returns:	Number of bytes copied
     ; Inputs:	pBuf:		-> destination buffer
     ;			offset:		buffer offset
     ;			u32value:	value to be written (always MSByte first)
     ==============================================================================*/
    static BYTE putUINT(BYTE *pBuf, size_t offset, ULONG u16value);
    static BYTE putULONG(BYTE *pBuf, size_t offset, ULONG u32value);
    static BYTE putUINT16(BYTE *pBuf, size_t offset, ULONG u16value);
    static BYTE putUINT32(BYTE *pBuf, size_t offset, ULONG u32value);
    static BYTE putUINT64(BYTE *pBuf, size_t offset, uint64 u64value);
    /*=============================================================================
     ; Function:	xcpy(dest, src, nBytes)
     ; Purpose:	send single character on applicaton uart
     ; Returns:	None
     ; Inputs:	dest:	destination address
     ;			src:	source address
     ;			nBytes:	number of bytes to copy
     ==============================================================================*/
    
    static void xcpy(void *dest, void *src, UINT nBytes);
};

class ParseExt_HTTP :public ParseExt{
public:
	  static string DECODE(const char *method, string &parse_buff, Http_ParseObj *pub_obj, string &to_obj);
	  static string LEN(vector<string> &paras, string &func_return);  
	  //static string ENCODE(vector<string> &paras, string &func_return);
	  static string URL_DECODE(string &url);
	  static string FUNC(vector<string> &paras, string &func_return);
	  static string _UPLOAD_FILE(vector<string> &paras, string &func_return);
	  static int _UPLOAD_FILE_savefiles(string file_path, char * file_data, int file_len, char* head1, char*head2, string &save_file_name);
	  static int _UPLOAD_FILE_creat_path_dir(const char * path, string split_str);
	  enum {
		  NULL_METHOD = 0,
		  POST = 1,
		  GET = 2
	  };
};

#if !defined(__ANDROID__) && !defined(__APPLE__)
class ParseExt_WEBSOCKET :public ParseExt{
public:
	  static string DECODE(const char *method, string &parse_buff, Http_ParseObj *pub_obj, string &to_obj);
	  static string LEN(vector<string> &paras, string &func_return); 
	  static string ENCODE(vector<string> &paras, string &func_return);
	  static string FUNC(vector<string> &paras, string &func_return);
	  static string _ACCEPT_KEY(vector<string> &paras, string &func_return);
	  static string _FRAME_TYPE(string &frame_str,string &func_return);
	  static string _FRAME_LEN(string &frame_str,string &func_return);
	  static string _FRAME_PAYLOAD(string &frame_str,string &func_return);
};
#endif
class ParseExt_MSHINET2 :public ParseExt {
public:
	/**********************MSHINET*********************/
	#define DATA_PACKTYPE						1
	#define NW_PCKT_STX				0x02
	#define NW_DATA_STX				0x01
	#define NW_DID_ACKNEEDED		0x02
	#define NW_DID_ACKRELATED		0x01

	#define PCKT_STX							0
	#define PCKT_PACKTYPE						1
	#define PCKT_PACKLEN						2
	#define PCKT_HDRLEN							2
	#define PCKT_PACKFLAG1						3
	#define PCKT_PACKFLAG2						4
	#define PCKT_PINLEN							4

		//----------------------------------------------------------------------------- PacketFlag BYTE1 Processing
	#define PACKFLAG1_ACKRELATED			0x80
	#define PACKFLAG1_ACK					0x20

	#define PACKFLAG1_STATUSNEEDED			0x10

	#define PACKFLAG1_ACKNEEDED				0x08

	#define PACKFLAG1_BRSCFLAG				0x20				// Bit 5
	#define PACKFLAG1_BR					0x00				// broadcast
	#define PACKFLAG1_SC					0x20				// single-cast
		//----------------------------------------------------------------------------- PacketFlag BYTE2 Processing
	#define PACKFLAG2_PACKDIRECTION			0x80				// Bit 7
	#define PACKFLAG2_PACKUPLOAD			0x00				// upload packet
	#define PACKFLAG2_PACKDOWNLOAD			0x80				// download packet

	#define PACKFLAG2_PACKPINATTACHED		0x40				// Bit 6 Pin attached

	#define PACKFLAG2_ENDDEVICEPACK			0x20				// Bit 5 End device-related
	#define PACKFLAG2_ENDDEVICEROUTERPACK	0x10				// Bit 4 Is it routed by a Router?

	typedef struct
	{
		BOOL ACKRelated; 			// 是否为ACK相关包
		BOOL bExtContent; 		// 是否有扩展内容
		BYTE HeaderStart; 		// STX，SOH
		BYTE PacketType;
		uint64 PacketLen; 			// 整个包的长度（包含尾部0x0D0A）//ly change BYTE->uint64
		BYTE BasicPacketLen; 		// 基本包的长度
		BYTE PacketFlags[2];
		BOOL ACKNeeded; 			// 是需要返回ACK
		BYTE SourceIDPos; 		// SourceID的起始Byte位置
		BYTE SourceIDLen; 		// SourceID的长度
		uint64 SourceID;
		BYTE DestIDPos; 			// DestID的起始Byte位置
		BYTE DestIDLen; 			// DestID长度
		uint64 DestID;
		BYTE TaskIDPos; 			// TaskID或PacketID的起始Byte位置
		BYTE TaskIDLen; 			// TaskID或PacketID的长度
		UINT TaskID;
		BYTE ContentStart; 		// 包内容的起始位置
		uint64 ContentLen; 		// 包内容的长度
		ULONG Pin;
		BOOL CRC;
	}tsHeaderParser;



	//以下结构含有需要组包时的必要信息，在组包之前必须新建一个该结构并且将该结构中的需要在包中使用的元素均置为需要的值。
	typedef struct
	{
		BYTE HeaderStart;		// STX or SOH
		BYTE PacketType;
		BYTE PacketFlags[2];
		UINT TaskID;
		uint64 SourceID;
		uint64 DestID;
		BOOL bExtContent;
	}tsHeaderBuilder;

	static string DECODE(const char *method, string &parse_buff, Http_ParseObj *pub_obj, string &to_obj);
	static string LEN(vector<string> &paras, string &func_return);
	static string ENCODE(vector<string> &paras, string &func_return);
	static string FUNC(vector<string> &paras, string &func_return);
	static string _BUILD_CRC(vector<string> &paras, string &func_return);

	/*=============================================================================
	; Function:	ParseHeader(pPacket);使用该函数就可以将存放于pPacket指针处的包头信息解析到tsHeaderParser结构体中
	; Abstract:	Retract header from a packet
	; Returns:	tsHeaderParser - structure for translating the header
	; Inputs:	*pPacket - packet buffer
	==============================================================================*/
	static tsHeaderParser ParseHeader(BYTE *pPacket);

	/*=============================================================================
	; Function:	BuildHeader(pPacket, shb, contentlen);该函数需要一个已经初始化好的tsHeaderBuilder结构体，运行使用该函数就可以将依据该结构体组建好的包头存放于pPacket指针处，该函数返回的是包头的总长度
	; Abstract:	Build header from a header builder
	; Returns:	Header length
	; Inputs:	*pPacketHeader - packet buffer (header put into packet buffer too)
	;			shb - struct tsHeaderBuilder
	;			contentlen - content length of the packet (including CRC & 0D0A)
	==============================================================================*/
	static BYTE BuildHeader(BYTE *pPacketHeader, tsHeaderBuilder *shb, uint64 contentlen);
	/*=============================================================================
	; Function:	MessagePacketFixedPartLen(PID)
	; Abstract:	Determines the length of the fixed part within a packet
	; Returns:	Fixed part length
	; Inputs:	PID - PacketID
	==============================================================================*/
	static BYTE GetPacketHeaderLen(BYTE STX, BYTE PID);
	/*=============================================================================
	; Function:	MessagePacketFixedPartLen(PID)
	; Abstract:	Determines the length of the fixed part within a packet
	; Returns:	Fixed part length
	; Inputs:	PID - PacketID
	==============================================================================*/
	static BYTE MessagePacketFixedPartLen(BYTE PID);
	/*=============================================================================
	; Function:	MessagePacketFixedPartLen(PID)
	; Abstract:	Determines the length of the fixed part within a packet
	; Returns:	Fixed part length
	; Inputs:	PID - PacketID
	==============================================================================*/
	static BYTE DataPacketFixedPartLen(BYTE PID);

	/*=============================================================================
	; Function:	BuildPacket(pPacket, shb, content, contentlen);在包的内容已经建立在content指针处（长度为contentlen），给定一个已经初始化好的tsHeaderBuilder结构体，使用下列BuildPacket就可以将一个完全建立好的包放置于pPacket指针处。
	; Abstract:	Build packet (add header and content together)
	; Returns:	BYTE - packet length
	; Inputs:	*pPacket - packet buffer
	;			shb - packet's header builder
	;			*content - content buffer
	;			contentlen - "false" content length
	==============================================================================*/
	static size_t BuildPacket(BYTE *pPacket, tsHeaderBuilder *shb, BYTE *content, uint64 contentlen);

	/*=============================================================================
	; Function:	BuildPacketContent(pPacketContent, len);该函数除了使用以上的BuildHeader函数以外，还使用了一个其他Private函数来计算包内容的CRC。
	; Abstract:	Build packet content and add tail (CRC+0D0A)
	;			Note: The packet content length is calculated but has not been
	;			added into packet yet. This will be done in BuildPacket() function.
	;			So this function shouldn't be called in code ever.
	; Returns:	BYTE - "Real" content length
	; Inputs:	*pPacketContent - packet content buffer
	;			len - content length of "false" content length, which is the length
	;			of the binary only without tail
	==============================================================================*/
	static uint64 BuildPacketExtContent(BYTE *pPacketContent, uint64 len);
	//-----------------------------------------------------------------------------
	static UINT u16CRC(BYTE *pData, size_t len);

	/*=============================================================================
	; Function:	scanUINT(*pBuf, offset)
	; Purpose:	scan 16 bit unsigned integer from indicated buffer at offset
	; Returns:	16 bit value
	; Inputs:	*pBuf:	-> Buffer
	;			offset:	starting offset (0 = first byte)
	==============================================================================*/
	/*static UINT scanUINT(BYTE *pBuf, size_t offset);
	static ULONG scanULONG(BYTE *pBuf, size_t offset);
	static uint64 scanUINT64(BYTE *pBuf, size_t offset);*/

	/*=============================================================================
	; Function:	putUINT32(pBuf, offset, u32value)
	; Abstract:	insert 32-bit unsigned integet into the indicated buffer @ offset
	; Returns:	Number of bytes copied
	; Inputs:	pBuf:		-> destination buffer
	;			offset:		buffer offset
	;			u32value:	value to be written (always MSByte first)
	==============================================================================*/
	/*static BYTE putUINT16(BYTE *pBuf, size_t offset, ULONG u16value);
	static BYTE putUINT32(BYTE *pBuf, size_t offset, ULONG u32value);
	static BYTE putUINT64(BYTE *pBuf, size_t offset, uint64 u64value);*/
	/*=============================================================================
	; Function:	xcpy(dest, src, nBytes)
	; Purpose:	send single character on applicaton uart
	; Returns:	None
	; Inputs:	dest:	destination address
	;			src:	source address
	;			nBytes:	number of bytes to copy
	==============================================================================*/

	//static void xcpy(void *dest, void *src, UINT nBytes);
};

class ParseExt_MSHINET :public ParseExt{
public:
	/**********************MSHINET*********************/
#ifdef MSHI3
    #define MSHI_PCKT_HDRMAXPID 4
    #define MSHI_PCKT_HDRMAXOID 4
    #define MSHI_PCKT_MAXPINLEN 16
    #define MSHI_PCKT_MAXCFSLEN 16
#else
	#define MSHI_PCKT_HDRMAXPID 2
	#define MSHI_PCKT_HDRMAXOID 4
	#define MSHI_PCKT_MAXPINLEN 16
	#define MSHI_PCKT_MAXCFSLEN 16
#endif
		//=============================================================================
		// Packet STX
		//=============================================================================
	#define MSHI_PCKT_STX						0xA5

		//=============================================================================
		// Packet 0x0D0A
		//=============================================================================
	#define MSHI_PCKT_0D0A						0x0D0A

		//=============================================================================
		// Packet Header
		//=============================================================================
	#define MSHI_PCKT_START						0
	#define MSHI_PCKT_STARTLEN					1
	#define MSHI_PCKT_PCKTLEN					1
	#define MSHI_PCKT_PCKTLENLEN				1
	#define MSHI_PCKT_PCKTTYPELEN				1
	#define MSHI_PCKT_NWKFLAGSLEN				1
	#define MSHI_PCKT_PCKTFLAGS1LEN				1
	#define MSHI_PCKT_PCKTFLAGS2LEN				1
	#define MSHI_PCKT_PIDFLAGSLEN				1
	#define MSHI_PCKT_OIDFLAGSLEN				1
	#define MSHI_PCKT_HDRCRCLEN					2

	#define MSHI_PCKT_HDRFIXEDLEN				8

		//=============================================================================
		// Packet Tail
		//=============================================================================
	#define MSHI_PCKT_0D0ALEN					2
	#define MSHI_PCKT_CRCLEN					2

		//=============================================================================
		// Network Flags
		//=============================================================================
	#define MSHI_NWF_PRIORITY					0x80
	#define MSHI_NWF_BROADCAST					0x40
	#define MSHI_NWF_ROUTING					0x20
	#define MSHI_NWF_PASSTHRU					0x10
	#define MSHI_NWF_EDRELATED					0x08

		//=============================================================================
		// Packet Flags #1
		//=============================================================================
	#define MSHI_PKF1_HDRCRC					0x80
	#define MSHI_PKF1_ENCRYPTED					0x40

	#define MSHI_PKF1_PINGLEN					0x30
	#define MSHI_PKF1_PINGBUF					0x30
	#define MSHI_PKF1_PINGLEN4					0x20
	#define MSHI_PKF1_PINGLEN2					0x10
	#define MSHI_PKF1_NOPING					0x00

	#define MSHI_PKF1_CNTFLEN					0x0C
	#define MSHI_PKF1_CNTFBUF					0x0C
	#define MSHI_PKF1_CNTFLEN4					0x08
	#define MSHI_PKF1_CNTFLEN2					0x04
	#define MSHI_PKF1_NOCNTF					0x00

	//#define MSHI_PCKTTYPE_UNICASTBS				0x31

	#define MSHI_PKF1_LEN0						0x00
	#define MSHI_PKF1_LEN2						0x01
	#define MSHI_PKF1_LEN4						0x02
	#define MSHI_PKF1_LENBUF					0x03

		//=============================================================================
		// Packet Flags #2 Non-ACK
		//=============================================================================
	#define MSHI_PKF2_NONACK					0x00
	#define MSHI_PKF2_ACKNEEDED					0x40
	#define MSHI_PKF2_STATUSNEEDED				0x20

		//=============================================================================
		// Packet Flags #2 ACK
		//=============================================================================
	#define MSHI_PKF2_ACKRELATED				0x80
	#define MSHI_PKF2_ACK						0x40
	#define MSHI_PKF2_ACKCODE					0x20
	#define MSHI_PKF2_STSREQ					0x10

	#define MSHI_PKF2_STSLEN					0x0C
	#define MSHI_PKF2_STSBUF					0x0C
	#define MSHI_PKF2_STSLEN8					0x08
	#define MSHI_PKF2_STSLEN4					0x04
	#define MSHI_PKF2_NOSTS						0x00

		//=============================================================================
		// ProcessID Flags
		//=============================================================================
    //mshinet3_add
#ifdef MSHI3
    #define MSHI_PIDF_CPIDLEN					0xC0
    #define MSHI_PIDF_CPIDLEN8					0xC0
    #define MSHI_PIDF_CPIDLEN4					0x80
    #define MSHI_PIDF_CPIDLEN2					0x40
    #define MSHI_PIDF_NOCPID					0x00
    
    #define MSHI_PIDF_CNPIDLEN					0x0C
    #define MSHI_PIDF_CNPIDLEN8					0x0C
    #define MSHI_PIDF_CNPIDLEN4					0x08
    #define MSHI_PIDF_CNPIDLEN2					0x04
    #define MSHI_PIDF_NOCNPID					0x00
#endif

    #define MSHI_PIDF_PIDLEN					0x30
	#define MSHI_PIDF_PIDLEN8					0x30
	#define MSHI_PIDF_PIDLEN4					0x20
	#define MSHI_PIDF_PIDLEN2					0x10
	#define MSHI_PIDF_NOPID						0x00

	#define MSHI_PIDF_NPIDLEN					0x03
	#define MSHI_PIDF_NPIDLEN8					0x03
	#define MSHI_PIDF_NPIDLEN4					0x02
	#define MSHI_PIDF_NPIDLEN2					0x01
	#define MSHI_PIDF_NONPID					0x00
#ifdef MSHI3
	#define MSHI_PID_CPIDINDEX					0
    #define MSHI_PID_PIDINDEX					1
    #define MSHI_PID_CNPIDINDEX					2
	#define MSHI_PID_NPIDINDEX					3
	#define MSHI_PID_NUMPID						4
	#define MSHI_PID_BYTESTREAM					0x0005
#else
    #define MSHI_PID_PIDINDEX					0
    #define MSHI_PID_NPIDINDEX					1
    #define MSHI_PID_NUMPID						2
    #define MSHI_PID_BYTESTREAM					0x0005
#endif

		//=============================================================================
		// OtherID Flags
		//=============================================================================
	#define MSHI_OIDF_OIDLEN					0xC0
	#define MSHI_OIDF_OIDLEN8					0xC0
	#define MSHI_OIDF_OIDLEN4					0x80
	#define MSHI_OIDF_OIDLEN2					0x40
	#define MSHI_OIDF_NOOID						0x00

	#define MSHI_OID_DESTIDINDEX				0
	#define MSHI_OID_SRCIDINDEX					1
	#define MSHI_OID_OID2INDEX					2
	#define MSHI_OID_OID3INDEX					3
	#define MSHI_OID_NUMOID						4

		//=============================================================================
		// Other header-related
		//=============================================================================
	#define MSHI_HDR_MAXPCKTLENLEN				8
	#define MSHI_HDR_PCKTLENLEN2				2
	#define MSHI_HDR_PCKTLENLEN4				4
	#define MSHI_HDR_PCKTLENLEN8				8
    
    //=============================================================================
    // MSHI PROCESSID
    //=============================================================================
#define MSHI_PID_NETWORKORGANIZE			0x0001
#define MSHI_PID_REPORT						0x0002
#define MSHI_PID_EVENT						0x0003
#define MSHI_PID_COMMAND					0x0004
#define MSHI_PID_BYTESTREAM					0x0005
#define MSHI_PID_NODETABLEDR				0x0006
#define MSHI_PID_NODETABLEDL				0x0007
#define MSHI_PID_4KEEPROMDR					0x0008
#define MSHI_PID_4KEEPROMDL					0x0009
#define MSHI_PID_FILEDR						0x000A
#define MSHI_PID_ONDDL						0x000B
#define MSHI_PID_CLOUDSPECIAL				0x000F
    
    //=============================================================================
    // MSHI PACKET TYPE
    //=============================================================================
    // can be used to guess PID
    //#define MSHI_PID_NETWORKORGANIZE			0x0001
    #define MSHI_PCKTTYPE_CHILDPOLL				0x01
    #define MSHI_PCKTTYPE_CHILDOND				0x02
    #define MSHI_PCKTTYPE_EDCONNECT				0x04
        
        //#define MSHI_PID_REPORT					0x0002
    #define MSHI_PCKTTYPE_REGISTRPT				0x11
    #define MSHI_PCKTTYPE_DATARPT				0x12
        
        //#define MSHI_PID_EVENT					0x0003
    #define MSHI_PCKTTYPE_EVENT					0x13
        
        //#define MSHI_PID_COMMAND					0x0004
    #define MSHI_PCKTTYPE_UNICASTCMD			0x21
    #define MSHI_PCKTTYPE_BRCASTCMD				0x22
    #define MSHI_PCKTTYPE_NIDRNGCMD				0x23
        
        //#define MSHI_PID_BYTESTREAM				0x0005
    #define MSHI_PCKTTYPE_UNICASTBS				0x31
    #define MSHI_PCKTTYPE_BRCASTBS				0x32
    #define MSHI_PCKTTYPE_NIDRNGBS				0x33
        
        // cannot be used to guess PID
    #define MSHI_PCKTTYPE_NWKCMD				0x03
        
    #define MSHI_PCKTTYPE_ZONEGROUPCMD			0x24
    #define MSHI_PCKTTYPE_LNIDCMD				0x25
    #define MSHI_PCKTTYPE_LNIDRNGCMD			0x26
    #define MSHI_PCKTTYPE_DUTYIDCMD				0x27
        
    #define MSHI_PCKTTYPE_ZONEGROUPBS			0x34
    #define MSHI_PCKTTYPE_UNICASTXBSSTART		0x35
    #define MSHI_PCKTTYPE_UNICASTXBSPCKT		0x36
    #define MSHI_PCKTTYPE_NIDRNGXBSSTART		0x37
    #define MSHI_PCKTTYPE_NIDRNGXBSPCKT			0x38
        
    #define MSHI_PCKTTYPE_NTDRREQUEST			0x41
    #define MSHI_PCKTTYPE_NTDRRESPONSE			0x42
    #define MSHI_PCKTTYPE_NTDRDATA				0x43
    #define MSHI_PCKTTYPE_NTDREND				0x44
        
    #define MSHI_PCKTTYPE_NTDLREQUEST			0x51
    #define MSHI_PCKTTYPE_NTDLDATA				0x52
    #define MSHI_PCKTTYPE_NTDLEND				0x53
        
    #define MSHI_PCKTTYPE_4KDRREQUEST			0x61
    #define MSHI_PCKTTYPE_4KDRDATA				0x62
    #define MSHI_PCKTTYPE_4KDREND				0x63
        
    #define MSHI_PCKTTYPE_4KDLREQUEST			0x71
    #define MSHI_PCKTTYPE_4KDLDATA				0x72
    #define MSHI_PCKTTYPE_4KDLEND				0x73
    
    //#define MSHI_PCKTTYPE_FILEDRREQUEST		0x81
    //#define MSHI_PCKTTYPE_FILEDRDATA			0x82
    //#define MSHI_PCKTTYPE_FILEDREND			0x83
    #define MSHI_PCKTTYPE_FILEDRREQUEST         0x81
    #define MSHI_PCKTTYPE_FILEDRRESPONSE        0x82
    #define MSHI_PCKTTYPE_FILEDRDATA            0x83
    #define MSHI_PCKTTYPE_FILEDREND             0x84
    
        
    #define MSHI_PCKTTYPE_ONDDLSTART			0x91
    #define MSHI_PCKTTYPE_ONDDLREQUEST			0x92
    #define MSHI_PCKTTYPE_ONDDLDATA				0x93
    #define MSHI_PCKTTYPE_ONDDLEND				0x94
    #define MSHI_PCKTTYPE_ONDDLACTIVATE			0x95
        
    #define MSHI_PCKTTYPE_PUSHNTFCWTID			0xF1
    #define MSHI_PCKTTYPE_PUSHNTFCWTIDINDEX		0xF2
#ifdef WIN32
    
#else
    #define HT_LITTLE_ENDIAN
#endif
#ifdef HT_LITTLE_ENDIAN
	typedef union
	{
		struct
		{
			BYTE bit0 : 1;
			BYTE bit1 : 1;
			BYTE bit2 : 1;
			BYTE edrelated : 1;
			BYTE passthru : 1;
			BYTE routing : 1;
			BYTE broadcast : 1;
			BYTE priority : 1;
		} flags;
		BYTE nwkflags;
	} NetworkFlags;

	typedef union
	{
		struct
		{
			BYTE bit0 : 1;
			BYTE asciihex : 1;
			BYTE contentflags : 2;
			BYTE pin : 2;
			BYTE encrypted : 1;
			BYTE headercrc : 1;
		} flags;
		BYTE pcktflags1;
	} PacketFlags1;
    #ifdef MSHI3
        typedef union
        {
            struct
            {
                BYTE bit0:1;
                BYTE bit1:1;
                BYTE bit2:1;
                BYTE bit3:1;
                BYTE bit4:1;
                BYTE statusneeded:1;
                BYTE ackneeded:1;
                BYTE nonack:1;
            } flags;
            BYTE pcktflags2;
        } NonACKPacketFlags2;
        
        typedef union
        {
            struct
            {
                BYTE bit0:1;
                BYTE bit1:1;
                BYTE status:2;
                BYTE statusreq:1;
                BYTE errcode:1;
                BYTE acknak:1;
                BYTE ackrelated:1;
            } flags;
            BYTE pcktflags2;
        } ACKPacketFlags2;
    #else
        typedef union
        {
            struct
            {
                BYTE nonack : 1;
                BYTE ackneeded : 1;
                BYTE statusneeded : 1;
                BYTE bit4 : 1;
                BYTE bit3 : 1;
                BYTE bit2 : 1;
                BYTE bit1 : 1;
                BYTE bit0 : 1;
            } flags;
            BYTE pcktflags2;
        } NonACKPacketFlags2;

        typedef union
        {
            struct
            {
                BYTE ackrelated : 1;
                BYTE acknak : 1;
                BYTE errcode : 1;
                BYTE statusreq : 1;
                BYTE status : 2;
                BYTE bit1 : 1;
                BYTE bit0 : 1;
            } flags;
            BYTE pcktflags2;
        } ACKPacketFlags2;
    #endif

#else
	typedef union
	{
		struct
		{
			BYTE priority : 1;
			BYTE broadcast : 1;
			BYTE routing : 1;
			BYTE passthru : 1;
			BYTE edrelated : 1;
			BYTE bit2 : 1;
			BYTE bit1 : 1;
			BYTE bit0 : 1;
		} flags;
		BYTE nwkflags;
	} NetworkFlags;

	typedef union
	{
		struct
		{
			BYTE headercrc : 1;
			BYTE encrypted : 1;
			BYTE pin : 2;
			BYTE contentflags : 2;
			BYTE asciihex : 1;
			BYTE bit0 : 1;
		} flags;
		BYTE pcktflags1;
	} PacketFlags1;

	typedef union
	{
		struct
		{
			BYTE nonack : 1;
			BYTE ackneeded : 1;
			BYTE statusneeded : 1;
			BYTE bit4 : 1;
			BYTE bit3 : 1;
			BYTE bit2 : 1;
			BYTE bit1 : 1;
			BYTE bit0 : 1;
		} flags;
		BYTE pcktflags2;
	} NonACKPacketFlags2;

	typedef union
	{
		struct
		{
			BYTE ackrelated : 1;
			BYTE acknak : 1;
			BYTE errcode : 1;
			BYTE statusreq : 1;
			BYTE status : 2;
			BYTE bit1 : 1;
			BYTE bit0 : 1;
		} flags;
		BYTE pcktflags2;
	} ACKPacketFlags2;
    
#endif

    typedef struct
    {
        BYTE HeaderStart;
        BYTE PacketType;
        uint64 PacketLen;
        NetworkFlags nwkflags;
        BYTE nwkflagspos;
        PacketFlags1 pcktflags1;
        NonACKPacketFlags2 pcktflags2;
        ACKPacketFlags2 ackflags2;
        BYTE ProcessIDLen[MSHI_PCKT_HDRMAXPID];
#ifdef MSHI3
        uint64 CloudProcessID;
        uint64 ProcessID;
        uint64 CloudNextProcessID;
        uint64 NextProcessID;
#else
        uint64 ProcessID;
        uint64 NextProcessID;
#endif
        BYTE OtherIDLen[MSHI_PCKT_HDRMAXOID];
        uint64 OtherID[MSHI_PCKT_HDRMAXOID];
        BYTE DestIDStart;
        BYTE HeaderLen;							// if no PIN or Flags in Content, HeaderLen = ContentStart
        BYTE PinLen;							// length of PIN in content, but extracted when processing header
        BYTE Pin[MSHI_PCKT_MAXPINLEN];			// PIN in content, but extracted when processing header, max 16 bytes
        BYTE ContentFlagsLen;					// length of ContentFlags in content, but extracted when processing header
        BYTE ContentFlags[MSHI_PCKT_MAXCFSLEN];	// ContentFlags in content, but extracted when processing header, max 16 bytes
        BYTE ContentStart;						// the real content start pos after PIN or Flags if there is any
        uint64 ContentLen;						// the real content length
        uint64 SrcAddr;
        BYTE parse_err;
    }tsHeaderParser;
    
/*
	typedef struct
	{
		BYTE HeaderStart;
		BYTE PacketType;
		uint64 PacketLen;
		NetworkFlags nwkflags;
		BYTE nwkflagspos;
		PacketFlags1 pcktflags1;
		NonACKPacketFlags2 pcktflags2;
		ACKPacketFlags2 ackflags2;
		BYTE ProcessIDLen[MSHI_PCKT_HDRMAXPID];
		uint64 ProcessID;
		uint64 NextProcessID;
		BYTE OtherIDLen[MSHI_PCKT_HDRMAXOID];
		uint64 OtherID[MSHI_PCKT_HDRMAXOID];
		BYTE DestIDStart;
		BYTE HeaderLen;							// if no PIN or Flags in Content, HeaderLen = ContentStart
		BYTE PinLen;							// length of PIN in content, but extracted when processing header
		BYTE Pin[MSHI_PCKT_MAXPINLEN];			// PIN in content, but extracted when processing header, max 16 bytes
		BYTE ContentFlagsLen;					// length of ContentFlags in content, but extracted when processing header
		BYTE ContentFlags[MSHI_PCKT_MAXCFSLEN];	// ContentFlags in content, but extracted when processing header, max 16 bytes
		BYTE ContentStart;						// the real content start pos after PIN or Flags if there is any
		uint64 ContentLen;						// the real content length
		uint64 SrcAddr;
	}tsHeaderParser;
 */

	static string DECODE(const char *method, string &parse_buff, Http_ParseObj *pub_obj, string &to_obj);
	static string LEN(vector<string> &paras, string &func_return); 
	static string ENCODE(vector<string> &paras, string &func_return);
	static int _ENCODE_IDS(string &src, BYTE &id_len, uint64 &id);
    static void _ENCODE_IDS64(string &src, BYTE &id_len, uint64 &id);
	static string FUNC(vector<string> &paras, string &func_return);
	static string STREAM_DOWNLOAD(vector<string> &paras, string &func_return);
	static string _BUILD_CRC(vector<string> &paras, string &func_return);

	/*=============================================================================
	; Function:	ParseHeader(pPacket);使用该函数就可以将存放于pPacket指针处的包头信息解析到tsHeaderParser结构体中
	; Abstract:	Retract header from a packet
	; Returns:	tsHeaderParser - structure for translating the header
	; Inputs:	*pPacket - packet buffer
	==============================================================================*/
	static tsHeaderParser ParseHeader(BYTE *pPacket);

	/*=============================================================================
	; Function:	BuildHeader(pPacket, shp)
	; Abstract:	Build header from a header builder
	; Returns:	Content start pointer
	; Inputs:	*pPacketHeader - packet buffer (header put into packet buffer too)
	;			shp - struct tsHeaderBuilder
	==============================================================================*/
	static BYTE BuildHeader(BYTE *pPacket, tsHeaderParser *shp);
		
    /*=============================================================================
	; Function:	BuildPacket(pPacket, shb, content)
	; Abstract:	Build packet (add header and content together)
	; Returns:	BYTE - packet length
	; Inputs:	*pPacket - packet buffer
	;			shb - packet's header builder
	;			*content - content buffer
	==============================================================================*/
	static UINT BuildPacket(BYTE *pPacket, tsHeaderParser *shp, BYTE *content);
    static void DuplicateHeader(tsHeaderParser *destshp, tsHeaderParser *srcshp);
    BYTE SendStorageReadPacket(UINT DestNodeID,UINT StorageID,UINT address,BYTE readlen);
    BYTE SendByteStreamDownloadPacket(UINT DestNodeID,UINT TID,UINT address,BYTE* bstream,BYTE bslen,BOOL bACKNeeded);

	//-----------------------------------------------------------------------------
	static UINT u16CRC(BYTE *pData, size_t len);

	/*=============================================================================
	; Function:	xcpy(dest, src, nBytes)
	; Purpose:	send single character on applicaton uart
	; Returns:	None
	; Inputs:	dest:	destination address
	;			src:	source address
	;			nBytes:	number of bytes to copy
	==============================================================================*/

	//static void xcpy(void *dest, void *src, UINT nBytes);
};

class ParseExt_MSHINET_CMD :public ParseExt{
    
public:
    #define MAX_PARAMS_IN_CMD 10
    //=============================================================================
    // CMD
    //=============================================================================
    #define CMD_CMDTYPE										0
    #define CMD_CMDCODE										1
    #define CMD_CMDFLAG										2
    #define CMD_EXTCONTENTSTART								3
        
    #define CMD_CMDTYPE_SENDPACKETMASK						0x80
        
    #define CMD_CMDFLAG_DESTIDATTACHED						0x80
    #define CMD_CMDFLAG_PARAMATTACHED						0x40
    #define CMD_CMDFLAG_BROADCAST							0x20
    #define CMD_CMDFLAG_SRCIDATTACHED						0x10
        
    #define CMD_CMDFLAG_DESTIDSRCIDLEN						0x0C
    #define CMD_CMDFLAG_DESTIDSRCIDLEN0						0x00
    #define CMD_CMDFLAG_DESTIDSRCIDLEN2						0x04
    #define CMD_CMDFLAG_DESTIDSRCIDLEN4						0x08
    #define CMD_CMDFLAG_DESTIDSRCIDLEN8						0x0C
        
    #define CMD_CMDFLAG_PRIORITY							0x02
    #define CMD_CMDFLAG_ACKNEEDED							0x01
        
    #define CMD_CMDFLAG_BROADCAST							0x20
        
    #define CMD_DIDTYPE_INTNTNODEID							0x00
    #define CMD_DIDTYPE_EXTNTNODEID							0x01
    #define CMD_DIDTYPE_NODEMAC								0x02
    #define CMD_DIDTYPE_NODEIDRNG							0x03
    #define CMD_DIDTYPE_ZONEGROUP							0x04
    #define CMD_DIDTYPE_LNODEID								0x05
    #define CMD_DIDTYPE_LNODEIDRNG							0x06
    #define CMD_DIDTYPE_DUTYID								0x07
        
        // passthru special type
    #define CMD_CMDTYPE_PASSTHRU							0x00
        
        // string command special type
    #define CMD_CMDTYPE_STRINGCMD							0x7F
        
        // stored procedure special type
    #define CMD_CMDTYPE_SPSPECIALCMD						0x7E
        
    #define CMD_CMDTYPE_SPSPEC_CMDCODE_DELAY				0x00
        
        // CO: Cloud-related
    #define CMD_CMDTYPE_CLOUD								0x01
        
    #define CMD_CMDTYPE_CLOUD_CMDCODE_STARTUP				0x00
        
        // commission-related
    #define CMD_CMDTYPE_COMMISSION							'C'
        
    #define CMD_CMDTYPE_COMMISSION_CMDCODE_READMODE			0x00
    #define CMD_CMDTYPE_COMMISSION_CMDCODE_CHANGEMODE		0x01
    #define CMD_CMDTYPE_COMMISSION_CMDCODE_COMMISSONNODE	0x02
    #define CMD_CMDTYPE_COMMISSION_CMDCODE_REGISTERNODE		0x03
    #define CMD_CMDTYPE_COMMISSION_CMDCODE_DELAYEDRESETALL	0x04
    #define CMD_CMDTYPE_COMMISSION_CMDCODE_UNCOMMISSONNODE	0x05
    #define CMD_CMDTYPE_COMMISSION_CMDCODE_UNREGISTERNODE	0x06
    #define CMD_CMDTYPE_COMMISSION_CMDCODE_DELETENODE		0x07
        
        // node system-related
    #define CMD_CMDTYPE_NODESYS								'Z'
        
    #define CMD_CMDTYPE_NODESYS_CMDCODE_READ				0x00
    #define CMD_CMDTYPE_NODESYS_CMDCODE_WRITE				0x01
    #define CMD_CMDTYPE_NODESYS_CMDCODE_ERASE				0x02
    #define CMD_CMDTYPE_NODESYS_CMDCODE_RESET				0x03
    #define CMD_CMDTYPE_NODESYS_CMDCODE_CLOCK				0x04
        
        // storage-related
    #define CMD_CMDTYPE_STORAGE								'E'
        
    #define CMD_CMDTYPE_STORAGE_CMDCODE_READ				0x00
    #define CMD_CMDTYPE_STORAGE_CMDCODE_FILL				0x01
    #define CMD_CMDTYPE_STORAGE_CMDCODE_ERASE				0x02
        
        // report/event-related
    #define CMD_CMDTYPE_RPTEVT								0x05
        
    #define CMD_CMDTYPE_RPTEVT_CMDCODE_REQUESTREPORT		0x00
    #define CMD_CMDTYPE_RPTEVT_CMDCODE_RPTOVERRIDEFLAGS		0x01
    #define CMD_CMDTYPE_RPTEVT_CMDCODE_EVTOVERRIDEFLAGS		0x02
        
        // register-related
    #define CMD_CMDTYPE_REGISTER							'R'
        
    #define CMD_CMDTYPE_REGISTER_CMDCODE_READREGISTER		0x00
    #define CMD_CMDTYPE_REGISTER_CMDCODE_CHANGEREGISTER		0x01
        
        // N_NodeTable-related
    #define CMD_CMDTYPE_NNODETABLE							'N'
        
    #define CMD_CMDTYPE_NNODETABLE_CMDCODE_READVALUENNTID		0x00
    #define CMD_CMDTYPE_NNODETABLE_CMDCODE_CHANGEVALUENNTID		0x01
    #define CMD_CMDTYPE_NNODETABLE_CMDCODE_STARTSAMPLENNTID		0x02
    #define CMD_CMDTYPE_NNODETABLE_CMDCODE_READVALUENNTINDEX	0x03
    #define CMD_CMDTYPE_NNODETABLE_CMDCODE_CHANGEVALUENNTINDEX	0x04
    #define CMD_CMDTYPE_NNODETABLE_CMDCODE_STARTSAMPLENNTINDEX	0x05
        
        // stored procedure-related
    #define CMD_CMDTYPE_STOREDPROC							'S'
        
    #define CMD_CMDTYPE_STOREDPROC_CMDCODE_READSP			0x00
    #define CMD_CMDTYPE_STOREDPROC_CMDCODE_TRIGGERSP		0x01
        
        // PWM-related
    #define CMD_CMDTYPE_PWMDIM								'P'
        
    #define CMD_CMDTYPE_PWMDIM_CMDCODE_READCHANNEL			0x00
    #define CMD_CMDTYPE_PWMDIM_CMDCODE_ALLCHANNELS			0x01
    #define CMD_CMDTYPE_PWMDIM_CMDCODE_SINGLECHANNEL		0x02
    #define CMD_CMDTYPE_PWMDIM_CMDCODE_MASKOFCHANNELS		0x03
    #define CMD_CMDTYPE_PWMDIM_CMDCODE_SCENECTRL			0x04
    #define CMD_CMDTYPE_PWMDIM_CMDCODE_SCENESAVE			0x05
        
        // DIO-related
    #define CMD_CMDTYPE_IOCTRL								'O'
        
    #define CMD_CMDTYPE_IOCTRL_CMDCODE_READCHANNEL				0x00
    #define CMD_CMDTYPE_IOCTRL_CMDCODE_SINGLECHANNEL			0x01
    #define CMD_CMDTYPE_IOCTRL_CMDCODE_SINGLECHANNELONDELAY		0x02
    #define CMD_CMDTYPE_IOCTRL_CMDCODE_SINGLECHANNELPULSE		0x03
    #define CMD_CMDTYPE_IOCTRL_CMDCODE_MASKOFCHANNELS			0x04
    #define CMD_CMDTYPE_IOCTRL_CMDCODE_MASKOFCHANNELSONDELAY	0x05
    #define CMD_CMDTYPE_IOCTRL_CMDCODE_MASKOFCHANNELSPULSE		0x06
        
        // identification-related
    #define CMD_CMDTYPE_IDENTIFY							'I'
        
    #define IDENTIFYCMD_BLINKTIMES							10
    #define CMD_CMDTYPE_IDENTIFY_CMDCODE_ALLPWMCH			0x00
    #define CMD_CMDTYPE_IDENTIFY_CMDCODE_MASKPWMCH			0x01
    #define CMD_CMDTYPE_IDENTIFY_CMDCODE_ALLPWMCHWRPT		0x02
    #define CMD_CMDTYPE_IDENTIFY_CMDCODE_MASKPWMCHWRPT		0x03
    #define CMD_CMDTYPE_IDENTIFY_CMDCODE_REGIRPT			0x04
    #define CMD_CMDTYPE_IDENTIFY_CMDCODE_DATARPT			0x05
        
        // calibration-related
    #define CMD_CMDTYPE_CALIBRATION							'c'
    #define CMD_CMDTYPE_LINEAR								'l'
        
    #define CMD_CMDTYPE_CALIBRATION_CMDCODE_ADREAD			0x00
    #define CMD_CMDTYPE_CALIBRATION_CMDCODE_CADREAD			0x01
    #define CMD_CMDTYPE_CALIBRATION_CMDCODE_CALPOINT		0x02
    #define CMD_CMDTYPE_CALIBRATION_CMDCODE_CALINDEX		0x03
        
    #define CMD_CMDTYPE_LINEAR_CMDCODE_ADREAD				0x00
    #define CMD_CMDTYPE_LINEAR_CMDCODE_LADREAD				0x01
    #define CMD_CMDTYPE_LINEAR_CMDCODE_LINPOINT				0x02
    #define CMD_CMDTYPE_LINEAR_CMDCODE_LININDEX				0x03



    typedef struct
    {
        BYTE bSendPacket;				// whether this cmd is to be sent out
        BYTE CmdType;
        BYTE CmdCode;
        BYTE CmdFlag;
        BYTE DestIDType;
        BYTE DestIDLen;
        uint64 DestID;
        BYTE NumParams;
        BYTE ParamLen;				// length of each parameter
        uint64 Params[MAX_PARAMS_IN_CMD];
        BYTE CmdLen;
        // CmdCode is a translated code for running this command, many commands may
        // share the same command code, as the processing of the commands is the same
        UINT CmdRealCode;
        // only used in string command
        UINT CmdStrStart;
    }tsCommand;
    
    typedef struct
    {
        BYTE CmdType;
        BYTE CmdCode;
        BYTE NumParams;
        ULONG Params[MAX_PARAMS_IN_CMD];
        BYTE ParamLen;			// length of each parameter
    }tsCommandBuilder;


    static string DECODE(const char *method, string &parse_buff, Http_ParseObj *pub_obj, string &to_obj);
    static string ENCODE(vector<string> &paras, string &func_return);
    static tsCommand ParseCommand(BYTE *CmdBuf);
    static BYTE BuildCommand(BYTE *CmdBuf, tsCommand*CmdBuilder);
    static BYTE BuildStringCommand(BYTE *CmdBuf, char *CmdStrBuf);
    static size_t ParseStringCommand(BYTE *CmdBuf, char *CmdStrBuf); //修改BYTE ParseStringCommand(BYTE *CmdBuf, char *CmdStrBuf);方式
    
    //static string LEN(vector<string> &paras, string &func_return);
    //static string FUNC(vector<string> &paras, string &func_return);
};

//ParseExt_MSHINET GB_parse_mshinet;

// Static, Public / RAM
//---------------------------------------------------------------------------
const unsigned char CRC_TABLE[256] = {
	  0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
      157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
      35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98 ,
      190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
      70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7 ,
      219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
      101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
      248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
      140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
       17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
      175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
       50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
      202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
       87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
      233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
      116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53,
}; 


#endif


