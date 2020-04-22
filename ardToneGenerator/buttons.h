#include "buttonActions.h" // makes buttons do something

void onPinActivated(int pinNr){
  //Serial.print("Pin activated: ");
  //Serial.println(pinNr);

  //Serial.println();
  
  switch (pinNr) {
    case 14:
      button1Action();
      break;
    case 15:
      button2Action();
      break;
    case 16:
      button3Action();
      break;
    case 5:
      button4Action();
      break;
    case 6:
      button5Action();
      break;
  }

  /*
  Serial.print("Value 1: ");
  Serial.print(value1);
  Serial.print(", Value 2: ");
  Serial.print(value2);
  Serial.print(", Value 3: ");
  Serial.println(value3);
  */
}

void onPinDeactivated(int pinNr){ // this does nothing, but is required
  //Serial.print("Pin deactivated: ");
  //Serial.println(pinNr);
}
