#include <stdio.h>
#include <stdlib.h> 
#include <assert.h>
#include <argp.h>

#include "nit.h"

//Init command
static char cmd_init_doc[] = {"Create an empty Nip repository"};
static error_t
parse_cmd_init(int key, char* arg, struct argp_state* state){
	switch(key){
		case ARGP_KEY_END:
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
		
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp_option cmd_init_options[] = {
	{ 0 }
};

static struct argp cmd_init_argp = {
	cmd_init_options, parse_cmd_init, 0, cmd_init_doc
};

void cmd_init(struct argp_state *state){
	int argc = state->argc - state->next + 1;
	char **argv = &state->argv[state->next - 1];
	
	argp_parse(&cmd_init_argp, argc, argv, ARGP_IN_ORDER, 0, 0);		
}

static error_t
parse_cmds(int key, char *arg, struct argp_state *state)
{
	switch (key){
		case ARGP_KEY_ARG:
			assert(arg);
			if(strcmp(arg, "init") == 0)
				cmd_init(state);
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
static char global_doc[] = 
	"\nNip -- Very simple implementation of Git"
	"\v"
	"Supported commands:\n"
	"   init        Create and empty Nip repository"	
;	

static struct argp_option global_options[] = {
	{ 0 }
};

static struct argp cmds_argp = { global_options, parse_cmds, 0, global_doc };	


//Main
int main(int argc, char **argv){
	
	argp_parse(&cmds_argp, argc, argv, ARGP_IN_ORDER, 0, 0);
	return EXIT_SUCCESS;
}
