#include <common.h>
#include <klib.h>


int i_open(file_t file, int flags){
	return 0;
}



int i_close(file_t *file){
	return 0;
}



ssize_t i_read(file_t *file, char *buf, size_t size){
       return 0;
}



ssize_t i_write(file_t *file, const char *buf, size_t size){
	return 0;
}



off_t i_lseek(file_t *file, off_t offset, int whence){
	return 0;
}



int i_mkdir(const char *name){
	return 0;
}



int i_rmdir(const char *name){
	return 0;
}



int i_link(const char *name, inode_t *inode){
	return 0;
}



int i_unlink(const char *name){
	return 0;
}
