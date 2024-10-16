#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

/* Meta Information */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple LKM for a gpio interrupt");

#define GPIO_17 529         // GPIO 17 as input
#define GPIO_18 530         // GPIO 18 as output
#define GPIO_23 535         // GPIO 23 generates 5Hz signal
#define LED_DELAY_MS 100   // 100ms delay (5Hz signal = 1000ms / 5 / 2 = 100ms)

/** variable contains pin number o interrupt controller to which GPIO 17 is mapped to */
unsigned int irq_number;
static struct timer_list gpio23_timer; // Timer to generate 5Hz signal on GPIO 23
static bool gpio23_state = false;      // State of GPIO 23 for toggling
static bool gpio18_state = false;      // State of GPIO 18 for toggling

/**
 * @brief Interrupt service routine is called, when interrupt is triggered
 */
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    printk("gpio_irq: Interrupt was triggered and ISR was called!\n");

    // Toggle the state of GPIO 18
    gpio18_state = !gpio18_state;
    gpio_set_value(GPIO_18, gpio18_state);
    printk("gpio_irq: GPIO 18 state changed to: %d\n", gpio18_state);

    return IRQ_HANDLED;
}

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void)
{
    printk("gpio_irq: Loading module...\n");

    /* Setup GPIO 17 */
    if (gpio_request(GPIO_17, "rpi-gpio-17")) {
        printk("Error!\nCan not allocate GPIO 17\n");
        return -1;
    }

    /* Set GPIO 17 direction */
    if (gpio_direction_input(GPIO_17)) {
        printk("Error!\nCan not set GPIO 17 to input!\n");
        gpio_free(GPIO_17);
        return -1;
    }

    /* Setup GPIO 18 */
    if (gpio_request(GPIO_18, "rpi-gpio-18")) {
        printk("Error!\nCan not allocate GPIO 18\n");
        gpio_free(GPIO_17);
        return -1;
    }

    /* Set GPIO 18 direction to output */
    if (gpio_direction_output(GPIO_18, gpio18_state)) {
        printk("Error!\nCan not set GPIO 18 to output!\n");
        gpio_free(GPIO_17);
        gpio_free(GPIO_18);
        return -1;
    }

    /* Setup the interrupt for GPIO 17 */
    irq_number = gpio_to_irq(GPIO_17);

    if (request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0) {
        printk("Error!\nCan not request interrupt nr.: %d\n", irq_number);
        gpio_free(GPIO_17);
        gpio_free(GPIO_18);
        return -1;
    }

    printk("Done!\n");
    printk("GPIO 17 is mapped to IRQ Nr.: %d\n", irq_number);

    return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void)
{
    printk("gpio_irq: Unloading module...\n");
    free_irq(irq_number, NULL);
    gpio_free(GPIO_17);
    gpio_free(GPIO_18);
}

module_init(ModuleInit);
module_exit(ModuleExit);
