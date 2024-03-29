#include <Arduino.h>

/*  version 0.01; July, 2016
    sound_generator: A arduino waveform generator that uses 3 analog pots for settings and drives a LCD
    hackaday.io/project/12756-a-feature-rich-arduino-waveform-generator

    Copyright (C) <2016>  <Joachim Kutzera>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    For a copy of the GNU General Public License
    see <http://www.gnu.org/licenses/>.
*/

/* Potentiometers

   Modes: (A2)
   0: sine
   1: sawtooth
   2: triangle
   3: squarewave 5%
   4: squarewave 15%
   5: squarewave 30%
   6: squarewave 50%
   7: squarewave 70%
   8: squarewave 85%
   9: squarewave 95%

   Power: (A1)
   0: 0-9 Hz
   1: 0-90 Hz, in steps of 10
   2: 0-900 Hz, in steps of 100
   3: 0-9000 Hz, in steps of 1000
   4: 0-90000 Hz, in steps of 10000
   5: frequency (0-1023) Hz
   6: 1000 + frequency * 8.79 Hz
   7: musical, octave 0
   8: musical, octave 1
   9: musical, octave 2

   Frequency; (A0)
   0: C
   1: D
   2: E
   3: F
   4: G
   5: A
   6: H
   7: C
   8: D
   9: E
*/

#include "configuration.h" // sets all variables
#include "LCD.h"           // prints to LCD
#include "buttons.h"       // handle button presses

#if !defined(nullptr)
#define nullptr NULL
#endif

#include "FTDebouncer.h" // load library for debouncing buttons
FTDebouncer pinDebouncer(30);

void activateFreq()
{ // calculates the nextval-value for the current frequency
  unsigned long freq;
  freq = frequency * correctionFactor;
  nextVal = (freq << bitShifter) / sampleFactor;
}

void setup()
{
  /*******************************
    Start LCD
  *******************************/
  lcd.begin(lcdColumns, lcdRows);
  lcd.clear();
  lcd.setCursor(0, 0); // print name of this program and boot message to the LCD
  lcd.print(programName);
  lcd.setCursor(0, 1);
  lcd.print("Booting ...");
  delay(333);

  /*******************************
    Setup FTDebouncer pins
  *******************************/
  lcd.setCursor(0, 1);
  lcd.print("FTDebouncer ...");

  pinDebouncer.addPin(A0, LOW); // pin has external pull-down resistor
  pinDebouncer.addPin(A1, LOW);
  pinDebouncer.addPin(A2, LOW);
  pinDebouncer.addPin(5, LOW);
  pinDebouncer.addPin(6, LOW);
  pinDebouncer.init();

  delay(333);

  lcd.setCursor(0, 1);
  lcd.print("Starting ...   ");
  activateFreq();

  delay(333);
  lcd.clear();

  // TIMER INTERRUPT SETUP
  DDRD = 0xFF; // all outputs
  DDRC = 0x00; // analog inputs
  // DDRB = 0x00;//digital inputs

  cli(); // disable interrupts

  Serial.end(); // begin(9600);

  // set timer1 interrupt at 65536kHz
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1 = 0;  // initialize counter value to 0;

  // set timer count for 65536khz increments
  OCR1A = 30; // = (16*10^6) / (65536*8) - 1

  // turn on CTC mode
  TCCR1B |= (1 << WGM12);

  // Set CS11 bit for 8 prescaler
  TCCR1B |= (1 << CS11);

  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei(); // enable interrupts
}

ISR(TIMER1_COMPA_vect)
{ // the timer-driven interrupt routine

  wavePos = wavePos + nextVal;

  if (modeDecVal == 0)
  { // sine wave
    waveP = wavePos >> bitShifter;
    PORTD = pgm_read_byte_near(sinewave + waveP);
  }

  if (modeDecVal == 1)
  { // sawtooth wave
    PORTD = wavePos >> bitShiftST;
  }

  if (modeDecVal == 2)
  { // triangle wave
    if (wavePos < triSizeHalf)
    {
      PORTD = wavePos >> bitShiftTRI;
    }
    if (wavePos > triSizeHalf)
    {
      PORTD = (waveSizeScaled - wavePos) >> bitShiftTRI;
    }
  }

  if (modeDecVal > 2)
  { // square wave
    if (wavePos < squareWavePerc[modeDecVal - 3])
    {
      PORTD = 255;
    }
    else
    {
      PORTD = 0;
    }
  }
}

byte getDecVal(byte whichA)
{ // returns the decimal value (0-9) for the requested analog port

  byte retval = 0;

  if (valA[whichA] >= Arefs[whichA][9])
  {
    retval = 9;
  }
  else
  {
    while (Arefs[whichA][retval] + ((Arefs[whichA][retval + 1] - Arefs[whichA][retval]) / 2) < valA[whichA] && retval < 8)
    {
      retval++;
    }
  }

  return (retval);
}

void setCurrentFreq()
{ // gets current frequency and mode from the potentiometer positions

  // sweepDecVal = getDecVal(5); // decimal value, 0-9, of frequency potentiometer, selects frequency or musical note
  // powerDecVal = getDecVal(1); // decimal value, 0-9, of power potentiometer, selects frequency range or musical notes octave
  // modeDecVal =  getDecVal(2); // decimal value, 0-9, of mode potentiometer, selects waveform

  if (modeDecVal != oldMode)
  { // restart frequency settings on mode change
    activateFreq();
  }

  if (powerDecVal <= 4)
  { // frequency range
    freqDigits[powerDecVal] = sweepDecVal;
    setfreq = freqDigits[4] * 10000 + freqDigits[3] * 1000 + freqDigits[2] * 100 + freqDigits[1] * 10 + freqDigits[0];
  }
  else if (powerDecVal == 5)
  { // direct frequency from frequency potentiometer
    setfreq = valA[0];
  }
  else if (powerDecVal == 6)
  {
    setfreq = 1000 + (valA[0] * 8.79);
  }
  else
  { // sets a musical note frequency
    setfreq = musicalNotes[(powerDecVal - 7) * 7 + sweepDecVal];
  }

  if (setfreq != frequency)
  { // set frequency
    frequency = setfreq;
    activateFreq();
  }

  oldMode = modeDecVal;
}

void loop()
{

  /*
  for (byte a = 0; a < 3; a++) { // reading the current values of the 3 pots
    valA[a] = analogRead(a);
  }
  */

  valA[5] = analogRead(5); // reads the value for potentiometer on A5

  pinDebouncer.run(); // reads and handles all buttons

  setCurrentFreq();

  printToLCD();
}
