/******************************************************************************
* File Name          : OzClientAPI.c
* Author             : Atzmon Dahan
* Date First Issued  : 30/12/2018 : Version 1.0
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "OsnClient.h"
#include "OzUtils.h"
#include "Version.h"


#if WIN32
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//using namespace ozclient;
/*-------------Defines---------------------------------------------------------------*/
/*------------ External reference -------------------------------------------------- */

/*-------------Variables ------------------------------------------------------------*/

//API status
const char STATE_STR[MAX_OSN_STATUS][40] =
{
		//Tunnel status
	    "P2P DIRECT CONNECTED",
		"P2P REMOTE CONNECTED",
		"P2P_RELAY CONNECTED",
		"OSN_ERROR API NOT INITIALIZED",    /// initialization failed, session wrong, certificates not found ...
		"OSN ERROR NO NETWORK",             /// no network available
		"OSN_ERROR_CANNOT_CONNECT_TO_AGENT", ///< could not connect to specified host
		"OSN_ERROR_LOCAL_PORT_ALREADY_USED", ///< could not connect via specified localport
		"OSN_ERROR_AGENT_RESOURCES_LIMIT_REACHED", ///< could not connect due to device max concurrent clients number reached
		"OSN_ERROR_INVALID_DEVICE_ID",///< could not connect due to invalid parameter passed
		"OSN_ERROR_INVALID_SERVICE_PORT",///< could not connect due to invalid parameter passed
		"OSN_ERROR_INVALID_LOCAL_PORT",///< could not connect due to invalid parameter passed
		"OSN_ERROR_INVALID_TUNNEL",    /// invalid tunnel data passed for destroy tunnel
		"OSN_ERROR",                  ///< an unspecified error occurred, necessary to check logfile to find out what actually went wrong
		"OSN_OK_DIRECT",               ///<  V25 direct tunnel
		"OSN_ERROR_INVALID_TUNNEL_TYPE",
		//Osn status
		"OSN_INITIALIZATION FAIL",
		"WRONG ENVIORENT",
		"WRONG CUSTOMER",
	    "OSN NO PROFILE",
	    "OSN ERROR READING CONFIG",
	    "OSN API NOT INITIALIZED",
	    "OSN INVALID SESSION",
	    "OSN OPEN CERT OR PK FAILED",
	    "OSN UNLOCK PK FAILED",
	    "OSN PORTAL LOGIN FAILURE",
	    "OSN CERT SIGNING ERROR",
	    "OSN CERT SAVING FAILURE",
	    "OSN ADDRESS IN USE",
	    "OSN INVALID ADDRESS",
	    "OSN_TCP SOCKET PROBLEM",
	    "OSN NO NETWORK",
	    "OSN CONNECT TO HOST FAILED",
	    "OSN STREAMING UNSUPPORTED",
	    "OSN INVALID STREAM",
	    "OSN DATA PENDING",
	    "OSN BUFFER FULL",
	    "OSN FAILED",
	    "OSN INVALID TUNNEL",
	    "OSN ILLEGAL PARAMETER",
	    "OSN INVALID RESOURCE",
	    "OSN INVALID STREAM OPTION",
	    "OSN INVALID STREAM OPTION ARGUMENT",
	    "OSN ABORTED",
	    "OSN STREAM CLOSED",
	    "OSN CANNOT CONNECT TO AGENT",
	    "OSN LOCAL PORT ALREADY USED",
	    "OSN AGENT RESOURCES LIMIT REACHED",
	    "OSN INVALID DEVICE ID",
	    "OSN INVALID SERVICE PORT",
	    "OSN INVALID LOCAL PORT",
	    "OSN INVALID CUSTOMER HANDLE",
	    "OSN V1 NO PERMISSION"
};

