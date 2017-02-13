#include <SparkFunESP8266Client.h>
#include <SparkFunESP8266Server.h>
#include <SparkFunESP8266WiFi.h>
#include <SoftwareSerial.h>
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
#define motorHi 2
#define motorLo 3

const char mySSID[] = "Albaert";
const char myPSK[] = "flyingbanana";

const char destServer[] = "http://lowcost-env.eweetsjdhy.us-west-2.elasticbeanstalk.com/";
const String htmlHeader = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/html\r\n"
                          "Connection: close\r\n\r\n"
                          "<!DOCTYPE HTML>\r\n"
                          "<html>\r\n";

const String httpRequest = "GET / HTTP/1.1\n"
                           "Host: http://lowcost-env.eweetsjdhy.us-west-2.elasticbeanstalk.com/\n"
                           "Connection: close\n\n";

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
boolean lockWrong; // 0 = right, 1 = wrong
boolean unlkWrong; // 0 = right, 1 = wrong

void setup() {
  // put your setup code here, to run once:
  // init SERIAL
  Serial.begin(9600);
  Serial.println("hello");
  // init LCD
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_WHITE);
  Serial.println("LCD initialized");
  // init MOTOR
  pinMode(motorHi, OUTPUT);
  pinMode(motorLo, OUTPUT);
  analogWrite(motorHi, 0);
  analogWrite(motorLo, 0);
  Serial.println("Motor pin initialized");
  // init WIFI
  initializeESP8266();
  connectESP8266();
  displayConnectInfo();
  serialTrigger(F("Press any key to connect client."));
  clientDemo();
}

void loop() {
  // prompt user to push #
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_BLUE);
  tft.setTextWrap(true);
  tft.print("Hi! Press # to begin.");
  char key;
  while (key != '#') {
    key = keypad.waitForKey();
  }
  
  // prompt user to enter slot #
  tft.setCursor(0, 20);
  tft.setTextColor(ST7735_BLUE);
  tft.setTextWrap(true);
  tft.print("Now enter the slot number you want to access.");
  key = keypad.waitForKey();
  slot = (atoi(key)) - 1;
  
  // check if taken
  tft.fillScreen(ST7735_WHITE);
  if (occupied[slot]) {
    // if the slot is taken, treat as unlocking

    // prompt user to enter pin # to unlock their bike
    tft.setCursor(0, 0);
    tft.setTextColor(ST7735_BLUE);
    tft.setTextWrap(true);
    tft.print("To unlock your bike, enter your pin.");
    for (int i = 0; i < 4; i += 1) {
      key = keypad.waitForKey();
      if (pins[slot][i] != atoi(key)) {
        unlkWrong = true;
        break;
      }
    }

    // if not correct pin, start over
    if (unlkWrong) {
      tft.setCursor(0, 20);
      tft.setTextColor(ST7735_BLUE);
      tft.setTextWrap(true);
      tft.print("Uh-oh, that's incorrect! Start over.");
      unlkWrong = false;
      break;
    }
    
    tft.setCursor(0, 20);
    tft.setTextColor(ST7735_BLUE);
    tft.setTextWrap(true);

    // unlock motor 0.5s
    analogWrite(motorHi, 0);
    analogWrite(motorLo, 255);
    delay(500);
    analogWrite(motorLo, 0);
    
    tft.print("Good job! You have 60 sec to get your bike.");
    // THIS IS WHERE WE START 60s TIMER AND USER GOES TO UNLOCK
    delay(60000);

    // lock motor, 0.5s
    analogWrite(motorLo, 0);
    analogWrite(motorHi, 255);
    delay(500);
    analogWrite(motorHi, 0);
  } else {
    // if the slot isn't, treat as locking
    
    // prompt user to enter pin # to lock their bike
    tft.setCursor(0, 0);
    tft.setTextColor(ST7735_BLUE);
    tft.setTextWrap(true);
    tft.print("To lock your bike, enter a pin number.");
    for (int i = 0; i < 4; i += 1) {
      key = keypad.waitForKey();
      pins[slot][i] = atoi(key);
    }
    
    // prompt user to confirm pin #
    tft.setCursor(0, 20);
    tft.setTextColor(ST7735_BLUE);
    tft.setTextWrap(true);
    tft.print("Re-enter your pin to confirm.");
    for (int j = 0; j < 4; j += 1) {
      key = keypad.waitForKey();
      if (pins[slot][j] != atoi(key)) {
        lockWrong = true;
        break;
      }
    }
    
    // if not same pin, start over
    if (lockWrong) {
      tft.setCursor(0, 40);
      tft.setTextColor(ST7735_BLUE);
      tft.setTextWrap(true);
      tft.print("Uh-oh, that's not the same pin! Start over.");
      lockWrong = false;
      break;
    }
    
    tft.setCursor(0, 40);
    tft.setTextColor(ST7735_BLUE);
    tft.setTextWrap(true);

    // unlock motor, 0.5s
    analogWrite(motorHi, 0);
    analogWrite(motorLo, 255);
    delay(500);
    analogWrite(motorLo, 0);
    
    tft.print("Good job! You have 60 sec to lock your bike.");
    // THIS IS WHERE WE START 60s TIMER AND USER GOES TO LOCK

    delay(60000);

    // lock motor, 0.5s
    analogWrite(motorLo, 0);
    analogWrite(motorHi, 255);
    delay(500);
    analogWrite(motorHi, 0);
  }
}

