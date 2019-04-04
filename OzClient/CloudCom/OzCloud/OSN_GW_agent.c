#ifdef WIN32
#define OSN_API __stdcall
#ifdef OSNCLIENT_EXPORTS
#define OSN_DECLSPEC __declspec(dllexport)
#else
#define OSN_DECLSPEC __declspec(dllimport)
#endif
#else
#define OSN_API
#define OSN_DECLSPEC
#endif
#ifdef __cplusplus
#define OSN_EXTERN extern "C" OSN_DECLSPEC
#else
#define OSN_EXTERN extern OSN_DECLSPEC
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* Description */

/* Include */
#include <string.h>
#include <stdio.h> 
#include <time.h> //createTimeStamp

#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "curl/curl.h"
#include "curl/easy.h"

#include "OSN_GW_agent.h"
#include "urlcode.h"
#include "base64.h"
#include "sha256.h"

/* Define */

#define OSN_GW_TIMESTAMP_SIZE       24  // "2014-02-20T10:00:00+0200"
#define OSN_WS_MAX_SIGN_LEN       1024
#define OSN_WS_MAX_BIG_SIGN_LEN   (64*OSN_WS_MAX_SIGN_LEN)  // MAX Length device status for NVR 64CH
#define SHARED_SECRET_SIZE         35

static const char* ss_1 = "Ozvision";
static const char* ss_2 = "client-cloud";
static const char* ss_3 = "protocol";

static char* shared_secret = NULL;

/* External reference */

/* Enum & type declarations */
typedef struct osn_gw_agent_params
{

	char* user_name; 
	char* user_password;
	char* organization_code;
	char* urlName;
	char* hostName;
	char* service;
	char* client_id;
	char* protocol_ver;
	unsigned int majorApiVersion;
	unsigned int minorApiVersion;
	int   urlPort;
	// below variables must be per thread
	char  iso_8601_time[GW_NUMBER_OF_THREADS][OSN_GW_TIMESTAMP_SIZE+1];
	CURL *curl[GW_NUMBER_OF_THREADS];
	curl_socket_t curlSockfd[GW_NUMBER_OF_THREADS];
} client_gw_params_t;

static client_gw_params_t clientGwParams_g;

/* Static function prototypes */
//static CURLcode sslctx_function(CURL *curl, void *sslctx, void *parm);
static int CreateTimestamp(osn_gw_threads_e threadId);
static int FindNumLength(int num);
static int WaitOnSocket(int sockfd, int forRecv, long timeout_ms);

/***********************************************************
  Description: Auxiliary function that waits on the socket.

  Parameters :

  Return     :
 **********************************************************/
static int WaitOnSocket(int sockfd, int forRecv, long timeout_ms)
{
  struct timeval tv;
  fd_set infd, outfd, errfd;
  int res;

  tv.tv_sec = timeout_ms / 1000;
  tv.tv_usec= (timeout_ms % 1000) * 1000;
  FD_ZERO(&infd);
  FD_ZERO(&outfd);
  FD_ZERO(&errfd);

  FD_SET(sockfd, &errfd); /* always check for error */

  if(forRecv)
    FD_SET(sockfd, &infd);
  else
    FD_SET(sockfd, &outfd);

  /* select() returns the socket number or -1 */
  res = select(sockfd + 1, &infd, &outfd, &errfd, &tv);

  return res;
}

