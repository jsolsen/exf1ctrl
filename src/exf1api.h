/* 
 * File:   libexf1.h
 * Author: Jens Skovgaard Olsen
 *
 * Created on 30. november 2010, 17:33
 */

#ifndef EXF1API_H
#define	EXF1API_H

#include "libexf1.h"

#define CV_NO_BACKWARD_COMPATIBILITY
#include <cv.h>
#include <ctype.h>
#include <jpeglib.h>
//#include <cderror.h>

#define JMESSAGE(code,string) string ,

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

void startConfig(char enableStillImage, char enablePreRecord);
void stopConfig();

int initCamera(void);
void halfShutter(void);
void shutter(char *fileName, char *thumbNail, int delay);
void terminateCamera(void);

void setupMovie(MOVIE_MODES movieMode, char enablePreRecord);
void setupShutter(SHUTTER_MODES shutterMode, char enablePreRecord);

void movie(char filename[], int delay);
void setupMonitor(char isPc);
void setupIso(WORD iso);
void setupAperture(WORD aperture);
void setupExposure(WORD exposure);
void setupFocus(WORD focus);

void zoom(char zoomIn, char continousZoom);
void focus(char focusIn, char continousFocus);

int grapPcMonitorFrame(char *jpgImage);
void getCameraFrame(IplImage* frame); 
void exitCamera(void);

extern char halfShutterPressed;
extern char stillImageEnabled;
extern char preRecordEnabled;
extern char continousShutterEnabled;

#endif	/* LIBEXF1_H */

