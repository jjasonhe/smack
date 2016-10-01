#include <Key.h>
#include <Keypad.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <gfxfont.h>

#define TFT_CS  10
#define TFT_RST 0
#define TFT_DC  8
#define SLOTS   4

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_C, TFT_RST);

const byte rows = 4; //four rows
const byte cols = 3; //three columns
char keys[rows][cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};
byte rowPins[rows] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[cols] = {8, 7, 6}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

boolean occupied[SLOTS] = 0; // 0 = not taken, 1 = taken

byte pins[SLOTS][4] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};

int slot;
boolean pinWrong; // 0 = right, 1 = wrong

void setup() {
  // put your setup code here, to run once:
  // init SERIAL
  Serial.begin(9600);
  Serial.println("hello");
  // init LCD
  tft.initR(INITR_BLACKTAB);
  Serial.println("LCD initialized");
}

void loop() {
  // put your main code here, to run repeatedly:

  // prompt user to push #
  char key;
  while (key != '#') {
    key = keypad.waitForKey();
  }
  // prompt user to enter slot #
  key = keypad.waitForKey();
  slot = (atoi(key)) - 1;
  // check if taken
  if (occupied[slot]) {
    // if the slot is taken, treat as unlocking


    
  } else {
    // if the slot isn't, treat as locking
    
    // prompt user to enter pin # to lock their bike
    for (int i = 0; i < 4; i += 1) {
      key = keypad.waitForKey();
      pins[slot][i] = atoi(key);
    }
    
    // prompt user to confirm pin #
    for (int j = 0; j < 4; j += 1) {
      key = keypad.waitForKey();
      if (pins[slot][j] != atoi(key)) {
        pinWrong = true;
      }
    }
    
    // if not same pin, start over
    if (pinWrong) {
      break;
    }

    // THIS IS WHERE WE START 60s TIMER AND USER GOES TO LOCK
  }
}
