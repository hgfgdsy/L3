#include <common.h>
#include <klib.h>
#include <devices.h>

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
		inode_t *next;
		next = (inode_t *)pmm->alloc(sizeof(inode_t));
		next = &root;
		for(int i = 1; i < len; i++){
			if(*(path+i) != '/')
				dir[lcnt++] = *(path+i);
			else{
				dir[lcnt] = '\0';
				rec = 0;
				int label = 0;
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
					fs->dev->ops->read(fs->dev,(MAP)+(I*64),(void *)next,sizeof(inode_t));
					dlen = fs->dev->ops->read(fs->dev,(D)+((next->bid)*(1<<12)),(void *)data,next->size);
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
              		dir[lcnt] = '\0';
			rec = 0;
			int label = 0;
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
				dlen = fs->dev->ops->read(fs->dev,(D)+(next->bid*(1<<12)),(void *)data,next->size);
				return next;
			}
			else{
				printf("Invalid path2!\n");
				return NULL;
			}
		}
	}





	return NULL;
}



int f_close(inode_t *inode){
	return 0;
}