/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
static int CreateTimestamp(osn_gw_threads_e threadId)
{
    unsigned int size;
    time_t rawtime;
    struct tm * timeinfo;

    rawtime = time(NULL);
    timeinfo = gmtime (&rawtime);

    size = strftime (clientGwParams_g.iso_8601_time[threadId],
            sizeof(clientGwParams_g.iso_8601_time[threadId]), "%FT%T%z",timeinfo);
    if(size == OSN_GW_TIMESTAMP_SIZE)
    {
		OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "Msg timestamp = %s(%d)",
                clientGwParams_g.iso_8601_time[threadId],rawtime);
    	return OZ_OK;
    }

    return OZ_ERROR;
}
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
static int FindNumLength(int num)
{
    int n = 0;

    while(num)
    {
        num /= 10;
        n++;
    }
    return n;
}
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
/*static int ConvertDataToJson(const char *data, char *dataJson, char eventType,
        osn_gw_action_e action, osn_gw_threads_e threadId)
{
    int maxLen = (action == UPLOAD_ADDRESS) ? OSN_GW_FILE_NAME_SIZE/2 \
                                            : OSN_GW_DEVICE_ID_SIZE;
    
    if(action == UPDATE_STATUS)
    {
        sprintf(dataJson, "%s", data);
    }
    else
    {
        if(data == NULL || strlen(data) > maxLen) return -1;


        if (action == UPLOAD_ADDRESS)
        {
            if(threadId == GW_THREADS_LOG || eventType == ' ' )
            {
                sprintf(dataJson, "{\"file_path\":\"%s\"}",data);
            }
            else
            {
                sprintf(dataJson, "{\"file_path\":\"%s\",\"metadata\": \
                        {\"event_type\":\"%c\",\"device_name\":\"%s\"}}",
                        data, eventType, clientGwParams_g.user_name );
            }
        }
        else
        {
            sprintf(dataJson, "{\"device_id\":\"%s\"}", data);
        }
    }
    return 0;
}
*/
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
oz_ret_t OSN_GW_init(const char *user_name, const char *user_password, const char *organization_code, const char* client_id)
{
	int i;
    /* Static variable definitions */
    clientGwParams_g.user_name = NULL;
    clientGwParams_g.user_password = NULL;
    clientGwParams_g.organization_code = NULL;
    clientGwParams_g.urlName = NULL;
    clientGwParams_g.client_id = NULL;

    if(user_name == NULL || user_password == NULL || organization_code == NULL || client_id == NULL)
    {
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "param1, param2, organization_code: one of the parameters is NULL");
	    return OZ_ERROR;
    }
   
    shared_secret = malloc(SHARED_SECRET_SIZE);
    if(!shared_secret)
        return OZ_ERROR;

    sprintf(shared_secret, "%s %s %s", ss_1, ss_2, ss_3);

    if(clientGwParams_g.user_name != NULL) free(clientGwParams_g.user_name);
    clientGwParams_g.user_name = (char*)malloc(strlen(user_name) + 1);
    strcpy(clientGwParams_g.user_name, user_name);

    if(clientGwParams_g.user_password != NULL) free(clientGwParams_g.user_password);
    clientGwParams_g.user_password = (char*)malloc(strlen(user_password) + 1);
    strcpy(clientGwParams_g.user_password, user_password);

    if(clientGwParams_g.organization_code != NULL) free(clientGwParams_g.organization_code);
    clientGwParams_g.organization_code = (char*)malloc(strlen(organization_code) + 1);
    strcpy(clientGwParams_g.organization_code, organization_code);

    if(clientGwParams_g.client_id != NULL) free(clientGwParams_g.client_id);
    clientGwParams_g.client_id = (char*)malloc(strlen(client_id) + 1);
    strcpy(clientGwParams_g.client_id, client_id);

    for(i = 0; i < GW_NUMBER_OF_THREADS; i++) clientGwParams_g.curl[i] = NULL;

    return OZ_OK;
}
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
oz_ret_t OSN_GW_setGateway(const char* hostName, unsigned int port,
        unsigned int majorApiVersion, unsigned int minorApiVersion, const char* service)
{
	unsigned int len;

	// Validation
	if (port > 65535)
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "port > 65535, port number: %d",port);

	if(majorApiVersion != 1 || minorApiVersion != 1)
		OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "API version not supported. major: %d, minor: %d",majorApiVersion, minorApiVersion);

	if(hostName == NULL || service == NULL)
	{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "host_name: %s, service: %s", hostName, service);
		return OZ_ERROR;
	}

	// url_name
	len = strlen(hostName) + strlen(service) + FindNumLength(port) + 1;

	if(clientGwParams_g.urlName != NULL) free(clientGwParams_g.urlName);

	clientGwParams_g.urlName = (char*)malloc(len + 10); // : . 'http://'
	if (clientGwParams_g.urlName == NULL) return OZ_ERROR;

	if(port == 8989) // "rnd-gateway-x.ozsn.net" port, use 'http'
		sprintf(clientGwParams_g.urlName , "http://%s:%d", hostName, port);
	else
		sprintf(clientGwParams_g.urlName , "https://%s:%d", hostName, port);

	// http_post_get & http_post_conf memory
	len += FindNumLength(majorApiVersion) + FindNumLength(minorApiVersion);

	clientGwParams_g.hostName = (char*)malloc(strlen(hostName) + 1);
	if (clientGwParams_g.hostName == NULL) return OZ_ERROR;
	
	clientGwParams_g.service = (char*)malloc(strlen(service) + 1);
	if (clientGwParams_g.service == NULL) return OZ_ERROR;
	
	strcpy(clientGwParams_g.hostName, hostName);
	strcpy(clientGwParams_g.service, service);
	clientGwParams_g.majorApiVersion = majorApiVersion;
	clientGwParams_g.minorApiVersion = minorApiVersion;

	clientGwParams_g.urlPort = port;

	return OZ_OK;
}


