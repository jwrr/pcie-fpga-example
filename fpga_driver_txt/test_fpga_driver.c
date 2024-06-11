// test_fpga_driver.c - read from fpga

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
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

  char cmdstr[BUFSIZE];
  while (fgets(cmdstr, BUFSIZE, stdin)) {
    size_t cmdstr_len = strlen(cmdstr);
    if (cmdstr_len > 0) {
      cmdstr[cmdstr_len-1] = '\0'; // remove '\n'
    }
    int errcode = 0;
    // Send command
    //  char cmdstr[BUFSIZE] = "w 0x1 0x12345678";
    //  char cmdstr[BUFSIZE] = "r 0x1 0x0"

    if (cmdstr[0] == 'w' || cmdstr[0] == 'r') {
      errcode = write(fd, cmdstr, cmdstr_len);
      if (errcode < 0) {
        perror("Error - could not send command to fpga_driver.\n");
        return errno;
      }
    }
    
    if (cmdstr[0] == 'w') {
      printf("%s\n", cmdstr); // echo write command
    } else if (cmdstr[0] == 'r') {
      errcode = read(fd, (char*)data_from_fpga_driver, 4);
      if (errcode < 0) {
        perror("Error - could not read from fpga_driver");
        return errno;
      }
      printf("%s = %s\n", cmdstr, data_from_fpga_driver);
    }
  } // while fgets
  
//    printf("Reading from FPGA over PCIe\n");
//    int numwords = 32;
//    int numbytes = 4 * numwords;
//
//    errcode = read(fd, (char*)data_from_fpga_driver32, numbytes);
//    if (errcode < 0) {
//       perror("Error - could not read from fpga_driver");
//       return errno;
//    }
//    
//    for (int ii=0; ii < numwords; ii++) {
//      printf("%d ", data_from_fpga_driver32[ii]);
//    }
//    printf("\n");
   return 0;
}
