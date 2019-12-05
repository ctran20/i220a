#include "fn-trace.h"
#include "x86-64_lde.h"

#include "memalloc.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct FnsData {
  int size;             /** current # of elements allocated for fnsData. */
  int nextIndex;        /** current # of elements used in fnsData. */
  FnInfo *fnsData;      /** point to base of allocated FnInfo's. */
};

/** Return index of function at address.  < 0 if not found. */
static int
get_fn_index(FnsData *fnsDataP, void *address)
{
  for (int i = 0; i < fnsDataP->nextIndex; i++) {
    if (fnsDataP->fnsData[i].address == address) return i;
  }
  return -1;
}

static int
new_fn(FnsData *fnsDataP, void *address)
{
  if (fnsDataP->nextIndex >= fnsDataP->size) {
    int newSize = (fnsDataP->size == 0) ? 2 : fnsDataP->size*2;
    fnsDataP->fnsData = reallocChk(fnsDataP->fnsData, newSize*sizeof(FnInfo));
    fnsDataP->size = newSize;
  }
  FnInfo *fnInfoP = &fnsDataP->fnsData[fnsDataP->nextIndex];
  fnInfoP->address = address; fnInfoP->length = 0;
  fnInfoP->nInCalls = 0; fnInfoP->nOutCalls = 0;
  return fnsDataP->nextIndex++;
}

enum {
  CALL_OP = 0xE8,
  RET_FAR_OP = 0xCB,
  RET_FAR_WITH_POP_OP = 0xCA,
  RET_NEAR_OP = 0xC3,
  RET_NEAR_WITH_POP_OP = 0xC2
};

static inline bool is_call(unsigned op) { return op == CALL_OP; }
static inline bool is_ret(unsigned op) {
  return
    op == RET_NEAR_OP || op == RET_NEAR_WITH_POP_OP ||
    op == RET_FAR_OP || op == RET_FAR_WITH_POP_OP;
}

/** Recursively trace fn and all functions called by it, recording
 *  information about traced functions in fnsData.
 */
static void
trace_fn(void *fn, FnsData *fnsDataP)
{
  int fnIndex = new_fn(fnsDataP, fn);
  fnsDataP->fnsData[fnIndex].nInCalls++;
  unsigned char *p = fn;
  int length = 0;
  int nCalls = 0;
  unsigned int op;
  do {
    op = *p;
    int n = get_op_length(p);
    if (n < 0) {
      fprintf(stderr, "cannot get length for op %d at %p in fn %p\n",
              *p, p, fn);
      break;
    }
    length += n;
    unsigned char *p0 = p;
    p += n;
    if (is_call(op)) {
      nCalls++;
      int disp = *(int *)(p0 + 1);
      void *callAddress = p + disp;
      int callFnIndex = get_fn_index(fnsDataP, callAddress);
      if (callFnIndex < 0) {
        trace_fn(callAddress, fnsDataP);
      }
      else {
        fnsDataP->fnsData[callFnIndex].nInCalls++;
      }
    }
  } while (!is_ret(op));
  fnsDataP->fnsData[fnIndex].length = length;
  fnsDataP->fnsData[fnIndex].nOutCalls = nCalls;
}

static int cmp_fn_infos(const void *p1, const void *p2) {
  const FnInfo *fnInfoP1 = (const FnInfo *)p1;
  const FnInfo *fnInfoP2 = (const FnInfo *)p2;
  return ((char *)fnInfoP1->address) - ((char *)fnInfoP2->address);
}


/** Return pointer to opaque data structure containing collection of
 *  FnInfo's for functions which are callable directly or indirectly
 *  from the function whose address is rootFn.
 */
const FnsData *
new_fns_data(void *rootFn)
{
  assert(sizeof(int) == 4);
  FnsData *fnsDataP = callocChk(1, sizeof(struct FnsData));
  trace_fn(rootFn, fnsDataP);
  qsort(fnsDataP->fnsData, fnsDataP->nextIndex, sizeof(FnInfo), cmp_fn_infos);
  return fnsDataP;
}

/** Free all resources occupied by fnsData. fnsData must have been
 *  returned by new_fns_data().  It is not ok to use to fnsData after
 *  this call.
 */
void
free_fns_data(FnsData *fnsData)
{
  free(fnsData->fnsData);
  free(fnsData);
}

/** Iterate over all FnInfo's in fnsData.  Make initial call with NULL
 *  lastFnInfo.  Keep calling with return value as lastFnInfo, until
 *  next_fn_info() returns NULL.  Values must be returned sorted in
 *  increasing order by fnInfoP->address.
 *
 *  Sample iteration:
 *
 *  for (FnInfo *fnInfoP = next_fn_info(fnsData, NULL); fnInfoP != NULL;
 *       fnInfoP = next_fn_info(fnsData, fnInfoP)) {
 *    //body of iteration
 *  }
 *
 */
const FnInfo *
next_fn_info(const FnsData *fnsData, const FnInfo *lastFnInfo)
{
  const FnsData *fnsDataP = fnsData;
  int lastIndex = (lastFnInfo == NULL) ? -1 : lastFnInfo - fnsDataP->fnsData;
  int nextIndex = lastIndex + 1;
  return
    nextIndex >= fnsDataP->nextIndex ? NULL : &fnsDataP->fnsData[nextIndex];
}