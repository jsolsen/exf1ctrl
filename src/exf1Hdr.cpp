//
// ExF1Ctrl ver. 0.2. 
//
// This program illustrates how to build a simple timelapse application that controls the Casio EXF1. 
// Firmware rev. 2.00 is required. This version is available here:
// http://support.casio.com/download.php?rgn=4&cid=001&pid=573
//
// Written by Jens Skovgaard Olsen (info@feischmeckerfoosball.com)
// 

#include "exf1Hdr.h"

int main(int argc, char** argv)
{
	char fileName[64]; 
	int stepSize = -1;
	bool firstPictureTaken = false; 
	exf1api exf1; 
 
	printf(" \n");
	printf(" ********************************************************************\n");
	printf(" *                                                                  *\n");
	printf(" *  ExF1Hdr ver. 0.1                                                *\n");
	printf(" *  -----------------                                               *\n");
	printf(" *  This program illustrates how to build a simple high dynamic     *\n");
	printf(" *  range application that controls the Casio EXF1 over USB.        *\n");
	printf(" *  Firmware rev. 2.00 is required and the camera must be put in    *\n");
	printf(" *  remote control mode before being connected to the host.         *\n");
	printf(" *  --                                                              *\n");
	printf(" *  Jens Skovgaard Olsen                                            *\n");
	printf(" *  info@feinschmeckerfoosball.com                                  *\n");
	printf(" *                                                                  *\n");
	printf(" ********************************************************************\n");
	printf(" \n");
	printf(" Example usage: exf1Hdr.exe -s 1 \n");
	printf(" The camera will then take 13 pictures with an EV step size of 1.\n");
   
	// Make sure that both parameters are set. 
	if (argc != 3)
	{
		printf(" \n");
		printf(" Step size (-s) must be specified. Quitting program.");
		printf(" \n");
		return 0; 
	}

	// Analyze input arguments. 
	for (int i = 1; i < argc; i++) { 
		if (argv[i][0] == '-') { 
			switch (argv[i][1]) { 
				case 's':   
					stepSize = atoi(argv[++i]); 
					break; 
				default: 
					fprintf(stdout, " Unrecognized parameter: %c!\n\n", argv[i][1]);
					fflush(stdout);
					return 0; 
			}
		}
	}    

	// Make sure that the range is valid. 
	if (stepSize < 1)
	{
		printf(" \n");
		printf(" Step size (-s) must have a value greater than zero. Quitting program. ");
		printf(" \n");
		return 0; 
	
	}

	printf(" \n");
	printf("> Initializing camera... \n");
	fflush(stdout);

	// Init camera. 
	if (!exf1.initCamera())
		return 0; 
   
	exf1.setupExposure(DATA_EXPOSURE_A); 

	// Sweep through exposure values using the selected step size. 
	for (int i=(int)exf1.M2_0; i<=(int)exf1.P2_0; i+=stepSize)
	{
		// Configure exposure value. 
		exf1.setupExposureValue((exf1api::EV)i); 

		printf("> Taking picture (EV=%d/%d)...\n", i, (int)exf1.P2_0);

		// Generate new file name. 
		sprintf(fileName, "CIMG%06d.jpg", i); 

		// Take the picture (no thumbnails). :o) 
		exf1.shutter(fileName, "", -1);
	}

	// Close the camera down. 
    exf1.exitCamera();
    exf1.terminateCamera();

	return 0;
}

