// External libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "../include/portaudio.h"

// Internal libraries
#ifndef H_INIT
#define H_INIT
  #include "../include/init.h"
#endif
#include "../include/global.h"
#include "../include/pitch_shifting.h"


/* @todo Underflow and overflow is disabled until we fix priming of blocking write. */
#define CHECK_OVERFLOW  (0)
#define CHECK_UNDERFLOW  (0)


/* Select sample format. */
#define PA_SAMPLE_TYPE  paFloat32
#define SAMPLE_SIZE (4)
#define SAMPLE_SILENCE  (0.0f)
#define CLEAR(a) memset( (a), 0, FRAMES_PER_INPUT_BUFFER * NUM_CHANNELS * SAMPLE_SIZE )
#define PRINTF_S_FORMAT "%.8f"

#define RESOURCE_PATH "resources/"

/*******************************************************************/


int main(void);
int main(void){ 
    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream = NULL;
    PaError err;
    float *sampleBlock; //variable temporaire acceuillant les données du buffer d'entrée
    int i,j;
    int numBytes; //nombre d'octets requis pour allouer sampleBlock et input_buffer
    DATA_OUTPUT_PROCESS * data_output_process; //structure contenant les signaux sortants de pitch_shifting
    DATA_INPUT_PROCESS * data_input_process; //structure contenant les signaux entrant dans pitch_shifting
    float pitchShiftCoef; //coefficient de changement de hauteur de la voix
    int cpt_buffer; //compte le nombre de buffer de 16 déjà remplis
    int flag_debut_process;


    
    printf("Starting pitch shifting program ...\n"); fflush(stdout);

    /*--------------- INITIALISATION DES VARIABLES ---------------------*/ 

    numBytes    = FRAMES_PER_INPUT_BUFFER * NUM_CHANNELS * SAMPLE_SIZE ;
    sampleBlock = (float *) malloc( numBytes );
    hanning_G       = (float *) malloc(128*sizeof(float));  //partie gauche de la fenêtre pour le time stretching
    hanning_D       = (float *) malloc(128*sizeof(float));  //partie droite de la fenêtre pour le time stretching
    pond        = (float *) malloc(241*sizeof(float));  //fenêtre de pondération de la corrélation pour le time stretching
    data_output_process = (DATA_OUTPUT_PROCESS *) malloc(sizeof(DATA_OUTPUT_PROCESS));
    data_output_process->trame_modifie_courante = (float *) malloc(CINQ_TRAME*UNE_TRAME*sizeof(float));
    data_input_process = (DATA_INPUT_PROCESS *) malloc(sizeof(DATA_INPUT_PROCESS));
    data_input_process->input_buffer = (float *) malloc( CINQ_TRAME*numBytes);
    data_input_process->trame_modifie_precedente = (float *) malloc(CINQ_TRAME*UNE_TRAME*sizeof(float));

    if(load_window_data((char*)RESOURCE_PATH"hanning_G", hanning_G) == 0){
      fprintf(stdout,"initialisation de la variable hanning_G : OK\n");
    }else{
      fprintf(stderr,"initialisation de la variable hanning_G : erreur\n");
    }
    if(load_window_data((char*)RESOURCE_PATH"hanning_D", hanning_D) == 0){
      fprintf(stdout,"initialisation de la variable hanning_D : OK\n");
    }else{
      fprintf(stderr,"initialisation de la variable hanning_D : erreur\n");
    }
    if(load_window_data((char*)RESOURCE_PATH"pond", pond) == 0){
      fprintf(stdout,"initialisation de la variable pond : OK\n");
    }else{
      fprintf(stderr,"initialisation de la variable pond : erreur\n");
    }

    int nbrFiles;
    nbrFiles = get_nbr_files((char*)RESOURCE_PATH"banque_filtre/");
    filter_bank = (FILTER *) malloc(nbrFiles*sizeof(FILTER));
    if(load_filter_bank((char*)RESOURCE_PATH"banque_filtre/", filter_bank) == 0){
      fprintf(stdout,"initialisation de la variable filter_bank : OK\n");
    }else{
      fprintf(stderr,"initialisation de la variable filter_bank : erreur\n");
    }    

// Vérifications
    // for(i =0 ; i<128 ; i++){
    //   printf("hanning_G[%d] = %.9f\n",i,hanning_G[i]);
    // }
    // for(i =0 ; i<128 ; i++){
    //   printf("hanning_D[%d] = %.9f\n",i,hanning_D[i]);
    // }

    // for(i = 0; i < nbrFiles ; i++){
    //   printf("filter_bank[%d].pitchShiftCoef : %f\n",i,filter_bank[i].pitchShiftCoef);
    //   printf("filter_bank[%d].p : %d\n",i,filter_bank[i].p);
    //   printf("filter_bank[%d].q : %d\n",i,filter_bank[i].q);
    //   printf("filter_bank[%d].length : %d\n",i,filter_bank[i].length);
    //   for(j = 0; j<filter_bank[i].length ; j++){
    //     printf("filter_bank[%d].data[%d]pitchShiftCoef : %.9f\n",i,j,filter_bank[i].data[j]);
    //   }
    // }

    pitchShiftCoef = 1.25; //banque [5]
    printf("pitchShiftCoef: %f\n",pitchShiftCoef);
    bankID = -1;
    for( i=0; i<nbrFiles ; i++){
      printf("Filter bank pitch coefficient: %f", filter_bank[i].pitchShiftCoef);
      if(pitchShiftCoef == filter_bank[i].pitchShiftCoef){
        bankID = i;
        break;
      }
    }
    if(bankID == -1){
      fprintf(stderr,"Le coefficient :  %d de changement de pitch n'est pas dans la liste\n",bankID);
      return -1;
    }

    // initialisation des variables resample
    taille_input_2 = 256*pitchShiftCoef+2*filter_bank[bankID].length;
    input_2 =(float *) malloc(taille_input_2*sizeof(float));
    taille_upsample = (int)(filter_bank[bankID].q*(pitchShiftCoef*256+2*filter_bank[bankID].length));
    sig_upsample = (float *) malloc(taille_upsample*sizeof(float));
    sig_filter = (float *) malloc(taille_upsample*sizeof(float));
    taille_dnsample_init = (int)( (taille_upsample + 2*(filter_bank[bankID].p-filter_bank[bankID].q))/filter_bank[bankID].p);
    sig_dnsample_init = (float *) malloc(taille_dnsample_init*sizeof(float));
    taille_resample_init = (int)( (taille_upsample + 2*(filter_bank[bankID].p-filter_bank[bankID].q))/filter_bank[bankID].p - 3);
    sig_resample_init = (float *) malloc(taille_resample_init*sizeof(float));

    taille_dnsample = (1 + (taille_upsample - filter_bank[bankID].q*(filter_bank[bankID].length+1))/filter_bank[bankID].p);
    sig_dnsample = (float *) malloc(taille_dnsample*sizeof(float));
    taille_resample = (1 + (taille_upsample - filter_bank[bankID].q*(filter_bank[bankID].length+1))/filter_bank[bankID].p - 3);
    sig_resample = (float *) malloc(taille_resample*sizeof(float));
    
    taille_sig_filter_input_2 = taille_upsample+filter_bank[bankID].length;
    sig_filter_input_2 = (float * ) malloc(taille_sig_filter_input_2*sizeof(float));
    ci = (float *) malloc(filter_bank[bankID].length*sizeof(float));
    for( i=0 ; i<filter_bank[bankID].length ; i++)  ci[i] =0;
    corr = (float *) malloc((256+2*120)*sizeof(float));


    fprintf(stdout,"\n\n");
    /*--------------- INITIALISATION DES FLUX I/O ---------------------*/ 
    // TODO: Issue with Pa_Initialize 
    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    printf( "Input device # %d.\n", inputParameters.device );
    printf( "Input LL: %g s\n", Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency );
    printf( "Input HL: %g s\n", Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency );
    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency ;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    printf( "Output device # %d.\n", outputParameters.device );
    printf( "Output LL: %g s\n", Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency );
    printf( "Output HL: %g s\n", Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency );
    outputParameters.channelCount = NUM_CHANNELS;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;




    /* -- setup -- */

   err = Pa_OpenStream(
              &stream,
              &inputParameters,
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_INPUT_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              NULL, /* no callback, use blocking API */
              NULL ); /* no callback, so no callback userData */
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;
    printf("Wire on. Will run\n"); fflush(stdout);



    cpt_buffer = 0;
    flag_debut_process = 1;



    // INITIALISATION DU BUFFER DE CALCULS
    while(cpt_buffer < CINQ_TRAME){
      err = Pa_ReadStream(stream, sampleBlock, FRAMES_PER_INPUT_BUFFER);
      if( err && CHECK_OVERFLOW ) goto xrun;
      for( i = 0 ; i < TAILLE_PETITE_TRAME ; i++){
        data_input_process->input_buffer[i+cpt_buffer*TAILLE_PETITE_TRAME] = sampleBlock[i];
      }      

      cpt_buffer++;
    }
    FILE * WriteOutput;
    FILE * WriteInput;

    while(1){


      if(!flag_debut_process){
        err = Pa_WriteStream( stream, data_output_process->output_buffer+cpt_buffer*TAILLE_PETITE_TRAME, FRAMES_PER_INPUT_BUFFER ); 
        if( err && CHECK_UNDERFLOW ) goto xrun;
      }



      if(cpt_buffer < UNE_TRAME - 1 ){ //à chaque fois qu'on a rempli une trame de 256;
        cpt_buffer++;
      }else{

        pitch_shifting( pitchShiftCoef,
                        data_input_process,
                        data_output_process,
                        flag_debut_process);

        if(flag_debut_process == 1)   flag_debut_process =0;

        WriteOutput = fopen("check_output","a");
        WriteInput = fopen("check_input","a");

        if(WriteOutput == NULL || WriteInput == NULL){
          perror("Impossible d'ouvrir un des fichiers a l'écriture");
        }else{
          for( i=0; i<256 ; i++){
            fprintf(WriteInput,"%f\n",data_input_process->input_buffer[i]);
            fprintf(WriteOutput,"%f\n",data_output_process->output_buffer[i]);
          }
        }
        fclose(WriteOutput);
        fclose(WriteInput);
        for( i=0 ; i<QUATRE_TRAME*TAILLE_PETITE_TRAME ; i++){
          data_input_process->input_buffer[i] = data_input_process->input_buffer[i+UNE_TRAME*TAILLE_PETITE_TRAME];
        }
        cpt_buffer = 0;
      }


      err = Pa_ReadStream( stream, sampleBlock, FRAMES_PER_INPUT_BUFFER );
      if( err && CHECK_OVERFLOW ) goto xrun;


      for( i = 0 ; i < TAILLE_PETITE_TRAME ; i++){
        data_input_process->input_buffer[i+(QUATRE_TRAME+cpt_buffer)*TAILLE_PETITE_TRAME] = sampleBlock[i];
      }

    }

    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;

    CLEAR( sampleBlock );

    free( sampleBlock );

    Pa_Terminate();
    return 0;

xrun:
    if( stream ) {
       Pa_AbortStream( stream );
       Pa_CloseStream( stream );
    }
    free( sampleBlock );
    Pa_Terminate();
    if( err & paInputOverflow )
       fprintf( stderr, "Input Overflow.\n" );
    if( err & paOutputUnderflow )
       fprintf( stderr, "Output Underflow.\n" );
    return -2;

error:
    if( stream ) {
       Pa_AbortStream( stream );
       Pa_CloseStream( stream );
    }
    free( sampleBlock );
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return -1;
}




