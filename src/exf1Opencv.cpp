#include "exf1Opencv.h"

int main( int argc, char** argv )
{
    IplImage *frame = cvCreateImage(cvSize(640,480), 8, 3);
	exf1api exf1; 

    if (!exf1.initCamera())
       return 0;

    exf1.setupMonitor(TRUE);

    printf("Press escape to quit the program...\n");
    while (cvWaitKey(1) != 27) {
        exf1.getCameraFrame(frame);
		//cvCvtColor(frame,frame,CV_RGB2BGR);
        cvShowImage("exf1Opencv", frame);
    }

    exf1.setupMonitor(FALSE);

    cvDestroyWindow("exf1Opencv");
    exf1.exitCamera();
    return 0;
}