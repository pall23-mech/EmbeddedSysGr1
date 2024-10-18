#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>   // For jiffies and time keeping

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Encoder counter with PPS and RPM calculation using GPIO interrupts");

#define GPIO_A 532  // Encoder Channel A (e.g., GPIO 20)
#define GPIO_B 533  // Encoder Channel B (e.g., GPIO 21)
#define PULSES_PER_REV 1400       // Number of pulses per revolution (example value, adjust as needed)

static int encoder_position = 0;       // Encoder pulse counter
static unsigned long last_jiffies = 0; // Last jiffies timestamp when the position was updated
static int last_position = 0;          // Last encoder position
static int irq_number_a, irq_number_b; // IRQ numbers for channels A and B
static bool last_state_a = false;      // Last state of Channel A
static bool last_state_b = false;      // Last state of Channel B

static int pps = 0;  // Pulses per second (fixed-point)
static int rpm = 0;  // Revolutions per minute (fixed-point)

// Function declarations
void updateSpeed(void);
int getSpeedPPS(void);
int getSpeedRPM(void);

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
        printk(KERN_INFO "Encoder position: %d, PPS: %d, RPM: %d\n", encoder_position, pps, rpm);
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
        printk(KERN_INFO "Encoder position: %d, PPS: %d, RPM: %d\n", encoder_position, pps, rpm);
    }

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
        pps = (delta_position * 1000) / delta_time_ms; // Calculate PPS correctly in integer arithmetic
        rpm = (pps * 60) / PULSES_PER_REV;             // Calculate RPM using the corrected PPS value

        // Update the last position and time
        last_position = encoder_position;
        last_jiffies = current_jiffies;
    }
}

/**
 * @brief Get the current speed in PPS (integer).
 */
int getSpeedPPS(void)
{
    return pps;
}

/**
 * @brief Get the current speed in RPM (integer).
 */
int getSpeedRPM(void)
{
    return rpm;
}

/**
 * @brief Initialize the module, setup the GPIOs, and interrupts for both channels.
 */
static int __init encoder_init(void)
{
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

    printk(KERN_INFO "Encoder counter: Module loaded\n");
    return 0;
}

/**
 * @brief Clean up and free resources when the module is unloaded.
 */
static void __exit encoder_exit(void)
{
    free_irq(irq_number_a, NULL);
    free_irq(irq_number_b, NULL);
    gpio_free(GPIO_A);
    gpio_free(GPIO_B);

    printk(KERN_INFO "Encoder counter: Module unloaded\n");
}

module_init(encoder_init);
module_exit(encoder_exit);
