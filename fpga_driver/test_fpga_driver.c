// test_fpga_driver.c - read from fpga

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define BUFSIZE 256
static char data_from_fpga_driver[BUFSIZE];
static uint32_t data_from_fpga_driver32[BUFSIZE];

int main()
{

   int fd = open("/dev/fpga_driver", O_RDWR);
   if (fd < 0) {
      perror("Error - could not open fpga_driver");
      return errno;
   }

   printf("Reading from FPGA over PCIe\n");
   int numwords = 32;
   int numbytes = 4 * numwords;
 
   int errcode = read(fd, (char*)data_from_fpga_driver32, numbytes);
   if (errcode < 0) {
      perror("Error - could not read from fpga_driver");
      return errno;
   }
   
   for (int ii=0; ii < numwords; ii++) {
     printf("%d ", data_from_fpga_driver32[ii]);
   }
   return 0;
}
