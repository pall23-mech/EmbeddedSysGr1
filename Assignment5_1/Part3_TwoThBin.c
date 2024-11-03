#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>

#define SERIAL_PORT "/dev/serial0"  // Serial port device file
#define BAUDRATE B57600             // Baudrate for the serial communication
#define MESSAGE_LENGTH sizeof(uint32_t) // Fixed message length (4 bytes)

// Global variable for the serial file descriptor
int serial_fd;

// Function to configure the serial port
void configure_serial_port(int fd) {
    struct termios options;
    tcgetattr(fd, &options);

    cfsetispeed(&options, BAUDRATE);
    cfsetospeed(&options, BAUDRATE);

    options.c_cflag |= (CLOCAL | CREAD);    // Enable receiver, set local mode
    options.c_cflag &= ~PARENB;             // No parity
    options.c_cflag &= ~CSTOPB;             // 1 stop bit
    options.c_cflag &= ~CSIZE;              // Clear size bits
    options.c_cflag |= CS8;                 // 8 data bits
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input mode (non-canonical)
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off software flow control
    options.c_oflag &= ~OPOST;              // Turn off output processing
    options.c_cc[VMIN] = 5;//MESSAGE_LENGTH;    // Minimum number of bytes to read (fixed message length)
    options.c_cc[VTIME] = 0;                // No timeout (blocking read)

    tcsetattr(fd, TCSANOW, &options);
}

// Helper function to print a uint32_t in binary format
void print_binary(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1); // Extract the i-th bit
        if (i % 8 == 0 && i != 0) {     // Add space every 8 bits for readability
            printf(" ");
        }
    }
    printf("\n");
}

// Writer thread function
void* writer_thread(void* arg) {
    uint32_t counter = 0;
    while (1) {
        uint32_t counter_to_send = htole32(counter); // Convert to little-endian explicitly

        // Write the counter value in binary format to the serial port
        int count = write(serial_fd, &counter_to_send, sizeof(counter_to_send));
        if (count < 0) {
            perror("Error writing to serial port");
        } else {
            printf("Sent: %u\n", counter);
        }

        // Increment the counter and sleep for 1 second
        counter++;
        sleep(1);
    }
    return NULL;
}

// Reader thread function (updated)
void* reader_thread(void* arg) {
    uint32_t received_counter;
    uint8_t buffer[MESSAGE_LENGTH];  // Buffer to hold the received bytes

    while (1) {
        // Read the counter value from the serial port
        int count = read(serial_fd, buffer, MESSAGE_LENGTH);
        if (count < 0) {
            perror("Error reading from serial port");
        } else if (count == MESSAGE_LENGTH) {
            // Reconstruct the uint32_t from the buffer (assuming little-endian format)
            received_counter = buffer[0] |
                               (buffer[1] << 8) |
                               (buffer[2] << 16) |
                               (buffer[3] << 24);

            printf("Received: ");
            print_binary(received_counter); // Print the binary representation
        }
    }
    return NULL;
}

int main() {
    // Open the serial port
    serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    if (serial_fd == -1) {
        perror("Unable to open serial port");
        return 1;
    }

    // Configure the serial port
    configure_serial_port(serial_fd);

    // Create threads for reading and writing
    pthread_t writer, reader;
    if (pthread_create(&writer, NULL, writer_thread, NULL) != 0) {
        perror("Failed to create writer thread");
        close(serial_fd);
        return 1;
    }

    if (pthread_create(&reader, NULL, reader_thread, NULL) != 0) {
        perror("Failed to create reader thread");
        close(serial_fd);
        return 1;
    }

    // Wait for the threads to finish (they won't, as they loop infinitely)
    pthread_join(writer, NULL);
    pthread_join(reader, NULL);

    // Close the serial port (will not reach here unless the program is terminated)
    close(serial_fd);
    return 0;
}
