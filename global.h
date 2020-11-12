#ifndef H_INIT
#define H_INIT
  #include "init.h"
#endif

float *hanning_G;
float *hanning_D;
float *pond;
FILTER * filter_bank;

// variables resample
float * input_2;
float * sig_upsample;
float * sig_filter;
float * sig_dnsample_init;
float * sig_resample_init;
float * sig_dnsample;
float * sig_resample;

float * corr;

float * sig_filter_input_2;
float * ci;

int taille_input_2;
int taille_upsample;
int taille_dnsample_init;
int taille_resample_init;
int taille_dnsample;
int taille_resample;
int taille_sig_filter_input_2;
int bankID;
