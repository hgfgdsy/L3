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
//	printf(":::  H  :::\n");
	int stdin = vfs->open(buf,O_RDONLY);
//	printf(":::  H  :::\n");
//	int stdout = vfs->open(buf,O_WRONLY);
	char name[5];
	strncpy(name,&buf[5],4);
	name[4] = '\0';
	device_t *tty = dev_lookup(name);
	char text[1024];
	char line[1024];
	int nread=0;
	while(1){
		if(nread!=0){
			if(line[nread-1] == '\n'){
			        line[nread-1] = '\0';
			        tty_write(tty, 0, line, strlen(line));
				nread=0;
			}
		}
		else{
			sprintf(text,"(%s) $",name);
			tty_write(tty, 0, text, strlen(name)+5);
			printf("hello\n");
			nread = vfs->read(stdin, line, sizeof(line));
			printf("cao!!\n");
		}
	}
}








