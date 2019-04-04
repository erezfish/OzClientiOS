#ifndef COM_DEF_H_
#define COM_DEF_H_

//#include "udpapp.h"

#ifndef NULL
	#define NULL ( (void *) 0)
#endif

#if !defined (__SIZE_T) && !defined(__SIZE_TYPE__)
#define __SIZE_T
typedef unsigned int size_t;
#endif

#define  OZ_EVENT   1
#define  OZ_OK      0
#define  OZ_ERROR  -1
typedef int oz_ret_t;

#ifdef USE_BOOLSTD   //for P2P 
#include <stdbool.h>
#else
#ifndef __STDBOOL_H
#define __STDBOOL_H
//typedef enum {false = 0, true} bool;
#endif
#endif

typedef enum {FALSE = 0, TRUE} boolean;

#define DAHUA_DEVICE_GROUP      defined(MODEL_dua_lhv20xx_DFS3) || defined (MODEL_dua_lnr6xx)
#define RAYSHARP_DEVICE_GROUP   defined(MODEL_ray_lh0414) || defined (MODEL_ray_lh0604) || defined (MODEL_ray_lhv41xx) || defined(MODEL_ray_lnk71xx) || \
	                            defined(MODEL_ray_lnk72xx) || defined(MODEL_ray_lha2108) || defined(MODEL_ray_lha4104) || defined(MODEL_ray_lha2116) || defined (MODEL_dua_nvr41xx) /* same as ray_71xx*/

#define FLIR_DEVICE_GROUP       DAHUA_DEVICE_GROUP || RAYSHARP_DEVICE_GROUP

#define OZVISION_DEVICE_GROUP   defined(MODEL_ozpro) || defined (MODEL_peq_mom) || defined (MODEL_cent_a8) || defined (MODEL_hikvis) || defined (MODEL_flight) || \
                                defined (MODEL_swann)  || defined (MODEL_nowl_hkv) || defined (MODEL_lgit)|| defined (MODEL_aoni) || defined(MODEL_telit) || \
								defined (MODEL_hikcori)

#define HIKVISION_GROUP         defined (MODEL_peq_mom) || defined (MODEL_hikvis) || defined (MODEL_flight) || defined (MODEL_nowl_hkv) || defined (MODEL_hikcori)

#define OZVISION_SCHEDULER      defined(MODEL_ozpro) || defined (MODEL_peq_mom) || defined (MODEL_cent_a8) || defined (MODEL_hikvis) || defined (MODEL_flight) || \
                                defined (MODEL_swann)  || defined (MODEL_nowl_hkv) || defined (MODEL_lgit) || defined(MODEL_telit) || defined (MODEL_hikcori)

typedef enum{
	CAMERA1,
	MAX_CAMERA
}CAMERA_E;

typedef enum{
	UNINITIALIZED,
	SDCARD,
	RAMDISK,
	MAX_STORAGE_TYPE
}VID_STORAGE_E;

#if defined (MODEL_lgit)
#define MAX_RCV_SIZE     (128*1024)  // derived from tumbnail size > 17K
#elif defined (MODEL_hikvis) || defined (MODEL_hikcori) || defined (MODEL_flight) || defined (MODEL_nowl_hkv)
#define MAX_RCV_SIZE     (2*1024)   // derived from device info size > 1K
#else
#define MAX_RCV_SIZE     (1*1024)
#endif

//TODO for 8/16 channels
#if defined  (DEVICE_64CH)
  #define MAX_DEVICE_CHANNEL         64      // DVR 64CG
#elif defined  (DEVICE_32CH)
  #define MAX_DEVICE_CHANNEL         32      // DVR 32CH
#else
#if defined (MODEL_peq_mom)  || defined (MODEL_hikvis) || defined (MODEL_flight) || \
	defined (MODEL_swann) || defined(MODEL_chk_a7)  || defined(MODEL_fxt_a9) || \
	defined(MODEL_telit) || defined (MODEL_hikcori) || defined (MODEL_aoni)  // camera
  #define MAX_DEVICE_CHANNEL          1
#elif defined (MODEL_nowl_hkv)
  #define MAX_DEVICE_CHANNEL         8
#else
  #define MAX_DEVICE_CHANNEL         16      // DVR 8/16 CH
#endif
#endif

typedef struct _DEVICE_RX_DATA_
{
	char data[MAX_RCV_SIZE];
	int len;
	int sock;
}device_rx_data;

