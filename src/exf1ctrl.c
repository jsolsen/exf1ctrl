//
// ExF1Ctrl ver. 0.1. 
//
// This program can be used to interface the Casio EX-F1 over USB. 
// Firmware rev. 1.10 is required. 
//
// Compile with: gcc -o ExF1Ctrl bulk.c -llibusb
//
// Written by Jens Skovgaard Olsen (info@feischmeckerfoosball.com)
// 

#include <usb.h>
#include <stdio.h>

#define MY_VID 0x07CF
#define MY_PID 0x1023

#define EP_IN 0x81
#define EP_OUT 0x02
#define EP_INT 0x83

#define BUF_SIZE 512
#define IMG_BUF_SIZE 65536
#define TIME_OUT 3500

usb_dev_handle *dev = NULL;
char tmp[BUF_SIZE];
char img[IMG_BUF_SIZE];

usb_dev_handle *open_dev(void);
int bulk_write(char tx_buffer[], char tx_size, char no_reads, int line_no); 
int string_match(char s1[], char s2[], int length); 

int init_camera(void); 
void half_shutter(void); 
void shutter(char filename[], char thumbnail[]); 
void terminate_camera(void); 
void setup_prerecord_movie_hs(void); 
void movie(char filename[], int delay); 

usb_dev_handle *open_dev(void)
{
  struct usb_bus *bus;
  struct usb_device *dev;

  for(bus = usb_get_busses(); bus; bus = bus->next) 
    {
      for(dev = bus->devices; dev; dev = dev->next) 
        {
          if(dev->descriptor.idVendor == MY_VID
             && dev->descriptor.idProduct == MY_PID)
            {
              return usb_open(dev);
            }
        }
    }
  return NULL;
}

int bulk_write(char tx_buffer[], char tx_size, char no_reads, int line_no)
{
  char i;
  int bytes_read = 0;
  if (usb_bulk_write(dev, EP_OUT, tx_buffer, tx_size, TIME_OUT) != tx_size)
      printf("error: bulk write failed: %s @ line %d. \n", tx_buffer, line_no);

  for (i=0; i<no_reads; i++) 
  {
     bytes_read = usb_bulk_read(dev, EP_IN, tmp + bytes_read, BUF_SIZE, TIME_OUT); 
     if (bytes_read < 0)
        printf("error: bulk read %d failed @ line %d. \n", i, line_no);
  }
  return bytes_read; 
}

int string_match(char s1[], char s2[], int length)
{
  int i, matches = 0;
   
  for (i=0; s1[i] == s2[i] && i<length; i++)
      matches++; 
  
  return matches-length; 
}

int init_camera(void)
{
  usb_init(); /* initialize the library */
  usb_find_busses(); /* find all busses */
  usb_find_devices(); /* find all connected devices */

  if(!(dev = open_dev()))
    {
      printf(" Error: camera not found!\n");
      return 0;
    }

  if(usb_set_configuration(dev, 1) < 0)
    {
      printf(" Error: setting config 1. \n");
      usb_close(dev);
      return 0;
    }

  if(usb_claim_interface(dev, 0) < 0)
    {
      printf(" Error: claiming interface 0 failed. \n");
      usb_close(dev);
      return 0;
    }
  
  if (usb_control_msg(dev, 0x21, 0x66, 0x00, 0x00, NULL, 0, TIME_OUT) < 0)
      printf("error: cmd write 1 failed\n");
 
  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      //printf("error: interrupt read 1 failed\n");
      
  if (usb_clear_halt(dev, EP_IN) < 0)
      printf("error: halt clear failed.\n"); 

  if (usb_clear_halt(dev, EP_OUT) < 0) 
      printf("error: halt clear failed.\n"); 
  
  if (usb_control_msg(dev, 0xA1, 0x67, 0x00, 0x00, &tmp[0], 0x0400, TIME_OUT) < 0)
      printf("error: cmd write 2 failed\n");      

  bulk_write((char[]){0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x10, 0xFF, 0xFF, 0xFF, 0xFF}, 12, 2, __LINE__); 
  bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x15, 0x06, 0x76, 0x19}, 16, 1, __LINE__); 
  bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x14, 0x10, 0x01, 0x00, 0x00, 0x00, 0x02, 0x50, 0x00, 0x00}, 16, 2, __LINE__); 
  bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x16, 0x10, 0x02, 0x00, 0x00, 0x00, 0x02, 0x50, 0x00, 0x00}, 16, 0, __LINE__); 
  bulk_write((char[]){0x0E, 0x00, 0x00, 0x00, 0x02, 0x00, 0x16, 0x10, 0x02, 0x00, 0x00, 0x00, 0x01, 0x80}, 14, 1, __LINE__); 

  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      printf("error: interrupt read 2 failed\n");

  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      printf("error: interrupt read 3 failed\n");
        
