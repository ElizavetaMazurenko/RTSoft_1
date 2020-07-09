#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/module.h> /* Содержит функции и определения для динамической загрузки модулей ядра */
#include <linux/init.h>  /* Указывает на функции инициализации и очистки */
#include <linux/fs.h>    /* Содержит функции регистрации и удаления драйвера */
#include <linux/cdev.h>  /* Содержит необходимые функции для символьного драйвера */
#include <linux/slab.h>  /* Содержит функцию ядра для управления памятью */
#include <asm/uaccess.h> /* Предоставляет доступ к пространству пользователя */
MODULE_AUTHOR("Elizaveta Mazurenko");
MODULE_LICENSE("GPL");

int SUCCESS = 0;
int Major;
char DEVICE_NAME[30];
int Device_Open;
char *msg_Ptr;

static int device_open(struct inode *inode, struct file *file){
char *msg;
static int counter = 0;
if (Device_Open)
return -EBUSY;
Device_Open++;
sprintf(msg, "I already told you %d times Hello world!\n", counter++);
msg_Ptr = msg;
try_module_get(THIS_MODULE);
return SUCCESS;
}


static int device_release(struct inode *inode, struct file *file){
Device_Open--; /* We're now ready for our next caller */
/** Decrement the usage count, or else once you opened the file, you'll
never get get rid of the module. */
module_put(THIS_MODULE);
return 0;
}

static ssize_t device_read(struct file *filp,
char __user *buffer, /* buffer to fill with data */
size_t length, /* length of the buffer */
loff_t * offset)
{
char *msg = "Hello world";
int bytes_read = 0; /* Number of bytes actually written to the buffer*/
/* If we're at the end of the message, * return 0 signifying end of file */
if (*msg_Ptr == 0) return 0;
/* Actually put the data into the buffer */
length = 11;
while (length && *msg_Ptr) {
/** The buffer is in the user data segment, not the kernel
* segment so "*" assignment won't work. We have to use
* put_user which copies data from the kernel data segment to
* the user data segment. */
put_user(*(msg++), buffer++);
length--;
bytes_read++;
}
/* Most read functions return the number of bytes put into the buffer */
return bytes_read;
}

struct file_operations fops = {
.read = device_read,
//.write = device_write,
.open = device_open,
.release = device_release
};


int s_init_module(void)
{
    Major = register_chrdev(0, DEVICE_NAME, &fops);
    if (Major < 0) {
    printk(KERN_ALERT "Registering char device failed with %d\n", Major);
    return Major;
}
    printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
    return SUCCESS;
}

void s_cleanup_module(void) {
/* Удаление символьного драйвера */
if (!Device_Open) {
unregister_chrdev(Major, DEVICE_NAME);
printk(KERN_ALERT "Сleanup_module OK \n");
}
else
printk(KERN_ALERT "Module is busy and cannot be cleared out \n");
}


module_init(s_init_module);
module_exit(s_cleanup_module);

