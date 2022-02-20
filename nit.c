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
 * Try to get an existing nit repository in a given path searching recursively
 *  through the parent directories.
 */
nit_repo_t*
get_nit_repo(char *worktree_path)
{
	char *nitdir_path = create_path(worktree_path, "/.nit");	
	
	if(!dir_exists(worktree_path))
		return NULL;
	
	if(!dir_exists(nitdir_path)){
		if(strcmp(worktree_path, "/") == 0)
			return NULL;
		else
			return get_nit_repo(get_parent_path(worktree_path));
	}

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
 * Creates an object of a specific type for a given repo
 */
nit_obj_t*
create_nit_obj(nit_repo_t *repo, char *obj_path)
{
	nit_obj_t *obj = malloc(sizeof(nit_obj_t));
	if(obj == NULL)
		return NULL;

	obj->repo = repo;
	strcpy(obj->obj_path, obj_path);
	return obj;
}


/**
 * Read a nit object
 */
nit_obj_t*
read_nit_object(char *hash, nit_repo_t *repo)
{
	if(repo == NULL || hash==NULL)
		return NULL;

	char dir[3] = "";
	strncpy(dir, hash, 2);
	char *obj_file = strdup(hash + 2);
		
	char dir_path[PATH_LIMIT];
	strcpy(dir_path, repo->nitdir);
	strcat(dir_path, "/objs/");
	strcat(dir_path, dir);

	char obj_path[PATH_LIMIT];
	strcpy(obj_path, dir_path);
	strcat(obj_path, "/");
	strcat(obj_path, hash + 2);

	if(!dir_exists(dir_path))
		return NULL;

	if(!file_exists(obj_path))
		return NULL;

	return create_nit_obj(repo, obj_path);	
}


/**
 * Write a nit object
 */
char*
write_nit_object(char *file, bool write, char *type, nit_repo_t *repo)
{
	if(file == NULL)
		return NULL;

	char obj_type[20] = "";
	if(type == NULL)
		strcpy(obj_type, "blob");
	else
		strcpy(obj_type, type);
	

	//Check if directoy where the object is going to be written has 
	// been created
	char aux_path[] = ".aux.aux";	
	FILE *aux_fd = fopen(aux_path, "w");
	FILE *file_fd = fopen(file, "r");
	
	if(aux_fd == NULL || file_fd == NULL)
		return NULL;

	//Create the objects header
	char file_sz[OBJ_SIZE_LEN];
	fseek(file_fd, 0, SEEK_END);
	sprintf(file_sz,"%ld",ftell(file_fd));
	fseek(file_fd, 0, SEEK_SET);

	fprintf(aux_fd, "%s%c%s%c", obj_type, ' ', file_sz, '\0');

	//Copy data for the header	
	char c;
	while ((c = fgetc(file_fd)) != EOF)
      		fputc(c, aux_fd);		
	
	fclose(aux_fd);
	fclose(file_fd);
	
	//Hash
	uint8_t *hash;
	sha256(aux_path, &hash);
	
	//Write	
	if(write){
		if(repo == NULL){
			fprintf(stderr, "No repo founded\n");
			return NULL;
		}
	
		char dir[3] = "";
		strncpy(dir, hash, 2);
		char dir_path[PATH_LIMIT];
		snprintf(dir_path, PATH_LIMIT, "%s/objs/%s", repo->nitdir,dir);
		char obj_path[PATH_LIMIT];
		snprintf(obj_path, PATH_LIMIT, "%s/%s", dir_path, hash + 2);
		
		if(!dir_exists(dir_path)){
			create_dir(dir_path);
		}
	
		if(Z_OK != compress_file(aux_path, obj_path))
			return NULL; 	
	}
 	remove(aux_path);
	return hash;	
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
	if(nit_repo != NULL){
		printf("Nit repository already created at %s\n", nit_repo->nitdir);
		return nit_repo;
	}
	
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



/**
 * Calculates de hash ID for the file given as an arg, the type of object (default is blob) and 
 *  writes it to the objs database of the repo (this last if optional)
 */
uint8_t* 
nit_hash_object(ho_args_t *args)
{
	if(args==NULL || args->file == NULL)
		return NULL;

	if(dir_exists(args->file)){
		fprintf(stderr, "ERROR: cannot hash '%s', is a directory\n", args->file);
		return NULL;
	}

	char actual_path[PATH_LIMIT];
	get_actual_path(actual_path);
	nit_repo_t *nit_repo = get_nit_repo(actual_path);
		
	return write_nit_object(args->file, args->write, args->type, nit_repo);
}


/**
 * Get the content or type of an object given the hash and prints it via stdout
 */ 
void*
nit_cat_file(cf_args_t *args)
{
	if(args==NULL || args->hash==NULL)
		return NULL;
	
	if(!is_sha256_hash(args->hash))
		return NULL;

	char actual_path[PATH_LIMIT];
	get_actual_path(actual_path);
	nit_repo_t *nit_repo = get_nit_repo(actual_path);
	nit_obj_t* obj = read_nit_object(args->hash, nit_repo);
	if(obj == NULL)
		return NULL;

	char aux_path[] = ".aux.aux";	
	if(Z_OK != decompress_file(obj->obj_path, aux_path))
		return NULL; 	
	
	FILE *aux_fd = fopen(aux_path, "r");	
	if(aux_fd == NULL)
		return NULL;
	
	char c;
	char obj_type[OBJ_TYPE_LEN];	
	int idx = 0;
	while ((c = fgetc(aux_fd)) != ' '){
		obj_type[idx++] = c;
	}
	obj_type[idx] = '\0';

	//Discard size, we dont care
	while ((c = fgetc(aux_fd)) != '\0') continue;

	if(args->print_content){	
		while ((c = fgetc(aux_fd)) != EOF){
			printf("%c", c);
		}
		fprintf(stdout, "\n");
	
	}else if(args->print_type){
		fprintf(stdout, "%s\n", obj_type);
	}

	remove(aux_path);	
	fclose(aux_fd);		
}
