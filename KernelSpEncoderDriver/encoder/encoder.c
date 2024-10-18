#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Encoder counter using GPIO interrupts");

#define GPIO_A 532  // Encoder Channel A (e.g., GPIO 20)
#define GPIO_B 533  // Encoder Channel B (e.g., GPIO 21)
#define DEVICE_NAME "encoder_counter"
#define CLASS_NAME "encoder_class"

static int encoder_position = 0;    // Encoder pulse counter
static int irq_number_a, irq_number_b;  // IRQ numbers for channels A and B
static dev_t dev_num;
static struct cdev encoder_cdev;
static struct class *encoder_class = NULL;
static bool last_state_a = false;   // Last state of Channel A
static bool last_state_b = false;   // Last state of Channel B

/**
 * @brief Interrupt handler for GPIO_A (Channel A).
 */
static irqreturn_t gpio_irq_handler_a(int irq, void *dev_id)
{
    // Read the current state of both encoder channels
    bool state_a = gpio_get_value(GPIO_A);
    bool state_b = gpio_get_value(GPIO_B);

    // Quadrature decoding logic: if Channel A changes state
    if (state_a != last_state_a) {
        if (state_a != state_b) {
            encoder_position++;  // Clockwise rotation
        } else {
            encoder_position--;  // Counterclockwise rotation
        }
        printk(KERN_INFO "Encoder position: %d\n", encoder_position);
    }

    last_state_a = state_a;
    last_state_b = state_b;
    return IRQ_HANDLED;
}

/**
 * @brief Interrupt handler for GPIO_B (Channel B).
 */
static irqreturn_t gpio_irq_handler_b(int irq, void *dev_id)
{
    // Read the current state of both encoder channels
    bool state_a = gpio_get_value(GPIO_A);
    bool state_b = gpio_get_value(GPIO_B);

    // Quadrature decoding logic: if Channel B changes state
    if (state_b != last_state_b) {
        if (state_a == state_b) {
            encoder_position++;  // Clockwise rotation
        } else {
            encoder_position--;  // Counterclockwise rotation
        }
        printk(KERN_INFO "Encoder position: %d\n", encoder_position);
    }

    last_state_a = state_a;
    last_state_b = state_b;
    return IRQ_HANDLED;
}

/**
 * @brief Read function for the character device to return the current encoder position to user-space.
 */
static ssize_t encoder_read(struct file *file, char __user *buffer, size_t len, loff_t *offset)
{
    char count_str[10];
    int count_len;

    if (*offset > 0) {
        return 0;  // EOF
    }

    count_len = sprintf(count_str, "%d\n", encoder_position);
    if (copy_to_user(buffer, count_str, count_len)) {
        return -EFAULT;
    }

    *offset = count_len;
    return count_len;
}

/**
 * @brief File operations for the character device.
 */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = encoder_read,
};

/**
 * @brief Initialize the module, setup the GPIOs, and interrupts for both channels.
 */
static int __init encoder_init(void)
{
    int result;

    printk(KERN_INFO "Encoder counter: Initializing module...\n");

    // Request GPIOs
    if (gpio_request(GPIO_A, "encoder_a") || gpio_request(GPIO_B, "encoder_b")) {
        printk(KERN_ERR "Error requesting GPIOs for encoder channels\n");
        return -1;
    }

    // Set GPIO direction
    gpio_direction_input(GPIO_A);
    gpio_direction_input(GPIO_B);

    // Setup interrupt for Channel A
    irq_number_a = gpio_to_irq(GPIO_A);
    if (request_irq(irq_number_a, gpio_irq_handler_a, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "encoder_irq_a", NULL)) {
        printk(KERN_ERR "Error requesting interrupt for GPIO A\n");
        gpio_free(GPIO_A);
        gpio_free(GPIO_B);
        return -1;
    }

    // Setup interrupt for Channel B
    irq_number_b = gpio_to_irq(GPIO_B);
    if (request_irq(irq_number_b, gpio_irq_handler_b, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "encoder_irq_b", NULL)) {
        printk(KERN_ERR "Error requesting interrupt for GPIO B\n");
        free_irq(irq_number_a, NULL);
        gpio_free(GPIO_A);
        gpio_free(GPIO_B);
        return -1;
    }

    // Register character device
    result = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (result < 0) {
        printk(KERN_ERR "Failed to allocate character device region\n");
        free_irq(irq_number_a, NULL);
        free_irq(irq_number_b, NULL);
        gpio_free(GPIO_A);
        gpio_free(GPIO_B);
        return result;
    }

    cdev_init(&encoder_cdev, &fops);
    if (cdev_add(&encoder_cdev, dev_num, 1) < 0) {
        printk(KERN_ERR "Failed to add cdev\n");
        unregister_chrdev_region(dev_num, 1);
        free_irq(irq_number_a, NULL);
        free_irq(irq_number_b, NULL);
        gpio_free(GPIO_A);
        gpio_free(GPIO_B);
        return -1;
    }

    // Create device class
encoder_class = class_create(CLASS_NAME);

if (IS_ERR(encoder_class)) {
    printk(KERN_ERR "Failed to create device class\n");
    cdev_del(&encoder_cdev);
    unregister_chrdev_region(dev_num, 1);
    free_irq(irq_number_a, NULL);
    free_irq(irq_number_b, NULL);
    gpio_free(GPIO_A);
    gpio_free(GPIO_B);
    return PTR_ERR(encoder_class);
}


    // Create device node
    if (IS_ERR(device_create(encoder_class, NULL, dev_num, NULL, DEVICE_NAME))) {
        printk(KERN_ERR "Failed to create device\n");
        class_destroy(encoder_class);
        cdev_del(&encoder_cdev);
        unregister_chrdev_region(dev_num, 1);
        free_irq(irq_number_a, NULL);
        free_irq(irq_number_b, NULL);
        gpio_free(GPIO_A);
        gpio_free(GPIO_B);
        return -1;
    }

    printk(KERN_INFO "Encoder counter: Module loaded\n");
    return 0;
}

/**
 * @brief Clean up and free resources when the module is unloaded.
 */
static void __exit encoder_exit(void)
{
    device_destroy(encoder_class, dev_num);
    class_destroy(encoder_class);
    cdev_del(&encoder_cdev);
    unregister_chrdev_region(dev_num, 1);
    free_irq(irq_number_a, NULL);
    free_irq(irq_number_b, NULL);
    gpio_free(GPIO_A);
    gpio_free(GPIO_B);

    printk(KERN_INFO "Encoder counter: Module unloaded\n");
}

module_init(encoder_init);
module_exit(encoder_exit);
