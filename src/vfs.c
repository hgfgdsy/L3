#include <klib.h>
#include <common.h>
#include <devices.h>

extern void f_init(struct filesystem *fs, const char *name, device_t *dev);
extern inode_t *f_lookup(struct filesystem *fs, const char *path, int flags);
extern int f_close(inode_t *inode);


static void vfs_init(){
	ES.init = &f_init;
	ES.lookup = &f_lookup;
	ES.close = &f_close;

}


static int vfs_access(const char *path, int mode){
	
	return 0;
}


static int vfs_mount(const char *path, filesystem_t *fs){
	return 0;
}


static int vfs_unmount(const char *path){
	return 0;
}


static int vfs_mkdir(const char *path){
	return 0;
}


static int vfs_rmdir(const char *path){
	return 0;
}


static int vfs_link(const char *oldpath, const char *newpath){
	return 0;
}


static int vfs_unlink(const char *path){
	return 0;
}


static int vfs_open(const char *path, int flags){
	return 0;
}


static ssize_t vfs_read(int fd, void *buf, size_t nbyte){
	return 0;
}


static ssize_t vfs_write(int fd, void *buf, size_t nbyte){
	return 0;
}


static off_t vfs_lseek(int fd, off_t offset, int whence){
	return 0;
}


static int vfs_close(int fd){
	return 0;
}



MODULE_DEF(vfs){
	.init = vfs_init,
	.access = vfs_access,
	.mount = vfs_mount,
	.unmount = vfs_unmount,
	.mkdir = vfs_mkdir,
	.rmdir = vfs_rmdir,
	.link = vfs_link,
	.unlink = vfs_unlink,
	.open = vfs_open,
	.read = vfs_read,
	.write = vfs_write,
	.lseek = vfs_lseek,
	.close = vfs_close,
};

