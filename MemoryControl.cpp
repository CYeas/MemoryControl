#include <sys/types.h>
#include <unistd.h>
#include "MemoryControl.h"
#include <math.h>

using namespace std;
// void *malloc(size_t size) {
//   void *p;
//   p = sbrk(0);
//   if (sbrk(size) == (void *)-1)
//     return NULL;
//   return p;
// }

void *malloc(size_t size) {
  memory_block* block = find_block(size);
  void* p = buddy_alloc(block, size);
  return p;
}

memory_block* find_block(size_t size) {
  memory_block* block = first_block;
  int last_level = block -> level;
  while (block && !((block -> free) && (block -> size >= size)) {
    last_level = block -> level;
    block = block -> next_block;
  }
  return block
}

void* buddy_alloc(memory_block* block, size_t size) {
  memory_block* deal_block = block;
  int sec_block_size = 2 << (deal_block -> level - 1);

  while(sec_block_size > size) {
    deal_block = divide_block(block, size);
    sec_block_size = 2 << (deal_block -> level - 1);
  }

  deal_block -> free = false;
  return deal_block -> start_pointer;
}

memory_block* divide_block(memory_block* block, size_t size) {
  memory_block* new_block_first = new memory_block;
  memory_block* new_block_second = new memory_block;
  int new_block_level = (block -> level) - 1;
  int new_block_size = 2 << new_block_level;
  make_memory_block(&new_block_first, new_block_size, true, new_block_level, block -> start_pointer);
  make_memory_block(&new_block_second, new_block_size, true, new_block_level, block -> start_pointer + new_block_size);
  //插入链表
  memory_block* pre_block = block -> pre_block;
  memory_block* next_block = block -> next_block;
  pre_block -> next_block = new_block_first;
  new_block_first -> pre_block = pre_block;
  new_block_first -> next_block = new_block_second;
  new_block_second -> pre_block = new_block_first;
  new_block_second -> next_block = next_block;
  if(next_block) {
    next_block.pre_block = new_block_second;
  }
  delete block;
  return new_block_first;
}

void make_memory_block(memory_block* block, int size, bool free, int level, void* start_pointer) {
  block -> size = size;
  block -> free = free;
  block -> level = level;
  block -> start_pointer = start_pointer;
}

void free_mem(void* p) {
  memory_block* block = find_block_by_mem(p);
  if(!block)
    return;
  buddy_free(block);
}

memory_block* find_block_by_mem(void* p) {
  memory_block* block = first_block;
  while(block && !(block -> start_pointer == p)) {
    block = block -> next_block;
  }

  return block;
}

void buddy_free(memory_block* block) {
  block -> free = true;
}