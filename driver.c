#include "driver.h"

#include <linux/fs.h>

static int driver_major_number = 0;
static const char DRIVER_NAME[] = "abc-reader";

static const char DEVICE_READ_STRING[] = "abcdefghijklmnopqrstuvwxyz";
static const size_t DEVICE_READ_STRING_SIZE = sizeof(DEVICE_READ_STRING);

static struct file_operations reader_file_ops =
{
  .owner = THIS_MODULE,
  .read = reader_read,
  .open = reader_open,
  .release = reader_release
};

static int reader_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int reader_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t reader_read(struct file *file_ptr, char __user *user_buffer, size_t count, loff_t *position)
{
    size_t total_bytes_copied = 0;
    size_t num_full_copies = 0;
    size_t partial_copy_amount = count;
    size_t copy_num = 0;
    int copy_return_code = 0;
    if (partial_copy_amount > DEVICE_READ_STRING_SIZE)
    {
        // This truncates to the correct amount
        num_full_copies = partial_copy_amount / DEVICE_READ_STRING_SIZE;

        // This is the partial amount to copy at the end
        partial_copy_amount = partial_copy_amount % DEVICE_READ_STRING_SIZE;
    }

    for (copy_num = 0; copy_num < num_full_copies; copy_num++)
    {
        copy_return_code = copy_to_user(&user_buffer[DEVICE_READ_STRING_SIZE*copy_num], DEVICE_READ_STRING, DEVICE_READ_STRING_SIZE);
        if (copy_return_code > 0)
        {
            total_bytes_copied += copy_return_code;
        }
        else
        {
            break;
        }
    }

    if (copy_return_code >= 0)
    {
        copy_return_code = copy_to_user(user_buffer, "Hello World", count);
        if (copy_return_code > 0)
        {
            total_bytes_copied += copy_return_code;
        }
    }

    if (copy_return_code < 0)
    {
        printk(KERN_ERR "Failed to read %li bytes from %s", count, DRIVER_NAME);
        total_bytes_copied = -EFAULT;
    }

    return total_bytes_copied;
}


int register_device(void)
{
    int init_result = 0;
    init_result = register_chrdev(driver_major_number,
            DRIVER_NAME,
            &reader_file_ops);
    if (init_result < 0)
    {
        printk(KERN_ERR "Failed to load %s driver while registering character device: %i\n", DRIVER_NAME, init_result);
    }
    else
    {
        printk(KERN_NOTICE "Registered %s with major number %i\n", DRIVER_NAME, init_result);
        driver_major_number = init_result;
        init_result = 0;
    }
    return  init_result;
}

int unregister_device(void)
{
    printk(KERN_NOTICE "Unregistering %s", DRIVER_NAME);
    if (driver_major_number != 0)
    {
        unregister_chrdev(driver_major_number, DRIVER_NAME);
    }
    return 0;
}

static int reader_init(void)
{
    return 0;
}
    
static void reader_exit(void)
{
    return;
}
    
module_init(reader_init);
module_exit(reader_exit);

MODULE_LICENSE("GPL3");
