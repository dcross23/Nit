#include "utils.h"

/**
 * Get actual path
 */
int
get_actual_path(char *actual_path)
{
	if(getcwd(actual_path, PATH_LIMIT) == NULL){
		fprintf(stderr, "ERROR: getcwd. Cannot get actual path\n");
		return -1;			
	}
	return 0;
}

/**
 * Concats 2 paths in one
 */
char*
create_path(char *path, char *subpath)
{
	char *dst = malloc(PATH_LIMIT * sizeof(char));
	dst[0] = '\0';
	strcpy(dst, path);
	strcat(dst, subpath);
	return dst;
}	

/**
 * Return 1 if directory exists, 0 if not, -1 if error.
 */ 
int
dir_exists(char *path)
{
	DIR *dir = opendir(path);
	if(dir != NULL){
		closedir(dir);
		return 1;

	}else if(ENOENT == errno)
		return 0;
	else
		return -1;
}

/**
 * Creates a directory if it does not exist
 */ 
int
create_dir(char *path)
{
	int exists = dir_exists(path);
	if(exists){
		if(exists == -1)		
			fprintf(stderr, "ERROR: opendir failed\n");
		return exists;
	}	

	//755 default mode
	mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
	return mkdir(path, mode);
}

/**
 * Return 1 if file exists, 0 if not
 */
int
file_exists(char *path)
{
	FILE *file = fopen(path, "r");
	if(file != NULL){
		fclose(file);
		return 1; 
	}
	return 0;
}

/**
 * Creates a file if it does not exist
 */
int 
create_file(char *path)
{
	int exists = file_exists(path);
	if(exists)
		return exists;
	
	FILE *file = fopen(path, "w");
	if(file == NULL)
		return -1;
	
	return 0;	
}

