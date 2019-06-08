#include <common.h>
#include <klib.h>
#include <devices.h>


inode_t *cref(inode_t *My, const char *name,int right){
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
	ap.file_type = 2;
	ap.name_len = nlen;
	mi->ops->write(mi, D + (My->bid)*(1<<12) + My->size,(void *)&ap,sizeof(ap));
	mi->ops->write(mi, D + (My->bid)*(1<<12) + My->size + sizeof(ap),dname,nlen+1);
	My -> size += ap.rec_len;
	My->son++;
	mi->ops->write(mi, MAP + (My->self)*64, (void *)My, sizeof(inode_t));
	if(My->self == 0)
		root.size = My->size;

	inode_t *new = (inode_t *)pmm->alloc(sizeof(inode_t));
	new->refcnt = 1;
	new->type = 2;
	new->size = 0;
	new->bid = k;
	new->self = k;
	new->ptr = (void *)mi;
	new->fs = My->fs;
	new->son = 0;
	new->ops = My->ops;
	new->right = right;
	new->status = 0;
	mi->ops->write(mi, MAP + 64*k, (void *)new, sizeof(inode_t));
/*
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
*/	return new;
}




void f_init(struct filesystem *fs, const char *name, device_t *dev){
	fs->dev = dev;
	strcpy(fs->name,name);
}



inode_t *f_lookup(struct filesystem *fs, const char *path, int flags, int from){
	if(from == 0){
		int len = strlen(path);
		char dir[50];
		int lcnt = 0;
		int rec = 0;
		char data[1<<12];
		int I;
		int dlen = fs->dev->ops->read(fs->dev, D, (void *)data, root.size);
//		printf("dlen = %d, next_size = %d\n",dlen,root.size);
		inode_t *next;
		next = (inode_t *)pmm->alloc(sizeof(inode_t));
		memcpy(next,&root,sizeof(inode_t));
		for(int i = 1; i < len; i++){
			if(*(path+i) != '/')
				dir[lcnt++] = *(path+i);
			else{
				dir[lcnt] = '\0';
				rec = 0;
				int label = 0;
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
				if(label == 1){
					fs->dev->ops->read(fs->dev,(MAP)+(I*64),(void *)next,sizeof(inode_t));
					dlen = fs->dev->ops->read(fs->dev,(D)+((next->bid)*(1<<12)),(void *)data,next->size);
//					printf("dlen = %d, next_size = %d\n",dlen,next->size);
					lcnt = 0;
				}
				else {
					printf("Invalid path1!\n");
					return NULL;
				}
			}
		}
		if(lcnt == 0) return next;
		else{
//			printf("rts4 = %d\n",root.size);
              		dir[lcnt] = '\0';
//			printf("%s\n",dir);
			rec = 0;
			int label = 0;
//			printf("%d\n",dlen);
			while(rec < dlen){
				I = *(int *)&data[rec];
				if((int)data[rec+6] != lcnt || I == -1 || data[rec+7] == '1'){
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
			if(label == 1){
				fs->dev->ops->read(fs->dev,(MAP)+I*64,(void *)next,sizeof(inode_t));
//				dlen = fs->dev->ops->read(fs->dev,(D)+(next->bid*(1<<12)),(void *)data,next->size);
				return next;
			}
			else{
				if(flags != 0) {
					return cref(next,dir,flags);
				}
				else {
				        printf("Invalid path2!\n");
				        return NULL;
				}
			}
		}
	}
	if(from == 1){
		inode_t *ret = (inode_t *)pmm->alloc(sizeof(inode_t));
		ret->ptr = NULL;
		char devname[20];
		int dcnt=0;
		int plen = strlen(path);
		for(int i=5;i<plen;i++){
			if(*(path+i) == '/'){
				printf("Unknown device\n");
				return NULL;
			}
			devname[dcnt++] = *(path+i);
		}
		devname[dcnt] = '\0';
		ret->ptr = (void *)dev_lookup(devname);
		ret->ops = &basic;
		ret->type = 4;
		if(ret->ptr==NULL) {printf("Unknown device\n"); return NULL;}
		return ret;
	}


	return NULL;
}



int f_close(inode_t *inode){
	pmm->free((void *)inode);
	return 0;
}


