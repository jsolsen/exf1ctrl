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

#define GET_WORD(ptr)  ((0xFF & *ptr) + ((0xFF & *(ptr+1)) << 8))
#define GET_DWORD(ptr) ((0xFF & *ptr) + ((0xFF & *(ptr+1)) << 8) + ((0xFF & *(ptr+2)) << 16) + ((0xFF & *(ptr+3)) << 24))

#define SET_WORD(ptr, val)  ({*ptr = (val & 0xFF); *(ptr+1) = (val & 0xFF00) >> 8;})
#define SET_DWORD(ptr, val) ({*ptr = (val & 0xFF); *(ptr+1) = (val & 0xFF00) >> 8; *(ptr+2) = (val & 0xFF0000) >> 16; *(ptr+3) = (val & 0xFF000000) >> 24;})

usb_dev_handle *open_dev(void);
int string_match(char s1[], char s2[], int length);
void usbCmdGen(short int cmd, short int postCmdReads, int nCmdParameters, char cmdParameters[]);
int init_camera(void);
void half_shutter(void);
void shutter(char filename[], char thumbnail[]);
void terminate_camera(void);
void setup_prerecord_movie_hs(void);
void movie(char filename[], int delay);
void setup_pc_monitor(void);
int grap_pc_monitor_frame(char *jpg_img);
void exit_camera(void);

#endif	/* LIBEXF1_H */

