/*
  This file is part of the ELEV-8 Flight Controller Firmware
  for Parallax part #80204, Revision A
  Version 1.0
  
  Copyright 2015 Parallax Incorporated

  ELEV-8 Flight Controller Firmware is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free Software Foundation, 
  either version 3 of the License, or (at your option) any later version.

  ELEV-8 Flight Controller Firmware is distributed in the hope that it will be useful, but 
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the ELEV-8 Flight Controller Firmware.  If not, see <http://www.gnu.org/licenses/>.
  
  Written by Jason Dorie
*/

#include <propeller.h>
#include "rc.h"
#include "pins.h"

static const int Scale = 80/2; // System clock frequency in Mhz, halved - we're converting outputs to 1/2 microsecond resolution

static struct {
  long Pins[8];
  long PinMask;
} data;

static char Cog;

void RC::Start(char UsePPM)
{
  data.Pins[0] = Scale * 2000;     // Throttle is "off"
  for( int i=1; i<8; i++ ) {
    data.Pins[i] = Scale * 3000;   // All other values are centered
  }

  if( UsePPM ) {
    data.PinMask = PIN_RC_0_MASK;   // Elev8-FC pins are defined in pins.h

    use_cog_driver(rc_driver_ppm);
    Cog = load_cog_driver(rc_driver_ppm, &data) + 1;
  }
  else {  
  	// Input pins are P0,1,2,3,4,5,26,27
    data.PinMask = PIN_RC_MASK;     // Elev8-FC pins are defined in pins.h

    #if defined( __PINS_V2_H__ )
    use_cog_driver(rc_driver_v2);
    Cog = load_cog_driver(rc_driver_v2, &data) + 1;
    #endif
  
    #if defined( __PINS_V3_H__ )
    use_cog_driver(rc_driver_v3);
    Cog = load_cog_driver(rc_driver_v3, &data) + 1;
    #endif
  }  
}

void RC::Stop(void)
{
  if(Cog) {
    cogstop(Cog - 1);
    Cog = 0;
  }
}  

//int RC::Get( int _pin ) {
//	// Get receiver servo pulse width in uS
//	return data.Pins[_pin] / Scale;     // Get pulse width from Pins[..] , convert to uSec
//}

int RC::GetRC(int _pin) {
	// Get receiver servo pulse width as normal r/c values (+/-1000) 
	return data.Pins[_pin] / Scale - 3000; // Get pulse width from Pins[..], convert to uSec, make 0 center
}

//int RC::Channel( int _pin ) {
//	return data.Pins[_pin] / Scale;
//}
