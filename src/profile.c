#include "sixpp.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

PPProfile *Profile_fromfd(int fd) {
  struct stat sa;
  if (-1 == fd)
    return NULL;
  if (fstat(fd, &sa))
    return NULL;
  size_t sz = sa.st_size;

  // mmap the data
  void *data = mmap(0, sz, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
  if (!data)
    return NULL;

  return perftools__profiles__profile__unpack(0, sz, data);
}

PPProfile *Profile_frompath(char *path) {
  int fd = open(path, O_RDONLY);
  return Profile_fromfd(fd);
}

PPProfile *Profile_frompathat(int dfd, char *path) {
  if (-1 == dfd)
    return NULL;
  int fd = openat(dfd, path, O_RDONLY);
  return Profile_fromfd(fd);
}
