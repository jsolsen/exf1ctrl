#include "libexf1.h"

usb_dev_handle *dev = NULL;
char tmp[BUF_SIZE];
char img[IMG_BUF_SIZE];

usb_dev_handle *open_dev(void)
{
  struct usb_bus *bus;
  struct usb_device *dev;

  for(bus = usb_get_busses(); bus; bus = bus->next)
    {
      for(dev = bus->devices; dev; dev = dev->next)
        {
          if(dev->descriptor.idVendor == MY_VID
             && dev->descriptor.idProduct == MY_PID)
            {
              return usb_open(dev);
            }
        }
    }
  return NULL;
}

int string_match(char s1[], char s2[], int length)
{
  int i, matches = 0;

  for (i=0; s1[i] == s2[i] && i<length; i++)
      matches++;

  return matches-length;
}

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

    exf1Cmd(CMD_OPEN_SESSION, SESSION_ID);
    exf1Cmd(CMD_WRITE, ADDR_FUNCTIONALITY, DATA_FUNC_EXTENDED);
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

unsigned int USB_CMD_ID = 0xFFFFFFFF;

//void exf1Cmd(WORD cmd, DWORD addr, WORD data) {
void exf1Cmd(WORD cmd, ...)
{
    DWORD dwordVal; WORD wordVal;
    char bytes[2];

    va_list ap;
    va_start(ap, cmd);

    switch(cmd){
        case CMD_WRITE:
            dwordVal = va_arg(ap, int);  // Address
            wordVal  = va_arg(ap, int);  // Value.
            usbTx(cmd, TYPE_CMD,  sizeof(dwordVal),(DWORD) dwordVal);
            usbTx(cmd, TYPE_DATA, sizeof(wordVal), (DWORD) wordVal);
            usbRx();

            if (dwordVal == ADDR_FUNCTIONALITY) {
                if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
                  printf(" Error: interrupt read 1 after setting ADDR_FUNCTIONALITY failed\n");

                if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
                  printf(" Error: interrupt read 2 after setting ADDR_FUNCTIONALITY failed\n");
            }

            break;

        case CMD_READ:
            break;

        case CMD_STILL_START:
        case CMD_MOVIE_START:
            dwordVal = va_arg(ap, int);
            usbTx(cmd, TYPE_CMD, sizeof(dwordVal), (DWORD) dwordVal);
            if (dwordVal) { // PreRecordEnabled...
                if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x81, bytes, 0x2, TIME_OUT) < 0)
                    printf("error: cmd write 1 failed\n");

                if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x2, bytes, 0x2, TIME_OUT) < 0)
                    printf("error: cmd write 2 failed\n");

                Sleep(1000);
            }
            usbRx();
            break;

        case CMD_HALF_PRESS:
            usbTx(cmd, TYPE_CMD, 0, 0);
            if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
                printf(" Error: Interrupt read 1 failed! \n");
            if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
                printf(" Error: Interrupt read 2 failed! \n");
            usbRx();
            break;

        case CMD_CLOSE_SESSION:
        case CMD_HALF_RELEASE:
        case CMD_STILL_STOP:
        case CMD_MOVIE_STOP:
            usbTx(cmd, TYPE_CMD, 0, 0);
            usbRx();
            break;

        case CMD_OPEN_SESSION:
            USB_CMD_ID = 0;
        case CMD_TRANSFER:
            dwordVal = va_arg(ap, int);
            usbTx(cmd, TYPE_CMD, sizeof(dwordVal), (DWORD) dwordVal);
            usbRx();
            break;

        default:
            printf("Unsupported command type!\n");
    }
    
    va_end(ap);
    USB_CMD_ID++;
}

void usbTx(WORD cmd, WORD cmdType, int nCmdParameterBytes, DWORD cmdParameters) {

    PTP_CONTAINER tx;
    int packetSize = 12 + nCmdParameterBytes;

    tx.length   = packetSize;
    tx.type     = cmdType;
    tx.code     = cmd;
    tx.trans_id = USB_CMD_ID;

    if (nCmdParameterBytes == 2) {
        tx.payload.word_params.param1 = 0xFFFF & cmdParameters;
    }
    else if (nCmdParameterBytes == 4) {
        tx.payload.dword_params.param1 = cmdParameters;
    }
    else if (nCmdParameterBytes == 8) {

        printf("This is not supported yet!\n");
        tx.payload.dword_params.param1 = cmdParameters;
        tx.payload.dword_params.param2 = cmdParameters;
    }

    /*
    int i;
    char *pTx = (char *) &tx;
    for (i=0; i<packetSize; i++)
        printf("%02X-", 0xFF & *(pTx + i));
    printf("\n");
    */

    if (usb_bulk_write(dev, EP_OUT, (char *) &tx, packetSize, TIME_OUT) != packetSize)
        printf("Error: Bulk write failed for this command: %02X!\n", 0xFFFF & cmd);

}

