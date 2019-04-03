#include <common.h>
#include <klib.h>
#define Mars 30
#define moons 12

typedef struct Node{
	struct Node *llink;
	int tag;
	int kval;
	int order;
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
int *OFFSET;


uintptr_t my_start,my_start1;

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
  my_start1 = pm_start;

  space -= (1<<15);
  avail = (Freelist *)my_start1;
  my_start1 += (1<<12);
  a0 = (Freelist *)my_start1;
  my_start1 += (1<<12);
  a1 = (Freelist *)my_start1;
  my_start1 += (1<<12);
  a2 = (Freelist *)my_start1;
  my_start1 += (1<<12);
  a3 = (Freelist *)my_start1;
  my_start1 += (1<<12);
  OFFSET = (int *)my_start1;
  my_start1 += (1<<12) + (1<<13);
  my_start = my_start1;
  int i;
  OFFSET[0] = 0;
  for(i = 0;i <= Mars;i++){
	  avail[i].nodesize = (1<<i);
	  OFFSET[i+1] = OFFSET[i];
	  if((space>>i)&1) {
		  node *r = (node *)my_start1;
		  avail[i].first = r;
		  my_start1 += avail[i].nodesize;
		  r->rlink = NULL;
		  r -> llink = r;
		  r -> tag =0;
		  r -> order = r -> kval = i;
		  OFFSET[i+1] += (1<<i);
	  }
	  else avail[i].first = NULL;
  }

  printf("space = %x\n",space);
}


node *my_buddy(node *p) {
  int s= (int)((uintptr_t)p - my_start - OFFSET[p->kval]);
  printf("processed p = %x\n",s);
  int m = (1<<(p->kval));
  int n = (1<<((p->kval)+1));
  
  if(s%n == 0) return (node *)((uintptr_t)p+(uintptr_t)m);
  if(s%n == m) return (node *)((uintptr_t)p-(uintptr_t)m);
  printf("bad address = %x,because s = %x and n= %x\n",(uintptr_t)p,s,n);
  while(1);
  return NULL;
} 

void *Bigloc(size_t size) {
  int k,i;
  node *pa,*suc,*pi;

  for(k = 0; k <= Mars &&(avail[k].nodesize < size || !avail[k].first); k++);

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
		  pi -> order = pa -> order;
		  pi -> llink = pi;
		  pi -> rlink = NULL;
		  pi -> tag = 0;
		  pi -> kval = k - i;
		  avail[k-i].first = pi;
	  }
	  pa -> tag = 1;
	  pa -> kval = k - (--i);
  }
  return (void *)pa+scale;
}

void release(node *p) {
  printf("freep->kval = %d\n",p->kval);
  node *s = my_buddy(p);
  printf("init s =%x\n",s);
  while((uintptr_t)s>=my_start && (uintptr_t)s<=pm_end && s->tag==0 && s->kval==p->kval)
  {
	  if(s->llink == s && s->rlink == NULL) avail[s->kval].first = NULL;
	  else
	  {
		  if(s->llink == s){
			  s->rlink->llink = s->rlink;
			  avail[s->kval].first = s->rlink;
		  }
		  else{
			  s->llink->rlink = s->rlink;
			  s->rlink->llink = s->llink;
		  }
	  }
	  if(((uintptr_t)p - my_start - OFFSET[p->kval])%(1<<((p->kval)+1)) == 0) p->kval++;
	  else
	  {
		  s->kval = p->kval +1;
		  p = s;
	  }
	  printf("s = %x\n",(uintptr_t)s);
	  printf("p->kval = %d\n",p->kval);
	  s = my_buddy(p);
  }
//  printf("free|p = %x",(uintptr_t)p);
  p -> tag = 0;
  if(avail[p->kval].first==NULL) {
	  avail[p->kval].first = p;
	  p->llink = p;
	  p->rlink = NULL;
  }
  else
  {
	  p->rlink = avail[p->kval].first;
	  p->rlink->llink = p;
	  p->llink = p;
	  avail[p->kval].first = p;
  }
}


static void *kalloc(size_t size) {
  void *temp;
  lock(&spinlock);
  cnt++;
  printf("This is %d request\n",cnt);
  temp = Bigloc((size_t)(size+0x10));
//  printf("alloc = %x\n",temp - scale);
  unlock(&spinlock);
  return temp;
}

static void kfree(void *ptr) {
  lock(&spinlock);
  printf("locked\n");
  printf("cpu = %d\n",_cpu());
  if(ptr!=NULL){printf("free = %x\n",(uintptr_t)ptr - scale);
  release((node *)((uintptr_t)ptr - scale));}
  printf("unlocked\n");
  unlock(&spinlock);
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
