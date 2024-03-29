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
extern int i_link(const char *name, inode_t *inode, inode_t *new);
extern int i_unlink(const char *name,inode_t *inode);


void proc_read(const char *path, int sto){
	if(strcmp(path+6,"cpuinfo") == 0){
		vfs->write(sto,cpuinfo,strlen(cpuinfo));
		return ;
	}
	if(strcmp(path+6,"meminfo") == 0){
		vfs->write(sto,meminfo,strlen(meminfo));
		return ;
	}
	if(*(path+6)>='0' && *(path+6)<='9'){
		int x = *(path+6) - '0';
		if(*(path+7)>='0' && *(path+7)<='9')
			x = x*10 + *(path+7) - '0';
		if(x>=20 || CTD[x] == 0)
			vfs->write(sto,"Unknown task\n",13);
		else{
			vfs->write(sto,pos[x],strlen(pos[x]));
			vfs->write(sto,"\n",1);
		}
		return ;
	}
	vfs->write(sto,"Unknown task\n",13);

}



static void vfs_init(){
	cpuinfo = pmm->alloc(100);
	strcpy((char *)cpuinfo,"CPU:4\n");
	meminfo = pmm->alloc(100);
	strcpy((char *)meminfo,"MemtTotal : 16303692kB\n");

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
	DFS.ops = &ES;
	DFS.dev = NULL;
	PFS.ops = &ES;
	PFS.dev = NULL;

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
	
//	vfs->mkdir("/dev");

//	vfs->mount("/dev",&EXT2, "devfs");

	char c[1] = "1";

	EXT2.dev->ops->write(EXT2.dev,0,c,1);
	EXT2.dev->ops->write(EXT2.dev,MAP,(void *)&root,sizeof(root));
	vfs->mkdir("/dev");

	vfs->mount("/dev",&DFS, "devfs");

	vfs->mkdir("/proc");

	vfs->mount("/proc",&PFS, "procfs");

//	vfs->mkdir("/proc/4");

	vfs->mkdir("/dev/tty1");

	vfs->mkdir("/dev/tty2");

	vfs->mkdir("/dev/tty3");

	vfs->mkdir("/dev/tty4");

	vfs->mkdir("/dev/ramdisk0");

	vfs->mkdir("/dev/ramdisk1");

	vfs->mkdir("/hgfgdsy");

	vfs->touch("/hgfgdsy/a.c",-1);

	vfs->edit("/hgfgdsy/a.c","#include<stdio.h>\n\nint main()\n{\nprintf(\"hello\\n\");\nreturn 0;\n}\n",-1);

/*	
	vfs->mkdir("/abc");
	vfs->mkdir("/sdfv");
//	printf("rts1 = %d\n",root.size);
	inode_t *temp = EXT2.ops->lookup(&EXT2,"/abc",0,0);
//	printf("rts2 = %d\n",root.size);
	printf("%d\n",temp->bid);
//	printf("rts3 = %d\n",root.size);
	vfs->mkdir("/abc/edf");
	vfs->mkdir("/abc/vfsg");
	vfs->mkdir("/abc/edfdff");
	vfs->mkdir("/abc/vfsgsdf");
	vfs->mkdir("/abc/vfsgsdf/dsac");
	inode_t *temp1 = EXT2.ops->lookup(&EXT2, "/abc/edf",0,0);
	inode_t *temp2 = EXT2.ops->lookup(&EXT2, "/abc/vfsg",0,0);
	inode_t *temp3 = EXT2.ops->lookup(&EXT2, "/abc/edfdff",0,0);
	inode_t *temp4 = EXT2.ops->lookup(&EXT2, "/abc/vfsgsdf",0,0);
	vfs->mkdir("/abc/edf/aaa");

//	inode_t *temp2 = EXT2.ops->lookup(&EXT2,"/abc/e",0,0);
	printf("%d %d %d %d\n",temp1->bid,temp2->bid,temp3->bid,temp4->bid);

	vfs->rmdir("/abc/edf");
	inode_t *temp7 = EXT2.ops->lookup(&EXT2, "/abc/edf/..",0,0);
	inode_t *temp8 = EXT2.ops->lookup(&EXT2, "/abc/vfsgsdf/dsac",0,0);
	printf("%d\n",temp8->bid);
	if(temp7 == NULL) printf("cap\n");
	else {printf("%d\n",temp7->bid);}
	inode_t *temp9 = EXT2.ops->lookup(&EXT2, "/abc/edf/a.c",1,0);
	printf("%d %d\n",temp9->bid,temp9->type);
	printf("%d\n",vfs->access("/abc/edf/a.c",1));
	printf("%d\n",sizeof(tory_t));
	vfs->link("/abc/edf/a.c","/abc/b.c");
	inode_t *temp10 = EXT2.ops->lookup(&EXT2, "/abc/b.c",0,0);
	if(temp10 == NULL) printf("cap\n");
	else{
		printf("%d %d %d\n",temp9->self,temp10->self,temp10->refcnt);
	}
//	vfs->unlink("/abc/edf/a.c");
	inode_t *temp11 = EXT2.ops->lookup(&EXT2, "/abc/b.c",0,0);
	if(temp11 == NULL) printf("yes\n");
*/



}


