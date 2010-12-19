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

#define CMD_START 1
#define CMD_END   2

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

#define CMD_HALF_PRESS      0x9029
#define CMD_HALF_RELEASE    0x902A

#define CMD_MOVIE_START     0x9041
#define CMD_MOVIE_STOP      0x9042

#define CMD_TRANSFER        0x9025

#define DATA_CAPTURE        0x0001
#define DATA_MONITOR        0x0002

// shutter  usbCmdGen(0x9024, NO_READS, 0, NULL);
// movie    usbCmdGen(0x9044, NO_READS, 0, NULL);

// get mov usbCmdGen(0x9025, NO_READS, 4, (char[]){0x01, 0x00, 0x00, 0x00});

// Camera settings.

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

#define ADDR_MONITOR        0x0000D001 
#define ADDR_STORAGE        0x0000D002
#define ADDR_RECORD_LIGHT   0x0000D008
#define ADDR_MOVIE_MODE     0x0000D00B
#define ADDR_HD_SETTING     0x0000D00C
#define ADDR_HS_SETTING     0x0000D00D
#define ADDR_CS_HIGH_SPEED  0x0000D00F
#define ADDR_CS_UPPER_LIMIT 0x0000D010
#define ADDR_CS_SHOT        0x0000D011

#define DATA_FUNC_BASIC     0x0000
#define DATA_FUNC_EXTENDED  0x8001

#define DATA_IS0_AUTO       0xFFFF
#define DATA_IS0_100        0x0064
#define DATA_IS0_200        0x00C8
#define DATA_IS0_400        0x0190
#define DATA_IS0_800        0x0320
#define DATA_IS0_1600       0x0640

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

#define DATA_FOCUS_AF       0x0002
#define DATA_FOCUS_MACRO    0x0003
#define DATA_FOCUS_INFINITY 0x0004
#define DATA_FOCUS_MANUAL   0x0005

#define DATA_EXPOSURE_M     0x0001
#define DATA_EXPOSURE_AUTO  0x0002
#define DATA_EXPOSURE_A     0x0003
#define DATA_EXPOSURE_S     0x0004

#define TYPE_CMD            0x0001
#define TYPE_DATA           0x0002
#define TYPE_RESPONSE       0x0003
#define TYPE_EVENT          0x0004


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


usb_dev_handle *open_dev(void);
int string_match(char s1[], char s2[], int length);

//void exf1Cmd(WORD cmd, DWORD addr, WORD data);
void exf1Cmd(WORD cmd, ...);
void usbCmdGen(short int cmd, short int postCmdReads, int nCmdParameters, char cmdParameters[]);
void usbTx(WORD cmd, WORD cmdType, int nCmdParameterBytes, DWORD cmdParameters);
void usbRx(); 

void start_config(char enableStillImage, char enablePreRecord);
void stop_config();

int init_camera(void);
void half_shutter(void);
void shutter(char filename[], char thumbnail[]);
void terminate_camera(void);

void setup_movie_hs(char enablePreRecord); 
void setup_movie_hd(char enablePreRecord); 

void movie(char filename[], int delay);
void setup_pc_monitor(void);
void setup_iso(WORD iso);
void setup_aperture(WORD aperture);
void setup_exposure(WORD exposure); 

int grap_pc_monitor_frame(char *jpg_img);
void exit_camera(void);

#endif	/* LIBEXF1_H */

