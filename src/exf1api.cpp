#include "exf1api.h"

exf1api::exf1api()
{
	halfShutterPressed = FALSE;
	stillImageEnabled = TRUE;
	preRecordEnabled  = FALSE;
	continousShutterEnabled = FALSE; 
}

int exf1api::initCamera(void)
{
    if (lib.usbStart() == 0)
        exit(0);

    // Print out device info.
    //exf1Cmd(CMD_GET_DEVICE_INFO);

    lib.exf1Cmd(CMD_OPEN_SESSION, SESSION_ID);
    lib.exf1Cmd(CMD_WRITE, ADDR_FUNCTIONALITY, DATA_FUNC_EXTENDED);

    // Print out device info.
    //exf1Cmd(CMD_GET_DEVICE_INFO);

    // Print out info about all the supported properties. 
    /*
    int i;
    for (i=0; i<deviceInfo.devicePropertiesSupported->noItems; i++) {
        printf("--\n");
        exf1Cmd(CMD_GET_PROP_DESC, deviceInfo.devicePropertiesSupported->data[i]);
    }
    usbRx(); 
    */

    startConfig(TRUE, FALSE);
    
    return 1;
}

void exf1api::zoom(char zoomIn, char continousZoom) {

    if (continousZoom) {
        printf("> Press enter to stop zooming...");
        if (zoomIn) {
            lib.exf1Cmd(CMD_CZ_PRESS, DATA_ZOOM_IN);
            getchar();
            lib.exf1Cmd(CMD_CZ_RELEASE);
        }
        else {
            lib.exf1Cmd(CMD_CZ_PRESS, DATA_ZOOM_OUT);
            getchar(); 
            lib.exf1Cmd(CMD_CZ_RELEASE);
        }
    }
    else {
        if (zoomIn)
            lib.exf1Cmd(CMD_ZOOM, DATA_ZOOM_IN);
        else
            lib.exf1Cmd(CMD_ZOOM, DATA_ZOOM_OUT);
    }
}

void exf1api::focus(char focusIn, char continousFocus) {

    if (continousFocus) {
        printf("> Press enter to stop focusing...");
        if (focusIn) {
            lib.exf1Cmd(CMD_CF_PRESS, DATA_FOCUS_IN);
            getchar(); 
            lib.exf1Cmd(CMD_CF_RELEASE);
        }
        else {
            lib.exf1Cmd(CMD_CF_PRESS, DATA_FOCUS_OUT);
            getchar(); 
            lib.exf1Cmd(CMD_CF_RELEASE);
        }
    }
    else {
        if (focusIn)
            lib.exf1Cmd(CMD_FOCUS, DATA_FOCUS_IN);
        else
            lib.exf1Cmd(CMD_FOCUS, DATA_FOCUS_OUT);
    }
}

void exf1api::halfShutter(void)
{
    if (halfShutterPressed == FALSE) {
        lib.exf1Cmd(CMD_HALF_PRESS);
        halfShutterPressed = TRUE; 
    }
    else {
        lib.exf1Cmd(CMD_HALF_RELEASE);
        halfShutterPressed = FALSE; 
    }
}

void exf1api::shutter(const char *fileName, const char *thumbNail, int delay)
{
    DWORD i;
    //char newFileName[255], newThumbNail[255];

    if (continousShutterEnabled) {
        lib.exf1Cmd(CMD_CS_PRESS);
        if (delay >= 0)
#ifdef WIN32
            Sleep(1000 * delay);
#else
			usleep(1000 * delay);
#endif
        else
            printf("> Press enter to stop recording... "), getchar();
        lib.exf1Cmd(CMD_CS_RELEASE, preRecordEnabled);
    }
    else 
        lib.exf1Cmd(CMD_SHUTTER);

    lib.exf1Cmd(CMD_GET_STILL_HANDLES);

    if (continousShutterEnabled) {
        for (i=0; i<lib.objectHandles->noItems; i++) {

            // Generate new file name.
            /*
            strncpy(newFileName, fileName, strlen(fileName)-4);
            sprintf(newFileName, "%s-%04d.jpg", newFileName);
            strncpy(newThumbNail, thumbNail, strlen(thumbNail)-4);
            sprintf(newThumbNail, "%s-%04d.jpg", newThumbNail);
*/
            printf("> Downloading %s and %s...\n", fileName, thumbNail);

            lib.exf1Cmd(CMD_GET_OBJECT_INFO, lib.objectHandles->data[i]);
			if (strlen(fileName) > 0)
				lib.exf1Cmd(CMD_GET_OBJECT,    TO_FILE, lib.objectHandles->data[i], fileName);
            if (strlen(thumbNail) > 0)
				lib.exf1Cmd(CMD_GET_THUMBNAIL, TO_FILE, lib.objectHandles->data[i], thumbNail);

        }
    }
    else {
        lib.exf1Cmd(CMD_GET_OBJECT_INFO, lib.objectHandles->data[0]);
        if (strlen(fileName) > 0)
			lib.exf1Cmd(CMD_GET_OBJECT,    TO_FILE, lib.objectHandles->data[0], fileName);
		if (strlen(thumbNail) > 0)
			lib.exf1Cmd(CMD_GET_THUMBNAIL, TO_FILE, lib.objectHandles->data[0], thumbNail);
    }
    lib.exf1Cmd(CMD_STILL_RESET);
}

