#include <sys/types.h>
#include <unistd.h>

struct memory_block {
  size_t size;
  memory_block* next_block;
  memory_block* pre_block;
  bool free;
  int level;
  void* start_pointer;
}

void *malloc(size_t size);
memory_block find_block(size_t size);

memory_block* first_block;