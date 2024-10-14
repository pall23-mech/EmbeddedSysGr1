#include <initialization_state.h>
#include <context.h>


int command = 0; // for incoming serial data

Context *context;

const int FLT_PIN = 2; // Pin for fault signal (modify according to your configuration)

void setup()
{
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps (115200 does not work for Margrét, the output is strange then)
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
  if (Serial.available() > 0)
  {
    // read the incoming byte:
    command = Serial.read();

    // compare the value received to a character constant:
    switch (command)
    {
    case 'r':
      context->reset(); // reset, in operational to initialization
      break;
    
    case 'p':
      context->pre_transition(); // transition, in initialization to operational
      break;

    case 'o':
      context->op_transition();
      break;

    case 'f':
      context->fault(); // fault detection (we will comment this line out, should be detected on the device...)
      break;

    case 'c':
      context->configure();
      break;

    default:
      Serial.println("Unknown command");
      break;
    }
  }
}