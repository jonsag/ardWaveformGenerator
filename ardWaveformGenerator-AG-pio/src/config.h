
// wavetype storage:
// 0 is pulse
// 1 is triangle
// 2 is saw
// 3 is sine
byte type = 0; // initialize as square
byte typecurrent = 8;
byte typelast;
byte typecounter[4];
byte i;

// variables for PW pot monitoring
float pulseWidth;
int pulseWidthScaled;
int PWCurrent;
byte PWTolerance = 8; // adjust this to increase/decrease stability of PW measurement

// variables for freq pot monitoring
int frequency;
int freqCurrent;
byte freqTolerance = 2; // adjust this to increase/decrease stability of frequency measurement
unsigned int freqscaled;

byte wave;
long t;
long samplerate;
long period;

// storage variables- I used these to cut down on the math being performed during the interrupt
int sawByte = 0;
byte sawInc;
int triByte = 0;
byte triInc;
int sinNum = 0;
int sinInc;
