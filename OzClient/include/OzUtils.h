#ifndef _OZCLOUD_H
#define _OZCLOUD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdarg.h"
#include "stdio.h"
#include "pthread.h"

#include <stdarg.h>
#ifdef ANDROID
#include <android/log.h>
#include <signal.h>
#endif

#include "OzClientAPI.h"
//#define V1_ON
#define V2_ON

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

typedef enum
{
   CUST_LOREX,
   CUST_MOMENTUM,
   CUST_SWANN,
   CUST_NIGHTOWL,
   CUST_HISENSE,
   CUST_OZVN,
   MAX_CUSTOM
}osn_customer_e;

typedef enum
{
   ENV_PROD,
   ENV_UAT,
   ENV_RND,
   ENV_TST,
   MAX_ENV
}osn_env_e;

#define  NONE_CUSTOM  MAX_CUSTOM
#define  NONE_ENV     MAX_ENV

#define  TELNET_TUNNEL    230
#define  SSH_TUNNEL       231
#define  DEBUG_TUNNEL     232

typedef struct _account_creds_
{
	char user_id[MAX_USER];
	char user_password[MAX_PASSWORD];
	char user_domain[MAX_DOMAIN];
}account_creds, *p_account_creds;

typedef struct _customer_info_
{
   const char *prod_signaling;
   const char *uat_signaling;
   const char *prod_gateway;
   const char *uat_gateway;
   const char *prod_acct;
   const char *uat_acct;
   const char *organization_code;
}customer_info, *p_customer_info;

typedef struct _Tunnel_type_
{
	servicePort_t video_port;
	servicePort_t command_port;
	servicePort_t debug_port;
}tunnel_type, *p_tunnel_type;

typedef struct _CUST_MANAGE_PASSWORD_
{
   const char *account;
   const char *password;
}CUST_MANAGE_PASSWORD, *P_CUST_MANAGE_PASSWORD;

typedef struct _ozclien_db_
{
    //p2p
    int   p2p_method;
    int   *p_p2p_method;
    char  *signaling_dns;
    char  *device_id;
    int   service_port;
    int   local_port;
    int   environment;
    int   customer;
    char *client_protocol_version;
    char *agent_protocol_version;
    char *uuid;
    account_creds     credentials;
    osn_tunnelInfo_t  tunnelInfoHandle;
    osn_status_t      *tunnel_status;
    // cloud
    char *gateway_dns;
    char *acct_dns;
    int cloud_port;
    CUST_MANAGE_PASSWORD managment;

}ozclien_db, *p_ozclien_db;

typedef struct _V25_THREAD_ARG_
{
	ozclien_db   oem_info;

}THREAD_V25_ARG_S, *P_THREAD_V25_ARG_S;

typedef struct _V1_THREAD_ARG_
{
	p_account_creds credentials;
	p_customer_info oem_info;

}THREAD_V1_ARG_S, *P_THREAD_V1_ARG_S;

OSN_EXTERN void ozInitTunnel(p_ozclien_db p_oem_info);
OSN_EXTERN int ozGetCustomer(char *str);
OSN_EXTERN int ozGetEnvironment(char *str);
OSN_EXTERN servicePort_t ozGetTunnelPort(servicePort_t tunnelType,int customer);

OSN_EXTERN print_callback _oz_print_callback;
OSN_EXTERN const char STATE_STR[MAX_OSN_STATUS][40];

/*print debug */

OSN_EXTERN print_callback _oz_print_callback;
#if 1
#define OZ_CLIENT 		"oz_client"
#define MAX_PRINT_BUFF   1024

static int _oz_severity_level = OSNCLIENT_LOG_INFO;

static void OZCLIENT_PRINTF(int level, const char *func, int line, const char *format, ...)
{
    va_list argptr;
    static char buff[MAX_PRINT_BUFF];

    if (level <= _oz_severity_level)
    {
        int num;

        va_start(argptr, format);
        num = snprintf(buff, MAX_PRINT_BUFF, "%s : [%d:%s:%d:%lx]", OZ_CLIENT, level, func, line, (unsigned long)pthread_self());
        num += vsnprintf(buff + num, MAX_PRINT_BUFF - num, format, argptr);
        if (!line)
        {
            num += snprintf(buff + num, MAX_PRINT_BUFF - num, "\n");
            int index;
            for (index = 0; index < num - 4; index++)
            {
                if (((buff[index] == 'n') || (buff[index] == 'N')) &&
                        ((buff[index+1] == 'a') || (buff[index+1] == 'A')) &&
                        ((buff[index+2] == 'b') || (buff[index+2] == 'B')) &&
                        ((buff[index+3] == 't') || (buff[index+3] == 'T')) &&
                        ((buff[index+4] == 'o') || (buff[index+4] == 'O')))
                {
                    buff[index] = '_';
                    buff[index+1] = '_';
                    buff[index+2] = 'O';
                    buff[index+3] = 'z';
                    buff[index+4] = 'C';
                    buff[index+5] = 'l';
                    buff[index+6] = 'i';
                }
            }

        }

#ifdef ANDROID
        __android_log_print(ANDROID_LOG_INFO, "%s", buff);
#else
        printf("%s", buff);
#endif

        if (_oz_print_callback)
            _oz_print_callback(buff);

        va_end(argptr);
    }
}
#endif

OSN_EXTERN p_ozclien_db p_client_env;

#ifdef __cplusplus
} //extern "C"
#endif

#endif // _OZCLOUD_H
