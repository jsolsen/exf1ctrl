/* 
 * File:   libexf1.h
 * Author: Jens Skovgaard Olsen
 *
 * Created on 30. november 2010, 17:33
 */

#ifndef LIBEXF1_H
#define	LIBEXF1_H

#include <usb.h>
#include <stdio.h>
#include <stdarg.h>

#define TRUE      1
#define FALSE     0 

#define NO_READS  0
#define ONE_READ  1
#define TWO_READS 2

#define MY_VID 0x07CF
#define MY_PID 0x1023

#define EP_IN 0x81
#define EP_OUT 0x02
#define EP_INT 0x83

#define BUF_SIZE 512
#define IMG_BUF_SIZE 65536
#define TIME_OUT 3500

#define WORD                short unsigned int      // 16bits
#define DWORD               unsigned int            // 32bits

#define GET_WORD(ptr)  ((0xFF & *(ptr)) + ((0xFF & *(ptr+1)) << 8))
#define GET_DWORD(ptr) ((0xFF & *(ptr)) + ((0xFF & *(ptr+1)) << 8) + ((0xFF & *(ptr+2)) << 16) + ((0xFF & *(ptr+3)) << 24))

#define SET_WORD(ptr, val)  ({*ptr = (val & 0xFF); *(ptr+1) = (val & 0xFF00) >> 8;})
#define SET_DWORD(ptr, val) ({*ptr = (val & 0xFF); *(ptr+1) = (val & 0xFF00) >> 8; *(ptr+2) = (val & 0xFF0000) >> 16; *(ptr+3) = (val & 0xFF000000) >> 24;})

#define SESSION_ID          0x19760615

#define CMD_GET_DEVICE_INFO 0x1001
#define CMD_OPEN_SESSION    0x1002
#define CMD_CLOSE_SESSION   0x1003
#define CMD_GET_STORAGE_IDS 0x1004

#define CMD_GET_PROP_DESC   0x1014

#define CMD_READ            0x1015
#define CMD_WRITE           0x1016
#define CMD_OK              0x2001

#define CMD_STILL_START     0x9001
#define CMD_STILL_STOP      0x9002

#define CMD_FOCUS           0x9007
#define CMD_CF_PRESS        0x9009
#define CMD_CF_RELEASE      0x900A
#define CMD_GET_OBJECT_INFO 0x900C

#define CMD_SHUTTER         0x9024
#define CMD_GET_STILL_HANDLES 0x9027
#define CMD_STILL_RESET     0x9028
#define CMD_HALF_PRESS      0x9029
#define CMD_HALF_RELEASE    0x902A
#define CMD_CS_PRESS        0x902B
#define CMD_CS_RELEASE      0x902C

#define CMD_ZOOM            0x902D
#define CMD_CZ_PRESS        0x902E
#define CMD_CZ_RELEASE      0x902F

#define CMD_MOVIE_START     0x9041
#define CMD_MOVIE_STOP      0x9042
#define CMD_MOVIE_PRESS     0x9043
#define CMD_MOVIE_RELEASE   0x9044
#define CMD_GET_MOVIE_HANDLES 0x9045
#define CMD_MOVIE_RESET     0x9046

#define CMD_GET_OBJECT      0x9025
#define CMD_GET_THUMBNAIL   0x9026

#define DATA_CAPTURE        0x0001
#define DATA_MONITOR        0x0002

// shutter  usbCmdGen(0x9024, NO_READS, 0, NULL);
// movie    usbCmdGen(0x9044, NO_READS, 0, NULL);

// get mov usbCmdGen(0x9025, NO_READS, 4, (char[]){0x01, 0x00, 0x00, 0x00});

// Camera settings.

#define ADDR_BATTERY_LEVEL  0x00005001
#define ADDR_FUNCTIONALITY  0x00005002
#define ADDR_IMAGE_SIZE     0x00005003
#define ADDR_QUALITY        0x00005004
#define ADDR_WHITE_BALANCE  0x00005005
#define ADDR_APERTURE       0x00005007
#define ADDR_FOCUS          0x0000500A
#define ADDR_METERING       0x0000500B
#define ADDR_FLASH          0x0000500C
#define ADDR_SHUTTER_SPEED  0x0000500D
#define ADDR_EXPOSURE       0x0000500E
#define ADDR_ISO            0x0000500F
#define ADDR_EV             0x00005010

#define ADDR_CAPTURE_MODE   0x00005013
#define ADDR_CONTRAST       0x00005014
#define ADDR_SHARPNESS      0x00005015
#define ADDR_METERING_MODE  0x0000501C

