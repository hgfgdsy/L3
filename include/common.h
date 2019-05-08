#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>

struct task {};
struct spinlock {};
struct semaphore {};

typedef struct handle {
	struct handle *pre;
	struct handle *suc;
	int seq;
	int event;
	handler_t handler;
};

handle *handle_head;

#endif
