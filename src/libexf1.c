#include "libexf1.h"

usb_dev_handle *dev = NULL;
char tmp[BUF_SIZE];
char img[IMG_BUF_SIZE];
char interruptBuffer[24]; 
PTP_DEVICE_INFO deviceInfo;
PTP_DEVICE_PROPERTY deviceProperty;
PTP_OBJECT_INFO objectInfo;
DWORD_DATA_SET *objectHandles;
PTP_CONTAINER *rx; 
DWORD zoomSetting;
DWORD focusSetting;
DWORD USB_CMD_ID = 0xFFFFFFFF;
DWORD frameNo = 0;

void exf1Cmd(WORD cmd, ...)
{
    DWORD dwordVal;
    WORD wordVal;
    char *pString;
    int *pInt;
    int i;

    va_list ap;
    va_start(ap, cmd);

    //if (cmd != CMD_CF_RELEASE && cmd != CMD_CZ_RELEASE)
    //    while (usbRxEvent() > 0);

    switch(cmd){
        case CMD_WRITE:
            dwordVal = va_arg(ap, int);  // Address
            wordVal  = va_arg(ap, int);  // Value.
            usbTx(cmd, TYPE_CMD,  sizeof(dwordVal),(DWORD) dwordVal, 0);
            usbTx(cmd, TYPE_DATA, sizeof(wordVal), (DWORD) wordVal, 0);
            usbRx();

            if (dwordVal == ADDR_FUNCTIONALITY || dwordVal == ADDR_MONITOR) {
                //do
                    //usbRxEvent();
                    //while (usbRxEvent() > 0);
                //while (rx->code != EVT_DEVICE_INFO_CHANGED);
            }

            break;

        case CMD_READ:
            break;

        case CMD_MOVIE_RESET: 
        case CMD_CS_RELEASE:
        case CMD_MOVIE_RELEASE:
        case CMD_STILL_START:
        case CMD_MOVIE_START:
            dwordVal = va_arg(ap, int);
            usbTx(cmd, TYPE_CMD, sizeof(dwordVal), (DWORD) dwordVal, 0);
//            if (dwordVal) { // PreRecordEnabled...
                i = 0;
                do {
                    usbGetStatus();
                    i++; 
                } while (usbRx() < 0 && i<10);
  //          }
    //        else
      //          usbRx();
            break;
            //usbRxEvent();

        case CMD_CS_PRESS:
        case CMD_SHUTTER:
        case CMD_HALF_PRESS:
            usbTx(cmd, TYPE_CMD, 0, 0, 0);
            //Sleep(500); // Wait for focus event.
            //do
            //usbRxEvent();
            //while (rx->code != EVT_FOCUS_OK);
            //printf("Event: 0x%04x\n", rx->code);
            usbRx();
            break;

        case CMD_STILL_RESET:
        case CMD_MOVIE_PRESS:
        case CMD_CLOSE_SESSION:
        case CMD_HALF_RELEASE:
        case CMD_STILL_STOP:
        case CMD_MOVIE_STOP:
            usbTx(cmd, TYPE_CMD, 0, 0, 0);
            //Sleep(250);
            //usbRxEvent();
            usbRx();
            break;

        case CMD_CF_PRESS:
        case CMD_CZ_PRESS:
            dwordVal = va_arg(ap, int);
            usbTx(cmd, TYPE_CMD, 2*sizeof(dwordVal), (DWORD) dwordVal, 0);
            usbRx();
            break;
        
        case CMD_CF_RELEASE:
        case CMD_CZ_RELEASE:
        case CMD_ZOOM:
        case CMD_FOCUS:
            dwordVal = va_arg(ap, int);
            usbTx(cmd, TYPE_CMD, 2*sizeof(dwordVal), (DWORD) dwordVal, 0);
            //while (usbRxEvent() > 0);
            usbRx();
            //while (usbRxEvent() > 0);
            break;

        case CMD_OPEN_SESSION:
            USB_CMD_ID = 0;
            dwordVal = va_arg(ap, int);
            usbTx(cmd, TYPE_CMD, sizeof(dwordVal), (DWORD) dwordVal, 0);
            usbRx();
            break;

        case CMD_GET_STILL_HANDLES:
        case CMD_GET_MOVIE_HANDLES:
        case CMD_GET_DEVICE_INFO:
            usbTx(cmd, TYPE_CMD, 0, 0, 0);
            usbRx();
            usbRx();
            break;

        case CMD_GET_OBJECT_INFO:
            dwordVal = va_arg(ap, int);
            usbTx(cmd, TYPE_CMD, 2*sizeof(DWORD), dwordVal, 0xFFFFFFFF);
            usbRx();
            usbRx();
            break;

        case CMD_GET_PROP_DESC:
            dwordVal = va_arg(ap, int);
            usbTx(cmd, TYPE_CMD, sizeof(dwordVal), (DWORD) dwordVal, 0);
            usbRx();
            usbRx();
            break;

        case CMD_GET_THUMBNAIL:
        case CMD_GET_OBJECT:
            wordVal  = va_arg(ap, int); // File/memory destination.
            dwordVal = va_arg(ap, int); // objectHandle
            pString = (char *) va_arg(ap, int); //
            
            switch (wordVal) {
                case TO_FILE:
                    usbTx(cmd, TYPE_CMD, sizeof(DWORD), dwordVal, 0);
                    usbRxToFile(pString);
                    usbRx();
                    break;
                case TO_MEM:
                    pInt = (int *) va_arg(ap, int); //
					do {
						usbTx(cmd, TYPE_CMD, sizeof(DWORD), dwordVal, 0);
						usbRxToMem(pString, pInt);
                    } while (rx->code == 0x2019);
					usbRx();
                    break;
            }
            break;

        default:
            printf("Unsupported command type!\n");
    }
    
    va_end(ap);
}

