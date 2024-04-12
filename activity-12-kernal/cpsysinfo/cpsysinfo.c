// cpsysinfo.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <linux/mman.h>
#include <linux/mmzone.h>
#include <linux/memblock.h>
#include <linux/proc_fs.h>
#include <linux/percpu.h>
#include <linux/seq_file.h>
#include <linux/swap.h>
#include <linux/vmstat.h>
#include <linux/atomic.h>
#include <linux/vmalloc.h>
#ifdef CONFIG_CMA
#include <linux/cma.h>
#endif
#include <asm/page.h>
/* Needed by all modules */
/* Needed for KERN_INFO */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("KRERK PIROMSOPA, PH.D. <Krerk.P@chula.ac.th>");
MODULE_DESCRIPTION("\"cpsysinfo\" Character Device");
#define DEVICENAME "cpsysinfo"
static int dev_major;
static int dev_open = 0;
static char *f_ptr;
static char buf_procs[1000];
static char buf_mem[1000];
struct task_struct *task;
static unsigned long tm, fm, am;
struct seq_file *m;
void *v;
// prototypes for device functions
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);

// Get meminfo
static int meminfo_proc_show(struct seq_file *m, void *v)
{
  struct sysinfo i;
  unsigned long committed;
  long cached;
  long available;
  unsigned long pages[NR_LRU_LISTS];
  unsigned long sreclaimable, sunreclaim;
  int lru;

  si_meminfo(&i);
  committed = vm_memory_committed();

  cached = global_node_page_state(NR_FILE_PAGES) -
           total_swapcache_pages() - i.bufferram;
  if (cached < 0)
    cached = 0;

  for (lru = LRU_BASE; lru < NR_LRU_LISTS; lru++)
    pages[lru] = global_node_page_state(NR_LRU_BASE + lru);

  available = si_mem_available();
  sreclaimable = global_node_page_state_pages(NR_SLAB_RECLAIMABLE_B);
  sunreclaim = global_node_page_state_pages(NR_SLAB_UNRECLAIMABLE_B);

  tm = i.totalram;
  fm = i.freeram;
  am = available;

  return 0;
}

// File operations structor
// Only implement those that will be used.
static struct file_operations dev_fops = {
    .read = device_read,
    .open = device_open,
    .release = device_release};

int init_module(void)
{
  printk(KERN_INFO "CPCHAR: dev cpsysinfo init\n");
  dev_major = register_chrdev(0, DEVICENAME, &dev_fops);
  if (dev_major < 0)
  {
    printk(KERN_ALERT "Fail register_chrdev cpsysinfo with %d\n", dev_major);
    return dev_major;
  }

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
  printk(KERN_INFO "CPCHAR: dev cpsysinfo cleanup\n");
  unregister_chrdev(dev_major, DEVICENAME);
}
static int device_open(struct inode *inode, struct file *file)
{
  if (dev_open)
    return -EBUSY;
  dev_open++;
  printk(KERN_INFO "dev minor %d\n", MINOR(inode->i_rdev));

  if (MINOR(inode->i_rdev) == 0) // active processes
  {
    buf_procs[0] = '\0';
    for_each_process(task)
    {
      snprintf(buf_procs + strlen(buf_procs), sizeof(buf_procs) - strlen(buf_procs), "%d,%s \n", task->pid, task->comm);
    }
    f_ptr = (char *)buf_procs;
  }

  meminfo_proc_show(m, v);

  if (MINOR(inode->i_rdev) == 1) // amount of memory
  {
    buf_mem[0] = '\0';
    snprintf(buf_mem + strlen(buf_mem), sizeof(buf_mem) - strlen(buf_mem), "MemTotal: %ld kB\n", tm);
    snprintf(buf_mem + strlen(buf_mem), sizeof(buf_mem) - strlen(buf_mem), "MemFree: %ld kB\n", fm);
    snprintf(buf_mem + strlen(buf_mem), sizeof(buf_mem) - strlen(buf_mem), "MemAvailable: %ld kB\n", am);
    f_ptr = (char *)buf_mem;
  }

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