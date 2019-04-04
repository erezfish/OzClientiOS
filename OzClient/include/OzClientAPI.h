#ifndef _OZCLIENT_API_H
#define _OZCLIENT_API_H

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

#include <stdbool.h>

typedef enum
{
	P2P_NONE,
	P2P_V1,
	P2P_V25,
	MAX_OSN_METHOD
}osn_method;

typedef enum
{
    OSN_CLOSED = -1,
    OSN_CONNECTING = 0,
    OSN_READY_FOR_RECONNECT = 1,
    OSN_UNKNOWN = 2,
    OSN_LOCAL = 3,
    OSN_REMOTE_P2P = 4,
    OSN_REMOTE_RELAY = 5,
    OSN_REMOTE_RELAY_MICRO = 6,
    OSN_MAX_TUNNEL_STATE
}osn_tunnel_state;

/**
 * Status codes from API functions
 */
typedef enum  {
/* tunnel connection status */
   OSN_OK,                          ///< operation successful
   OSN_P2P_REMOTE,                  ///< operation successful created tunnel type P2P
   OSN_P2P_RELAYED,                 ///< operation successful created tunnel type RALAYED and timed out
   OSN_ERROR_API_NOT_INITIALIZED,    /// initialization failed, session wrong, certificates not found ...
   OSN_ERROR_NO_NETWORK,             /// no network available
   OSN_ERROR_CANNOT_CONNECT_TO_AGENT, ///< could not connect to specified host
   OSN_ERROR_LOCAL_PORT_ALREADY_USED, ///< could not connect via specified localport
   OSN_ERROR_AGENT_RESOURCES_LIMIT_REACHED, ///< could not connect due to device max concurrent clients number reached
   OSN_ERROR_INVALID_DEVICE_ID,///< could not connect due to invalid parameter passed
   OSN_ERROR_INVALID_SERVICE_PORT,///< could not connect due to invalid parameter passed
   OSN_ERROR_INVALID_LOCAL_PORT,///< could not connect due to invalid parameter passed
   OSN_ERROR_INVALID_TUNNEL,    /// invalid tunnel data passed for destroy tunnel
   OSN_ERROR,                  ///< an unspecified error occurred, necessary to check logfile to find out what actually went wrong
   OSN_OK_DIRECT,               ///< V25 direct tunnel
   OSN_ERROR_TUNNEL_TYPE,
/*error osn status 15 */
   OSN_INIT_FAIL,
   OSN_WRONG_ENV,
   OSN_WRONG_CUSTOMER,
   OSN_NO_PROFILE,
   OSN_ERROR_READING_CONFIG,
   OSN_API_NOT_INITIALIZED,
   OSN_INVALID_SESSION,
   OSN_OPEN_CERT_OR_PK_FAILED,
   OSN_UNLOCK_PK_FAILED,
   OSN_PORTAL_LOGIN_FAILURE,
   OSN_CERT_SIGNING_ERROR,
   OSN_CERT_SAVING_FAILURE,
   OSN_ADDRESS_IN_USE,
   OSN_INVALID_ADDRESS,
   OSN_NO_NETWORK,
   OSN_TCP_SOCKET_PROBLEM,
   OSN_CONNECT_TO_HOST_FAILED,
   OSN_STREAMING_UNSUPPORTED,
   OSN_INVALID_STREAM,
   OSN_DATA_PENDING,
   OSN_BUFFER_FULL,
   OSN_FAILED,
   OSN_INVALID_TUNNEL,
   OSN_ILLEGAL_PARAMETER,
   OSN_INVALID_RESOURCE,
   OSN_INVALID_STREAM_OPTION,
   OSN_INVALID_STREAM_OPTION_ARGUMENT,
   OSN_ABORTED,
   OSN_STREAM_CLOSED,
   /* adding more error codes 44*/
   OSN_CANNOT_CONNECT_TO_AGENT,
   OSN_LOCAL_PORT_ALREADY_USED,
   OSN_AGENT_RESOURCES_LIMIT_REACHED,
   OSN_INVALID_DEVICE_ID,
   OSN_INVALID_SERVICE_PORT,
   OSN_INVALID_LOCAL_PORT,
   OSN_INVALID_CUSTOMER_HANDLE,
   OSN_V1_NO_PERMISSION,
   MAX_OSN_STATUS
}osn_status_t;

typedef enum  {
	OZ_HANDLE = 1000
}oz_handle_t;

typedef enum
{
	ACTION_BY_USER = 0,
	ACTION_BY_MANAGEMENT,
	NUM_OF_OWNERS
} action_owner_e;

#define  OSN_P2P_DIRECT  	OSN_OK
#define  VIDEO_TUNNEL       41
#define  COMMAND_TUNNEL     42

