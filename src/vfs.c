#include <klib.h>
#include <common.h>
#include <devices.h>

extern void f_init(struct filesystem *fs, const char *name, device_t *dev);
extern inode_t *f_lookup(struct filesystem *fs, const char *path, int flags, int from);
extern int f_close(inode_t *inode);

extern int i_open(file_t *file, int flags);
extern int i_close(file_t *file);
extern ssize_t i_read(file_t *file, char *buf, size_t size);
extern ssize_t i_write(file_t *file, const char *buf, size_t size);
extern off_t i_lseek(file_t *file, off_t offset, int whence);
extern int i_mkdir(inode_t *My, const char *name);
extern int i_rmdir(inode_t *My, const char *name);
extern int i_link(const char *name, inode_t *inode);
extern int i_unlink(const char *name);

static void vfs_init(){
	basic.open = &i_open;
	basic.close = &i_close;
	basic.read = &i_read;
	basic.write = &i_write;
	basic.lseek = &i_lseek;
	basic.mkdir = &i_mkdir;
	basic.rmdir = &i_rmdir;
	basic.link = &i_link;
	basic.unlink = &i_unlink;

	for(int i=0; i<10;i++) mnt[i] = NULL;
	ES.init = &f_init;
	ES.lookup = &f_lookup;
	ES.close = &f_close;
	EXT2.ops = &ES;
	EXT2.dev = dev_lookup("ramdisk1"); 

	root.refcnt = 0;
	root.ptr = NULL;
	root.bid = 0;
	root.type = 1;
	root.fs = &EXT2;
	root.size = 0;
	root.self = 0;
	root.son = 0;
	root.ptr = (void *)EXT2.dev;
	root.ops = &basic;


	vfs->mount("/",&EXT2,"blkfs");

	char c[1] = "1";

	EXT2.dev->ops->write(EXT2.dev,0,c,1);
	EXT2.dev->ops->write(EXT2.dev,MAP,(void *)&root,sizeof(root));

	
	vfs->mkdir(&EXT2, "/", "abc");
//	printf("rts1 = %d\n",root.size);
	inode_t *temp = EXT2.ops->lookup(&EXT2,"/abc",0,0);
//	printf("rts2 = %d\n",root.size);
	printf("%d\n",temp->bid);
//	printf("rts3 = %d\n",root.size);
	vfs->mkdir(&EXT2, "/abc", "edf");
	vfs->mkdir(&EXT2, "/abc", "vfsg");
	vfs->mkdir(&EXT2, "/abc", "edfdff");
	vfs->mkdir(&EXT2, "/abc", "vfsgsdf");
	vfs->mkdir(&EXT2,"/abc/vfsgsdf","dsac");
	inode_t *temp1 = EXT2.ops->lookup(&EXT2,"/abc/edf",0,0);
	inode_t *temp2 = EXT2.ops->lookup(&EXT2,"/abc/vfsg",0,0);
	inode_t *temp3 = EXT2.ops->lookup(&EXT2,"/abc/edfdff",0,0);
	inode_t *temp4 = EXT2.ops->lookup(&EXT2,"/abc/vfsgsdf",0,0);
	vfs->mkdir(&EXT2,"/abc/edf","aaa");

//	inode_t *temp2 = EXT2.ops->lookup(&EXT2,"/abc/e",0,0);
	printf("%d %d %d %d\n",temp1->bid,temp2->bid,temp3->bid,temp4->bid);

	vfs->rmdir(&EXT2,"/abc","edf");
	inode_t *temp7 = EXT2.ops->lookup(&EXT2,"/abc/edf/..",0,0);
	inode_t *temp8 = EXT2.ops->lookup(&EXT2,"/abc/vfsgsdf/dsac",0,0);
	printf("%d\n",temp8->bid);
	if(temp7 == NULL) printf("cap\n");
	else {printf("%d\n",temp7->bid);}
	inode_t *temp9 = EXT2.ops->lookup(&EXT2,"/abc/edf/a.c",1,0);
	printf("%d %d\n",temp9->bid,temp9->type);
	printf("%d\n",vfs->access(&EXT2,"/abc/edf/a.c",1));




}


static int vfs_access(const char *path, int mode){
	char dir[50];
	int lcnt = 0;
	for(int i=1;;i++){
		if(*(path+i) == '/') break;
		else dir[lcnt++] = *(path+i);
	}
	dir[lcnt] ='\0';
	if(strcmp(dir,"proc")==0) { printf("Invalid path(vfs_proc)\n"); return -1;}
	if(strcmp(dir,"dev")==0) {printf("Invalid path(vfs_dev)\n"); return -1;}

	filesystem_t *fs = &EXT2;
	inode_t *now = fs->ops->lookup(fs, path, 0, 0);
	if(now == NULL){
		printf("No such file\n");
		return -1;
	}
	if(now->right != mode){
		printf("can't access in mode %d %d",mode,now->right);
		return -1;
	}
	return 0;
}


static int vfs_mount(const char *path, filesystem_t *fs,char *name){
	int i;
	for(i=0; i<10; i++){
		if(mnt[i] == NULL)
			break;
	}
	mnt[i] = fs;

	strcpy(fs->name,name);
	fs->mounted = 1;

	strcpy(fs->mounton, path);
	
	return 0;
}


static int vfs_unmount(const char *path){
	for(int i=0;i<10;i++){
		if(mnt[i]!=NULL){
			if(strcmp(path,mnt[i]->mounton) == 0){
				if(mnt[i]->mounted == 0){
					printf("This filesystem has been unmounted!\n");
					return -1;
				}
				else{
					mnt[i]->mounted = 0;
					return 0;
				}
			}
		}
		else break;
	}
	printf("No such filesystem under this path!\n");
	return -1;
}


static int vfs_mkdir(const char *path,const char *name){
	char dir[50];
	int lcnt = 0;
	for(int i=1;;i++){
		if(*(path+i) == '/') break;
		else dir[lcnt++] = *(path+i);
	}
	dir[lcnt] ='\0';
	if(strcmp(dir,"proc")==0) { printf("Invalid path(vfs_proc)\n"); return -1;}
	if(strcmp(dir,"dev")==0) {printf("Invalid path(vfs_dev)\n"); return -1;}

	filesystem_t *fs = &EXT2;
	inode_t *now = fs->ops->lookup(fs,path,0,0);
	if(now == NULL) {
		printf("Invalid path3!\n");
		return -1;
	}
	now->ops->mkdir(now, name);
	return 0;
}


static int vfs_rmdir(const char *path, const char *name){
	char dir[50];
	int lcnt = 0;
	for(int i=1;;i++){
		if(*(path+i) == '/') break;
		else dir[lcnt++] = *(path+i);
	}
	dir[lcnt] ='\0';
	if(strcmp(dir,"proc")==0) { printf("Invalid path(vfs_proc)\n"); return -1;}
	if(strcmp(dir,"dev")==0) {printf("Invalid path(vfs_dev)\n"); return -1;}

	filesystem_t *fs = &EXT2;
	inode_t *now = fs->ops->lookup(fs,path,0,0);
	now->ops->rmdir(now,name);

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