void usbRx() {

    int bytesRead = usb_bulk_read(dev, EP_IN, tmp, BUF_SIZE, TIME_OUT);

    if (bytesRead > 0) {
        PTP_CONTAINER *rx = (PTP_CONTAINER *) tmp;

        switch (rx->type) {
            case TYPE_DATA:
                break;
            case TYPE_RESPONSE:
                if (rx->code != CMD_OK) printf("Ack = %02X.\n", rx->code);
                break;
            case TYPE_EVENT:
                printf("Read an event message.\n");
                usbRx();
                break;
        }
    }
    else {
        printf("Error: Bulk read failed! %d \n", bytesRead);
    }

}

//void usbCmdGen(short int cmdId, int cmdParameter, short int postCmdReads) {
void usbCmdGen(short int cmd, short int postCmdReads, int nCmdParameters, char cmdParameters[]) {

    int bytesRead = 0, packetSize = 12, i, cmdIndex = 1;
    char cmdBuffer[20], *pCmdBuffer, *pCmdParameters;

    if (nCmdParameters%4 == 0 || nCmdParameters > 4) {

        // Calculate package size;
        packetSize = 12 + 4*(int)(nCmdParameters / 4);
        //printf("PacketSize=%d\n", packetSize);

        // Assuming big endianess here!
        pCmdBuffer  = cmdBuffer;
        pCmdBuffer += 0; SET_DWORD(pCmdBuffer, packetSize);   // Number of bytes in USB packet.
        pCmdBuffer += 4; SET_WORD (pCmdBuffer, cmdIndex);     // Command index.
        pCmdBuffer += 2; SET_WORD (pCmdBuffer, cmd);          // Command.
        pCmdBuffer += 2; SET_DWORD(pCmdBuffer, USB_CMD_ID);   // Command number.
        pCmdBuffer += 4;

        pCmdParameters = cmdParameters;
        for (i=12; i<packetSize; i++)
            *(pCmdBuffer++) = *(pCmdParameters++);               // Command parameters.

        /*
        for (i=0; i<packetSize; i++)
            printf("%02X-", 0xFF & cmdBuffer[i]);
        printf("\n");
        */

        if (usb_bulk_write(dev, EP_OUT, cmdBuffer, packetSize, TIME_OUT) != packetSize)
            printf("Error: Bulk write failed for this command: %02X!\n", 0xFFFF & cmd);

        cmdIndex++;
    }

    if (nCmdParameters%4 == 2) {

        // Calculate package size;
        packetSize = 14;
        //printf("PacketSize=%d\n", packetSize);

        // Assuming big endianess here!
        pCmdBuffer  = cmdBuffer;
        pCmdBuffer += 0; SET_DWORD(pCmdBuffer, packetSize);   // Number of bytes in USB packet.
        pCmdBuffer += 4; SET_WORD (pCmdBuffer, cmdIndex);     // Command index.
        pCmdBuffer += 2; SET_WORD (pCmdBuffer, cmd);          // Command.
        pCmdBuffer += 2; SET_DWORD(pCmdBuffer, USB_CMD_ID);   // Command number.
        pCmdBuffer += 4;

        pCmdParameters = cmdParameters + nCmdParameters - 2;
        for (i=12; i<packetSize; i++)
            *(pCmdBuffer++) = *(pCmdParameters++);               // Command parameters.

        /*
        for (i=0; i<packetSize; i++)
            printf("%02X-", 0xFF & cmdBuffer[i]);
        printf("\n");
        */

        if (usb_bulk_write(dev, EP_OUT, cmdBuffer, packetSize, TIME_OUT) != packetSize)
            printf("Error: Bulk write failed for this command: %02X!\n", 0xFFFF & cmd);
    }

    for (i=0; i<postCmdReads; i++) {
        bytesRead = usb_bulk_read(dev, EP_IN, tmp, BUF_SIZE, TIME_OUT);
        if (bytesRead < 0)
            printf("Error: Bulk read failed for this command: %02X!\n", 0xFFFF & cmd);
    }

    USB_CMD_ID++;
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
