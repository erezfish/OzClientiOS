#ifndef _OSNCLIENT_API_HPP
#define _OSNCLIENT_API_HPP


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

#include "OzClientAPI.h"



/*================ P2PV1 APIs prototype ===================*/

typedef void (*print_callback)(char *);
OSN_EXTERN osn_status_t OSN_API fnOsnClient_register_print_callback_V1(print_callback print, int level);
OSN_EXTERN osn_status_t OSN_API fnOsnClient_startup_V1(const char* path);
OSN_EXTERN    osn_status_t     OSN_API fnOsnClient_createTunnel_V1(
                                                const char*          deviceId,
                                                servicePort_t        servicePort,
                                                localPort_t          localPort,
                                                osn_tunnelInfo_t     tunnelInfoHandle
                                              );
OSN_EXTERN osn_status_t OSN_API fnOsnClient_destroyTunnel_V1(osn_tunnel_info *tunnelInfoHandle);
OSN_EXTERN osn_status_t OSN_API fnOsnClient_shutdown_V1(void);
OSN_EXTERN const char* OSN_API fnOsnClient_getVersion_V1(void);

/*================ P2PV2.5 APIs prototype ===================*/
/*!
 * Definition of the callback function for the OsnRegisterLogCallback used on ANDROID to enable logs in debugger
 * function. This should use the OsnAPI calling convention in the future.
 * @param line     The line to log.
 * @param size     The length of the line.
 */
typedef void (*OsnLogCallbackFunc)(const char*line , size_t size);


/*!
 * Register a LOG callback function.
 * @param callback     The callback which is called for every LOG line.
 * @return    OSN_OK. This is the only value returned.
 */
OSN_EXTERN osn_status_t OSN_API OsnRegisterLogCallback(OsnLogCallbackFunc callback);

OSN_EXTERN osn_status_t OSN_API fnOsnClient_register_print_callback(print_callback print, int level);
OSN_EXTERN osn_status_t OSN_API fnOsnClient_startup(const char* path);
/*
OSN_EXTERN    osn_status_t     OSN_API fnOsnClient_createTunnel(
                                                const char*          deviceId,
                                                servicePort_t        servicePort,
                                                localPort_t          localPort,
						                         const osn_tunnelCreds_t creds,
                                                osn_tunnelInfo_t     tunnelInfoHandle
                                              );
                                              */

/*
* tunnel status change Callback signature
*/

typedef void (*OSN_api_tunnel_status_callback_t)(osn_tunnelInfo_t state, void* data);

/*
* tunnel status change Callback signature
*/

typedef void (*OSN_api_tunnel_free_user_data_t)(void *data);
OSN_EXTERN osn_status_t OSN_API fnOsnClient_createTunnel_V2 (
		         const char *dvr_id,
				 const char *dnsName,
			     servicePort_t service_port,
			     localPort_t local_port,
                 const osn_tunnel_creds *credentials,
			     osn_tunnel_info *tunnel_info,
			     OSN_api_tunnel_status_callback_t status_callback,
			     void *user_data,
			     OSN_api_tunnel_free_user_data_t freeUserData);


OSN_EXTERN osn_status_t OSN_API fnOsnClient_destroyTunnel(osn_tunnel_info *tunnelInfoHandle);
OSN_EXTERN osn_status_t OSN_API fnOsnClient_shutdown(void);
OSN_EXTERN const char* OSN_API fnOsnClient_getAPIVersion(void);

/*DEBUG!!*/
void *main_debug(void *_tunnel_data);
#endif // _OSNCLIENT_API_HPP
