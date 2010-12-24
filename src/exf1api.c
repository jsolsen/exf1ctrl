#include "exf1api.h"

int init_camera(void)
{
    usb_init();
    usb_find_busses();
    usb_find_devices();

    if(!(dev = open_dev())) {
      printf(" Error: camera not found!\n");
      return 0;
    }

    if(usb_set_configuration(dev, 1) < 0) {
      printf(" Error: setting config 1. \n");
      usb_close(dev);
      return 0;
    }

    if(usb_claim_interface(dev, 0) < 0) {
      printf(" Error: claiming interface 0 failed. \n");
      usb_close(dev);
      return 0;
    }

    if (usb_control_msg(dev, 0x21, 0x66, 0x00, 0x00, NULL, 0, TIME_OUT) < 0)
      printf("error: cmd write 1 failed\n");

    if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      //printf("error: interrupt read 1 failed\n");

    if (usb_clear_halt(dev, EP_IN) < 0)
      printf("error: halt clear failed.\n");

    if (usb_clear_halt(dev, EP_OUT) < 0)
      printf("error: halt clear failed.\n");

    if (usb_control_msg(dev, 0xA1, 0x67, 0x00, 0x00, &tmp[0], 0x0400, TIME_OUT) < 0)
      printf("error: cmd write 2 failed\n");

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

char halfShutterPressed = FALSE; 

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

void shutter(char filename[], char thumbnail[])
{

  int bytesRead = -1, transfer_jpg = 1;
  FILE * pFile;

  usbCmdGen(0x9024, NO_READS, 0, NULL);

  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      printf("error: interrupt read 7 failed\n");
  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      printf("error: interrupt read 8 failed\n");

  usbCmdGen(0x9027, TWO_READS, 0, NULL);
  usbCmdGen(0x900C, TWO_READS, 8, (char[]){0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF});

  // Get fullsize image.
  usbCmdGen(0x9025, ONE_READ, 4, (char[]){0x01, 0x00, 0x00, 0x00});

  pFile = fopen (filename, "wb");
  while (transfer_jpg) {

     bytesRead = usb_bulk_read(dev, EP_IN, img, IMG_BUF_SIZE, TIME_OUT);

     if (bytesRead > 0) {

         if (string_match((char[]){0x25, 0x90, 0x0C, 0x00, 0x00, 0x00}, img+6, 6) == 0) {
            printf("> Transferring %d bytes... \n", img[0] + (img[1] << 8) + (img[2] << 16) + (img[3] << 24));
            fwrite(img+12, 1, bytesRead-12, pFile);
         }
         else if (string_match((char[]){0x18, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x20, 0x0C, 0x00, 0x00, 0x00}, img, 12) == 0) {
            printf("> %s saved to disk. \n", filename);
            transfer_jpg = 0;
         }
         else
            fwrite(img, 1, bytesRead, pFile);
     }
  }
  fclose (pFile);

  // Get thumbnail image.
  usbCmdGen(0x9026, NO_READS, 4, (char[]){0x01, 0x00, 0x00, 0x00});

  transfer_jpg = 1;

  pFile = fopen (thumbnail, "wb");
  while (transfer_jpg) {

     bytesRead = usb_bulk_read(dev, EP_IN, img, BUF_SIZE, TIME_OUT);

     if (bytesRead > 0) {

         if (string_match((char[]){0x26, 0x90, 0x0D, 0x00, 0x00, 0x00}, img+6, 6) == 0) {
            printf("> Transferring %d bytes... \n", img[0] + (img[1] << 8) + (img[2] << 16) + (img[3] << 24));
            fwrite(img+12, 1, bytesRead-12, pFile);
         }
         else if (string_match((char[]){0x18, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x20, 0x0D, 0x00, 0x00, 0x00}, img, 12) == 0) {
            printf("> %s saved to disk. \n", thumbnail);
            transfer_jpg = 0;
         }
         else
            fwrite(img, 1, bytesRead, pFile);
     }
  }
  fclose (pFile);

  usbCmdGen(0x9028, ONE_READ, 0, NULL);

}

char stillImageEnabled = TRUE;
char preRecordEnabled  = FALSE;

void setup_movie_hs(char enablePreRecord)
{
    stop_config();
    exf1Cmd(CMD_WRITE, ADDR_MOVIE_MODE, DATA_MOVIE_MODE_HS);
    start_config(FALSE, enablePreRecord);

    // Verify that ADDR_HS_SETTING is set to something valid?
    //usbCmdGen(0x1015, TWO_READS, 4, (char[]){0x0D, 0xD0, 0x00, 0x00});

    stop_config();
    exf1Cmd(CMD_WRITE, ADDR_FOCUS, DATA_FOCUS_AF);
    start_config(FALSE, enablePreRecord);
}

void setup_movie_hd(char enablePreRecord)
{
    stop_config();
    exf1Cmd(CMD_WRITE, ADDR_MOVIE_MODE, DATA_MOVIE_MODE_HD);
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

void movie(char filename[], int delay)
{
    char bytes[2];
    int bytesRead = -1, movSize = 0;
    int bytesCopied = -1, bytesRemaining = -1;
    FILE * pFile;

    usbCmdGen(0x9043, ONE_READ, 0, NULL);

    if (delay >= 0)
      Sleep(1000 * delay);
    else
      printf("> Press enter to stop recording... "), getchar();

    Sleep(2);

    if (preRecordEnabled) {
        usbCmdGen(0x9044, NO_READS, 0, NULL);
        do {
            if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x81, bytes, 0x2, TIME_OUT) < 0)
                printf("error: cmd write 1 failed\n");

            if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x2, bytes, 0x2, TIME_OUT) < 0)
                printf("error: cmd write 2 failed\n");

        } while (usb_bulk_read(dev, EP_IN, tmp, BUF_SIZE, TIME_OUT) < 0);
    }
    else
        usbCmdGen(0x9044, ONE_READ, 0, NULL);

    usbCmdGen(0x9045, TWO_READS, 0, NULL);
    usbCmdGen(0x900C, TWO_READS, 8, (char[]){0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF});

    do {
        usbCmdGen(0x9025, NO_READS, 4, (char[]){0x01, 0x00, 0x00, 0x00});
        bytesRead = usb_bulk_read(dev, EP_IN, img, IMG_BUF_SIZE, TIME_OUT);
    } while (bytesRead < 0);
    bytesCopied = bytesRead;

    movSize = GET_DWORD(img);
    movSize -= 12;

    //printf("Movie file size: %d\n", movSize);

    pFile = fopen (filename, "wb");
    fwrite(img+12, 1, bytesRead-12, pFile);

    if (movSize > 0) {

        printf("> Transferring %d bytes... \n", movSize);
        do {
            
            bytesRemaining = movSize - bytesCopied + 12;

            do
               bytesRead = usb_bulk_read(dev, EP_IN, img, IMG_BUF_SIZE, TIME_OUT);
            while (bytesRead < 0);
            bytesCopied += bytesRead;
            bytesRemaining -= bytesRead;
            fwrite(img, 1, bytesRead, pFile);
            
            //printf("bytesCopied=%d bytesRemaining=%d\n", bytesCopied, bytesRemaining);

        } while (bytesRemaining > 0);
        usb_bulk_read(dev, EP_IN, img, IMG_BUF_SIZE, TIME_OUT);

    } else
       printf("Error: Negative MOV size!\n");

    fclose (pFile);
    printf("> %s saved to disk. \n", filename);

    if (preRecordEnabled) {
        usbCmdGen(0x9046, NO_READS, 0, NULL);
        do {
            if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x81, bytes, 0x2, TIME_OUT) < 0)
                printf("error: cmd write 1 failed\n");

            if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x2, bytes, 0x2, TIME_OUT) < 0)
                printf("error: cmd write 2 failed\n");

        } while (usb_bulk_read(dev, EP_IN, tmp, BUF_SIZE, TIME_OUT) < 0);
    }
    else
        usbCmdGen(0x9046, ONE_READ, 0, NULL);

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
