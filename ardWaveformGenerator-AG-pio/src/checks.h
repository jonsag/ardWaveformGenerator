
void checkFreq()
{
  freqCurrent = analogRead(A5);
  if (abs(freqCurrent - frequency) > freqTolerance)
  {                                  // if reading from pot exceeds tolerance
    frequency = freqCurrent;         // new frequency- number between 0 and 1024
    freqscaled = 48 * frequency + 1; // from 1 to ~50,000
    period = samplerate / freqscaled;
    pulseWidthScaled = int(pulseWidth / 1023 * period);
    triInc = 511 / period;
    if (triInc == 0)
    {
      triInc = 1;
    }
    sawInc = 255 / period;
    if (sawInc == 0)
    {
      sawInc = 1;
    }
    sinInc = 20000 / period;
  }
}

void checkPW()
{
  PWCurrent = analogRead(A4);
  if (abs(PWCurrent - pulseWidth) > PWTolerance)
  {                         // if reading from pot exceeds tolerance
    pulseWidth = PWCurrent; // new pulse width, val between 0 and 1023
    pulseWidthScaled = int(pulseWidth / 1023 * period);
  }
}

void checkShape()
{ // debounce and check states of buttons
  //  4 momentary switches control waveshape
  //   -pulse connects to A0
  //   -triangle A1
  //   -saw A2
  //   -sine A3
  typelast = typecurrent;
  if (digitalRead(A0) == LOW)
  {
    typecurrent = 1;
  }
  else if (digitalRead(A1) == LOW)
  {
    typecurrent = 2;
  }
  else if (digitalRead(A2) == LOW)
  {
    typecurrent = 4;
  }
  else if (digitalRead(A3) == LOW)
  {
    typecurrent = 8;
  }

  for (i = 0; i < 4; i++)
  {
    if (i == type)
    {
    }
    else
    {
      if ((typecurrent & (1 << i)) ^ (typelast & (1 << i)))
      { // current diff than prev and debounce
        if ((typecurrent & (1 << i)))
        {           // currently depressed
          type = i; // set wave type
        }
        else
        {
          typecounter[i] = 12; // else set debounce counter to 12
        }
      }
      else if (((typecurrent & (1 << i)) == (typelast & (1 << i))))
      { // if current same as prev and diff than debounce
        if (typecounter[i] > 0 && --typecounter[i] == 0)
        { // decrease debounce counter and check to see if = 0
          if ((typecurrent & (1 << i)))
          { // if debounce counter = 0 toggle debounced state
            type = i;
          }
        }
      }
    }
  }
}
