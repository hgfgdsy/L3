#include <common.h>
#include <klib.h>
#define Mars 30
#define moons 12

typedef struct Node{
	struct Node *llink;
	int tag;
	int kval;
	struct Node *rlink;
}node;

typedef struct HeadNode{
	int nodesize;
	node *first;
}Freelist;

Freelist *avail;
Freelist *a0;
Freelist *a1;
Freelist *a2;
Freelist *a3;


/*int my_pow(int m,int x){
	int ret = 1;
	while(x--) ret*=m;
	return ret;
}*/

static uintptr_t pm_start, pm_end;

void cli(){asm volatile ("cli");}
void sti(){asm volatile ("sti");}


intptr_t spinlock;
int scale = sizeof(node);
int cnt=0;

void lock(intptr_t *lock){ cli();while(_atomic_xchg(lock,1));}
void unlock(intptr_t *lock){ _atomic_xchg(lock,0);sti();}

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  printf("%x\n",pm_start);
  printf("%x\n",pm_end);
  if(pm_start > pm_end)
  printf("heap(up to down) = %x\n",pm_start - pm_end);
  else
  printf("heap(down to up) = %x\n",pm_end - pm_start);
  spinlock = 0;

  //init list
  uintptr_t space = pm_end - pm_start;
  uintptr_t my_start = pm_start;
  space -= (1<<15);
  avail = (Freelist *)my_start;
  my_start += (1<<12);
  a0 = (Freelist *)my_start;
  my_start += (1<<12);
  a1 = (Freelist *)my_start;
  my_start += (1<<12);
  a2 = (Freelist *)my_start;
  my_start += (1<<12);
  a3 = (Freelist *)my_start;
  my_start += (1<<14);
  int i;
  for(i = 0;i <= Mars;i++){
	  avail[i].nodesize = (1<<i);
	  if((space>>i)&1) {
		  node *r = (node *)my_start;
		  avail[i].first = r;
		  my_start += avail[i].nodesize;
		  r->rlink = NULL;
		  r -> llink = r;
		  r -> tag =0;
		  r -> kval = i;
	  }
	  else avail[i].first = NULL;
  }
}

void *Bigloc(size_t size) {
  int k,i;
  node *pa,*suc,*pi;

  for(k = 0; k <= Mars &&(avail[k].nodesize < size || !avail[k].first); k++);
  printf("k=%d\n",k);

  if(k>Mars) return NULL;
  else
  {
	  pa = avail[k].first;
	  suc = pa->rlink;
	  
	  if(suc == NULL) avail[k].first = NULL;
	  else
	  {
		  avail[k].first = suc;
		  suc->llink = suc;
	  }

	  for(i=1;k-i>=0 && avail[k-i].nodesize >= size;i++){
		  pi = (node *)((uintptr_t)pa + (1<<(k-i)));
		  pi -> llink = pi;
		  pi -> rlink = NULL;
		  pi -> tag = 0;
		  pi -> kval = k - i;
		  avail[k-i].first = pi;
	  }
	  pa -> tag = 1;
	  pa -> kval = k - (--i);
  }
  return pa+scale;
}


static void *kalloc(size_t size) {
  void *temp;
  lock(&spinlock);
  cnt++;
  printf("This is "%d" request\n",cnt);
  temp = Bigloc(size+scale);
  unlock(&spinlock);
  return temp;
}

static void kfree(void *ptr) {
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
