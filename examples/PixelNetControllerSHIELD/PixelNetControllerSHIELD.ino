/*
  Brilldea
  Timothy D. Swieter, P.E.
  Copyright (c) 2012 Timothy D. Swieter, P.E..  All right reserved.
  
  UPDATED: January 28, 2012

  PROGRAM DESCRIPTION:
  This program demonstrates the use of the BrilldeaA6281 IC library for the Arduino and on the PixelNet controller-SHIELD.
  
  REFERENCE:
    A6281 Datasheet:  http://www.allegromicro.com/en/Products/Part_Numbers/6281/6281.pdf
*/

// Import the BrilldeaA6281 IC Library.  The A6281 IC is used on the PixelNet Controller-SHIELD.
#include <BrilldeaA6281.h>

/*
First order of business is to define the way the PixelNet Controller-SHIELD is connected to the
Arduino.  There are four data signals.   The four data signals are as follows.  The PixelNet Controller-SHIELD
uses a Brilldea PolkaDOT-51.
*/
int pin_CLOCK    = 13; // Clock Input (CI).  An Arduino output.
int pin_ENABLE   = 12; // Output Enable Input (OEI).  An Arduino output.
int pin_LATCH    = 11; // Latch Input (LI).  An Arduino output.
int pin_DATA     = 10; // Serial Data Input (SDI).  An Arduino output.

int pin_LED      = 9;  // General purpose LED.  An Arduino output.
int pin_BUTTON   = 8;  // General purpose Button.  An Arduino input, externally pulled-high.  Button press = LOW.

/*
Up next you want to define how many A6281 ICs are in the chain, for the PixelNet Controller-SHIELD, we start
with just a single IC.  More can be added by adding additional PolkaDOT-51s or Led Painter II-8s.
In addition to defining the number of ICs, we need to define the arrays for holding the data that 
each IC requires such as the dot correction of each LED and the intensity of each LED.

Adjust the "LPs = x" where x is the number of LPII-8s connected in the chain.
*/
const byte ICs = 1;             // Number of A6281 ICs, just one for the PixelNet Controller-ShIELD.  This should NOT exceed 250 ICs
byte DotCorrection[ICs * 3];    // A byte of date for each Dot Correction value for each output on each IC.  ICs * 3, only the lower 7-bits of each element are used. 0 to 127.
int Intensity[ICs * 3];         // A integer for each Intensity value for each output on each IC.  ICs * 3, only the lower 10-bits of each element are used.  0 to 1023.

// Finally we need to establish an instance of the library.  We are calling the instant "PixelNet"  This will
// setup the proper pins and IC count.
BrilldeaA6281 PixelNet(ICs, pin_CLOCK, pin_ENABLE, pin_LATCH, pin_DATA);

// The setup loop, executed only once before the main loop
void setup() {  

  // Now we are into executing code.  Let's first make sure everything is initialized in each A6281 IC in the chain.
  /*
  First up is the Dot Correction.  Dot Correction is adjustments in the overall current the A6281 IC will output on each output.
  On the A6281 IC, there is an external resistor which sets the maximum current allowed on all three outputs.  The Dot Correction
  value can be applied to scale the output to less than this maximum.  Please review the A6281 IC data sheet to understand this value.  
  To have 100% output, the dot correction should be set at 127.  To have 36.5% output, the Dot Correction can be set at 0.
  After we update the Dot Correction array, we need to then send the data to the ICs.  Usually the Dot Correction is established
  at the system startup and is rarely used after this first setup.
  
  To be safe, the below code is limiting the dot correction to so it doesn't over drive any LEDs you have attached by setting the Dot Correction to 0.  
  Be sure to understand Dot Correction so that you can use your LEDs to the fullest capability.
  */
  PixelNet.DotCorrectionSetAllIC(DotCorrection, 0, 0, 0);
  PixelNet.SendPacketDotCorrection(DotCorrection);  
  
  /*
  Second is the Intensity.  Intensity is the brightness or current for each output.  This is a 10 bit value from 0 (off) to 1023 (brightest).
  The intensity value is the unit the system will change frequently.  It is recommend that you update the array with all the "frame" changes
  and the send the entire packet at once.  Think of it like painting into a "video buffer" and then sending the packet updates to the screen.
  This will make more sense as you review the code in the "loop".
  */
  PixelNet.IntensitySetAllIC(Intensity, 0, 0, 0);      
  PixelNet.SendPacketIntensity(Intensity); 
  
  // Finally enable the chain of ICs so the outputs are on.
  PixelNet.EnableIC(true);
  
  // Next we are going to establish the general purpose LED and Button I/O.
  pinMode(pin_LED, OUTPUT);
  pinMode(pin_BUTTON, INPUT);
  
}
// End of setup