customer_info OEM_CLOUD[MAX_CUSTOM] =
{
	{"prod-flir-a.signaling.p2pbox.net",     "uat-flir-a.signaling.p2pbox.net",     "multicloud-us.flirservices.com", 		  "uat-multicloud-us.flirservices.com"    ,"acct.flirservices.com", "uat-multiacct-us.flirservices.com","Lemon"},
    {"swann.signaling.p2pbox.net",           "uat-swann-a.signaling.p2pbox.net",    "gateway.safebyswann.com",        		  "uat-swann-a-platform.safebyswann.com"  ,"acct.safebyswann.com", "uat-swann-a-acct.safebyswann.com","Sugar"},
    {"peq.signaling.p2pbox.net",             "uat-peq-a.signaling.p2pbox.net",      "gateway.momentum-cam.com",       	      "uatgateway.momentum-cam.com"           ,"accounting.momentum-cam.com", "uatacct.momentum-cam.com","Melon"},
    {"prod-nightowl-a.signaling.p2pbox.net", "uat-nightowl-a.signaling.p2pbox.net", "prod-nightowl-a-platform.nightowlc.com", "uat-nightowl-a-platform.nightowlc.com" ,"prod-nightowl-a-acct.nightowl.ozv.cloud", "uat-nightowl-a-acct.nightowl.ozv.cloud","Noodles"},
    {"swann.signaling.p2pbox.net",           "uat-swann-a.signaling.p2pbox.net",    "gateway.safebyswann.com",        		  "uat-swann-a-platform.safebyswann.com"  ,"acct.safebyswann.com", 	"uat-swann-a-acct.safebyswann.com","Orange"},
    {"rnd.signaling.p2pbox.net",			 "tst.signaling.p2pbox.net",            "rnd-platform.ozsn.net", 		 		  "tst-platform.ozsn.net",				  "rnd-acct.ozsn.net", "tst-acct.ozsn.net","Orange"}  // for ozvision develop and test
};

static int g_permission;
extern p_ozclien_db p_client_env;


/*-------------Prototypes------------------------------------------------------------*/

oz_handle_t p2p_initialize(const char* path,osn_tunnelCreds_t cred,osn_collectData_t collect_data)
{
	int osn_status = OSN_FAILED;
	int env,oem;

    //Get cloud environment
    env = ozGetEnvironment(collect_data->environment.cloud);
	if(env >=MAX_ENV)
	{
		osn_status = OSN_WRONG_ENV;
        return (oz_handle_t)osn_status;
	}
    //Set Cloud
	oem = ozGetCustomer(collect_data->environment.customer);
	if(oem >=MAX_CUSTOM)
	{
		osn_status = OSN_WRONG_CUSTOMER;
        return (oz_handle_t)osn_status;
	}

	if((env == ENV_UAT) ||(env == ENV_TST)){
		p_client_env->signaling_dns = (char*)OEM_CLOUD[oem].uat_signaling;
		p_client_env->gateway_dns = (char*)OEM_CLOUD[oem].uat_gateway;
		p_client_env->acct_dns = (char*)OEM_CLOUD[oem].uat_acct;
	}
	else if((env == ENV_PROD) ||(env == ENV_RND)){
		p_client_env->signaling_dns = (char*)OEM_CLOUD[oem].prod_signaling;
		p_client_env->gateway_dns = (char*)OEM_CLOUD[oem].prod_gateway;
		p_client_env->acct_dns = (char*)OEM_CLOUD[oem].prod_acct;
	}

	//Credentials
    strcpy(p_client_env->credentials.user_id,cred->user_id);
    strcpy(p_client_env->credentials.user_domain ,(char*)OEM_CLOUD[oem].organization_code);
    strcpy(p_client_env->credentials.user_password ,cred->password);


	p_client_env->uuid = collect_data->uuid;
	p_client_env->environment = env;
	p_client_env->customer = oem;
	p_client_env->cloud_port = 443;

	oz_selectP2P(P2P_V25);   //TODO remove it when auto find V1 or V2
#ifdef V2_ON //TODO
	//for OEM which is not 'lorex' startup P2P V25
	//if(oem != CUST_LOREX)
	{
		osn_status=fnOsnClient_startup(path);
		if(osn_status == OSN_OK){
			OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Startup V25 successfully\n");
		}
		else{
			OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Startup V25 failure [%d]\n");
		}
	}
#endif
#ifdef V1_ON //TODO
	//else
	{
		osn_status=fnOsnClient_startup_V1(path);
		if(osn_status == OSN_OK){
			OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Startup V1 successfully\n");
		}
		else{
			OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Startup V1 failure [%d]\n");
		}
	}
#endif

	if(osn_status == OSN_OK)
	{
		osn_status =(int)OZ_HANDLE;
	}

	return ((oz_handle_t)osn_status);
}

