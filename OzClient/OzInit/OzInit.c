/******************************************************************************
* File Name          : oz_init.c
* Author             : Erez Fishhimer
* Date First Issued  : 22/01/2019 : Version 1.0
* Description        : Client API for APP init cloud and p2p environment
* *
* oz_initialization (API)
************************************************************************************
* History:
*
***********************************************************************************/

#include <stdio.h>
#include <stdbool.h>

#include "OzClientAPI.h"
#include "OzUtils.h"

#if WIN32
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

oz_handle_t p2p_initialize(const char* path,osn_tunnelCreds_t cred,osn_collectData_t collect_data);
int cloud_init(const char *user_id, const char *user_password, const char *organization_code, const char *host_name, const char* uuid, int port);

/*-------------Defines---------------------------------------------------------------*/
/*------------ External reference -------------------------------------------------- */

/*-------------Variables ------------------------------------------------------------*/
ozclien_db client_env =
{
	.client_protocol_version = "0.0.1",
	.agent_protocol_version  = "0.0.1",
	.uuid = "abc123"
};
p_ozclien_db p_client_env = &client_env;

oz_handle_t OSN_API oz_initialize(const char* path,osn_tunnelCreds_t cred,osn_collectData_t collect_data)
{
	int ret = p2p_initialize(path, cred, collect_data);
	if (ret == OZ_HANDLE)
	{
		cloud_init(p_client_env->credentials.user_id ,p_client_env->credentials.user_password, p_client_env->credentials.user_domain,p_client_env->gateway_dns,(const char*)p_client_env->uuid, p_client_env->cloud_port);
	}

	return ret;
}


#ifdef __cplusplus
}
#endif

/*************************End Module*******************************/

