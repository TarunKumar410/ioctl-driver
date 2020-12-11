#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>//for kernel module
#include<linux/kdev_t.h>//for device file
#include<linux/fs.h> // file structure
#include<linux/cdev.h> //for cdev struct
#include<linux/device.h>
#include<linux/slab.h> // for kmalloc memory
#include<linux/uaccess.h>//for user_to_copy,copy_from_user, means communication b/t user space and kernel space
#include<linux/ioctl.h>//for input output contol for communicating b/w user space and kernel space

#define mem_size 1024

//define the ioctl code
#define WR_DATA _IOW('a','a',int32_t*)
#define RD_DATA _IOR('a','b',int32_t*)

int32_t val = 0;
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev my_cdev;//cdev structure

uint8_t *kernel_buffer;

static int my_open(struct inode *inode,struct file *file);
static int my_release(struct inode *inode,struct file *file);
static ssize_t my_read(struct file *filp,char __user *buf,size_t len,loff_t *off);
static ssize_t my_write(struct file *filp,const char __user *buf,size_t len,loff_t *off);
static long my_ioctl(struct file *filp,unsigned int cmd,unsigned long arg);


static struct file_operations fops = 
{
	.owner		= THIS_MODULE,
	.read		= my_read,
	.write		= my_write,
	.open		= my_open,
	.unlocked_ioctl = my_ioctl,
	.release 	= my_release,
}; 

static int my_open(struct inode *inode,struct file *file)
{
	//creating pyhsical memory
	if((kernel_buffer = kmalloc(mem_size,GFP_KERNEL))==0)
	{
		printk(KERN_INFO "cannot allocate the memory to the kernel\n");
		return -1;
	}
	printk(KERN_INFO "device file opened\n");
	return 0;
}
static int my_release(struct inode *inode,struct file *file)
{
	kfree(kernel_buffer);
	printk(KERN_INFO "device file release func\n");
	return 0;
}

static ssize_t my_read(struct file *filp,char __user *buf,size_t len,loff_t *off)
{
	copy_to_user(buf,kernel_buffer, mem_size);
	printk(KERN_INFO "device file read func\n");
	return 0;
}

static ssize_t my_write(struct file *filp,const char __user *buf,size_t len,loff_t *off){
	copy_from_user(kernel_buffer,buf,mem_size);
	printk(KERN_INFO "device file in write func\n");
	return len;
}

static long my_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	switch(cmd){
		case WR_DATA:
			copy_from_user(&val,(int32_t *)arg,sizeof(val));
			printk(KERN_INFO "in ioctl write func val = %d\n",val);
			break;
		case RD_DATA:
			copy_to_user((int32_t *)arg,&val,sizeof(val));
			printk(KERN_INFO "in ioctl read func\n");
			break;
	}
	return 0;
}

static int __init chr_driver_init(void)
{
	//allocating major number
	if((alloc_chrdev_region(&dev,0,1,"my_dev"))<0){
		printk(KERN_INFO "unable to create major number\n");
		return 0;
	}
	printk(KERN_INFO "major: %d minor : %d\n",MAJOR(dev),MINOR(dev));
	
	//create cdev structure "my_cdev" is our cdev strcture, "fops" is file operation struct
	cdev_init(&my_cdev,&fops);

	//adding character device to the system
	if((cdev_add(&my_cdev,dev,1))<0){
		printk(KERN_INFO "cannot add the device to the system\n");
		goto r_class;
	}

	//create struct class
	if((dev_class = class_create(THIS_MODULE,"my_class"))==NULL){
		printk(KERN_INFO "cannot create the struct class\n");
		goto r_class;
	}

	//create device
	if((device_create(dev_class,NULL,dev,NULL,"my_device"))==NULL){
		printk(KERN_INFO "cannot create device\n");
		goto r_device;
	}

	printk(KERN_INFO "device driver insert done sucessfully\n");
	return 0;
	
r_device:
	class_destroy(dev_class);

r_class:
	unregister_chrdev_region(dev,1);
	return -1;
}


void __exit chr_driver_exit(void){
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev,1);

	printk(KERN_INFO "device driver remove successfully\n");
}

module_init(chr_driver_init);
module_exit(chr_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("sample ioctl code(takes interger as input)");
MODULE_AUTHOR("TARUN");
MODULE_VERSION("1.1");

