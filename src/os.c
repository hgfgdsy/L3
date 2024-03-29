#include <common.h>
#include <klib.h>
#include <devices.h>

extern char initrd_start, initrd_end;
extern void shell_thread(void *ttyid);
/*
uintptr_t cha[500];
int cnt = 0;

extern void lock(intptr_t *lock);
extern void unlock(intptr_t *lock);

intptr_t sp;

uintptr_t allmem;
*/
int t1[1],t2[2],t3[1],t4[1];

handle *handle_head;

spinlock_t OR;

extern ssize_t tty_write(device_t *dev, off_t offset, const void *buf, size_t count);

void syr(void *name){
	while(1){
	char temp = *(char *)name;
	_putc(temp);
	_putc('f');
	_putc("01234567"[_cpu()]);
	_putc('\n');
	_yield();
	}
}
/*
void left(void *s) {
	while(1){
	kmt->sem_wait(&closehl);
	_putc(*(char *)s);
	kmt->sem_signal(&openhl);
	}
}

void right(void *s) {
	while(1) {
		kmt->sem_wait(&openhl);
		_putc(*(char *)s);
		kmt->sem_signal(&closehl);
	}
}
*/
extern int holding(spinlock_t *lk);


void echo_task(void *name) {
  device_t *tty = dev_lookup(name);
  while (1) {
    char line[128], text[128];
    sprintf(text, "(%s) $ ", name); tty_write(tty, 0, text ,strlen(name)+5);
    int nread = tty->ops->read(tty, 0, line, sizeof(line));
    line[nread - 1] = '\0';
    sprintf(text, "Echo: %s.\n", line); tty_write(tty, 0, text, strlen(line)+8);
  }
}

static void os_init() {
  afd = 0;
  memset(pos,0,sizeof(pos));
  for(int i=0;i<20;i++) CTD[i] = 0;
  pmm->init();
  handle_head = NULL;
  kmt->init();
/*  kmt->sem_init(&openhl,"sem1",0);
  kmt->sem_init(&closehl,"sem2",1);*/
  dev->init();
/*  printf("%x\n",-(intptr_t)&initrd_start + (intptr_t)&initrd_end);
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test1",syr,"1");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test2",syr,"2");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test3",syr,"3");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test4",syr,"4");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test5",syr,"5");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test6",syr,"6");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test7",syr,"7");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test8",syr,"8");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test9",syr,"9");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test10",syr,"10");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test11",syr,"11");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test12",syr,"12");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test13",syr,"13");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test14",syr,"14");
  kmt->create(pmm->alloc(sizeof(task_t)),"easy_test15",syr,"15");
*/



  kmt->spin_init((spinlock_t *)&OT,"locktrap");
  vfs->init();
  afd = 1;


//  kmt->create(pmm->alloc(sizeof(task_t)),"producer",left,"(");
//  kmt->create(pmm->alloc(sizeof(task_t)),"consumer",right,")");

/*
  kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty1");
  kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty2");
  kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty3");
  kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty4");
*/

  t1[0] = 1;
  t2[0] = 2;
  t3[0] = 3;
  t4[0] = 4;
  kmt->create(pmm->alloc(sizeof(task_t)), "print", shell_thread, t1);
  kmt->create(pmm->alloc(sizeof(task_t)), "print", shell_thread, t2);
  kmt->create(pmm->alloc(sizeof(task_t)), "print", shell_thread, t3);
  kmt->create(pmm->alloc(sizeof(task_t)), "print", shell_thread, t4);

//  handle *now = handle_head;
//  while(now!=NULL) {printf("%d\n",now->seq); now = now->suc;}

//  kmt->spin_init((spinlock_t *)&OR,"lockirq");
//  os->on_irq(0,_EVENT_NULL,hello);
/*  srand(uptime()+990);
  allmem = 0;
  sp = 0;
  for(int i =0 ;i<=499;i++) cha[i] = 0;*/
}

static void hello() {
  for (const char *ptr = "Hello from CPU #"; *ptr; ptr++) {
    _putc(*ptr);
  }
  _putc("12345678"[_cpu()]); _putc('\n');
}

