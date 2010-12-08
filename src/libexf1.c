#include "libexf1.h"

/*  Command observations:
 *  Byte 1-4   = Number of bytes in packet.
 *  Byte 5-6   = Command idx. for broken up commands: 0x0001, 0x0002 ...
 *  Byte 7-8   = Command.
 *  Byte 9-12  = Command number: 0x00000000 - 0xFFFFFFFF.
 *  Byte 13-14 = Parameter 1.
 *  Byte 15-16 = Parameter 2.
 *  Byte 17-18 = Parameter 3.
 *  Byte 19-20 = Parameter 4.
 */

/*  Ack observations:
 *  Byte 1-4   = Number of bytes in packet.
 *  Byte 5-6   = Ack idx. Ends with 0x0003.
 *  Byte 7-8   = Ack. 0x2001 = OK, 0x0002 = More data on the way. 
 *  Byte 9-12  = Command number: 0x00000000 - 0xFFFFFFFF.
 *  Byte 13-14 = Parameter 1.
 *  Byte 15-16 = Parameter 2.
 *  Byte 17-18 = Parameter 3.
 *  Byte 19-20 = Parameter 4.
 *  Byte 21-22 = Parameter 5.
 *  Byte 23-24 = Parameter 6.
 */

/*  Command 0x1016 is used to configure camera settings.
 *  Byte 1-4   = Setting address.
 *  Byte 5-6   = Setting.
 *
 *  Examples:
 *
 *  Manual exposure:
 *  Exposure   = 0x0000500E, 0x0001 = M, 0x0002 = Auto, etc.
 *  ShutterSpe = 0x0000500D, 0x0023 = 1/40, 0x002B = 1/250, etc.
 *  Aperture   = 0x00005007, 0x0001 = F2.7, 0x0002 = F3.0, etc. 
 *
 *  Still image:
 *  Image size = 0x00005003, Size is written in ASCII.
 *  Quality    = 0x00005004, 0x0001 = Economy, 0x0002 = Normal, etc.
 *  WB         = 0x00005005, 0x0002 = Auto WB, 0x0004 = Daylight, etc.
 *  Metering   = 0x0000500B, 0x0002 = Center weighted, 0x0003 = Set multi, etc. 
 *  Flash      = 0x0000500C, 0x0001 = Auto flash, 0x0002 = Flash off, etc. 
 *  ISO        = 0x0000500F, 0xFFFF = Auto, 0x0064 = 100, 0x00C8 = 200, etc.
 *  EV         = 0x00005010, 0x0000 = -2.0EV, 0x0001 = -1.7EV, etc.
 *  Storage    = 0x0000D002, 0x0000 = Save to SD, 0x0001 = Save to PC. 
 *  Rec light  = 0x0000D008, 0x0000 = Off, 0x0001 = On
 *  Movie mode = 0x0000D00B, 0x0001 = HD, 0x0002 = HS
 *
 *  Continues shutter:
 *  High-speed = 0x0000D00F, 0x0001 = 1fps, 0x000C = 30fps, etc
 *  UpperLimit = 0x0000D010, 0x0001 = 3fps, 0x0002 = 5fps, etc.
 *
 *  Prerecord Still Image:
 *  CS shot    = 0x0000D011, 0x001E = 30, 0x0028 = 40, etc. 
 *
 *  Movie:
 *  HD setting = 0x0000D00C, 0x0000 = FHD, 0x0001 = HD
 *  HS setting = 0x0000D00D, 0x0000 = 300fps, 0x0001 = 600fps, etc. 
 *
 *  Focus      = 0x0000500A, 0x0002 = AF, 0x0003 = Macro focus, etc. 
 *
 */

/*  Command 0x1014 / 0x1015 reads out a camera setting.
 *  Byte 1-4   = Setting address.
 *
 *  1. packet  = Setting?
 *  2. packet  = ACK?
 */


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
  usb_init(); /* initialize the library */
  usb_find_busses(); /* find all busses */
  usb_find_devices(); /* find all connected devices */

  if(!(dev = open_dev()))
    {
      printf(" Error: camera not found!\n");
      return 0;
    }

  if(usb_set_configuration(dev, 1) < 0)
    {
      printf(" Error: setting config 1. \n");
      usb_close(dev);
      return 0;
    }

  if(usb_claim_interface(dev, 0) < 0)
    {
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

  usbCmdGen(0x1001, TWO_READS, 0, NULL);
  usbCmdGen(0x1002, ONE_READ,  4, (char[]){0x15, 0x06, 0x76, 0x19});
  usbCmdGen(0x1014, TWO_READS, 4, (char[]){0x02, 0x50, 0x00, 0x00});
  usbCmdGen(0x1016, ONE_READ,  6, (char[]){0x02, 0x50, 0x00, 0x00, 0x01, 0x80}); // 8001 read from 0x1014.

  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      printf("error: interrupt read 2 failed\n");

  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      printf("error: interrupt read 3 failed\n");

  usbCmdGen(0x1001, TWO_READS, 0, NULL);

  start_config(TRUE, FALSE);

  usbCmdGen(0x1014, TWO_READS, 4, (char[]){0x0A, 0x50, 0x00, 0x00});
  usbCmdGen(0x1014, TWO_READS, 4, (char[]){0x0E, 0x50, 0x00, 0x00});
  usbCmdGen(0x1014, TWO_READS, 4, (char[]){0x0D, 0xD0, 0x00, 0x00});
  usbCmdGen(0x1014, TWO_READS, 4, (char[]){0x02, 0xD0, 0x00, 0x00});

  return 1;
}

