#include "libexf1.h"

usb_dev_handle *dev = NULL;
char tmp[BUF_SIZE];
char img[IMG_BUF_SIZE];
PTP_DEVICE_INFO deviceInfo;
PTP_DEVICE_PROPERTY deviceProperty;

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

        case CMD_GET_DEVICE_INFO:
            usbTx(cmd, TYPE_CMD, 0, 0);
            usbRx();
            usbRx();
            break;

        case CMD_GET_PROP_DESC:
            dwordVal = va_arg(ap, int);
            usbTx(cmd, TYPE_CMD, sizeof(dwordVal), (DWORD) dwordVal);
            usbRx();
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
                        int i;
                        char *pTx;
    if (bytesRead > 0) {
        PTP_CONTAINER *rx = (PTP_CONTAINER *) tmp;

        switch (rx->type) {
            case TYPE_DATA:
                switch (rx->code) {
                    case CMD_GET_DEVICE_INFO:
                        setDeviceInfo(rx->payload.data);
                        printDeviceInfo();
                        break;
                    
                    case CMD_GET_PROP_DESC:
                        setDeviceProperty(rx->payload.data);
                        printDeviceProperty();
                        break;

                    default:
                        printf("Unhandled data package: 0x%04X\n", rx->type);
                        pTx = rx->payload.data;
                        for (i=0; i<rx->length-12; i++)
                            printf("%02X-", 0xFF & *(pTx + i));
                        printf("\n");
                }
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

WORD getStringDataSet(STRING_DATA_SET **dst, char *src) {

    WORD byteSize;
    STRING_DATA_SET *sds = (STRING_DATA_SET *) src;

    byteSize = (sds->noItems) * sizeof(WORD) + 1;
    *dst     = malloc(byteSize);
    memcpy(*dst, sds, byteSize);

    return byteSize;
}

DWORD getWordDataSet(WORD_DATA_SET **dst, char *src) {

    DWORD byteSize;
    WORD_DATA_SET *wds = (WORD_DATA_SET *) src;

    byteSize = (wds->noItems + 2) * sizeof(WORD);
    *dst     = malloc(byteSize);
    memcpy(*dst, wds, byteSize);

    return byteSize;
}

void setDeviceInfo(char *pData) {

    // Free up old data sets. 
    free(deviceInfo.vendorExtensionDesc);
    free(deviceInfo.operationsSupported);
    free(deviceInfo.eventsSupported);
    free(deviceInfo.devicePropertiesSupported);
    free(deviceInfo.captureFormats);
    free(deviceInfo.imageFormats);
    free(deviceInfo.manufacturer);
    free(deviceInfo.model);
    free(deviceInfo.deviceVersion);
    free(deviceInfo.serialNumber);

    // Retrieve new device info from packet.
    deviceInfo.standardVersion          = GET_WORD (pData);  pData += sizeof(WORD);
    deviceInfo.vendorExtensionID        = GET_DWORD(pData);  pData += sizeof(DWORD);
    deviceInfo.vendorExtensionVersion   = GET_WORD (pData);  pData += sizeof(WORD);
    pData += getStringDataSet(&deviceInfo.vendorExtensionDesc, pData);
    deviceInfo.functionalMode           = GET_WORD (pData);  pData += sizeof(WORD);
    pData += getWordDataSet(&deviceInfo.operationsSupported, pData);
    pData += getWordDataSet(&deviceInfo.eventsSupported, pData);
    pData += getWordDataSet(&deviceInfo.devicePropertiesSupported, pData);
    pData += getWordDataSet(&deviceInfo.captureFormats, pData);
    pData += getWordDataSet(&deviceInfo.imageFormats, pData);
    pData += getStringDataSet(&deviceInfo.manufacturer, pData);
    pData += getStringDataSet(&deviceInfo.model, pData);
    pData += getStringDataSet(&deviceInfo.deviceVersion, pData);
    pData += getStringDataSet(&deviceInfo.serialNumber, pData);

}

void printStringDataSet(char *pDescrition, STRING_DATA_SET *pDataSet) {
    
    int i;
    printf("%s", pDescrition);
    for (i=0; i<pDataSet->noItems; i++)
        printf("%c", pDataSet->data[i]);
    printf("\n");
    
}

void printWordDataSet(char *pDescrition, WORD_DATA_SET *pDataSet) {

    int i;
    printf("%s", pDescrition);
    for (i=0; i<pDataSet->noItems; i++)
        printf("0x%02X, ", pDataSet->data[i]);
    printf("\n");

}

void printDeviceInfo() {

    printStringDataSet("Manufacturer  : ", deviceInfo.manufacturer);
    printStringDataSet("Model         : ", deviceInfo.model);
    printStringDataSet("DeviceVersion : ", deviceInfo.deviceVersion);
    printStringDataSet("SerialNumber  : ", deviceInfo.serialNumber);
                printf("ExtensionID   : 0x%04X\n", deviceInfo.vendorExtensionID);
    printStringDataSet("Description   : ", deviceInfo.vendorExtensionDesc);
                printf("Version       : 0x%04X\n", deviceInfo.vendorExtensionVersion);
      printWordDataSet("Properties    : ", deviceInfo.devicePropertiesSupported);
      printWordDataSet("CaptureFormats: ", deviceInfo.captureFormats);
      printWordDataSet("ImageFormats  : ", deviceInfo.imageFormats);

}

void printEnumDataSet(char *pDescrition, ENUM_FORM *pDataSet, WORD dataType) {

    int i;
    printf("%s", pDescrition);
    switch (dataType) {
        case DATA_TYPE_CHAR:
            for (i=0; i<pDataSet->numberOfValues; i++) printf("0x%02X, ", *(char *)pDataSet->supportedValue[i]);
            break;
        case DATA_TYPE_WORD:
            for (i=0; i<pDataSet->numberOfValues; i++) printf("0x%04X, ", *(WORD *)pDataSet->supportedValue[i]);
            break;
        case DATA_TYPE_DWORD:
            for (i=0; i<pDataSet->numberOfValues; i++) printf("0x%08X, ", *(DWORD *)pDataSet->supportedValue[i]);
            break;
        case DATA_TYPE_STRING:
            for (i=0; i<pDataSet->numberOfValues; i++) {
                printStringDataSet("", (STRING_DATA_SET *)pDataSet->supportedValue[i]);
            }
            break; 
    }
    printf("\n");

}

void setDeviceProperty(char *pData) {

    int i; 

    // Free up old data sets.
    free(deviceProperty.defaultValue);
    free(deviceProperty.currentValue);

    switch (deviceProperty.formFlag) {
        case PROPERTY_FORM_RANGE:
            free(deviceProperty.form.rangeForm.maximumValue);
            free(deviceProperty.form.rangeForm.minimumValue);
            free(deviceProperty.form.rangeForm.stepSize);
            break;
            
        case PROPERTY_FORM_ENUM:
            for (i=0; i<deviceProperty.form.enumForm.numberOfValues; i++)
                free(deviceProperty.form.enumForm.supportedValue[i]);
            free(deviceProperty.form.enumForm.supportedValue);
            break;
             
    }

    // Retrieve new device info from packet.
    deviceProperty.code     = GET_WORD (pData);  pData += sizeof(WORD);
    deviceProperty.dataType = GET_WORD (pData);  pData += sizeof(WORD);
    deviceProperty.getSet   = 0xFF & GET_WORD (pData);  pData += 1;

    switch (deviceProperty.dataType) {
        case DATA_TYPE_CHAR:
            deviceProperty.defaultValue = malloc(sizeof(char));
            deviceProperty.currentValue = malloc(sizeof(char)); 
            *(char*)deviceProperty.defaultValue = *pData; pData += sizeof(char);
            *(char*)deviceProperty.currentValue = *pData; pData += sizeof(char);
            break;

        case DATA_TYPE_WORD:
            deviceProperty.defaultValue = malloc(sizeof(WORD));
            deviceProperty.currentValue = malloc(sizeof(WORD));
            *(WORD *)deviceProperty.defaultValue = *pData; pData += sizeof(WORD);
            *(WORD *)deviceProperty.currentValue = *pData; pData += sizeof(WORD);
            break;

        case DATA_TYPE_DWORD:
            deviceProperty.defaultValue = malloc(sizeof(DWORD));
            deviceProperty.currentValue = malloc(sizeof(DWORD));
            *(DWORD *)deviceProperty.defaultValue = *pData; pData += sizeof(DWORD);
            *(DWORD *)deviceProperty.currentValue = *pData; pData += sizeof(DWORD);
            break;

        case DATA_TYPE_STRING:
            pData += getStringDataSet((STRING_DATA_SET **)&deviceProperty.defaultValue, pData);
            pData += getStringDataSet((STRING_DATA_SET **)&deviceProperty.currentValue, pData);
            break;

        default:
            printf("Unknown data type: 0x%04X!", deviceProperty.dataType);

    }

    deviceProperty.formFlag = 0xFF & GET_WORD (pData);  pData += 1;

    switch (deviceProperty.formFlag) {
        case PROPERTY_FORM_NONE:
            break;
        case PROPERTY_FORM_RANGE:
            switch (deviceProperty.dataType) {
                case DATA_TYPE_CHAR:
                    deviceProperty.form.rangeForm.minimumValue = malloc(sizeof(char));
                    deviceProperty.form.rangeForm.maximumValue = malloc(sizeof(char));
                    deviceProperty.form.rangeForm.stepSize = malloc(sizeof(char));
                    *(char*)deviceProperty.form.rangeForm.minimumValue = *pData; pData += sizeof(char);
                    *(char*)deviceProperty.form.rangeForm.maximumValue = *pData; pData += sizeof(char);
                    *(char*)deviceProperty.form.rangeForm.stepSize = *pData; pData += sizeof(char);
                    break;

                case DATA_TYPE_WORD:
                    deviceProperty.form.rangeForm.minimumValue = malloc(sizeof(WORD));
                    deviceProperty.form.rangeForm.maximumValue = malloc(sizeof(WORD));
                    deviceProperty.form.rangeForm.stepSize = malloc(sizeof(WORD));
                    *(WORD*)deviceProperty.form.rangeForm.minimumValue = *pData; pData += sizeof(WORD);
                    *(WORD*)deviceProperty.form.rangeForm.maximumValue = *pData; pData += sizeof(WORD);
                    *(WORD*)deviceProperty.form.rangeForm.stepSize = *pData; pData += sizeof(WORD);
                    break;

                case DATA_TYPE_DWORD:
                    deviceProperty.form.rangeForm.minimumValue = malloc(sizeof(DWORD));
                    deviceProperty.form.rangeForm.maximumValue = malloc(sizeof(DWORD));
                    deviceProperty.form.rangeForm.stepSize = malloc(sizeof(DWORD));
                    *(DWORD*)deviceProperty.form.rangeForm.minimumValue = *pData; pData += sizeof(DWORD);
                    *(DWORD*)deviceProperty.form.rangeForm.maximumValue = *pData; pData += sizeof(DWORD);
                    *(DWORD*)deviceProperty.form.rangeForm.stepSize = *pData; pData += sizeof(DWORD);
                    break;

                case DATA_TYPE_STRING:
                    pData += getStringDataSet((STRING_DATA_SET **)&deviceProperty.form.rangeForm.minimumValue, pData);
                    pData += getStringDataSet((STRING_DATA_SET **)&deviceProperty.form.rangeForm.maximumValue, pData);
                    pData += getStringDataSet((STRING_DATA_SET **)&deviceProperty.form.rangeForm.stepSize, pData);
                    break;

                default:
                    printf("Unknown data type: 0x%04X!", deviceProperty.dataType);

            }
            break;
        case PROPERTY_FORM_ENUM:

            deviceProperty.form.enumForm.numberOfValues = GET_WORD(pData); pData += sizeof(WORD);
            switch (deviceProperty.dataType) {
                case DATA_TYPE_CHAR:
                    deviceProperty.form.enumForm.supportedValue = malloc(deviceProperty.form.enumForm.numberOfValues * sizeof(char*));
                    for (i=0; i<deviceProperty.form.enumForm.numberOfValues ; i++) {
                        deviceProperty.form.enumForm.supportedValue[i]  = malloc(sizeof(char));
                        *(char *)deviceProperty.form.enumForm.supportedValue[i] = *pData; pData += sizeof(char);
                    }
                    break;

                case DATA_TYPE_WORD:
                    deviceProperty.form.enumForm.supportedValue = malloc(deviceProperty.form.enumForm.numberOfValues * sizeof(WORD*));
                    for (i=0; i<deviceProperty.form.enumForm.numberOfValues ; i++) {
                        deviceProperty.form.enumForm.supportedValue[i]  = malloc(sizeof(WORD));
                        *(WORD *)deviceProperty.form.enumForm.supportedValue[i] = GET_WORD(pData); pData += sizeof(WORD);
                    }
                    break;

                case DATA_TYPE_DWORD:
                    deviceProperty.form.enumForm.supportedValue = malloc(deviceProperty.form.enumForm.numberOfValues * sizeof(DWORD*));
                    for (i=0; i<deviceProperty.form.enumForm.numberOfValues ; i++) {
                        deviceProperty.form.enumForm.supportedValue[i]  = malloc(sizeof(DWORD));
                        *(DWORD *)deviceProperty.form.enumForm.supportedValue[i] = GET_DWORD(pData); pData += sizeof(DWORD);
                    }
                    break;

                case DATA_TYPE_STRING:
                    deviceProperty.form.enumForm.supportedValue = malloc(deviceProperty.form.enumForm.numberOfValues * sizeof(char*));
                    for (i=0; i<deviceProperty.form.enumForm.numberOfValues ; i++) {
                        deviceProperty.form.enumForm.supportedValue[i]  = malloc(255*sizeof(char));
                        pData += getStringDataSet((STRING_DATA_SET **)&deviceProperty.form.enumForm.supportedValue[i], pData);
                    }



                    
                    break;

                default:
                    printf("Unknown data type: 0x%04X!", deviceProperty.dataType);

            }
            break;
        default:
            printf("Unknown form flag: 0x%01X!", deviceProperty.formFlag);
    }

}

void printDeviceProperty() {

    printf("Code     : 0x%02X\n", deviceProperty.code);
    printf("DataType : 0x%02X\n", deviceProperty.dataType);
    printf("GetSet   : 0x%01X\n", deviceProperty.getSet);
    printf("FormFlag : 0x%01X\n", deviceProperty.formFlag);

    switch (deviceProperty.dataType) {
        case DATA_TYPE_CHAR:
            printf("Default  : 0x%02X\n", *(char *)deviceProperty.defaultValue);
            printf("Current  : 0x%02X\n", *(char *)deviceProperty.currentValue);
            break;

        case DATA_TYPE_WORD:
            printf("Default  : 0x%04X\n", *(WORD *)deviceProperty.defaultValue);
            printf("Current  : 0x%04X\n", *(WORD *)deviceProperty.currentValue);
            break;

        case DATA_TYPE_DWORD:
            printf("Default  : 0x%08X\n", *(DWORD *)deviceProperty.defaultValue);
            printf("Current  : 0x%08X\n", *(DWORD *)deviceProperty.currentValue);
            break;

        case DATA_TYPE_STRING:
            printStringDataSet("Default  : ", deviceProperty.defaultValue);
            printStringDataSet("Current  : ", deviceProperty.currentValue);
            break;

        default:
            printf("Unknown data type: 0x%04X!", deviceProperty.dataType);

    }

     switch (deviceProperty.formFlag) {
        case PROPERTY_FORM_NONE:
            break;
        case PROPERTY_FORM_RANGE:
            switch (deviceProperty.dataType) {
                case DATA_TYPE_CHAR:
                    printf("Minimum  : 0x%02X\n", *(char *)deviceProperty.form.rangeForm.minimumValue);
                    printf("Maximum  : 0x%02X\n", *(char *)deviceProperty.form.rangeForm.maximumValue);
                    printf("StepSize : 0x%02X\n", *(char *)deviceProperty.form.rangeForm.stepSize);
                    break;

                case DATA_TYPE_WORD:
                    printf("Minimum  : 0x%04X\n", *(WORD *)deviceProperty.form.rangeForm.minimumValue);
                    printf("Maximum  : 0x%04X\n", *(WORD *)deviceProperty.form.rangeForm.maximumValue);
                    printf("StepSize : 0x%04X\n", *(WORD *)deviceProperty.form.rangeForm.stepSize);
                    break;

                case DATA_TYPE_DWORD:
                    printf("Minimum  : 0x%08X\n", *(DWORD *)deviceProperty.form.rangeForm.minimumValue);
                    printf("Maximum  : 0x%08X\n", *(DWORD *)deviceProperty.form.rangeForm.maximumValue);
                    printf("StepSize : 0x%08X\n", *(DWORD *)deviceProperty.form.rangeForm.stepSize);
                    break;

                case DATA_TYPE_STRING:
                    printStringDataSet("Minimum  : ", deviceProperty.form.rangeForm.minimumValue);
                    printStringDataSet("Maximum  : ", deviceProperty.form.rangeForm.maximumValue);
                    printStringDataSet("StepSize : ", deviceProperty.form.rangeForm.stepSize);
                    break;

                default:
                    printf("Unknown data type: 0x%04X!", deviceProperty.dataType);

            }        
            break; 
         case PROPERTY_FORM_ENUM:
            printEnumDataSet("Values   : ", &deviceProperty.form.enumForm, deviceProperty.dataType);
            break;
            
        default:
            printf("Unknown form flag: 0x%02X!", deviceProperty.formFlag);
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