#if defined MODEL_peq_mom
//#define SDCARD_PATH     "/usr/tfcard"
#define DEV_SD_PATH		"/dev/mmcblk0p1"
#elif defined( MODEL_hikvis) || defined( MODEL_hikcori) || defined (MODEL_flight) || defined (MODEL_nowl_hkv)
//#define SDCARD_PATH     "/mnt/mmc01"
#define DEV_SD_PATH		"/dev/mmc01"
#elif defined MODEL_swann
//#define SDCARD_PATH     "/tmp/sdcard"
#define DEV_SD_PATH		"/dev/sda1"
#elif defined MODEL_ozpro
//#define SDCARD_PATH     "/mnt/sd"
#define DEV_SD_PATH		"/dev/mmc0p1"
#elif defined MODEL_aoni
//#define SDCARD_PATH     "/mnt/sd_card"
#define DEV_SD_PATH		"/dev/mmcblk0p1"
#else
//#define SDCARD_PATH     ""
#define DEV_SD_PATH		""
#endif

//#define AGENT_LOGFILE_PATH     "/var/log"    //default

#define CAMERA_MAX_DEVICES     1

#define KEY_SIZE               16
#define MAX_DEBUG_ON_SIZE      10
#define MAX_DEVICE_ID_SIZE     32
#define MAX_GET_CNFG_REQ_SIZE  256
#define MAX_DNS_SIZE           256
#define MAX_DEVICE_FILE_SIZE   1024
#define MAX_PATH_SIZE          256
#define MAX_DEVICE_NAME_SIZE   64
#define MAX_RTSP_URL_SIZE      128
#define MAX_TUMBNAIL_API_SIZE  128
#define MAX_MSG_SIZE   		   512
#define MAX_BASIC_AUTH 		   128
#define MAX_TIMEZONE		   128
#define MAX_CAMNAME		       128

// Key hiding
#define SEED 25
#define NEEDLE_SIZE (KEY_SIZE * 2)
#define HAYSTACK_SIZE 1024

//macros
enum{
	BIT_0,
	BIT_1,
	BIT_2,
	BIT_3,
	BIT_4,
	BIT_5,
	BIT_6,
	BIT_7
};
#define h_to_a(x)      ( ((x) >= 10 && (x) <= 15) ?   (x)-10+0x41  : (x)+'0' )
#define a_to_h(c)      ( ((c) >= 'A' && (c) <= 'F') ? (c)-'A'+10 : (c)-'0' )
#define InRange(a,b,c) ( (b) <= (a) && (a) <= (c) )

#define TEST_BIT(var,bit)   ((var) & (1<<(bit)))
#define SET_BIT(var,bit)    ((var) |=(1<<(bit)))
#define CLR_BIT(var,bit)    ((var) &=~(1<<(bit)))

#define LOBYTE(hw) ((BYTE)(hw))
#define HIBYTE(hw) ((BYTE)((WORD)(hw)>>8))

#define HI_BYTE_D(x)		((x)>>8)
#define LO_BYTE_D(x)		((x) & 0x00ff)

#define LOWORD(w) ((u16)(u32)(w))
#define HIWORD(w) ((u16)((u32)(w)>>16))

#define MAKEWORD(high, low) (((u16)((u8)(high))<<8) | (u16)((u8)(low)))
#define MAKELONG(high, low) (((u32)((u16)(high))<<16) | (u32)((u16)(low)))

typedef union
{
    unsigned char  buffer[2];
    struct
    {
       unsigned short  hword;
    }val;
}WORD_U;

typedef union
{
    unsigned char  buffer[4];
    struct
    {
       unsigned long  lword;
    }val;
}LONG_U;

typedef union
{
    unsigned char  buffer[8];
    struct
    {
       unsigned long long lword;
    }val;
}DUBLE_U;

#define write_bit(buffer,loc,val) \
{ \
	unsigned int bit_no,i,mask; \
	i=loc/8; \
	bit_no=loc%8; \
	mask=*(buffer+i); \
	if(val) \
	  SET_BIT(mask,bit_no); \
	else \
	  CLR_BIT(mask,bit_no); \
   *(buffer+i)=mask; \
} \

#define read_bit(buffer,loc,val) \
{ \
    unsigned int bit_no,i,mask; \
	i=loc/8; \
	bit_no=loc%8; \
	mask=*(buffer+i); \
    val=TEST_BIT(mask,bit_no); \
    if(val) \
      val = 1; \
    return val; \
} \

#endif /* COM_DEF_H_ */
