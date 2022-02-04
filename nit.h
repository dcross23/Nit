#ifndef __NIT_H__
#define __NIT_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "utils.h"


typedef struct nit_config{
	uint8_t repo_format_version;
	bool filemode;
	bool bare;		
}nit_config_t;


typedef struct nit_repo{
	char worktree[PATH_LIMIT];  //Path to worktree
	char nitdir[PATH_LIMIT];    //Path to .nit directory
	nit_config_t config; 	    //Config of the .nit repo 
}nit_repo_t;


nit_repo_t* nit_init();


#endif
