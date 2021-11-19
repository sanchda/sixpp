#include "sixpp.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

PPProfile *Profile_frompath(char *path) {
  // Open file and get size
  struct stat sa;
  int fd = open(path, O_RDONLY);
  if (-1 == fd || fstat(fd, &sa))
    return NULL;
  size_t sz = sa.st_size;

  // mmap the data
  void *data = mmap(0, sz, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
  if (!data)
    return NULL;

  return perftools__profiles__profile__unpack(0, sz, data);
}
