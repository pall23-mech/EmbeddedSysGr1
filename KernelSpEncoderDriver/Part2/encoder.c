#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/kobject.h> // For sysfs

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Encoder counter with PPS and RPM calculation using GPIO interrupts and sysfs");

#define GPIO_A 532  // Encoder Channel A (e.g., GPIO 20)
#define GPIO_B 533  // Encoder Channel B (e.g., GPIO 21)
#define PULSES_PER_REV 1400 // Number of pulses per revolution

static int encoder_position = 0;       // Encoder pulse counter
static unsigned long last_jiffies = 0; // Last jiffies timestamp when the position was updated
static int last_position = 0;          // Last encoder position
static int irq_number_a, irq_number_b; // IRQ numbers for channels A and B
static bool last_state_a = false;      // Last state of Channel A
static bool last_state_b = false;      // Last state of Channel B

static int pps = 0;  // Pulses per second
static int rpm = 0;  // Revolutions per minute

static struct kobject *encoder_kobj;

// Function declarations
void updateSpeed(void);
static ssize_t encoder_position_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t encoder_pps_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t encoder_rpm_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);

/**
 * @brief Interrupt handler for GPIO_A (Channel A).
 */
static irqreturn_t gpio_irq_handler_a(int irq, void *dev_id)
{
    bool state_a = gpio_get_value(GPIO_A);
    bool state_b = gpio_get_value(GPIO_B);

    // Quadrature decoding logic: if Channel A changes state
    if (state_a != last_state_a) {
        if (state_a != state_b) {
            encoder_position++;  // Clockwise rotation
        } else {
            encoder_position--;  // Counterclockwise rotation
        }
        updateSpeed();  // Update the speed whenever the position changes
    }

    // Update last states
    last_state_a = state_a;
    last_state_b = state_b;
    return IRQ_HANDLED;
}

/**
 * @brief Interrupt handler for GPIO_B (Channel B).
 */
static irqreturn_t gpio_irq_handler_b(int irq, void *dev_id)
{
    bool state_a = gpio_get_value(GPIO_A);
    bool state_b = gpio_get_value(GPIO_B);

    // Quadrature decoding logic: if Channel B changes state
    if (state_b != last_state_b) {
        if (state_a == state_b) {
            encoder_position++;  // Clockwise rotation
        } else {
            encoder_position--;  // Counterclockwise rotation
        }
        updateSpeed();  // Update the speed whenever the position changes
    }

    // Update last states
    last_state_a = state_a;
    last_state_b = state_b;
    return IRQ_HANDLED;
}

/**
 * @brief Function to update speed in PPS and RPM using fixed-point arithmetic.
 */
void updateSpeed(void)
{
    unsigned long current_jiffies = jiffies;
    unsigned long delta_jiffies = current_jiffies - last_jiffies;

    // Convert delta_jiffies to milliseconds
    unsigned long delta_time_ms = (delta_jiffies * 1000) / HZ;

    if (delta_time_ms > 0) {
        int delta_position = encoder_position - last_position;
        pps = (delta_position * 1000) / delta_time_ms; // Calculate PPS correctly
        rpm = (pps * 60) / PULSES_PER_REV;             // Calculate RPM using the PPS value

        // Update last position and time
        last_position = encoder_position;
        last_jiffies = current_jiffies;
    }
}

/**
 * @brief Show function for encoder position.
 */
static ssize_t encoder_position_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", encoder_position);
}

/**
 * @brief Show function for PPS.
 */
static ssize_t encoder_pps_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", pps);
}

/**
 * @brief Show function for RPM.
 */
static ssize_t encoder_rpm_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", rpm);
}

// Define attributes for sysfs
static struct kobj_attribute encoder_position_attr = __ATTR(position, 0444, encoder_position_show, NULL);
static struct kobj_attribute encoder_pps_attr = __ATTR(pps, 0444, encoder_pps_show, NULL);
static struct kobj_attribute encoder_rpm_attr = __ATTR(rpm, 0444, encoder_rpm_show, NULL);

static struct attribute *encoder_attrs[] = {
    &encoder_position_attr.attr,
    &encoder_pps_attr.attr,
    &encoder_rpm_attr.attr,
    NULL,
};

static struct attribute_group encoder_attr_group = {
    .attrs = encoder_attrs,
};

/**
 * @brief Initialize the module, setup the GPIOs, and interrupts for both channels.
 */
static int __init encoder_init(void)
{
    int retval;

    printk(KERN_INFO "Encoder counter: Initializing module...\n");

    encoder_kobj = kobject_create_and_add("encoder", kernel_kobj);
    if (!encoder_kobj) {
        printk(KERN_ERR "Failed to create sysfs entry\n");
        return -ENOMEM;
    }

    retval = sysfs_create_group(encoder_kobj, &encoder_attr_group);
    if (retval) {
        printk(KERN_ERR "Failed to create sysfs group\n");
        kobject_put(encoder_kobj);
        return retval;
    }

    if (gpio_request(GPIO_A, "encoder_a") || gpio_request(GPIO_B, "encoder_b")) {
        printk(KERN_ERR "Error requesting GPIOs for encoder channels\n");
        return -1;
    }

    gpio_direction_input(GPIO_A);
    gpio_direction_input(GPIO_B);

    irq_number_a = gpio_to_irq(GPIO_A);
    if (request_irq(irq_number_a, gpio_irq_handler_a, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "encoder_irq_a", NULL)) {
        printk(KERN_ERR "Error requesting interrupt for GPIO A\n");
        gpio_free(GPIO_A);
        gpio_free(GPIO_B);
        return -1;
    }

    irq_number_b = gpio_to_irq(GPIO_B);
    if (request_irq(irq_number_b, gpio_irq_handler_b, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "encoder_irq_b", NULL)) {
        printk(KERN_ERR "Error requesting interrupt for GPIO B\n");
        free_irq(irq_number_a, NULL);
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
    kobject_put(encoder_kobj);
    free_irq(irq_number_a, NULL);
    free_irq(irq_number_b, NULL);
    gpio_free(GPIO_A);
    gpio_free(GPIO_B);

    printk(KERN_INFO "Encoder counter: Module unloaded\n");
}

module_init(encoder_init);
module_exit(encoder_exit);
