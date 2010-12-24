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
   char i, input[64], com, name[32], tname[32];
   int arg, nargs, addr, val;
 
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
   printf(" Hint: h activates half-press.\n");
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
   printf(" Hint: m [x [y]] records a x second long movie called y.\n");
   printf(" Hint: s [x [y]] activates shutter and stores a picture called x\n");
   printf("       and a thumbnail called y.\n");
   printf("\n");
   printf("\n");
   printf("> Initializing camera... \n");
   fflush(stdout);
		   
   if (!init_camera())
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
                 setup_aperture(DATA_APERTURE_F2_7);
                 break;
               case 2:
                 setup_aperture(DATA_APERTURE_F3_0);
                 break;
               case 3:
                 setup_aperture(DATA_APERTURE_F3_3);
                 break;
               case 4:
                 setup_aperture(DATA_APERTURE_F3_8);
                 break;
               case 5:
                 setup_aperture(DATA_APERTURE_F4_2);
                 break;
               case 6:
                 setup_aperture(DATA_APERTURE_F4_7);
                 break;
               case 7:
                 setup_aperture(DATA_APERTURE_F5_3);
                 break;
               case 8:
                 setup_aperture(DATA_APERTURE_F6_0);
                 break;
               case 9:
                 setup_aperture(DATA_APERTURE_F6_7);
                 break;
               case 10:
                 setup_aperture(DATA_APERTURE_F7_5);
                 break;
               default:
                 printf("> Unknown aperture setting. \n");
            }
            break;

         case 'c': 
            nargs = sscanf(input, "%c %d", &com, &arg);
            printf("> Configuring mode / movie mode... \n");
            switch (arg) {
                case 6:
                    setup_movie_hd(FALSE);
                    break; 
                case 7:
                    setup_movie_hd(TRUE);
                    break; 
                case 8:
                    setup_movie_hs(FALSE);
                    break; 
                case 9:
                    setup_movie_hs(TRUE);
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
                 setup_exposure(DATA_EXPOSURE_M);
                 break;
               case 2:
                 setup_exposure(DATA_EXPOSURE_AUTO);
                 break;
               case 3:
                 setup_exposure(DATA_EXPOSURE_A);
                 break;
               case 4:
                 setup_exposure(DATA_EXPOSURE_S);
                 break;
               default:
                 printf("> Unknown exposure setting. \n");
            }
            break; 

         case 'h':
            printf("> Half press... \n");
            half_shutter(); 
            break; 
            
         case 'i':
            nargs = sscanf(input, "%c %d", &com, &arg);
            printf("> Configuring ISO... \n");
            switch (arg) {
               case 1:
                 setup_iso(DATA_IS0_AUTO);
                 break;
               case 2:
                 setup_iso(DATA_IS0_100);
                 break;
               case 3:
                 setup_iso(DATA_IS0_200);
                 break;                 
               case 4:
                 setup_iso(DATA_IS0_400);
                 break;
               case 5:
                 setup_iso(DATA_IS0_800);
                 break;
               case 6:
                 setup_iso(DATA_IS0_1600);
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
            exit_camera();
            printf("> Bye! \n");
            terminate_camera();
            return 0;    
            
         case 's':
            nargs = sscanf(input, "%c %s %s", &com, &name, &tname);
            printf("> Taking picture... \n");   
            if (nargs == 3) 
               shutter(strcat(name,".jpg"), strcat(tname,".jpg"));
            else if (nargs == 2) 
               shutter(strcat(name,".jpg"), strcat(name, "_thumb.jpg"));
            else
               shutter("CIMG001.jpg", "CIMG001_thumb.jpg");
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

         default: 
            printf("> Unrecognized command: %s \n", input);   
            break; 
            
      }
   }
    
   return 0;
}

