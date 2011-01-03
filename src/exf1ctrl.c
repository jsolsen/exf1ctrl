//
// ExF1Ctrl ver. 0.1. 
//
// This program can be used to interface the Casio EX-F1 over USB. 
// Firmware rev. 2.00 is required. This version is available here:
// http://support.casio.com/download.php?rgn=4&cid=001&pid=573
//
// Written by Jens Skovgaard Olsen (info@feischmeckerfoosball.com)
// 

#include "exf1ctrl.h"

int main(int argc, char** argv)
{
   char input[64], com, name[32], tname[32], jpgImage[3*IMG_BUF_SIZE];
   int arg, nargs, addr, val, i;
 
   printf(" \n");
   printf(" ********************************************************************\n");
   printf(" *                                                                  *\n");
   printf(" *  ExF1Ctrl ver. 0.1                                               *\n");
   printf(" *  -----------------                                               *\n");
   printf(" *  This program is able to interface to the Casio EX-F1 over USB.  *\n");
   printf(" *  Firmware rev. 2.00 is required and the camera must be put in    *\n");
   printf(" *  remote control mode before being connected to the host.         *\n");
   printf(" *  --                                                              *\n");
   printf(" *  Jens Skovgaard Olsen                                            *\n");
   printf(" *  info@feinschmeckerfoosball.com                                  *\n");
   printf(" *                                                                  *\n");
   printf(" ********************************************************************\n");
   printf(" \n");
   printf(" Hint: a [x] sets aperture (x = 1-10).\n");
   printf("          1: F2.7 (default).\n");
   printf("          2: F3.0.\n");
   printf("          3: F3.3.\n");
   printf("          4: F3.8.\n");
   printf("          5: F4.2.\n");
   printf("          6: F4.7.\n");
   printf("          7: F5.3.\n");
   printf("          8: F6.0.\n");
   printf("          9: F6.7.\n");
   printf("         10: F7.5.\n");
   printf(" \n");
   printf(" Hint: c [x] sets mode / movie mode (x = 1-9).\n");
   printf("          1: Single shot (default).\n");
   printf("          2: Continuous shutter.\n");
   printf("          3: Prerecord still image.\n");
   printf("          4: Movie (STD).\n");
   printf("          5: Prerecord movie (STD).\n");
   printf("          6: Movie (HD).\n");
   printf("          7: Prerecord movie (HD).\n");
   printf("          8: Movie (HS).\n");
   printf("          9: Prerecord movie (HS).\n");
   printf("\n");
   printf(" Hint: e [x] sets exposure (x = 1-4).\n");
   printf("          1: M.\n");
   printf("          2: Auto (default).\n");
   printf("          3: A.\n");
   printf("          4: S.\n");
   printf("\n");
   printf(" Hint: f [x] sets focus (x = 1-4).\n");
   printf("          1: Auto (default).\n");	
   printf("          2: Macro.\n");
   printf("          3: Infinity.\n");
   printf("          4: Manual.\n");
   printf("\n");
   printf(" Hint: h activates half press.\n");
   printf("\n");
   printf(" Hint: i [x] sets iso (x = 1-6).\n");
   printf("          1: Auto (default).\n");
   printf("          2: 100.\n");
   printf("          3: 200.\n");
   printf("          4: 400.\n");
   printf("          5: 800.\n");
   printf("          6: 1600.\n");
   printf("\n");
   printf(" Hint: q quits this program.\n");
   printf("\n");
   printf(" Hint: m [x [y]] records a x second long movie called y.\n");
   printf("\n");
   printf(" Hint: s [x [y [z]]] activates shutter and stores a picture called x\n");
   printf("       and a thumbnail called y. If the continous shutter is enabled\n");
   printf("       (modes 2 and 3), z determines the shutter duration.\n");
   printf("\n");
   printf(" Hint: v [x [y]] focus y steps. x=in focuses in and x=out focuses out.\n");
   printf("       Continous focus is used if y is not defined.\n");
   printf("\n");
   printf(" Hint: z [x [y]] zooms y steps. x=in zooms in and x=out zooms out.\n");
   printf("       Continous zoom is used if y is not defined.\n");
   printf("\n");
   printf("\n");
   printf("> Initializing camera... \n");
   fflush(stdout);
		   
   if (!initCamera())
     return 0; 
   
   while (1)
   {
      printf("> ");
      
      gets(input);
      sscanf(input, "%c", &com);
      
      switch (com) {
         case 'a':
            nargs = sscanf(input, "%c %d", &com, &arg);
            printf("> Configuring aperture... \n");
            switch (arg) {
               case 1:
                 setupAperture(DATA_APERTURE_F2_7);
                 break;
               case 2:
                 setupAperture(DATA_APERTURE_F3_0);
                 break;
               case 3:
                 setupAperture(DATA_APERTURE_F3_3);
                 break;
               case 4:
                 setupAperture(DATA_APERTURE_F3_8);
                 break;
               case 5:
                 setupAperture(DATA_APERTURE_F4_2);
                 break;
               case 6:
                 setupAperture(DATA_APERTURE_F4_7);
                 break;
               case 7:
                 setupAperture(DATA_APERTURE_F5_3);
                 break;
               case 8:
                 setupAperture(DATA_APERTURE_F6_0);
                 break;
               case 9:
                 setupAperture(DATA_APERTURE_F6_7);
                 break;
               case 10:
                 setupAperture(DATA_APERTURE_F7_5);
                 break;
               default:
                 printf("> Unknown aperture setting. \n");
            }
            break;

         case 'c': 
            nargs = sscanf(input, "%c %d", &com, &arg);
            printf("> Configuring shutter mode / movie mode... \n");
            switch (arg) {
                case 1:
                    setupShutter(SHUTTER_NORMAL, FALSE);
                    break;
                case 2:
                    setupShutter(SHUTTER_CONTINOUS, FALSE);
                    break;
                case 3:
                    setupShutter(SHUTTER_NORMAL, TRUE);
                    break;
                case 4:
                    setupMovie(MOVIE_STD, FALSE);
                    break;
                case 5:
                    setupMovie(MOVIE_STD, TRUE);
                    break;
                case 6:
                    setupMovie(MOVIE_HD, FALSE);
                    break; 
                case 7:
                    setupMovie(MOVIE_HD, TRUE);
                    break; 
                case 8:
                    setupMovie(MOVIE_HS, FALSE);
                    break; 
                case 9:
                    setupMovie(MOVIE_HS, TRUE);
                    break;
                default:
                    printf("> This mode is yet to be supported. \n");
            }
            break; 

          case 'e':
            nargs = sscanf(input, "%c %d", &com, &arg);
            printf("> Configuring exposure... \n");
            switch (arg) {
               case 1:
                 setupExposure(DATA_EXPOSURE_M);
                 break;
               case 2:
                 setupExposure(DATA_EXPOSURE_AUTO);
                 break;
               case 3:
                 setupExposure(DATA_EXPOSURE_A);
                 break;
               case 4:
                 setupExposure(DATA_EXPOSURE_S);
                 break;
               default:
                 printf("> Unknown focus setting. \n");
            }
            break; 

          case 'f':
            nargs = sscanf(input, "%c %d", &com, &arg);
            printf("> Configuring focus... \n");
            switch (arg) {
               case 1:
                 setupFocus(DATA_FOCUS_AF);
                 break;
               case 2:
                 setupFocus(DATA_FOCUS_MACRO);
                 break;
               case 3:
                 setupFocus(DATA_FOCUS_INFINITY);
                 break;
               case 4:
                 setupFocus(DATA_FOCUS_MANUAL);
                 break;
               default:
                 printf("> Unknown exposure setting. \n");
            }
            break;

         case 'g':
             nargs = sscanf(input, "%c %d", &com, &arg);
             printf("> Configuring monitor... \n");
             setupMonitor(TRUE);
             
             if (nargs != 2) {
                 arg = 1; 
             }
             
             for (i=0; i<arg; i++) {
                 printf("> Grapping frame... \n");
                 grapPcMonitorFrame(jpgImage);
             }

             printf("> Configuring monitor... \n");
             setupMonitor(FALSE);
             break;

         case 'h':
            printf("> Half press... \n");
            halfShutter();
            break; 
            
         case 'i':
            nargs = sscanf(input, "%c %d", &com, &arg);
            printf("> Configuring ISO... \n");
            switch (arg) {
               case 1:
                 setupIso(DATA_IS0_AUTO);
                 break;
               case 2:
                 setupIso(DATA_IS0_100);
                 break;
               case 3:
                 setupIso(DATA_IS0_200);
                 break;                 
               case 4:
                 setupIso(DATA_IS0_400);
                 break;
               case 5:
                 setupIso(DATA_IS0_800);
                 break;
               case 6:
                 setupIso(DATA_IS0_1600);
                 break;
               default:
                 printf("> Unknown ISO setting. \n");
            }
            break; 
            
         case 'm':
            nargs = sscanf(input, "%c %d %s", &com, &arg, &name);
            printf("> Recording... \n");
            if (nargs == 3) 
               movie(strcat(name, ".MOV"), arg);
            else if (nargs == 2) 
               movie("CIMG001.MOV", arg);
            else 
               movie("CIMG001.MOV", -1);
            break; 
            
         case 'q':
            exitCamera();
            printf("> Bye! \n");
            terminateCamera();
            return 0;    
            
         case 's':
            nargs = sscanf(input, "%c %s %s %d", &com, &name, &tname, &arg);
            printf("> Taking picture... \n");
            switch (nargs) {
                case 1: 
                    shutter("CIMG001.jpg", "CIMG001_thumb.jpg", -1);
                    break; 
                case 2: 
                    shutter(strcat(name,".jpg"), strcat(name, "_thumb.jpg"), -1);     
                    break; 
                case 3:
                    shutter(strcat(name,".jpg"), strcat(tname,".jpg"), -1);
                    break; 
                case 4:
                    shutter(strcat(name,".jpg"), strcat(tname,".jpg"), arg);
                    break; 
            }
            break;

         case 'v':
            nargs = sscanf(input, "%c %s %d", &com, &name, &arg);
            printf("> Focusing... \n");

            switch (nargs) {
                case 1:
                    focus(TRUE, TRUE);
                    break;
                case 2:
                    if (strcmp(name, "in") == 0)
                        focus(TRUE, TRUE);
                    else if (strcmp(name, "out") == 0)
                        focus(FALSE, TRUE);
                    else
                        printf("> Unrecognized parameter: %s!\n", name);
                    break;
                case 3:
                    if (strcmp(name, "in") == 0)
                        for (i=0; i<arg; i++)
                            focus(TRUE, FALSE);
                    else if (strcmp(name, "out") == 0)
                        for (i=0; i<arg; i++)
                            focus(FALSE, FALSE);
                    else
                        printf("> Unrecognized parameter: %s!\n", name);
                    break;
            }
            break;

         case 'x':
            nargs = sscanf(input, "%c %x %x", &com, &addr, &val);
            if (nargs == 3) {
               printf("> Setting property 0x%X to 0x%X... \n", addr, val);
               exf1Cmd(CMD_WRITE, addr, val);
            }
            else
               printf("> Three arguments required... \n");
            break;

         case 'z':
            nargs = sscanf(input, "%c %s %d", &com, &name, &arg);
            printf("> Zooming... \n");

            switch (nargs) {
                case 1:
                    zoom(TRUE, TRUE);
                    break;
                case 2:
                    if (strcmp(name, "in") == 0)
                        zoom(TRUE, TRUE);
                    else if (strcmp(name, "out") == 0)
                        zoom(FALSE, TRUE);
                    else
                        printf("> Unrecognized parameter: %s!\n", name);
                    break;
                case 3:
                    if (strcmp(name, "in") == 0)
                        for (i=0; i<arg; i++)
                            zoom(TRUE, FALSE);
                    else if (strcmp(name, "out") == 0)
                        for (i=0; i<arg; i++)
                            zoom(FALSE, FALSE);
                    else
                        printf("> Unrecognized parameter: %s!\n", name);
                    break;
            }
            break;

         default: 
            printf("> Unrecognized command: %s \n", input);   
            break; 
            
      }
   }
    
   return 0;
}

