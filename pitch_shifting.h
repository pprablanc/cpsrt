#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#ifndef H_INIT
#define H_INIT
  #include "init.h"
#endif
#include "resample.h"

#define SAMPLE_RATE             16000
#define FRAMES_PER_INPUT_BUFFER 16 //on utilise un buffer de 16 car il n'y a pas de clics, on peut utiliser jusqu'à 32 au delà, le son présente des clics.
#define NUM_CHANNELS            1
#define TAILLE_PETITE_TRAME     FRAMES_PER_INPUT_BUFFER*NUM_CHANNELS  
#define TAILLE_TRAME            256
#define CINQ_TRAME              5*16    // 4*16*PETITES_TRAMES = 4*256 = taille_buffer pour le time stretching
#define QUATRE_TRAME            4*16
#define UNE_TRAME               16  

// #define SEUIL_INTENSITE         1061
#define SEUIL_INTENSITE         -45
#define TAILLE_POND             120 
#define TAILLE_XCORR			2*TAILLE_POND+1
#define TAILLE_WIN					128


// float output_buffer[256] : buffer de sortie, il est fixe et de même taille que le buffer d'entrée
// float * trame_modifie_courante : trames modifiées au cours du traitement
typedef struct data_output_process_t{
	float output_buffer[256]; 
	float * trame_modifie_courante;	
} DATA_OUTPUT_PROCESS;

// float * input_buffer : buffer d'entrée, il est rempli par paquets de 16 mini-buffers
// float * trame_modifie_precedente : trame modifiée du traitement précédent
typedef struct data_input_process_t{
	float * input_buffer;
	float * trame_modifie_precedente;
} DATA_INPUT_PROCESS;

typedef struct max_t{
	float value;
	int indice;
} MAX;


void pitch_shifting(	float pitchShiftCoef, 
						DATA_INPUT_PROCESS * data_in,
						DATA_OUTPUT_PROCESS * data_out,
						int flag); // indicateur de début de traitement. si flag = 1, on effectue les traitements sans conditions initiales

// xcorr calcule l'intercorrélation entre x et y. 
// RESTRICTION : Il faut que x soit plus grand que y
void xcorr(float * x,int taille_x, float * y, int taille_y);
MAX get_max(float * x, int taille_x);