/***********************************************************
  Description: create GW api's parameters for sending to GW 

  Parameters : data can be either file_name (for getUploadAddress) or device_id (for rest of api's)

  Return     : parameters string to send. with the http request. must remember to free parameters.
 **********************************************************/
char *OSN_GW_createRequestParameters(const char *dataJson, osn_gw_threads_e threadId, action_owner_e action_owner)
{
	//char dataJson[OSN_GW_FILE_NAME_SIZE + 50];
	char signatureSha256_out[SHA256_OUT_SIZE + 1];
	unsigned int signatureBase64_outLen = 256;
	unsigned int passwordBase64_outLen = 256;
	unsigned int signatureLen = 0;
	unsigned int maxSignatureLen = OSN_WS_MAX_BIG_SIGN_LEN;
	static char *signatureBase64_out = NULL;
	static char *signatureIn = NULL;
	char ident_user[20];
	char passwordSha256_out[SHA256_OUT_SIZE + 1];
	char passwordBase64_out[256];
	char* parameters = NULL;
	const char* prot = "0.0.1"; //for now constant need to see where we get it from
	int size;

	if(action_owner == ACTION_BY_MANAGEMENT)
	{
		strcpy(ident_user, "management_user");
	}
	else // ACTION_BY_USER
	{
		strcpy(ident_user, "user");
	}

	if(signatureBase64_out == NULL) signatureBase64_out = (char*)malloc(signatureBase64_outLen);
	if(signatureBase64_out == NULL) return NULL;

	// Get current time, in ISO8601 format
	CreateTimestamp(threadId);

	if(!clientGwParams_g.user_name || !clientGwParams_g.user_password || !dataJson || !clientGwParams_g.organization_code || !clientGwParams_g.client_id)
		return NULL;

	SHA256_digest(clientGwParams_g.user_password, passwordSha256_out);
	base64encode((const void*)passwordSha256_out,
            SHA256_OUT_SIZE / 2, passwordBase64_out, passwordBase64_outLen);

	// Create a signature from : file name, organization_code, time, id & key & shared secret key
	signatureLen = strlen(dataJson) + strlen(clientGwParams_g.organization_code) + strlen(clientGwParams_g.iso_8601_time[threadId]) +
								 strlen(clientGwParams_g.user_name) + strlen(prot) + 
                                 strlen(clientGwParams_g.client_id) + strlen(shared_secret) + strlen(passwordBase64_out) + 1;

	if(signatureLen > maxSignatureLen)
	{
        printf("signature length is too long");
		return NULL;
	}

	if(signatureIn == NULL) signatureIn = (char*)malloc(maxSignatureLen);
	if(signatureIn == NULL) return NULL;
	
	sprintf(signatureIn, "%s%s%s%s%s%s%s%s",
			clientGwParams_g.organization_code, clientGwParams_g.user_name, prot,  
            clientGwParams_g.iso_8601_time[threadId], passwordBase64_out, 
            clientGwParams_g.client_id, dataJson, shared_secret);

	SHA256_digest(signatureIn, signatureSha256_out);

	size = base64encode((const void*)signatureSha256_out,
            SHA256_OUT_SIZE / 2, signatureBase64_out, signatureBase64_outLen);

	if(size == 0){
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "base64encode return size = 0, cant create get upload address params");
	}
	else
	{
		char* urlDataJson = UrlEncode((char*)dataJson);
		char* urlTime = UrlEncode(clientGwParams_g.iso_8601_time[threadId]);
		char* urlSignature = UrlEncode(signatureBase64_out);
        
		// Create message parameters from : file name, organization_code, time, user_name & signature
		unsigned int parametersLen = 100 + strlen(urlDataJson) + strlen(urlTime) + strlen(urlSignature) \
                                      + strlen(clientGwParams_g.organization_code) + strlen(clientGwParams_g.user_name) + strlen(clientGwParams_g.client_id);

		parameters = (char*)malloc(parametersLen);
        if(parameters != NULL)
        {

            sprintf(parameters ,"data=%s&organization_code=%s&timestamp=%s&%s=%s&client_id=%s&clt_cld_prot_ver=%s&signature=%s",
                    urlDataJson, clientGwParams_g.organization_code, urlTime, ident_user, clientGwParams_g.user_name, clientGwParams_g.client_id, prot, urlSignature);

        }
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "parameters: %s\n", parameters);

		free(urlDataJson);
		free(urlTime);
		free(urlSignature);
	}

	return parameters;
}
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
/*
char*  OSN_WS_deviceGetConfiguration(char* dataJson, osn_gw_threads_e threadId)
{
	return OSN_WS_create_update_device_status_parameters(dataJson, threadId);
}
***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************
char* OSN_WS_create_notification_send_parameters(char* dataJson, OSN_WS_threads_E thread_id)
{
	return OSN_WS_create_update_device_status_parameters(dataJson, thread_id);
}
*/
/***********************************************************
  Description: create WS action.

  Parameters : data - data to send to cloud, inserted inside action

  Return     : action allocated on the heap. NULL if failed
 **********************************************************/
