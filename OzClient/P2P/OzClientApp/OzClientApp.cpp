#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "OsnClient.h"

#include "cJSON.h"
#include "OZ_memory.h"
#include "agent_api.h"
#include "client_api.h"
#include "OzUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

extern oz_handle_t p2p_initialize(const char* path,osn_tunnelCreds_t cred,osn_collectData_t collect_data);

#define  OZAGENT
//#define  OZCLOUD

#define MAX_GETCHAR   256


typedef struct _SET_CLOUD_PARAMS_
{
	oz_handle_t customerHandle;
	char *device_id;
	osn_tunnel_creds user;
}SET_CLOUD_PARAMS, *P_SET_CLOUD_PARAMS;


static void OZ_setCloudAPI(int api_no,P_SET_CLOUD_PARAMS p_params);

int main(int argc, char* argv[])
{
    char buffer[MAX_GETCHAR];
    servicePort_t servicePort;
    localPort_t localPort;
    osn_tunnel_info tunnelInfoHandle;
    char DEVICE_ID[MAX_DEVICEID];
    char USER_ID[MAX_USER];
    char USER_PASSWORD[MAX_PASSWORD];
    collect_data collection_info;
    const char *version_buff;
    osn_status_t osn_status;
    SET_CLOUD_PARAMS cloud_params;

    if (argc == 6)
    {
        strncpy(DEVICE_ID, argv[1], MAX_DEVICEID);
        cloud_params.device_id = DEVICE_ID;
        strncpy(USER_ID, argv[2], MAX_USER);
        strncpy(USER_PASSWORD, argv[3], MAX_PASSWORD);
        sprintf(collection_info.environment.customer,"%s",argv[4]);
        sprintf(collection_info.environment.cloud,"%s",argv[5]);
        sprintf(collection_info.uuid,"abc123");
        cloud_params.user.user_id  = USER_ID;
        cloud_params.user.password = USER_PASSWORD;
        cloud_params.customerHandle = p2p_initialize("./",&cloud_params.user,&collection_info);
        //customerHandle = (oz_handle_t)999;
        if(cloud_params.customerHandle < OZ_HANDLE) {
        	osn_status = (osn_status_t)cloud_params.customerHandle;
        	if(osn_status < MAX_OSN_STATUS)
        		printf("Fail to start up osn_status[%d]: %s\n",osn_status,STATE_STR[osn_status]);
        	else{
        		printf("Fail to start up osn_status[%d]: OSN INVALID CUSTOMER HANDLE\n",osn_status);
        	}
        }
        tunnelInfoHandle.deviceId = NULL;
        oz_register_print_callback(NULL,OSNCLIENT_LOG_INFO);

        while(1)
        {
        	printf("Type command :\n");
        	printf("01: creatTunnel    02: destroyTunnel 03: getVersion\n");
			#ifdef OZAGENT
           	printf("04: agentSendRequestAndGetRespons\n");
            #endif
            #ifdef OZCLOUD
           	printf("05: cloudSendRequestAndGetRespons\n");
			#endif
          	printf("99: quit\n");
        	printf("> ");
        	fflush(stdout);

        	memset(buffer, 0, MAX_GETCHAR);
        	fgets(buffer, MAX_GETCHAR, stdin);

        	if(strstr(buffer, "01\n"))
        	{
        		do{
        			printf("Enter tunnel type(41 Video/42 Cmd,99 to return)\n");
        			printf("> ");
        			fgets(buffer, MAX_GETCHAR, stdin);
        			if(char *temp = strchr(buffer,'\n')){
        				*temp = '\0';
        			}
        			servicePort = atoi(buffer);
        			if(servicePort == 99){
        				break;
        			}
        			//
        			printf("Enter local port,99 to return\n");
        			printf("> ");
        			fgets(buffer, MAX_GETCHAR, stdin);
        			if(char *temp = strchr(buffer,'\n')){
        				*temp = '\0';
        			}
        			//
        			localPort = atoi(buffer);
        			printf("CUSTOMER = %s ENV = %s deviceId = %s, servicePort = %d, localPort = %d\n",collection_info.environment.customer,collection_info.environment.cloud,cloud_params.device_id, servicePort, localPort);
        			time_t start_time = time(NULL);
        			osn_status = oz_createTunnel(cloud_params.customerHandle,cloud_params.device_id, servicePort, localPort, &tunnelInfoHandle);
        			if((tunnelInfoHandle.tunnelStatus == OSN_OK_DIRECT) || (tunnelInfoHandle.tunnelStatus <= OSN_P2P_RELAYED))
        			{
        				time_t end_time = time(NULL);
        				int diff = (end_time-start_time);
        				printf("================================\n");
        				printf("Time create tunnel = %d seconds\n",(int)diff);
        				printf("================================\n");
        				printf("create tunnel %s on port %d for service port %d connect status[%d]: %s\n",tunnelInfoHandle.deviceId,tunnelInfoHandle.localPort,tunnelInfoHandle.servicePort,osn_status,STATE_STR[osn_status]);
        			}
        			else{
        				printf("%s\n",STATE_STR[osn_status]);
        			}
        		}while(localPort != 99);
        	}
         	else if(strstr(buffer, "02\n"))
        	{
         		if(tunnelInfoHandle.deviceId != NULL)
         		{
             		printf("device id[%s]\n",tunnelInfoHandle.deviceId);;
         			osn_status = oz_destroyTunnel(cloud_params.customerHandle,&tunnelInfoHandle);
         			printf("destroy tunnel osn_status[%d]\n",osn_status);
         	        sleep(1);
         		}
         		else{
         			printf("Tunnel doesn't exist\n");
         		}
        	}
         	else if(strstr(buffer, "03\n"))
        	{
            	version_buff = oz_getVersion();
                printf("OsnClient version %s\n",version_buff);
        	}
			#ifdef OZAGENT
         	else if(strstr(buffer, "04\n"))
        	{
                int api_no;
         		//if(tunnelInfoHandle.deviceId == NULL)
         		//{
         		//	printf("Tunnel doesn't exist\n");
         		//	continue;
         		//}
                do{
                   	printf("Enter agent API(99 to return)\n00 - getCameraName\n01 - setCameraName\n02 - getPrivacMode\n03 - setPrivacMode\n04 - startRecording\n");
                   	printf("05 - stopRecording\n06 - takeSnapshot\n07 - getGarageDoorState\n08 - setGarageDoorState\n");
                   	printf("09 - toggleGarageDoorState\n10 - fwUpgradeControl\n11 - fwUpgradeStatus\n12 - flipImage\n");
                   	printf("13 - setTwoWayAudioEnabled\n14 - localstorageDeviceGetInfo\n15 - localStorageDeviceFormat\n16 - localstorageGetDirectoryName\n");
                   	printf("17 - localstorageGetList\n18 - localstoragePlayback\n19 - localstorageDelete\n20 - localstorageDeleteRange\n");
                   	printf("21 - setTimeZone\n");
                	printf("> ");
                	fgets(buffer, MAX_GETCHAR, stdin);
                	if(char *temp = strchr(buffer,'\n')){
                		*temp = '\0';
                	}
                	api_no = atoi(buffer);
                	if(api_no < MAX_AGENT_API)
                	{
                	    char *recvBuf = (char*)malloc(1024*64);
                	    char *request = OZ_setAgentAPI[api_no].agent_api_F(api_no);
                	  	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "JSON request[%d]: %s\n",api_no, request);
                	    if(request != NULL)
                	    {
                	    	//int response = oz_agentSendRequestAndGetResponse(cloud_params.customerHandle,cloud_params.device_id,request,tunnelInfoHandle.localPort,recvBuf);
                	    	int len = oz_agentSendRequestAndGetResponse(cloud_params.customerHandle,cloud_params.device_id,request,tunnelInfoHandle.localPort,recvBuf);

                	    	//if(len>0)
                        	  	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "Response[%d]: %s\n",len, recvBuf);

                	    }
                		free(recvBuf);
                	}
                	else{
                		printf("API No is out of range %d\n",api_no);
                	}
                	printf("Type any key to continue\n");
                	fgets(buffer, MAX_GETCHAR, stdin);
                }while(api_no != 99);
        	}
            #endif
            #ifdef OZCLOUD
         	else if(strstr(buffer, "05\n"))
        	{
                int api_no;
          		if(tunnelInfoHandle.deviceId == NULL)
          		{
          			printf("Tunnel doesn't exist\n");
          			continue;
          		}
                do{
                   	printf("Enter cloud API(99 to return)\n00 - userCreate\n01 - accountCreate\n02 - userRemoved\n03 - accountRemoved\n");
                   	printf("04 - deviceGetStatus\n05 -deviceGetP2PVersion\n06 - userUpdatePassword\n07 - deviceControl\n");
                   	printf("08 - deviceIsOwned\n09 - accountRemoveDevice\n10 - pqmQuotaSet\n11 - userDescribeAccounts\n");
                   	printf("12 - accountAddDevice\n");
                   	printf(">");

                	fgets(buffer, MAX_GETCHAR, stdin);
                	if(char *temp = strchr(buffer,'\n')){
                		*temp = '\0';
                	}
                	api_no = atoi(buffer);
                	if(api_no < MAX_CLOUD_API)
                	{
                		//printf("%s\n",(char*)CLOUD_API[api_no]);
                	    char *recvBuf = (char*)malloc(1024*64);
                	    oz_agentSendRequestAndGetResponse(customerHandle,DEVICE_ID,(char*)CLOUD_API[api_no],tunnelInfoHandle.servicePort,recvBuf);
                		//int result = oz_cloudSendRequestAndGetResponse(customerHandle,DEVICE_ID,"deviceIsOwned",(char*)CLOUD_API[api_no],recvBuf);
                		printf("%s\n",recvBuf);
                		free(recvBuf);
                	}
                	else{
                		printf("API No is out of range %d\n",api_no);
                	}
                }while(api_no != 99);
        	}
            #endif
         	else if(strstr(buffer, "99\n"))
        	{
                exit(0);
        	}
        } //end 'while'
    }
    else // end 'argc'
        printf("#usage argument: device id, user id, user password, CUSTOMER, ENV \n");
}
#ifdef OZCLOUD
/***********************************************************
  Description:

  Parameters :

  Return     :
 **********************************************************/

