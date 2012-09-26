/*
  Brilldea
  Timothy D. Swieter, P.E.
  Copyright (c) 2011 - 2012 Timothy D. Swieter, P.E..  All right reserved.
  
  UPDATED: January 28, 2012

  FILE:  BrilldeaA6281.h
  FILE DESCRIPTION:  A library for the Allegro Microsystems A6281 IC
  FILE VERSION: 0.3
  
  VERSION HISTORY:
	0.1 - initial release
	0.2 - changed 'int' to 'byte' on several items to make the code more effecient.
	0.3 - made the code possible for Arduino 1.0
  
  REFERENCE:
    A6281 Datasheet:  http://www.allegromicro.com/en/Products/Part_Numbers/6281/6281.pdf
*/

#ifndef BrilldeaA6281_h
#define BrilldeaA6281_h

// include types & constants of Wiring core API
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


class BrilldeaA6281

{
  public:
    BrilldeaA6281(byte ICs, byte clock, byte enable, byte latch, byte data);
    
    void DotCorrectionSetbyIC(byte *DCarray, byte IC, byte out0, byte out1, byte out2);
    void DotCorrectionSetAllIC(byte *DCarray, byte out0, byte out1, byte out2);
    void SendPacketDotCorrection(byte *DCarray);

    void IntensitySetByIC(int *Iarray, byte IC, int out0, int out1, int out2);
    void IntensitySetAllIC(int *Iarray, int out0, int out1, int out2);
    void SendPacketIntensity(int *Iarray);

    void EnableIC(boolean enable);

  private:
    void SendPacket(unsigned long tPacket);
    void SendLatch();

    byte _numIC;     	//The number of A6281 ICs in the chain.

    byte _pin_CLOCK;   	// A6281 Clock Input (CI).  An Arduino output.
    byte _pin_ENABLE;  	// A6281 Output Enable Input (OEI).  An Arduino output.
    byte _pin_LATCH;   	// A6281 Latch Input (LI).  An Arduino output.
    byte _pin_DATA;    	// A6281 Serial Data Input (SDI).  An Arduino output.

    unsigned long Packet;    
};
		

#endif