#include <common.h>
#include <devices.h>
#include <klib.h>
extern ssize_t tty_write(device_t *dev, off_t offset, const void *buf, size_t count);
void shell_thread(void *ttyid) {
	int tty_id = *(int *)ttyid;
	char buf[128];
	strncpy(buf,"/dev/tty",8);
	buf[8] = '0' + tty_id;
	buf[9] = '\0';
	int stdin = vfs->open(buf,O_RDONLY);
	int stdout = vfs->open(buf,O_WRONLY);
	char name[5];
	strncpy(name,&buf[5],4);
	name[4] = '\0';
//	device_t *tty = dev_lookup(name);
	char text[1024];
	char line[1024];
	int nread=0;
	char path[1024];
	memset(path,0,1024);
	path[0] = '/';
	while(1){
		if(nread!=0){
			if(line[nread-1] == '\n'){
			        line[nread] = '\0';
//			        vfs->write(stdout, line, strlen(line));
				char cmd[50];
				int lcnt=0;
				for(int i=0;i<nread;i++){
					if(line[i] == ' ') break;
					cmd[lcnt++] = line[i];
				}
				cmd[lcnt] = '\0';
				if(strcmp(cmd,"ls") == 0){
					vfs->ls(path,stdout);
				}
				
				nread=0;
			}
		}
		else{
			sprintf(text,"(%s) $",name);
			vfs->write(stdout, text, strlen(name)+5);
			vfs->write(stdout, path, strlen(path));
			nread = vfs->read(stdin, line, sizeof(line));
		}
	}
}








