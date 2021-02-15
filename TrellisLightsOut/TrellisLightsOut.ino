#include <Wire.h>
#include "Adafruit_Trellis.h"

Adafruit_Trellis matrix0 = Adafruit_Trellis();

Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0);

#define NUMTRELLIS 1

#define numKeys (NUMTRELLIS * 16)

// Connect Trellis Vin to 5V and Ground to ground.
// Connect the INT wire to pin #5 (can change later
#define INTPIN 5
// Connect I2C SDA pin to your Arduino SDA line
// Connect I2C SCL pin to your Arduino SCL line
// All Trellises share the SDA, SCL and INT pin! 
// Even 8 tiles use only 3 wires max


void setup() {
  Serial.begin(9600);
  Serial.println("Trellis Lights Out");

  // INT pin requires a pullup
  pinMode(INTPIN, INPUT);
  digitalWrite(INTPIN, HIGH);

  trellis.begin(0x70);
  
  for (uint8_t i=0; i<numKeys; i++) {
    trellis.setLED(i);
    trellis.writeDisplay();
    delay(50);
  }  

  clearBoard();
  trellis.writeDisplay();

  // Set up a random board
  makeRandomBoard();
}

void toggle(int placeVal) {
  if (trellis.isLED(placeVal)) {
    trellis.clrLED(placeVal);
  } else {
    trellis.setLED(placeVal);
  }
}

int toggleNeighbours(int placeVal) {
  //  {0, 1, 2, 3},
  //  {4, 5, 6, 7},
  //  {8, 9, 10,11},
  //  {12,13,14,15}

  int right = placeVal + 1;
  if (right % 4 != 0) { toggle(right); } // don't if it wrapped

  int down = placeVal + 4;
  if (down < 16) { toggle(down); }

  int left = placeVal - 1;
  if (left >= 0 && left != 3 && left != 7 && left != 11 && left != 15) { toggle(left); } // don't if it wrapped

  int up = placeVal - 4;
  if (up >= 0) { toggle(up); }
}

void clearBoard() {
  for (int i=0; i<16; i++) {
    trellis.clrLED(i);
  }
}

void checkForWin() {
  for (int i=0; i<16; i++) {
    if (trellis.isLED(i)) {
      return;
    }
  }
  // must all be out if we reach here

  for (int loops=0; loops<20; loops++) {
    for (int i=0; i<16; i++) {
      toggle(i);
    }
    trellis.writeDisplay();
    delay(100);
  }

  clearBoard();
  trellis.writeDisplay();
  makeRandomBoard();
}

void makeRandomBoard() {
  int x = 0;
  for (int i=0; i<10; i++) {
    x = random(0,15);
    makeYourMove(x);
  }
  trellis.writeDisplay();
  delay(100);
}

void makeYourMove(int placeVal) {
  toggle(placeVal);
  toggleNeighbours(placeVal);
}

void loop() {
  delay(30); // 30ms delay is required, dont remove me! (?)

  // If a button was just pressed or released...
  if (trellis.readSwitches()) {
    // go through every button
    for (uint8_t i=0; i<numKeys; i++) {
      if (trellis.justPressed(i)) {
        makeYourMove(i);
      }
    }
    // tell the trellis to set the LEDs we requested
    trellis.writeDisplay();

    // check for a win condition (all out)
    checkForWin();
  }
}
