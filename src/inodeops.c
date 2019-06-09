#include <common.h>
#include <klib.h>
#include <devices.h>

int i_open(file_t *file, int flags){
	inode_t *now = file->inode;
	device_t *mi = (device_t *)now->ptr;
	inode_t ask;
	mi->ops->read(mi,MAP+64*(now->self),(void *)&ask,sizeof(ask));
	ask.status += 1;
	mi->ops->write(mi,MAP+64*(now->self),(void *)&ask,sizeof(ask));
	return 0;
}



int i_close(file_t *file){
	inode_t *now = file->inode;
	device_t *mi = (device_t *)now->ptr;
	inode_t ask;
	mi->ops->read(mi,MAP+64*(now->self),(void *)&ask,sizeof(ask));
	ask.status -= 1;
	mi->ops->write(mi,MAP+64*(now->self),(void *)&ask,sizeof(ask));

	return 0;
}



ssize_t i_read(file_t *file, char *buf, size_t size){
	inode_t *now = file->inode;
	device_t *mi = (device_t *)now->ptr;
	int off = 0;
	if(file->type == 1){
		off = D + (now->bid)*(1<<12) + file->offset;
		if(size < now->size - file->offset){
			ssize_t rb =  mi->ops->read(mi, off, (void *)buf, size);
			file->offset += size;
			return rb;
		}
		int len = mi->ops->read(mi, off, (void *)buf, now->size - file->offset);
		file->offset = now->size;
		return len;
	}
	return mi->ops->read(mi, off, (void *)buf, size);
}



ssize_t i_write(file_t *file, const char *buf, size_t size){
	inode_t *now = file->inode;
	device_t *mi = (device_t *)now->ptr;
	int off = 0;
	if(file->type == 1){
		off = D + (now->bid)*(1<<12) + file->offset;
/*		if(size < (1<<12) - file->offset){
			ssize_t rb =  mi->ops->write(mi, off, (void *)buf, size);
			file->offset += size;
			return rb;
		}
		printf("The file is too big\n");
		int len = mi->ops->read(mi, off, (void *)buf, (1<<12) - file->offset);
		file->offset = now-;
		return len;*/
		mi->ops->write(mi,off,(void *)buf,size);
		int newsize = file->offset + size;
		if(newsize > now->size){
			inode_t ask;
			mi->ops->read(mi,MAP+64*(now->self),(void *)&ask,sizeof(ask));
			ask.size = newsize;
			mi->ops->write(mi,MAP+64*(now->self),(void *)&ask,sizeof(ask));
		}
		return size;
	}

	ssize_t nwrite = mi->ops->write(mi, file->offset, (void *)buf, size);
	file->offset+=size;
	return nwrite;

}



off_t i_lseek(file_t *file, off_t offset, int whence){
	inode_t *now = file->inode;
	if(whence == 0){
		file->offset = offset;
		return offset;
	}
	if(whence == 1){
		file->offset+=offset;
		return file->offset;
	}
	if(whence == 2){
		file->offset+=offset+now->size;
		return file->offset;
	}
	printf("Unknown whence\n");
	return -1;
}



int i_mkdir(inode_t *My, const char *name){
	device_t *mi = (device_t *)My->ptr;
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
	ap.name_len = nlen;
	mi->ops->write(mi, D + (My->bid)*(1<<12) + My->size,(void *)&ap,sizeof(ap));
	mi->ops->write(mi, D + (My->bid)*(1<<12) + My->size + sizeof(ap),dname,nlen+1);
	My -> size += ap.rec_len;
	My->son++;
	mi->ops->write(mi, MAP + (My->self)*64, (void *)My, sizeof(inode_t));
	if(My->self == 0)
		root.size = My->size;

	inode_t new;
	new.type = 1;
	new.size = 11;
	new.bid = k;
	new.self = k;
	new.ptr = (void *)mi;
	new.fs = My->fs;
	new.son = 0;
	new.ops = My->ops;
	mi->ops->write(mi, MAP + 64*k, (void *)&new, sizeof(inode_t));

	tory_t ddot;
	ddot.I = My->self;
	ddot.rec_len = 11;
	ddot.file_type = 1;
	ddot.name_len = 2;
	dname[0] ='.';
	dname[1] ='.';
	dname[2] ='\0';
	mi->ops->write(mi, D + k*(1<<12),(void *)&ddot,sizeof(ddot));
	mi->ops->write(mi, D + k*(1<<12)+ sizeof(ddot),dname,2+1);



	return 0;
}



