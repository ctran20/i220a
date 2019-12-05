#include "cache-sim.h"

#include "memalloc.h"

#include <stdbool.h>
#include <stddef.h>
int pow_2(int n){
	int result = 1;
	for(int i = 0; i < n; i++){
		result *= 2;
	}
	return result;
}

typedef struct CacheLineImpl{
 MemAddr addr;
 unsigned tag;
 int valid;
 unsigned LRUVal;
}CacheLine;

typedef struct CacheSetImpl{
 CacheLine *lines;
 unsigned mruHolder;
}CacheSet;

struct CacheSimImpl{
 CacheSet *sets;
 int numSets;
 unsigned nSetBits;      
 unsigned nLinesPerSet;  
 unsigned nLineBits;     
 unsigned nMemAddrBits;  
                         
 Replacement replacement;
};

/** Create and return a new cache-simulation structure for a
 *  cache for main memory withe the specified cache parameters params.
 *  No guarantee that *params is valid after this call.
 */
CacheSim *
new_cache_sim(const CacheParams *params)
{
  CacheSim *cache;
  cache = malloc(sizeof(CacheSim));  
  cache->nSetBits = params->nSetBits;
  cache->nLinesPerSet = params->nLinesPerSet;
  cache->nLineBits = params->nLineBits;
  cache->nMemAddrBits = params->nMemAddrBits;
  cache->replacement = params->replacement;

  cache->numSets = (1 << cache->nSetBits);
  
  //Malloc
  cache->sets = malloc((cache->numSets)* (sizeof(CacheSet)));
  for (int i = 0; i < cache->numSets; ++i) {
   cache->sets[i].lines = malloc(cache->nLinesPerSet* (sizeof(CacheLine)));
  }
  
  //Init
  for (int i = 0; i < cache->numSets; ++i)
  {
      for (int j = 0; j < cache->nLinesPerSet; ++j)
      {
          cache->sets[i].lines[j].valid = 0;
		  cache->sets[i].lines[j].tag = 0;
      }
  }
  return cache;
}

/** Free all resources used by cache-simulation structure *cache */
void
free_cache_sim(CacheSim *cache)
{
  for (int i = 0; i < cache->numSets; ++i) {
   free(cache->sets[i].lines);
  }
  free(cache->sets);
  free(cache);
}

/** Return result for addr requested from cache */
CacheResult
cache_sim_result(CacheSim *cache, MemAddr addr)
{
  //Error if address 0
  CacheResult result  = { CACHE_MISS_WITH_REPLACE, 0x0 };
  unsigned s = cache->nSetBits;
  unsigned E = cache->nLinesPerSet;
  unsigned b = cache->nLineBits;
  unsigned m = cache->nMemAddrBits;

  Replacement replace = cache->replacement;
  
  unsigned long tagBits = m-(s+b);
  unsigned long tag = addr >> (s + b);
  unsigned long sIndex = (addr >> b) % pow_2(m-tagBits-b);

  for (int i = 0; i < E; ++i)
  {
	  if(cache->sets[sIndex].lines[i].valid == 1){
		  if(cache->sets[sIndex].lines[i].tag == tag){
			  result.status = CACHE_HIT;
			  cache->sets->mruHolder = tag;
              break;
		  }
	  }
	  else if(cache->sets[sIndex].lines[i].valid != 0){
		  if (replace==LRU_R){
				result.replaceAddr = cache->sets[sIndex].lines[E - 1].tag << (s + b);
				cache->sets[sIndex].lines[E - 1].tag = tag;
				result.status = CACHE_MISS_WITH_REPLACE;
				break;
			}
			else if(replace==MRU_R){
				for (int i = 0; i < E; ++i)
				{
					if(cache->sets[sIndex].lines[i].tag ==
					cache->sets->mruHolder){

					cache->sets[sIndex].lines[i].tag=tag;
					result.status = CACHE_MISS_WITH_REPLACE;
					result.replaceAddr = addr;
					cache->sets->mruHolder = tag;
					break;				  
					}
				}
			}
			else{
				int i = 0;
				i = rand () % E;
				cache->sets[sIndex].lines[i].tag = tag;
				result.status = CACHE_MISS_WITH_REPLACE;
				break;
			}
	  }
	  else{
		cache->sets[sIndex].lines[i].valid = 1;
        cache->sets[sIndex].lines[i].tag = tag;
        result.status = CACHE_MISS_WITHOUT_REPLACE;
        cache->sets->mruHolder = tag;
        break;
	  }
  }
  return result;
}
