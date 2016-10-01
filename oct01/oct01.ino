#include <Key.h>
#include <Keypad.h>

#include <Adafruit_ST7735.h>
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <gfxfont.h>

#define TFT_CS 10
#define TFT_RST 0
#define TFT_DC  8

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
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

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
  
}
