#include <Arduino.h>
#include "encoder.h"

// Initialize the encoder on A2 (PCINT10, pin 16) and A3 (PCINT11, pin 17) and 1400 pulses per rev, from the specifications
Encoder encoder(16, 17, 1400.0);
int null_postion=0;  //originial encoder value
unsigned long startTime=0;
unsigned long endTime=0;
unsigned long elapsedTime=0;
float maxPPS = 3490;   //measured in project 2, part 1,i)
float omega1 =0.63*maxPPS;

void setup()
{
    Serial.begin(9600); // Start serial communication at 9600 baud
    encoder.init();     // Initialize the encoder and interrupts
}

void looping()
{
    static unsigned long lastPrintTime = 0;
    unsigned long currentTime = millis();
    
    if (encoder.position()!=null_postion&&null_postion==0)
    {
        startTime = currentTime;
        null_postion=1;
    }

    if (encoder.speedPPS()>= omega1&&null_postion < 2)
    {
        endTime = currentTime;
        elapsedTime = endTime-startTime;
        Serial.print("Maximum reached [ms]: ");
        Serial.println(elapsedTime);
        null_postion =2;
    }
    // Print the position at a regular interval (every second)
    if (currentTime - lastPrintTime >= 1000)
    {
        encoder.updateSpeed(); // Update the speed (PPS and RPM) based on the current position and time

        Serial.print("Position: ");
        Serial.println(encoder.position());

        Serial.print("Speed (PPS): ");
        Serial.println(encoder.speedPPS());

        Serial.print("Speed (RPM): ");
        Serial.println(encoder.speedRPM());

        lastPrintTime = currentTime;
    }
}

int main()
{
    
    //take position comparison,
    // Initialize Arduino core functions
    init(); // IMPORTANT: Initialize Arduino core libraries (including Serial)

    // Call the setup() function
    setup();

    // Infinite loop that continuously calls loop() as Arduino normally would
    while (1)
    {
        looping(); // Continuously check for updates and print position

    }

    return 0;
}