void usbTx(WORD cmd, WORD cmdType, int nCmdParameterBytes, DWORD cmdParameter1, DWORD cmdParameter2) {

    PTP_CONTAINER tx;
    int packetSize = 12 + nCmdParameterBytes;

    tx.length   = packetSize;
    tx.type     = cmdType;
    tx.code     = cmd;
    tx.trans_id = USB_CMD_ID;

    if (nCmdParameterBytes == 2) {
        tx.payload.word_params.param1 = 0xFFFF & cmdParameter1;
    }
    else if (nCmdParameterBytes == 4) {
        tx.payload.dword_params.param1 = cmdParameter1;
    }
    else if (nCmdParameterBytes == 8) {
        tx.payload.dword_params.param1 = cmdParameter1;
        tx.payload.dword_params.param2 = cmdParameter2;
    }

    /*
    int i;
    char *pTx = (char *) &tx;
    for (i=0; i<packetSize; i++)
        printf("%02X-", 0xFF & *(pTx + i));
    printf("\n");
    */

    //while (usbRxEvent()>0);
	usbRxEvent();
	//printf("Grabbing frame: %d\n", frameNo);
    if (usb_bulk_write(dev, EP_OUT, (char *) &tx, packetSize, 100) != packetSize)
        printf("Error: Bulk write failed for this command: %02X!\n", 0xFFFF & cmd);

    USB_CMD_ID++;
}

int usbRx() {

    int bytesRead = -1;

    // Clear old PTP header.
    memset(tmp, 0, 12);

    do {
        //while (usbRxEvent()>0);
		usbRxEvent();
        bytesRead = usb_bulk_read(dev, EP_IN, tmp, BUF_SIZE, 100);
        //printf("bytesRead (usbRx): %d\n", bytesRead);
    } while (bytesRead == -116);

    rx = (PTP_CONTAINER *) tmp;
    
    if (bytesRead > 0) {
/*
        printf("usbRx: 0x%04X 0x%04X\n", rx->code, rx->type);
        char *pTx = rx->payload.data;
        for (i=0; i<rx->length-12; i++)
            printf("%02X-", 0xFF & *(pTx + i));
        printf("\n");
*/
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

                    case CMD_GET_OBJECT_INFO:
                        setObjectInfo(rx->payload.data);
                        //printObjectInfo();
                        break;

                    case CMD_GET_STILL_HANDLES:
                    case CMD_GET_MOVIE_HANDLES:
                        free(objectHandles);
                        getDwordDataSet(&objectHandles, rx->payload.data);
                        //printDwordDataSet("ObjectHandles   : ", objectHandles);
                        break; 

                    default:
                        printf("Unhandled data package (usbRx): 0x%04X 0x%04X ", rx->code, rx->type);
                        /*char *pTx = rx->payload.data;
                        for (i=0; i<rx->length-12; i++)
                            printf("%02X-", 0xFF & *(pTx + i));
                        printf("\n");*/
                        break; 
                }
                break;
            case TYPE_RESPONSE:
                if (rx->code != CMD_OK) printf("Ack (usbRx) = %02X.\n", rx->code);
                break;
                /*
            case TYPE_EVENT:
                printf("Read an event message.\n");
                //usbRx();
                if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
                    printf(" Error: interrupt read failed!\n");
                bytesRead = rx->code; 
                break;
                 * */
            default:
                printf("Unknown message type: 0x%04x\n", 0xFFFF & rx->type);
        }
    }
    //else if (bytesRead != -116) {
    else
        printf("Error: Bulk read failed (usbRx)! %d %s\n", bytesRead, usb_strerror());
    //}
    return bytesRead;
}

