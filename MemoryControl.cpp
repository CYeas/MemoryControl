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
  int new_block_level = (block -> level) - 1;
  int new_block_size = 2 << new_block_level;
  memory_block* new_block_first = make_memory_block(new_block_size, true, new_block_level, block -> start_pointer);
  memory_block* new_block_second = make_memory_block(new_block_size, true, new_block_level, block -> start_pointer + new_block_size);
  //插入链表
  memory_block* pre_block = block -> pre_block;
  memory_block* next_block = block -> next_block;
  if (pre_block) {
    pre_block -> next_block = new_block_first;
  } else {
    first_block = new_block_first;
  }
  new_block_first -> pre_block = pre_block;
  new_block_first -> next_block = new_block_second;
  new_block_second -> pre_block = new_block_first;
  new_block_second -> next_block = next_block;
  if(next_block) {
    next_block -> pre_block = new_block_second;
  }
  delete block;
  return new_block_first;
}

void make_memory_block(int size, bool free, int level, void* start_pointer) {
  memory_block* block = new memory_block;
  block -> size = size;
  block -> free = free;
  block -> level = level;
  block -> start_pointer = start_pointer;
  return block;
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
  memory_block* pre_block = block -> pre_block;
  memory_block* next_block = block -> next_block;
  memory_block* merged_block = NULL;
  if (pre_block && pre_block -> free && (pre_block -> level) == (block -> level)) {
    merged_block = merge_block(pre_block, block);
  } else if (next_block &&  next_block -> free && (next_block -> level) == (block -> level)) {
    merged_block = merge_block(block, next_block);
  }
  if(merged_block) {
    buddy_free(merged_block);
  }
}

memory_block* merge_block(memory_block* f_block, memory_block* n_block) {
    if(!(f_block -> level == n_block -> level))
        return NULL;
    int upper_level = f_block -> level + 1;
    int size = 2 << upper_level;
    memory_block* new_block = make_memory_block(size, true, upper_level, f_block -> start_pointer);
    memory_block* list_pre = f_block -> pre_block;
    memory_block* list_next = n_block -> next_block;
    if (list_pre) {
        list_pre -> next_block = new_block;
    }
    new_block -> pre_block = list_pre;
    if (list_next) {
        list_next -> pre_block = new_block;
    }
    new_block -> next_block = list_next;
    delete f_block;
    delete n_block;
    return new_block;
}
