/******************************************************************************
* File Name          : OzUtils.c
* Author             : Atzmon Dahan
* Date First Issued  : 30/12/2018 : Version 1.0
* Description        : Client initialization process
* *
* ozInit  - initialize sequence (called by 'oz_initialization')
* ozClient_register_print_callback - set print level and APP callback
* ozGetCustomer - get customer OEM
* ozGetEnvironment - get CLOUD environment
************************************************************************************
* History:
*
***********************************************************************************/

/*-------------Includes----------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#ifdef ANDROID
#include <android/log.h>
#include <signal.h>
#endif

#include "OsnClient.h"
#include "OzUtils.h"


#if WIN32
#include <windows.h>
#endif

//namespace ozclient
//{
#ifdef __cplusplus
extern "C" {
#endif


/*-------------Defines---------------------------------------------------------------*/

/*------------ External reference -------------------------------------------------- */
extern osn_status_t  OSN_API oz_createTunnel_V2(const char*       deviceId,
                                      servicePort_t     servicePort,
                                      localPort_t       localPort,
                                      osn_tunnelInfo_t  tunnelInfoHandle
                                      );
#ifdef V1_ON
extern osn_status_t  OSN_API oz_createTunnel_V1(const char*       deviceId,
                                      servicePort_t     servicePort,
                                      localPort_t       localPort,
                                      osn_tunnelInfo_t  tunnelInfoHandle
                                      );
#endif
/*-------------Variables ------------------------------------------------------------*/

const char *OEM_ENV[MAX_CUSTOM] =
{
	"lorex",
    "swann",
    "momentum",
    "nightowl",
    "hisense",
    "ozvn"
};

const char *CLOUD_ENV[MAX_ENV] =
{
	"prod" ,
    "uat" ,
    "rnd" ,
    "tst" ,
};

const tunnel_type OEM_SERVICE_PORT[MAX_CUSTOM] =
{
	{554, 9500 ,80},
	{8282,9500 ,7891},
	{554, 9500 ,9030},
	{554, 9500 ,80},
	{8282,9500 ,7891},
	{554, 9500 ,80}
};

print_callback _oz_print_callback = NULL;
//THREAD_V25_ARG_S thread_arg;  TODO when find V1/V2
//pthread_t tid_V25,tid_V1;

/*-------------Prototypes------------------------------------------------------------*/
static void* ozCreateTunnelV25(void *arg);
//static void* ozCreateTunnelV1(void *arg);

osn_status_t OSN_API ozClient_register_print_callback(print_callback print, int level)
{
    _oz_severity_level = level;
    _oz_print_callback = print;

    return OSN_OK;
}

int ozGetCustomer(char *str)
{
	char * match;
	int i;

	for(i = 0; i < MAX_CUSTOM; i++)
	{
		const char *oem = OEM_ENV[i];
		if((match = strstr(str,oem))){
			return i;
		}
	}

	return NONE_CUSTOM;
}


int ozGetEnvironment(char *str)
{
	char * match;
	int i;

	for(i = 0; i < MAX_ENV; i++)
	{
		const char *env = CLOUD_ENV[i];
		if((match = strstr(str,env))){
			return i;
		}
	}

	return NONE_ENV;
}

servicePort_t ozGetTunnelPort(servicePort_t tunnelType,int customer)
{
	servicePort_t servicePort = 0;

	if(customer > MAX_CUSTOM){
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Wrong customer %d\n",customer);
		return 0;
	}
	if(tunnelType == VIDEO_TUNNEL){
		servicePort = OEM_SERVICE_PORT[customer].video_port;
	}
	else if(tunnelType == COMMAND_TUNNEL){
		servicePort = OEM_SERVICE_PORT[customer].command_port;
	}
	else if(tunnelType == TELNET_TUNNEL){
		servicePort = 23;
	}
	else if(tunnelType == SSH_TUNNEL){
		servicePort = 22;
	}
	else if(tunnelType == DEBUG_TUNNEL){
		servicePort = OEM_SERVICE_PORT[customer].debug_port;
	}

	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "Service port %d\n",servicePort);

	return (servicePort);
}