int usbRxToFile(char *fileName) {

    FILE *pFile;
    
    int bytesRead = -1;
    int bytesRemaining = -1, objectSize = 0;

    // Clear old PTP header. 
    memset(img, 0, 12); 

    do {
        //while (usbRxEvent() > 0);
		usbRxEvent();
        bytesRead = usb_bulk_read(dev, EP_IN, img, IMG_BUF_SIZE, 100);
        //printf("bytesRead (usbRxToFile): %d\n", bytesRead);
    } while (bytesRead == -116);
    
    rx = (PTP_CONTAINER *) img;

    if (bytesRead > 0) {

        switch (rx->type) {
            case TYPE_DATA:
                objectSize = rx->length-12;
                pFile = fopen(fileName, "wb");
                fwrite(rx->payload.data, 1, bytesRead-12, pFile);
                printf("> Transferring %d bytes... \n", objectSize);
                for (bytesRemaining = rx->length - bytesRead; bytesRemaining > 0; bytesRemaining -= bytesRead) {
                    do bytesRead = usb_bulk_read(dev, EP_IN, img, IMG_BUF_SIZE, TIME_OUT);
                    while (bytesRead < 0);
                    fwrite(img, 1, bytesRead, pFile);
                }
                fclose (pFile);
                printf("> %s saved to disk. \n", fileName);
                break;
            case TYPE_RESPONSE:
                if (rx->code != CMD_OK) printf("Ack (usbRxToFile) = %02X.\n", rx->code);
                break;
            default:
                printf("Unhandled data package (usbRxToFile): 0x%04X 0x%04X\n", rx->type, rx->code);
                /*char *pTx = rx->payload.data;
                int i;
                for (i=0; i<rx->length-12; i++)
                    printf("%02X-", 0xFF & *(pTx + i));
                printf("\n");*/
                break; 
        }
    }
    return bytesRemaining;
}

int usbRxToMem(char *jpgImage, int *jpgSize) {

    int bytesRead = -1;
    int bytesRemaining = -1;

    // Clear old PTP header.
    memset(img, 0, 12);

    do {
        //while (usbRxEvent() > 0);
		usbRxEvent();
        bytesRead = usb_bulk_read(dev, EP_IN, img, IMG_BUF_SIZE, 50);
        //printf("bytesRead (usbRxToMem): %d\n", bytesRead);
    } while (bytesRead == -116);

    rx = (PTP_CONTAINER *) img;

    if (bytesRead > 0) {

        switch (rx->type) {
            case TYPE_DATA:
                *jpgSize = rx->length-12;
                memcpy(jpgImage, rx->payload.data, bytesRead-12);
                jpgImage -= 12; // Don't count the PTP header...

                for (bytesRemaining = rx->length - bytesRead; bytesRemaining > 0; bytesRemaining -= bytesRead) {
                    jpgImage += bytesRead;
                    printf("jpgSize: %d\n",jpgSize); 
					
					/*do {
						usbRxEvent();
						bytesRead = usb_bulk_read(dev, EP_IN, jpgImage, 512 * ((int)(bytesRemaining/512)+1), 100);
						//printf("bytesRead (usbRxToMem): %d\n", bytesRead);
					} while (bytesRead == -116);*/
                }
                break;
            case TYPE_RESPONSE:
                if (rx->code != CMD_OK) printf("Ack (usbRxToMem) = %02X.\n", rx->code);
                break;
            default:
                printf("Unhandled data package (usbRxToMem): 0x%04X 0x%04X\n", rx->type, rx->code);
                /*char *pTx = rx->payload.data;
                int i;
                for (i=0; i<rx->length-12; i++)
                    printf("%02X-", 0xFF & *(pTx + i));
                printf("\n");*/
                break;
        }
    }
    else if (bytesRead < 0) {
        printf("Error: Bulk read failed (usbRxToMem)! %d %s\n", bytesRead, usb_strerror());
        memset(img,0,IMG_BUF_SIZE);
    }
    return bytesRemaining;
}

