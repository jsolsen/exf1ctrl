//
// ExF1Ctrl ver. 0.1. 
//
// This program illustrates how to build a simple timelapse application that controls the Casio EXF1. 
// Firmware rev. 2.00 is required. This version is available here:
// http://support.casio.com/download.php?rgn=4&cid=001&pid=573
//
// Written by Jens Skovgaard Olsen (info@feischmeckerfoosball.com)
// 

#include "exf1Timelapse.h"

int main(int argc, char** argv)
{
	char fileName[64]; 
	int noPictures = -1;
	double timeInterval = -1.0; 
	bool firstPictureTaken = false; 
	time_t timeNow, timeLastPictureTaken;
	exf1api exf1; 
 
	printf(" \n");
	printf(" ********************************************************************\n");
	printf(" *                                                                  *\n");
	printf(" *  ExF1TimeLapse ver. 0.1                                          *\n");
	printf(" *  -----------------                                               *\n");
	printf(" *  This program illustrates how to build a simple timelapse        *\n");
	printf(" *  application that controls the Casio EXF1 over USB.              *\n");
	printf(" *  Firmware rev. 2.00 is required and the camera must be put in    *\n");
	printf(" *  remote control mode before being connected to the host.         *\n");
	printf(" *  --                                                              *\n");
	printf(" *  Jens Skovgaard Olsen                                            *\n");
	printf(" *  info@feinschmeckerfoosball.com                                  *\n");
	printf(" *                                                                  *\n");
	printf(" ********************************************************************\n");
	printf(" \n");
	printf(" Example usage: exf1Timelapse.exe -n 5 -t 10 \n");
	printf(" The camera will then take 5 pictures with a 10 second interval.\n");
   
	// Make sure that both parameters are set. 
	if (argc != 5)
	{
		printf(" \n");
		printf(" Both -n and -t parameters must be specified. Quitting program.");
		printf(" \n");
		return 0; 
	}

	// Analyze input arguments. 
	for (int i = 1; i < argc; i++) { 
		if (argv[i][0] == '-') { 
			switch (argv[i][1]) { 
				case 'n':   
					noPictures = atoi(argv[++i]); 
					break; 
				case 't':   
					timeInterval = atof(argv[++i]); 
					break; 
				default: 
					fprintf(stdout, " Unrecognized parameter: %c!\n\n", argv[i][1]);
					fflush(stdout);
					return 0; 
			}
		}
	}    

	// Make sure that the range is valid. 
	if (noPictures < 1 || timeInterval < 1)
	{
		printf(" \n");
		printf(" Both -n and -t parameters must have a value greater than zero. Quitting program. ");
		printf(" \n");
		return 0; 
	
	}

	printf(" \n");
	printf("> Initializing camera... \n");
	fflush(stdout);

	// Init camera. 
	if (!exf1.initCamera())
		return 0; 
   
	// Perform the actual timelapse. 
	for (int i=0; i<noPictures;)
	{
		time(&timeNow);
		if (difftime(timeNow, timeLastPictureTaken) >= timeInterval || !firstPictureTaken)
		{
			printf("> Taking picture: %d/%d... \n", i+1, noPictures);

			// Generate new file name. 
			sprintf(fileName, "CIMG%06d.jpg", i); 
   		   
			// Record the time where the picture is taken. 
			time(&timeLastPictureTaken);

			// Take the picture (no thumbnails). :o) 
			exf1.shutter(fileName, "", -1);
			firstPictureTaken = true; 
			i++; 
		}
	}

	// Close the camera down. 
    exf1.exitCamera();
    exf1.terminateCamera();

	return 0;
}

