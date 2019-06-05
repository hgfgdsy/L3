#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>
#include <x86.h>

#define INT_MAX 2147483647
#define INT_MIN (-INT_MAX - 1)

struct task {
	int tag;
	int incpu;
	const char *name;
	_Context context;
	char stack[4096];
};

struct spinlock {
	const char *name;
	int locked;
	int cpu;
};
struct semaphore {
	const char *name;
	int count;
	spinlock_t lock;
	int tks[20];
};

typedef struct my_handle {
	struct my_handle *pre;
	struct my_handle *suc;
	int seq;
	int event;
	handler_t handler;
} handle ;

typedef struct my_cpu {
	int ncli;
	unsigned int INIF;
} MYCPU;

task_t *current[8];

spinlock_t OT;

typedef struct {
  void (*init)();
  int (*access)(const char *path, int mode);
  int (*mount)(const char *path, filesystem_t *fs);
  int (*unmount)(const char *path);
  int (*mkdir)(const char *path);
  int (*rmdir)(const char *path);
  int (*link)(const char *oldpath, const char *newpath);
  int (*unlink)(const char *path);
  int (*open)(const char *path, int flags);
  ssize_t (*read)(int fd, void *buf, size_t nbyte);
  ssize_t (*write)(int fd, void *buf, size_t nbyte);
  off_t (*lseek)(int fd, off_t offset, int whence);
  int (*close)(int fd);
} MODULE(vfs);


#endif
