#include "exf1api.h"

char halfShutterPressed = FALSE;
char stillImageEnabled = TRUE;
char preRecordEnabled  = FALSE;
char continousShutterEnabled = FALSE; 

int init_camera(void)
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

    start_config(TRUE, FALSE);

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

void half_shutter(void)
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

void setup_shutter(SHUTTER_MODES shutterMode, char enablePreRecord)
{
    stop_config();
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
    start_config(FALSE, enablePreRecord);

    // Verify that ADDR_HS_SETTING is set to something valid?
    //usbCmdGen(0x1015, TWO_READS, 4, (char[]){0x0D, 0xD0, 0x00, 0x00});

    stop_config();
    exf1Cmd(CMD_WRITE, ADDR_FOCUS, DATA_FOCUS_AF);
    start_config(FALSE, enablePreRecord);
}

void setup_movie(MOVIE_MODES movieMode, char enablePreRecord)
{
    stop_config();
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
    start_config(FALSE, enablePreRecord);

    // Verify that ADDR_HS_SETTING is set to something valid?
    //usbCmdGen(0x1015, TWO_READS, 4, (char[]){0x0D, 0xD0, 0x00, 0x00});

    stop_config();
    exf1Cmd(CMD_WRITE, ADDR_FOCUS, DATA_FOCUS_AF);
    start_config(FALSE, enablePreRecord);
}

void setup_iso(WORD iso)
{
    stop_config();
    exf1Cmd(CMD_WRITE, ADDR_ISO, iso);
    start_config(stillImageEnabled, preRecordEnabled);
}

void setup_aperture(WORD aperture)
{
    stop_config();
    exf1Cmd(CMD_WRITE, ADDR_APERTURE, aperture);
    start_config(stillImageEnabled, preRecordEnabled);
}

void setup_exposure(WORD exposure)
{
    stop_config();
    exf1Cmd(CMD_WRITE, ADDR_EXPOSURE, exposure);
    start_config(stillImageEnabled, preRecordEnabled);
}

void setup_focus(WORD focus)
{
    stop_config();
    exf1Cmd(CMD_WRITE, ADDR_FOCUS, focus);
    start_config(stillImageEnabled, preRecordEnabled);
}

void stop_config()
{
    if (stillImageEnabled)
        exf1Cmd(CMD_STILL_STOP);
    else
        exf1Cmd(CMD_MOVIE_STOP);
}

void start_config(char enableStillImage, char enablePreRecord)
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
    char bytes[2];
    
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

void setup_pc_monitor(void)
{
  stop_config();
  usbCmdGen(0x1016, ONE_READ, 6, (char[]){0x01, 0xD0, 0x00, 0x00, 0x02, 0x00});
  start_config(stillImageEnabled, preRecordEnabled);
}

void setup_lcd_monitor(void)
{
  stop_config();
  usbCmdGen(0x1016, ONE_READ, 6, (char[]){0x01, 0xD0, 0x00, 0x00, 0x01, 0x00});
  start_config(stillImageEnabled, preRecordEnabled);
}

void exit_camera(void)
{
    /*
    usb_bulk_read(dev, EP_IN, tmp, BUF_SIZE, TIME_OUT);

    if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
        printf("error: interrupt read 1 failed\n");

    if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
        printf("error: interrupt read 1 failed\n");

    usbCmdGen(0x9002, NO_READS, 0, NULL);

    if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
        printf("error: interrupt read 1 failed\n");

    if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
        printf("error: interrupt read 1 failed\n");

    usb_bulk_read(dev, EP_IN, tmp, BUF_SIZE, TIME_OUT);

    usbCmdGen(0x1016, ONE_READ, 6, (char[]){0x01, 0xD0, 0x00, 0x00, 0x01, 0x00});
    usbCmdGen(0x9001, ONE_READ, 4, (char[]){0x00, 0x00, 0x00, 0x00});
    */

    stop_config();
    exf1Cmd(CMD_WRITE, ADDR_FUNCTIONALITY, DATA_FUNC_BASIC);
    exf1Cmd(CMD_CLOSE_SESSION);

    printf("Exit done\n");
}

int grap_pc_monitor_frame(char *jpg_img)
{
   int bytesRead = -1, bytesCopied = 0, frameNo = 0, jpgSize = -1;

   // Not needed?
   do {
        bytesRead = usb_bulk_read(dev, EP_IN, img, IMG_BUF_SIZE, TIME_OUT);
        //printf("1. Bytes read=%d\n", bytesRead);
   } while (bytesRead < 0);

    do {
       frameNo = 0;
       do {
            bytesRead = usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT);
            //printf("2. Bytes read=%d\n", bytesRead);

            if (bytesRead == 8) {
               frameNo = GET_DWORD(tmp);
               //printf("Frame no.= %d\n", frameNo);
            }
        } while ((frameNo-1)%3 != 0);

       printf("Get frame: %d!\n", frameNo);

       usbCmdGen(0x9025, NO_READS, 4, (char[]){0x02, 0x00, 0x00, 0x10});
       bytesRead = usb_bulk_read(dev, EP_IN, img, 512, TIME_OUT);
       //printf("3. Bytes read=%d\n", bytesRead);

   } while (bytesRead < 0);

   memcpy(jpg_img, img+12, bytesRead-12);
   bytesCopied = bytesRead;

   jpgSize = GET_DWORD(img);
   jpgSize -= 12;

   //printf("JPG size = %d ", jpgSize);

   if (jpgSize > 0) {

       int bytesRemaining = jpgSize - bytesCopied + 12;

       do {
           bytesRead = usb_bulk_read(dev, EP_IN, jpg_img + bytesCopied - 12, 512 * ((int)(bytesRemaining/512)+1), TIME_OUT);
           //printf("4. Bytes read=%d\n", bytesRead);
       } while (bytesRead < 0);
       bytesCopied += bytesRead;
       bytesRemaining -= bytesRead;
       //printf("bytesCopied=%d bytesRemaining=%d\n", bytesCopied, bytesRemaining);

   } else
       printf("Error: Negative JPG size!\n");

   //printf("Done!\n");
   return jpgSize;
}

void terminate_camera(void)
{
  usb_release_interface(dev, 0);
  usb_close(dev);
}