void half_shutter(void)
{
  // half shutter
  usbCmdGen(0x9029, NO_READS, 0, NULL);

  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      printf("error: interrupt read 5 failed\n");
/*
  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      printf("error: interrupt read 6 failed\n");
*/
  usb_bulk_read(dev, EP_IN, tmp, BUF_SIZE, TIME_OUT);
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
    if (stillImageEnabled)
        exf1Cmd(CMD_STILL_START);
    else
        exf1Cmd(CMD_MOVIE_START, enablePreRecord);

    stillImageEnabled = enableStillImage;
    preRecordEnabled  = enablePreRecord; 
}

void movie(char filename[], int delay)
{
    char bytes[2];
    int bytesRead = -1, transfer_mov = 1, movSize = 0;
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
    DWORD addr; WORD data;
    char bytes[2];

    va_list ap;
    va_start(ap, cmd);

    switch(cmd){
        case CMD_WRITE:
            addr = va_arg(ap, int); data = va_arg(ap, int);
            usbTx(cmd, 1, sizeof(addr), (DDWORD) addr);
            usbTx(cmd, 2, sizeof(data), (DDWORD) data);
            usbRx(TRUE);
            break;

        case CMD_READ:
            break;

        case CMD_STILL_START:
        case CMD_MOVIE_START:
            addr = va_arg(ap, int);
            usbTx(cmd, 1, sizeof(addr), (DDWORD) addr);
            if (addr) { // PreRecordEnabled...
                if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x81, bytes, 0x2, TIME_OUT) < 0)
                    printf("error: cmd write 1 failed\n");

                if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x2, bytes, 0x2, TIME_OUT) < 0)
                    printf("error: cmd write 2 failed\n");

                Sleep(1000);
            }
            usbRx(TRUE);
            break;

        case CMD_STILL_STOP:
        case CMD_MOVIE_STOP:
            usbTx(cmd, 1, 0, 0);
            usbRx(TRUE);
            break;

        case CMD_TRANSFER:
            addr = va_arg(ap, int);
            usbTx(cmd, 1, sizeof(addr), (DDWORD) addr);
            usbRx(TRUE);
            break;
    }
    
    va_end(ap);
    USB_CMD_ID++;
}

void usbTx(WORD cmd, DWORD cmdIndex, int nCmdParameterBytes, DDWORD cmdParameters) {

    int packetSize = 12, i;
    char cmdBuffer[20], *pCmdBuffer, *pCmdParameters;

    // Calculate package size;
    packetSize = 12 + nCmdParameterBytes;
    //printf("PacketSize=%d\n", packetSize);

    // Assuming big endianess here!
    pCmdBuffer  = cmdBuffer;
    pCmdBuffer += 0; SET_DWORD(pCmdBuffer, packetSize);   // Number of bytes in USB packet.
    pCmdBuffer += 4; SET_WORD (pCmdBuffer, cmdIndex);     // Command index.
    pCmdBuffer += 2; SET_WORD (pCmdBuffer, cmd);          // Command.
    pCmdBuffer += 2; SET_DWORD(pCmdBuffer, USB_CMD_ID);   // Command number.

    pCmdParameters = ((char*) cmdParameters);

    if (nCmdParameterBytes == 2)
        pCmdBuffer += 4, SET_WORD (pCmdBuffer, cmdParameters);
    else if (nCmdParameterBytes == 4)
        pCmdBuffer += 4, SET_DWORD(pCmdBuffer, cmdParameters);
    else if (nCmdParameterBytes == 8) {
        pCmdBuffer += 4, SET_DWORD(pCmdBuffer, cmdParameters);
        pCmdBuffer += 4, SET_DWORD(pCmdBuffer, cmdParameters+4);
    }
    
    if (usb_bulk_write(dev, EP_OUT, cmdBuffer, packetSize, TIME_OUT) != packetSize)
        printf("Error: Bulk write failed for this command: %02X!\n", 0xFFFF & cmd);
    
}

void usbRx(char isAck) {

    int i, bytesRead = usb_bulk_read(dev, EP_IN, tmp, BUF_SIZE, TIME_OUT);
    
    if (bytesRead < 0 || (isAck && GET_WORD(tmp+6) != CMD_OK)) {
        printf("Error: Bulk read failed! ");

        if (isAck) printf("Ack = %02X.\n", 0xFFFF & GET_WORD(tmp+6));

        for (i=0; i<bytesRead; i++)
            printf("%02X-", 0xFF & tmp[i]);
        
        printf("\n");
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
    exf1Cmd(CMD_WRITE, ADDR_POWER, DATA_POWER_OFF);

    if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
        printf("error: interrupt read 1 failed\n");

    if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
        printf("error: interrupt read 1 failed\n");

    usbCmdGen(0x1003, ONE_READ, 0, NULL);

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