int usbRxEvent(){

//    char tmpBuffer[16];

    //int bytesRead = usb_interrupt_read(dev, EP_INT, tmp, 24, 5);

    int bytesRead = -1;

    // Clear tmp buffer.
    memset(tmp, 0, 16);
    rx = (PTP_CONTAINER *) tmp;

	bytesRead = usb_interrupt_read(dev, EP_INT, tmp, 24, 10);

	/*
    // Attempt read.
    bytesRead = usb_bulk_read(dev, EP_INT, tmp, 16, 20);
    if (rx->length > bytesRead && bytesRead > 0) {
        bytesRead += usb_bulk_read(dev, EP_INT, tmp+bytesRead, 16, 20);
		if (bytesRead != -116 && bytesRead < 0)
			printf(" Error: interrupt read 2 failed (usbRxEvent): %s %d!\n", usb_strerror(), bytesRead);
	}
	*/
    if (bytesRead > 0) {

        /*printf("usbRxEvent: 0x%04X 0x%04X ", rx->code, rx->type);
        for (i=0; i<12; i++)
            printf("%02X-", 0xFF & rx->payload.data[i]);
        printf("\n");*/

        if (rx->type == TYPE_EVENT) {
			//printf("Got 0x%04x event...\n", rx->code); 
            switch (rx->code) {
                case EVT_MONITOR_CHANGED:
					frameNo = rx->payload.dword_params.param2;
                    //printf("frameNo: %d\n", frameNo);
                    break; 
                case EVT_FOCUS_CHANGED:
                    focusSetting = rx->payload.dword_params.param1;
                    break;
                case EVT_SOMETHING_CHANGED:
                case EVT_FOCUS_OK:
                case EVT_DEVICE_INFO_CHANGED:
                    break;
                case EVT_ZOOM_CHANGED:
                    zoomSetting = rx->payload.dword_params.param1;
                    break;
                default:
                    printf("Unhandled event code: 0x%04x\n", rx->code);
            }
        }
        //else
        //    printf("Not an event message!\n");
    }
    else if (bytesRead != -116)
        printf(" Error: interrupt read failed (usbRxEvent): %s %d!\n", usb_strerror(), bytesRead);

    return bytesRead;
}

void usbGetStatus() {

    char statusBytes[2];
    
    if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x81, statusBytes, 0x2, TIME_OUT) < 0)
        printf("error: cmd write 1 failed\n");
    //printf("81 status: 0x%02x\n", statusBytes);

    if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x2, statusBytes, 0x2, TIME_OUT) < 0)
        printf("error: cmd write 2 failed\n");
    //printf("02 status: 0x%02x\n", statusBytes);
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

DWORD getDwordDataSet(DWORD_DATA_SET **dst, char *src) {

    DWORD byteSize;
    DWORD_DATA_SET *wds = (DWORD_DATA_SET *) src;

    byteSize = (wds->noItems + 1) * sizeof(DWORD);
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

    DWORD i;
    printf("%s", pDescrition);
    for (i=0; i<pDataSet->noItems; i++)
        printf("0x%04X, ", pDataSet->data[i]);
    printf("\n");

}

void printDwordDataSet(char *pDescrition, DWORD_DATA_SET *pDataSet) {

    DWORD i;
    printf("%s", pDescrition);
    for (i=0; i<pDataSet->noItems; i++)
        printf("0x%08X, ", pDataSet->data[i]);
    printf("\n");

}

