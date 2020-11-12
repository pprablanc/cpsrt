#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#ifndef H_INIT
#define H_INIT
  #include "init.h"
#endif

extern float *hanning_G;
extern float *hanning_D;
extern float *pond;
extern FILTER * filter_bank;

extern float * corr;

extern float * input_2;
extern float * sig_upsample;
extern float * sig_filter;
extern float * sig_dnsample;
extern float * sig_resample;
extern float * sig_dnsample_init;
extern float * sig_resample_init;
extern float * sig_filter_input_2;
extern float * ci;

extern int taille_input_2;
extern int taille_upsample;
extern int taille_dnsample_init;
extern int taille_resample_init;
extern int taille_dnsample;
extern int taille_resample;
extern int taille_sig_filter_input_2;
extern int bankID;



void resample( 	float * input,
 				int taille_input, 
 				float * cond_init,
 				float * output, 
 				int taille_output, 
 				int resample_coef,
 				int flag);

void filtre_resample(	float * sig_input, 
						float * sig_output, 
						float * cond_init,
						int taille_ci);
