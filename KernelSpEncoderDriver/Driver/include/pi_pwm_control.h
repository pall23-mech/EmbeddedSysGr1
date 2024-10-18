#ifndef PI_PWM_CONTROL_H
#define PI_PWM_CONTROL_H

// Pin definitions (GPIO12 and GPIO13 for PWM)
#define PWM_PIN1 12
#define PWM_PIN2 13

// Function prototypes
void pwm_init(void);
void pwm_set(int pin, int value);
void pwm_cleanup(void);

#endif // PWM_CONTROL_H