void exf1api::setupShutter(SHUTTER_MODES shutterMode, char enablePreRecord)
{
    stopConfig();
    switch(shutterMode) {
        case SHUTTER_NORMAL:
            lib.exf1Cmd(CMD_WRITE, ADDR_CAPTURE_MODE, DATA_CAPTURE_NORMAL);
            continousShutterEnabled = FALSE;
            break;
        case SHUTTER_CONTINOUS:
            lib.exf1Cmd(CMD_WRITE, ADDR_CAPTURE_MODE, DATA_CAPTURE_CS);
            continousShutterEnabled = TRUE;
            break;
        case SHUTTER_PRERECORD:
            lib.exf1Cmd(CMD_WRITE, ADDR_CAPTURE_MODE, DATA_CAPTURE_PREREC);
            continousShutterEnabled = TRUE;
            break;
    }
    startConfig(FALSE, enablePreRecord);

    // Verify that ADDR_HS_SETTING is set to something valid?
    //usbCmdGen(0x1015, TWO_READS, 4, (char[]){0x0D, 0xD0, 0x00, 0x00});

    stopConfig();
    lib.exf1Cmd(CMD_WRITE, ADDR_FOCUS, DATA_FOCUS_AF);
    startConfig(FALSE, enablePreRecord);
}

void exf1api::setupMovie(MOVIE_MODES movieMode, char enablePreRecord)
{
    stopConfig();
    switch (movieMode) {
        case MOVIE_STD:
            lib.exf1Cmd(CMD_WRITE, ADDR_MOVIE_MODE, DATA_MOVIE_MODE_STD);
            break;    
        case MOVIE_HD:
            lib.exf1Cmd(CMD_WRITE, ADDR_MOVIE_MODE, DATA_MOVIE_MODE_HD);
            break;
        case MOVIE_HS:
            lib.exf1Cmd(CMD_WRITE, ADDR_MOVIE_MODE, DATA_MOVIE_MODE_HS);
            break;
    }
    startConfig(FALSE, enablePreRecord);

    // Verify that ADDR_HS_SETTING is set to something valid?
    //usbCmdGen(0x1015, TWO_READS, 4, (char[]){0x0D, 0xD0, 0x00, 0x00});

    stopConfig();
    lib.exf1Cmd(CMD_WRITE, ADDR_FOCUS, DATA_FOCUS_AF);
    startConfig(FALSE, enablePreRecord);
}

void exf1api::setupFrameRate(WORD fps)
{
    stopConfig();
    lib.exf1Cmd(CMD_WRITE, ADDR_HS_SETTING, fps);
    startConfig(stillImageEnabled, preRecordEnabled);
}

void exf1api::setupIso(WORD iso)
{
    stopConfig();
    lib.exf1Cmd(CMD_WRITE, ADDR_ISO, iso);
    startConfig(stillImageEnabled, preRecordEnabled);
}

void exf1api::setupAperture(WORD aperture)
{
    stopConfig();
    lib.exf1Cmd(CMD_WRITE, ADDR_APERTURE, aperture);
    startConfig(stillImageEnabled, preRecordEnabled);
}

void exf1api::setupExposure(WORD exposure)
{
    stopConfig();
    lib.exf1Cmd(CMD_WRITE, ADDR_EXPOSURE, exposure);
    startConfig(stillImageEnabled, preRecordEnabled);
}

void exf1api::setupExposureValue(EV exposureValue)
{
    stopConfig();
    lib.exf1Cmd(CMD_WRITE, ADDR_EV, (WORD) exposureValue);
    startConfig(stillImageEnabled, preRecordEnabled);
}


void exf1api::setupFocus(WORD focus)
{
    stopConfig();
    lib.exf1Cmd(CMD_WRITE, ADDR_FOCUS, focus);
    startConfig(stillImageEnabled, preRecordEnabled);
}

void exf1api::stopConfig()
{
    if (stillImageEnabled)
        lib.exf1Cmd(CMD_STILL_STOP);
    else
        lib.exf1Cmd(CMD_MOVIE_STOP);
}

