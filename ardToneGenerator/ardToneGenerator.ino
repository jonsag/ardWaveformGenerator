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

// some R-code to make 16384 bytes of sinewave:
// sine = round((sin(seq(0,2*pi, length=16384))+1)*127.7)
// sink("temp.txt")
// for (a in 1:length(sine)) cat(sine[a],",")
// sink()

#include "configuration.h" // sets all variables
#include "LCD.h" // prints to LCD





void activateFreq() { // calculates the nextval-value for the current frequency
  unsigned long freq;
  freq = frequency * correctionFactor;
  nextVal = (freq << bitShifter) / sampleFactor;
}

void setup() {
  /*******************************
    Start LCD
  *******************************/
  lcd.begin(lcdColumns, lcdRows);
  lcd.clear();
  lcd.setCursor(0, 0); // print name of this program and boot message to the LCD
  lcd.print(programName);
  lcd.setCursor(0, 1);
  lcd.print("Booting ...");
  delay(1000);
  lcd.clear();

  //cp.reset();
  activateFreq();
  freqDisplay = frequency;
  //pinMode(dispPin, INPUT);
  //set port/pin  mode
  //TIMER INTERRUPT SETUP
  DDRD = 0xFF;//all outputs
  DDRC = 0x00;//analog inputs
  //DDRB = 0x00;//digital inputs
  cli();//disable interrupts
  Serial.end(); // begin(9600);
  //set timer1 interrupt at 65536kHz
  TCCR1A = 0;// set entire TCCR1A register to 0
  //TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0;
  // set timer count for 65536khz increments
  OCR1A = 30; // = (16*10^6) / (65536*8) - 1
  // turn on CTC mode
  //TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  //TCCR1B |= (1 << CS11);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // enable interrupts
}

ISR(TIMER1_COMPA_vect) { // the timer-driven interrupt routine
  wavePos = wavePos + nextVal;
  if (modeDecVal == 0) { // sine wave
    waveP = wavePos >> bitShifter;
    PORTD = pgm_read_byte_near(sinewave + waveP);
  }
  if (modeDecVal == 1) { // sawtooth wave
    PORTD = wavePos >> bitShiftST;
  }
  if (modeDecVal == 2) { // triangle wave
    if (wavePos < triSizeHalf)
      PORTD = wavePos >> bitShiftTRI;
    if (wavePos > triSizeHalf)
      PORTD = (waveSizeScaled - wavePos) >> bitShiftTRI;
  }
  if (modeDecVal > 2) { // square wave
    if (wavePos < squareWavePerc[modeDecVal - 3])
      PORTD = 255;
    else
      PORTD = 0;
  }
}



byte getDecVal(byte whichA) { // returns the decimal value (0-9) for the requested analog port
  byte retval = 0;
  if (valA[whichA] >= Arefs[whichA][9])
    retval = 9;
  else
    while (Arefs[whichA][retval] + ( (Arefs[whichA][retval + 1] - Arefs[whichA][retval]) / 2 ) < valA[whichA] && retval < 8)
      retval ++;
  return (retval);
}

void setCurrentFreq() { // gets current frequency from the poti positions
  sweepDecVal = getDecVal(0);
  powerDecVal = getDecVal(1);
  modeDecVal =  getDecVal(2);
  if (modeDecVal != oldMode)
    activateFreq();
  if (powerDecVal <= 4) {
    freqDigits[powerDecVal] = sweepDecVal;
    setfreq = freqDigits[4] * 10000 + freqDigits[3] * 1000 + freqDigits[2] * 100 + freqDigits[1] * 10 + freqDigits[0];
  }
  if (powerDecVal == 5)
    setfreq = valA[0];
  if (powerDecVal == 6)
    setfreq = 1000 + (valA[0] * 8.79);
  if (powerDecVal > 6)
    setfreq = musicalNotes[(powerDecVal - 7) * 7 + sweepDecVal];
  if (setfreq != frequency) {
    frequency = setfreq;
    activateFreq();
  }
  oldMode = modeDecVal;
}

void loop() {
  /*  if (digitalRead(dispPin) == 1) {
      if (!dispOn) {
        cp.reset();
        freqDisplay = frequency;
        dispOn = true;
      }
      if ((freqDisplay < frequency - 2) || (freqDisplay > frequency + 2)) // avoids a too shaky number in sweep mode
        freqDisplay = frequency;
      if (powerDecVal < 5)
        freqDisplay = frequency;
      cp.setMultiplier(delayMultipliers[modeDecVal]); // having the display update reasonably fast in each waveform requires different multipliers
      cp.writeNumber(freqDisplay);
    } else {
      dispOn = false;
    }
  */

  for (byte a = 0; a < 3; a++) // reading the current values of the 3 pots
    valA[a] = analogRead(a);
  setCurrentFreq();

  printToLCD();
  /*
  for (byte count = 0; count < 3; count++) { // show the current values of the 3 pots
    if (count == 0) {
      cursX = 0;
    } else if (count == 1) {
      cursX = 5;
    } else {
      cursX = 10;
    }
    lcd.setCursor(cursX, 1);
    lcd.print(valA[count]);
    //lcd.print("hej");
  }
  */
}