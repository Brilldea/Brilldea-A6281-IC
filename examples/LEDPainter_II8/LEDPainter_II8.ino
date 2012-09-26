/*
  Brilldea
  Timothy D. Swieter, P.E.
  Copyright (c) 2011 Timothy D. Swieter, P.E..  All right reserved.
  
  UPDATED: September 18, 2011

  PROGRAM DESCRIPTION:
  This program demonstrates the use of the BrilldeaA6281 IC library for the Arduino.
  
  REFERENCE:
    LED Painter II-8: http://www.brilldea.com/product_LPII-8.html
    A6281 Datasheet:  http://www.allegromicro.com/en/Products/Part_Numbers/6281/6281.pdf
*/

// Import the BrilldeaA6281 IC Library.  The A6281 IC is used on the LED Painter II-8.
#include <BrilldeaA6281.h>

/*
First order of business is to define the way the LED Painter II-8 (LPII-8) is connected to the
Arduino.  There are four data signals and in addition to this there should be power and ground
applied to the LPII-8.   The four data signals are as follows.  They can be attached to any 
digital output pins of the Arduino.  Adjust the four definitions below to match your physical
connections.
*/
int pin_CLOCK    = 13; // LPII-8 Clock Input (CI).  An Arduino output.
int pin_ENABLE   = 12; // LPII-8 Output Enable Input (OEI).  An Arduino output.
int pin_LATCH    = 11; // LPII-8 Latch Input (LI).  An Arduino output.
int pin_DATA     = 10; // LPII-8 Serial Data Input (SDI).  An Arduino output.

/*
Up next you want to define how many LED Painter IIs are connected in the chain.  Or rather, you 
are defining how many A6281 ICs are in the chain.  Each LPII-8 has a quantity of 8 A6281 ICs on it.
In addition to defining the number of ICs, we need to define the arrays for holding the data that 
each IC requires such as the dot correction of each LED and the intensity of each LED.

Adjust the "LPs = x" where x is the number of LPII-8s connected in the chain.
*/
const byte LPs = 1;             // Number of LPII-8 in a chain.  This should NOT exceed 31.
const byte ICs = LPs * 8;       // Automatic calculation of A6281 ICs as each LPII-8 has 8 ICs.  This should NOT exceed 250 ICs
byte DotCorrection[ICs * 3];    // A byte of date for each Dot Correction value for each output on each IC.  ICs * 3, only the lower 7-bits of each element are used. 0 to 127.
int Intensity[ICs * 3];         // A integer for each Intensity value for each output on each IC.  ICs * 3, only the lower 10-bits of each element are used.  0 to 1023.

// Finally we need to establish an instance of the library.  We are calling the instant "LPII8"  This will
// setup the proper pins and IC count.
BrilldeaA6281 LPII8(ICs, pin_CLOCK, pin_ENABLE, pin_LATCH, pin_DATA);

// The setup loop, executed only once before the main loop
void setup() {  

  // Now we are into executing code.  Let's first make sure everything is initialized in each A6281 IC in the LPII-8 chain.
  /*
  First up is the Dot Correction.  Dot Correction is adjustments in the overall current the A6281 IC will output on each output.
  On the A6281 IC, there is an external resistor which sets the maximum current allowed on all three outputs.  The Dot Correction
  value can be applied to scale the output to less than this maximum.  Please review the A6281 IC data sheet to understand this value.  
  The LPII-8 are physically able to putout up to 150mA of current per output.  To have 100% output, the dot correction should be set at 127.
  To have 36.5% output, the Dot Correction can be set at 0.
  After we update the Dot Correction array, we need to then send the data to the ICs.  Usually the Dot Correction is established
  at the system startup and is rarely used after this first setup.
  
  To be safe, the below code is limiting the dot correction so it doesn't over drive any LEDs you have attached.  Be sure to understand 
  Dot Correction so that you can use your LEDs to the fullest capability.
  */
  LPII8.DotCorrectionSetAllIC(DotCorrection, 0, 0, 0);
  LPII8.SendPacketDotCorrection(DotCorrection);  
  
  /*
  Second is the Intensity.  Intensity is the brightness or current for each output.  This is a 10 bit value from 0 (off) to 1023 (brightest).
  The intensity value is the unit the system will change frequently.  It is recommend that you update the array with all the "frame" changes
  and the send the entire packet at once.  Think of it like painting into a "video buffer" and then sending the packet updates to the screen.
  This will make more sense as you review the code in the "loop".
  */
  LPII8.IntensitySetAllIC(Intensity, 0, 0, 0);      
  LPII8.SendPacketIntensity(Intensity); 
  
  // Finally enable the chain of ICs so the outputs are on.
  LPII8.EnableIC(true);
}
// End of setup