enum
{
    OSNCLIENT_LOG_EMERG, OSNCLIENT_LOG_ALERT, OSNCLIENT_LOG_CRIT, OSNCLIENT_LOG_ERR, OSNCLIENT_LOG_WARNING,
    OSNCLIENT_LOG_NOTICE, OSNCLIENT_LOG_INFO, OSNCLIENT_LOG_DEBUG
};

typedef unsigned int localPort_t; // available range from 1024 - 65536
typedef unsigned int servicePort_t; // available servicePort (for example 80,9000,1025 or 23)

/**
 * Struct with all tunnel info with its created connection type
 * that should be returned on   success create tunnel or NULL if create tunnel failed
 */
#define MAX_NAME        256
#define MAX_USER        128
#define MAX_PASSWORD    64
#define MAX_DOMAIN      128
#define MAX_VERSION     256
#define MAX_DEVICEID    32

#define NUM_OF_RETRIES 	   50
#define MAX_TUNNEL_ENTRIES 100


typedef struct _osn_tunnel_creds_
{
    char* user_id;
    char* domain;
    char* password;
}osn_tunnel_creds, *osn_tunnelCreds_t;

typedef struct _osn_tunnel_info_
{
   servicePort_t         servicePort;
   localPort_t           localPort;
   osn_status_t          tunnelStatus;
   const char*           deviceId;
}osn_tunnel_info, *osn_tunnelInfo_t;

typedef struct
{
   servicePort_t        servicePort;
   localPort_t          localPort;
   osn_tunnel_state     tunnelStatus;
   char                 deviceId[MAX_NAME];
   osn_tunnel_creds     cred;
   void                 *context;
}osn_tunnel_data;

typedef struct _osn_environment_
{
   char cloud[MAX_NAME];
   char customer[MAX_NAME];
}osn_environment, *osn_environment_t;

typedef struct _collect_data_
{
   osn_environment environment;
   char wifi_network[MAX_NAME];
   char cellular_network[MAX_NAME];
   char os_version[MAX_VERSION];
   char app_name[MAX_NAME];
   char app_version[MAX_VERSION];
   char uuid[MAX_NAME];
   char *library;
   char *gps;
}collect_data, *osn_collectData_t;

typedef struct oz_pair_info
{
    const char* device_id;
    const char *jsonData;
} oz_pair_info_t;


/*================ APIs prototype ===================*/

typedef void (*print_callback)(char *);
OSN_EXTERN osn_status_t OSN_API oz_register_print_callback(print_callback print, int level);

//typedef void (*oz_print_callback)(char *);
OSN_EXTERN osn_status_t OSN_API ozClient_register_print_callback(print_callback print, int level);
/**
* OSN_API oz_initialization
* Set the start directory of the tunneling environment
*
* @param - path - A path to a writable working directory (runtime installed) directory. The following subdirectories and files should exist :
*       roots\ca.crt
*       roots\ozvision-root.crt
*       users\<customer>_at_ozvision.com.crt
*       users\<customer>_at_ozvision.com.key
*       configuration\mimetypes.ini
*
* If path == NULL or the this API was not called before create tunnel api  then current directory is used (normally in  WIN/MAC/LINUX )
* On Android and IOS platforms, the path is the directory where the apk/ipa package unpacked during application start (in runtime) and the above directories and files copied into it from assets (resources).
* @return - osn_status_t
* Only OSN_OK returned
*
*/
OSN_EXTERN oz_handle_t OSN_API oz_initialize(const char* path,osn_tunnelCreds_t cred,osn_collectData_t collect_data);

/**
*  oz_handle OSN_API oz_createTunnel
*  Creates a tunnel to deviceId:servicePort on local port
*  on first call initialize the tunneling environment, opens session on Base Server
*  to re-initialize environment use shutdown call before.
* @param - deviceId - unique device identifier, must match the device id that the agent uses
* @param - servicePort - Agents port number to create a tunnel to
* @param - localPort_t  - local port number of the tunnel. Valid values are 1025 to 65535 or 0.
*  If 0 - OSN will find an available port number for the local tunnel end
*  If 1025-65535 - OSN will assign the local end the specified value or will fail is not available
* @param output osn_tunnelInfo_t
* @return - output param osn_status_t allocatedLocalPort ;
* [RW] - Please write the possible failure codes returned and their meaning
* On success valid return values OSN_OK_RELAYED or OSN_OK_P2P
* on failure the different OSN_ERRORs can be returned
*/

OSN_EXTERN osn_status_t OSN_API oz_createTunnel(
												oz_handle_t			 customerHandle,
                                                const char*          deviceId,
                                                servicePort_t        servicePort,
                                                localPort_t          localPort,
                                                osn_tunnelInfo_t     tunnelInfoHandle
                                              );



