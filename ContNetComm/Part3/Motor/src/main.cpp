#include <initialization_state.h>
#include <context.h>

int command = 0; // for incoming serial data
Context *context;

const int FLT_PIN = 2; // Pin for fault signal (modify according to your configuration)

uint16_t ModRTU_CRC(uint8_t buf[], int len) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc
        for (int i = 8; i != 0; i--) { // Loop over each bit
            if ((crc & 0x0001) != 0) { // If the LSB is set
                crc >>= 1; // Shift right and XOR 0xA001
                crc ^= 0xA001;
            } else {
                crc >>= 1; // Just shift right
            }
        }
    }
    return crc;
}

void setup() {
    Serial.begin(115200); // Initialize serial port
    context = new Context(new InitializationState()); // Start in Initialization state
    pinMode(LED_BUILTIN, OUTPUT); // Set LED pin as output
    pinMode(FLT_PIN, INPUT); // Set FLT pin as input
}

void loop() {
    context->do_work(); // Execute current state's behavior

    if (digitalRead(FLT_PIN) == HIGH) {
        context->fault(); // Transition to Stopped state if FLT pin is high
    }

    // Process data only when enough bytes are available in serial buffer
    if (Serial.available() >= 8) { // Ensure that the full message is available
        const size_t MSG_LEN = 8;
        uint8_t msg[MSG_LEN];
        Serial.readBytes(msg, MSG_LEN);

        // Check if this message is intended for this node (verify device ID in msg[0])
        if (msg[0] == 0x01) { // Check if the command is for this Arduino
            uint16_t reg = (msg[2] << 8) | msg[3]; // Extract register
            uint16_t ref = (msg[4] << 8) | msg[5]; // Extract reference value

            // Handle CANopen state control commands for this Arduino
            if (reg == 0x01) { // State control command
                switch (ref) {
                    case 0x01:  // Set node operational
                        context->op_transition();
                        break;
                    case 0x02:  // Stop node
                        context->fault();
                        break;
                    case 0x80:  // Set node pre-operational
                        context->pre_transition();
                        break;
                    case 0x81:  // Reset node
                    case 0x82:  // Reset communications
                        context->reset();
                        break;
                    default:
                        break;
                }
            }

            // Compute CRC for verification
            uint16_t myCRC = ModRTU_CRC(msg, 6);
            uint8_t myCRC1 = (uint8_t)(myCRC >> 8);
            uint8_t myCRC2 = (uint8_t)(myCRC & 0x00FF);

            // Verify CRC and send acknowledgment if correct
            if (msg[6] == myCRC1 && msg[7] == myCRC2) {
                Serial.write(msg, MSG_LEN);  // Echo back to confirm reception
            }
        }
    }
}
