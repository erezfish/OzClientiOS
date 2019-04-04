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

#include <stdio.h>
#include <stdlib.h>

#include "OSN_GW_agent.h"
#include "OzClientAPI.h"

static int send_http_request(const char *dataJson, char* recvBuf, const char *action, action_owner_e action_owner);

int SendRequestToGW(const char *dataJson, osn_gw_threads_e threadId, const char *action, action_owner_e action_owner)
{
    char *requestParams = NULL;
    char *request = NULL;
    int ret = -1;

    requestParams = OSN_GW_createRequestParameters(dataJson, threadId, action_owner);
    if (requestParams != NULL)
        request = OSN_GW_createRequest(action , requestParams);
    if (request != NULL)
        ret = OSN_GW_sendRequest(request, threadId);


    free(requestParams);
    free(request);

    return ret;
}

void InitGW(const char *user_id, const char *user_password, const char *organization_code, const char* host, const char* client_id, int port)
{
    OSN_GW_init(user_id, user_password, organization_code, client_id);
    OSN_GW_setGateway(host, port, 1, 1, GW_SERVICE);
}

OSN_EXTERN int OSN_API oz_cloudSendRequestAndGetResponse(oz_handle_t handle, const char* dataJson, const char* action, const char* device_id,
	       	char* recvBuf, action_owner_e action_owner)
{	
	int ret = 0;
	
	ret = send_http_request(dataJson, recvBuf, action, action_owner);

	return ret;
}

/***********************************************************
  Description: 
   	   	   	   
  Parameters :

  Return     :
 **********************************************************/
int cloud_init(const char *user_id, const char *user_password, const char *organization_code, const char *host_name, const char* client_id, int port)
{
	int ret = 0;

	InitGW(user_id, user_password, organization_code, host_name, client_id, port);

	return ret;
}

/***********************************************************
  Description: 
   	   	   	   
  Parameters :

  Return     :
 **********************************************************/
int send_http_request(const char *dataJson, char* recvBuf, const char *action, action_owner_e action_owner)
{
	int nRecv = 0;
	int ret;
	
	if ((ret = SendRequestToGW(dataJson, GW_THREADS_NOTIF, action, action_owner)) != 0)
	{
		printf("Failed to send action to gateway\n");
		return -1;
	}

	nRecv = OSN_GW_recvActionResponse(recvBuf, OSN_GW_MAX_RECEIVE_MESSAGE, GW_THREADS_NOTIF);
	if (nRecv == 0)
	{
		printf("Failed to receive responnse from gateway\n");
		return -1;
	}

	OSN_GW_close();
	
	return 0;
}

#ifdef __cplusplus
}
#endif