char* OZ_buildDeviceIsOwned_API(const char *device_id)
{
	cJSON *root = NULL;
	char *temp;
	//int ret;

	root=cJSON_CreateObject();

	cJSON_AddItemToObject(root, "device_id", cJSON_CreateString(device_id));
	temp = cJSON_Print(root);

	cJSON_Delete(root);

	return temp;
}
/***********************************************************
  Description:
 {"userCreate": {"email":"support@flirservices.com","pass":"12345678"}}

  Parameters :

  Return     :
 **********************************************************/

char* OZ_userCreate_API(char *username, char *password)
{
	cJSON *root = NULL;
	char *temp;

	root=cJSON_CreateObject();

	cJSON_AddItemToObject(root, "email", cJSON_CreateString(username));
	cJSON_AddItemToObject(root, "pass", cJSON_CreateString(password));
	temp = cJSON_Print(root);

	cJSON_Delete(root);

	return temp;
}
/***********************************************************
  Description:
{"accountCreate": {"ownerUserMail":"support@flirservices.com"}}

  Parameters :

  Return     :
 **********************************************************/

char* OZ_uaccountCreate_API(char *username)
{
	cJSON *root = NULL;
	char *temp;

	root=cJSON_CreateObject();

	cJSON_AddItemToObject(root, "ownerUserMail", cJSON_CreateString(username));;
	temp = cJSON_Print(root);

	cJSON_Delete(root);

	return temp;
}
/***********************************************************
  Description:
 {"userDescribeAccounts": {"login_name":"support@flirservices.com","password": "12345678"}}

  Parameters :

  Return     :
 **********************************************************/
