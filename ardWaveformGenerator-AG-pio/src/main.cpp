#include <Arduino.h>

// Arduino Function Generator
// by Amanda Ghassaei
// http://www.instructables.com/id/Arduino-Waveform-Generator/
// April 2012

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 */

// in most of this code I have used the arduino portpin assignments to send data to pins, 
// you can read more about how that works here: 
// http://www.arduino.cc/en/Reference/PortManipulation

/*********************************************************
some notes about pin setup:
4 momentary switches control waveshape (analog in 0-3)
   -sine connects to A0
   -triangle A1
   -saw A2
   -pulse A3
freq control pot (log taper) is connected to analog in 4
pulse width modulation pot (linear taper) connected to analog in 5
PORT D (digital pins 0-7) are 8 bit function out

*********************************************************/

#include <sineForm.h>
#include <config.h>
#include <checks.h>
#include <comp.h>

void setup()
{
  // set port/pin  mode
  DDRD = 0xFF; // all outputs
  DDRC = 0x00; // all inputs
  DDRB = 0xFF; // all outputs
  // TIMER INTERRUPT SETUP

  cli(); // disable interrupts
  // timer 1:
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  // set compare match register- 100khz to start
  OCR1A = 159; // = (16 000 000 / 100 000) - 1 = 159
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 bit for 0 prescaler
  TCCR1B |= (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  samplerate = 100000;

  PORTB = 0;
  PORTB = 1 << type;

  // initialize variables
  frequency = analogRead(A5);      // initialize frequency
  freqscaled = 48 * frequency + 1; //from 1 to ~50,000 period = samplerate/freqscaled;

  pulseWidth = analogRead(A4); // initialize pulse width
  pulseWidthScaled = int(pulseWidth / 1023 * period);

  triInc = 511 / period;
  sawInc = 255 / period;
  sinInc = 20000 / period;

  sei(); // enable interrupts
}

void loop()
{
  checkFreq();
  checkShape();
  checkPW();
  PORTB = 1 << type;
}
