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
   int arg, nargs; 
 
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
   printf(" Hint: i activates interval shutter.\n");
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
         case 'c': 
            nargs = sscanf(input, "%c %d", &com, &arg);
            printf("> Configuring mode / movie mode... \n");
            switch (arg) {
               case 9: 
                 setup_prerecord_movie_hs();        
                 break; 
               default: 
                 printf("> This mode is yet to be supported. \n");
            }
            break; 
         
         case 'h':
            printf("> Half press... \n");
            half_shutter(); 
            break; 
            
         case 'i':
            printf("> Interval shutter... \n");
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
            
         default: 
            printf("> Unrecognized command: %s \n", input);   
            break; 
            
      }
   }
    
   return 0;
}

