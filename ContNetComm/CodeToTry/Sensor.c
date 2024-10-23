// Define the analog pin
const int analogPin = A0;

// Baud rate for serial communication
const long baudRate = 9600;

// Interval for reading the analog value (1000 ms = 1 second)
const unsigned long interval = 1000;
unsigned long previousMillis = 0;

void setup() {
  // Start serial communication
  Serial.begin(baudRate);
}

void loop() {
  // Check if it's time to read and send the value
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    // Save the current time
    previousMillis = currentMillis;
    
    // Read the analog value
    int analogValue = analogRead(analogPin);
    
    // Print the value to the serial monitor
    Serial.print("Analog Value: ");
    Serial.println(analogValue);
    
    // Send the value over UART (if needed for other UART communication, add additional code)
    Serial.write(analogValue); // Sends the value as a byte (0-255)
    
    // If you want to send as a string, uncomment the next line
    // Serial.println(analogValue);
  }
}
