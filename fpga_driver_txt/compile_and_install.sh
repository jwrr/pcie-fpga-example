make
source sign_fpga_driver.sh 
rmmod fpga_driver
insmod fpga_driver.ko
ls /dev/fpga_driver