#define ADDR_MONITOR        0x0000D001 
#define ADDR_STORAGE        0x0000D002 //Not reported by DeviceInfo?
#define ADDR_UNKNOWN_1      0x0000D004
#define ADDR_UNKNOWN_2      0x0000D005
#define ADDR_UNKNOWN_3      0x0000D007
#define ADDR_RECORD_LIGHT   0x0000D008
#define ADDR_UNKNOWN_4      0x0000D009
#define ADDR_UNKNOWN_5      0x0000D00A
#define ADDR_MOVIE_MODE     0x0000D00B
#define ADDR_HD_SETTING     0x0000D00C
#define ADDR_HS_SETTING     0x0000D00D
#define ADDR_CS_HIGH_SPEED  0x0000D00F
#define ADDR_CS_UPPER_LIMIT 0x0000D010
#define ADDR_CS_SHOT        0x0000D011
#define ADDR_UNKNOWN_6      0x0000D012
#define ADDR_UNKNOWN_7      0x0000D013
#define ADDR_UNKNOWN_8      0x0000D015
#define ADDR_UNKNOWN_9      0x0000D017
#define ADDR_UNKNOWN_10     0x0000D018
#define ADDR_UNKNOWN_11     0x0000D019
#define ADDR_UNKNOWN_12     0x0000D01A
#define ADDR_UNKNOWN_13     0x0000D01B
#define ADDR_UNKNOWN_14     0x0000D01C
#define ADDR_UNKNOWN_15     0x0000D01D
#define ADDR_UNKNOWN_16     0x0000D020
#define ADDR_UNKNOWN_17     0x0000D030
#define ADDR_UNKNOWN_18     0x0000D080

#define DATA_FUNC_BASIC     0x0000
#define DATA_FUNC_EXTENDED  0x8001

#define DATA_CAPTURE_NORMAL 0x0001
#define DATA_CAPTURE_CS     0x8001
#define DATA_CAPTURE_PREREC 0x8002

#define DATA_IS0_AUTO       0xFFFF
#define DATA_IS0_100        0x0064
#define DATA_IS0_200        0x00C8
#define DATA_IS0_400        0x0190
#define DATA_IS0_800        0x0320
#define DATA_IS0_1600       0x0640

#define DATA_MOVIE_MODE_STD 0x0000
#define DATA_MOVIE_MODE_HD  0x0001
#define DATA_MOVIE_MODE_HS  0x0002

#define DATA_APERTURE_F2_7  0x0001
#define DATA_APERTURE_F3_0  0x0002
#define DATA_APERTURE_F3_3  0x0003
#define DATA_APERTURE_F3_8  0x0004
#define DATA_APERTURE_F4_2  0x0005
#define DATA_APERTURE_F4_7  0x0006
#define DATA_APERTURE_F5_3  0x0007
#define DATA_APERTURE_F6_0  0x0008
#define DATA_APERTURE_F6_7  0x0009
#define DATA_APERTURE_F7_5  0x000A

#define DATA_FOCUS_MANUAL   0x0001
#define DATA_FOCUS_AF       0x0002
#define DATA_FOCUS_MACRO    0x0003
#define DATA_FOCUS_INFINITY 0x8001

#define DATA_EXPOSURE_M     0x0001
#define DATA_EXPOSURE_AUTO  0x0002
#define DATA_EXPOSURE_A     0x0003
#define DATA_EXPOSURE_S     0x0004

#define DATA_ZOOM_OUT       0x0000
#define DATA_ZOOM_IN        0x0001

#define DATA_FOCUS_OUT      0x0000
#define DATA_FOCUS_IN       0x0001

#define TYPE_CMD            0x0001
#define TYPE_DATA           0x0002
#define TYPE_RESPONSE       0x0003
#define TYPE_EVENT          0x0004

#define EVT_DEVICE_INFO_CHANGED 0x4008
#define EVT_ZOOM_CHANGED    0xC011
#define EVT_FOCUS_CHANGED   0xC012

#define EVT_FOCUS_OK        0xC014
#define EVT_UNKNOWN_CHANGED 0xC015

struct _WORD_DATA_SET {
    DWORD noItems;
    WORD  data[];
};
typedef struct _WORD_DATA_SET WORD_DATA_SET;

struct _DWORD_DATA_SET {
    DWORD noItems;
    DWORD  data[];
};
typedef struct _DWORD_DATA_SET DWORD_DATA_SET;

// GCC pack pragmas.
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))

// VC++ pack pragmas,
//#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )

PACK(
struct _STRING_DATA_SET {
    char noItems;
    WORD data[];
});

typedef struct _STRING_DATA_SET STRING_DATA_SET;

struct _PTP_CONTAINER {
    DWORD length;
    WORD  type;
    WORD  code;
    DWORD trans_id;
    union {
        struct {
            WORD param1;
            WORD param2;
            WORD param3;
            WORD param4;
        } word_params;
        struct {
            DWORD param1;
            DWORD param2;
        } dword_params;
        unsigned char data[BUF_SIZE-12];
    } payload;
};
typedef struct _PTP_CONTAINER PTP_CONTAINER;

