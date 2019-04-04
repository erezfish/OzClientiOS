/******************************************************************************
* File Name          : agent_api.cpp
* Author             : Atzmon Dahan
* Created on		 : Jan 31, 2019
* Description        : Agent build APIs for APP
*
************************************************************************************
* History:
*
***********************************************************************************/

#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "cJSON.h"
#include "OZ_memory.h"
#include "agent_api.h"
#include "OzUtils.h"


#ifdef __cplusplus
extern "C" {
#endif

/*------------ External reference -------------------------------------------------- */

/*-------------Defines---------------------------------------------------------------*/
static char* OZ_Null_API(int api_no);
static char* OZ_setCameraName_API(int api_no);
static char* OZ_setPriovacyMode_API(int api_no);
static char* OZ_setStartRecord_API(int api_no);
static char* OZ_setStopRecord_API(int api_no);
static char* OZ_localstorageGetList_API(int api_no);


const FUNCTION_API_T
	OZ_setAgentAPI[MAX_AGENT_API] =
{
   {OZ_Null_API},
   {OZ_setCameraName_API},
   {OZ_Null_API},
   {OZ_Null_API},
   {OZ_setStartRecord_API},
   {OZ_setStopRecord_API},
   {OZ_Null_API},
   {OZ_Null_API},
   {OZ_Null_API},
   {OZ_Null_API},
   {OZ_Null_API},
   {OZ_Null_API},
   {OZ_Null_API},
   {OZ_Null_API},
   {OZ_Null_API},
   {OZ_Null_API},
   {OZ_Null_API},
   {OZ_localstorageGetList_API},
   {OZ_Null_API},
   {OZ_Null_API},
   {OZ_Null_API},
   {OZ_Null_API}
};

/*-------------Variables ------------------------------------------------------------*/

static const char AGENT_API[MAX_AGENT_API][128] =
{
  "{\"req\": \"get camera name\"}",
  "{\"req\": \"set camera name\", \"name\": <value>}",
  "{\"req\": \"get privacy mode\"}",
  "{\"req\": \"set privacy mode\", \"mode\": \"<value>\"}",
  "{\"req\": \"start recording\", \"stream name\", <value>}",
  "{\"req\": \"stop recording\", \"stream name\", <value>}",
  "{\"req\": \"take snapshot\"}",
  "{\"req\": \"get garage door state\", \"number\": <value>\"}",
  "{\"req\": \"set garage door state\", \"number\": <value>, \"state\": \"<value>\"}",
  "{\"req\": \"toggle garage door state\", \"number\": <value>}",
  "{\"req\": \"fw upgrade control\", \"action\": \"<value>\"}",
  "{\"req\": \"fw upgrade status\"}",
  "{\"req\": \"flip image\"}",
  "{\"req\": \"set two way audio enabled\", \"enabled\": <value>}",
  "{\"req\": \"localstorage device get info\"}",
  "{\"req\": \"localstorage device format\"}",
  "{\"req\": \"localstorage get directory name\"}",
  "{\"req\": \"localstorage get list\", start_time: <value>, end_time: <value>, file_type: <value>}",
  "{\"req\": \"localstorage playback\", filepath: <value>}",
  "{\"req\": \"localstorage delete\", filepath: <value>}",
  "{\"req\": \"localstorage delete range\", start_time: <value>, end_time: <value>}",
  "{\"req\": \"set time zone\", \"offset\": <value>}"
};


/*-------------Prototype------------------------------------------------------------*/

static time_t create_timestamp(void)
{
	struct tm t;
    time_t t_of_day;

    t.tm_year = 2019-1900;
    t.tm_mon = 0;           // Month, 0 - jan
    t.tm_mday = 30;          // Day of the month
    t.tm_hour = 14;
    t.tm_min = 00;
    t.tm_sec = 00;
    t.tm_isdst = -1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
    t_of_day = mktime(&t);

	OZCLIENT_PRINTF(OSNCLIENT_LOG_INFO, __FUNCTION__, __LINE__, "t_of_day:%d\n",t_of_day);

	return t_of_day;
}

static char* OZ_Null_API(int api_no)
{
	return ((char*)AGENT_API[api_no]);//.data);
}

static char* OZ_setCameraName_API(int api_no)
{
	cJSON *root = NULL;
	char *temp;

	root=cJSON_CreateObject();

	cJSON_AddItemToObject(root, "req", cJSON_CreateString("set camera name"));
	cJSON_AddItemToObject(root, "name", cJSON_CreateString("Cori Cam"));
	temp = cJSON_Print(root);

	cJSON_Delete(root);

	return temp;

}

static char* OZ_setPriovacyMode_API(int api_no)
{
	cJSON *root = NULL;
	char *temp;

	root=cJSON_CreateObject();

	cJSON_AddItemToObject(root, "req", cJSON_CreateString("set privacy mode"));
	cJSON_AddItemToObject(root, "mode", cJSON_CreateString("off"));
	temp = cJSON_Print(root);

	cJSON_Delete(root);

	return temp;
}

static char* OZ_setStartRecord_API(int api_no)
{
	cJSON *root = NULL;
	char *temp;

	root=cJSON_CreateObject();

	cJSON_AddItemToObject(root, "req", cJSON_CreateString("start recording"));
	cJSON_AddItemToObject(root, "stream name", cJSON_CreateString("rtsp://appagent:streaming@127.0.0.1:554/h264/ch1/main/av_stream"));
	temp = cJSON_Print(root);

	cJSON_Delete(root);

	return temp;
}

static char* OZ_setStopRecord_API(int api_no)
{
	cJSON *root = NULL;
	char *temp;

	root=cJSON_CreateObject();

	cJSON_AddItemToObject(root, "req", cJSON_CreateString("stop recording"));
	cJSON_AddItemToObject(root, "stream name", cJSON_CreateString("rtsp://appagent:streaming@127.0.0.1:554/h264/ch1/main/av_stream"));
	temp = cJSON_Print(root);

	cJSON_Delete(root);

	return temp;
}

static char* OZ_localstorageGetList_API(int api_no)
{
	cJSON *root = NULL;
	char *temp;

	root=cJSON_CreateObject();

	time_t start_time = create_timestamp();
	time_t end_time = start_time + 180;

	cJSON_AddItemToObject(root, "req", cJSON_CreateString("localstorage get list"));
	cJSON_AddItemToObject(root, "start_time", cJSON_CreateNumber(start_time));
	cJSON_AddItemToObject(root, "end_time", cJSON_CreateNumber(end_time));
	cJSON_AddItemToObject(root, "file_type", cJSON_CreateString("mp4"));
	temp = cJSON_Print(root);

	cJSON_Delete(root);

	return temp;

}


#ifdef __cplusplus
}
#endif
