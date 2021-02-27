#include "ConnSSL.h"
#include <iostream>
//#include "STRING_Helper.h"
#ifdef ADD_OPENSSL
    #include <openssl/ssl.h>
    #include <openssl/err.h>
    #include <openssl/rand.h>
    #include "openssl_hostname_validation.h"
#endif
#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>
#include <stdio.h>
#include <string.h>
#include "ParseXml.h"
using namespace std;

/*extern "C"
{
#include "openssl_hostname_validation.h"
}*/

void ConnSSL::CONN_PROTOCOL_ENV_INIT() {
#ifdef ADD_OPENSSL
	#if OPENSSL_VERSION_NUMBER < 0x10100000L
		// Initialize OpenSSL
		SSL_library_init();
		ERR_load_crypto_strings();
		SSL_load_error_strings();
		OpenSSL_add_all_algorithms();
		//conn_protocal_env_inited=true;
	#endif
#endif
}
void ConnSSL::CONN_PROTOCOL_ENV_CLOSE() {
#ifdef ADD_OPENSSL
	#if OPENSSL_VERSION_NUMBER < 0x10100000L
		EVP_cleanup();
		ERR_free_strings();

	#ifdef EVENT__HAVE_ERR_REMOVE_THREAD_STATE
		ERR_remove_thread_state(NULL);
	#else
		ERR_remove_state(0);
	#endif
		CRYPTO_cleanup_all_ex_data();

		sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
	#endif /*OPENSSL_VERSION_NUMBER < 0x10100000L */
#endif
}

ConnSSL::ConnSSL(int server_client) {
	this->ctx = NULL;
	this->ssl = NULL;
	this->type= server_client;

	this->certificate_chain = THIS_CERT_FILE;
	this->private_key = THIS_KEY_FILE;
#ifdef ADD_OPENSSL
	if (server_client==CONN_SSL_CTX_METHOD_CLIENT_HTTPS){
		this->ca_cert="/etc/ssl/certs/ca-certificates.crt";
		this->CTX_Init_Client_HTTPS();
	}else{
		this->ca_cert = CA_CERT_FILE;
		this->CTX_Init(NULL, NULL, server_client);
	}
#endif
}

ConnSSL::ConnSSL(int server_client,const char *host,const char *cert) {
	this->ctx = NULL;
	this->ssl = NULL;
	this->type= server_client;

	this->certificate_chain = THIS_CERT_FILE;
	this->private_key = THIS_KEY_FILE;
#ifdef ADD_OPENSSL
	if (server_client==CONN_SSL_CTX_METHOD_CLIENT_HTTPS){
		if (cert==NULL)
			this->ca_cert="/etc/ssl/certs/ca-certificates.crt";
		else
			this->ca_cert=cert;
		this->CTX_Init_Client_HTTPS(host);
		//this->ssl=this->CTX_Client_HTTP_SetSSL(host); //ly memleak2 del
	}else{
		this->ca_cert = CA_CERT_FILE;
		this->CTX_Init(NULL, NULL, server_client);
	}
#endif
}

ConnSSL::ConnSSL(const char *this_cert, const char *private_key, int server_client, char* password){
	this->ctx=NULL;
	this->ssl = NULL;
	this->ca_cert = CA_CERT_FILE;
	this->certificate_chain = THIS_CERT_FILE;
	this->private_key = THIS_KEY_FILE;
	this->type=server_client;
#ifdef ADD_OPENSSL
	this->CTX_Init(this_cert,private_key,server_client,password);
#endif
}

#ifdef ADD_OPENSSL

