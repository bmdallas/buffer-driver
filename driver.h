#ifndef ABC_READER_DRIVER_H_
#define ABC_READER_DRIVER_H_

#include <linux/init.h>
#include <linux/module.h>

int register_device(void);
int unregister_device(void);
static int reader_init(void);
static void reader_exit(void);
static int reader_open(struct inode *inode, struct file *file);
static int reader_release(struct inode *inode, struct file *file);
static ssize_t reader_read(struct file *file_ptr, char __user *user_buffer, size_t count, loff_t *position);

#endif // ABC_READER_DRIVER_H_
