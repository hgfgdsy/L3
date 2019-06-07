#include <common.h>
#include <klib.h>
#include <devices.h>

int i_open(file_t *file, int flags){
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



int i_mkdir(inode_t *My, const char *name){
	devide_t *mi = (devide_t *)My->ptr;
	int nlen = strlen(name);
	char omit[1];
	int k;
	for(k = 0; k < 960; k++){
		mi->ops->read(mi,k,omit,1);
		if(omit[0] == 0) break;
	}
	omit[0] = '1';
	mi->ops->write(mi,k,omit,1);
	char dname[nlen+1];
	strncpy(dname,name,nlen);
	dname[nlen] = '\0';
	tory_t ap;
	ap.I = k;
	ap.rec_len = nlen + 9;
	ap.file_type = 1;
	ap.namelen = nlen+1;
	mi->ops->write(mi, D + (My->bid)*(1<<12) + My->size,(void *)&ap,sizeof(ap));
	mi->ops->write(mi, D + (My->bid)*(1<<12) + My->size + sizeof(ap),dname,nlen+1);
	My -> size += ap.rec_len;



	inode_t new;
	new.type = 1;
	new.size = 0;
	new.bid = k;
	new.self = k;
	new.ptr = (void *)mi;
	new.fs = My->fs;
	new.ops = My->ops;
	mi->ops->write(mi, MAP + 64*k, (void *)&new, sizeof(inode_t));


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