char* OZ_userDescribeAccounts_API(char *username, char *password)
{
	cJSON *root = NULL;
	char *temp;

	root=cJSON_CreateObject();

	cJSON_AddItemToObject(root, "login_name", cJSON_CreateString(username));
	cJSON_AddItemToObject(root, "password", cJSON_CreateString(password));
	temp = cJSON_Print(root);

	cJSON_Delete(root);

	return temp;
}

/***********************************************************
 Description:

 Parameters :

 Return     :
 **********************************************************/
#if 0
static char*  OZ_userDescribeAccountsParser(char *json)
{
	cJSON *root,*tmp_object,*array_element;

    json = strstr(json, "{");
    root = cJSON_Parse(json);
    if(!root)
    {
    	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "Could not parse JSON: %s\n", json);
        return NULL;
    }
#if 0
	root = cJSON_GetObjectItem(root, "header");

    tmp_object = cJSON_GetObjectItem(root, "response_status");
    if (!tmp_object)
	{
       	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "Element \"response_status\" is found\n");
        return NULL;
	}
   	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "%s\n", tmp_object->valuestring);
#endif
   	root = cJSON_GetObjectItem(root, "body");

	//root = cJSON_GetObjectItem(root, "body");

    tmp_object = cJSON_GetArrayItem(root, 0);
    if (!tmp_object)
	{
       	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "Account id is found: %s\n", json);
        return NULL;
	}