char* OSN_GW_createRequest(const char *action , char* data)
{
	char* requestUrl = NULL;
	char httpRequest[100];
	int dataLen;
	unsigned int requestLen = 0;

	if(data == NULL) return NULL;
	dataLen = strlen(data);

	sprintf(httpRequest, "POST /%d.%d/%s/%s HTTP/1.1\r\nHost: %s:%d\r\n",
			clientGwParams_g.majorApiVersion, clientGwParams_g.minorApiVersion, clientGwParams_g.service, action, clientGwParams_g.hostName, clientGwParams_g.urlPort);
	
	requestLen = strlen(httpRequest) + dataLen + 128; // 'Content-' ...

	requestUrl = (char *)malloc(requestLen);
	if(requestUrl != NULL)
	{
		sprintf(requestUrl ,"%sContent-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n%s",
				httpRequest, dataLen, data);
		requestUrl[requestLen-1] = '\0';
	}

	return requestUrl;
}
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
oz_ret_t OSN_GW_sendRequest(char* request, osn_gw_threads_e threadId)
{
	CURLcode res;
	size_t inlen, outlen;
	long sockextr;
	int i = 0;
	oz_ret_t ret = OZ_OK;

	if(request == NULL) return OZ_ERROR;

    //TODO is this need to be here? why init every send? why not once?
    if(OSN_GW_curlInit(threadId) == -1)
    {
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "curl_init failed");
		//free(request);
		return OZ_ERROR;
    }
    else
    {
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "curl_easy_perform(connect to gateway)");

	    res = curl_easy_perform(clientGwParams_g.curl[threadId]);
	    if(CURLE_OK != res)
	    {
			OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Error sending curl message to gateway %s res=%d errno:%s",
                    clientGwParams_g.urlName, res,(char*)curl_easy_strerror(res));
	        goto error;	
	    }
	    /* Extract the socket from the curl handle - we'll need it for waiting. */
	    res = curl_easy_getinfo(clientGwParams_g.curl[threadId], CURLINFO_LASTSOCKET, &sockextr);
	    if(CURLE_OK != res)
	    {
			OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "curl_get_info failed");
	        goto error;	
	    }
	    clientGwParams_g.curlSockfd[threadId] = sockextr;
