/*
  Brilldea
  Timothy D. Swieter, P.E.
  Copyright (c) 2011 - 2012 Timothy D. Swieter, P.E..  All right reserved.
  
  UPDATED: January 28, 2012

  FILE:  BrilldeaA6281.cpp
  FILE DESCRIPTION:  A library for the Allegro Microsystems A6281 IC
  FILE VERSION: 0.3
  
  VERSION HISTORY:
	0.1 - initial release
	0.2 - changed 'int' to 'byte' on several items to make the code more effecient.
        - added some masking and tests to verify passed data.  Clarified comments.
	0.3 - made the code possible for Arduino 1.0	
		
  REFERENCE:
    A6281 Datasheet:  http://www.allegromicro.com/en/Products/Part_Numbers/6281/6281.pdf
*/

#include "BrilldeaA6281.h"

BrilldeaA6281::BrilldeaA6281(byte ICs, byte clock, byte enable, byte latch, byte data)
  // Establish all the IO pins and directions, set the initial state of pins and variables.
{
  // Ensure the numbers of ICs is a valid number.
  if (_numIC <= 250)
  {
	_numIC = ICs;       // The number of A6281 ICs in the chain.	  
  }
  else
  {
	_numIC = 250;    	// The number of A6281 ICs in the chain.	    
  }

  _pin_CLOCK = clock;   // A6281 Clock Input (CI).  An Arduino output.
  _pin_ENABLE = enable; // A6281 Output Enable Input (OEI).  An Arduino output.
  _pin_LATCH = latch;   // A6281 Latch Input (LI).  An Arduino output.
  _pin_DATA = data;     // A6281 Serial Data Input (SDI).  An Arduino output.

  // Initialize the digital pin's I/O direction appropriately.
  pinMode(_pin_CLOCK, OUTPUT);     
  pinMode(_pin_ENABLE, OUTPUT);     
  pinMode(_pin_LATCH, OUTPUT);       
  pinMode(_pin_DATA, OUTPUT);  

  // Initialize the pin states
  digitalWrite(_pin_LATCH, LOW);    // Set latch low
  EnableIC(false);		    // Disable the A6281 IC Chain
}

void BrilldeaA6281::DotCorrectionSetbyIC(byte *DCarray, byte IC, byte out0, byte out1, byte out2) {
 // Populate the dot correction array for only the IC specified
 // Note IC is a 1 base number (start counting at 1)
 // Note out0, out1, and out2 only uses the least significant 7-bits.  0 to 127.

 if ((IC >= 1) & (IC <= _numIC))
 {
   DCarray[((IC * 3) - 3)] = (0x7F & out0);
   DCarray[((IC * 3) - 2)] = (0x7F & out1);
   DCarray[((IC * 3) - 1)] = (0x7F & out2);  
 }
}

void BrilldeaA6281::DotCorrectionSetAllIC(byte *DCarray, byte out0, byte out1, byte out2) {
 // Updates the dot correct array with the appropriate values for use in sending to A6281 IC.  
 // Note out0, out1, and out2 only uses the least significant 7-bits. 0 to 127.

  for(byte IC = _numIC; IC > 0; IC--) {
   DCarray[((IC * 3) - 3)] = (0x7F & out0);
   DCarray[((IC * 3) - 2)] = (0x7F & out1);
   DCarray[((IC * 3) - 1)] = (0x7F & out2);  
  }
}

void BrilldeaA6281::SendPacketDotCorrection(byte *DCarray) {
  // Dot correction packet building and sending out for the entire array
  // We need to loop through the entire array of ICs sending the last IC
  // IC of the chain first and the working towards the first IC.
  for(byte IC = _numIC; IC > 0; IC--) {
    Packet = B01;                                                   // Address "1"
    Packet = (Packet << 3)  | B000;                                 // ATBs and don't care
    Packet = (Packet << 7)  | (0x7F & DCarray[((IC * 3) - 1)]);  	// Dot Correction Register 2
    Packet = (Packet << 3)  | B000;                                 // Don't cares
    Packet = (Packet << 7)  | (0x7F & DCarray[((IC * 3) - 2)]);  	// Dot Correction Register 1
    Packet = (Packet << 1)  | B0;                                   // Don't cares
    Packet = (Packet << 2)  | B00;                                  // Clock Mode
    Packet = (Packet << 7)  | (0x7F & DCarray[((IC * 3) - 3)]);  	// Dot Correction Register 0

	// This is sending enough data for one IC, that is why we loop.
    SendPacket(Packet);
  }

  // Always latch in the data after sending an entire chain of data
  SendLatch();
}