static int vfs_touch(const char *path, int sto){
//	printf("%s\n",path);
	if(strncmp(path,"/dev",4)==0){
		vfs->write(sto,"Permission denied\n",18);
		return -1;
	}
	if(strncmp(path,"/proc",5)==0){
		vfs->write(sto,"Permission denied\n",18);
		return -1;
	}
	filesystem_t *fs = &EXT2;
	inode_t *now = fs->ops->lookup(fs,path,1,0);
	return now->self;
}


static int vfs_edit(const char *path,const char *buf,int sto){
	if(strncmp(path,"/dev",4)==0){
		vfs->write(sto,"Permission denied\n",18);
		return -1;
	}
	if(strncmp(path,"/proc",5)==0){
		vfs->write(sto,"Permission denied\n",18);
		return -1;
	}
	filesystem_t *fs = &EXT2;
	inode_t *now = fs->ops->lookup(fs,path,0,0);
	if(now == NULL){
		vfs->write(sto,"Invalid target\n",15);
		return -1;
	}
	if(now->type == 1){
		vfs->write(sto, "Is is a directory\n",18);
		return -1;
	}
	device_t *mi = (device_t *)now->ptr;
	int dlen = strlen(buf);
	mi->ops->write(mi,D+(now->bid)*(1<<12)+now->size,buf,dlen);
	now->size+=dlen;
	mi->ops->write(mi,MAP+(now->self)*64,(void *)now,sizeof(inode_t));
	return 0;
}


static int vfs_cat(const char *path, int sto){
	if(strncmp(path,"/dev",4)==0){
		vfs->write(sto,"Permission denied\n",18);
		return -1;
	}
	if(strncmp(path,"/proc",5)==0){
		proc_read(path,sto);
		return 0;
//		vfs->write(sto,"Permission denied\n",18);
//		return -1;
	}
	filesystem_t *fs = &EXT2;
	inode_t *now = fs->ops->lookup(fs,path,0,0);
	if(now == NULL){
		vfs->write(sto,"Invalid target\n",15);
		return -1;
	}
	if(now->type == 1){
		vfs->write(sto, "Is is a directory\n",18);
		return -1;
	}
	device_t *mi = (device_t *)now->ptr;
	char buf[1<<12];
	mi->ops->read(mi,D+(now->self)*(1<<12),buf,now->size);
	buf[now->size] = '\n';
	vfs->write(sto,buf,now->size);
	vfs->write(sto,"\n",1);
	return 0;
}



static int vfs_rm(const char *path,int sto){
	if(strncmp(path,"/dev",4)==0){
		vfs->write(sto,"Permission denied\n",18);
		return -1;
	}
	if(strncmp(path,"/proc",5)==0){
		vfs->write(sto,"Permission denied\n",18);
		return -1;
	}
	filesystem_t *fs = &EXT2;
	inode_t *now = fs->ops->lookup(fs,path,0,0);
	if(now == NULL){
		printf("%s\n",path);
		vfs->write(sto,"Invalid target\n",15);
		return -1;
	}
	if(now->type == 2){
		vfs->unlink(path);
	}
	else
		vfs->rmdir(path);
	return 0;
}

