#include <common.h>
#include <klib.h>


void f_init(struct filesystem *fs, const char *name, device_t *dev){
	fs->dev = dev;
	fs->name = name;
}



inode_t *f_lookup(struct filesystem *fs, const char *path, int flags, int from){
//	if(from == 0){




	return NULL;
}



int f_close(inode_t *inode){
	return 0;
}