ConnSSL::ConnSSL(int server_client,const char *host,ParseXml_Node cfg_node) {
    this->ctx = NULL;
    this->ssl = NULL;
    this->type= server_client;
    if (server_client==CONN_SSL_CTX_METHOD_CLIENT){
        this->CTX_Init_CfgXml(host,cfg_node);
        //this->ssl=this->CTX_Client_HTTP_SetSSL(host); //ly memleak2;
    }else{
        cout << "Error: ConnSSL::ConnSSL(,ParseXml_Node cfg_node) not support CONN_SSL_CTX_METHOD_CLIENT_HTTPS!" << endl;
        this->ca_cert = CA_CERT_FILE;
        this->CTX_Init(NULL, NULL, server_client);
    }
}

ConnSSL::~ConnSSL(){
	if (this->ctx!=NULL)
		SSL_CTX_free (this->ctx);
}

int ConnSSL::CTX_Init(const char *cert, const char *key, int server_client, char* password) {
	int err=0,stat=0;
	this->ctx=NULL;
	this->ssl = NULL;
	this->type=server_client;

	//user defined
	if (cert!=NULL)
		this->certificate_chain=cert;
	if (key!=NULL)
		this->private_key=key;
	cout << "SSL_CTX_Init() start...." << endl;

	//Create CTX
	ctx = SSL_CTX_new(server_client==CONN_SSL_CTX_METHOD_SERVER ? TLSv1_2_server_method(): SSLv23_client_method());		//version 2.1
	//temp up: ctx = SSL_CTX_new(server_client==CONN_SSL_CTX_METHOD_SERVER ? SSLv23_server_method() : SSLv23_client_method());		//version 2.1
	if (!ctx)
		return -9;

//may be in 2.0.0 version
	//是否要求校验对方证书 此处不验证客户端身份所以为： SSL_VERIFY_NONE
	//SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

//new 2.1.1 version
	SSL_CTX_set_options(ctx,
		SSL_OP_SINGLE_DH_USE |
		SSL_OP_SINGLE_ECDH_USE |
		SSL_OP_NO_SSLv2);

	/* Cheesily pick an elliptic curve to use with elliptic curve ciphersuites.
	* We just hardcode a single curve which is reasonably decent.
	* See http://www.mail-archive.com/openssl-dev@openssl.org/msg30957.html */
	EC_KEY *ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
	if (!ecdh)
	{
		err=-1;	//die_most_horribly_from_openssl_error("EC_KEY_new_by_curve_name");
		this->ShowError("EC_KEY_new_by_curve_name");
		goto end;
	}

	if (1 != SSL_CTX_set_tmp_ecdh(ctx, ecdh)){
		err=-2;
		this->ShowError("SSL_CTX_set_tmp_ecdh");
		goto end;
	}//die_most_horribly_from_openssl_error("SSL_CTX_set_tmp_ecdh");

//all 2.00 2.10 version
	//Loading CA certificate  // 加载CA的证书
	if(this->ca_cert!=NULL && !SSL_CTX_load_verify_locations(ctx, this->ca_cert, NULL)){
		err=-3;
		this->ShowError("SSL_CTX_load_verify_locations error!");
		goto end;
	}
	//cout << this->ca_cert << endl;

	// 设置证书文件的口令
	if (password!=NULL)
	    SSL_CTX_set_default_passwd_cb_userdata(ctx, password);

	// 加载自己的证书   //Loading server certificate_chain
	if (1 != (stat=SSL_CTX_use_certificate_chain_file (ctx, this->certificate_chain))){
		err=-3;
		ERR_print_errors_fp(stdout);
		this->ShowError("SSL_CTX_use_certificate_chain_file error!",stat); //arg2: NULL or SSL_FILETYPE_ASN1
		goto end;
	}

	//加载自己的私钥  私钥的作用是，ssl握手过程中，对客户端发送过来的随机 //消息进行加密，然后客户端再使用服务器的公钥进行解密，若解密后的原始消息跟  //客户端发送的消息一直，则认为此服务器是客户端想要链接的服务器
	if (1 != (stat=SSL_CTX_use_PrivateKey_file (ctx, this->private_key, SSL_FILETYPE_PEM))){
		err=-5;
		ERR_print_errors_fp(stdout);
		this->ShowError("SSL_CTX_use_PrivateKey_file",stat);
		goto end;
	}	//

	// 判定私钥是否正确
	if (1 != (stat=SSL_CTX_check_private_key (ctx))){
		err=-6;
		ERR_print_errors_fp(stdout);
		this->ShowError("SSL_CTX_check_private_key",stat);
		goto end;
	} 	//die_most_horribly_from_openssl_error ("SSL_CTX_check_private_key");

end:
	if (err!=0)
		SSL_CTX_free (ctx);

	cout << "SSL_CTX_Init() sucdessed!" << endl;
    
	return err;
}