static int vfs_cd(const char *left,char *path, int sto){
	int i;
	int llen = strlen(left);

	for(i=0;i<llen;i++)
		if(*(left+i)!=' ')
			break;
	int tlen = strlen(path);
	if(tlen == 1 && *(path+i) == '.'){
		vfs->write(sto,"error\n",6);
		return -1;
	}
	char lu[256];
	memset(lu,0,sizeof(lu));
	if(strcmp(path,"/dev")==0 && *(left+i) != '.'){
		vfs->write(sto,"Permission denied\n",18);
		return -1;
	}
	strcpy(lu,path);
	if(tlen!=1){
		lu[tlen] = '/';
		strcpy(&lu[tlen+1],left+i);
	}
	else{
	        strcpy(&lu[tlen],left+i);
	}
	int olen = strlen(lu);
	lu[olen-1] = '\0';
	filesystem_t *fs = &EXT2;
	inode_t *now = fs->ops->lookup(fs,lu,0,0);
	if(now == NULL){
		vfs->write(sto,"It is not a directory\n",22);
		return -1;
	}
	if(now->type == 2){
		vfs->write(sto,"It is not a directory\n",22);
		return -1;
	}
	int j;
	if(*(left+i) == '.'){
		for(j=tlen-1;j>=0;j--){
			if(*(path+j)=='/')
				break;
		}
		if(j!=0)
		path[j] ='\0';
		else{
			path[j+1] = '\0';
		}
		return 0;
	}
	strcpy(path,lu);
	return 0;
}



static int vfs_ls(const char *path,int sto){
/*	char dir[50];
	int lcnt = 0;
	int llen=strlen(path);
	if(llen>1){
	for(int i=1;i<llen;i++){
		if(*(path+i) == '/') break;
		else dir[lcnt++] = *(path+i);
	}
	dir[lcnt] ='\0';*/
//	if(strcmp(dir,"proc")==0) { printf("Invalid path(vfs_proc)\n"); return -1;}
//	if(strcmp(dir,"dev")==0) {printf("Invalid path(vfs_dev)\n"); return -1;}
//        }
        filesystem_t *fs = &EXT2;
	inode_t *now = fs->ops->lookup(fs,path,0,0);
	printf("now -> self = %d\n",now->self);
        char data[1<<12];
	int rec = 0;
	device_t *mi = (device_t *)now->ptr;
	int dlen = mi->ops->read(mi,D+(1<<12)*now->self,(void *)data,now->size);
	int I;
	while(rec < dlen){
		I = *(int *)&data[rec];
		if(I == -1){
			rec += *(short *)&data[rec+4];
			continue;
		}
		else{
			vfs->write(sto,&data[rec+8],*(char *)&data[rec+6]);
			vfs->write(sto,"     ",5);
			rec +=*(short *)&data[rec+4];
		}
	}
	vfs->write(sto,"\n",1);
	fs->ops->close(now);
	return 0;
}





