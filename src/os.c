#include <common.h>
#include <klib.h>

uintptr_t allmem;

static void os_init() {
  pmm->init();
  srand(uptime()+3);
  allmem = 0;
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
  int cnt = 0;
  uintptr_t cha[500];
  for(int i=0;i<=499;i++) cha[i] = 0;

  while (1) {
  int i;
  int fk = rand()%2+1;
  if(fk == 1){
	  if(cnt==498) {printf("I'm full"); break;}
	  int rc = rand()%100 +1;
	  if(rc>=98){
		  int order1 = rand()%13+12;
		  int my_rand1 = rand()%(1<<(order1-1)) +1;
                  uintptr_t po = (uintptr_t)pmm->alloc((1<<order1)+my_rand1);
		  if(!po) {printf("allmem is %x but po = %x\n",allmem,(1<<order1)+my_rand1);break;}
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
//		  allmem+=(1<<order2)+my_rand2;
		  uintptr_t pi = (uintptr_t)pmm->alloc((1<<order2)+my_rand2);
		  if(!pi) {printf("allmem is %x but pi = %x\n",allmem,(1<<order2)+my_rand2);break;}
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
    _yield();
  }
}

static _Context *os_trap(_Event ev, _Context *context) {
  return context;
}

static void os_on_irq(int seq, int event, handler_t handler) {
}

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};