static string ssl_error_string( )
{
	//SSL_get_error();
	unsigned long ulErr = ERR_get_error();  // 获取错误号
	char szErrMsg[1024] = {0};
	char *pTmp = NULL;
	pTmp = ERR_error_string(ulErr,szErrMsg); // 格式：error:errId:库:函数:原因
	return szErrMsg;
}

/* See http://archives.seul.org/libevent/users/Jan-2013/msg00039.html */
static int ConnSSL_CertVerifyCallback(X509_STORE_CTX *x509_ctx, void *arg)
{
	char cert_str[256];
	const char *host = (const char *) arg;
	const char *res_str = "X509_verify_cert failed";
	HostnameValidationResult res = Error;
    //cout << "ConnSSL_CertVerifyCallback(x509)!...." << endl;

	/* This is the function that OpenSSL would call if we hadn't called
	 * SSL_CTX_set_cert_verify_callback().  Therefore, we are "wrapping"
	 * the default functionality, rather than replacing it. */
	int ok_so_far = 0;
	X509 *server_cert = NULL;
	if (ConnSSL_bIgnoreCert) {
		return 1;
	}

	ok_so_far = X509_verify_cert(x509_ctx);
	server_cert = X509_STORE_CTX_get_current_cert(x509_ctx);
	if (ok_so_far != X509_V_OK && ok_so_far != X509_V_ERR_CERT_UNTRUSTED && ok_so_far != X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY)
		{
			std::cout<<ssl_error_string().c_str()<<endl;
			return 0;
		}
    //cout << "509 verify cert()!"
	res = validate_hostname(host, server_cert);
	switch (res) {
	case MatchFound:
		res_str = "MatchFound";
		break;
	case MatchNotFound:
		res_str = "MatchNotFound";
		break;
	case NoSANPresent:
		res_str = "NoSANPresent";
		break;
	case MalformedCertificate:
		res_str = "MalformedCertificate";
		break;
	case Error:
		res_str = "Error";
		break;
	default:
		res_str = "WTF!";
		break;
	}

	X509_NAME_oneline(X509_get_subject_name (server_cert),
			  cert_str, sizeof (cert_str));

	if (res == MatchFound) {
		printf("https server '%s' has this certificate, "
		       "which looks good to me:\n%s\n",
		       host, cert_str);
		return 1;
	} else{
		fprintf(stderr, "WARNING! ssl verify failed: %d \n", ok_so_far);
		std::cout<<ssl_error_string().c_str()<<endl;
		ERR_print_errors_fp(stderr);
		printf("Got '%s' for hostname '%s' and certificate: something error!\n",res_str, host);
		return 0;
	} /*else {
		printf("Got '%s' for hostname '%s' and certificate:\n%s\n",
		       res_str, host, cert_str);
		return 0;
	}*/
}