static int vfs_access(const char *path, int mode){
	char dir[50];
	int lcnt = 0;
	int llen = strlen(path);
	for(int i=1;i<llen;i++){
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


static int vfs_mkdir(const char *path){
/*	char dir[50];
	int lcnt = 0;
	int llen = strlen(path);
	if(llen>1){
	for(int i=1;i<llen;i++){
		if(*(path+i) == '/') break;
		else dir[lcnt++] = *(path+i);
	}
	dir[lcnt] ='\0';
//	if(strcmp(dir,"proc")==0) { printf("Invalid path(vfs_proc)\n"); return -1;}
//	if(strcmp(dir,"dev")==0) {printf("Invalid path(vfs_dev)\n"); return -1;}
        }*/

	filesystem_t *fs = &EXT2;
	
	char name[50];
	int lcnt=0;
	int len = strlen(path);
	for(int i=0;i<len;i++){
		if(*(path+i) == '/' && i != len-1) lcnt = 0;
		else name[lcnt++] = *(path+i);
	}
	name[lcnt] = '\0';
	char *my_path = (char *)pmm->alloc(200);
	strncpy(my_path,path,len-lcnt-1);
	if(my_path[0]!='/') my_path[0] = '/';
	inode_t *now = fs->ops->lookup(fs,my_path,0,0);
	if(now == NULL) {
		printf("Invalid path3!\n");
		return -1;
	}
	now->ops->mkdir(now, name);
	return 0;
}


static int vfs_rmdir(const char *path){
	char dir[50];
	int lcnt = 0;
	int llen = strlen(path);
	if(llen>1){
	for(int i=1;i<llen;i++){
		if(*(path+i) == '/') break;
		else dir[lcnt++] = *(path+i);
	}
	dir[lcnt] ='\0';
	if(strcmp(dir,"proc")==0) { printf("Permission denied\n"); return -1;}
	if(strcmp(dir,"dev")==0) {printf("Permission denied\n"); return -1;}
	}
	filesystem_t *fs = &EXT2;

	char name[50];
	int len = strlen(path);
	for(int i=0;i<len;i++){
		if(*(path+i) == '/' && i != len-1) lcnt = 0;
		else name[lcnt++] = *(path+i);
	}
	name[lcnt] = '\0';
	char *my_path = (char *)pmm->alloc(200);
	strncpy(my_path,path,len-lcnt-1);
	if(my_path[0]!='/') my_path[0] = '/';
	printf("my_path = %s\n",my_path);
	inode_t *now = fs->ops->lookup(fs,my_path,0,0);
	return now->ops->rmdir(now,name);
}


static int vfs_link(const char *oldpath, const char *newpath){
	char dir[50];
	int lcnt = 0;
	int old = 0,new = 0;
	int ol = strlen(oldpath);
	int nl = strlen(newpath);
	for(int i=1;i<ol;i++){
		if(*(oldpath+i) == '/') break;
		else dir[lcnt++] = *(oldpath+i);
	}
	dir[lcnt] ='\0';
	if(strcmp(dir,"proc")==0) {/*printf("Invalid path(vfs_proc)\n"); return -1;*/ old = 1;}
	if(strcmp(dir,"dev")==0) {/*printf("Invalid path(vfs_dev)\n"); return -1;*/ old = 2;}

	lcnt = 0;
	for(int i=1;i<nl;i++){
		if(*(newpath+i) == '/') break;
		else dir[lcnt++] = *(newpath+i);
	}
	dir[lcnt] ='\0';
	if(strcmp(dir,"proc")==0) {/* printf("Invalid path(vfs_proc)\n"); return -1;*/ new = 1;}
	if(strcmp(dir,"dev")==0) {/* printf("Invalid path(vfs_dev)\n"); return -1;*/ new = 2;}

	if(new != old){
		printf("can't link files between different fs\n");
		return -1;
	}

	if(new !=0 || old != 0){
		printf("link fail\n");
		return -1;
	}

	filesystem_t *fs = &EXT2;
	inode_t *now = fs->ops->lookup(fs,oldpath,0,0);

	char name[50];
	int len = strlen(newpath);
	for(int i=0;i<len;i++){
		if(*(newpath+i) == '/' && i != len-1) lcnt = 0;
		else name[lcnt++] = *(newpath+i);
	}
	name[lcnt] = '\0';
	char *my_path = (char *)pmm->alloc(200);
	strncpy(my_path,newpath,len-lcnt-1);


	inode_t *fre = fs->ops->lookup(fs,my_path,0,0);
	
	return now->ops->link(name,now,fre);
}


static int vfs_unlink(const char *path){
	char dir[50];
	int lcnt = 0;
	int llen = strlen(path);
//	int nl = strlen(newpath);
	for(int i=1;i<llen;i++){
		if(*(path+i) == '/') break;
		else dir[lcnt++] = *(path+i);
	}
	dir[lcnt] ='\0';
	if(strcmp(dir,"proc")==0) { printf("Permission denied\n"); return -1;}
	if(strcmp(dir,"dev")==0) {printf("Permission denied\n"); return -1;}

	filesystem_t *fs = &EXT2;

	char name[50];
	int len = strlen(path);
	for(int i=0;i<len;i++){
		if(*(path+i) == '/' && i != len-1) lcnt = 0;
		else name[lcnt++] = *(path+i);
	}
	name[lcnt] = '\0';
	char *my_path = (char *)pmm->alloc(200);
	strncpy(my_path,path,len-lcnt-1);

	inode_t *now = fs->ops->lookup(fs,my_path,0,0);
	return now->ops->unlink(name,now);
}


static int vfs_open(const char *path, int flags){
	char dir[50];
	int lcnt = 0;
	int dlen = strlen(path);
	for(int i=1;i<dlen;i++){
		if(*(path+i) == '/') break;
		else dir[lcnt++] = *(path+i);
	}
	dir[lcnt] ='\0';
//	if(strcmp(dir,"proc")==0) { printf("Invalid path(vfs_proc)\n"); return -1;}
//	if(strcmp(dir,"dev")==0) {printf("Invalid path(vfs_dev)\n"); return -1;}

	if(strcmp(dir,"proc") == 0) {

		printf("Permission denied\n");
		return -1;
	}

	if(strcmp(dir,"dev") == 0) {
		int i;
		int label = 0;
		for(i=0;i<10;i++){
			if(mnt[i] != NULL && strncmp(&mnt[i]->mounton[1],"dev", 3) == 0)
			{
				
				printf("%s\n",mnt[i]->mounton);
				label=1;
				break;
			}
		}
		if(label == 0){
			printf("No such filesystem hahha\n");
			return -1;
		}
		if(mnt[i]->mounted == 0){
			printf("This filesystem has been unmounted\n");
			return -1;
		}
		inode_t *new = mnt[i]->ops->lookup(mnt[i],path,0,1);
		if(new == NULL) {printf("Unknown device\n"); return -1;}
		int rec = cpuisin[_cpu()];
		for(i=0;i<20;i++){
			if(tasks[rec]->fildes[i] == NULL) break;
		}
		file_t *fp = (file_t *)pmm->alloc(sizeof(file_t));
		fp->inode = new;
		fp->offset = 0;
		fp->type = 2;
		tasks[rec]->fildes[i] = fp;
		return i;
	}

	int i;
	filesystem_t *fs = &EXT2;
	inode_t *now = fs->ops->lookup(fs,path,flags,0);
	if(now == NULL) {printf("No such file\n"); return -1;}
	if(now->type == 1) {printf("Is is a directory\n"); return -1;}
        int rec = cpuisin[_cpu()];
	for(i=0;i<20;i++){
		if(tasks[rec]->fildes[i] == NULL) break;
	}
	file_t *fp = (file_t *)pmm->alloc(sizeof(file_t));
	now->ops->open(fp,flags);
	fp->inode = now;
	fp->offset = 0;
	fp->type = 1;
	tasks[rec]->fildes[i] = fp;
	return i;
}


static ssize_t vfs_read(int fd, void *buf, size_t nbyte){
	int rec = cpuisin[_cpu()];
//	printf("rec = %d\n",rec);
	file_t *fp = tasks[rec]->fildes[fd];
/*	if(fp == NULL){
		printf("wei shen me\n");
		while(1);
	}
*/	if(fp == NULL) {printf("Invalid file discriptor\n"); return 0;}
	if(fp->type == 3){

	}
	if(fp->type == 2){
		inode_t *now = fp->inode;
		return now->ops->read(fp,buf,nbyte);
	}
	inode_t *now = fp->inode;
	return now->ops->read(fp,buf,nbyte);
}


static ssize_t vfs_write(int fd, void *buf, size_t nbyte){
	int rec = cpuisin[_cpu()];
	file_t *fp = tasks[rec]->fildes[fd];
	if(fp == NULL) {printf("Invalid file discriptor\n"); return 0;}
	if(fp->type == 3){

	}
	if(fp->type == 2){
		inode_t *now = fp->inode;
		return now->ops->write(fp,buf,nbyte);
	}
	inode_t *now = fp->inode;
	return now->ops->write(fp,buf,nbyte);
}


static off_t vfs_lseek(int fd, off_t offset, int whence){
	int rec =cpuisin[_cpu()];
	file_t *fp = tasks[rec]->fildes[fd];
	if(fp->type == 2){
		printf("permission denied\n");
		return -1;
	}
	if(fp->type == 3){
	}
	inode_t *now = fp->inode;
	now->ops->lseek(fp,offset,whence);
	return 0;
}


static int vfs_close(int fd){
	int rec = cpuisin[_cpu()];
	file_t *temp = tasks[rec]->fildes[fd];
	inode_t *now = temp->inode;
	tasks[rec]->fildes[fd] = NULL;
	if(temp->type == 1)
	now->ops->close(temp);
	return 0;
}



MODULE_DEF(vfs){
	.init = vfs_init,
	.touch = vfs_touch,
	.edit = vfs_edit,
	.cat = vfs_cat,
	.rm = vfs_rm,
	.cd = vfs_cd,
	.ls = vfs_ls,
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

