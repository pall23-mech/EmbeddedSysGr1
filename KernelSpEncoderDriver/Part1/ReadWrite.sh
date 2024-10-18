#!/bin/bash
toggle=0  # Initialize toggle variable

while true; do
    # Toggle GPIO 23 every 0.1 seconds
    if [ "$toggle" -eq 0 ]; then
        gpioset gpiochip0 23=1   # Set GPIO 23 high
        toggle=1                 # Update toggle state
    else
        gpioset gpiochip0 23=0   # Set GPIO 23 low
        toggle=0                 # Update toggle state
    fi

    # Read the state of GPIO 18
    input=$(gpioget gpiochip0 18)

    # If GPIO 18 is different from GPIO 23, set GPIO 18 to match GPIO 23
    if [ "$input" -ne "$toggle" ]; then
        gpioset gpiochip0 18=$toggle   # Set GPIO 18 to the same state as GPIO 23
    fi
    
    sleep 0.1   # Delay for 0.1 seconds before toggling again
done
