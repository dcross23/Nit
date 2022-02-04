#include "nit.h"

/**
 * Load the config if there is an existing nit repository
 */
nit_config_t*
get_nit_repo_config(char *nitdir_path)
{
	if(!dir_exists(nitdir_path))
		return NULL;
	
	char *conf_path = create_path(nitdir_path, "/config");
	if(!file_exists(conf_path))
		return NULL;
	
	nit_config_t *nit_config = malloc(sizeof(nit_config_t));
	if(nit_config == NULL){
		fprintf(stderr, "ERROR: malloc. Unable to allocate memory for nit config.\n");
		return NULL;	
	}

	FILE *conf_file = fopen(conf_path, "r");
	if(conf_file == NULL)
		return NULL;

	char conf_line[CONF_LIMIT];
	//[core]
	fgets(conf_line, CONF_LIMIT, conf_file);
	
	//repository_format_version
	fgets(conf_line, CONF_LIMIT, conf_file);
	STRIP(conf_line)
	strtok(conf_line, "=");	
	nit_config->repo_format_version = (uint8_t) atoi(strtok(NULL, "="));	
	
	//filemode
	fgets(conf_line, CONF_LIMIT, conf_file);
	STRIP(conf_line)
	strtok(conf_line, "=");	
	nit_config->filemode = (bool) atoi(strtok(NULL, "="));	
	
	//bare
	fgets(conf_line, CONF_LIMIT, conf_file);
	STRIP(conf_line)
	strtok(conf_line, "=");	
	nit_config->bare = (bool) atoi(strtok(NULL, "="));	
	
	fclose(conf_file);
	return nit_config;	
}


/**
 * Saves the nit repository config in the config file
 */
int
set_nit_repo_config(nit_config_t *nit_config, char *nitdir_path)
{
	if(nit_config == NULL)
		return -1;

	if(!dir_exists(nitdir_path))
		return -1;

	FILE *conf_file = fopen(create_path(nitdir_path, "/config"), "w");
	if(conf_file == NULL)
		return -1;

	fprintf(conf_file, "[core]\n");
	fprintf(conf_file, "repo_format_version=%d\n", nit_config->repo_format_version);
	fprintf(conf_file, "filemode=%d\n", nit_config->filemode);
	fprintf(conf_file, "bare=%d\n", nit_config->bare);
	fclose(conf_file);
	return 0;
}



/**
 * Try to get an existing nit repository in a given path.
 */
nit_repo_t*
get_nit_repo(char *worktree_path)
{
	char *nitdir_path = create_path(worktree_path, "/.nit");	

	if(!dir_exists(worktree_path))
		return NULL;
	
	if(!dir_exists(nitdir_path))
		return NULL;

	nit_repo_t *nit_repo = malloc(sizeof(nit_repo_t));
	if(nit_repo == NULL){
		fprintf(stderr, "ERROR: malloc. Unable to allocate memory for nit repository.\n");
		return NULL;
	}
	
	strcpy(nit_repo->worktree, worktree_path);
	strcpy(nit_repo->nitdir  , nitdir_path);
	nit_config_t *nit_config = get_nit_repo_config(nitdir_path);
	nit_repo->config = *nit_config;
	return nit_repo;
}


/**
 * Create an empty Nip repository
 */
nit_repo_t* 
nit_init()
{
	char worktree_path[PATH_LIMIT];
	if(get_actual_path(worktree_path) != 0)
		return NULL;		

	nit_repo_t *nit_repo = get_nit_repo(worktree_path);
	if(nit_repo != NULL)
		return nit_repo;
	
	//If does not exist, just create it
	char *nitdir_path = create_path(worktree_path, "/.nit");	
	if(create_dir(nitdir_path) != 0){
		fprintf(stderr, "ERROR: create_dir. Cannot create .nit directory.\n");	
		return NULL;					
	}

	//Create all repo dirs
	char *repo_dirs[] = {
		"/branches",	
		"/objs",
		"/refs",
		"/refs/heads",
		"/refs/tags"
	};

	char *repo_files[] = {
		"/config",
		"/HEAD",
		"/description"
	};

	nit_config_t nit_config_default = {
		.repo_format_version = 0,
		.filemode = false,
		.bare = false
	};

	for(int i=0; i<ARRAYLEN(repo_dirs); i++){
		if(create_dir(create_path(nitdir_path, repo_dirs[i])) != 0){
			fprintf(stderr, "ERROR: create_dir. Cannot create %s directory.\n", repo_dirs[i]);	
			return NULL;					
		}
	}
	
	for(int i=0; i<ARRAYLEN(repo_files); i++){
		if(create_file(create_path(nitdir_path, repo_files[i])) != 0){
			fprintf(stderr, "ERROR: create_dir. Cannot create %s file.\n", repo_files[i]);	
			return NULL;					
		}
	}
	
	
	nit_repo = malloc(sizeof(nit_repo_t));
	if(nit_repo == NULL){
		fprintf(stderr, "ERROR: malloc. Unable to allocate memory for nit repository.\n");
		return NULL;
	}
	
	strcpy(nit_repo->worktree, worktree_path);
	strcpy(nit_repo->nitdir  , nitdir_path);
	nit_repo->config = nit_config_default;	
	
	if(set_nit_repo_config(&nit_config_default, nitdir_path) == -1){
		fprintf(stderr, "ERROR: set_nit_repo_config. Cannot set nit repo configuration.\n");	
		return NULL;					
	}

	return nit_repo;
}


