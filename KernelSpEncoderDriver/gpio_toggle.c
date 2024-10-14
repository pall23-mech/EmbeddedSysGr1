#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/workqueue.h>
#include <linux/hrtimer.h>

#define GPIO_PIN_1 17  // GPIO 17 for 1 Hz
#define GPIO_PIN_2 18  // GPIO 18 for 10 Hz
#define GPIO_PIN_3 27  // GPIO 27 for 100 Hz

// Workqueue structure for each pin toggling
static struct workqueue_struct *gpio_wq1;
static struct workqueue_struct *gpio_wq2;
static struct workqueue_struct *gpio_wq3;

// Work structures
static struct delayed_work work1;
static struct delayed_work work2;
static struct delayed_work work3;

// Delay times (in milliseconds)
#define DELAY_1HZ 500  // 500 ms for 1 Hz
#define DELAY_10HZ 50  // 50 ms for 10 Hz
#define DELAY_100HZ 5  // 5 ms for 100 Hz

// Toggle GPIO function for 1 Hz
static void toggle_gpio_1hz(struct work_struct *work) {
    static bool state = 0;
    state = !state;
    gpio_set_value(GPIO_PIN_1, state);
    queue_delayed_work(gpio_wq1, &work1, msecs_to_jiffies(DELAY_1HZ));
}

// Toggle GPIO function for 10 Hz
static void toggle_gpio_10hz(struct work_struct *work) {
    static bool state = 0;
    state = !state;
    gpio_set_value(GPIO_PIN_2, state);
    queue_delayed_work(gpio_wq2, &work2, msecs_to_jiffies(DELAY_10HZ));
}

// Toggle GPIO function for 100 Hz
static void toggle_gpio_100hz(struct work_struct *work) {
    static bool state = 0;
    state = !state;
    gpio_set_value(GPIO_PIN_3, state);
    queue_delayed_work(gpio_wq3, &work3, msecs_to_jiffies(DELAY_100HZ));
}

static int __init gpio_module_init(void) {
    int ret;

    // Request GPIOs
    ret = gpio_request(GPIO_PIN_1, "GPIO_PIN_1");
    if (ret) {
        printk(KERN_ERR "Failed to request GPIO %d\n", GPIO_PIN_1);
        return ret;
    }
    gpio_direction_output(GPIO_PIN_1, 0);

    ret = gpio_request(GPIO_PIN_2, "GPIO_PIN_2");
    if (ret) {
        printk(KERN_ERR "Failed to request GPIO %d\n", GPIO_PIN_2);
        gpio_free(GPIO_PIN_1);
        return ret;
    }
    gpio_direction_output(GPIO_PIN_2, 0);

    ret = gpio_request(GPIO_PIN_3, "GPIO_PIN_3");
    if (ret) {
        printk(KERN_ERR "Failed to request GPIO %d\n", GPIO_PIN_3);
        gpio_free(GPIO_PIN_1);
        gpio_free(GPIO_PIN_2);
        return ret;
    }
    gpio_direction_output(GPIO_PIN_3, 0);

    // Initialize workqueues
    gpio_wq1 = create_workqueue("gpio_wq1");
    gpio_wq2 = create_workqueue("gpio_wq2");
    gpio_wq3 = create_workqueue("gpio_wq3");

    if (!gpio_wq1 || !gpio_wq2 || !gpio_wq3) {
        printk(KERN_ERR "Failed to create workqueues\n");
        return -ENOMEM;
    }

    // Initialize delayed work and start the toggle loop
    INIT_DELAYED_WORK(&work1, toggle_gpio_1hz);
    INIT_DELAYED_WORK(&work2, toggle_gpio_10hz);
    INIT_DELAYED_WORK(&work3, toggle_gpio_100hz);

    // Queue the first instance of the work
    queue_delayed_work(gpio_wq1, &work1, msecs_to_jiffies(DELAY_1HZ));
    queue_delayed_work(gpio_wq2, &work2, msecs_to_jiffies(DELAY_10HZ));
    queue_delayed_work(gpio_wq3, &work3, msecs_to_jiffies(DELAY_100HZ));

    printk(KERN_INFO "GPIO Module loaded: 1Hz, 10Hz, 100Hz toggling started\n");
    return 0;
}

static void __exit gpio_module_exit(void) {
    // Cancel delayed work
    cancel_delayed_work_sync(&work1);
    cancel_delayed_work_sync(&work2);
    cancel_delayed_work_sync(&work3);

    // Destroy workqueues
    destroy_workqueue(gpio_wq1);
    destroy_workqueue(gpio_wq2);
    destroy_workqueue(gpio_wq3);

    // Free GPIOs
    gpio_set_value(GPIO_PIN_1, 0);
    gpio_set_value(GPIO_PIN_2, 0);
    gpio_set_value(GPIO_PIN_3, 0);

    gpio_free(GPIO_PIN_1);
    gpio_free(GPIO_PIN_2);
    gpio_free(GPIO_PIN_3);

    printk(KERN_INFO "GPIO Module unloaded\n");
}

module_init(gpio_module_init);
module_exit(gpio_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A kernel module to toggle GPIO pins at 1Hz, 10Hz, and 100Hz.");