//  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
//      printf("error: interrupt read 4 failed\n");
        
  bulk_write((char[]){0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x10, 0x03, 0x00, 0x00, 0x00}, 12, 2, __LINE__); 
  bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x90, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 16, 1, __LINE__); 
  bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x14, 0x10, 0x05, 0x00, 0x00, 0x00, 0x0A, 0x50, 0x00, 0x00}, 16, 2, __LINE__); 
  bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x14, 0x10, 0x06, 0x00, 0x00, 0x00, 0x0E, 0x50, 0x00, 0x00}, 16, 2, __LINE__); 
  bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x14, 0x10, 0x07, 0x00, 0x00, 0x00, 0x0D, 0xD0, 0x00, 0x00}, 16, 2, __LINE__); 
  bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x14, 0x10, 0x08, 0x00, 0x00, 0x00, 0x02, 0xD0, 0x00, 0x00}, 16, 2, __LINE__); 
  
  return 1; 
}

void half_shutter(void)
{
  // half shutter
  bulk_write((char[]){0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x29, 0x90, 0x0F, 0x00, 0x00, 0x00}, 12, 0, __LINE__);  

  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      printf("error: interrupt read 5 failed\n");
  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      printf("error: interrupt read 6 failed\n");   
}

void shutter(char filename[], char thumbnail[])
{
   
  int bytes_read = -1, transfer_jpg = 1; 
  FILE * pFile;
   
  bulk_write((char[]){0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x24, 0x90, 0x09, 0x00, 0x00, 0x00}, 12, 0, __LINE__);  

  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      printf("error: interrupt read 7 failed\n");
  if (usb_interrupt_read(dev, EP_INT, tmp, 16, TIME_OUT) < 0)
      printf("error: interrupt read 8 failed\n");   
 
  bulk_write((char[]){0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x27, 0x90, 0x0A, 0x00, 0x00, 0x00}, 12, 2, __LINE__);  
  bulk_write((char[]){0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0C, 0x90, 0x0B, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF}, 20, 2, __LINE__);  
  
  // Get fullsize image. 
  bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x25, 0x90, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}, 16, 1, __LINE__);  

  pFile = fopen (filename, "wb");  
  while (transfer_jpg) {
     
     bytes_read = usb_bulk_read(dev, EP_IN, img, IMG_BUF_SIZE, TIME_OUT); 
  
     if (bytes_read > 0) {
     
         if (string_match((char[]){0x25, 0x90, 0x0C, 0x00, 0x00, 0x00}, img+6, 6) == 0) {
            printf("> Transferring %d bytes... \n", img[0] + (img[1] << 8) + (img[2] << 16) + (img[3] << 24)); 
            fwrite(img+12, 1, bytes_read-12, pFile);      
         }
         else if (string_match((char[]){0x18, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x20, 0x0C, 0x00, 0x00, 0x00}, img, 12) == 0) {
            printf("> %s saved to disk. \n", filename); 
            transfer_jpg = 0;  
         }
         else
            fwrite(img, 1, bytes_read, pFile);    
     }
  }
  fclose (pFile);  
  
  // Get thumbnail image. 
  bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x26, 0x90, 0x0D, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}, 16, 0, __LINE__);  
  transfer_jpg = 1; 
  
  pFile = fopen (thumbnail, "wb");  
  while (transfer_jpg) {
     
     bytes_read = usb_bulk_read(dev, EP_IN, img, BUF_SIZE, TIME_OUT); 
  
     if (bytes_read > 0) {
     
         if (string_match((char[]){0x26, 0x90, 0x0D, 0x00, 0x00, 0x00}, img+6, 6) == 0) {
            printf("> Transferring %d bytes... \n", img[0] + (img[1] << 8) + (img[2] << 16) + (img[3] << 24)); 
            fwrite(img+12, 1, bytes_read-12, pFile);      
         }
         else if (string_match((char[]){0x18, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x20, 0x0D, 0x00, 0x00, 0x00}, img, 12) == 0) {
            printf("> %s saved to disk. \n", thumbnail); 
            transfer_jpg = 0;  
         }
         else
            fwrite(img, 1, bytes_read, pFile);    
     }
  }
  fclose (pFile);    
  
  bulk_write((char[]){0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x28, 0x90, 0x0E, 0x00, 0x00, 0x00}, 12, 1, __LINE__); 

}