/**
* OSN_API oz_destroyTunnel
* Gracefully destroy a tunnel identified by osn_tunnelInfo_t .deviceId, osn_tunnelInfo_t .servicePort osn_tunnelInfo_t .localPort from map of active tunnels. After the function is called the local port that was associated with this tunnel is available for new tunnel creation.
*
* @param - osn_tunnelInfo_t  - struct with tunnel info of created previously tunnel
* @return - osn_status_t valid return values OSN_ERROR_INVALID_TUNNEL or OSN_OK
*/

OSN_EXTERN osn_status_t OSN_API oz_destroyTunnel(oz_handle_t customerHandle,osn_tunnel_info *tunnelInfoHandle);

/**
* osn_status_t OSN_API oz_shutdown
* Gracefully destroys all previously created tunnels and shuts down the tunneling environment.
* @params oz_handle_t customerHandle - OzHandl customer for activate the OZ APIs
* @param  osn_tunnel_info *tunnelInfoHandle
* @return - osn_status_t
* valid return values OSN_OK on success
* or
*    OSN_ERROR_INVALID_SESSION,            /// could not close session
*    OSN_ERROR_API_NOT_INITIALIZED
*/

OSN_EXTERN osn_status_t OSN_API oz_shutdown(oz_handle_t customerHandle);

/**
 * Function definition
 * @params oz_handle_t customerHandle - OzHandl customer for activate the OZ APIs
 * @return char * version - Full api and Core API version string
 */
OSN_EXTERN  const char* OSN_API oz_getVersion(void);

/**
 * Function definition
 *
 * @param  oz_handle_t handle
 * @param  recvBuf - buffer to fill with response
 * @param  const char* dataJson - pointer to the API json
 * @param  const char* action - cloud API action
 * @return - 0 on success
 */
OSN_EXTERN int OSN_API oz_cloudSendRequestAndGetResponse(oz_handle_t handle, const char* dataJson, const char* action, const char* device_id,
	       	char* recvBuf, action_owner_e action_owner);

/**
 * Function definition
 * @params oz_handle_t customerHandle - OzHandl customer for activate the OZ APIs
 * @params const char *device_id
 * @params  const char *api - pointer to API
 * @param char *payload - pointer to json payload
 * @return - pointer to the cloud response buffer
 *----------------------------------------------------------------------------------
 *({"req": "get camera name"})
 *({"req": "set camera name", "name": <value>})
 *({"req": "get privacy mode"})
 *({"req": "set privacy mode", "mode": "<value>"})
 *({"req": "start recording", “stream name”, <value>})
 *({"req": "stop recording", “stream name”, <value>})
 *({"req": "take snapshot"})
 *({"req": "get garage door state", “number”: <value>, "state": "<value>"})
 *({"req": "set garage door state", “number”: <value>, "state": "<value>"})
 *({"req": "toggle garage door state”, “number”: <value>})
 *({"req": "fw upgrade control", "action": "<value>"})
 *({"req": "flip image"})
 *({"req": "fw upgrade status"})
 *({"req": "set two way audio enabled", "enabled": <value>})
 *({"req": "localstorage device get info"})
 *({"req": "localstorage device format"})
 *({"req": "localstorage get directory name"})
 *({"req": "localstorage get list", start_time: <value>, end_time: <value>, file_type: <value>})
 *({"req": "localstorage playback", filepath: <value>})
 *({"req": "localstorage delete", filepath: <value>})
 *({"req": "localstorage delete range", start_time: <value>, end_time: <value>})
 *({"req": "set time zone”, “offset”: <value>}})
 *------------------------------------------------------
 * @return - pointer to the FW response buffer
 */
OSN_EXTERN int OSN_API oz_agentSendRequestAndGetResponse(oz_handle_t handle,const char* device_id, char *jsonData, int local_port, char* recvBuf);

/**
 * Function definition
 *
 * Set permission connect to V1 (by OzConnect)
 * @params oz_handle_t customerHandle - OzHandl customer for activate the OZ APIs
 * @return char * none
 */
OSN_EXTERN void OSN_API oz_setPermission(oz_handle_t customerHandle,int permision);

/**
 * Function definition
 *
 * Ozvision part in the pairing process - must be called in order for pairing to succeed.
 * @params oz_pair_info_t - pairing information needed.
 * @return 0 for success -1 for error
 */
OSN_EXTERN int OSN_API oz_devicePairing(oz_pair_info_t* pair_info);

OSN_EXTERN int OSN_API oz_selectP2P(int osnp2p);  //Tag:required modification for: Dual client

#endif // _OZCLIENT_API_H
