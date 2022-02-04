#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define PATH_LIMIT 100
#define CONF_LIMIT 100

#define ARRAYLEN(array) (sizeof(array)/sizeof(array[0]))
#define STRIP(s) if(s[strlen(s)-1] == '\n') s[strlen(s)-1] = '\0';

int   get_actual_path(char *actual_path);
char* create_path(char *path, char *subpath);
int   dir_exists(char *path);
int   create_dir(char *path);
int   file_exists(char *path);
int   create_file(char *path);


#endif
