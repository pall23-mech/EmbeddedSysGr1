#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/timer.h>

// Meta Information
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple LKM for GPIO interrupt and 5Hz signal generation");

#define GPIO_17 529 // GPIO 17 as input (interrupt trigger)
#define GPIO_18 530 // GPIO 18 as output (toggle on interrupt)
#define GPIO_23 535 // GPIO 23 generates a 5Hz signal
#define LED_DELAY_MS 100 // 100ms delay for 5Hz signal (1000ms / 5Hz / 2 = 100ms)

unsigned int irq_number;  // Store IRQ number for GPIO 17
static struct timer_list gpio23_timer; // Timer for generating 5Hz signal on GPIO 23
static bool gpio23_state = false;      // State of GPIO 23
static bool gpio18_state = false;      // State of GPIO 18

// Timer function to toggle GPIO 23 (5Hz signal generation)
static void gpio23_timer_function(struct timer_list *timer)
{
    gpio23_state = !gpio23_state;  // Toggle GPIO 23 state
    gpio_set_value(GPIO_23, gpio23_state);  // Set GPIO 23 to the new state
    mod_timer(&gpio23_timer, jiffies + msecs_to_jiffies(LED_DELAY_MS)); // Re-arm the timer
}

// Interrupt service routine called when an interrupt is triggered on GPIO 17
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    gpio18_state = !gpio18_state;  // Toggle GPIO 18 state
    gpio_set_value(GPIO_18, gpio18_state);  // Set GPIO 18 to the new state
    return IRQ_HANDLED;
}

// This function is called when the module is loaded into the kernel
static int __init ModuleInit(void)
{
    int result;

    // Print kernel message that the module is loading
    printk(KERN_INFO "gpio_irq: Loading module...");

    // Setup GPIO 17 as input
    result = gpio_request(GPIO_17, "rpi-gpio-17");
    if (result) {
        printk(KERN_ERR "Error! Cannot allocate GPIO 17\n");
        return result;
    }

    result = gpio_direction_input(GPIO_17);
    if (result) {
        printk(KERN_ERR "Error! Cannot set GPIO 17 to input!\n");
        gpio_free(GPIO_17);
        return result;
    }

    // Setup GPIO 18 as output
    result = gpio_request(GPIO_18, "rpi-gpio-18");
    if (result) {
        printk(KERN_ERR "Error! Cannot allocate GPIO 18\n");
        gpio_free(GPIO_17);
        return result;
    }
    gpio_direction_output(GPIO_18, gpio18_state);  // Set GPIO 18 as output, initially low

    // Setup GPIO 23 as output for 5Hz signal
    result = gpio_request(GPIO_23, "rpi-gpio-23");
    if (result) {
        printk(KERN_ERR "Error! Cannot allocate GPIO 23\n");
        gpio_free(GPIO_17);
        gpio_free(GPIO_18);
        return result;
    }
    gpio_direction_output(GPIO_23, gpio23_state);  // Set GPIO 23 as output, initially low

    // Setup the timer for 5Hz signal generation on GPIO 23
    timer_setup(&gpio23_timer, gpio23_timer_function, 0);  // Initialize the timer
    mod_timer(&gpio23_timer, jiffies + msecs_to_jiffies(LED_DELAY_MS));  // Start the timer

    // Setup the interrupt for GPIO 17
    irq_number = gpio_to_irq(GPIO_17);
    if (irq_number < 0) {
        printk(KERN_ERR "Error! Failed to get IRQ for GPIO 17\n");
        gpio_free(GPIO_17);
        gpio_free(GPIO_18);
        gpio_free(GPIO_23);
        return irq_number;
    }

    result = request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL);
    if (result) {
        printk(KERN_ERR "Error! Cannot request IRQ for GPIO 17: %d\n", irq_number);
        gpio_free(GPIO_17);
        gpio_free(GPIO_18);
        gpio_free(GPIO_23);
        return result;
    }

    printk(KERN_INFO "Done! GPIO 17 is mapped to IRQ number: %d\n", irq_number);

    return 0;
}

// This function is called when the module is removed from the kernel
static void __exit ModuleExit(void)
{
    printk(KERN_INFO "gpio_irq: Unloading module...");

    // Remove the 5Hz signal timer for GPIO 23
    del_timer(&gpio23_timer);

    // Free the IRQ and GPIOs
    free_irq(irq_number, NULL);
    gpio_free(GPIO_17);
    gpio_free(GPIO_18);
    gpio_free(GPIO_23);
}

module_init(ModuleInit);
module_exit(ModuleExit);