void initializeESP8266() {
  // esp8266.begin() verifies that the ESP8266 is operational
  // and sets it up for the rest of the sketch.
  // It returns either true or false -- indicating whether
  // communication was successul or not.
  // true
  int test = esp8266.begin();
  if (test != true)
  {
    Serial.println(F("Error talking to ESP8266."));
    errorLoop(test);
  }
  Serial.println(F("ESP8266 Shield Present"));
}

void connectESP8266() {
  // The ESP8266 can be set to one of three modes:
  //  1 - ESP8266_MODE_STA - Station only
  //  2 - ESP8266_MODE_AP - Access point only
  //  3 - ESP8266_MODE_STAAP - Station/AP combo
  // Use esp8266.getMode() to check which mode it's in:
  int retVal = esp8266.getMode();
  if (retVal != ESP8266_MODE_STA)
  { // If it's not in station mode.
    // Use esp8266.setMode([mode]) to set it to a specified
    // mode.
    retVal = esp8266.setMode(ESP8266_MODE_STA);
    if (retVal < 0)
    {
      Serial.println(F("Error setting mode."));
      errorLoop(retVal);
    }
  }
  Serial.println(F("Mode set to station"));

  // esp8266.status() indicates the ESP8266's WiFi connect
  // status.
  // A return value of 1 indicates the device is already
  // connected. 0 indicates disconnected. (Negative values
  // equate to communication errors.)
  retVal = esp8266.status();
  if (retVal <= 0)
  {
    Serial.print(F("Connecting to "));
    Serial.println(mySSID);
    // esp8266.connect([ssid], [psk]) connects the ESP8266
    // to a network.
    // On success the connect function returns a value >0
    // On fail, the function will either return:
    //  -1: TIMEOUT - The library has a set 30s timeout
    //  -3: FAIL - Couldn't connect to network.
    retVal = esp8266.connect(mySSID, myPSK);
    if (retVal < 0)
    {
      Serial.println(F("Error connecting"));
      errorLoop(retVal);
    }
  }
}

void displayConnectInfo() {
  char connectedSSID[24];
  memset(connectedSSID, 0, 24);
  // esp8266.getAP() can be used to check which AP the
  // ESP8266 is connected to. It returns an error code.
  // The connected AP is returned by reference as a parameter.
  int retVal = esp8266.getAP(connectedSSID);
  if (retVal > 0)
  {
    Serial.print(F("Connected to: "));
    Serial.println(connectedSSID);
  }

  // esp8266.localIP returns an IPAddress variable with the
  // ESP8266's current local IP address.
  IPAddress myIP = esp8266.localIP();
  Serial.print(F("My IP: ")); Serial.println(myIP);
}

void clientDemo() {
  // To use the ESP8266 as a TCP client, use the 
  // ESP8266Client class. First, create an object:
  ESP8266Client client;

  // ESP8266Client connect([server], [port]) is used to 
  // connect to a server (const char * or IPAddress) on
  // a specified port.
  // Returns: 1 on success, 2 on already connected,
  // negative on fail (-1=TIMEOUT, -3=FAIL).
  int retVal = client.connect(destServer, 80);
  if (retVal <= 0)
  {
    Serial.println(F("Failed to connect to server."));
    return;
  }

  // print and write can be used to send data to a connected
  // client connection.
  client.print(httpRequest);

  // available() will return the number of characters
  // currently in the receive buffer.
  while (client.available())
    Serial.write(client.read()); // read() gets the FIFO char
  
  // connected() is a boolean return value - 1 if the 
  // connection is active, 0 if it's closed.
  if (client.connected())
    client.stop(); // stop() closes a TCP connection.
}