int ConnSSL::CTX_Init_CfgXml(const char *host,ParseXml_Node cfg_node) {
//!!!may this fucn can't for Win32;
	int err=0,r=0;
	this->ctx=NULL;
	this->ssl = NULL;

	/* This isn't strictly necessary... OpenSSL performs RAND_poll
		 * automatically on first use of random number generator. */
	r = RAND_poll();
	if (r == 0) {
		this->ShowError("RAND_poll");
		return-8;
	}

	/* Create a new OpenSSL context */
	if (this->type==CONN_SSL_CTX_METHOD_SERVER){
		this->ShowError("SSL_CTX_new() NOT SUPPOR SERVER MODE !");
		return-10;
	}

	const char *ctx_method=cfg_node.child_value("Method");
	if (ctx_method!=NULL && strcmp(ctx_method,"TLSv1")==0){
		this->ctx = SSL_CTX_new(TLSv1_client_method());
	}else
		this->ctx = SSL_CTX_new(SSLv23_client_method());
	if (!this->ctx) {
		this->ShowError("SSL_CTX_new() error!");
		return-9;
	}

#ifndef _WIN32
		//ca_cert_root
		const char *do_cert_verify_cb_str;
		this->ca_cert=cfg_node.child_value("CaCert");
		if (this->ca_cert!=NULL && this->ca_cert[0]!=0){
			if (1 != SSL_CTX_load_verify_locations(this->ctx, this->ca_cert, NULL)) {
				err=-1;	//die_most_horribly_from_openssl_error("EC_KEY_new_by_curve_name");
				ERR_print_errors_fp(stdout);
				this->ShowError("ConnSSL()-> SSL_load_verify_locations CaCert_Error!");
				goto end;
			}
		}

		// certificate_chain
		this->certificate_chain=cfg_node.child_value("PrivateCert");
		if (this->certificate_chain!=NULL && this->certificate_chain[0]!=0){
			if (SSL_CTX_use_certificate_file(this->ctx, this->certificate_chain, SSL_FILETYPE_PEM) <= 0){
				err=-1;	//die_most_horribly_from_openssl_error("EC_KEY_new_by_curve_name");
				ERR_print_errors_fp(stdout);
				this->ShowError("ConnSSL()-> load Private cert_chain(private cert) error!");
				goto end;
			}
		}
		//char keyword1[]="1234";
		this->password=cfg_node.child_value("PrivatePwd");
		if (this->password!=NULL && this->password[0]!=0)
			SSL_CTX_set_default_passwd_cb_userdata(this->ctx,(void *)this->password);

		this->private_key=cfg_node.child_value("PrivateKey");
		if (this->private_key!=NULL && this->private_key[0]!=0){
			if (SSL_CTX_use_PrivateKey_file(this->ctx, this->private_key, SSL_FILETYPE_PEM) <= 0){
				err=-1;	//die_most_horribly_from_openssl_error("EC_KEY_new_by_curve_name");
				ERR_print_errors_fp(stdout);
				this->ShowError("ConnSSL()->load private key error!");
				goto end;
				}
		}
		// make sure the key and certificate file match
		if (SSL_CTX_check_private_key(this->ctx) == 0){
			err=-1;	//die_most_horribly_from_openssl_error("EC_KEY_new_by_curve_name");
			ERR_print_errors_fp(stdout);
			this->ShowError("ConSSL()->private_key not match certificate!");
			goto end;
		}

		do_cert_verify_cb_str=cfg_node.child_value("CertVerifyCB");
		if (do_cert_verify_cb_str!=NULL && strcmp(do_cert_verify_cb_str,"true")==0)
			ConnSSL_bIgnoreCert=false;
		if (strcmp(do_cert_verify_cb_str,"false")==0)
			ConnSSL_bIgnoreCert=true;

		SSL_CTX_set_cert_verify_callback(this->ctx, ConnSSL_CertVerifyCallback,(void *)host);
	end:
#endif
	
	if (err!=0){
		SSL_CTX_free (this->ctx);
		this->ctx=NULL;
	}
	return err;
}

