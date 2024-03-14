/*
 *      vdr-plugin-vnsi - KODI server plugin for VDR
 *
 *      Copyright (C) 2007 Chris Tallon
 *      Copyright (C) 2010 Alwin Esch (Team XBMC)
 *      Copyright (C) 2010, 2011 Alexander Pipelka
 *      Copyright (C) 2015 Team KODI
 *
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "requestpacket_old.h"
#include "vnsicommand.h"
#include "config.h"
//#include "ICommandVisitor.h"

class ICommandVisitor
{
public:
	//virtual void
	//visit( SocketError& command ) = 0;

	virtual void
	visit( cRequestPacketOld& command ) = 0;

	//virtual void
	//visit( StatusRecording& command ) = 0;
        //
	//virtual void
	//visit( StatusOsdStatusMessage& command ) = 0;
        //
	//virtual void
	//visit( StatusChannelChange& command ) = 0;
        //
	//virtual void
	//visit( StatusChannelsChange& command ) = 0;
        //
	//virtual void
	//visit( StatusRecordingsChange& command ) = 0;
        //
	//virtual void
	//visit( StatusSignalTimerChange& command ) = 0;
        //
	//virtual void
	//visit( StatusEpgChange& command ) = 0;
        //
	//VNSI_INTERFACE( ICommandVisitor );
};

#include <stdlib.h>
#include <string.h>

#ifndef __FreeBSD__
#include <asm/byteorder.h>
#else
#include <sys/endian.h>
#define __be64_to_cpu be64toh
#define __cpu_to_be64 htobe64
#endif

cRequestPacketOld::cRequestPacketOld(uint32_t requestID, uint32_t opcode, uint8_t* data, size_t dataLength)
 : userData(data), userDataLength(dataLength), opCode(opcode), requestID(requestID)
{
  packetPos       = 0;
  channelID       = 0;
  streamID        = 0;
  flag            = 0;
}

cRequestPacketOld::~cRequestPacketOld()
{
  delete[] userData;
}

bool cRequestPacketOld::end() const
{
  return (packetPos >= userDataLength);
}

void
cRequestPacketOld::execute( ICommandVisitor& visitor )
{
	visitor.visit( *this );
}

char* cRequestPacketOld::extract_String()
{
  char *p = (char *)&userData[packetPos];
  const char *end = (const char *)memchr(p, '\0', userDataLength - packetPos);
  if (end == NULL)
    /* string is not terminated - fail */
    throw MalformedVNSIPacket();

  int length = end - p;
  packetPos += length + 1;
  return p;
}

uint8_t cRequestPacketOld::extract_U8()
{
  if ((packetPos + sizeof(uint8_t)) > userDataLength)
    throw MalformedVNSIPacket();
  uint8_t uc = userData[packetPos];
  packetPos += sizeof(uint8_t);
  return uc;
}

uint32_t cRequestPacketOld::extract_U32()
{
  if ((packetPos + sizeof(uint32_t)) > userDataLength)
    throw MalformedVNSIPacket();
  uint32_t ul;
  memcpy(&ul, &userData[packetPos], sizeof(uint32_t));
  ul = ntohl(ul);
  packetPos += sizeof(uint32_t);
  return ul;
}

uint64_t cRequestPacketOld::extract_U64()
{
  if ((packetPos + sizeof(uint64_t)) > userDataLength)
    throw MalformedVNSIPacket();
  uint64_t ull;
  memcpy(&ull, &userData[packetPos], sizeof(uint64_t));
  ull = __be64_to_cpu(ull);
  packetPos += sizeof(uint64_t);
  return ull;
}

int64_t cRequestPacketOld::extract_S64()
{
  if ((packetPos + sizeof(int64_t)) > userDataLength)
    throw MalformedVNSIPacket();
  int64_t ll;
  memcpy(&ll, &userData[packetPos], sizeof(int64_t));
  ll = __be64_to_cpu(ll);
  packetPos += sizeof(int64_t);
  return ll;
}

double cRequestPacketOld::extract_Double()
{
  if ((packetPos + sizeof(uint64_t)) > userDataLength)
    throw MalformedVNSIPacket();
  uint64_t ull;
  memcpy(&ull, &userData[packetPos], sizeof(uint64_t));
  ull = __be64_to_cpu(ull);
  double d;
  memcpy(&d, &ull, sizeof(double));
  packetPos += sizeof(uint64_t);
  return d;
}

int32_t cRequestPacketOld::extract_S32()
{
  if ((packetPos + sizeof(int32_t)) > userDataLength)
    throw MalformedVNSIPacket();
  int32_t l;
  memcpy(&l, &userData[packetPos], sizeof(int32_t));
  l = ntohl(l);
  packetPos += sizeof(int32_t);
  return l;
}

//uint8_t* cRequestPacketOld::getData()
//{
//  return userData;
//}
