
/*******************************
  button1
*******************************/
void button1Action() {

  //Serial.println("Button 1 pressed");

  modeDecVal += 1;
  if (modeDecVal >= 10) {
    modeDecVal = 0;
  }

}

/*******************************
  button2
*******************************/
void button2Action() {

  //Serial.println("Button 2 pressed");

  powerDecVal += 1;
  if (powerDecVal >= 10) {
    powerDecVal = 0;
  }

}

/*******************************
  button3
*******************************/
void button3Action() {

  //Serial.println("Button 3 pressed");

  sweepDecVal += 1;
  if (sweepDecVal >= 10) {
    sweepDecVal = 0;
  }
  
}

/*******************************
  button4
*******************************/
void button4Action() {

  //Serial.println("Button 4 pressed");

}

/*******************************
  button5
*******************************/
void button5Action() {

  //Serial.println("Button 5 pressed");

}
