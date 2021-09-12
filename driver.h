#ifndef BUFFER_DRIVER_H_
#define BUFFER_DRIVER_H_

#include <linux/init.h>
#include <linux/module.h>

int allocate_new_buffer(size_t buffer_size);
static int buffer_init(void);
static void buffer_exit(void);
static int buffer_open(struct inode *inode, struct file *file);
static int buffer_release(struct inode *inode, struct file *file);
static ssize_t buffer_read(struct file *file_ptr, char __user *user_buffer, size_t count, loff_t *position);
static ssize_t buffer_write(struct file *file_ptr, const char __user *user_buffer, size_t count, loff_t *position);

#endif // BUFFER_DRIVER_H_
