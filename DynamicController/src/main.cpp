#include <initialization_state.h>
#include <context.h>

int command = 0; // for incoming serial data

Context *context;

void setup()
{
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
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
    case 'c':
      context->event1(); // too cool, in Idle
      break;
    
    case 'h':
      context->event2(); // too hot, in Idle
      break;

    case 'f':
      context-> event1(); // failure, in Heating and Cooling
      break;

    case 'o':
      context-> event2(); // OK, in Heating and Cooling
      break;

    case 'n':
      context-> event1(); // failure cleared, in Failure
    
    default:
      Serial.println("Unknown command");
      break;
    }

  }
}