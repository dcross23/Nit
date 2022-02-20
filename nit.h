#ifndef __NIT_H__
#define __NIT_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <openssl/sha.h>
#include "utils.h"

#define OBJ_TYPE_LEN 10
#define OBJ_SIZE_LEN 30

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

typedef struct nit_obj{
	nit_repo_t *repo;
	char obj_path[PATH_LIMIT];
}nit_obj_t;


//Command args
typedef struct ho_args{
	char *type;
	bool write;
	char *file;
}ho_args_t;

typedef struct cf_args{
	bool print_type;
	bool print_content;
	char *hash;
}cf_args_t;

nit_repo_t* get_nit_repo(char *worktree_path);

nit_repo_t* nit_init();
uint8_t* nit_hash_object(ho_args_t *args);
void* nit_cat_file(cf_args_t *args);

#endif
