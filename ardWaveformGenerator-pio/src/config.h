
// Uncomment the line below if you want to change the Phase instead of the FREQ register
// #define usePhase

AD9833 sigGen(10, 24000000);        // Initialise our AD9833 with FSYNC pin = 10 and a master clock frequency of 24MHz
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD Initialise
Rotary encoder(3, 2);               // Initialise the encoder on pins 2 and 3 (interrupt pins)

// Variables used to input data and walk through menu
unsigned long encValue;        // Value used by encoder
unsigned long lastButtonPress; // Value used by button debounce
unsigned char lastButtonState;
unsigned char settingsPos[] = {0, 14, 20, 29};
unsigned char button;
volatile unsigned char cursorPos = 0;
unsigned char lastCursorPos = 0;
unsigned char menuState = 0;
const int buttonPin = 4;
int digitPos = 0;
const unsigned long maxFrequency = 14000000;
const unsigned int maxPhase = 4095; // Only used if you enable PHASE setting instead of FREQ register
unsigned long newFrequency = 1000;
volatile bool updateDisplay = false;
unsigned long depressionTime;
int freqRegister = 0; // Default FREQ register is 0

// LCD constants
const String powerState[] = {" ON", "OFF"};
const String mode[] = {"SIN", "TRI", "CLK"};

// Variables used to store phase, frequency, mode and power
unsigned char currentMode = 0;
unsigned long frequency0 = 1000;
unsigned long frequency1 = 1000;
unsigned long frequency = frequency0;
unsigned long currFrequency; // Current frequency used, either 0 or 1
unsigned long phase = 0;     // Only used if you enable PHASE setting instead of FREQ register
unsigned char currentPowerState = 0;

// Greek PHI symbol for phase shift
// Only used if you enable PHASE setting instead of FREQ register
uint8_t phi[8] = {0b01110, 0b00100, 0b01110, 0b10101,
                  0b10101, 0b01110, 0b00100, 0b01110};
