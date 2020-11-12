#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

typedef struct filter_t{
	float pitchShiftCoef;
	int p;
	int q;
	int length;
	float * data;
} FILTER;


int load_window_data(char * filename, float * pdata_init);
int get_nbr_files(char *directory);
int load_filter_bank(char * directory, FILTER * b_filter);
