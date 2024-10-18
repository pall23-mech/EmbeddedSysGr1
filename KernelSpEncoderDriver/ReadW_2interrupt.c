#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

/* Meta Information */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple LKM for a GPIO interrupt and 5Hz oscillation");

#define GPIO_OUT 529     // Output toggled by interrupt in
#define GPIO_IN 532      // Input pin for interrupt
#define GPIO_22 534      // GPIO 22 set to oscillate at 5Hz
#define LED_DELAY_MS 100 // 5Hz = 100ms delay (1000ms / 5 / 2)

/** Variables */
unsigned int irq_number;
static struct hrtimer hr_timer;  // High-resolution timer
static bool gpio22_state = false;
ktime_t kt_period;  // Time period for the timer

/**
 * @brief Interrupt service routine is called, when interrupt is triggered
 */
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    static int value = 1;
    printk("gpio_irq: Interrupt was triggered and ISR was called!\n");
    gpio_set_value(GPIO_OUT, !value);
    value = !value;
    return IRQ_HANDLED;
}

/**
 * @brief High-resolution timer callback function to toggle GPIO 22 at 5Hz
 */
enum hrtimer_restart gpio22_timer_callback(struct hrtimer *timer)
{
    gpio22_state = !gpio22_state;                // Toggle the state
    printk("gpio_irq: GPIO 22 state toggled to: %d\n", gpio22_state);
    gpio_set_value(GPIO_22, gpio22_state);       // Set the GPIO 22 to new state

    // Re-arm the high-resolution timer for the next toggle
    hrtimer_forward_now(timer, kt_period);
    return HRTIMER_RESTART;
}

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) 
{
    printk("gpio_irq: Loading module...\n");

    /* Setup GPIO IN and OUT */
    if (gpio_request(GPIO_IN, "rpi-gpio-in")) { 
        printk("Error!\nCan not allocate GPIO %d\n", GPIO_IN);
        return -1;
    }

    if (gpio_request(GPIO_OUT, "rpi-gpio-out")) { 
        printk("Error!\nCan not allocate GPIO %d\n", GPIO_OUT);
        return -1;
    }

    if (gpio_request(GPIO_22, "rpi-gpio-22")) { 
        printk("Error!\nCan not allocate GPIO %d\n", GPIO_22);
        return -1;
    }

    /* Set GPIO directions */
    if (gpio_direction_input(GPIO_IN)) {
        printk("Error!\nCan not set GPIO %d to input!\n", GPIO_IN);
        gpio_free(GPIO_IN);
        return -1;
    }

    if (gpio_direction_output(GPIO_OUT, 0)) {
        printk("Error!\nCan not set GPIO %d to output!\n", GPIO_OUT);
        gpio_free(GPIO_OUT);
        return -1;
    }

    if (gpio_direction_output(GPIO_22, 0)) {
        printk("Error!\nCan not set GPIO %d to output!\n", GPIO_22);
        gpio_free(GPIO_22);
        return -1;
    }

    /* Setup the interrupt */
    irq_number = gpio_to_irq(GPIO_IN);

    if (request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "my_gpio_irq", NULL) != 0) {
        printk("Error!\nCan not request interrupt nr.: %d\n", irq_number);
        gpio_free(GPIO_IN);
        gpio_free(GPIO_OUT);
        gpio_free(GPIO_22);
        return -1;
    }

    /* Initialize the high-resolution timer for GPIO 22 5Hz oscillation */
    kt_period = ktime_set(0, LED_DELAY_MS * 1E6L);  // Set period in nanoseconds (100ms = 100,000,000ns)
    hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    hr_timer.function = &gpio22_timer_callback;
    hrtimer_start(&hr_timer, kt_period, HRTIMER_MODE_REL);

    printk("Done!\nGPIO %d is mapped to IRQ Nr.: %d\n", GPIO_IN, irq_number);

    return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) 
{
    printk("gpio_irq: Unloading module...\n");
    hrtimer_cancel(&hr_timer);  // Stop the high-resolution timer
    free_irq(irq_number, NULL);
    gpio_free(GPIO_IN);
    gpio_free(GPIO_OUT);
    gpio_free(GPIO_22);
}

module_init(ModuleInit);
module_exit(ModuleExit);