const char* OSN_API oz_getVersion(void)
{
	const char *buff;

    #ifdef V1_ON
	//if(p_client_env->p2p_method == P2P_V1)
	{
		buff = fnOsnClient_getVersion_V1();
	}
    #endif
#ifdef V2_ON
	//if(p_client_env->p2p_method == P2P_V25)
	{
		buff = fnOsnClient_getAPIVersion();
	}


#endif
	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "OzClient version: %s %s\n",buff,BUILD_VERSION);

	return(buff);
}

void OSN_API OSN_API oz_setPermission(oz_handle_t customerHandle,int permission)
{
	if(customerHandle != OZ_HANDLE)
	{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "[handle=%d]:%s\n",customerHandle,STATE_STR[OSN_INVALID_CUSTOMER_HANDLE]);
		return;
	}
	g_permission = permission;
    OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__,"Permission: %d\n",g_permission);
}

#ifdef V2_ON
osn_status_t  OSN_API oz_createTunnel_V2(const char*    deviceId,
                                      servicePort_t     servicePort,
                                      localPort_t       localPort,
                                      osn_tunnelInfo_t  tunnelInfoHandle
                                      )
{
	   osn_status_t osn_status;
	   osn_tunnel_creds cred;
	   // TODO:  should work in "organization_code" for signaling server authentication.
	   if((p_client_env->environment == ENV_UAT) || (p_client_env->environment == ENV_RND) || (p_client_env->environment == ENV_TST)){
		   p_client_env->credentials.user_domain;
	   }
	   else{
		   cred.domain   = (char*)"default";  //EVT_PROD
	   }
	   cred.user_id  = p_client_env->credentials.user_id;
	   cred.password = p_client_env->credentials.user_password;
       OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__,"Crediencials:[%s (%s)]\n",p_client_env->credentials.user_id,p_client_env->credentials.user_domain);
       OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__,"DNS: %s\n",p_client_env->signaling_dns);


		osn_status = fnOsnClient_createTunnel_V2(
		        deviceId,
				(const char*)p_client_env->signaling_dns,
		        servicePort,
		        localPort,
				&cred,
		        tunnelInfoHandle,
				NULL,NULL,NULL
		      );

	return(osn_status);
}
#endif

#ifdef V1_ON
osn_status_t  OSN_API oz_createTunnel_V1(const char*       deviceId,
                                      servicePort_t     servicePort,
                                      localPort_t       localPort,
                                      osn_tunnelInfo_t  tunnelInfoHandle
                                      )
{
	return(fnOsnClient_createTunnel_V1(deviceId,servicePort,localPort,tunnelInfoHandle));
}
#endif