int ConnSSL::CTX_Init_Client_HTTPS(const char *host,const char *cert) {
//!!!may this fucn can't for Win32;
	int err=0,r=0;
	this->ctx=NULL;
	//this->ssl = NULL;

	//user defined
	if (cert!=NULL)
		this->ca_cert=cert;

	/* This isn't strictly necessary... OpenSSL performs RAND_poll
	 * automatically on first use of random number generator. */
	r = RAND_poll();
	if (r == 0) {
		this->ShowError("RAND_poll");
		return -8;
	}

	/* Create a new OpenSSL context */
	this->ctx = SSL_CTX_new(SSLv23_client_method());
	if (!this->ctx) {
		this->ShowError("SSL_CTX_new() error!");
		return -9;
	}

#ifndef _WIN32
	/* TODO: Add certificate loading on Windows as well */

	/* Attempt to use the system's trusted root certificates.
	 * (This path is only valid for Debian-based systems.) */
	if (1 != SSL_CTX_load_verify_locations(this->ctx, this->ca_cert, NULL)) {
		err=-1;	//die_most_horribly_from_openssl_error("EC_KEY_new_by_curve_name");
		ERR_print_errors_fp(stdout);
		this->ShowError("SSL_CTX_load_verify_locations");
		goto end;
	}
	/* Ask OpenSSL to verify the server certificate.  Note that this
		 * does NOT include verifying that the hostname is correct.
		 * So, by itself, this means anyone with any legitimate
		 * CA-issued certificate for any website, can impersonate any
		 * other website in the world.  This is not good.  See "The
		 * Most Dangerous Code in the World" article at
		 * https://crypto.stanford.edu/~dabo/pubs/abstracts/ssl-client-bugs.html
		 */
	SSL_CTX_set_verify(this->ctx, SSL_VERIFY_PEER, NULL);
	/* This is how we solve the problem mentioned in the previous
	 * comment.  We "wrap" OpenSSL's validation routine in our
	 * own routine, which also validates the hostname by calling
	 * the code provided by iSECPartners.  Note that even though
	 * the "Everything You've Always Wanted to Know About
	 * Certificate Validation With OpenSSL (But Were Afraid to
	 * Ask)" paper from iSECPartners says very explicitly not to
	 * call SSL_CTX_set_cert_verify_callback (at the bottom of
	 * page 2), what we're doing here is safe because our
	 * cert_verify_callback() calls X509_verify_cert(), which is
	 * OpenSSL's built-in routine which would have been called if
	 * we hadn't set the callback.  Therefore, we're just
	 * "wrapping" OpenSSL's routine, not replacing it. */
	SSL_CTX_set_cert_verify_callback(this->ctx, ConnSSL_CertVerifyCallback,(void *) host);
end:
#endif

	if (err!=0){
		SSL_CTX_free (this->ctx);
		this->ctx=NULL;
	}

	return err;
}

SSL *ConnSSL::CTX_Client_HTTP_SetSSL(const char *host){
	if (this->ctx==NULL && strlen(host)==0){
		return NULL;
	}

	// Create OpenSSL bufferevent and stack evhttp on top of it
	this->ssl = SSL_new(this->ctx);
	if (ssl == NULL) {
		return NULL;
	}//ly memleak

#ifdef SSL_CTRL_SET_TLSEXT_HOSTNAME
	// Set hostname for SNI extension
	SSL_set_tlsext_host_name(this->ssl, host);
#endif
	return this->ssl;
}

struct bufferevent *ConnSSL::bufferevent_ssl_socket_new(struct event_base  *base, evutil_socket_t  fd, int state,const char *host) {
	if (this->ctx==NULL)
		return NULL;
	// Create OpenSSL bufferevent and stack evhttp on top of it
	this->ssl = SSL_new(this->ctx);
	if (this->ssl == NULL) {
		return NULL;
	}
	struct bufferevent *bev;
	if (state==CONN_SSL_ACCEPTING){
		bev=bufferevent_openssl_socket_new(base, fd, this->ssl,BUFFEREVENT_SSL_ACCEPTING, BEV_OPT_CLOSE_ON_FREE);
	}else if (state==CONN_SSL_CONNECTING){
		/*#ifdef SSL_CTRL_SET_TLSEXT_HOSTNAME
			// Set hostname for SNI extension
			SSL_set_tlsext_host_name(this->ssl, ""); //ly memleak2 add
		#endif*/
		bev=bufferevent_openssl_socket_new(base, fd, this->ssl,BUFFEREVENT_SSL_CONNECTING, BEV_OPT_CLOSE_ON_FREE);
		/*if (bev!=NULL){
			bufferevent_openssl_set_allow_dirty_shutdown(bev, 1);
		}*/
	}
	return bev;
}