#ifdef ANDROID
int pthread_cancel(pthread_t h) {
        return pthread_kill(h, 0);
}
#endif

#ifdef AUTO_P2P  //TODO - find V1/v2
void ozInitTunnel(p_ozclien_db p_oem_info)
{
	int err;

	/* Start V25 thread */
	thread_arg.oem_info.device_id = p_oem_info->device_id;
	thread_arg.oem_info.service_port = p_oem_info->service_port;
	thread_arg.oem_info.local_port = p_oem_info->local_port;
	thread_arg.oem_info.tunnel_status = p_oem_info->tunnel_status;
	thread_arg.oem_info.tunnelInfoHandle = p_oem_info->tunnelInfoHandle;
	thread_arg.oem_info.p_p2p_method = p_oem_info->p_p2p_method;
#ifdef V2_ON
	err = pthread_create(&(tid_V25), NULL, &ozCreateTunnelV25, (void*)&thread_arg);
	if (err != 0){
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "%s\n",strerror(errno));
	}
	//pthread_join(tid_V25, NULL);
#endif
#ifdef V1_ON   //TODO
	sleep(3);
	/* Start V1 thread*/
	err = pthread_create(&(tid_V1), NULL, &ozCreateTunnelV1, (void*)&thread_arg);
	if (err != 0){
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "%s\n",strerror(errno));
	}
	//pthread_join(tid_V1, NULL);
#endif
}

/*********************************************************************
  Description: ozCreateTunnelV25
               Start V25 thread to learn if V25 exist by create tunnel
               then APP will use V25 P2P.
  Parameters :
       Inputs:device_id,service_port,local_port
       Output: tunnelInfoHandle

  Return     :
 ********************************************************************/
#ifdef AOUTO_P2P//def V1_ON TODO
static void* ozCreateTunnelV25(void *arg)
{
	P_THREAD_V25_ARG_S p_thread_V25_arg = (P_THREAD_V25_ARG_S)arg;
	osn_status_t osn_status;

	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "Start V25 create tunnel\n");

	osn_status = oz_createTunnel_V2(p_thread_V25_arg->oem_info.device_id, p_thread_V25_arg->oem_info.service_port, p_thread_V25_arg->oem_info.local_port, p_thread_V25_arg->oem_info.tunnelInfoHandle);
	*p_thread_V25_arg->oem_info.tunnel_status = osn_status;
    *p_thread_V25_arg->oem_info.p_p2p_method = P2P_V25;

	pthread_detach(tid_V25);  //storage thread can be reclaimed when that thread is terminated

	return ((void*)0);
}

/*********************************************************************
  Description: ozCreateTunnelV1
               Start V1 thread to learn if V25 is not exist by create tunnel
               then APP will use V1 P2P.
  Parameters :

  Return     :
********************************************************************/
//#ifdef V1_ON
//TODO
static void* ozCreateTunnelV1(void *arg)
{
	THREAD_V1_ARG_S p_thread_V1_arg = (THREAD_V1_ARG_S)arg;
	osn_status_t osn_status;

	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "Start V1 create tunnel\n");

	osn_status = oz_createTunnel_V1(p_thread_V1_arg->oem_info.device_id, p_thread_V1_arg->oem_info.service_port, p_thread_V1_arg->oem_info.local_port, p_thread_V1_arg->oem_info.tunnelInfoHandle);
	*p_thread_V1_arg->oem_info.tunnel_status = osn_status;
    *p_thread_V1_arg->oem_info.p_p2p_method = P2P_V1;

	pthread_detach(tid_V1);  //storage thread can be reclaimed when that thread is terminated

	return ((void*)0);
}
#endif
#endif

#ifdef __cplusplus
}
#endif

/*************************End Module*******************************/

