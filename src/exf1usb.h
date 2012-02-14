/* 
 * File:   exf1usb.h
 * Author: Jens Skovgaard Olsen
 *
 * Created on 27. december 2011, 17:33
 */

#ifndef EXF1USB_H
#define	EXF1USB_H

#define LIBUSB_VER 0
//#define PANDABOARD 

#ifdef PANDABOARD
    #define CTRL_TIME_OUT			3500
    #define READ_TIME_OUT			150
    #define WRITE_TIME_OUT			150
#else
    #define CTRL_TIME_OUT			3500
    #define READ_TIME_OUT			100
    #define WRITE_TIME_OUT			100
#endif

#if LIBUSB_VER == 1 

    #include <libusb.h>

    #define usbHandle       libusb_device_handle

    #define usbBulkWrite(pData, packetSize, pBytesTransfered)       libusb_bulk_transfer(dev, EP_OUT, (unsigned char *) pData, packetSize, pBytesTransfered, WRITE_TIME_OUT)   
    #define usbBulkRead(pData, packetSize, pBytesTransfered)        libusb_bulk_transfer(dev, EP_IN, (unsigned char *) pData, packetSize, pBytesTransfered, READ_TIME_OUT)
    #define usbInterruptRead(pData, packetSize, pBytesTransfered)   libusb_interrupt_transfer(dev, EP_INT, (unsigned char *) pData, packetSize, pBytesTransfered, READ_TIME_OUT)
    
    #define usbControl(type, req, val, idx, pData, length) libusb_control_transfer(dev, type, req, val, idx, (unsigned char *) pData, length, CTRL_TIME_OUT)

    #define usbInit()       libusb_init(NULL)
    #define usbSetDebug(x)  libusb_set_debug(NULL, x)
    #define usbReset()      libusb_reset(dev)
    #define usbError()      printf("Error\n") //libusb_strerror(LIBUSB_ERROR_NOT_SUPPORTED)

    #define usbSetConfig(x) libusb_set_configuration(dev, x)
    #define usbClaim(x)     libusb_claim_interface(dev, x)
    #define usbResetEP(x)   libusb_resetep(dev, x)
    #define usbClearHalt(x) libusb_clear_halt(dev, x)
    #define usbClose()      libusb_close(dev)

#else

    #include <usb.h>

    #define usbHandle       usb_dev_handle

    #define usbBulkWrite(pData, packetSize, pBytesTransfered)      *pBytesTransfered = usb_bulk_write(dev, EP_OUT, (char *) pData, packetSize, WRITE_TIME_OUT)   
    #define usbBulkRead(pData, packetSize, pBytesTransfered)       *pBytesTransfered = usb_bulk_read(dev, EP_IN, (char *) pData, packetSize, READ_TIME_OUT)
    #define usbInterruptRead(pData, packetSize, pBytesTransfered)  *pBytesTransfered = usb_interrupt_read(dev, EP_INT, (char *) pData, packetSize, READ_TIME_OUT)
    
    #define usbControl(type, req, val, idx, pData, length) usb_control_msg(dev, type, req, val, idx, pData, length, CTRL_TIME_OUT)

    #define usbInit()       usb_init()
    #define usbSetDebug(x)  usb_set_debug(x)
    #define usbError()      usb_strerror()

    #define usbSetConfig(x) usb_set_configuration(dev, x)
    #define usbClaim(x)     usb_claim_interface(dev, x)
    #define usbReset()      usb_reset(dev) 
    #define usbResetEP(x)   usb_resetep(dev, x)
    #define usbClearHalt(x) usb_clear_halt(dev, x)
    #define usbClose()      usb_close(dev)

    #define usbReleaseInterface(x) usb_release_interface(dev, x) 
    #define usbReset()      usb_reset(dev)
    #define usbClose()      usb_close(dev)

#endif

#define MY_VID 0x07CF
#define MY_PID 0x1023

#define EP_IN 0x81
#define EP_OUT 0x02
#define EP_INT 0x83

#define BUF_SIZE 512
#define IMG_BUF_SIZE		196608 //131072

#endif