void BrilldeaA6281::IntensitySetByIC(int *Iarray, byte IC, int out0, int out1, int out2) {
 // Populate the intensity array for only the IC specified
 // Note IC is a 1 base number (start counting at 1)
 // Note out0, out1, and out2 only uses the least significant 10-bits.  0 to 1023.

 if ((IC >= 1) & (IC <= _numIC))
 {
   Iarray[((IC * 3) - 3)] = (0x3FF & out0);
   Iarray[((IC * 3) - 2)] = (0x3FF & out1);
   Iarray[((IC * 3) - 1)] = (0x3FF & out2);  
 }
}

void BrilldeaA6281::IntensitySetAllIC(int *Iarray, int out0, int out1, int out2) {
 // Updates the intensity array with the appropriate values for use in sending to A6281 IC.  
 // Note out0, out1, and out2 only uses the least significant 10-bits.  0 to 1023.

  for(byte IC = _numIC; IC > 0; IC--) {
   Iarray[((IC * 3) - 3)] = (0x3FF & out0);
   Iarray[((IC * 3) - 2)] = (0x3FF & out1);
   Iarray[((IC * 3) - 1)] = (0x3FF & out2);  
  }
}

void BrilldeaA6281::SendPacketIntensity(int *Iarray) {
  // Intensity packet building and sending out for the entire array
  // We need to loop through the entire array of ICs sending the last IC
  // IC of the chain first and the working towards the first IC.  
  for(byte IC = _numIC; IC > 0; IC--) {
    Packet = B00;                                                   //Address "0"
    Packet = (Packet << 10) | (0x3FF & Iarray[((IC * 3) - 1)]);  	//PWM Counter 2/out2
    Packet = (Packet << 10) | (0x3FF & Iarray[((IC * 3) - 2)]);  	//PWM Counter 1/out1
    Packet = (Packet << 10) | (0x3FF & Iarray[((IC * 3) - 3)]);  	//PWM Counter 0/Out0

	// This is sending enough data for one IC, that is why we loop.	
    SendPacket(Packet);
  }
  
  // Always latch in the data after sending an entire chain of data
  SendLatch();
}

void BrilldeaA6281::SendPacket(unsigned long tPacket) {
  // One A6291 IC worth of data is 32-bits
  // Shift out 32-bits of data (8-bits for each ShiftOut)
  shiftOut(_pin_DATA, _pin_CLOCK, MSBFIRST, tPacket >> 24);
  shiftOut(_pin_DATA, _pin_CLOCK, MSBFIRST, tPacket >> 16);
  shiftOut(_pin_DATA, _pin_CLOCK, MSBFIRST, tPacket >> 8);
  shiftOut(_pin_DATA, _pin_CLOCK, MSBFIRST, tPacket );        
}

void BrilldeaA6281::SendLatch() {
  // Latch in the data after all of it is shifted out
  delayMicroseconds(2);              
  digitalWrite(_pin_LATCH, HIGH);
  delayMicroseconds(2);              
  digitalWrite(_pin_LATCH, LOW);
}

void BrilldeaA6281::EnableIC(boolean enable) {
  // Enable or disable the A6281 IC
  // When enabled (true), the A6281 (and connected LED) will display the intensity/dot correction
  // When disabled (false), the A6281 (and connected LED) will be off

  if (enable)
  {
    digitalWrite(_pin_ENABLE, LOW);  //Enable the IC
  }
  else
  {
    digitalWrite(_pin_ENABLE, HIGH);  //Disable the IC
  }
}


