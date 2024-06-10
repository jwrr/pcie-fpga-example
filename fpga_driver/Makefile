obj-m+=fpga_driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	gcc test_fpga_driver.c -o test_fpga_driver
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm test_fpga_driver