wait_tx: // until socket ready - overcome curl error code 81
	    /* Wait for the socket to become ready for sending */
	    if(!WaitOnSocket(clientGwParams_g.curlSockfd[threadId], 0, 150000L))
	    {
			OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "wait on socket failed");
	    	goto error;
	    }
	}
	static int retry = 0;
	/* Send the request */
	inlen = strlen(request);
	res = curl_easy_send(clientGwParams_g.curl[threadId], request, inlen, &outlen);
	if(CURLE_OK != res)
	{
		if(retry > 4)
		{
			retry = 0;
			OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "curl_easy_send failed, errno: %s", (char*)curl_easy_strerror(res));
            goto error;
		}
		else
		{
			retry++;
			OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "curl error=%d retry=%d",res,retry);
			goto wait_tx;
		}
	}
	/* Check to see if all the request has been sent */
	if(inlen != outlen)
	{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "not all the request has been sent, inlen != outlen");
	    goto error;		
    }    
	OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "request has been sent to GW: %s", request);

    retry = 0;

    return ret;

error:
    curl_easy_cleanup(clientGwParams_g.curl[threadId]);
    clientGwParams_g.curl[threadId] = NULL;
    return OZ_ERROR;

}
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
int OSN_GW_recvActionResponse(char* buf, int bufSize, osn_gw_threads_e threadId)
{
	CURLcode res;
	size_t iolen;
	int nread;
	int rc;

    rc = WaitOnSocket(clientGwParams_g.curlSockfd[threadId], 1, 60000L); // 60 sec.
    if (rc <= 0)
    {
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Timeout");
    	nread = 0; // Timeout
    }
    else
    {
    	/* read the response */
    	res = curl_easy_recv(clientGwParams_g.curl[threadId], buf, bufSize, &iolen);
    	if(CURLE_OK != res)
    	{
    		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__,"curl_easy_recv errno[res=%d] %s",res,(char*)curl_easy_strerror(res));
    		nread = 0;
    	}
    	else
    		nread = (int)iolen;
    }

	/* Curl close  process */
    if(clientGwParams_g.curl[threadId] !=NULL)
    {
    	curl_easy_cleanup(clientGwParams_g.curl[threadId]);
    	clientGwParams_g.curl[threadId] = NULL;
    }
    else
    {
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__,"curl_easy_recv errno[res=%d] %s",res,(char*)curl_easy_strerror(res));
    }
	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__,"received json from gateway: %s", buf);

   return nread;
}
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
oz_ret_t OSN_GW_close(void)
{
	free(clientGwParams_g.urlName);
	clientGwParams_g.urlName = NULL;

	free(clientGwParams_g.user_name);
	clientGwParams_g.user_name = NULL;

	free(clientGwParams_g.user_password);
	clientGwParams_g.user_password = NULL;

	free(clientGwParams_g.organization_code);
	clientGwParams_g.organization_code = NULL;
	
	return OZ_OK;
}
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/
int OSN_GW_curlInit(osn_gw_threads_e threadId)
{
    int ret = -1;
    CURLcode res;
    char *cainfo = "ca.bundle.crt";

    /* Curl initialization process */
	if(clientGwParams_g.curl[threadId] != NULL)
	{
        printf("curl session already exist");
		curl_easy_cleanup(clientGwParams_g.curl[threadId]);
        clientGwParams_g.curl[threadId] = NULL;
	}
	clientGwParams_g.curl[threadId] = curl_easy_init();

	if(clientGwParams_g.curl[threadId])
	{
        ret = 0;
		/* enable verbose for easier tracing */
        //if(OZ_LOG_LEVEL == VERBOSE) //TODO uncomment
		curl_easy_setopt(clientGwParams_g.curl[threadId], CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(clientGwParams_g.curl[threadId], CURLOPT_NOSIGNAL, 1);

	curl_easy_setopt(clientGwParams_g.curl[threadId], CURLOPT_SSL_VERIFYPEER, 0); // verify device
	//res = curl_easy_setopt(clientGwParams_g.curl[threadId], CURLOPT_SSL_CTX_FUNCTION, *sslctx_function);
	//res = curl_easy_setopt(clientGwParams_g.curl[threadId], CURLOPT_CAINFO, cainfo);
	//if(CURLE_OK != res)
	//{
	//	printf("Error getting ca info res=%d errno:%s",res,(char*)curl_easy_strerror(res));
//		curl_easy_cleanup(clientGwParams_g.curl[threadId]);
//		clientGwParams_g.curl[threadId] = NULL;
//		//COM_memory_put(action, __FILE__, __LINE__);
//		return OZ_ERROR; // Enable to 'Manager' reporting of status 
//	}

        //long ssl_version = CustomGetSslVersion();
		//curl_easy_setopt(clientGwParams_g.curl[threadId], CURLOPT_SSLVERSION, ssl_version);
        //OZ_LOG_PRINTF(__FILE__, COM_LOG_DEBUG, __LINE__,"CURLOPT_SSLVERSION=%u\n", ,ssl_version);
		
        curl_easy_setopt(clientGwParams_g.curl[threadId], CURLOPT_TIMEOUT, 60);

		curl_easy_setopt(clientGwParams_g.curl[threadId], CURLOPT_URL, clientGwParams_g.urlName);
	    /* Do not do the transfer - only connect to gateway */
	    curl_easy_setopt(clientGwParams_g.curl[threadId], CURLOPT_CONNECT_ONLY, 1L);
    }

    return ret;
}
// TODO for future use
/*
static CURLcode sslctx_function(CURL *curl, void *sslctx, void *parm)
{
  CURLcode rv = CURLE_ABORTED_BY_CALLBACK;
  X509_STORE *store = NULL;
  X509 *cert = NULL;
  BIO *bio = NULL;
    
  // clear the current thread's OpenSSL error queue
  ERR_clear_error();
 
  // get a BIO
  bio = BIO_new_mem_buf(ca_bundle_crt, -1);
  if(!bio)
    goto err;
 
  // use it to read the PEM formatted certificate from memory into an X509
  // structure that SSL can use
  // 
  if(!PEM_read_bio_X509(bio, &cert, 0, NULL))
    goto err;
 
  // get a pointer to the X509 certificate store (which may be empty!) 
  store = SSL_CTX_get_cert_store((SSL_CTX *)sslctx);
  if(!store)
    goto err;
 
  // add our certificate to this store 
  if(!X509_STORE_add_cert(store, cert)) {
    unsigned long error = ERR_peek_last_error();
 
    // Ignore error X509_R_CERT_ALREADY_IN_HASH_TABLE which means the
    // certificate is already in the store. That could happen if
    // libcurl already loaded the certificate from a ca cert bundle
    // set at libcurl build-time or runtime.
    // 
    if(ERR_GET_LIB(error) != ERR_LIB_X509 ||
       ERR_GET_REASON(error) != X509_R_CERT_ALREADY_IN_HASH_TABLE)
      goto err;
 
    ERR_clear_error();
  }
 
  rv = CURLE_OK;
 
err:
  if(rv != CURLE_OK) {
    char errbuf[256];
    unsigned long error = ERR_peek_last_error();
 
    fprintf(stderr, "error adding certificate\n");
    if(error) {
      ERR_error_string_n(error, errbuf, sizeof(errbuf));
      fprintf(stderr, "%s\n", errbuf);
    }
  }
 
  X509_free(cert);
  BIO_free(bio);
  ERR_clear_error();
 
  return rv;
}*/
#ifdef __cplusplus
}
#endif
