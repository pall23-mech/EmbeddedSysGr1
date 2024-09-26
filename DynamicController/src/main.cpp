#include <initialization_state.h>
#include <context.h>

int command = 0; // for incoming serial data

Context *context;

void setup()
{
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  // can put serial.println for commands available here...
  context = new Context(new InitializationState()); // Start in Initialization state
}

void loop()
{
  // The main code:

  context->do_work(); // Execute current state's behavior

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
    
    case 't':
      context->transition(); // transition, in initialization to operational
      break;

    default:
      Serial.println("Unknown command");
      break;
    }
  }
}