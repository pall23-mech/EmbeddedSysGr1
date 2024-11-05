#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/select.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    int file, count;

    if (argc != 2) {
        printf("Invalid number of arguments, exiting!\n");
        return -2;
    }

    // Open serial port
    if ((file = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY)) < 0) {
        perror("UART: Failed to open the file.\n");
        return -1;
    }

    struct termios options;
    tcgetattr(file, &options);
    options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
    options.c_iflag = IGNPAR | ICRNL;
    tcflush(file, TCIFLUSH);
    tcsetattr(file, TCSANOW, &options);

    // Send the command (with null character at the end)
    if ((count = write(file, argv[1], strlen(argv[1]) + 1)) < 0) {
        perror("Failed to write to the output\n");
        close(file);
        return -1;
    }

    // Set up to check for a single response
    unsigned char receive[100];
    fd_set read_fds;
    struct timeval timeout;

    // Set up the file descriptor set.
    FD_ZERO(&read_fds);
    FD_SET(file, &read_fds);

    // Set up the timeout: 100ms.
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;

    // Wait for data to be available for reading.
    int rv = select(file + 1, &read_fds, NULL, NULL, &timeout);

    if (rv == -1) {
        perror("select"); // Error occurred in select()
        close(file);
        return -1;
    } else if (rv == 0) {
        printf("No data available to read.\n"); // Timeout occurred, no data
    } else {
        // Data is available; read it.
        count = read(file, (void *)receive, sizeof(receive) - 1);
        if (count < 0) {
            perror("Failed to read from the input\n");
            close(file);
            return -1;
        }
        if (count > 0) {
            receive[count] = 0;  // Null-terminate the received data
            
            // Check the source of the message
            if (strncmp((char *)receive, "SE", 2) == 0) {
                printf("The following was read from the sensor [%d]: %s\n", count, receive);
            } else if (strncmp((char *)receive, "MO", 2) == 0) {
                printf("The following was read from the motor [%d]: %s\n", count, receive);
            } else {
                printf("Source of message unknown: [%d]: %s\n", count, receive);
            }
        }
    }

    close(file); // Close the serial port
    return 0;
}