static void os_run() {
  hello();
  _intr_write(1);

  while(1){
	  _yield();
  }

/*  while (1) {
  lock(&sp);
  int i;
  int fk = rand()%2+1;
  if(fk == 1){
	  if(cnt==498) {//printf("I'm full"); 
	  for(int i=0;i<=499;i++){
				  if(cha[i]!=0) pmm->free((void *)cha[i]);
			  }
			 // _yield();
          }
	  int rc = rand()%100 +1;
	  if(rc>=58){
		  int order1 = rand()%13+12;
		  int my_rand1 = rand()%(1<<(order1-1)) +1;
                  uintptr_t po = (uintptr_t)pmm->alloc((1<<order1)+my_rand1);
		  if(!po)// {printf("allmem is %x but po = %x\n",allmem,(1<<order1)+my_rand1);break;}
		  {
			  for(int i=0;i<=499;i++){
				  if(cha[i]!=0) pmm->free((void *)cha[i]);
			  }
		  }
		  allmem+=(1<<*(int*)((char *)po-12));
		  for(i=0;i<=499;i++) {
			  if(cha[i]==0) {
				  cha[i] = po;
				  cnt++;
				  break;
			  }
		  }
	  }
	  else{
		  int order2 = rand()%11+1;
		  int my_rand2 = rand()%(1<<(order2-1)) +1;
		  uintptr_t pi = (uintptr_t)pmm->alloc((1<<order2)+my_rand2);
		  if(!pi)// {printf("allmem is %x but pi = %x\n",allmem,(1<<order2)+my_rand2);break;}
		  {
                          for(int i=0;i<=499;i++){
				  if(cha[i]!=0) pmm->free((void *)cha[i]);
			  }
                  }
		  allmem+=(1<<*(int *)((char *)pi-12));
		  for(i=0;i<=499;i++) {
			  if(cha[i]==0) {
				  cha[i] = pi;
				  cnt++;
				  break;
			  }
		  }
	  }
  }
  else{
	  if(cnt == 0) pmm->free(NULL);
	  else {
		  for(int i=0;i<=499;i++) if(cha[i] != 0) {allmem-=(1<<*(int *)((char *)cha[i]-12));pmm->free((void *)cha[i]); cnt--;  cha[i] = 0; break;
		  }
	  }
  }
  unlock(&sp);
    _yield();
  }*/
}

static _Context *os_trap(_Event ev, _Context *context) {
  int label = 0;
  if(!holding((spinlock_t *)&OT)){label = 1;
  kmt->spin_lock((spinlock_t *)&OT);
  /*printf("CPU #%d locked\n",_cpu());*/}

  _Context *ret = NULL;
  handle *now = handle_head;
  while(now != NULL){
	  if(now->event == _EVENT_NULL || now->event == ev.event) {
		  _Context *next = now->handler(ev,context);
		  if (next!=NULL) ret = next;
	  }
	  now = now->suc;
  }
  if(label==1){
  kmt->spin_unlock((spinlock_t *)&OT);/*printf("CPU #%d unlocked\n",_cpu());*/}
  return ret;
}

static void os_on_irq(int seq, int event, handler_t handler) {
  if(handle_head == NULL) {
	  handle_head = (handle *)pmm->alloc(sizeof(handle));
	  handle_head -> pre = NULL;
	  handle_head -> suc = NULL;
	  handle_head -> seq = seq;
	  handle_head -> event = event;
	  handle_head -> handler = handler;
  }
  else {
	  handle *now = handle_head;
	  while(now -> suc != NULL && now->seq < seq) {
		  now = now -> suc;
	  }
	  if(now == handle_head) {
		  if(now->seq < seq) {
			  handle *tail = (handle *)pmm->alloc(sizeof(handle));
			  tail -> pre = now;
			  tail -> suc = NULL;
			  tail -> seq = seq;
			  tail -> event = event;
			  tail -> handler = handler;
			  now  -> suc = tail;
		  }
		  else {
		  handle_head = (handle *)pmm->alloc(sizeof(handle));
		  handle_head -> pre = NULL;
		  handle_head -> suc = now;
		  handle_head -> seq = seq;
		  handle_head -> event = event;
		  handle_head -> handler = handler;
		  now -> pre = handle_head;
		  }
	  }
	  else {
		  if(seq > now -> seq) {
			  handle *tail = (handle *)pmm->alloc(sizeof(handle));
			  now -> suc = tail;
			  tail -> pre = now;
			  tail -> suc = NULL;
			  tail -> seq = seq;
			  tail -> event = event;
			  tail -> handler = handler;
		  }
		  else {
			  handle *mid = (handle *)pmm->alloc(sizeof(handle));
			  mid -> pre = now -> pre;
			  mid -> suc = now;
			  mid -> seq = seq;
			  mid -> event = event;
			  mid -> handler = handler;
			  now -> pre -> suc = mid;
			  now -> pre = mid;
		  }
	  }
  }
}

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};
