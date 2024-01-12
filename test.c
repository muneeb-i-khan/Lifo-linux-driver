#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define WR_VALUE _IOW('a', 'a', int32_t *)
#define RD_VALUE _IOR('a', 'b', int32_t *)
#define PUSH _IOW('a', 'c', int32_t *)

struct data {
  int length;
  char *buffer;
};

int main() {
  int fd;
  int32_t value, number;
  unsigned long write_buff;
  char *read_buff;
  printf("\nOpening Driver\n");
  fd = open("/dev/lifo_device", O_RDWR);
  if (fd < 0) {
    printf("Cannot open device file...\n");
    return 0;
  }

  struct data d1;

  d1.buffer = (char *)malloc(20 * sizeof(char));
  read_buff = (char *)malloc(sizeof(char));

  printf("Enter the Value to send\n");
  scanf("%[^\n]%*c", (d1.buffer));
  d1.length = strlen(d1.buffer);
  printf("%d\n", d1.length);
  printf("Writing Value to Driver\n");
  ioctl(fd, PUSH, d1.buffer);
  printf("Closing Driver\n");
  ioctl(fd, RD_VALUE, read_buff);
  printf("Popped: %s\n", (char *)read_buff);
  close(fd);
  free(d1.buffer);
  free(read_buff);
}
