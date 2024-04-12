// cpuinfo.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
/* Needed by all modules */
/* Needed for KERN_INFO */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("KRERK PIROMSOPA, PH.D. <Krerk.P@chula.ac.th>");
MODULE_DESCRIPTION("\"cpuinfo\" Character Device");
#define DEVICENAME "cpuinfo"
static int dev_major;
static int dev_open = 0;
static char *f_ptr;
static char dest[1000];
static char buf[1000];
// prototypes for device functions
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
// File operations structor
// Only implement those that will be used.

static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
                                unsigned int *ecx, unsigned int *edx)
{
  /* ecx is often an input as well as an output. */
  asm volatile("cpuid"
               : "=a"(*eax),
                 "=b"(*ebx),
                 "=c"(*ecx),
                 "=d"(*edx)
               : "0"(*eax), "2"(*ecx)
               : "memory");
}

static struct file_operations dev_fops = {
    .read = device_read,
    .open = device_open,
    .release = device_release};
int init_module(void)
{
  printk(KERN_INFO "CPCHAR: dev cpuinfo init\n");
  dev_major = register_chrdev(0, DEVICENAME, &dev_fops);
  if (dev_major < 0)
  {
    printk(KERN_ALERT "Fail register_chrdev cpuinfo with %d\n", dev_major);
    return dev_major;
  }

  unsigned eax, ebx, ecx, edx;
  eax = 0;
  native_cpuid(&eax, &ebx, &ecx, &edx);
  snprintf(buf, 1000, "Vendor ID ", eax & 0xF);
  strncat(dest, buf, 1000);
  snprintf(buf, 1000, "%c%c%c%c", (ebx) & 0xFF, (ebx >> 8) & 0xFF, (ebx >> 16) & 0xFF, (ebx >> 24) & 0xFF);
  strncat(dest, buf, 1000);
  snprintf(buf, 1000, "%c%c%c%c", (edx) & 0xFF, (edx >> 8) & 0xFF, (edx >> 16) & 0xFF, (edx >> 24) & 0xFF);
  strncat(dest, buf, 1000);
  snprintf(buf, 1000, "%c%c%c%c\n", (ecx) & 0xFF, (ecx >> 8) & 0xFF, (ecx >> 16) & 0xFF, (ecx >> 24) & 0xFF);
  strncat(dest, buf, 1000);

  eax = 1;
  native_cpuid(&eax, &ebx, &ecx, &edx);
  snprintf(buf, 1000, "stepping %d\n", eax & 0xF);
  strncat(dest, buf, 1000);
  snprintf(buf, 1000, "model %d\n", (eax >> 4) & 0xF);
  strncat(dest, buf, 1000);
  snprintf(buf, 1000, "family %d\n", (eax >> 8) & 0xF);
  strncat(dest, buf, 1000);
  snprintf(buf, 1000, "processor type %d\n", (eax >> 12) & 0x3);
  strncat(dest, buf, 1000);
  snprintf(buf, 1000, "extended model %d\n", (eax >> 16) & 0xF);
  strncat(dest, buf, 1000);
  snprintf(buf, 1000, "extended family %d\n", (eax >> 20) & 0xFF);
  strncat(dest, buf, 1000);

  eax = 3;
  native_cpuid(&eax, &ebx, &ecx, &edx);
  snprintf(buf, 1000, "serial number 0x%08x%08x\n", edx, ecx);
  strncat(dest, buf, 1000);

  printk(KERN_INFO "Device MajorNumber %d.\n", dev_major);
  printk(KERN_INFO "To create a device file:\n");
  printk(KERN_INFO "\t'mknod /dev/%s c %d 0'.\n", DEVICENAME, dev_major);
  printk(KERN_INFO "Try varying minor numbers.\n");
  printk(KERN_INFO "Please remove the device file and module when done.\n");
  /* * non 0 - means init_module failed */
  return 0;
}
void cleanup_module(void)
{
  printk(KERN_INFO "CPCHAR: dev cpuinfo cleanup\n");
  unregister_chrdev(dev_major, DEVICENAME);
}
static int device_open(struct inode *inode, struct file *file)
{
  if (dev_open)
    return -EBUSY;
  dev_open++;
  printk(KERN_INFO "dev minor %d\n", MINOR(inode->i_rdev));

  f_ptr = (char *)dest;

  // lock module
  try_module_get(THIS_MODULE);
  return 0;
}
static int device_release(struct inode *inode, struct file *file)
{
  dev_open--; /* We're now ready for our next caller */
  // release module
  module_put(THIS_MODULE);
  return 0;
}
static ssize_t device_read(struct file *filp,
                           char *buffer,
                           /* see include/linux/fs.h */
                           /* buffer to fill with data */
                           /* length of the buffer */
                           size_t length,
                           loff_t *offset)
{
  int bytes_read = 0;
  if (*f_ptr == 0)
  {
    return 0;
  }
  while (length && *f_ptr)
  {
    put_user(*(f_ptr++), buffer++);
    length--;
    bytes_read++;
  }
  return bytes_read;
}