// fpga_driver.c - runs in kernel-space

#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exitf
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>        // Required for the copy to user function
#include <linux/pci.h>


// /dev/fpga_driver
#define  DEVICE_NAME "fpga_driver"

// /sys/class/fpga_class
#define  CLASS_NAME  "fpga_class"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ME");
MODULE_DESCRIPTION("Linux char driver for PCIe FPGA");
MODULE_VERSION("0.1");


// =========================================================================================
// =========================================================================================
// START OF PCI STUFF
//

// lspci to get bus=1, device=00 and function=0 01:00.0 Memory controller: Xilinx Corporation Device 903f (rev ff)
// lspci -n to get vendor=10ee and device id=903f 01:00.0 0580: 10ee:903f (rev ff)
// vendor=10ee and device id=903f
#define PCIFPGA_VENDOR_ID 0x10ee
#define PCIFPGA_DEVICE_ID 0x903f

static struct pci_device_id pcifpga_ids[] = {
  { PCI_DEVICE(PCIFPGA_VENDOR_ID, PCIFPGA_DEVICE_ID) },
  { }
};

MODULE_DEVICE_TABLE(pci, pcifpga_ids);


static void __iomem *ptr_bar0;

// This probe function is called when a PCI device exists and it is registered
static int pcifpga_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
  printk("In function pcifpga_probe ******************************\n");

  u16 vid, did;
  u8  capability_ptr;
  u32 bar0, saved_bar0;

  u8 pci_header_byte_offset = 0x0;
  if (pci_read_config_word(dev, pci_header_byte_offset, &vid)) {
    printk("  Error pci_read_config_word. Failed to read Vendor ID\n");
    return -1;
  }
  printk("  Vendor ID = 0x%x", vid);

  pci_header_byte_offset = 0x2;
  if (pci_read_config_word(dev, pci_header_byte_offset, &did)) {
    printk("  Error pci_read_config_word. Failed to read Device ID\n");
    return -1;
  }
  printk("  Device ID = 0x%x", did);

  pci_header_byte_offset = 0x34;
  if (pci_read_config_byte(dev, pci_header_byte_offset, &capability_ptr)) {
    printk("  Error pci_read_config_byte. Failed to read Capability Pointer\n");
    return -1;
  }

  if (capability_ptr) {
    printk("  Capability Pointer = 0x%x\n", capability_ptr);
  } else {
    printk("  No Capabilities Found\n");
  }

  pci_header_byte_offset = 0x10;
  if (pci_read_config_dword(dev, pci_header_byte_offset, &bar0)) {
    printk("  Error pci_read_config_dword. Failed to read BAR0\n");
    return -1;
  }
  saved_bar0 = bar0;

  if (pci_write_config_dword(dev, pci_header_byte_offset, 0xffffffff)) {
    printk("  Error pci_write_config_dword. Failed to write BAR0\n");
    return -1;
  }
  if (pci_read_config_dword(dev, pci_header_byte_offset, &bar0)) {
    printk("  Error pci_read_config_dword. Failed to read BAR0 after writing to it\n");
    return -1;
  }

  if ((bar0 & 0x3) == 1) {
    printk("  BAR0 is IO space\n");
  } else {
    printk("  BAR0 is MEMORY space = 0x%x\n", bar0);
  }

  bar0 &= 0xFFFFFFFD; // why do this???
  bar0 = ~bar0;
  bar0 += 1;
  printk("  BAR0 Size = %d KBytes\n", bar0/1024);

  if (pci_write_config_dword(dev, pci_header_byte_offset, saved_bar0)) {
    printk("  Error pci_write_config_dword. Failed to write BAR0 = saved_bar0\n");
    return -1;
  }

  int bar_id = 0;
  int bar_len = pci_resource_len(dev, bar_id);
  int bar_start = pci_resource_start(dev, bar_id);
  printk("  BAR%d starts at: 0x%x, length = %d\n", bar_id, bar_start, bar_len);

  // Use pci managed (pcim) to enable the device
  int status = pcim_enable_device(dev);
  if (status) {
    printk("  Error: Failed to enable device\n");
    return status;
  }
  printk("  Device enabled\n");

  status = pcim_iomap_regions(dev, BIT(bar_id), KBUILD_MODNAME);
  if (status) {
    printk("  Error: Failed pcim_iomap_regions\n");
    return status;
  }
  printk("  pcim_iomap_regions worked\n");

  ptr_bar0 = pcim_iomap_table(dev)[bar_id];
  if (ptr_bar0 == NULL) {
    printk("  Error: Failed pcim_iomap_table. Invalid ptr for BAR%d\n", bar_id);
    return -1;
  }
  printk("  pcim_iomap_table  worked\n");

  u32 read_value = ioread32(ptr_bar0);
  printk("  Read Value = 0x%x\n", read_value);
  iowrite32(42,ptr_bar0);
  read_value = ioread32(ptr_bar0);
  if (read_value==42) {
    printk("  Pass: Read Value = %d\n", read_value);
  } else {
    printk("  Fail: Read Value = %d. Expected 42\n", read_value);
  }

  iowrite32(100,ptr_bar0+4);
  read_value = ioread32(ptr_bar0+4);
  if (read_value==100) {
    printk("  Pass: Read Value = %d\n", read_value);
  } else {
    printk("  Fail: Read Value = %d. Expected 100\n", read_value);
  }

  read_value = ioread32(ptr_bar0);
  if (read_value==42) {
    printk("  Pass: Read Value = %d\n", read_value);
  } else {
    printk("  Fail: Read Value = %d. Expected 42\n", read_value);
  }

  printk("  Fill FPGA Memory\n");
  for (int ii=0; ii<512; ii++) {
    iowrite32(10000+ii,ptr_bar0+4*ii);
  }

  printk("  Read-Back FPGA Memory\n");
  int err_cnt = 0;
  for (int ii=0; ii<512; ii++) {
    read_value = ioread32(ptr_bar0+4*ii);
    if (read_value != (10000+ii)) {
      err_cnt++;
      if (err_cnt<10) {
        printk("  Fail: fpga[%d]=%d, expected=%d. err_cnt=%d", ii, read_value, 10000+ii, err_cnt);
      }
    }
  }
  if (err_cnt==0) {
    printk("  Read-Back PASSED!\n");
  } else {
    printk("  Read-Back FAILED with %d errors\n", err_cnt);
  }

  return 0;
}


