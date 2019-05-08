#include <common.h>
#include <klib.h>


/*
uintptr_t cha[500];
int cnt = 0;

extern void lock(intptr_t *lock);
extern void unlock(intptr_t *lock);

intptr_t sp;

uintptr_t allmem;
*/


static void os_init() {
  pmm->init();
  handle_head = NULL;
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
  _Context *ret = NULL;
  handle *now = handle_head;
  while(now != NULL){
	  if(now->event == _EVENT_NULL || now->event == ev.event) {
		  _Context *next = now->handler(ev,context);
		  if (next) ret = next;
	  }
  }
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
		  handle_head = (handle *)pmm->alloc(sizeof(handle));
		  handle_head -> pre = NULL;
		  handle_head -> suc = now;
		  handle_head -> seq = seq;
		  handle_head -> event = event;
		  handle_head -> handler = handler;
		  now -> pre = handle_head;
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

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};