/*
struct bufferevent *ConnSSL::bufferevent_ssl_socket_new(struct event_base  *base, evutil_socket_t  fd, int state,const char *host) {
	if (this->ctx==NULL)
		return NULL;
	// Create OpenSSL bufferevent and stack evhttp on top of it
//	this->ssl = SSL_new(this->ctx);
//	if (this->ssl == NULL) {
//		return NULL;
//	} //ly memleak
	struct bufferevent *bev;
	if (state==CONN_SSL_ACCEPTING){
		this->ssl = SSL_new(this->ctx);
		if (this->ssl == NULL)
				return NULL;

		bev=bufferevent_openssl_socket_new(base, fd, this->ssl,BUFFEREVENT_SSL_ACCEPTING, BEV_OPT_CLOSE_ON_FREE);
	}else if (state==CONN_SSL_CONNECTING){

		if (this->ssl==NULL)
			this->ssl = SSL_new(this->ctx);//ly memleak;
		if (this->ssl == NULL)
				return NULL;

		bev=bufferevent_openssl_socket_new(base, fd, this->ssl,BUFFEREVENT_SSL_CONNECTING, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);
		if (bev!=NULL){
			bufferevent_openssl_set_allow_dirty_shutdown(bev, 1);
		}
	}
	return bev;
}*/

struct bufferevent *ConnSSL::bufferevent_ssl_filter_new(struct event_base *base,struct bufferevent *underlying,int state) {
	if (this->ctx==NULL)
		return NULL;
	// Create OpenSSL bufferevent and stack evhttp on top of it
	this->ssl = SSL_new(this->ctx);
	if (this->ssl == NULL) {
		return NULL;
	}
	if (state!=CONN_SSL_CONNECTING)
		return NULL;

	struct bufferevent *b_ssl = bufferevent_openssl_filter_new(base,underlying, this->ssl, BUFFEREVENT_SSL_CONNECTING,BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);
	if (b_ssl!=NULL)
		bufferevent_openssl_set_allow_dirty_shutdown(b_ssl, 1);

	return b_ssl;
}

void ConnSSL::ShutDownSSL(struct bufferevent *bev){//ly memleak2
	SSL *ssl =bufferevent_openssl_get_ssl(bev);
	if (ssl!=NULL){
		cout <<" doing ssl shutdown...." <<endl;
		SSL_set_session(ssl,NULL);
		//SSL_shutdown (conn_ssl->ssl);
		SSL_set_shutdown(ssl, SSL_RECEIVED_SHUTDOWN);
		SSL_shutdown(ssl);
	}
}

#else     //#ifdef ADD_OPENSSL

ConnSSL::~ConnSSL() {
    this->ctx = NULL;
}

struct bufferevent *ConnSSL::bufferevent_ssl_socket_new(struct event_base *base, evutil_socket_t  fd, int state,const char *host)
{
    return bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
};

void ConnSSL::ShutDownSSL(struct bufferevent *bev){
	return;
}

struct bufferevent *ConnSSL::bufferevent_ssl_filter_new(struct event_base *base,struct bufferevent *underlying,int state)
{
    cout << "error: system not support bufferevent_ssl_filter_new!" << endl;
    return NULL;
};

#endif  //#ifdef ADD_OPENSSL

void ConnSSL::ShowError(const char *err_str,int stat){
	cout <<"ConnErr: "<< err_str;
	if (stat!=0)
		cout << " stat=" << stat;
	cout << endl;
}