//	elements_number = cJSON_GetArraySize(tmp_object);

//	array_element = cJSON_GetArrayItem(tmp_object, elements_number-1);

    return tmp_object->valuestring;
}
#else
static char* OZ_userDescribeAccountsParser(char *recv_buf)
{
	char *match;
	char *account_id = NULL;

	match = strstr(recv_buf,"\"response_status\":\"OK\"");
	if(match != NULL){
		match = strstr(match,"account_id");
		if(match != NULL){
			account_id = strchr(match,':');
			if(account_id != NULL){
				account_id +=2;
				match = strchr(account_id,'\"');
				if(match != NULL){
					*match = 0;
					strcpy(p_agent_db->account_id,account_id);
			    	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "account id = %s ",p_agent_db->account_id);
				}
			}
		}
	}

	return account_id;
}
#endif
/***********************************************************
  Description:
   {"account_id":"0123456789abcdefghigklmnopqrstuvwxyz01","device_id":"dev1610d8687"}}

  Parameters :

  Return     :
 **********************************************************/
char* OZ_userAddDevice_API(char *account_id, char *device_id)
{
	cJSON *root = NULL;
	char *temp;

	root=cJSON_CreateObject();

	cJSON_AddItemToObject(root, "account_id", cJSON_CreateString(account_id));
	cJSON_AddItemToObject(root, "device_id", cJSON_CreateString(device_id));
	temp = cJSON_Print(root);

	cJSON_Delete(root);

	return temp;
}


static void OZ_setCloudAPI(int api_no,P_SET_CLOUD_PARAMS p_params)
{
	char *json;

    if(api_no == DEVICE_ISOWNED_API)
    {
    	json = OZ_buildDeviceIsOwned_API(p_params->device_id);
    	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "[API=%d]:%s\n[JSON]:%s\n",api_no,STR_API[api_no],json);
    	(void)oz_cloudSendRequestAndGetResponse(p_params->customerHandle,p_params->device_id,STR_API[api_no],json);
    }
    else if(api_no == USER_DESCRIBE_ACCOUNT_API)
    {
    	json = OZ_userDescribeAccounts_API(p_params->user.user_id,p_params->user.password);
    	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "[API=%d]:%s\n[JSON]:%s\n",api_no,STR_API[api_no],json);
    	(void)oz_cloudSendRequestAndGetResponse(p_params->customerHandle,p_params->device_id,STR_API[api_no],json);
    }
    else if(api_no == ACCOUNT_ADD_DEVICE_API)
    {
    	    //Get the account_id
    		json = OZ_userDescribeAccounts_API(p_params->user.user_id,p_params->user.password);
    		OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "[API=%d]:%s\n[JSON]:%s\n",api_no,STR_API[USER_DESCRIBE_ACCOUNT_API],json);
    		char *response = oz_cloudSendRequestAndGetResponse(p_params->customerHandle,p_params->device_id,STR_API[USER_DESCRIBE_ACCOUNT_API],json);
    		char *account_id = OZ_userDescribeAccountsParser(response);

    		//Add device for theaccount_id
    		json = OZ_userAddDevice_API(account_id,p_params->device_id);
    		OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "[API=%d]:%s\n[JSON]:%s\n",api_no,STR_API[USER_DESCRIBE_ACCOUNT_API],json);
        	(void)oz_cloudSendRequestAndGetResponse(p_params->customerHandle,p_params->device_id,STR_API[api_no],json);
    }
    else if(api_no == USER_CREATE_API)
    {
    	json = OZ_userCreate_API(p_params->user.user_id,p_params->user.password);
    	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "[API=%d]:%s\n[JSON]:%s\n",api_no,STR_API[api_no],json);
    	(void)oz_cloudSendRequestAndGetResponse(p_params->customerHandle,p_params->device_id,STR_API[api_no],json);


    }
    else{
    	printf("API No is out of range %d\n",api_no);
    }

}
#endif
#ifdef __cplusplus
}
#endif