struct _PTP_DEVICE_INFO {
    WORD  standardVersion;
    DWORD vendorExtensionID;
    WORD  vendorExtensionVersion;
    STRING_DATA_SET *vendorExtensionDesc;
    WORD  functionalMode;
    WORD_DATA_SET *operationsSupported;
    WORD_DATA_SET *eventsSupported;
    WORD_DATA_SET *devicePropertiesSupported;
    WORD_DATA_SET *captureFormats;
    WORD_DATA_SET *imageFormats;
    STRING_DATA_SET *manufacturer;
    STRING_DATA_SET *model;
    STRING_DATA_SET *deviceVersion;
    STRING_DATA_SET *serialNumber;
};
typedef struct _PTP_DEVICE_INFO PTP_DEVICE_INFO;

struct _RANGE_FORM {
    void *minimumValue;
    void *maximumValue;
    void *stepSize;
};
typedef struct _RANGE_FORM RANGE_FORM;

struct _ENUM_FORM {
    WORD numberOfValues;
    void **supportedValue;
};
typedef struct _ENUM_FORM ENUM_FORM;

PACK(
struct _PTP_DEVICE_PROPERTY {
    WORD code;
    WORD dataType;
    char getSet;
    void *defaultValue;
    void *currentValue;
    char formFlag;
    union {
        ENUM_FORM enumForm;
        RANGE_FORM rangeForm;
    } form;
});
typedef struct _PTP_DEVICE_PROPERTY PTP_DEVICE_PROPERTY;

#define PROPERTY_FORM_NONE  	0x00
#define PROPERTY_FORM_RANGE     0x01
#define PROPERTY_FORM_ENUM	0x02

#define DATA_TYPE_CHAR		0x0002
#define DATA_TYPE_WORD  	0x0004
#define DATA_TYPE_DWORD 	0x0006
#define DATA_TYPE_STRING	0xFFFF

#define TO_FILE                 1
#define TO_MEM                  2

struct _PTP_OBJECT_INFO {
	DWORD storageId;
	WORD objectFormat;
	WORD protectionStatus;
	DWORD objectCompressedSize;
	WORD thumbFormat;
	DWORD thumbCompressedSize;
	DWORD thumbPixWidth;
	DWORD thumbPixHeight;
	DWORD imagePixWidth;
	DWORD imagePixHeight;
	DWORD imageBitDepth;
	DWORD parentObject;
	WORD associationType;
	DWORD associationDesc;
	DWORD sequenceNumber;
	STRING_DATA_SET *fileName;
	STRING_DATA_SET *captureDate;
	STRING_DATA_SET *modificationDate;
	STRING_DATA_SET *keyWords;
};
typedef struct _PTP_OBJECT_INFO PTP_OBJECT_INFO;

usb_dev_handle *open_dev(void);

void exf1Cmd(WORD cmd, ...);
void usbCmdGen(short int cmd, short int postCmdReads, int nCmdParameters, char cmdParameters[]);
void usbTx(WORD cmd, WORD cmdType, int nCmdParameterBytes, DWORD cmdParameter1, DWORD cmdParameter2);
int usbRx();
int usbRxToFile(char *fileName);
int usbRxEvent(); 
void usbGetStatus();
int usbInit();

void printStringDataSet(char *pDescrition, STRING_DATA_SET *pDataSet);
void printWordDataSet(char *pDescrition, WORD_DATA_SET *pDataSet);
void printDwordDataSet(char *pDescrition, DWORD_DATA_SET *pDataSet);

void setDeviceInfo(char *pData);
void printDeviceInfo();

void printEnumDataSet(char *pDescrition, ENUM_FORM *pDataSet, WORD dataType);
void setDeviceProperty(char *pData); 
void printDeviceProperty();

void setObjectInfo(char *pData);
void printObjectInfo();

WORD getStringDataSet(STRING_DATA_SET **dst, char *src);
DWORD getWordDataSet(WORD_DATA_SET **dst, char *src);
DWORD getDwordDataSet(DWORD_DATA_SET **dst, char *src);

void start_config(char enableStillImage, char enablePreRecord);
void stop_config();

extern usb_dev_handle *dev;
extern char tmp[BUF_SIZE];
extern char img[IMG_BUF_SIZE];
extern PTP_DEVICE_INFO deviceInfo;
extern PTP_DEVICE_PROPERTY deviceProperty;
extern PTP_OBJECT_INFO objectInfo;
extern DWORD_DATA_SET *objectHandles;
extern DWORD zoomSetting;
extern DWORD focusSetting;
extern DWORD USB_CMD_ID;

#endif	/* LIBEXF1_H */

