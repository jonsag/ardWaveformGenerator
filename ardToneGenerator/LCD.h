/////////////////////////////// get length ///////////////////////////////
int getLength(int value) {
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
void printToLCD() {
  for (byte b = 0; b < 3; b++) { // show the current values of the 3 pots
    if (b == 0) {
      cursX = 0;
    } else if (b == 1) {
      cursX = 5;
    } else {
      cursX = 10;
    }
    lcd.setCursor(cursX,1);
    lcd.print(valA[b]);

    int length1 = getLength(valA[b]);
    lcd.setCursor(cursX + length1, 1);
    lcd.print("   ");
  }
}
