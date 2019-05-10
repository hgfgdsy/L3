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
struct semaphore {};

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

void panic(char *s){
	printf("%s\n",s);
	_halt(1);
}

void pushcli() {
	int eflags;

	eflags = get_efl();
	cli();
	if(mycpu[_cpu()].ncli == 0)
		mycpu[_cpu()].INIF = eflags & FL_IF;
	mycpu[_cpu()].ncli += 1;
}

void popcli() {
	if(get_efl() & FL_IF)
		panic("popcli while If = 1\n");
	if(--mycpu[_cpu()].ncli < 0)
		panic("ncli less than 0\n");
	if(mycpu[_cpu()].ncli == 0 && mycpu[_cpu()].INIF != 0)
		sti();
}

int holding(spinlock_t *lk) {
	int r;
	pushcli();
	r = lk -> locked && ((lk -> cpu) == _cpu());
	popcli();
	return r;
}


#endif