// Used by pci_unregister_driver via struct pcifpga_driver
static void pcifpga_remove(struct pci_dev *dev)
{
  printk("xxxIn function pcifpga_remove  ****************************\n");
}


// pci driver struct
static struct pci_driver pcifpga_driver = {
  .name = "fpga_driver",
  .id_table = pcifpga_ids,
  .probe = pcifpga_probe,
  .remove = pcifpga_remove,
};

// END OF PCI STUFF
// =========================================================================================
// =========================================================================================


// =========================================================================================
// =========================================================================================
// START OF CHAR DRIVER STUFF

// Device drivers have a major and minor number.  The major number is used by the kernel
// to identify the device driver and the minor number is used only by the device driver.
// To view the major (236) and minor (0) numbers use the `ls -l` command:
//  ls -l /dev/fpga_driver
//  crw------- 1 root root 236, 0 Jun  9 20:06 /dev/fpga_driver

static int    major_number;
static int    open_count = 0;
static char   message[256] = {0};
static u32    message32[512] = {0};
static short  message_size;


// These structs are initialized by class_create and device_create in fpga_driver_init().
static struct class*  fpga_driver_class  = NULL;
static struct device* fpga_driver_device = NULL;


// Here are functions a character driver usually has.
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


// The file_operations struct is how the device is represented in the kernel.
static struct file_operations fops =
{
  .open = dev_open,       // Called each time the device is opened from user space
  .read = dev_read,       // Called when data is sent from the device to user space
  .write = dev_write,     // Called when data is sent from user space to the device
  .release = dev_release, // Called when the device is closed in user space
};