// The main loop where all the action happens.
void loop() {
  
  // Once you get to here, the ICs are all initialized and eabled, but off.
  // Lets light them up!
  
  /*
  First we will change color on the entire LPII-8.  We will cycle through all Out0 then all Out1 and then all Out2 with the following code.
  Be sure to send the packet after each array update.
  */
  // Out0
  LPII8.IntensitySetAllIC(Intensity, 1023, 0, 0);
  LPII8.SendPacketIntensity(Intensity);
  delay(2000);
  
  // Out1
  LPII8.IntensitySetAllIC(Intensity, 0, 1023, 0);
  LPII8.SendPacketIntensity(Intensity);
  delay(2000);  
  
  // Out2
  LPII8.IntensitySetAllIC(Intensity, 0, 0, 1023);
  LPII8.SendPacketIntensity(Intensity);
  delay(2000);    
  
  // Everything on
  LPII8.IntensitySetAllIC(Intensity, 1023, 1023, 1023);
  LPII8.SendPacketIntensity(Intensity);
  delay(2000); 
  
  // Everything off
  LPII8.IntensitySetAllIC(Intensity, 0, 0, 0);
  LPII8.SendPacketIntensity(Intensity);
  delay(2000);   
  
  /*
  Next, lets do an example of addressing just one of the groups on the LPII-8.
  Again, be sure to send the packet after each array update.
  */
  
  // Group 4, out0
  LPII8.IntensitySetByIC(Intensity, 4, 1023, 0, 0);
  LPII8.SendPacketIntensity(Intensity);
  delay(2000);   
  
  // Group 1, 6 and 8 at the same time, all outputs at half.  Pay attention here, the Intensity setting function is updating the array.  The
  // Send packet function then sends that array.  This means you can change all the items you want in the array but the LPII-8 won't
  // update until you send the data.  This is the idea of "painting" into a buffer and when the buffer has all it needs, the buffer
  // then updates the screen.
  LPII8.IntensitySetByIC(Intensity, 1, 512, 512, 512);
  LPII8.IntensitySetByIC(Intensity, 6, 512, 512, 512);  
  LPII8.IntensitySetByIC(Intensity, 8, 512, 512, 512);  
  LPII8.SendPacketIntensity(Intensity);
  delay(2000);

  // Everything off
  LPII8.IntensitySetAllIC(Intensity, 0, 0, 0);
  LPII8.SendPacketIntensity(Intensity);
  delay(2000);  

  /*
  Finally, how about showing how to fade with a loop?  We will fade the entire LPII-8 at once.  We will stay in this mode until the Arduino is reset
  or turned off.  One can fade by looping through all the intensity values and updating the array each time.
  */ 
  while(1) {
    
    // fade up
    for(int fadeintensity = 0; fadeintensity <= 1023; fadeintensity++) {
      LPII8.IntensitySetAllIC(Intensity, fadeintensity, fadeintensity, fadeintensity);
      LPII8.SendPacketIntensity(Intensity);
      // delay(5);              // adjust how fast or slow the fade will occur.  How long to wait inbetween each step
    }
    
    // wait before fading down
    delay(1000);
    
    // fade down
    for(int fadeintensity = 1023; fadeintensity >= 0; fadeintensity--) {
      LPII8.IntensitySetAllIC(Intensity, fadeintensity, fadeintensity, fadeintensity);
      LPII8.SendPacketIntensity(Intensity);
      // delay(5);              // adjust how fast or slow the fade will occur.  How long to wait inbetween each step
    }
    
    // wait before repeating
    delay(1000);
    
  } // End of infinite while
} 
// End of loop and the program
