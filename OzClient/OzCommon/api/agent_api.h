/*
 * agent_api.h
 *
 *  Created on: Jan 31, 2019
 *      Author: Atzmon
 */

#ifndef AGENT_API_H_
#define AGENT_API_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
   GET_CAMERA_API,
   SET_CAMERA_API,
   GET_PRIVACY_API,
   SET_PRIVACY_API,
   START_RECORDING_API,
   STOP_RECORDING_API,
   SNAPSHOT_API,
   GET_GARAGE_DOOR_API,
   SET_GARAGE_DOOR_API,
   TOGGLE_GARAGE_DOOR_API,
   UPGRADE_CTRL_API,
   UPGRADE_STATUS_API,
   FLIP_IMAGE_API,
   SET_TOWAY_VOICE_API,
   LOCAL_STORAGE_GET_INFO_API,
   LOCAL_STORAGE_FORMAT_API,
   LOCAL_STORAGE_GET_DIR_NAME_API,
   LOCAL_STORAGE_GET_LIST_API,
   LOCAL_STORAGE_PLAYBACK_API,
   LOCAL_STORAGE_DELETE_API,
   LOCAL_STORAGE_DELETE_RANGE_API,
   SET_TIMEZONE_API,
   MAX_AGENT_API
}AGENT_API_E;

typedef struct
{
    char* (*agent_api_F)(int api_no);
}FUNCTION_API_T;

extern const FUNCTION_API_T OZ_setAgentAPI[MAX_AGENT_API];

#ifdef __cplusplus
}
#endif
#endif /* AGENT_API_H_ */
