/******************************************************************************
* File Name          : client_agent.c
* Author             : Erez Fishhimer
* Date First Issued  : 24/01/2019 : Version 1.0
* Description        : Client API for agent/fw requests
* *
* oz_initialization (API)
************************************************************************************
* History:
*
***********************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h> // send
#include <sys/socket.h> // send
#include <unistd.h> // close
#include <netinet/in.h> // IPPROTO_TCP
#include <arpa/inet.h> // inet_pton
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "sha256.h"
#include "base64.h"
#include "urlcode.h"
#include "OzClientAPI.h"
#include "OzUtils.h"

#if WIN32
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//using namespace ozclient;
/*-------------Defines---------------------------------------------------------------*/
#define  OZ_EVENT   1
#define  OZ_OK      0
#define  OZ_ERROR  -1

#define RECV_START		0
#define RECV_IN_PROC	1
#define RECV_COMPLETE	2
#define MAX_RCV_SIZE     (2*1024)

#define HOST_IP  "127.0.0.1"

/*------------ External reference -------------------------------------------------- */
extern char* clientAgentEncoder(char* clt_agt_prot_ver,char*uuid,char* user_id,const char* device_id,char *datain);

extern p_ozclien_db p_client_env;
/*-------------Variables ------------------------------------------------------------*/

/*-------------Static function declaration ------------------------------------------------------------*/
static int create_tcp_socket(void);
static int init_client_socket(const char* remoteIp, int socket, int l4_port);
static int recieve_buff_from_socket(int sock, char* buf);

/*int main()
{
	const char* jsonData = "{\"req\": \"localstorage get list\", \"start_time\": 1548331910 , \"end_time\": 1548335662, \"file_type\": \"mp4\"}";
	const char* device_id = ""; 
       	int ch = 0;
       	int port  = 9500;
       	char recvBuf[20*1024];

	oz_agentSendRequestAndGetResponse(device_id, jsonData, ch, port, recvBuf);

	printf("%s\n",recvBuf);
	return 0;
}*/

OSN_EXTERN int OSN_API oz_devicePairing(oz_pair_info_t* pair_info)
{
    return 0;
}
	
int OSN_API oz_agentSendRequestAndGetResponse(oz_handle_t handle,const char* device_id, char *jsonData, int local_port, char* recvBuf)
{

	int ret = 0;
	int cur_sent = 0;
	int sock = -1;

	if(handle != OZ_HANDLE){
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Wrong API handle [%d]\n",handle);
		return OZ_ERROR;
	}
	// open tcp connection
	sock = create_tcp_socket();
	if(sock < 0)
	{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Failed to create tcp socket. errno: %s\n", strerror(errno));
		return OZ_ERROR;
	}

	if(init_client_socket(HOST_IP, sock, local_port) != OZ_OK)
	{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Failed to init tcp socket: %d. errno: %s\n", sock, strerror(errno));
		return OZ_ERROR;
	}

	// send request
	char* api_request = clientAgentEncoder(p_client_env->agent_protocol_version,p_client_env->uuid,p_client_env->credentials.user_id,device_id,jsonData);
	if(api_request != NULL)
	{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_DEBUG, __FUNCTION__, __LINE__, "%s\n",api_request);
    	cur_sent = send(sock, api_request, strlen(api_request), 0);
    	free(api_request);
		if(cur_sent < 0)
		{
			OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Failed to send command to firmware on socket %d. errno: %s\n", sock, strerror(errno));
			return OZ_ERROR;
		}
    }
	else{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Fail to create agent data\n");
	}

	// get response
	if((ret = recieve_buff_from_socket(sock, recvBuf)) == 0)
	{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Failed to recieve message from socket %d. errno: %s\n", sock, strerror(errno));
    	return OZ_ERROR;
	}

	close(sock);

	return ret;
}

static int create_tcp_socket(void)
{
  int sock = -1;
  if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) >= 0)
  {
	  struct timeval tv;

	  tv.tv_sec = 5;  /* 5 Secs Timeout */
	  tv.tv_usec = 0;

	  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
	  setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));

  }
  else{
	  OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Can't create TCP socket\n");
  }

  return sock;
}

static int init_client_socket(const char* remoteIp, int socket, int l4_port)
{
	struct sockaddr_in remote;
	int retVal;

	memset(&remote, 0, sizeof(struct sockaddr_in));
	remote.sin_family = AF_INET;
	retVal = inet_pton(AF_INET, remoteIp, (void *)(&(remote.sin_addr.s_addr))); // converts IP from text to binary form
	if( retVal <= 0)
	{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Can't set remote.sin_addr.s_addr\\n");
		if( retVal == 0)
			OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "%s is not a valid IP address", remoteIp);
		return OZ_ERROR;
	}

	remote.sin_port = htons(l4_port);

	if(connect(socket, (struct sockaddr *)&remote, sizeof(struct sockaddr)) < 0)
	{
		OZCLIENT_PRINTF(OSNCLIENT_LOG_ERR, __FUNCTION__, __LINE__, "Could not connect to tcp port %d %s\n", l4_port,strerror(errno));
		return OZ_ERROR;
	}

	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "Socket %d successfully initialized for port %d IP=%s\n", socket, l4_port, remoteIp);

	return OZ_OK;
}

static int recieve_buff_from_socket(int sock, char* buf)
{
    int len = 0;
    int totalLength = 0;
    int recvState = RECV_START;
    int nextBufOffset = 0;

    memset(buf, 0, MAX_RCV_SIZE);

    do{
    	len = recv(sock, (char*)(buf + nextBufOffset), MAX_RCV_SIZE, 0);
    	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "Got Response from FW (len = %d)", totalLength);

    	//len = strnlen((char*)(buf + nextBufOffset), MAX_RCV_SIZE); // since recv in non-blocking returns only the max size (bug?), we will rely on the preliminary zero memset and use strnlen

    	if(len > 0)
    	{
    		totalLength += len;
    		buf[totalLength] = '\0'; // terminate the string
    		nextBufOffset += len;
        	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "Message partially received. temp len = %d total = %d.\n", len, nextBufOffset);
    	}
    	else // no message waiting in receive queue
    	{
    		recvState = RECV_COMPLETE;
    	}
     }
     while (recvState != RECV_COMPLETE);

     return totalLength;
}

#ifdef __cplusplus
}
#endif