void setup_prerecord_movie_hs(void)
{
   char bytes[2]; 
   
   bulk_write((char[]){0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x90, 0x25, 0x00, 0x00, 0x00}, 12, 1, __LINE__); 
   bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x16, 0x10, 0x26, 0x00, 0x00, 0x00, 0x0B, 0xD0, 0x00, 0x00}, 16, 0, __LINE__); 
   bulk_write((char[]){0x0E, 0x00, 0x00, 0x00, 0x02, 0x00, 0x16, 0x10, 0x26, 0x00, 0x00, 0x00, 0x02, 0x00}, 14, 1, __LINE__); 
   bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x41, 0x90, 0x27, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}, 16, 0, __LINE__); 
   
   if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x81, bytes, 0x2, TIME_OUT) < 0)
      printf("error: cmd write 1 failed\n");

   if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x2, bytes, 0x2, TIME_OUT) < 0)
      printf("error: cmd write 2 failed\n");      

   sleep(1); 
      
   if (usb_bulk_read(dev, EP_IN, tmp, BUF_SIZE, TIME_OUT) < 0)
      printf("error: bulk read 1 failed. \n");   

   bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x15, 0x10, 0x28, 0x00, 0x00, 0x00, 0x0D, 0xD0, 0x00, 0x00}, 16, 2, __LINE__);    
   bulk_write((char[]){0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x42, 0x90, 0x29, 0x00, 0x00, 0x00}, 12, 1, __LINE__);    
   bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x16, 0x10, 0x2A, 0x00, 0x00, 0x00, 0x0A, 0x50, 0x00, 0x00}, 16, 0, __LINE__);    
   bulk_write((char[]){0x0E, 0x00, 0x00, 0x00, 0x02, 0x00, 0x16, 0x10, 0x2A, 0x00, 0x00, 0x00, 0x02, 0x00}, 14, 1, __LINE__);      
   bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x41, 0x90, 0x2B, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}, 16, 0, __LINE__); 

   if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x81, bytes, 0x2, TIME_OUT) < 0)
      printf("error: cmd write 3 failed\n");

   if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x2, bytes, 0x2, TIME_OUT) < 0)
      printf("error: cmd write 4 failed\n");      

   sleep(1);       
   if (usb_bulk_read(dev, EP_IN, tmp, BUF_SIZE, TIME_OUT) < 0)
      printf("error: bulk read 2 failed. \n");   

}

void movie(char filename[], int delay)
{
   char bytes[2]; 
   int bytes_read = -1, transfer_mov = 1; 
   FILE * pFile;
   
   bulk_write((char[]){0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x43, 0x90, 0x2C, 0x00, 0x00, 0x00}, 12, 1, __LINE__); 
   
   if (delay >= 0) 
      sleep(delay); 
   else 
      printf("> Press enter to stop recording... "), getchar(); 

   sleep(2); 
   
   bulk_write((char[]){0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x44, 0x90, 0x2D, 0x00, 0x00, 0x00}, 12, 1, __LINE__); 
   bulk_write((char[]){0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x45, 0x90, 0x2E, 0x00, 0x00, 0x00}, 12, 2, __LINE__); 
   bulk_write((char[]){0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0C, 0x90, 0x2F, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF}, 20, 2, __LINE__); 
   bulk_write((char[]){0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x25, 0x90, 0x30, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}, 16, 0, __LINE__); 

   pFile = fopen (filename, "wb");  
   while (transfer_mov) {
     
     bytes_read = usb_bulk_read(dev, EP_IN, img, IMG_BUF_SIZE, TIME_OUT); 
   
     if (bytes_read > 0) {
     
         if (string_match((char[]){0x25, 0x90, 0x30, 0x00, 0x00, 0x00}, img+6, 6) == 0) {
            printf("> Transferring %d bytes... \n", img[0] + (img[1] << 8) + (img[2] << 16) + (img[3] << 24)); 
            fflush(stdout); 
            fwrite(img+12, 1, bytes_read-12, pFile);      
         }
         else if (string_match((char[]){0x18, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x20, 0x30, 0x00, 0x00, 0x00}, img, 12) == 0) {
            printf("> %s saved to disk. \n", filename); 
            transfer_mov = 0;  
         }
         else
            fwrite(img, 1, bytes_read, pFile);    
     }
   }
   fclose (pFile);  
   
   bulk_write((char[]){0x0C, 0x00, 0x00, 0x00, 0x01, 0x00, 0x46, 0x90, 0x31, 0x00, 0x00, 0x00}, 12, 1, __LINE__); 
   
   if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x81, bytes, 0x2, TIME_OUT) < 0)
      printf("error: cmd write 1 failed\n");
   
   if (usb_control_msg(dev, 0x82, 0x00, 0x00, 0x2, bytes, 0x2, TIME_OUT) < 0)
      printf("error: cmd write 2 failed\n");   
}

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
	printf(" *  Firmware rev. 1.10 is required and the camera must be put in    *\n");
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

void terminate_camera(void)
{
  usb_release_interface(dev, 0);
  usb_close(dev);   
}
