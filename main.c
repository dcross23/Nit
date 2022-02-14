#include <stdio.h>
#include <stdlib.h> 
#include <assert.h>
#include <argp.h>

#include "utils.h"
#include "nit.h"

//Init command
static char cmd_init_doc[] = {"Create an empty Nip repository"};
static error_t
parse_cmd_init(int key, char* arg, struct argp_state* state){
	switch(key){
		case ARGP_KEY_END:
			nit_repo_t *nit_repo = nit_init();
			
			assert(nit_repo != NULL);
			printf("%s\n", nit_repo->worktree);	
			printf("%s\n", nit_repo->nitdir);
			printf("%d\n", nit_repo->config.repo_format_version);	
			printf("%d\n", nit_repo->config.filemode);	
			printf("%d\n", nit_repo->config.bare);	
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

//Hash-object command
static char cmd_hash_obj_doc[] = {"Compute object hash (optionally creates a blob object from the file)"};

static error_t
parse_cmd_hash_obj(int key, char* arg, struct argp_state* state){
	ho_args_t *arguments = state->input;
	
	switch(key){
		case 't':
			if(strcmp(arg, "blob"  ) != 0 &&
			   strcmp(arg, "commit") != 0 &&
			   strcmp(arg, "tag"   ) != 0 &&
			   strcmp(arg, "tree"  ) != 0
			){
				argp_usage(state);
			}else
				arguments->type = strdup(arg);
		
		break;

		case 'w':
			arguments->write = true;
		break;
		
			
		case ARGP_KEY_ARG:
			if (state->arg_num >= 1)
				argp_usage (state);

		     	arguments->file = strdup(arg);

		break;
		
		case ARGP_KEY_END:
			if (state->arg_num < 1)
        			argp_usage (state);
			
		break;
		
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp_option cmd_hash_obj_options[] = {
	{"type", 't', "type", 0, "Specify the type of the object (blob by default)"},
	{"write", 'w', 0, 0, "Write object into the objs database"},
	{ 0 }
};

static char cmd_hash_obj_args_doc[] = "FILE";

static struct argp cmd_hash_obj_argp = {
	cmd_hash_obj_options, parse_cmd_hash_obj, cmd_hash_obj_args_doc, cmd_hash_obj_doc
};

void cmd_hash_obj(struct argp_state *state){
	int argc = state->argc - state->next + 1;
	char **argv = &state->argv[state->next - 1];
	
	ho_args_t arguments = {NULL, false, NULL};
	argp_parse(&cmd_hash_obj_argp, argc, argv, 0, 0, &arguments);
	
	uint8_t* ret = nit_hash_object(&arguments);	
	if(ret != NULL) printf("%s\n", ret);
}

//Cat-file command
static char cmd_cat_file_doc[] = {"Get content of repository objects"};

static error_t
parse_cmd_cat_file(int key, char* arg, struct argp_state* state){
	cf_args_t *arguments = state->input;
	
	switch(key){
		case 't':
			arguments->print_type = true;
		break;

		case 'p':
			arguments->print_content = true;
		break;
		
			
		case ARGP_KEY_ARG:
			if (state->arg_num >= 1)
				argp_usage (state);

		     	arguments->hash = strdup(arg);
		break;
		
		case ARGP_KEY_END:
			if (state->arg_num < 1)
        			argp_usage (state);
		break;
		
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp_option cmd_cat_file_options[] = {
	{"type" , 't', 0, 0, "Show the object type instead of the content"},
	{"print", 'p', 0, 0, "Show the content of the object (has priority)"},
	{ 0 }
};

static char cmd_cat_file_args_doc[] = "OBJ";

static struct argp cmd_cat_file_argp = {
	cmd_cat_file_options, parse_cmd_cat_file, cmd_cat_file_args_doc, cmd_cat_file_doc
};

void cmd_cat_file(struct argp_state *state){
	int argc = state->argc - state->next + 1;
	char **argv = &state->argv[state->next - 1];
	
	cf_args_t arguments = {false, false};
	argp_parse(&cmd_cat_file_argp, argc, argv, 0, 0, &arguments);
	nit_cat_file(&arguments);	
}


//Global
static error_t
parse_cmds(int key, char *arg, struct argp_state *state)
{
	switch (key){
		case ARGP_KEY_ARG:
			assert(arg);
			if(strcmp(arg, "init") == 0)
				cmd_init(state);
			else if(strcmp(arg, "hash-object") == 0)
				cmd_hash_obj(state);
			else if(strcmp(arg, "cat-file") == 0)
				cmd_cat_file(state);			
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
	"   init            Create an empty Nip repository\n"
	"   hash-object	    Compute object hash [creates a blob object from the file]\n"
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
