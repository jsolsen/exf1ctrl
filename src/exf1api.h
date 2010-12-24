/* 
 * File:   libexf1.h
 * Author: Jens Skovgaard Olsen
 *
 * Created on 30. november 2010, 17:33
 */

#ifndef EXF1API_H
#define	EXF1API_H

#include "libexf1.h"

void start_config(char enableStillImage, char enablePreRecord);
void stop_config();

int init_camera(void);
void half_shutter(void);
void shutter(char filename[], char thumbnail[]);
void terminate_camera(void);

void setup_movie_hs(char enablePreRecord); 
void setup_movie_hd(char enablePreRecord); 

void movie(char filename[], int delay);
void setup_pc_monitor(void);
void setup_iso(WORD iso);
void setup_aperture(WORD aperture);
void setup_exposure(WORD exposure); 

int grap_pc_monitor_frame(char *jpg_img);
void exit_camera(void);

#endif	/* LIBEXF1_H */

