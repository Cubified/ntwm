/*
 * libspool.h: a small memory pool implementation
 *
 * Credits to https://graphics.stanford.edu/~seander/bithacks.html
 * for clever bit manipulation tricks
 */

#ifndef __LIBSPOOL_H
#define __LIBSPOOL_H

#include <stdlib.h>

#define flip_bit(n, b) (n ^= pow2(b))

#define pool_get(i, p) (p->pool[i])
#define pool_astype(i, t) (*(t*)i)
#define pool_first(p) lsb(p->avail^0xFFFF)
#define pool_foreach(p) int ind;for(ind=pool_first(p);p->avail<0xFFFF&&ind<p->size;ind=next_unset_bit(p->avail,ind))
#define pool_foreach_nodecl(p) for(ind=pool_first(p);p->avail<0xFFFF&&ind<p->size;ind=next_unset_bit(p->avail,ind)) /* If ANSI/C99/C11 compliance is necessary */

#define DIR_LEFT  0
#define DIR_RIGHT 1

typedef struct pool {
  int ind;     /* Index of first available slot (least significant bit) */
  int size;    /* Maximum size of the memory pool */
  int avail;   /* Integer holding all available slots */
  void **pool; /* Pool itself */
} pool;

static int pow2(int n);
static int lsb(int n);
static int bits_set(int n);
static int set_all(int size);
static int next_unset_bit(int n, int start);

static pool *pool_init();
static void pool_push(void *data, pool *p);
/* pool_pop(int ind, pool *p); - Defined as macro */
static int  pool_adj(int ind, int dir, pool *p);
static int  pool_find(void *data, pool *p);
static int  pool_check(int ind, pool *p);
static int  pool_count(pool *p);
static void pool_free(pool *p);

/*
 * Finds powers of 2 by lookup table
 */
int pow2(int n){
  int powers[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536 };

  return powers[n];
}

/*
 * Finds the least significant bit in
 * an integer
 */
int lsb(int n){
  float f;

  if(n == 0){
    return -1;
  }

  f = (float)(n & -n);
  return (*(int *)&f >> 23) - 0x7f;
}

/*
 * Returns the number of bits set
 * to one in an integer by use of
 * lookup table
 */
int bits_set(int n){
  static const unsigned char BitsSetTable256[256] = {
#   define B2(n) n,     n+1,     n+1,     n+2
#   define B4(n) B2(n), B2(n+1), B2(n+1), B2(n+2)
#   define B6(n) B4(n), B4(n+1), B4(n+1), B4(n+2)
    B6(0), B6(1), B6(1), B6(2)
  };

  return (BitsSetTable256[n & 0xff] + 
    BitsSetTable256[(n >> 8) & 0xff] + 
    BitsSetTable256[(n >> 16) & 0xff] + 
    BitsSetTable256[n >> 24]);
}

/*
 * Returns an integer with all bits
 * to the right of a given value
 * set to 1
 */
int set_all(int size){
  return (1 << size) - 1;
}

/*
 * Returns the position of the next bit
 * equal to zero moving right-to-left
 * from a given starting point
 */
int next_unset_bit(int n, int start){
  unsigned int flip = ~n;
  flip >>= start+1;
  return lsb(flip)+start+1;
}

/***/

/*
 * Initializes a new memory pool by allocating
 * a block of preset size, removing continued
 * calls to malloc() which slow execution
 */
pool *pool_init(int size){
  pool *p = malloc(sizeof(pool));
  p->ind = 0;
  p->size = size;
  p->avail = set_all(size);
  p->pool = malloc(sizeof(void*)*size);

  return p;
}

/*
 * Pushes a given pointer to the pool and
 * moves the index of the nearest
 * available block (overwriting
 * old entries if enabled)
 */
void pool_push(void *data, pool *p){
  if(p->ind == -1){
    error("Out of memory.");
    return;
  }

  flip_bit(p->avail, p->ind);
  p->pool[p->ind] = data;
  p->ind = lsb(p->avail);
}

/*
 * Frees a pool item simply by
 * flagging it for overwriting
 */
#define pool_pop(i, p) flip_bit(p->avail, i);p->ind=i

/*
 * Returns the index of the next
 * filled slot in the given
 * direction (left/right),
 * wrapping if necessary
 */
int pool_adj(int pos, int dir, pool *p){
  int out = pos,
      cnt = 0,
      hit = 0;

  flip_bit(p->avail, pos);

  if(dir == DIR_LEFT){
    while(cnt < p->size){
      if(pool_check(out, p) == 0){
        hit = 1;
        break;
      }
      out--;
      cnt++;

      if(out < 0){
        out = p->size-1;
      }
    }
  } else {
    while(cnt < p->size){
      if(pool_check(out, p) == 0){
        hit = 1;
        break;
      }
      out++;
      cnt++;

      if(out >= p->size){
        out = 0;
      }
    }
  }

  flip_bit(p->avail, pos);

  return (hit ? out : -1);
}

/*
 * Finds an item in the pool
 * based on its data,
 * returning -1 if
 * not found
 */
int pool_find(void *data, pool *p){
  pool_foreach(p){
    if(pool_get(ind, p) == data){
      return ind;
    }
  }
  return -1;
}

/*
 * Returns 1 if a given slot is
 * available
 */
int pool_check(int ind, pool *p){
  return (p->avail & (1<<ind));
}

/*
 * Returns the number of currently-
 * occupied slots
 */
int pool_count(pool *p){
  return p->size - bits_set(p->avail);
}

/*
 * Frees a pool and its associated
 * arrays
 */
void pool_free(pool *p){
  free(p->pool);
  free(p);
}

#endif
