#include <common.h>
#include <klib.h>

static void os_init() {
  pmm->init();
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

  while (1) {
  int my_rand = rand()%(1<<18)+1;
  printf("my_rand = %x\n",my_rand);
  uintptr_t po = (uintptr_t)pmm->alloc(0x10000+my_rand);
  
  printf("%x\n",po);
//  pmm->free((void *)po);
  if(!po) break;
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
