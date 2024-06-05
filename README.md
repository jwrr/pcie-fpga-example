PCIe FPGA Example
=====================

lspci to get bus=1, device=00 and function=0
01:00.0 Memory controller: Xilinx Corporation Device 903f (rev ff)

lspci -n to get vendor=10ee and device id=903f
01:00.0 0580: 10ee:903f (rev ff)

Register and Unregister a PCIe Device
--------------------------------------

Makefile
pcifpga.c


Make Error
----------

```
$ make
make -C /lib/modules/6.5.0-35-generic/build M=/home/jwrr/git/fpga_pcie_driver modules
make[1]: Entering directory '/usr/src/linux-headers-6.5.0-35-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: x86_64-linux-gnu-gcc-12 (Ubuntu 12.3.0-1ubuntu1~22.04) 12.3.0
  You are using:           
  CC [M]  /home/jwrr/git/fpga_pcie_driver/pcifpga.o
/bin/sh: 1: gcc-12: not found
make[3]: *** [scripts/Makefile.build:251: /home/jwrr/git/fpga_pcie_driver/pcifpga.o] Error 127
make[2]: *** [/usr/src/linux-headers-6.5.0-35-generic/Makefile:2039: /home/jwrr/git/fpga_pcie_driver] Error 2
make[1]: *** [Makefile:234: __sub-make] Error 2
make[1]: Leaving directory '/usr/src/linux-headers-6.5.0-35-generic'
make: *** [Makefile:4: all] Error 2
```

```
sudo apt install --reinstall gcc-12
Then all you should have to do is update the link for gcc to go to the gcc-12 binary:
sudo ln -s -f /usr/bin/gcc-12 /usr/bin/gcc
You can set it back to 11 as well by doing the following:
sudo ln -s -f /usr/bin/gcc-11 /usr/bin/gcc
You can check it by running gcc --version. 
```

INSMOD LOAD ERROR
-----------------

```
$ sudo insmod pcifpga.ko 
insmod: ERROR: could not insert module pcifpga.ko: Key was rejected by service
jwrr@jwrr-500w:~/git/fpga_pcie_driver$ sudo -s
root@jwrr-500w:/home/jwrr/git/fpga_pcie_driver# ls
Makefile  modules.order  Module.symvers  pcifpga.c  pcifpga.ko  pcifpga.mod  pcifpga.mod.c  pcifpga.mod.o  pcifpga.o  README.md
root@jwrr-500w:/home/jwrr/git/fpga_pcie_driver# insmod pcifpga.ko
insmod: ERROR: could not insert module pcifpga.ko: Key was rejected by service
root@jwrr-500w:/home/jwrr/git/fpga_pcie_driver# dmesg | tail -1
[16457.464959] Loading of unsigned module is rejected
```


```
Ubuntu 22.04
sudo mokutil --disable-validation
sudo reboot

When it restart it will open a blue screen choose change secure boot, and then it 
will ask you about your password characters positions for example your 
password is "Ilovelinux123!" it will ask you to enter character 3 in your 
password then you have to type “o” after doing this for 3 or 4 times it 
will bring you do you want to disable choose yes. That's it. 
```

```
  To Disable Secure Boot in shim-signed running sudo update-secureboot-policy. This wiki page explains this method:
    Open a terminal (Ctrl + Alt + T), and execute sudo update-secureboot-policy and then select Yes.
    Enter a temporary password between 8 to 16 digits. (For example, 12345678, we will use this password later
    Enter the same password again to confirm.
    Reboot the system and press any key when you see the blue screen (MOK management
    Select Change Secure Boot state
    Enter the password you had selected in Step 2 and press Enter.
    Select Yes to disable Secure Boot in shim-signed.
    Press Enter key to finish the whole procedure.
  To enable Secure Boot in shim-signed again. Just execute
    sudo update-secureboot-policy --enable and then follow the steps above
```

```
$ sudo dmesg |tail 
[    3.160458] input: HDA Intel HDMI HDMI/DP,pcm=8 as /devices/pci0000:00/0000:00:03.0/sound/card0/input17
[    5.041272] loop14: detected capacity change from 0 to 8
[    6.401481] rfkill: input handler disabled
[    6.642382] e1000e 0000:00:19.0 eno1: NIC Link is Up 1000 Mbps Full Duplex, Flow Control: Rx/Tx
[  135.755070] pcifpga: loading out-of-tree module taints kernel.
[  135.755076] pcifpga: module license 'MIT' taints kernel.
[  135.755077] Disabling lock debugging due to kernel taint
[  135.755078] pcifpga: module verification failed: signature and/or required key missing - tainting kernel
[  135.755079] pcifpga: module license taints kernel.
[  135.755396] In function my_init
```


```
$ sudo insmod pcifpga.ko
insmod: ERROR: could not insert module pcifpga.ko: File exists
$ lsmod |grep fpga
pcifpga                12288  0
$ sudo rmmod pcifpga
$ lsmod |grep fpga
$ 
$ sudo insmod pcifpga.ko
$ sudo dmesg |tail 
```
