#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <openssl/sha.h>
#include <zlib.h>


#define PATH_LIMIT 200
#define CONF_LIMIT 100

#define ARRAYLEN(array) (sizeof(array)/sizeof(array[0]))
#define STRIP(s) if(s[strlen(s)-1] == '\n') s[strlen(s)-1] = '\0';

#define COMPRESS_CHUNK 16384

//Paths, files and directories
int   get_actual_path(char *actual_path);
char* create_path(char *path, char *subpath);
char* get_parent_path(char *path);
int   dir_exists(char *path);
int   create_dir(char *path);
int   file_exists(char *path);
int   create_file(char *path);


//Encryption
int sha256(char *file, unsigned char **sha256hash);


//Compression/Decompression
//Based on example in: https://zlib.net/zpipe.c
int compress_file(char *file_in_path, char *file_out_path);
int decompress_file(char *file_in_path, char *file_out_path);

#endif
