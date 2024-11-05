#include <Arduino.h>
#include <unity.h>
#include "pi_controller.h"
#include "p_controller.h"

// Test 1: Normal Operation Without Saturation
void test_normal_operation() {
    PI_Controller piController(2.0, 0.5, 0.01);  // Example values for Kp, Ti, and T_step
    double controlSignal = piController.update(1000.0, 900.0);  // Target: 1000 PPS, Actual: 900 PPS

    // Check if the control signal is between 0% and 100%
    TEST_ASSERT_TRUE(controlSignal >= 0.0 && controlSignal <= 100.0);
}

// Test 2: Saturation and Integral Windup Prevention
void test_integral_windup_prevention() {
    PI_Controller piController(2.0, 0.5, 0.01);  // Example values for Kp, Ti, T_step

    // Force saturation: Large error should cause control signal to hit 100% PWM
    double controlSignal = piController.update(3000.0, 500.0);  // High error
    TEST_ASSERT_EQUAL(100.0, controlSignal);  // Expect 100% PWM

    // Check if the integral term is not accumulating when saturated
    double newControlSignal = piController.update(3000.0, 500.0);  // Saturation should hold
    TEST_ASSERT_EQUAL(100.0, newControlSignal);  // Still saturated at 100%
}

// Test 3: Resume Integral After Leaving Saturation
void test_integral_resumes_after_saturation() {
    PI_Controller piController(2.0, 0.5, 0.01);  // Example values for Kp, Ti, T_step

    // Simulate period of saturation
    piController.update(3000.0, 500.0);  // Saturated
    piController.update(3000.0, 500.0);  // Still saturated

    // Lower the target, control signal should leave saturation and be below 100%
    double controlSignal = piController.update(1500.0, 1400.0);  // Lower error
    TEST_ASSERT_TRUE(controlSignal < 100.0);  // No longer saturated
}

void setup() {
    // Initialize Arduino Serial and Unity test framework
    Serial.begin(115200);
    UNITY_BEGIN();

    // Run each test function
    RUN_TEST(test_normal_operation);
    RUN_TEST(test_integral_windup_prevention);
    RUN_TEST(test_integral_resumes_after_saturation);

    UNITY_END();  // End the test
}

void loop() {
    // Empty loop as tests are in setup
}