int i_rmdir(inode_t *My, const char *name){
	device_t *mi = (device_t *)My->ptr;
	char data[1<<12];
	mi->ops->read(mi, D + (My->self)*(1<<12), (void *)data, (My->size));
	int rec = 0;
	int I;
	int label = 0;
	int lcnt = strlen(name);
	tory_t ap;
	while(rec < My->size){
		I = *(int *)&data[rec];
		if((int)data[rec+6] != lcnt || I == -1){
			rec += *(short *)&data[rec+4];
			continue;
		}
		else{
			if(strncmp(name,&data[rec+8],lcnt) != 0){
			        rec += *(short *)&data[rec+4];
			        continue;
			}
			else{
				label = 1;
				break;
			}
		}
	}
	if(label == 0){
		printf("No such directory\n");
		return -1;
	}
	else{
		mi->ops->read(mi, D + (My->self)*(1<<12) + rec, (void *)&ap, (sizeof(ap)));
		if(ap.I == -1){
			printf("This directory has been deleted\n");
			return -1;
		}
		inode_t ta;
		mi->ops->read(mi, MAP + 64*ap.I, (void *)&ta, sizeof(ta));
		if(ta.son != 0){
			printf("It is not a empty directory\n");
			return -1;
		}
		char c[1] = "0";
		mi->ops->write(mi, 8*ap.I, (void *)c, 1);
		ap.I = -1;
		mi->ops->write(mi, D + (My->self)*(1<<12) + rec, (void *)&ap, (sizeof(ap)));
		My->son--;
	}
	return 0;
}



int i_link(const char *name, inode_t *inode, inode_t *new){
	if(inode == NULL){
		printf("The target file does not exist\n");
		return -1;
	}
	if(new == NULL){
		printf("The origin dirrectory does not exist\n");
		return -1;
	}
	int I = inode->self;
	int nlen = strlen(name);
	char dname[50];
	strcpy(dname,name);
	dname[nlen] = '\0';
	tory_t ap;
	ap.I = I;
	ap.rec_len = nlen + 9;
	ap.file_type = 2;
	ap.name_len = nlen;
	device_t *mi = (device_t *)new->ptr;
	mi->ops->write(mi,D + (new->bid)*(1<<12) + new->size, (void *)&ap, sizeof(ap));
	mi->ops->write(mi,D + (new->bid)*(1<<12) + new->size + sizeof(ap), dname, nlen+1);
	new->size += ap.rec_len;
	new->son++;
	mi->ops->write(mi,MAP + (new->self)*64, (void *)new, sizeof(inode_t));
	if(new->self == 0) root.size+=ap.rec_len;

	inode->refcnt += 1;
	mi->ops->write(mi,MAP + (inode->self)*64, (void *)inode, sizeof(inode_t));
	return 0;
}



int i_unlink(const char *name, inode_t *inode){
	if(inode == NULL) {
		printf("Invalid path\n");
		return -1;
	}

	device_t *mi = (device_t *)inode->ptr;
	char data[1<<12];
	int dlen = mi->ops->read(mi,D + (inode->self)*(1<<12), (void *)data, inode->size);
	int rec = 0;
	int label = 0;
	char dir[50];
	int I;
	int lcnt = strlen(name);
	strcpy(dir,name);
	dir[lcnt] = '\0';
	while(rec < dlen){
		I = *(int *)&data[rec];
		if((int)data[rec+6] != lcnt || I == -1){
			rec += *(short *)&data[rec+4];
			continue;
		}
		else{
			if(strncmp(dir,&data[rec+8],lcnt) != 0){
			        rec += *(short *)&data[rec+4];
			        continue;
			}
			else{
				label = 1;
				break;
			}
		}
	}
	if(label == 0){
		printf("No such file\n");
		return -1;
	}
	tory_t ap;
	memcpy((void *)&ap,(void *)&data[rec],sizeof(tory_t));
	ap.I = -1;
	mi->ops->write(mi , D + (inode->bid)*(1<<12) + rec, (void *)&ap, sizeof(tory_t));
	inode_t tar;
	mi->ops->read(mi, MAP + I*64, (void *)&tar, sizeof(inode_t));
	if(tar.refcnt == 1 && tar.status == 0){
		char c[1] = "0";
		mi->ops->write(mi, I*8, (void *)c, 1);
		inode->son--;
		mi->ops->write(mi,MAP+64*(inode->self),inode,sizeof(inode_t));
	}
	else{
		tar.refcnt--;
		mi->ops->write(mi, MAP + I*64, (void *)&tar, sizeof(inode_t));
	}
	return 0;
}
