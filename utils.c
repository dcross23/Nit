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
 * Returns the parent path of a given path.
 */	
char*
get_parent_path(char *path){
	if(strcmp(path, "/") == 0)
		return NULL;
	else{
		char *parent = strdup(path);
		strrchr(parent, '/')[0] = '\0';
		return parent;
	}
}

/**
 * Return 1 if directory exists, 0 if not.
 */ 
int
dir_exists(char *path)
{
	struct stat sb;
	if(stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
		return 1;
	else
		return 0;
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



/**
 * Calculates sha256 hash of a given file.
 */ 
int 
sha256(char *file, unsigned char **sha256hash){
	size_t len;
	unsigned char chunk[SHA256_DIGEST_LENGTH];	
	unsigned char hash[SHA256_DIGEST_LENGTH]  = { 0 };
	
	FILE *fd = fopen(file, "rb");
	if(fd == NULL)
		return -1;
	
	SHA256_CTX ctx;
  	SHA256_Init(&ctx);
  	while (len = fread(chunk, 1, SHA256_DIGEST_LENGTH, fd)) {
		SHA256_Update(&ctx, chunk, len);
  	}
  	SHA256_Final(hash, &ctx);
	
	fclose(fd);
	
	//Reserve 65 bytes for the string: 64 bytes (256 bits * 2) for the hash in 
	// hexadecimal (2 hex digits for each of the 32 bytes of the hash), and the 
	// last for '\0'
	(*sha256hash) = malloc((2*SHA256_DIGEST_LENGTH + 1) * sizeof(unsigned char));
	memset(*sha256hash, '\0', 65);

	for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
		sprintf((*sha256hash) + 2*i, "%02x", hash[i]);
	}   
	return 0;
}


/**
 * Compress the file in into file out
 */
int 
compress_file(char *file_in_path, char *file_out_path)
{
	FILE *fin = fopen(file_in_path, "rb");
	FILE *fout = fopen(file_out_path, "wb");
	
	if(fin == NULL || fout == NULL)
		return Z_ERRNO;


	z_stream stream = {Z_NULL, Z_NULL, Z_NULL};
	int ret, flush;
	unsigned int have;	
	uint8_t in[COMPRESS_CHUNK];	
	uint8_t out[COMPRESS_CHUNK];	

	ret = deflateInit(&stream, Z_DEFAULT_COMPRESSION);	
	if(ret != Z_OK)
		return ret;

	do{
		stream.avail_in = fread(in, 1, COMPRESS_CHUNK, fin);
		if(ferror(fin)){
			deflateEnd(&stream);
			return Z_ERRNO;
		}
	
		flush = feof(fin) ? Z_FINISH : Z_NO_FLUSH;
		stream.next_in = in;
		do{	
			stream.avail_out = COMPRESS_CHUNK;
			stream.next_out = out;
			
			ret = deflate(&stream, flush);
			assert(ret != Z_STREAM_ERROR);

			have = COMPRESS_CHUNK - stream.avail_out;
			if(fwrite(out, 1, have, fout) != have || ferror(fout)){
				deflateEnd(&stream);
				return Z_ERRNO;
			}	
		}while(stream.avail_out == 0);
		assert(stream.avail_in == 0);

	}while(flush != Z_FINISH);
	assert(ret == Z_STREAM_END);	

	deflateEnd(&stream);
	return Z_OK;
}


/**
 * Decompress the file in into file out
 */
int 
decompress_file(char *file_in_path, char *file_out_path)
{
	FILE *fin = fopen(file_in_path, "rb");
	FILE *fout = fopen(file_out_path, "wb");
	
	if(fin == NULL || fout == NULL)
		return Z_ERRNO;

	z_stream stream = {
		.zalloc = Z_NULL,
		.zfree  = Z_NULL,
		.opaque  = Z_NULL,
		.avail_in = 0,
		.next_in = Z_NULL	
	};	
		
	int ret;
	unsigned int have;	
	uint8_t in[COMPRESS_CHUNK];	
	uint8_t out[COMPRESS_CHUNK];	

	ret = inflateInit(&stream);
	if(ret != Z_OK)
		return ret;

	do{
		stream.avail_in = fread(in, 1, COMPRESS_CHUNK, fin);
		if(ferror(fin)){
			inflateEnd(&stream);
			return Z_ERRNO;
		}

		if(stream.avail_in == 0) 
			break;
	
		stream.next_in = in;
		do{
			stream.avail_out = COMPRESS_CHUNK;
			stream.next_out  = out;

			ret = inflate(&stream, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);

			switch(ret){
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					inflateEnd(&stream);
					return ret;
			}

			have = COMPRESS_CHUNK - stream.avail_out;
			if(fwrite(out, 1, have, fout) != have || ferror(fout)){
				inflateEnd(&stream);
				return Z_ERRNO;
			}
		}while(stream.avail_out == 0);		
	}while(ret != Z_STREAM_END);

	inflateEnd(&stream);
	if(ret == Z_STREAM_END)
		return Z_OK;
	else
		return Z_DATA_ERROR;		
}


