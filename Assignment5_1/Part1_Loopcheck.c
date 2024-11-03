#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#define SERIAL_PORT "/dev/serial0"  // Serial port device file
#define BAUDRATE B9600              // Baudrate for the serial communication
#define TEST_STRING "Hello, Torfi!"

int main() {
    int serial_fd;
    struct termios options;
    char buffer[256];
    int total_count = 0;
    int count;
    struct timespec start_time, end_time;
    double elapsed_time;

    // Open the serial port in blocking mode
    serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    if (serial_fd == -1) {
        perror("Unable to open serial port");
        return 1;
    }

    // Configure the serial port
    tcgetattr(serial_fd, &options);
    cfsetispeed(&options, BAUDRATE);
    cfsetospeed(&options, BAUDRATE);
    options.c_cflag |= (CLOCAL | CREAD);    // Enable receiver, set local mode
    options.c_cflag &= ~PARENB;             // No parity
    options.c_cflag &= ~CSTOPB;             // 1 stop bit
    options.c_cflag &= ~CSIZE;              // Clear size bits
    options.c_cflag |= CS8;                 // 8 data bits
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input mode
    options.c_cc[VMIN] = 0;                 // Minimum number of characters to read
    options.c_cc[VTIME] = 10;               // Timeout in deciseconds (1 second)

    tcsetattr(serial_fd, TCSANOW, &options);

    // Get the start time
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Write the test string to the serial port
    count = write(serial_fd, TEST_STRING, strlen(TEST_STRING));
    if (count < 0) {
        perror("Error writing to serial port");
        close(serial_fd);
        return 1;
    }
    printf("Sent: %s\n", TEST_STRING);

    // Read the response back into buffer in a loop
    while (total_count < strlen(TEST_STRING)) {
        count = read(serial_fd, buffer + total_count, strlen(TEST_STRING) - total_count);
        if (count < 0) {
            perror("Error reading from serial port");
            close(serial_fd);
            return 1;
        } else if (count == 0) {
            // Timeout or no more data available
            break;
        }
        total_count += count;
    }
    buffer[total_count] = '\0'; // Null-terminate the received string

    // Get the end time
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Calculate the elapsed time
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + 
                   (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    // Output the results
    printf("Received: %s\n", buffer);
    printf("Characters sent: %d\n", (int)strlen(TEST_STRING));
    printf("Characters received: %d\n", total_count);
    printf("Time elapsed: %.6f seconds\n", elapsed_time);

    // Close the serial port
    close(serial_fd);

    return 0;
}
