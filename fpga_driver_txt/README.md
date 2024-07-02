fpga_driver_txt
===============
Simple PCIe driver that gets ASCII write/read commands from the app, and 
returns ASCII response data to the APP. The test app accepts data from stdin, 
which can be from the keyboard, file or pipe.


Compile and Install Driver and App
-----------------------------------

Note: You need a signing key at ~/signing_key.x509

```
# source compile_and_install.sh 
make -C /lib/modules/6.5.0-35-generic/build/ M=/home/jwrr/git/pcie-fpga-example/fpga_driver_txt modules
make[1]: Entering directory '/usr/src/linux-headers-6.5.0-35-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: x86_64-linux-gnu-gcc-12 (Ubuntu 12.3.0-1ubuntu1~22.04) 12.3.0
  You are using:           gcc-12 (Ubuntu 12.3.0-1ubuntu1~22.04) 12.3.0
make[1]: Leaving directory '/usr/src/linux-headers-6.5.0-35-generic'
gcc test_fpga_driver.c -o test_fpga_driver
/dev/fpga_driver
```

Run Test1 from File
-------------------

```
# ./test_fpga_driver < test1.txt 
w 0x0 0x55
w 0x4 0xaa
w 0x8 0xcc
w 0xc 0xff
r 0x0 = 0x00000055
r 0x4 = 0x000000aa
r 0x8 = 0x000000cc
r 0xc = 0x000000ff
```

Run Test2 from Pipe

```
# ./test2.py | ./test_fpga_driver
w 0x0000 0x000003e8
w 0x0004 0x000003e9
w 0x0008 0x000003ea
w 0x000c 0x000003eb
w 0x0010 0x000003ec
r 0x0000 0x000003e8 = 0x000003e8 - pass
r 0x0004 0x000003e9 = 0x000003e9 - pass
r 0x0008 0x000003ea = 0x000003ea - pass
r 0x000c 0x000003eb = 0x000003eb - pass
r 0x0010 0x000003ec = 0x000003ec - pass
```

Notes
-----

* The 'w addr data' writes the data to the FPGA's memory. The address start from 0 and increments by 4 (the 2nd value is at location 4).
* The 'r addr' reads from the addr location.
* The 'r addr exp' reads from the addr location and compares the result with the expected value
* Address and Data values must be HEX values

