// test_fpga_driver.c - read from fpga

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include <ctype.h>

#define BUFSIZE 256
static char data_from_fpga_driver[BUFSIZE];
static uint32_t data_from_fpga_driver32[BUFSIZE];


int numwords(const char* str)
{
  if (!str) return 0;
  int cnt = 0;
  char prev = ' ';
  int len = strlen(str);
  for (int i=0; i<len; i++) {
    if (isspace(prev) && !isspace(str[i])) {
      cnt++;
    }
    prev = str[i];
  }
  return cnt;
}

int endswith(const char* str, const char* suf)
{
  int str_len = strlen(str);
  int suf_len = strlen(suf);
  if (str_len < suf_len) return 0;
  int suf_offset = str_len - suf_len;
  const char* str2 = &str[suf_offset];
  return (strcmp(str2, suf) == 0);
}

int main()
{

  int fd = open("/dev/fpga_driver", O_RDWR);
  if (fd < 0) {
    perror("Error - could not open fpga_driver");
    return errno;
  }

  int errcnt = 0;
  int tstcnt = 0;
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
      printf("%s = %s", cmdstr, data_from_fpga_driver);
      
      if (numwords(cmdstr) <= 2) {
        printf("\n");
      } else if (endswith(cmdstr, data_from_fpga_driver)) {
        printf(" - pass\n");
        tstcnt++;
      } else {
        errcnt++;
        tstcnt++;
        printf(" - FAIL\n");
      }
    }
  } // while fgets
  
  if (errcnt>0) {
    printf("Test FAILED. %d errors detected out of %d tests\n", errcnt, tstcnt);
  } else if (tstcnt>0) {
    printf("Test PASSED. No errors detected out of %d tests\n", tstcnt);
  } else {
    printf("Test complete\n");
  }
  
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
