
/******************************************************************************
* File Name          : ClientAgentEncoder.c
* Author             : Atzmon Dahan
* Date First Issued  : Jan 29, 2019 : Version 1.0
* Description        : Client API for APP
* *
* oz_initialization (API)
* oz_getVersion (API)
* oz_createTunnel (API)
* oz_destroyTunnel (API)
* oz_shutdown (API)
* oz_register_print_callback (API)
************************************************************************************
* History:
*
***********************************************************************************/


#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include "OzClientAPI.h"
#include "OzUtils.h"
#include "sha256.h"
#include "base64.h"
#include "urlcode.h"


#if WIN32
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//using namespace ozclient;
/*-------------Defines---------------------------------------------------------------*/
#define SECRET_SHARED_KEY  		"OzvisionAgentCloudProprietaryProtocol"
#define OSN_WS_TIMESTAMP_SIZE    24  // "2014-02-20T10:00:00+0200"
#define DEVICE_ID_SIZE 		     32
#define OSN_WS_MAX_API_LEN       (1024*1)

/*------------ External reference -------------------------------------------------- */

/*-------------Variables ------------------------------------------------------------*/

/*-------------Prototype ------------------------------------------------------------*/

/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/

static char* osn_ws_create_timestamp(void)
{
    unsigned int size;
    time_t rawtime;
    struct tm * timeinfo;
	static char  iso_8601_time[OSN_WS_TIMESTAMP_SIZE+1];

    time (&rawtime);
    timeinfo = gmtime (&rawtime);

    size = strftime (iso_8601_time, sizeof(iso_8601_time), "%FT%T%z",timeinfo);
    OZCLIENT_PRINTF(OSNCLIENT_LOG_DEBUG, __FUNCTION__, __LINE__, "%s",iso_8601_time);

    if(size == OSN_WS_TIMESTAMP_SIZE){
    	return iso_8601_time;
    }

    OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, NULL);

    return NULL;
}
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/

char* clientAgentEncoder(char* clt_agt_prot_ver,char*uuid,char* user_id,const char* device_id,char *datain)
{
	char data_json[OSN_WS_MAX_API_LEN];
	char signature_sha256_out[SHA256_OUT_SIZE + 1];
	unsigned int signature_base64_out_len = 256;
	static char* signature_base64_out = NULL;
	static char* signature_in = NULL;
	int size;

	// Convert file name to JSON format
	if((*device_id == '\0') || (strlen(device_id) > DEVICE_ID_SIZE) || \
			(uuid == NULL) || (user_id == NULL) || (datain == NULL) ||  \
			(clt_agt_prot_ver == NULL)){
	    OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "clt_agt_prot_ver=%s,uuid=%s, user_id=%s,device_id=%s\njson=%s\n",clt_agt_prot_ver,uuid,user_id,device_id,datain);
		return NULL;
	}

	// JSON data
	sprintf(data_json, "%s",datain);

	if(signature_base64_out == NULL){
		signature_base64_out = (char*)malloc(signature_base64_out_len);
	}

	// Get current time, in ISO8601 format
	char *p_tstamp = osn_ws_create_timestamp();

	// Create a signature from : file name, domain, time, id & key
	unsigned int signature_len = (strlen(clt_agt_prot_ver) + strlen(uuid) + strlen(user_id) + strlen(device_id) + strlen(p_tstamp) \
			+ strlen(SECRET_SHARED_KEY) + strlen(data_json) + 1);

	if(signature_len > OSN_WS_MAX_API_LEN)
	{
	    OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "signature_len[%d] out of range",signature_len);
		return NULL;
	}

	if(signature_in == NULL){
		signature_in = (char*)malloc(OSN_WS_MAX_API_LEN);
	}
	sprintf(signature_in, "%s%s%s%s%s%s%s",
			p_tstamp, uuid, user_id, device_id, clt_agt_prot_ver, data_json, SECRET_SHARED_KEY);


	SHA256_digest(signature_in, signature_sha256_out);
	size = base64encode((const void*)signature_sha256_out, SHA256_OUT_SIZE / 2, signature_base64_out, signature_base64_out_len);

#ifdef ENCODE_DATA
	if(size == 0){
	     OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, NULL);
	}
	else
	{
		// Create message parameters from : file name, domain, time, device_ID & signature
		unsigned int parameters_len = 64 + url_encode_out_size(data_json) +
										url_encode_out_size(p_tstamp) + strlen(uuid) + strlen(user_id) + strlen(device_id) +
										url_encode_out_size(signature_base64_out);

		char* parameters = (char*)malloc(parameters_len);

		char* url_data_json = url_encode(data_json);
		char* url_time = url_encode(p_tstamp);
		char* url_signature = url_encode(signature_base64_out);

		sprintf(parameters ,"timestamp=%s&uuid=%s&user=%s&device_id=%s&clt_agt_prot_ver=%s&signature=%s&data=%s",
				 url_time, uuid, user_id, device_id, clt_agt_prot_ver, url_signature, url_data_json);

		free(url_data_json);
		free(url_time);
		free(url_signature);
#else
	if(size == 0){
		  OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, NULL);
	}
	else
	{
		// Create message parameters from : file name, domain, time, device_ID & signature
		unsigned int parameters_len = 128 + strlen(data_json) +
				strlen(p_tstamp) + strlen(clt_agt_prot_ver) + strlen(uuid)  + strlen(user_id) + strlen(device_id) +
				strlen(signature_base64_out);

		char* parameters = (char*)malloc(parameters_len);

		sprintf(parameters ,"timestamp=%s&uuid=%s&user=%s&device_id=%s&clt_agt_prot_ver=%s&signature=%s&data=%s",
				 p_tstamp,uuid,user_id,device_id,clt_agt_prot_ver,signature_base64_out,data_json);
#endif

		//OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "%s\n",parameters);

		return parameters;
	}

	return NULL;
}


#ifdef __cplusplus
}
#endif

/*-------------End Module ------------------------------------------------------------*/
