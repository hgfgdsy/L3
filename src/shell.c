#include <common.h>
#include <devices.h>
#include <klib.h>
extern ssize_t tty_write(device_t *dev, off_t offset, const void *buf, size_t count);
void shell_thread(void *ttyid) {
	int tty_id = *(int *)ttyid;
	char buf[32];
	strncpy(buf,"/dev/tty",8);
	buf[8] = '0' + tty_id;
	buf[9] = '\0';
	int stdin = vfs->open(buf,O_RDONLY);
	int stdout = vfs->open(buf,O_WRONLY);
	char name[5];
	strncpy(name,&buf[5],4);
	name[4] = '\0';
//	device_t *tty = dev_lookup(name);
	char text[32];
	char line[128];
	int nread=0;
	char path[128];
	memset(path,0,sizeof(path));
	path[0] = '/';
	path[1] = '\0';
	path[2] = 'b';
	path[3] = 'c';
	char hal[1] = "$";
	while(1){
		if(nread != 0){
			if(line[nread-1] == '\n'){
			        line[nread] = '\0';
				char cmd[50];
				int lcnt=0;
				for(int i=0;i<nread-1;i++){
					if(line[i] == ' ') break;
					cmd[lcnt++] = line[i];
				}
				cmd[lcnt] = '\0';
				if(strcmp(cmd,"ls") == 0){
					vfs->ls(path,stdout);
				}
				if(strcmp(cmd,"cd") == 0){
					vfs->cd(&line[3],path,stdout);
					printf("path = %d\n",strlen(path));
				}
				if(strcmp(cmd,"mkdir") == 0){
					char np[256];
					memset(np,0,sizeof(np));
					strcpy(np,path);
					int olen = strlen(path);
					np[olen++] = '/';
					for(int j=6;j<nread-1;j++){
						if(line[j]!=' ')
							np[olen++] = line[j];
					}
					vfs->mkdir(np);
				}
				if(strcmp(cmd,"rm") == 0){
					char np[256];
					memset(np,0,sizeof(np));
					strcpy(np,path);
					int olen = strlen(path);
					np[olen++] = '/';
					for(int j=3;j<nread-1;j++){
						if(line[j]!=' ')
							np[olen++] = line[j];
					}
					vfs->rm(np,stdout);
				}
				if(strcmp(cmd,"cat") == 0){
					char np[256];
					memset(np,0,sizeof(np));
					strcpy(np,path);
					int olen = strlen(path);
					np[olen++] = '/';
					for(int j=4;j<nread-1;j++){
						if(line[j]!=' ')
							np[olen++] = line[j];
					}
					vfs->cat(np,stdout);
				}
				if(strcmp(cmd,"edit") == 0){
					char np[256];
					memset(np,0,sizeof(np));
					strcpy(np,path);
					int olen = strlen(path);
					np[olen++] = '/';
					int j;
					for(j=5;line[j]==' ';j++);
					int k;
					for(k = j;k < nread-1;k++){
						if(line[k]==' ') break;
						np[olen++] = line[k];
					}
					for(k=k+1 ;line[k]==' ';k++);
					int rcnt=0;
					char buf[256];
					for(k=k+1;line[k]!='"';k++){
						buf[rcnt++] = line[k];
					}
					buf[rcnt] = '\0';
					vfs->edit(np,buf,stdout);
				}

				nread=0;
			}
		}
		else{
			sprintf(text,"(%s) $ ",name);
			vfs->write(stdout, text, strlen(name)+5);
			vfs->write(stdout, path, strlen(path));
			vfs->write(stdout, hal, 1);
			nread = vfs->read(stdin, line, sizeof(line));
		}
	}
}








