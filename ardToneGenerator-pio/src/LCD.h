/////////////////////////////// get length ///////////////////////////////
int getLength(int value)
{
  int length;
  if (value > 9999)
    length = 5;
  else if (value > 999)
    length = 4;
  else if (value > 99)
    length = 3;
  else if (value > 9)
    length = 2;
  else
    length = 1;

  return length;
}

/////////////////////////////// print to LCD ///////////////////////////////
void printToLCD()
{
  /*******************************
    Print wave type
  *******************************/
  if (modeDecVal != oldModeDecVal)
  {
    lcd.setCursor(typeX, typeY);
    if (modeDecVal == 0)
    {
      modeText = "Sine";
    }
    else if (modeDecVal == 1)
    {
      modeText = "Sawtooth";
    }
    else if (modeDecVal == 2)
    {
      modeText = "Triangle";
    }
    else
    {
      modeText = "Square";
    }
    lcd.print(modeText);

    if (modeDecVal >= 3)
    {
      lcd.setCursor(typeX + modeText.length(), typeY);
      if (modeDecVal == 3)
      {
        sqwText = " 5";
      }
      else if (modeDecVal == 4)
      {
        sqwText = " 15";
      }
      else if (modeDecVal == 5)
      {
        sqwText = " 30";
      }
      else if (modeDecVal == 6)
      {
        sqwText = " 50";
      }
      else if (modeDecVal == 7)
      {
        sqwText = " 70";
      }
      else if (modeDecVal == 8)
      {
        sqwText = " 85";
      }
      else
      {
        sqwText = " 95";
      }
      lcd.print(sqwText);
      lcd.setCursor(typeX + modeText.length() + sqwText.length(), typeY);
      lcd.print("%");
    }

    if (modeDecVal <= 2)
    {
      for (byte c = typeX + modeText.length(); c < lcdColumns - 5; c++)
      {
        lcd.setCursor(c, typeY);
        lcd.print(" ");
      }
    }
    else
    {
      for (byte c = typeX + modeText.length() + sqwText.length() + 1; c < lcdColumns - 5; c++)
      {
        lcd.setCursor(c, typeY);
        lcd.print(" ");
      }
    }
    oldModeDecVal = modeDecVal;
  }

  /*******************************
      Print frequency
  *******************************/
  lcd.setCursor(freqX, freqY);
  lcd.print(freqText);

  lcd.setCursor(freqX + freqText.length(), freqY);
  lcd.print(frequency);

  length1 = getLength(frequency);
  lcd.setCursor(freqX + freqText.length() + length1, freqY);
  lcd.print("Hz");

  for (byte d = freqX + freqText.length() + length1 + 2; d < lcdColumns - 2; d++)
  {
    lcd.setCursor(d, freqY);
    lcd.print(" ");
  }

  /*******************************
      Print note names
  *******************************/
  lcd.setCursor(noteX, noteY);

  if (powerDecVal >= 7)
  {
    noteName = noteNames[(powerDecVal - 7) * 7 + sweepDecVal];

    if (noteName != oldNoteName)
    {
      lcd.print(noteName);
      oldNoteName = noteName;
    }
  }
  else
  {
    lcd.print("  ");
    oldNoteName = "xx";
  }

  /*******************************
      Debugging potentiometer values
  *******************************/
  if (debug)
  {
    for (byte b = 0; b < 3; b++)
    { // show the current values of the 3 pots
      if (b == 0)
      {
        potValue = sweepDecVal;
        lcd.setCursor(debugX, debugY);
      }
      else if (b == 1)
      {
        potValue = powerDecVal;
        lcd.setCursor(debugX + 2, debugY);
      }
      else
      {
        potValue = modeDecVal;
        lcd.setCursor(debugX + 4, debugY);
      }
      lcd.print(potValue);
      /*
          length1 = getLength(potValue);
          lcd.setCursor(cursX + length1 + 1, 1);
          lcd.print("   ");
      */
    }
  }
  else
  {
    lcd.setCursor(debugX, debugY);
    lcd.print("     ");
  }
}
