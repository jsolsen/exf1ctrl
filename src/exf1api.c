#include "exf1api.h"

char halfShutterPressed = FALSE;
char stillImageEnabled = TRUE;
char preRecordEnabled  = FALSE;
char continousShutterEnabled = FALSE; 

int initCamera(void)
{
    if (usbInit() == 0)
        exit(0);

    // Print out device info.
    //exf1Cmd(CMD_GET_DEVICE_INFO);

    exf1Cmd(CMD_OPEN_SESSION, SESSION_ID);
    exf1Cmd(CMD_WRITE, ADDR_FUNCTIONALITY, DATA_FUNC_EXTENDED);

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

void zoom(char zoomIn, char continousZoom) {

    if (continousZoom) {
        printf("> Press enter to stop zooming...");
        if (zoomIn) {
            exf1Cmd(CMD_CZ_PRESS, DATA_ZOOM_IN);
            getchar();
            exf1Cmd(CMD_CZ_RELEASE);
        }
        else {
            exf1Cmd(CMD_CZ_PRESS, DATA_ZOOM_OUT);
            getchar(); 
            exf1Cmd(CMD_CZ_RELEASE);
        }
    }
    else {
        if (zoomIn)
            exf1Cmd(CMD_ZOOM, DATA_ZOOM_IN);
        else
            exf1Cmd(CMD_ZOOM, DATA_ZOOM_OUT);
    }
}

void focus(char focusIn, char continousFocus) {

    if (continousFocus) {
        printf("> Press enter to stop focusing...");
        if (focusIn) {
            exf1Cmd(CMD_CF_PRESS, DATA_FOCUS_IN);
            getchar(); 
            exf1Cmd(CMD_CF_RELEASE);
        }
        else {
            exf1Cmd(CMD_CF_PRESS, DATA_FOCUS_OUT);
            getchar(); 
            exf1Cmd(CMD_CF_RELEASE);
        }
    }
    else {
        if (focusIn)
            exf1Cmd(CMD_FOCUS, DATA_FOCUS_IN);
        else
            exf1Cmd(CMD_FOCUS, DATA_FOCUS_OUT);
    }
}

void halfShutter(void)
{
    if (halfShutterPressed == FALSE) {
        exf1Cmd(CMD_HALF_PRESS);
        halfShutterPressed = TRUE; 
    }
    else {
        exf1Cmd(CMD_HALF_RELEASE);
        halfShutterPressed = FALSE; 
    }
}

void shutter(char *fileName, char *thumbNail, int delay)
{
    int i;
    //char newFileName[255], newThumbNail[255];

    if (continousShutterEnabled) {
        exf1Cmd(CMD_CS_PRESS);
        if (delay >= 0)
            Sleep(1000 * delay);
        else
            printf("> Press enter to stop recording... "), getchar();
        exf1Cmd(CMD_CS_RELEASE, preRecordEnabled);
    }
    else 
        exf1Cmd(CMD_SHUTTER);

    exf1Cmd(CMD_GET_STILL_HANDLES);

    if (continousShutterEnabled) {
        for (i=0; i<objectHandles->noItems; i++) {

            // Generate new file name.
            /*
            strncpy(newFileName, fileName, strlen(fileName)-4);
            sprintf(newFileName, "%s-%04d.jpg", newFileName);
            strncpy(newThumbNail, thumbNail, strlen(thumbNail)-4);
            sprintf(newThumbNail, "%s-%04d.jpg", newThumbNail);
*/
            printf("> Downloading %s and %s...\n", fileName, thumbNail);

            exf1Cmd(CMD_GET_OBJECT_INFO, objectHandles->data[i]);
            exf1Cmd(CMD_GET_OBJECT,    TO_FILE, objectHandles->data[i], fileName);
            exf1Cmd(CMD_GET_THUMBNAIL, TO_FILE, objectHandles->data[i], thumbNail);

        }
    }
    else {
        exf1Cmd(CMD_GET_OBJECT_INFO, objectHandles->data[0]);
        exf1Cmd(CMD_GET_OBJECT,    TO_FILE, objectHandles->data[0], fileName);
        exf1Cmd(CMD_GET_THUMBNAIL, TO_FILE, objectHandles->data[0], thumbNail);
    }
    exf1Cmd(CMD_STILL_RESET);
}

void setupShutter(SHUTTER_MODES shutterMode, char enablePreRecord)
{
    stopConfig();
    switch(shutterMode) {
        case SHUTTER_NORMAL:
            exf1Cmd(CMD_WRITE, ADDR_CAPTURE_MODE, DATA_CAPTURE_NORMAL);
            continousShutterEnabled = FALSE;
            break;
        case SHUTTER_CONTINOUS:
            exf1Cmd(CMD_WRITE, ADDR_CAPTURE_MODE, DATA_CAPTURE_CS);
            continousShutterEnabled = TRUE;
            break;
        case SHUTTER_PRERECORD:
            exf1Cmd(CMD_WRITE, ADDR_CAPTURE_MODE, DATA_CAPTURE_PREREC);
            continousShutterEnabled = TRUE;
            break;
    }
    startConfig(FALSE, enablePreRecord);

    // Verify that ADDR_HS_SETTING is set to something valid?
    //usbCmdGen(0x1015, TWO_READS, 4, (char[]){0x0D, 0xD0, 0x00, 0x00});

    stopConfig();
    exf1Cmd(CMD_WRITE, ADDR_FOCUS, DATA_FOCUS_AF);
    startConfig(FALSE, enablePreRecord);
}

void setupMovie(MOVIE_MODES movieMode, char enablePreRecord)
{
    stopConfig();
    switch (movieMode) {
        case MOVIE_STD:
            exf1Cmd(CMD_WRITE, ADDR_MOVIE_MODE, DATA_MOVIE_MODE_STD);
            break;    
        case MOVIE_HD:
            exf1Cmd(CMD_WRITE, ADDR_MOVIE_MODE, DATA_MOVIE_MODE_HD);
            break;
        case MOVIE_HS:
            exf1Cmd(CMD_WRITE, ADDR_MOVIE_MODE, DATA_MOVIE_MODE_HS);
            break;
    }
    startConfig(FALSE, enablePreRecord);

    // Verify that ADDR_HS_SETTING is set to something valid?
    //usbCmdGen(0x1015, TWO_READS, 4, (char[]){0x0D, 0xD0, 0x00, 0x00});

    stopConfig();
    exf1Cmd(CMD_WRITE, ADDR_FOCUS, DATA_FOCUS_AF);
    startConfig(FALSE, enablePreRecord);
}

void setupIso(WORD iso)
{
    stopConfig();
    exf1Cmd(CMD_WRITE, ADDR_ISO, iso);
    startConfig(stillImageEnabled, preRecordEnabled);
}

void setupAperture(WORD aperture)
{
    stopConfig();
    exf1Cmd(CMD_WRITE, ADDR_APERTURE, aperture);
    startConfig(stillImageEnabled, preRecordEnabled);
}

void setupExposure(WORD exposure)
{
    stopConfig();
    exf1Cmd(CMD_WRITE, ADDR_EXPOSURE, exposure);
    startConfig(stillImageEnabled, preRecordEnabled);
}

void setupFocus(WORD focus)
{
    stopConfig();
    exf1Cmd(CMD_WRITE, ADDR_FOCUS, focus);
    startConfig(stillImageEnabled, preRecordEnabled);
}

void stopConfig()
{
    if (stillImageEnabled)
        exf1Cmd(CMD_STILL_STOP);
    else
        exf1Cmd(CMD_MOVIE_STOP);
}

void startConfig(char enableStillImage, char enablePreRecord)
{
    if (enableStillImage)
        exf1Cmd(CMD_STILL_START);
    else
        exf1Cmd(CMD_MOVIE_START, enablePreRecord);

    stillImageEnabled = enableStillImage;
    preRecordEnabled  = enablePreRecord; 
}

void movie(char *fileName, int delay)
{
    exf1Cmd(CMD_MOVIE_PRESS);

    if (delay >= 0)
        Sleep(1000 * delay);
    else
        printf("> Press enter to stop recording... "), getchar();

    exf1Cmd(CMD_MOVIE_RELEASE, preRecordEnabled);
    exf1Cmd(CMD_GET_MOVIE_HANDLES);
    exf1Cmd(CMD_GET_OBJECT_INFO, objectHandles->data[0]);
    exf1Cmd(CMD_GET_OBJECT, TO_FILE, objectHandles->data[0], fileName);
    exf1Cmd(CMD_MOVIE_RESET, preRecordEnabled);
}

void setupMonitor(char isPc) {
    stopConfig();
    if (isPc) 
        exf1Cmd(CMD_WRITE, ADDR_MONITOR, DATA_MONITOR_PC);
    else
        exf1Cmd(CMD_WRITE, ADDR_MONITOR, DATA_MONITOR_LCD);
    startConfig(stillImageEnabled, preRecordEnabled);
    Sleep(2000);
}

void exitCamera(void)
{
    stopConfig();
    exf1Cmd(CMD_WRITE, ADDR_FUNCTIONALITY, DATA_FUNC_BASIC);
    exf1Cmd(CMD_CLOSE_SESSION);
    while (usbRxEvent()>0);
    printf("Exit done\n");
}

int grapPcMonitorFrame(char *jpgImage)
{
    int jpgSize = -1;
    //exf1Cmd(CMD_GET_STILL_HANDLES);
    //exf1Cmd(CMD_GET_OBJECT_INFO, 0x10000002);
    exf1Cmd(CMD_GET_OBJECT, TO_MEM, 0x10000002, jpgImage, &jpgSize);
    return jpgSize;
}

void terminateCamera(void)
{
  usb_release_interface(dev, 0);
  usb_reset(dev);
  usb_close(dev);
}
