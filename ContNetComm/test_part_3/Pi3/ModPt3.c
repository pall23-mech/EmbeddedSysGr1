#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// Compute the MODBUS RTU CRC
uint16_t ModRTU_CRC(uint8_t buf[], int len);
void make_msg(uint8_t ID, uint8_t func, uint16_t reg, uint16_t data, uint8_t* msg);

#define MSG_LEN 8
uint8_t msg[MSG_LEN];

int main(int argc, char *argv[]) {
    int file, count;
    uint16_t sens_val;
    uint16_t rpm;

    if (argc != 5) {
        printf("Invalid number of arguments, exiting!\n");
        return -2;
    }

    if ((file = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY)) < 0) {
        perror("UART: Failed to open the file.\n");
        return -1;
    }

    struct termios options;
    tcgetattr(file, &options);
    options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
    options.c_iflag = IGNPAR | ICRNL;
    tcflush(file, TCIFLUSH);
    cfmakeraw(&options);
    tcsetattr(file, TCSANOW, &options);

    // --- Send the first message (1 6 1 1) ---
    make_msg(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), msg);

    // Clear buffer before sending
    tcflush(file, TCIFLUSH);
    if ((count = write(file, msg, MSG_LEN)) < 0) {
        perror("Failed to write to the output\n");
        return -1;
    }

    printf("Sent request (1 6 1 1):\t");
    for (int i = 0; i < MSG_LEN; i++) {
        printf("%02X ", msg[i]);
    }
    printf("\n");

    usleep(300000); // Wait 0.3 seconds for a response

    // --- Read the response to the first message ---
    unsigned char receive[8];  // Exact buffer size for expected response
    if ((count = read(file, (void*)receive, 8)) < 0) {
        perror("Failed to read from the input\n");
        return -1;
    }

    if (count == 8) {  // Check if exactly 8 bytes were received
        printf("Received response to (1 6 1 1):\t");
        for (int i = 0; i < 8; i++) {
            printf("%02X ", receive[i]);
        }
        printf("\n");

        // Check if the response starts with the expected device ID
        if (receive[0] == 0x01) {
            // Process the response if ID is 0x01
            // You may add specific handling here if needed for (1 6 1 1)
        }
    } else {
        printf("Unexpected response length: %d\n", count);
    }

    // --- Send the second message (2 3 0 1) ---
    make_msg(2, 3, 0, 1, msg); // Creating message with ID=2, func=3, reg=0, data=1

    // Clear buffer before sending
    tcflush(file, TCIFLUSH);
    if ((count = write(file, msg, MSG_LEN)) < 0) {
        perror("Failed to write the second message\n");
        return -1;
    }

    printf("Sent request (2 3 0 1):\t");
    for (int i = 0; i < MSG_LEN; i++) {
        printf("%02X ", msg[i]);
    }
    printf("\n");

    usleep(300000); // Wait 0.3 seconds for a response to the second message

    // --- Read the response to the second message ---
    if ((count = read(file, (void*)receive, 8)) < 0) {
        perror("Failed to read from the input\n");
        return -1;
    }

    if (count == 8) {  // Check if exactly 8 bytes were received
        printf("Received response to (2 3 0 1):\t");
        for (int i = 0; i < 8; i++) {
            printf("%02X ", receive[i]);
        }
        printf("\n");

        // Check if response ID is 0x02 and function code is 0x03 (Modbus Read Holding Registers)
        if (receive[0] == 0x02 && receive[1] == 0x03) {
            // Extract the sensor value, assuming the format is high byte in receive[4] and low byte in receive[5]
            sens_val = (receive[4] << 8) | receive[5];
            
            // Calculate RPM or any other metric based on sens_val
            rpm = sens_val * 120 / 1023;
            printf("Calculated RPM from sensor: %d\n", rpm);
        } else {
            printf("Unexpected response format or ID\n");
        }
    } else {
        printf("Unexpected response length for second message: %d\n", count);
    }

    close(file);
    return 0;
}

uint16_t ModRTU_CRC(uint8_t buf[], int len) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc
        for (int i = 8; i != 0; i--) { // Loop over each bit
            if ((crc & 0x0001) != 0) { // If the LSB is set
                crc >>= 1; // Shift right and XOR 0xA001
                crc ^= 0xA001;
            } else { // Else LSB is not set
                crc >>= 1; // Just shift right
            }
        }
    }
    return crc;
}

void make_msg(uint8_t ID, uint8_t func, uint16_t reg, uint16_t data, uint8_t* msg) {
    msg[0] = ID;
    msg[1] = func;
    msg[2] = reg >> 8;
    msg[3] = reg & 0x00FF;
    msg[4] = data >> 8;
    msg[5] = data & 0x00FF;
    uint16_t crc = ModRTU_CRC(msg, MSG_LEN - 2);
    msg[6] = (uint8_t)(crc >> 8);
    msg[7] = (uint8_t)(crc & 0x00FF);
}
