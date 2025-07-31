#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;
static int delay_time = 3;
static int default_time = 10;

#define DRIVER_NAME "step_driver"
#define DRIVER_CLASS "MotorModuleClass"

void set_value(int p1,int p2,int p3,int p4)
{
    gpio_set_value(12, p1);
    gpio_set_value(16, p2);
    gpio_set_value(20, p3);
    gpio_set_value(21, p4);
}

static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs)
{
    int to_copy, not_copied, delta, i;
    char value;

    to_copy = min(count, sizeof(value));

    not_copied = copy_from_user(&value, user_buffer, to_copy);

    switch(value){
        case 'R':
            for(i = 0; i < default_time; i++) {
                set_value(1, 1, 0, 0);
                msleep(delay_time);
                set_value(0, 1, 1, 0);
                msleep(delay_time);
                set_value(0, 0, 1, 1);
                msleep(delay_time);
                set_value(1, 0, 0, 1);
                msleep(delay_time);
            }
            break;
        case 'L':
            for(i = default_time; i > 0; i--) {
                set_value(1, 0, 0, 1);
                msleep(delay_time);
                set_value(0, 0, 1, 1);
                msleep(delay_time);
                set_value(0, 1, 1, 0);
                msleep(delay_time);
                set_value(1, 1, 0, 0);
                msleep(delay_time);
            }
            break;
        case 'S':
            set_value(0, 0, 0, 0);
            break;
        default:
            set_value(0, 0, 0, 0);
            break;
    }
    delta = to_copy - not_copied;
    return delta;
}

static int driver_open(struct inode *device_file, struct file *instance)
{
    printk(KERN_NOTICE "Stepper Motor - open was called!\n");
    return 0;
}

static int driver_close(struct inode *device_file, struct file *instance)
{
    printk(KERN_NOTICE "Stepper Motor - close was called!\n");
    return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open  = driver_open,
	.release = driver_close,
	.write = driver_write
};

static int __init ModuleInit(void) {
   printk("Hello, Motor_Driver!\n");

   /* Allocate a device nr */
   if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
      printk("Device Nr. could not be allocated!\n");
      return -1;
   }
   printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

   /* Create device class */
   if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
      printk("Device class can not e created!\n");
      goto ClassError;
   }

   /* create device file */
   if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
      printk("Can not create device file!\n");
      goto FileError;
   }

   /* Initialize device file */
   cdev_init(&my_device, &fops);

   /* Regisering device to kernel */
   if(cdev_add(&my_device, my_device_nr, 1) == -1) {
      printk("Registering of device to kernel failed!\n");
      goto AddError;
   }

    /* Set GPIO 12 */
    if(gpio_request(12, "rpi-gpio-12")) {
        printk("Can not allocate GPIO 12\n");
      goto AddError;
    }

    if(gpio_direction_output(12, 0)) {
        printk("Can not allocate GPIO 12\n");
        goto Gpio12Error;
    }

    /* Set GPIO 16 */
    if(gpio_request(16, "rpi-gpio-16")) {
        printk("Can not allocate GPIO 16\n");
      goto AddError;
    }

    if(gpio_direction_output(16, 0)) {
        printk("Can not allocate GPIO 16\n");
        goto Gpio16Error;
    }

    /* Set GPIO 20 */
    if(gpio_request(20, "rpi-gpio-20")) {
        printk("Can not allocate GPIO 20\n");
      goto AddError;
    }

    if(gpio_direction_output(20, 0)) {
        printk("Can not allocate GPIO 20\n");
        goto Gpio20Error;
    }

    /* Set GPIO 21 */
    if(gpio_request(21, "rpi-gpio-21")) {
        printk("Can not allocate GPIO 21\n");
      goto AddError;
    }

    if(gpio_direction_output(21, 0)) {
        printk("Can not allocate GPIO 21\n");
        goto Gpio21Error;
    }

    return 0;

Gpio12Error:
    gpio_free(12);
Gpio16Error:
    gpio_free(16);
Gpio20Error:
    gpio_free(20);
Gpio21Error:
    gpio_free(21);
AddError:
    device_destroy(my_class, my_device_nr);
FileError:
   class_destroy(my_class);
ClassError:
   unregister_chrdev_region(my_device_nr, 1);
   return -1;
}

static void __exit ModuleExit(void)
{
    gpio_set_value(12, 0);
    gpio_set_value(16, 0);
    gpio_set_value(20, 0);
    gpio_set_value(21, 0);
    gpio_free(12);
    gpio_free(16);
    gpio_free(20);
    gpio_free(21);

     cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev_region(my_device_nr, 1);
    printk("Goodbye, Motor_driver\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);