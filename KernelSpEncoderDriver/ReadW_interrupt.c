#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

/* Meta Information */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple LKM for a gpio interrupt");

#define GPIO_17 529 // according to /sys/kernel/debug/gpio
#define GPIO_18 530         // GPIO 18 as output
#define GPIO_23 535         // GPIO 23 generates 5Hz signal
#define LED_DELAY_MS 100   // 100ms delay (5Hz signal = 1000ms / 5 / 2 = 100ms)
/** variable contains pin number o interrupt controller to which GPIO 17 is mapped to */
unsigned int irq_number;
static struct timer_list gpio23_timer; // Timer for generating 5Hz signal on GPIO 23
static bool gpio23_state = false;      // State of GPIO 23
static bool gpio18_state = false;      // State of GPIO 18

/**
 * @brief Interrupt service routine is called, when interrupt is triggered
 */
static void gpio23_timer_function(struct timer_list *timer) {
    gpio23_state = !gpio23_state;
    gpio_set_value(GPIO_23, gpio23_state); // Toggle GPIO 23
    mod_timer(&gpio23_timer, jiffies + msecs_to_jiffies(LED_DELAY_MS)); // Re-arm timer
}

static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
 printk("gpio_irq: Interrupt was triggered and ISR was called!\n");
 return IRQ_HANDLED;
}


/**
 * @brief This function is called, when the module is loaded into the kernel
  static int __init ModuleInit(void) {
 printk("qpio_irq: Loading module... ");

 /* Setup the gpio */
 if(gpio_request(GPIO_17, "rpi-gpio-17")) { 
 printk("Error!\nCan not allocate GPIO 17\n");
 return -1;
 }

 /* Set GPIO 17 direction */
 if(gpio_direction_input(17)) {
 printk("Error!\nCan not set GPIO 17 to input!\n");
 gpio_free(GPIO_17);
 return -1;
 }

 /* Setup the interrupt */
 irq_number = gpio_to_irq(GPIO_17);

 if(request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0){
 printk("Error!\nCan not request interrupt nr.: %d\n", irq_number);
 gpio_free(GPIO_17);

 return -1;
 }

 printk("Done!\n");
 printk("GPIO 17 is mapped to IRQ Nr.: %d\n", irq_number);

 return 0;
}


/**

 * @brief This function is called, when the module is removed from the kernel

 */

static void __exit ModuleExit(void) {
 printk("gpio_irq: Unloading module... ");
 free_irq(irq_number, NULL);
 gpio_free(17);
}

module_init(ModuleInit);
module_exit(ModuleExit);