#include "exf1Opencv.h"

static const char * const cdjpeg_message_table[] = {
  NULL
};

void getCameraFrame(IplImage* frame)
{
    char jpgImage[3*IMG_BUF_SIZE];
    int jpgSize;
	int offset = 0;
    JSAMPROW rowptr[1];
    JDIMENSION num_scanlines;

    jpgSize = grapPcMonitorFrame(jpgImage);
    //printf("jpgSize: %d\n", jpgSize);
    if (jpgSize > 0) {

        //cvInitImageHeader(frame, cvSize(640,480), IPL_DEPTH_8U, 3, IPL_ORIGIN_TL, 4);
        //cvCreateData(frame);

        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);

        jerr.addon_message_table = cdjpeg_message_table;
        //jerr.first_addon_message = JMSG_FIRSTADDONCODE;
        //jerr.last_addon_message = JMSG_LASTADDONCODE;

        // Now safe to enable signal catcher.
        #ifdef NEED_SIGNAL_CATCHER
          //enable_signal_catcher((j_common_ptr) &cinfo);
        #endif

        jpeg_mem_src(&cinfo, (unsigned char*) jpgImage, (unsigned long) jpgSize);
        //(void) jpeg_read_header(&cinfo, TRUE);

        if (jpeg_read_header(&cinfo, TRUE) == JPEG_HEADER_OK) {
            jpeg_start_decompress(&cinfo);

            while (cinfo.output_scanline < cinfo.output_height) {
                rowptr[0] = (JSAMPROW)&(frame->imageData[offset * 640 * 3]);
                num_scanlines = jpeg_read_scanlines(&cinfo, rowptr,1);

                offset+=1;
            }
            jpeg_finish_decompress(&cinfo);
            cvCvtColor(frame,frame,CV_RGB2BGR);
        }
        jpeg_destroy_decompress(&cinfo);
    }
    else
        printf("JPG size is negative!\n"); 
}

int main( int argc, char** argv )
{
    IplImage *frame; 
	
	frame = cvCreateImage(cvSize(640,480), 8, 3);

    if (!initCamera())
       return 0;

    setupMonitor(TRUE);

    printf("Press escape to quit the program...\n");
    while (cvWaitKey(1) != 27) {
        getCameraFrame(frame);
        cvShowImage("exf1Opencv", frame);
    }

    setupMonitor(FALSE);

    cvDestroyWindow("exf1Opencv");
    exitCamera();
    return 0;
}