// The main loop where all the action happens.
void loop() {
  
  // Once you get to here, the ICs are all initialized and eabled, but off.
  // Lets light them up!

  /*
  First we will change color.  We will cycle through all Out0 then all Out1 and then all Out2 with the following code.
  Be sure to send the packet after each array update.  The general purpose LED is blinked while doing the A6281 transactions.
  */
  // Out0
  digitalWrite(pin_LED, HIGH);
  PixelNet.IntensitySetAllIC(Intensity, 1023, 0, 0);
  PixelNet.SendPacketIntensity(Intensity);
  digitalWrite(pin_LED, LOW);  
  delay(2000);
  
  // Out1
  digitalWrite(pin_LED, HIGH);  
  PixelNet.IntensitySetAllIC(Intensity, 0, 1023, 0);
  PixelNet.SendPacketIntensity(Intensity);
  digitalWrite(pin_LED, LOW);   
  delay(2000);  
  
  // Out2
  digitalWrite(pin_LED, HIGH);  
  PixelNet.IntensitySetAllIC(Intensity, 0, 0, 1023);
  PixelNet.SendPacketIntensity(Intensity);
  digitalWrite(pin_LED, LOW);   
  delay(2000);    
  
  // Everything on
  digitalWrite(pin_LED, HIGH);  
  PixelNet.IntensitySetAllIC(Intensity, 1023, 1023, 1023);
  PixelNet.SendPacketIntensity(Intensity);
  digitalWrite(pin_LED, LOW);   
  delay(2000); 
  
  // Everything off
  digitalWrite(pin_LED, HIGH);  
  PixelNet.IntensitySetAllIC(Intensity, 0, 0, 0);
  PixelNet.SendPacketIntensity(Intensity);
  digitalWrite(pin_LED, LOW);   
  delay(500);   
  
  // Prompt for the button to be pressed, then enter a fading of the RGB LED
  while (digitalRead(pin_BUTTON)) {
    
    // Blink the LED near the button
    digitalWrite(pin_LED, HIGH);       
    delay(250);   
    digitalWrite(pin_LED, LOW);       
    delay(250);   
  }

  digitalWrite(pin_LED, LOW);       
  
  /*
  Finally, how about showing how to fade with a loop?  We will stay in this mode until the Arduino is reset
  or turned off.  One can fade by looping through all the intensity values and updating the array each time.
  */ 
  while(1) {
    
    // fade up
    for(int fadeintensity = 0; fadeintensity <= 1023; fadeintensity++) {
      PixelNet.IntensitySetAllIC(Intensity, fadeintensity, fadeintensity, fadeintensity);
      PixelNet.SendPacketIntensity(Intensity);
      // delay(5);              // adjust how fast or slow the fade will occur.  How long to wait inbetween each step
    }
    
    // wait before fading down
    delay(1000);
    
    // fade down
    for(int fadeintensity = 1023; fadeintensity >= 0; fadeintensity--) {
      PixelNet.IntensitySetAllIC(Intensity, fadeintensity, fadeintensity, fadeintensity);
      PixelNet.SendPacketIntensity(Intensity);
      // delay(5);              // adjust how fast or slow the fade will occur.  How long to wait inbetween each step
    }
    
    // wait before repeating
    delay(1000);
    
  } // End of infinite while
} 
// End of loop and the program
