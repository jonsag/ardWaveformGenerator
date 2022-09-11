#include <Arduino.h>

#include <LiquidCrystal_I2C.h>
#include <AD9833.h>
#include <Rotary.h>

#include <config.h>
#include <modules.h>

void setup()
{
  // Initialise the LCD, start the backlight and print a "bootup" message for two seconds
  lcd.begin();
  lcd.backlight();
  lcd.createChar(0, phi); // Custom PHI char for LCD
  lcd.home();
  lcd.print("AllAboutCircuits");
  lcd.setCursor(0, 1);
  lcd.print("Signal Generator");
  delay(2000);

  // Display initial set values for freq, phase, mode and power
  lcd.clear();
  displayFrequency();
  displayMode();

#ifdef usePhase
  displayPhase();
#endif

  displayPower();

#ifndef usePhase
  displayFreqRegister();
#endif

  // Initialise the AD9833 with 1KHz sine output, no phase shift for both
  // registers and remain on the FREQ0 register
  // sigGen.lcdDebugInit(&lcd);
  sigGen.reset(1);
  sigGen.setFreq(frequency0);
  sigGen.setPhase(phase);
  sigGen.setFPRegister(1);
  sigGen.setFreq(frequency1);
  sigGen.setPhase(phase);
  sigGen.setFPRegister(0);
  sigGen.mode(currentMode);
  sigGen.reset(0);

  // Set pins A and B from encoder as interrupts
  attachInterrupt(digitalPinToInterrupt(2), encChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), encChange, CHANGE);

  // Initialise pin as input with pull-up enabled and debounce variable for
  // encoder button
  pinMode(4, INPUT_PULLUP);
  lastButtonPress = millis();
  lastButtonState = 1;
  button = 1;

  // Set Cursor to initial position
  lcd.setCursor(0, 0);
}

void loop()
{
  checkButton(); // Check to see if the button has been pressed

  if (updateDisplay == true) // Update display if needed
  {
    displayFrequency();

#ifdef usePhase
    displayPhase();
#endif

    displayPower();

#ifndef usePhase
    displayFreqRegister();
#endif

    displayMode();
    updateDisplay = false;
  }

  // We are using the variable menuState to know where we are in the menu and
  // what to do in case we press the button or increment/decrement via the
  // encoder
  // Enter setting mode if the button has been pressed and display blinking
  // cursor over options (menuState 0)
  // Pick a setting (menuState 1)
  // Change that particular setting and save settings (menuState 2-5)
  switch (menuState)
  {
  case 0: // Default state
  {
    lcd.noBlink();
    if (button == 0)
    {
      button = 1;
      lcd.setCursor(0, 0);
      lcd.blink();
      menuState = 1;
      cursorPos = 0;
    }
  }
  break;

  case 1: // Settings mode
  {
    if (button == 0)
    {
      button = 1;

      if (cursorPos == 1) // If the setting in Power just toggle between on and off
      {
        currentPowerState = abs(1 - currentPowerState);
        updateDisplay = true;
        menuState = 0;
        if (currentPowerState == 1)
        {
          sigGen.sleep(3); // Both DAC and clock turned OFF
        }
        else
        {
          sigGen.sleep(0); // DAC and clock are turned ON
        }
      }

#ifndef usePhase // If usePhase has not been set
      else if (cursorPos == 2)
      {
        updateDisplay = true;
        menuState = 0; // return to "main menu"

        if (freqRegister == 0)
        {
          freqRegister = 1;
          sigGen.setFPRegister(1);
          frequency = frequency1;
        }
        else
        {
          freqRegister = 0;
          sigGen.setFPRegister(0);
          frequency = frequency0;
        }
      }
#endif

      else // Otherwise just set a new state
      {
        menuState = cursorPos + 2;
      }
    }

    if (lastCursorPos != cursorPos) // Move the cursor position in case it changed
    {
      unsigned char realPosR = 0;
      unsigned char realPosC;

      if (settingsPos[cursorPos] < 16)
        realPosC = settingsPos[cursorPos];
      else
      {
        realPosC = settingsPos[cursorPos] - 16;
        realPosR = 1;
      }

      lcd.setCursor(realPosC, realPosR);
      lastCursorPos = cursorPos;
    }
  }

  case 2: // Frequency setting
  {
    // Each button press will either enable to change the value of another digit
    // or if all digits have been changed, to apply the setting
    if (button == 0)
    {
      button = 1;
      if (digitPos < 7)
        digitPos++;
      else
      {
        digitPos = 0;
        menuState = 0;
        sigGen.setFreq(frequency);
      }
    }
    else if (button == 2)
    {
      button = 1;
      digitPos = 0;
      menuState = 0;
    }

    lcd.setCursor(9 - digitPos, 0); // Set the blinking cursor on the digit you can currently modify
  }
  break;

  case 4: // Phase setting
  {
    if (button == 0)
    {
      button = 1;
      if (digitPos < 3)
        digitPos++;
      else
      {
        digitPos = 0;
        menuState = 0;
        sigGen.setPhase(phase);
      }
    }
    lcd.setCursor(5 - digitPos, 1);
  }
  break;

  case 5: // Change the mode (sine/triangle/clock)
  {
    if (button == 0)
    {
      button = 1;
      menuState = 0;
      sigGen.mode(currentMode);
    }
    lcd.setCursor(13, 1);
  }
  break;

  default: // Just in case we messed something up
  {
    menuState = 0;
  }
  }
}
