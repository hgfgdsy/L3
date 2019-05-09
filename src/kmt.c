#include <klib.h>
#include <common.h>

void panic(char *s){
	printf("%s\n",s);
	_halt(1);
}

task_t *tasks[20];
int tagging[20];
MYCPU mycpu[20];

_Context init_tasks[8];
int osruntk[8];

static _Context *kmt_context_save(_Event ev, _Context *context) {
	if(osruntk[_cpu()] == 0) {
		memcpy((void *)&init_tasks[_cpu()],(void *)context, sizeof(_Context));
	}
	else
	        memcpy((void *)&tasks[current[_cpu()] -> tag] -> context,(void *)context, sizeof(_Context));
	return NULL;
}

static _Context *kmt_context_switch(_Event ev, _Context *context) {
	int cur_rec = -1;
	if(osruntk[_cpu()] == 0) {
		for(int i = 0; i < 20; i++) {
			if(tagging[i] != -1 && tasks[i] -> incpu == -1) {
				cur_rec = i;
				break;
			}
		}
	}
	else {
	for(int i = current -> tag+1; i < 20; i++) {
		if(tagging[i] != -1 && tasks[i] -> incpu == -1) {
			cur_rec = i;
			break;
		}
	}
	if(cur_rec == -1) {
		for(int i = 0 ;i <= current->tag-1; i++) {
			if(tagging[i] != -1 && tasks[i] -> incpu == -1) {
				cur_rec = i;
				break;
			}
		}
	}}
	if(cur_rec == -1) {
		if(osruntk[_cpu()] == 0) {
			return (_Context *)&init_tasks[_cpu()];
		}
		else {
			return (_Context *)&current[_cpu()] -> context;
		}
	}
	else {
		if(osruntk[_cpu()] == 0) {
			osruntk[_cpu()] = 1;
			tasks[cur_rec] -> incpu = _cpu();
			current[_cpu()] = tasks[cur_rec];
			return (_Context *)&current -> context;
		}
		else {
	                tasks[current[_cpu()] -> tag] -> incpu = -1;
	                tasks[cur_rec] -> incpu = _cpu();
	                current = tasks[cur_rec];
	                return (_Context *)&current -> context;
		}
}
	

static void kmt_init(){
	for(int i=0;i<8;i++) { osruntk[i] = 0;}
	os->on_irq(INT_MIN, _EVENT_NULL, kmt_context_save);
	os->on_irq(INT_MAX, _EVENT_NULL, kmt_context_switch);
	for(int i=0;i<20;i++) { tagging[i] = -1; tasks[i] = NULL; 
		                mycpu[i].ncli = 0; mycpu[i].INIF = 1;
	}

}




static int kmt_create(task_t *task, const char *name, 
		void (*entry)(void *arg), void *arg){
	int rec=0;
	for(int i = 0 ;i < 20 ;i++) {
		if(tagging[i] == -1) {
			rec = i;
			break;
		}
	}
	tagging[rec] = rec;
	task->incpu = -1;
	task->tag = rec;
	task->name = name;
	tasks[rec] = task;
	_Area stack = (_Area){task->stack,task->stack + sizeof(task->stack)};
	task->context = *_kcontext(stack,entry,arg);
	return rec;
}



static void kmt_teardown(task_t *task){
	tagging[task->tag] = -1;
	tasks[task->tag] = NULL;
	pmm->free((void *)task);
}


static void kmt_spin_init(spinlock_t *lk,const char *name){
	lk->name = name;
	lk->locked = 0;
	lk->cpu = -1;
}

void pushcli() {
	int eflags = get_efl();
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
	if(mycpu[_cpu()].ncli == 0 && mycpu[_cpu()].INIF)
		sti();
}

int holding(spinlock_t *lk) {
	int r;
	pushcli();
	r = lk -> locked & ((lk -> cpu) == _cpu());
	popcli();
	return r;
}


static void kmt_spin_lock(spinlock_t *lk){
	pushcli();
	if(holding(lk))
		panic("have required");
	while(_atomic_xchg(&lk->locked,1));
	lk -> cpu = _cpu();
}


static void kmt_spin_unlock(spinlock_t *lk){
	if(holding(lk)) panic("have required");
	lk -> cpu = -1;
        _atomic_xchg(&lk->locked,0);
	popcli();
}



static void kmt_sem_init(sem_t *sem, const char *name, int value){

}




static void kmt_sem_wait(sem_t *sem){

}




static void kmt_sem_signal(sem_t *sem){

}











MODULE_DEF(kmt){
	.init = kmt_init,
	.create = kmt_create,
	.teardown = kmt_teardown,
	.spin_init = kmt_spin_init,
	.spin_lock = kmt_spin_lock,
	.spin_unlock = kmt_spin_unlock,
	.sem_init = kmt_sem_init,
	.sem_wait = kmt_sem_wait,
	.sem_signal = kmt_sem_signal,
};
