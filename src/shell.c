#include <common.h>
#include <devices.h>
#include <klib.h>

void shell_thread(int tty_id) {
	char buf[128];
	strncpy(buf,"/dev/tty",8);
	buf[8] = '0' + tty_id;
	buf[9] = '\0';
	int stdin = vfs->open(buf,O_RDONLY);
	int stdout = vfs->open(buf,O_WRONLY);
	char name[5];
	strncpy(name,&buf[4],4);
	name[4] = '\0';
	device_t *tty = dev_lookup(name);
	char text[1024];
	char line[1024];
	int nread;
	while(1){
		if(nread!=0 && line[nread-1] == '\n'){
			line[nread-1] = '\0';
			tty_write(tty, 0, line, strlen(line));
		}
		else{
			sprintf(text,"(%s) $",name);
			tty_write(tty, 0, text, strlen(name)+5);
			nread = tty->ops->read(tty, 0, line, sizeof(line));
		}
	}
}








