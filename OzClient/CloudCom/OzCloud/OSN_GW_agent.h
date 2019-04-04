#ifndef __OSN_GW_agent_h__
#define __OSN_GW_agent_h__

//#include "util.h"
#include "COM_def.h"
#include "OzUtils.h"

#define OSN_GW_FILE_NAME_SIZE           500
#define OSN_GW_PATH_NAME_SIZE           500
#define OSN_GW_MAX_RECEIVE_MESSAGE      1024*2
#define OSN_GW_DEVICE_ID_SIZE           24

#define GW_SERVICE  "osn"

typedef enum 
{
    GW_THREADS_GET = 0,
    GW_THREADS_NOTIF,
    GW_THREADS_LOG,
    GW_MNG_KA,
    GW_ADP_NOTIF,
    GW_NUMBER_OF_THREADS
} osn_gw_threads_e;

oz_ret_t OSN_GW_init(const char *deviceId, const char *deviceKey, const char *organization_code, const char* client_id);
oz_ret_t OSN_GW_close(void);

oz_ret_t OSN_GW_setGateway(const char* hostName, unsigned int port,
        unsigned int majorApiVersion, unsigned int minorApiVersion, const char* service);

char *OSN_GW_createRequestParameters(const char *dataJson, osn_gw_threads_e threadId, action_owner_e action_owner);
char* OSN_GW_createRequest(const char *action , char* data);

oz_ret_t OSN_GW_sendRequest(char* request, osn_gw_threads_e threadId);
int OSN_GW_recvActionResponse(char* buf, int bufSize, osn_gw_threads_e threadId);

int OSN_GW_curlInit(osn_gw_threads_e threadId);


#endif // __OSN_GW_agent_h__
