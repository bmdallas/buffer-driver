#include "driver.h"

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>

#define DRIVER_NAME "buffer-driver"
#define DEFAULT_BUFFER_SIZE 1024

static char *driver_buffer = NULL;
static size_t driver_buffer_length = 0;

struct class *driver_class;
struct cdev driver_cdev;
dev_t dev_num;

static struct file_operations buffer_file_ops =
{
  .owner = THIS_MODULE,
  .read = buffer_read,
  .write = buffer_write,
  .open = buffer_open,
  .release = buffer_release
};

static int buffer_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int buffer_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t buffer_write(struct file *file_ptr, const char __user *user_buffer, size_t len, loff_t *offset)
{
    size_t user_offset = 0;
    int return_value = 0;
    size_t actual_write_len = len;
    printk( KERN_NOTICE "Entering write with %ld bytes at offset %lld", len, *offset );
    if (actual_write_len > driver_buffer_length)
    {
        /* Only take the last overwrite of the buffer */
        user_offset = len - driver_buffer_length;
        actual_write_len = driver_buffer_length;
    }

    if (*offset + actual_write_len > driver_buffer_length)
    {
        size_t initial_copy = actual_write_len - *offset;
        return_value = copy_from_user(&driver_buffer[*offset], &user_buffer[user_offset], initial_copy);
        if (return_value == 0)
        {
            return_value = copy_from_user(driver_buffer, &user_buffer[user_offset + initial_copy], len - initial_copy);
        }

        if (return_value == 0)
        {
            return_value = actual_write_len;
        }
        else
        {
            return_value = -EFAULT;
        }
    }
    else
    {
        if (copy_from_user(&driver_buffer[*offset], &user_buffer[user_offset], len))
        {
            return_value = -EFAULT;
        }
        else
        {
            return_value = actual_write_len;
        }
    }

    printk( KERN_NOTICE "exiting write %d", return_value );
    return return_value;
}

static ssize_t buffer_read(struct file *file_ptr, char __user *user_buffer, size_t len, loff_t *offset)
{
    int return_value = 0;
    int actual_read_len = min(len, driver_buffer_length);

    if (*offset >= driver_buffer_length)
    {
        *offset = *offset % driver_buffer_length;
    }

    if (*offset + actual_read_len > driver_buffer_length)
    {
        size_t initial_copy = driver_buffer_length - *offset;
        size_t bytes_left = actual_read_len - initial_copy;
        size_t bytes_copied = 0;
        return_value = copy_to_user(user_buffer, &driver_buffer[*offset], initial_copy);
        bytes_copied = initial_copy;

        while (bytes_left != 0 && return_value == 0)
        {
            size_t copy_size = min(bytes_left, driver_buffer_length);
            return_value = copy_to_user(&user_buffer[bytes_copied], driver_buffer, copy_size);
            bytes_copied += copy_size;
            bytes_left -= copy_size;
        }

        if (return_value != 0)
        {
            return_value = -EFAULT;
        }
        else
        {
            return_value = actual_read_len;
        }
    }
    else
    {
        if (copy_to_user(user_buffer, &driver_buffer[*offset], actual_read_len))
        {
            return_value = -EFAULT;
        }
        else
        {
            return_value = actual_read_len;
        }
    }

    return return_value;
}

int allocate_new_buffer(size_t buffer_size)
{
    int return_value = 0;
    
    if (buffer_size <= 0)
    {
        return_value = -EFAULT;
    }
    else
    {
        char *buffer = kmalloc(buffer_size, GFP_KERNEL);
        
        if (NULL != driver_buffer && NULL != buffer)
        {
            size_t copy_size = min(buffer_size, driver_buffer_length);
            memcpy(buffer, driver_buffer, copy_size);
            kfree(driver_buffer);
            driver_buffer = NULL;
            driver_buffer_length = 0;
        }

        if (NULL != buffer)
        {
            driver_buffer = buffer;
            driver_buffer_length = buffer_size;
        }
        else
        {
            return_value = -EAGAIN;
        }
    }
    
    return return_value;
}

static int __init buffer_init(void)
{
    /* Assume success until proven otherwise */
    int init_result = 0;
    dev_t curr_dev;

    alloc_chrdev_region(&dev_num, 0, 1, DRIVER_NAME);

    driver_class = class_create(THIS_MODULE, DRIVER_NAME "_class");

    cdev_init(&driver_cdev, &buffer_file_ops);

    curr_dev = MKDEV(MAJOR(dev_num), MINOR(dev_num));

    device_create(driver_class, NULL, curr_dev, NULL, "buffer_driver0");

    cdev_add(&driver_cdev, curr_dev, 1); 

    if (init_result < 0)
    {
        printk(KERN_ERR "Failed to load %s driver while registering character device: %i\n", DRIVER_NAME, init_result);
    }
    else
    {
        printk(KERN_NOTICE "Registered %s with major number %i\n", DRIVER_NAME, init_result);
        init_result = allocate_new_buffer(DEFAULT_BUFFER_SIZE);
    }
    
    return init_result;
}
    
static void __exit buffer_exit(void)
{
    printk(KERN_NOTICE "Unregistering %s", DRIVER_NAME);

    cdev_del(&driver_cdev); 

    device_destroy(driver_class, dev_num);

    class_destroy(driver_class);

    unregister_chrdev_region(dev_num, 1);

    if (NULL != driver_buffer)
    {
        kfree(driver_buffer);
        driver_buffer = NULL;
        driver_buffer_length = 0;
    }
}
    
module_init(buffer_init);
module_exit(buffer_exit);

MODULE_LICENSE("GPL");
