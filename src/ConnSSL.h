#ifndef CONN_SSL_
#define CONN_SSL_
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(OPEN_WRT) && !defined(__ANDROID__) && !defined(__APPLE__)
    #define ADD_OPENSSL
#endif
#include <event2/util.h>
#ifdef ADD_OPENSSL
#ifndef EVENT__HAVE_OPENSSL
	#define EVENT__HAVE_OPENSSL
#endif
    #include <openssl/ssl.h>
    #include <openssl/err.h>
    #include <openssl/rand.h>
    #include "ParseXml.h"
#endif


#ifndef ADD_OPENSSL
    typedef void SSL_CTX;
    typedef void SSL;
#endif


//namespace std;
static bool ConnSSL_bIgnoreCert =true;

class ConnSSL
{
/*#define CA_CERT_FILE 	"certificate-authorities.pem" //"./ssl/d54495fd73b79723.crt"		//
#define THIS_CERT_FILE "server-certificate-chain.pem" //	"./ssl/gd_bundle-g2-g1.crt"  	//
#define THIS_KEY_FILE  "server-private-key.pem"  //"./ssl/mshinet.key"   // "./ssl/unmshinet.key"      //*/

/*#define CA_CERT_FILE 	"./ssl_local/server.crt"		//
#define THIS_CERT_FILE "./ssl_local/ca.crt"  	//
#define THIS_KEY_FILE  "./ssl_local/server.key"      //*/

#define CA_CERT_FILE "ssl/gd_bundle-g2-g1.crt"  	//
#define THIS_CERT_FILE "ssl/d54495fd73b79723.crt"		//
#define THIS_KEY_FILE  "ssl/mshinet.key"   // "./ssl/unmshinet.key"

private:
	const char *ca_cert;
	const char *certificate_chain;
	const char *private_key;
	const char *password;

public:
	SSL_CTX *ctx;
	SSL  *ssl;
	enum{
		CONN_SSL_CTX_METHOD_SERVER=1,
		CONN_SSL_CTX_METHOD_CLIENT=2,
		CONN_SSL_CTX_METHOD_CLIENT_HTTPS=3,
		CONN_SSL_OPEN = 0,
		CONN_SSL_CONNECTING = 1,
		CONN_SSL_ACCEPTING = 2,
	  };

	static void CONN_PROTOCOL_ENV_INIT();
	static void CONN_PROTOCOL_ENV_CLOSE();

	ConnSSL(int server_client);
	ConnSSL(int server_client,const char *host,const char *cert=NULL);
	ConnSSL(const char *this_cert, const char *private_key, int server_client, char* password);
    
#ifdef ADD_OPENSSL
    ConnSSL(int server_client,const char *host,ParseXml_Node cfg_node);
	int CTX_Init(const char *this_cert, const char *private_key, int server_client = CONN_SSL_CTX_METHOD_SERVER, char* password = NULL);

	int CTX_Init_Client_HTTPS(const char *host=NULL,const char *cert=NULL);
	int CTX_Init_Client_APNS(const char *host=NULL,const char *cert=NULL);
	int CTX_Init_CfgXml(const char *host,ParseXml_Node cfg_node);
	SSL *CTX_Client_HTTP_SetSSL(const char *host);
#endif
    ~ConnSSL();
	struct bufferevent *bufferevent_ssl_socket_new(struct event_base *base, evutil_socket_t  fd, int state,const char *host=NULL);
	struct bufferevent *bufferevent_ssl_filter_new(struct event_base *base,struct bufferevent *underlying,int state);
	//SSL * bufferevent_openssl_get_ssl(struct bufferevent *bev);
	void ShutDownSSL(struct bufferevent *bev);//ly memleak;
	void ShowError(const char*, int stat = 0);
	int type;
};

#endif
