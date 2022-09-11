
unsigned long power(int a, int b)
{
  unsigned long res;
  if (b == 0)
  {
    res = 1;
  }
  else
  {
    res = a;
    for (int i = 0; i < (b - 1); i++)
    {
      res *= a;
    }
  }
  return res;
}

// Function to debounce the button
// 0 = pressed, 1 = depressed, 2 = long press
void checkButton()
{
  if ((millis() - lastButtonPress) > 100)
  {
    if (digitalRead(buttonPin) != lastButtonState)
    {
      button = digitalRead(buttonPin);
      lastButtonState = button;
      lastButtonPress = millis();
    }
  }
}

void encChange()
{
  // Depending in which menu state you are
  // the encoder will either change the value of a setting:
  //-+ frequency, change between FREQ0 and FREQ1 register (or -+ phase), On/Off, mode
  // or it will change the cursor position
  unsigned char state = encoder.process();
  // Direction clockwise
  if (state == DIR_CW)
  {
    switch (menuState)
    {
    case 1:
    {
      if (cursorPos == 3)
        cursorPos = 0;
      else
        cursorPos++;
    }
    break;

    case 2:
    {
      // Here we initialise two variables.
      // newFrequency is the value of the frequency after we increment it
      // dispDigit is the digit that we are currently modifing, and we obtain it
      // by a neat little trick, using operator % in conjunction with division
      // We then compare these variables with the maximum value for our
      // frequency, if all is good, make the change
      unsigned long newFrequency = frequency + power(10, digitPos);
      unsigned char dispDigit = frequency % power(10, digitPos + 1) / power(10, digitPos);

      if (newFrequency <= maxFrequency && dispDigit < 9)
      {
        frequency += power(10, digitPos);
        updateDisplay = true;
      }

      if (freqRegister == 0)
      {
        frequency0 = frequency;
      }
      else if (freqRegister == 1)
      {
        frequency1 = frequency;
      }
    }
    break;

    case 4:
    {
      // if usePhase has been defined, changes in the encoder will vary the phase
      // value (upto 4096)
      // A better implementation would be to use increment of pi/4 or submultiples of
      // pi where 2pi = 4096
#ifdef usePhase
      unsigned long newPhase = phase + power(10, digitPos);
      unsigned char dispDigit =
          phase % power(10, digitPos + 1) / power(10, digitPos);
      if (newPhase < maxPhase && dispDigit < 9)
      {
        phase += power(10, digitPos);
        updateDisplay = true;
      }
#endif
    }
    break;

    case 5:
    {
      if (currentMode == 2)
        currentMode = 0;
      else
        currentMode++;
      updateDisplay = true;
    }
    break;
    }
  }
  // Direction counter clockwise
  else if (state == DIR_CCW)
  {
    switch (menuState)
    {
    case 1:
    {
      if (cursorPos == 0)
        cursorPos = 3;
      else
        cursorPos--;
    }
    break;

    case 2:
    {
      unsigned long newFrequency = frequency + power(10, digitPos);
      unsigned char dispDigit =
          frequency % power(10, digitPos + 1) / power(10, digitPos);
      if (newFrequency > 0 && dispDigit > 0)
      {
        frequency -= power(10, digitPos);
        updateDisplay = true;
      }

      if (freqRegister == 0)
      {
        frequency0 = frequency;
      }
      else if (freqRegister == 1)
      {
        frequency1 = frequency;
      }
    }
    break;

    case 4:
    {
      // if usePhase has been defined, changes in the encoder will vary the phase
      // value (upto 4096)
      // A better implementation would be to use increment of pi/4 or submultiples of
      // pi where 2pi = 4096
#ifdef usePhase
      unsigned long newPhase = phase + power(10, digitPos);
      unsigned char dispDigit =
          phase % power(10, digitPos + 1) / power(10, digitPos);
      if (newPhase > 0 && dispDigit > 0)
      {
        phase -= power(10, digitPos);
        updateDisplay = true;
      }
#endif
    }
    break;

    case 5:
    {
      if (currentMode == 0)
        currentMode = 2;
      else
        currentMode--;
      updateDisplay = true;
    }
    break;
    }
  }
}

// Function to display the current frequency in the top left corner
void displayFrequency()
{
  unsigned long frequencyToDisplay = frequency;
  lcd.setCursor(0, 0);
  lcd.print("f=");
  for (int i = 7; i >= 0; i--)
  {
    unsigned char dispDigit = frequencyToDisplay / power(10, i);
    lcd.print(dispDigit);
    frequencyToDisplay -= dispDigit * power(10, i);
  }
  lcd.print("Hz");
}

// Function to display power state (ON/OFF) in the top right corner
void displayPower()
{
  lcd.setCursor(13, 0);
  lcd.print(powerState[currentPowerState]);
}

// Function to display the mode in the bottom right corner
void displayMode()
{
  lcd.setCursor(13, 1);
  lcd.print(mode[currentMode]);
}

// Function to display the mode in the bottom left corner
// Only used if you enable PHASE setting instead of FREQ register
void displayPhase()
{
  unsigned int phaseToDisplay = phase;
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.print("=");
  for (int i = 3; i >= 0; i--)
  {
    unsigned int dispDigit = phaseToDisplay / power(10, i);
    lcd.print(dispDigit);
    phaseToDisplay -= dispDigit * power(10, i);
  }
}

// Function to display the FREQ register (either 0 or 1) in the bottom left
// corner
void displayFreqRegister()
{
  lcd.setCursor(0, 1);
  lcd.print("FREQ");
  lcd.print(freqRegister);
}
