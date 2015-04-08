The purpose of this project is to reverse engineer the USB protocol controling the Casio EX-F1 high speed camera.

The following camera functionality is currently available:

  * Configuration of
    * Aperture
    * Movie / still mode
    * Exposure
    * Flash mode
    * Focus
    * ISO
    * Frame rate
    * Shutter speed

  * Control of
    * Shutter
    * Movie shutter
    * Manual focus
    * Zoom
    * Live view
    * 30/300FPS

Four example projects are provided:

  * exf1Ctrl
    * Demonstrates the basic usage of the API.

  * exf1Hdr
    * Demonstrates how to build a simple HDR application.

  * exf1Opencv
    * Demonstrates the live view functionality and provides an interface to OpenCV.

  * exf1Timelapse
    * Demonstrates how to build a simple timelapse application.

All projects compile with GCC (using Netbeans) and MSVC (using Visual Studio 2010 express). There are a couple of different ways you can use the compiled binaries:

  * Windows
    * Using Casio QV Remote Device Driver
      1. Using libusb < 1.x.x: Just install libusb win32.
      1. Using libusb > 1.x.x: Install libusb win32 and add a filter.
    * Using libexf1 driver
      1. Just update the driver using device manager.
    * Using libusb-1.0 windows backend
      1. Install libusb-1.0 windows backend and select WinUSB driver in Zadig.exe.
      1. Recompile binaries with LIBUSB\_VER set to 1.

  * Linux
    * Works with libusb-0.1 and libusb-1.0.
      1. Tested on Pandaboard ES running Ubuntu.

Please note that the code is developed for the sake of my own hobby projects, but I hope that others will find it useful too. Please use and modify the code at your own risk.

Jens Skovgaard Svane ([email](mailto:info@feinschmeckerfoosball.com))