// This function initializes the driver when the insmod command is run
static int __init fpga_driver_init(void)
{
  printk(KERN_INFO "fpga_driver_init: Initializing the fpga_driver LKM\n");

  // Try to dynamically allocate a major number for the device -- more difficult but worth it
  major_number = register_chrdev(0, DEVICE_NAME, &fops);
  if (major_number < 0) {
    printk(KERN_ALERT "fpga_driver_init: Failed to register a major number\n");
    return major_number;
  }
  printk(KERN_INFO "fpga_driver_init: Registered correctly with major number %d\n", major_number);

  // Register the device class
  // ERROR: older kernel had two parameteter fpga_driver_classfpga_driver_class = class_create(THIS_MODULE, CLASS_NAME);
  fpga_driver_class = class_create(CLASS_NAME);

  // Check for error and clean up if there is
  if (IS_ERR(fpga_driver_class)) {
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_ALERT "Failed to register device class\n");
    return PTR_ERR(fpga_driver_class);
  }
  printk(KERN_INFO "fpga_driver_init: device class registered correctly\n");

  // Register the device driver, hard-code minor number of zero
  fpga_driver_device = device_create(fpga_driver_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);

  // Clean up if there is an error
  if (IS_ERR(fpga_driver_device)) {
    class_destroy(fpga_driver_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_ALERT "Failed to create the device\n");
    return PTR_ERR(fpga_driver_device);
  }
  printk(KERN_INFO "fpga_driver_init - Before pci_register_driver \n");
  return pci_register_driver(&pcifpga_driver);
}


// This is the driver's cleanup function
static void __exit fpga_driver_exit(void)
{
  device_destroy(fpga_driver_class, MKDEV(major_number, 0));
  class_unregister(fpga_driver_class);
  class_destroy(fpga_driver_class);
  unregister_chrdev(major_number, DEVICE_NAME);
  pci_unregister_driver(&pcifpga_driver);
  printk(KERN_INFO "fpga_driver_exit: device cleanup done\n");
}


// This function is called when a the device file is opened (usually from a user-space app)
static int dev_open(struct inode *inodep, struct file *filep){
  open_count++;
  printk(KERN_INFO "fpga_driver.dev_open: open_count = %d\n", open_count);
  return 0;
}


// This function is called when a read is performed (usually from a user-space app)
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{

  len = len / 4; // convert bytes to 32-bit words
  for (int ii=0; ii<len; ii++) {
    message32[ii] = ioread32(ptr_bar0+4*ii);
    printk(KERN_INFO "fpga[%d]=%d ", ii, message32[ii]);
  }
  message_size = 4*len;

  int error_count = 0;
  // copy_to_user has the format ( * to, *from, size) and returns 0 on success
  error_count = copy_to_user(buffer, (char*)message32, message_size);
  if (error_count==0) { 
    printk(KERN_INFO "fpga_driver.dev_read: %d bytes read by user\n", message_size);
    return (message_size=0);
  } else {
    printk(KERN_ALERT "fpga_driver.dev_read: read failed\n");
    return -EFAULT;
  }
}


// This function is called when a write is performed (usually from a user-space app)
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
  const char delta = 'a' - 'A';
  int i = 0;
  for (; i<len; i++) {
    if (buffer[i] == '\0') break;
    message[i] = (buffer[i]>='a' && buffer[i] <= 'z') ? buffer[i] - delta : buffer[i];
  }
  message[i] = '\0';

  message_size = i;
  printk(KERN_INFO "fpga_driver.dev_write: bytes written = %d\n", message_size);
  return len;
}


// This function is called when a close is performed (usually from a user-space app)
static int dev_release(struct inode *inodep, struct file *filep)
{
  printk(KERN_INFO "fpga_driver.dev_release: closing device\n");
  return 0;
}


// Define the functions that will run when the device is loaded (insmod) and unloaded (rmmod)
module_init(fpga_driver_init);
module_exit(fpga_driver_exit);

