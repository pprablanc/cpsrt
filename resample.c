#include "resample.h"


void resample( 	float * input,
 				int taille_input, 
 				float * cond_init,
 				float * output, 
 				int taille_output, 
 				int resample_coef,
 				int flag){

	int i;

	for( i=0 ; i<taille_upsample ; i++){
		sig_upsample[i] = 0;
	}

	if(flag == 1){
		for( i=0 ; i<taille_input ; i++){
			sig_upsample[i*filter_bank[bankID].q] = input[i];
		}
		filtre_resample(	sig_upsample,
				sig_filter,
				ci,
				filter_bank[bankID].length);

		if(resample_coef < 1){
			for( i=0 ; i<taille_dnsample_init ; i++){
				sig_dnsample_init[i] = sig_filter[i*filter_bank[bankID].p+3];
			}
			for( i=0; i<taille_resample_init ; i++){
				sig_resample_init[i] = sig_dnsample_init[i+3];
			}
		}else{
			for( i=0 ; i<taille_dnsample_init ; i++){
				sig_dnsample_init[i] = sig_filter[i*filter_bank[bankID].p];
			}
			for( i=0; i<taille_resample_init ; i++){
				sig_resample_init[i] = sig_dnsample_init[i+3];
			}
		}
		for( i=0 ; i<256 ; i++){
			output[i] = sig_resample_init[i];
		}
	}else{
		for( i=0 ; i<filter_bank[bankID].length ; i++){
			input_2[i] = cond_init[i];
		}
		for( i=filter_bank[bankID].length ; i<taille_input_2 ; i++){
			input_2[i] = input[i];
		}

		for( i=0 ; i<taille_input_2 ; i++){
			sig_upsample[i*filter_bank[bankID].q] = input_2[i];
		}

		filtre_resample(	sig_upsample,
				sig_filter,
				ci,
				filter_bank[bankID].length);
		if(resample_coef<1){
			for( i=0 ; i<taille_dnsample ; i++){
				sig_dnsample[i] = sig_filter[filter_bank[bankID].q*filter_bank[bankID].length+i*filter_bank[bankID].p+3];
			}
			for( i=0; i<taille_resample ; i++){
				sig_resample[i] = sig_dnsample[i+3];
			}			
		}else{
			for( i=0 ; i<taille_dnsample ; i++){
				sig_dnsample[i] = sig_filter[filter_bank[bankID].q*filter_bank[bankID].length+i*filter_bank[bankID].p];
			}
			for( i=0; i<taille_resample ; i++){
				sig_resample[i] = sig_dnsample[i+3];
			}						
		}
		for( i=0 ; i<256 ; i++){
			output[i] = sig_resample[i];
		}
	}
}

void filtre_resample(	float * sig_input, 
						float * sig_output, 
						float * cond_init,
						int taille_ci){

	int i,p;
	float tmp;
	for( i=0 ; i<taille_sig_filter_input_2 ; i++ ){
		sig_filter_input_2[i] = 0;
	}

	for( i=0 ; i<taille_ci ; i++){
		sig_filter_input_2[i] = cond_init[i];
	}

	for( i=taille_ci ; i<taille_upsample ; i++){
		sig_filter_input_2[i] = sig_input[i-taille_ci];
	}

	p=0;
	while(p < taille_sig_filter_input_2){
		tmp =0;
		for( i = 0 ; i<filter_bank[bankID].length ; i++){
			tmp += filter_bank[bankID].data[filter_bank[bankID].length-i] * sig_filter_input_2[p+i];
		}
		sig_output[p] = tmp;
		p++;
	}

}