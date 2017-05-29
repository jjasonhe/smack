![Logo](logo.png)

## Synopsis

This repo is for the winning senior design project from the University of Texas at Austin, developed from Aug 2016 to May 2017. We created a smart bike rack with the goal of reducing the threat of bike theft. We believe Smack will greatly alter the future of university and urban transportation.

## Usage Requirements

* Keil uVision 5 (not 4, like we use in 319K/445L)
* Energia (for some older files)
* Arduino (for even older files)

## Folder Structure

In the process of trying different microcontrollers and technologies, we made quite a mess. This should help explain where to begin.

### /keil/smatrix

This Keil project was used to test the Matrix Keypad system. If anything isn't working, you can turn to it to see how the Matrix Keypad should be set up and run. It is based on ValvanoWare's MatrixKeypad project.

### /msp432

I have no idea what this was for, to be honest. I think I was trying to port the MSP432 Keil code into the Arduino/Energia interface. It wasn't working so hot for me.

### /nov13

This was our original Arduino code, intended to run on an Arduino Mega. We have since departed from this, primarily because we wanted to create a custom PCB, and we didn't know how to flash a custom PCB with an Atmel chip (the kind Arduinos use).

### /SMACKWIFI

I believe this is the working code! If you go into the SMACKWIFI folder, then go into KEIL and open the uvprojx file in Keil uVision 5, you should be able to rebuild the project and flash it and make it work. The logic should be complete, the only things to note: the GPIO pins used can and should be modified depending on what y'all use!

#### main.c

The organization of the main is pretty simple. A lot of the ugly, crazy looking code was from the TI CC3100 WiFi sample project and is used for wireless connectivity. Change lines 15 and 17 to whatever WiFi you're running on.

The code I wrote starts at line 299. Modify SLOTS and RACK for number of slots on the rack and rack ID, respectively. allig8r is a bitmap of an alligator. The OneShot timer was used to do a timeout--if the system was idle for a certain amount of time on each step, it would revert back to the main screen.

The WiFiFetchStatus, WiFiFetchPins, WiFiPushStatus, and WiFiPushPins functions were used to push things to the database (which is actually done by the MQTTPublish function). After something is sent to the database, the returned info calls the babyGotBack function on line 1248. Depending on what flag was set, it would respond differently.

And finally, starting at line 921 is the main. Some initialization code, then it connects to internet, and the process begins. Flow diagrams can be found in the Google Drive.

#### Butts.c, Locks.c, Dets.c

These three files are very similar. Butts and Dets are both inputs (buttons and "detects"), and Locks are outputs. You may need to modify the GPIO initializations and code, depending on what pins you use on the controller for each function. Biggest thing to watch out for: DO NOT overlap pins with WiFi or the Matrix Keypad!!

The buttons are the switches that are pushed when the lock is inserted into the hole. They are negative logic, so when a button is pressed, the input will go low. Normally, the inputs are high. To check what the input is, you just need to mask the PxIN register with the corresponding bit. Example: P5IN & 0x01 for P5.0.

The "detects" are for the current running through the cable, to see if a lock is cut or not. It's essentially a switch, where the cable is the switch. When the cable is not locked in, the input should be high. When the cable is locked, it shorts the input to ground (because of the metal keycap), so the input is low. So basically the same as the butts.

The locks are outputs to the solenoids. When low, there is no current across the BJTs, so they stay off, which means no current runs through the solenoid. When the outputs go high, current runs across and switches the BJTs on, so the solenoids pop into the open position.

#### Keypad.c

This is taken from the smatrix project. One thing to watch out for--MAKE SURE the GPIO pins used for the keypad initialization do not conflict with WiFi (or anything else)!! Also, don't forget to connect three 10K resistors, one for each of the column pins, between the column GPIO pin and 3.3V.

To modify the GPIO pins used for the keypad, these are the steps:
1) Go to the KeypadInit (or KeypadInitLP) function you want to use. Set all the 7 pins you want to use as inputs (clear them by doing PxDIR &= ~0xbits).
2) Go to the MatrixKeypad_Scan (or MatrixKeypad_ScanLP) function you want to use. After the line with `while(pt->direction){`, set the direction of the ROW pins so that ROW0 is in bit 0, ROW1 is in bit 1, etc. In the current example, since we use P5.0 for ROW0, the 0th bit of P5DIR needs to equal pt->direction & 0x01, the 0th bit of pt->direction. But, since we use P5.2 for ROW1 (not P5.1!!), the 2nd bit of P5DIR needs to equal the 1st bit of pt->direction. That's why I have `((pt->direction)&0x02)<<1`. The shifting puts it in 5.2, not 5.1.
3) In the same function, modify the line where we set column. In this case, you want to put the column bits in the right spots. So for example, we use P4.2 for COL1 (the second column bit). We need to get this into the 1st bit (zero-indexed) of the column variable, so we shift it over to the right 1 bit. If we wanted to use 4.3 for COL1, we would have to shift twice. Etc.

#### Tips

Biggest thing that caused us problems was using bits twice. Check the CC3100 pin map carefully! If you duplicate pins, weird things may happen and you won't know why. Let Jason know if there's stuff you can't figure out at all.

## Contributors

* Akshans Verma
* Clint Simpson
* Jason He
* Nick Sehy
* Qian He
* Ted Mao

## License

Some sort of educational license or something idk
