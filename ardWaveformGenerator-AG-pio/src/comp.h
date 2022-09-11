
ISR(TIMER1_COMPA_vect)
{ // timer 1 interrupt
  // increment t and reset each time it reaches period
  t += 1;
  if (t >= period)
  {
    t = 0;
  }
  switch (type)
  {
  case 0: // pulse
    if (pulseWidthScaled <= t)
    {
      wave = 255;
    }
    else
    {
      wave = 0;
    }
    break;
  case 1: // triangle
    if ((period - t) > t)
    {
      if (t == 0)
      {
        triByte = 0;
      }
      else
      {
        triByte += triInc;
      }
    }
    else
    {
      triByte -= triInc;
    }
    if (triByte > 255)
    {
      triByte = 255;
    }
    else if (triByte < 0)
    {
      triByte = 0;
    }
    wave = triByte;
    break;
  case 2: // saw
    if (t = 0)
    {
      sawByte = 0;
    }
    else
    {
      sawByte += sawInc;
    }
    wave = sawByte;
    break;
  case 3: // sine
    sinNum = t * sinInc;
    wave = pgm_read_byte_near(sine20000 + sinNum);
    break;
  }

  PORTD = wave;
}
