#include <stdio.h>
#include <stdlib.h> 
#include <argp.h>

#include "nit.h"

struct arguments { };

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
	switch (key){
		//"init" command
		case 'i':
			nit_repo_t *nit_repo = nit_init();
			if(nit_repo == NULL){
				printf("No repo\n");			
			}else{
				printf("%s\n", nit_repo->worktree);	
				printf("%s\n", nit_repo->nitdir);
				printf("%d\n", nit_repo->config.repo_format_version);	
				printf("%d\n", nit_repo->config.filemode);	
				printf("%d\n", nit_repo->config.bare);	
			}
			break;
		
		case ARGP_KEY_ARG:
			if (state->arg_num > 1)
				argp_usage (state);
			break;

		case ARGP_KEY_END:
			if (state->arg_num <= 0)
				argp_usage (state);
			break;

		default:
			return ARGP_ERR_UNKNOWN;
	}
  	return 0;
}



const char *argp_program_version = "Nit 0.1";
const char *argp_program_bug_address = "<>";
static char doc[] = "Nip -- Very simple implementation of Git";	

static struct argp_option options[] = {
	{"init", 'i', 0, 0,  "Create an empty Nip repository" },
	{"add" , 'a', 0, 0,  "Adds to staging area" },
	{ 0 }
};

static struct argp argp = { options, parse_opt, 0, doc };	


int main(int argc, char **argv){
	
	struct arguments args;
	argp_parse(&argp, argc, argv, 0, 0, &args);
	return EXIT_SUCCESS;
}
