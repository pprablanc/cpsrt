#include "init.h"

int load_window_data(char * filename, float * pdata_init) {

	FILE * fp;
	float * data = pdata_init;

	fp = fopen(filename,"rb");
	if (fp==NULL){ 
		return -1;
	}else { 
		while(fscanf(fp,"%f",data) != EOF){
			data++;
		}
	}
	if(fclose(fp) != 0){
		return -1;
	}else{
		return 0;
	} 
}

int load_filter_bank(char * directory, FILTER * b_filter){

	FILE * fp;
	int nbrFiles;
	int i,j;
	char path[256];
	

    DIR* dir = NULL;
    struct dirent* currentFile = NULL; /* Déclaration d'un pointeur vers la structure dirent. */

    dir = opendir(directory);
    if (dir == NULL){ 
        perror("directory : ");
    }else{
		i = 0;
    	while((currentFile = readdir(dir)) != NULL){
       		if (strcmp(currentFile->d_name, ".") != 0 && strcmp(currentFile->d_name, "..") != 0){ 
       			strcpy(path,directory);
    			strcat(path,currentFile->d_name);
				fp = fopen(path,"rb");
				if( fp==NULL){
					perror("file : ");
					return -1;
				}else{
					fscanf(fp,"%f",&(b_filter[i].pitchShiftCoef));
					fscanf(fp,"%d",&(b_filter[i].p));
					fscanf(fp,"%d",&(b_filter[i].q));
					fscanf(fp,"%d",&(b_filter[i].length));
					b_filter[i].data	= (float *) malloc(b_filter[i].length*sizeof(float));
					float * data_filter = b_filter[i].data;

					for(j = 0; j < b_filter[i].length ; j++){
						fscanf(fp,"%f",data_filter);
						data_filter++;
					}
				i++;	 

				}
				if(fclose(fp) != 0){
					perror("file :");
					return 	-1;	
				}
			}
    	}
    }
    closedir(dir);
    return 0;
}

int get_nbr_files(char *directory){


    DIR* dir = NULL;
    int nbrFiles;
    struct dirent* currentFile = NULL; /* Déclaration d'un pointeur vers la structure dirent. */
    dir = opendir(directory); /* Ouverture d'un dossier */

    if (dir == NULL){ /* Si le dossier n'a pas pu être ouvert, */
        perror(""); /* perror() va nous écrire l'erreur. */
    }else{
    	nbrFiles =0;
    	while ((currentFile = readdir(dir)) != NULL){
    		if(strcmp(currentFile->d_name, ".") != 0 && strcmp(currentFile->d_name, "..") != 0){
      			nbrFiles++;
			}
 		}
	}
	closedir(dir);
	return nbrFiles;
}