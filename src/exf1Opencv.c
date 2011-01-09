#include "exf1Opencv.h"

int main( int argc, char** argv )
{
    IplImage *frame = cvCreateImage(cvSize(640,480), 8, 3);

    if (!initCamera())
       return 0;

    setupMonitor(TRUE);

    printf("Press escape to quit the program...\n");
    while (cvWaitKey(1) != 27) {
        getCameraFrame(frame);
		cvCvtColor(frame,frame,CV_RGB2BGR);
        cvShowImage("exf1Opencv", frame);
    }

    setupMonitor(FALSE);

    cvDestroyWindow("exf1Opencv");
    exitCamera();
    return 0;
}