void exf1api::startConfig(char enableStillImage, char enablePreRecord)
{
    if (enableStillImage)
        lib.exf1Cmd(CMD_STILL_START, 0);
    else
        lib.exf1Cmd(CMD_MOVIE_START, enablePreRecord);

    stillImageEnabled = enableStillImage;
    preRecordEnabled  = enablePreRecord; 
}

void exf1api::movie(const char *fileName, int delay)
{
    char c;

    lib.exf1Cmd(CMD_MOVIE_PRESS);

    if (delay >= 0)
#ifdef WIN32
		Sleep(1000 * delay);
#else
		usleep(1000 * delay);
#endif
    else
	{
        printf("> Press 's' enter to stop recording... \n");
		printf("> Press 't' enter to toggle between 30/300FPS when in HS mode... \n");
		printf("> "); 
		do {
			c = getchar();	
			if (c == 't') {
				lib.exf1Cmd(CMD_30_300_PRESS);
				printf("> ");
			}
		} while (c != 's');  
		rewind(stdin); 
	}
    lib.exf1Cmd(CMD_MOVIE_RELEASE, preRecordEnabled);
    lib.exf1Cmd(CMD_GET_MOVIE_HANDLES);
    lib.exf1Cmd(CMD_GET_OBJECT_INFO, lib.objectHandles->data[0]);
    lib.exf1Cmd(CMD_GET_OBJECT, TO_FILE, lib.objectHandles->data[0], fileName);
    lib.exf1Cmd(CMD_MOVIE_RESET, preRecordEnabled);
}

void exf1api::setupMonitor(char isPc) {
    stopConfig();
    if (isPc) 
        lib.exf1Cmd(CMD_WRITE, ADDR_MONITOR, DATA_MONITOR_PC);
    else
        lib.exf1Cmd(CMD_WRITE, ADDR_MONITOR, DATA_MONITOR_LCD);
	startConfig(stillImageEnabled, preRecordEnabled);
	//Sleep(2000); 
}

void exf1api::exitCamera(void)
{
    stopConfig();
    lib.exf1Cmd(CMD_WRITE, ADDR_FUNCTIONALITY, DATA_FUNC_BASIC);
    lib.exf1Cmd(CMD_CLOSE_SESSION);
    while (lib.usbRxEvent()>0);
    printf("Exit done\n");
}

int exf1api::grapPcMonitorFrame(const char *jpgImage)
{
    int jpgSize = -1;
	if (lib.frameNo > 0) 
		lib.exf1Cmd(CMD_GET_OBJECT, TO_MEM, 0x10000002, jpgImage, &jpgSize);
	else
		lib.usbRxEvent(); 
    return jpgSize;
}

static const char * const cdjpeg_message_table[] = {
  NULL
};

void exf1api::getCameraFrame(IplImage* frame)
{
    char jpgImage[3*IMG_BUF_SIZE];
    int jpgSize;
	int offset = 0;
    JSAMPROW rowptr[1];
    JDIMENSION num_scanlines;

    jpgSize = grapPcMonitorFrame(jpgImage);
	if (jpgSize > 3*IMG_BUF_SIZE) 
		printf("jpgSize: %d maxSize is: %d \n", jpgSize, 3*IMG_BUF_SIZE);

    if (jpgSize > 0) {

        //cvInitImageHeader(frame, cvSize(640,480), IPL_DEPTH_8U, 3, IPL_ORIGIN_TL, 4);
        //cvCreateData(frame);

        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);

        jerr.addon_message_table = cdjpeg_message_table;
        //jerr.first_addon_message = JMSG_FIRSTADDONCODE;
        //jerr.last_addon_message = JMSG_LASTADDONCODE;

        // Now safe to enable signal catcher.
        #ifdef NEED_SIGNAL_CATCHER
          //enable_signal_catcher((j_common_ptr) &cinfo);
        #endif

        jpeg_mem_src(&cinfo, (unsigned char*) jpgImage, (unsigned long) jpgSize);
        //(void) jpeg_read_header(&cinfo, TRUE);

        if (jpeg_read_header(&cinfo, TRUE) == JPEG_HEADER_OK) {
            jpeg_start_decompress(&cinfo);

            while (cinfo.output_scanline < cinfo.output_height) {
                rowptr[0] = (JSAMPROW)&(frame->imageData[offset * 640 * 3]);
                num_scanlines = jpeg_read_scanlines(&cinfo, rowptr,1);

                offset+=1;
            }
            jpeg_finish_decompress(&cinfo);
        }
        jpeg_destroy_decompress(&cinfo);
    }
    else
        printf("JPG size is negative!\n"); 
}

void exf1api::terminateCamera(void)
{
	lib.usbStop();
}
