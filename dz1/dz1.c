#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
MODULE_LICENSE("GPL");
int init_module(void) {
printk(KERN_INFO "The world is quiet here \n");
/*
* Модуль должен возвращать 0 в случае успешной загрузки.
*/
return 0;
}
void cleanup_module(void) {
printk(KERN_INFO "There is not happy ending not here and not now \n ");
}
