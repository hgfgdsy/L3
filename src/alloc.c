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

static uintptr_t pm_start, pm_end;


int scale = sizeof(node);
int cntt=0;


spinlock_t heaplock;

static void pmm_init() {
  kmt->spin_init((spinlock_t *)&heaplock,"lock_alloc");
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
/*  printf("%x\n",pm_start);
  printf("%x\n",pm_end);
  if(pm_start > pm_end)
  printf("heap(up to down) = %x\n",pm_start - pm_end);
  else
  printf("heap(down to up) = %x\n",pm_end - pm_start);
*/
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
		  r -> order = i;
		  r -> kval = i;
		  OFFSET[i+1] += (1<<i);
	  }
	  else avail[i].first = NULL;
  }
}


node *my_buddy(node *p) {
  int s= (int)((uintptr_t)p - my_start - OFFSET[p->order]);
  int m = (1<<(p->kval));
  int n = (1<<((p->kval)+1));
  
  if(s%n == 0) return (node *)((char *)p+m);
  if(s%n == m) return (node *)((char *)p-m);
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
  return (void *)((char *)pa+0x20);
}

void release(node *p) {
  if(p->kval < p->order && p->kval > 10){
  node *s = my_buddy(p);
  while(s->tag==0 && s->kval==p->kval)
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
	  if(((uintptr_t)p - my_start - OFFSET[p->order])%(1<<((p->kval)+1)) == 0) p->kval++;
	  else
	  {
		  s->kval = p->kval +1;
		  p = s;
	  }
	  if(p->order == p->kval) break;
	  s = my_buddy(p);
  }
  }
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
  kmt->spin_lock((spinlock_t *)&heaplock);
  cntt++;
//  printf("This is %d request\n",cntt);
  temp = Bigloc((size_t)(size+0x20));
  if(temp==NULL){
	  printf("Memoty omit\n");
  }
  memset(temp,0,size);
  kmt->spin_unlock((spinlock_t *)&heaplock);
  return temp;
}

static void kfree(void *ptr) {
  kmt->spin_lock((spinlock_t *)&heaplock);
  if(ptr!=NULL){//printf("free = %x\n",(uintptr_t)ptr - scale);
  release((node *)((uintptr_t)ptr - scale));}
  kmt->spin_unlock((spinlock_t *)&heaplock);
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
