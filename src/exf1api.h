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

class exf1api {

	public:
	enum 			SHUTTER_MODES {SHUTTER_NORMAL, SHUTTER_CONTINOUS, SHUTTER_PRERECORD};
	enum			MOVIE_MODES {MOVIE_STD, MOVIE_HD, MOVIE_HS};
	enum		    EV {M2_0, M1_7, M1_3, M1_0, M0_7, M0_3, P0_0, P0_3, P0_7, P1_0, P1_3, P1_7, P2_0};

	exf1api();

	char halfShutterPressed;
	char stillImageEnabled;
	char preRecordEnabled;
	char continousShutterEnabled; 

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
	void setupFrameRate(WORD fps);
	void setupIso(WORD iso);
	void setupAperture(WORD aperture);
	void setupExposure(WORD exposure);
	void setupExposureValue(EV exposureValue);
	void setupFocus(WORD focus);

	void zoom(char zoomIn, char continousZoom);
	void focus(char focusIn, char continousFocus);

	int grapPcMonitorFrame(char *jpgImage);
	void getCameraFrame(IplImage* frame); 
	void exitCamera(void);

	libexf1 lib; 

	};

#endif	/* LIBEXF1_H */

