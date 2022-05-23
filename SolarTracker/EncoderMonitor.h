// integer position that can change at any time.
volatile int encoderPosition;

// ISR for A
void EncoderMonitorA()
{
  if (digitalRead(2) == digitalRead(3)) //input A and input B are equal // input A and input B are pins 2&3 respectively.
    {
      encoderPosition++;
    }
  else
  {
    encoderPosition--;
  }
}

// ISR for B
void EncoderMonitorB()
{
  if (digitalRead(2) == digitalRead(3)) //input A and input B are equal // input A and input B are pins 2&3 respectively.
    {
      encoderPosition--;
    }
  else
  {
    encoderPosition++;
  }
}
 
// Function to start encoder monitoring
void EncoderInitialize()
{
  // Set pins 2 and 3 to input
  pinMode(2, INPUT);
  pinMode(3, INPUT);

  // Attach the ISR to the pins, choose the ISR function, and set the waveform condition
  attachInterrupt(digitalPinToInterrupt(2), EncoderMonitorA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), EncoderMonitorB, CHANGE);
}
