#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#define DRIVER_NAME "ads1115_driver"
#define CLASS_NAME "ads1115"
#define DEVICE_NAME "ads1115"

#define ADS1115_REG_CONVERSION 0x00 // Register address for conversion result
#define ADS1115_REG_CONFIG     0x01 // Register address for configuration

#define ADS1115_IOCTL_MAGIC 'a'
#define ADS1115_IOCTL_READ_CONVERSION _IOR(ADS1115_IOCTL_MAGIC, 1, int) // IOCTL command to read conversion

static struct i2c_client *ads1115_client; // I2C client structure
static struct class *ads1115_class = NULL; // Device class for the driver
static struct device *ads1115_device = NULL; // Device structure for the driver
static int major_number; // Major number for device file

// Configure the ADS1115 to start a conversion on AIN0-GND in single-shot mode
static int ads1115_configure(struct i2c_client *client) {
    u8 config[3];
    config[0] = ADS1115_REG_CONFIG;
    config[1] = 0xC2; // OS = 1 (start conversion), MUX = 100 (AIN0-GND), PGA = 2.048V, MODE = single-shot
    config[2] = 0x83; // DR = 128SPS, disable comparator

    return i2c_master_send(client, config, 3); // Send configuration command to ADS1115
}

// Read 16-bit conversion result from ADS1115
static int ads1115_read_conversion(struct i2c_client *client) {
    u8 reg = ADS1115_REG_CONVERSION;
    u8 buf[2];
    int ret;

    ret = ads1115_configure(client); // Start conversion
    if (ret < 0) return ret;

    msleep(10); // Wait for the conversion to complete

    ret = i2c_master_send(client, &reg, 1); // Send read command for conversion register
    if (ret < 0) return ret;

    ret = i2c_master_recv(client, buf, 2); // Receive the 16-bit conversion result
    if (ret < 0) return ret;

    return (buf[0] << 8) | buf[1]; // Combine the two 8-bit bytes into a 16-bit result
}

// IOCTL interface for reading conversion results
static long ads1115_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int data;
    int ret;

    switch (cmd) {
        case ADS1115_IOCTL_READ_CONVERSION:
            data = ads1115_read_conversion(ads1115_client); // Read conversion result
            if (data < 0) return data; // Return error if the read failed
            break;
        default:
            return -EINVAL; // Return invalid argument error if command is unrecognized
    }

    ret = copy_to_user((int __user *)arg, &data, sizeof(data)); // Copy data to user space
    if (ret) return -EFAULT; // Return error if copy fails

    return 0; // Success
}

// Open the device (called when the device is opened by a user-space program)
static int ads1115_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "ADS1115: Device opened\n");
    return 0;
}

// Release the device (called when the device is closed by a user-space program)
static int ads1115_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "ADS1115: Device closed\n");
    return 0;
}

// Read from the device (called when user-space program reads from the device file)
static ssize_t ads1115_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    int data;
    char data_str[16];
    int ret;

    data = ads1115_read_conversion(ads1115_client); // Read conversion result
    if (data < 0) return data; // Return error if read fails

    ret = snprintf(data_str, sizeof(data_str), "%d\n", data); // Convert data to string
    if (ret < 0) return -EFAULT; // Return error if string formatting fails

    if (len < ret) return -EINVAL; // Return error if buffer size is insufficient

    if (copy_to_user(buf, data_str, ret)) return -EFAULT; // Copy string to user space

    return ret; // Return number of bytes written
}

// File operations structure for the driver
static struct file_operations fops = {
    .open = ads1115_open,
    .read = ads1115_read,
    .unlocked_ioctl = ads1115_ioctl,
    .release = ads1115_release,
};

// Probe function called when the I2C device is detected
static int ads1115_probe(struct i2c_client *client, const struct i2c_device_id *id) {
    int ret;
    ads1115_client = client;

    ret = register_chrdev(0, DEVICE_NAME, &fops); // Register the character device
    if (ret < 0) return ret;

    major_number = ret;
    ads1115_class = class_create(THIS_MODULE, CLASS_NAME); // Create device class
    if (IS_ERR(ads1115_class)) {
        unregister_chrdev(major_number, DEVICE_NAME); // Unregister device if class creation fails
        return PTR_ERR(ads1115_class);
    }

    ads1115_device = device_create(ads1115_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME); // Create the device
    if (IS_ERR(ads1115_device)) {
        class_destroy(ads1115_class); // Destroy class if device creation fails
        unregister_chrdev(major_number, DEVICE_NAME); // Unregister device
        return PTR_ERR(ads1115_device);
    }

    printk(KERN_INFO "ADS1115: Driver installed\n");
    return 0;
}

// Remove function called when the device is removed
static int ads1115_remove(struct i2c_client *client) {
    ads1115_client = NULL;
    device_destroy(ads1115_class, MKDEV(major_number, 0)); // Destroy device
    class_destroy(ads1115_class); // Destroy device class
    unregister_chrdev(major_number, DEVICE_NAME); // Unregister character device
    printk(KERN_INFO "ADS1115: Driver removed\n");
    return 0;
}

// Match table for device tree
static const struct of_device_id ads1115_of_match[] = {
    { .compatible = "texas_instruments,ads1115", },
    { },
};
MODULE_DEVICE_TABLE(of, ads1115_of_match);

// I2C device ID table
static const struct i2c_device_id ads1115_id[] = {
    { "ads1115", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, ads1115_id);

// I2C driver structure
static struct i2c_driver ads1115_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(ads1115_of_match),
    },
    .probe = ads1115_probe,
    .remove = ads1115_remove,
    .id_table = ads1115_id,
};

// Module initialization function
static int __init ads1115_init(void) {
    return i2c_add_driver(&ads1115_driver); // Register the I2C driver
}

// Module exit function
static void __exit ads1115_exit(void) {
    i2c_del_driver(&ads1115_driver); // Unregister the I2C driver
    printk(KERN_INFO "ADS1115: Module exited\n");
}

module_init(ads1115_init);
module_exit(ads1115_exit);

MODULE_AUTHOR("User");
MODULE_DESCRIPTION("ADS1115 I2C Client Driver with IOCTL Interface");
MODULE_LICENSE("GPL");
