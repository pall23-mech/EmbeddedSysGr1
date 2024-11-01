#include <initialization_state.h>
#include <context.h>


int command = 0; // for incoming serial data

Context *context;

const int FLT_PIN = 2; // Pin for fault signal (modify according to your configuration)

uint16_t ModRTU_CRC(uint8_t buf[], int len)
{
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc
        for (int i = 8; i != 0; i--) { // Loop over each bit
            if ((crc & 0x0001) != 0) { // If the LSB is set
                crc >>= 1; // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else {// Else LSB is not set
                crc >>= 1; // Just shift right
            }
        }
    }
    // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
    return crc;
}

void setup()
{
  Serial.begin(115200); // opens serial port, sets data rate to 9600 bps (115200 does not work for Margrét, the output is strange then)
  // can put serial.println for commands available here...
  context = new Context(new InitializationState()); // Start in Initialization state
  pinMode(LED_BUILTIN, OUTPUT); // Set LED pin as output
  pinMode(FLT_PIN, INPUT); // Set FLT pin as input
 }

void loop()
{
  // The main code:

  context->do_work(); // Execute current state's behavior

  if (digitalRead(FLT_PIN) == HIGH) {
    context->fault(); // If FLT pin is high, a fault is detected, transition to Stopped state
  }

  // send data only when you receive data:
  if (Serial.available() > 8) // Ensure that the full message is available
  {
    const size_t MSG_LEN = 8;
    uint8_t msg[MSG_LEN];
    Serial.readBytes(msg, MSG_LEN);

    // Print out message bytes for debugging
    Serial.print("Received bytes: ");
    for (int i = 0; i < MSG_LEN; i++) {
        Serial.print(msg[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    // Verify the command is intended for this node (chech node ID in msg[0])
    if(msg[0] == 0x01){   // checking if this command is for this arduino?
      uint16_t reg = (msg[2] << 8) | msg[3];  // Extract register
      uint16_t ref = (msg[4] << 8) | msg[5];  // Extract reference value

      Serial.print("Reg: ");
      Serial.println(reg, HEX);
      Serial.print("Ref: ");
      Serial.println(ref, HEX);

      // Handle CANopen state control commands for this Arduino
      if (reg == 0x01) {  // State control command
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
            Serial.println("Unknown state command");
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
      } else {
        Serial.println("CRC mismatch");
      }
    }
  }
}