/* 
 * File:   libexf1.h
 * Author: Jens Skovgaard Olsen
 *
 * Created on 30. november 2010, 17:33
 */

#ifndef EXF1API_H
#define	EXF1API_H

#include "libexf1.h"

typedef enum {
    SHUTTER_NORMAL,
    SHUTTER_CONTINOUS,
    SHUTTER_PRERECORD,
} SHUTTER_MODES;

typedef enum {
    MOVIE_STD,
    MOVIE_HD,
    MOVIE_HS,
} MOVIE_MODES;

void start_config(char enableStillImage, char enablePreRecord);
void stop_config();

int init_camera(void);
void half_shutter(void);
void shutter(char *fileName, char *thumbNail, int delay);
void terminate_camera(void);

void setup_movie(MOVIE_MODES movieMode, char enablePreRecord);
void setup_shutter(SHUTTER_MODES shutterMode, char enablePreRecord);

void movie(char filename[], int delay);
void setup_pc_monitor(void);
void setup_iso(WORD iso);
void setup_aperture(WORD aperture);
void setup_exposure(WORD exposure);
void setup_focus(WORD focus); 

void zoom(char zoomIn, char continousZoom);
void focus(char focusIn, char continousFocus);

int grap_pc_monitor_frame(char *jpg_img);
void exit_camera(void);

extern char halfShutterPressed;
extern char stillImageEnabled;
extern char preRecordEnabled;
extern char continousShutterEnabled;

#endif	/* LIBEXF1_H */