osn_status_t  OSN_API oz_createTunnel(oz_handle_t customerHandle,
									  const char*       deviceId,
                                      servicePort_t     tunnelType,
                                      localPort_t       localPort,
                                      osn_tunnelInfo_t  tunnelInfoHandle
                                      )
{
	static osn_status_t osn_status = OSN_FAILED;
	static int p2p_method = P2P_V25;//P2P_NONE; TODO for V1
	int retries = 15;
	servicePort_t  servicePort;

	if(customerHandle != OZ_HANDLE)
	{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "[handle=%d]:%s\n",customerHandle,STATE_STR[OSN_INVALID_CUSTOMER_HANDLE]);
		return OSN_INVALID_CUSTOMER_HANDLE;
	}

	servicePort = ozGetTunnelPort(tunnelType,p_client_env->customer);
	tunnelInfoHandle->deviceId = NULL;
	tunnelInfoHandle->localPort = 0;
	tunnelInfoHandle->servicePort = 0;
	tunnelInfoHandle->tunnelStatus = OSN_ERROR;

	if(servicePort == 0){
		tunnelInfoHandle->tunnelStatus = OSN_ERROR_TUNNEL_TYPE;
		return (OSN_ERROR_TUNNEL_TYPE);
	}

	//On first createTunnel for find V25 or V1
	if(p2p_method == P2P_NONE)
	{
		p_client_env->tunnel_status = &osn_status;
		p_client_env->p_p2p_method  = &p2p_method;
		p_client_env->device_id     = (char*)deviceId;
		p_client_env->service_port  = servicePort;
		p_client_env->local_port    = localPort;
		p_client_env->tunnelInfoHandle = tunnelInfoHandle;

//TODO		ozInitTunnel(p_client_env);
		do{
			osn_status = *p_client_env->tunnel_status;
			if(--retries == 0){
				break;
			}
			sleep(1);
		}while((osn_status > OSN_P2P_RELAYED) && (osn_status != OSN_OK_DIRECT));

		if(retries > 0)
		{
			p_client_env->p2p_method = *p_client_env->p_p2p_method;
			OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "CreateTunnel[status=%d method=%d seconds=%d][%s %d %d]\n",osn_status, \
							p_client_env->p2p_method,
							retries,
							p_client_env->tunnelInfoHandle->deviceId, \
							p_client_env->tunnelInfoHandle->servicePort, \
							p_client_env->tunnelInfoHandle->localPort);
		}
	}
	else{
        #ifdef V1_ON
		if(p_client_env->p2p_method == P2P_V1){
			//if(g_permission == false){   //TODO
			//	return (OSN_V1_NO_PERMISSION);
			//}
			osn_status = oz_createTunnel_V1(deviceId,servicePort,localPort,tunnelInfoHandle);
		}

        #endif
        #ifdef V2_ON
		if(p_client_env->p2p_method == P2P_V25){
			osn_status = oz_createTunnel_V2(deviceId,servicePort,localPort,tunnelInfoHandle);
		}
        #endif
		else{
			OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "%s  %d: P2P method[%d] is not supported!\n",p_client_env->p2p_method);
		}
	}

	return(osn_status);
}

osn_status_t OSN_API oz_destroyTunnel(oz_handle_t customerHandle,osn_tunnel_info *tunnelInfoHandle)
{
	osn_status_t osn_status = OSN_FAILED;

	if(customerHandle != OZ_HANDLE)
	{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "[handle=%d]:%s\n",customerHandle,STATE_STR[OSN_INVALID_CUSTOMER_HANDLE]);
		return OSN_INVALID_CUSTOMER_HANDLE;
	}
    #ifdef V1_ON
	if(p_client_env->p2p_method == P2P_V1){
		osn_status = fnOsnClient_destroyTunnel_V1(tunnelInfoHandle);
	}
    #endif
    #ifdef V2_ON
	if(p_client_env->p2p_method == P2P_V25){
		osn_status = fnOsnClient_destroyTunnel(tunnelInfoHandle);
	}
    #endif
	else{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "P2P method[%d] is not supported!\n",p_client_env->p2p_method);
	}

	return (osn_status);
}

osn_status_t OSN_API oz_shutdown(oz_handle_t customerHandle)
{
	osn_status_t osn_status = OSN_FAILED;

	if(customerHandle != OZ_HANDLE)
	{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "[handle=%d]:%s\n",customerHandle,STATE_STR[OSN_INVALID_CUSTOMER_HANDLE]);
		return OSN_INVALID_CUSTOMER_HANDLE;
	}

    #ifdef V1_ON
	if(p_client_env->p2p_method == P2P_V1){
		osn_status = fnOsnClient_shutdown_V1();
	}
    #endif
    #ifdef V2_ON
	if(p_client_env->p2p_method == P2P_V25){
		osn_status = fnOsnClient_shutdown();
	}
    #endif
	else{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "%s  %d: P2P method[%d] is not supported!\n",p_client_env->p2p_method);
	}

	return (osn_status);
}

int OSN_API oz_selectP2P(int osnp2p)
{
	p_client_env->p2p_method = osnp2p;
	OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "P2P method = %d\n",p_client_env->p2p_method);

	return (p_client_env->p2p_method);
}

osn_status_t OSN_API oz_register_print_callback(print_callback print, int level)
{
	osn_status_t osn_status = OSN_FAILED;

    #ifdef V1_ON
	if(p_client_env->p2p_method == P2P_V1){
		osn_status = fnOsnClient_register_print_callback_V1(print, level);
	}
    #endif

	ozClient_register_print_callback(print, level);

	return (osn_status);
}
#ifdef __cplusplus
}
#endif

/*************************End Module*******************************/

