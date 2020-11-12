#include "pitch_shifting.h"
#include "resample.h"

void pitch_shifting(  float pitchShiftCoef,   
                      DATA_INPUT_PROCESS * data_in, //structure de données entrantes et données sauvegardées du passage précédent
                      DATA_OUTPUT_PROCESS * data_out, //structure de données sortantes                      
                      int flag) {

  unsigned short long_trame;
  float intensite, intensiteLog;
  int i;
  int k_pos;
  MAX I;
  k_pos =0;
  int pointeur_s, pointeur_a;
  pointeur_s = 256*pitchShiftCoef;
  pointeur_a = 256;
  long_trame = 256*pitchShiftCoef + 2*TAILLE_POND + 255;


  intensite 	= FLT_MIN;
  for(i = 0; i<long_trame ; i++){
  	intensite += pow(data_in->input_buffer[i],2);
  }
  intensiteLog = 10*log10(intensite);
  if(intensiteLog > SEUIL_INTENSITE ){
    if(flag == 0){
	  	xcorr(data_in->trame_modifie_precedente+pointeur_s-120, 256+TAILLE_XCORR, data_in->input_buffer+256, 256);
      for( i=0 ; i<TAILLE_XCORR ; i++){
        corr[i] = pond[i]*corr[i];
      }      
      I = get_max(corr,TAILLE_XCORR);
      k_pos = I.indice - TAILLE_POND;
      // k_pos =0;
      // printf("k_pos = %d\n",k_pos);
    }
	}else{
    k_pos =0;
          // printf("k_pos = %d\n",k_pos);
  }

// pitch_shifting
  if(flag ==1){
    // time_stretching

    for( i=0 ; i<CINQ_TRAME*TAILLE_PETITE_TRAME ; i++){
      data_out->trame_modifie_courante[i] = data_in->input_buffer[i];
    }
    
    xcorr(data_in->input_buffer+pointeur_s-120, 256+TAILLE_XCORR, data_in->input_buffer+256, 256);
    for( i=0 ; i<TAILLE_XCORR ; i++){
      corr[i] = pond[i]*corr[i];
    }
    I = get_max(corr,TAILLE_XCORR);
    k_pos = I.indice - TAILLE_POND;
    printf("premier k_pos = %d\n",k_pos);

    
    for( i=TAILLE_WIN ; i<CINQ_TRAME*TAILLE_PETITE_TRAME-pointeur_s+k_pos; i++){
      data_out->trame_modifie_courante[pointeur_s-k_pos+i] = data_in->input_buffer[i+pointeur_a];
    }
    for( i=0 ; i<TAILLE_WIN ; i++){ //3
      data_out->trame_modifie_courante[i+pointeur_s-k_pos] = 
            data_in->input_buffer[i+pointeur_a]*hanning_G[i] + 
            data_out->trame_modifie_courante[pointeur_s-k_pos+i]*hanning_D[i];
    }

    // resample
    resample(   data_out->trame_modifie_courante,
                pointeur_s + filter_bank[bankID].length, 
                NULL, //On n'utilise pas de conditions initiale la première fois
                data_out->output_buffer, 
                256, 
                pitchShiftCoef,
                flag);


  }else{
    // time_stretching

    for( i=0 ; i<CINQ_TRAME*TAILLE_PETITE_TRAME ; i++){
      data_out->trame_modifie_courante[i] = data_in->trame_modifie_precedente[i];

    }

    for( i=TAILLE_WIN ; i<CINQ_TRAME*TAILLE_PETITE_TRAME-pointeur_s+k_pos; i++){
      data_out->trame_modifie_courante[pointeur_s-k_pos+i] = data_in->input_buffer[i+pointeur_a];   
    }


    for( i=0 ; i<TAILLE_WIN ; i++){ //3
      data_out->trame_modifie_courante[pointeur_s+i-k_pos] = 
            data_in->input_buffer[i+pointeur_a]*hanning_G[i] + 
            data_in->trame_modifie_precedente[pointeur_s-k_pos+i]*hanning_D[i];
    }


    // resample
    resample(   data_out->trame_modifie_courante,
                pointeur_s + 2*filter_bank[bankID].length, 
                data_in->trame_modifie_precedente+pointeur_s-filter_bank[bankID].length,
                data_out->output_buffer, 
                256, 
                pitchShiftCoef,
                flag);

  }
    //mise à jour des sauvegardes de trames
    for(i=0 ; i<CINQ_TRAME*TAILLE_PETITE_TRAME ; i++){
      data_in->trame_modifie_precedente[i] = data_out->trame_modifie_courante[i+pointeur_s];
    }
}


void xcorr(float * x,int taille_x, float * y, int taille_y){
    int i,j;
    int taille_corr;


  taille_corr = taille_x-taille_y;
    for( i=0 ; i<taille_corr ; i++){
      corr[i] = 0;
    }

    for( i=taille_y ; i<taille_x ; i++){
      for( j=0 ; j<=taille_y ; j++){
        corr[i-taille_y] += x[taille_x-1-i+j]*y[j];
      }
    }
}


MAX get_max(float * x, int taille_x){
  int i;
  MAX max;
  max.indice =0;
  max.value = 0;
  for( i=0 ; i<taille_x ; i++){   
    if(x[i]>=0){
      if(x[i]>=max.value){
        max.value = x[i];
        max.indice = i;
      }
    }else{
      if(-x[i]>=max.value){
        max.value = -x[i];
        max.indice = i;
      }
    }
  }
  return max;
}
