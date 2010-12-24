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

