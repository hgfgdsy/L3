#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>
#include <x86.h>

#define INT_MAX 2147483647
#define INT_MIN (-INT_MAX - 1)

typedef struct file file_t;


struct task {
	int tag;
	int incpu;
	const char *name;
	_Context context;
	file_t *fildes[20];
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

#define MODULE(name) \
  mod_##name##_t; \
  extern mod_##name##_t *name
#define MODULE_DEF(name) \
  extern mod_##name##_t __##name##_obj; \
  mod_##name##_t *name = &__##name##_obj; \
  mod_##name##_t __##name##_obj = 


typedef struct filesystem filesystem_t;


typedef struct {
  void (*init)();
  int (*access)(const char *path, int mode);
  int (*mount)(const char *path, filesystem_t *fs, char *name);
  int (*unmount)(const char *path);
  int (*mkdir)(filesystem_t *fs, const char *path, const char *name);
  int (*rmdir)(filesystem_t *fs, const char *path);
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
  void (*init)(struct filesystem *fs, const char *name, device_t *dev);
  inode_t *(*lookup)(struct filesystem *fs, const char *path, int flags, int from);
  int (*close)(inode_t *inode);
} fsops_t;



struct filesystem{
  char mounton[50];
  char name[50];
  int mounted;
  fsops_t *ops;
  device_t *dev;
};



typedef struct inodeops {
  int (*open)(file_t *file, int flags);
  int (*close)(file_t *file);
  ssize_t (*read)(file_t *file, char *buf, size_t size);
  ssize_t (*write)(file_t *file, const char *buf, size_t size);
  off_t (*lseek)(file_t *file, off_t offset, int whence);
  int (*mkdir)(inode_t *My, const char *name);
  int (*rmdir)(inode_t *My, const char *name);
  int (*link)(const char *name, inode_t *inode);
  int (*unlink)(const char *name);
  // 你可以自己设计readdir的功能
} inodeops_t;

struct inode {
	int size;
	int refcnt;
	void *ptr;
	int bid;
	int type;
	int self;
	filesystem_t *fs;
	inodeops_t *ops;
};

filesystem_t EXT2;
fsops_t ES;

filesystem_t *mnt[10];

inodeops_t basic;

inode_t root;


struct tory{
	int I;
	short rec_len;
	char name_len;
	char file_type;
};

typedef struct tory tory_t;

#define MAP (1<<12)
#define D (1<<16)

#endif
