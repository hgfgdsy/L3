#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>
#include <x86.h>

void panic(char *s){
	printf("%s\n",s);
	_halt(1);
}

struct task {
	int tag;
	const char *name;
	_Context context;
	char stack[4096];
};

struct spinlock {
	const char *name;
	int locked;
	int cpu;
};
struct semaphore {};

typedef struct my_handle {
	struct my_handle *pre;
	struct my_handle *suc;
	int seq;
	int event;
	handler_t handler;
} handle ;

handle *handle_head;

typedef struct my_cpu {
	int ncli;
	int INIF;
} MYCPU;

#endif
