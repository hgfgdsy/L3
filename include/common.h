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

typedef struct inode inode_t;

struct file {
	int refcnt;
	inode_t *inode;
	uint64_t offset;
};



typedef struct fsops {
  void (*init)(struct filesystem *fs, const char *name, dev_t *dev);
  inode_t *(*lookup)(struct filesystem *fs, const char *path, int flags);
  int (*close)(inode_t *inode);
} fsops_t;



struct filesystem{
  fsops_t *ops;
  dev_t *dev;
};

typedef struct filesystem filesystem_t;


typedef struct inodeops {
  int (*open)(file_t *file, int flags);
  int (*close)(file_t *file);
  ssize_t (*read)(file_t *file, char *buf, size_t size);
  ssize_t (*write)(file_t *file, const char *buf, size_t size);
  off_t (*lseek)(file_t *file, off_t offset, int whence);
  int (*mkdir)(const char *name);
  int (*rmdir)(const char *name);
  int (*link)(const char *name, inode_t *inode);
  int (*unlink)(const char *name);
  // 你可以自己设计readdir的功能
} inodeops_t;

struct inode {
	int refcnt;
	void *ptr;
	filesystem_t *fs;
	inodeops_t *ops;
}


#endif