void printDeviceInfo() {

    printStringDataSet("Manufacturer  : ", deviceInfo.manufacturer);
    printStringDataSet("Model         : ", deviceInfo.model);
    printStringDataSet("DeviceVersion : ", deviceInfo.deviceVersion);
    printStringDataSet("SerialNumber  : ", deviceInfo.serialNumber);
                printf("ExtensionID   : 0x%08X\n", deviceInfo.vendorExtensionID);
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



void setObjectInfo(char *pData) {

    // Free up old data sets.
    free(objectInfo.fileName);
    free(objectInfo.captureDate);
    free(objectInfo.modificationDate);
    free(objectInfo.keyWords);

    // Retrieve new object info from packet.
    objectInfo.storageId = GET_DWORD(pData);  pData += sizeof(DWORD);
    objectInfo.objectFormat = GET_WORD (pData);  pData += sizeof(WORD);
    objectInfo.protectionStatus = GET_WORD (pData);  pData += sizeof(WORD);
    objectInfo.objectCompressedSize = GET_DWORD(pData);  pData += sizeof(DWORD);
    objectInfo.thumbFormat = GET_WORD (pData);  pData += sizeof(WORD);
    objectInfo.thumbCompressedSize = GET_DWORD(pData);  pData += sizeof(DWORD);
    objectInfo.thumbPixWidth = GET_DWORD(pData);  pData += sizeof(DWORD);
    objectInfo.thumbPixHeight = GET_DWORD(pData);  pData += sizeof(DWORD);
    objectInfo.imagePixWidth = GET_DWORD(pData);  pData += sizeof(DWORD);
    objectInfo.imagePixHeight = GET_DWORD(pData);  pData += sizeof(DWORD);
    objectInfo.imageBitDepth = GET_DWORD(pData);  pData += sizeof(DWORD);
    objectInfo.parentObject = GET_DWORD(pData);  pData += sizeof(DWORD);
    objectInfo.associationType = GET_WORD (pData);  pData += sizeof(WORD);
    objectInfo.associationDesc = GET_DWORD(pData);  pData += sizeof(DWORD);
    objectInfo.sequenceNumber = GET_DWORD(pData);  pData += sizeof(DWORD);
    pData += getStringDataSet(&objectInfo.fileName, pData);
    pData += getStringDataSet(&objectInfo.captureDate, pData);
    pData += getStringDataSet(&objectInfo.modificationDate, pData);
    pData += getStringDataSet(&objectInfo.keyWords, pData);

}

void printObjectInfo() {

    printStringDataSet("Filename        : ", objectInfo.fileName);
    printStringDataSet("CaptureDate     : ", objectInfo.captureDate);
    printStringDataSet("Modification    : ", objectInfo.modificationDate);
    printStringDataSet("Keywords        : ", objectInfo.keyWords);
                printf("StorageID       : 0x%08X\n", objectInfo.storageId);
                printf("ObjectFormat    : 0x%04X\n", objectInfo.objectFormat);
                printf("Protection      : 0x%04X\n", objectInfo.protectionStatus);
                printf("ObjectDimension : %dx%dx%d\n", objectInfo.imagePixWidth, objectInfo.imagePixHeight, objectInfo.imageBitDepth);
                printf("CompressedSize  : %d bytes\n", objectInfo.objectCompressedSize);
                printf("ThumbFormat     : 0x%04X\n", objectInfo.thumbFormat);
                printf("ThumbSize       : %d bytes\n", objectInfo.thumbCompressedSize);
                printf("ThumbDimension  : %dx%d\n", objectInfo.thumbPixWidth, objectInfo.thumbPixHeight);
                printf("ParentObject    : 0x%08X\n", objectInfo.parentObject);
                printf("AssociationType : 0x%04X\n", objectInfo.associationType);
                printf("AssociationDesc : 0x%08X\n", objectInfo.associationDesc);
                printf("SequenceNumber  : 0x%08X\n", objectInfo.sequenceNumber);

}

int usbInit()
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

    // DeviceReset.
    if (usb_control_msg(dev, 0x21, 0x66, 0, 0, NULL, 0, TIME_OUT) < 0)
      printf("error: cmd write 1 failed\n");

    usb_resetep(dev, EP_IN);
    usb_resetep(dev, EP_OUT);

    usbRxEvent();

    if (usb_clear_halt(dev, EP_IN) < 0)
      printf("error: halt clear failed.\n");

    if (usb_clear_halt(dev, EP_OUT) < 0)
      printf("error: halt clear failed.\n");

    // GetDeviceStatus.
    if (usb_control_msg(dev, 0xA1, 0x67, 0x00, 0x00, &tmp[0], 4, TIME_OUT) < 0)
      printf("error: cmd write 2 failed\n");

    //printf("DeviceStatus: 0x%04x...\n", GET_DWORD(tmp));

    return 1; 
}

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
