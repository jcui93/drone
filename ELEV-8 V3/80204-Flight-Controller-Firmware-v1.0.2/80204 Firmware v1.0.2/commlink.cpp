/*
  This file is part of the ELEV-8 Flight Controller Firmware
  for Parallax part #80204, Revision A
  
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

#include "commlink.h"


u16 COMMLINK::cachedLength;
u16 COMMLINK::packetChecksum;
u8  COMMLINK::packetBuf[64];
u8  COMMLINK::bufIndex;

u16 Checksum(u16 checksum, u16 * buf , int len );


void COMMLINK::StartPacket( char port, u8 type , u16 length )
{
  cachedLength = length + 8;    // 2 byte signature, 2 byte type, 2 byte length, 2 byte checksum

  u16 buf[3];
  buf[0] = 0xAA55;
  buf[1] = type;
  buf[2] = cachedLength;

  packetChecksum = Checksum( 0, buf, 3 );  // 6 bytes = 3 u16's, and the checksum is done on u16's for speed
  S4_Put_Bytes( port, buf, 6 );
}

void COMMLINK::AddPacketData( char port, void * data , u16 Count )
{
  packetChecksum = Checksum( packetChecksum, (u16*)data, Count>>1 );
  S4_Put_Bytes( port, data, Count );
}


void COMMLINK::StartPacket( u8 type , u16 length )
{
  cachedLength = length + 8;    // 2 byte signature, 1 byte type, 2 byte length, 2 byte crc

  ((u16*)packetBuf)[0] = 0xAA55;  // 55AA signature when done in little-endian
  ((u16*)packetBuf)[1] = type;
  ((u16*)packetBuf)[2] = cachedLength;
  bufIndex = 6; // 3 x U16s = 6 bytes
}

void COMMLINK::AddPacketData( void * data , u16 Count )
{
  memcpy( packetBuf + bufIndex , data, Count );
  bufIndex += Count;
}

void COMMLINK::EndPacket(void)
{
  packetChecksum = Checksum( 0, (u16*)packetBuf, bufIndex>>1 );
  memcpy( packetBuf + bufIndex, &packetChecksum, 2 );
  bufIndex += 2;
}

void COMMLINK::BuildPacket( u8 type , void * data , u16 length )
{
  StartPacket( type , length );
  AddPacketData( data , length );
  EndPacket();
}



u16 Checksum(u16 checksum, u16 * buf , int len )
{
  for( int i=0; i<len; i++) {
    checksum = ((checksum << 5) | (checksum >> (16-5))) ^ buf[i];
  }
  return checksum;
}
