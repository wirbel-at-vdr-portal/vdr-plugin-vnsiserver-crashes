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

/*
 * This code is taken from VOMP for VDR plugin.
 */

#include "responsepacket_old.h"
#include "vnsicommand.h"
#include "config.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>

#ifndef __FreeBSD__
#include <asm/byteorder.h>
#else
#include <sys/endian.h>
#define __be64_to_cpu be64toh
#define __cpu_to_be64 htobe64
#endif

/* Packet format for an RR channel response:

4 bytes = channel ID = 1 (request/response channel)
4 bytes = request ID (from serialNumber)
4 bytes = length of the rest of the packet
? bytes = rest of packet. depends on packet
*/

cResponsePacketOld::cResponsePacketOld()
	: bufSize( 512 )
	, buffer( static_cast<uint8_t*>(malloc(bufSize)) )
	, bufUsed( 0 )
{
}

cResponsePacketOld::~cResponsePacketOld()
{
	free(buffer);
	buffer = nullptr;
}

void cResponsePacketOld::init(uint32_t requestID)
{
  bufUsed = 0;

  uint32_t ul;

  ul = htonl(VNSI_CHANNEL_REQUEST_RESPONSE);                     // RR channel
  memcpy(&buffer[0], &ul, sizeof(uint32_t));
  ul = htonl(requestID);
  memcpy(&buffer[4], &ul, sizeof(uint32_t));
  ul = 0;
  memcpy(&buffer[userDataLenPos], &ul, sizeof(uint32_t));

  bufUsed = headerLength;
}

void cResponsePacketOld::initScan(uint32_t opCode)
{
  bufUsed = 0;
  uint32_t ul;

  ul = htonl(VNSI_CHANNEL_SCAN);                     // RR channel
  memcpy(&buffer[0], &ul, sizeof(uint32_t));
  ul = htonl(opCode);
  memcpy(&buffer[4], &ul, sizeof(uint32_t));
  ul = 0;
  memcpy(&buffer[userDataLenPos], &ul, sizeof(uint32_t));

  bufUsed = headerLength;
}

void cResponsePacketOld::initStatus(uint32_t opCode)
{
  bufUsed = 0;
  uint32_t ul;

  ul = htonl(VNSI_CHANNEL_STATUS);                     // RR channel
  memcpy(&buffer[0], &ul, sizeof(uint32_t));
  ul = htonl(opCode);
  memcpy(&buffer[4], &ul, sizeof(uint32_t));
  ul = 0;
  memcpy(&buffer[userDataLenPos], &ul, sizeof(uint32_t));

  bufUsed = headerLength;
}

void cResponsePacketOld::initStream(uint32_t opCode, uint32_t streamID, uint32_t duration, int64_t pts, int64_t dts, uint32_t serial)
{
  bufUsed = 0;
  uint32_t ul;
  uint64_t ull;

  ul =  htonl(VNSI_CHANNEL_STREAM);            // stream channel
  memcpy(&buffer[0], &ul, sizeof(uint32_t));
  ul = htonl(opCode);                          // Stream packet operation code
  memcpy(&buffer[4], &ul, sizeof(uint32_t));
  ul = htonl(streamID);                        // Stream ID
  memcpy(&buffer[8], &ul, sizeof(uint32_t));
  ul = htonl(duration);                        // Duration
  memcpy(&buffer[12], &ul, sizeof(uint32_t));
  ull = __cpu_to_be64(pts);                    // PTS
  memcpy(&buffer[16], &ull, sizeof(uint64_t));
  ull = __cpu_to_be64(dts);                    // DTS
  memcpy(&buffer[24], &ull, sizeof(uint64_t));
  ul = htonl(serial);
  memcpy(&buffer[32], &ul, sizeof(uint32_t));
  ul = 0;
  memcpy(&buffer[userDataLenPosStream], &ul, sizeof(uint32_t));

  bufUsed = headerLengthStream;
}

void cResponsePacketOld::initOsd(uint32_t opCode, int32_t wnd, int32_t color, int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
  bufUsed = 0;
  uint32_t ul;
  int32_t l;

  ul =  htonl(VNSI_CHANNEL_OSD);               // stream OSD
  memcpy(&buffer[0], &ul, sizeof(uint32_t));
  ul = htonl(opCode);                          // OSD operation code
  memcpy(&buffer[4], &ul, sizeof(uint32_t));
  l = htonl(wnd);                              // Window
  memcpy(&buffer[8], &l, sizeof(int32_t));
  l = htonl(color);                            // Color
  memcpy(&buffer[12], &l, sizeof(int32_t));
  l = htonl(x0);                               // x0
  memcpy(&buffer[16], &l, sizeof(int32_t));
  l = htonl(y0);                               // y0
  memcpy(&buffer[20], &l, sizeof(int32_t));
  l = htonl(x1);                               // x1
  memcpy(&buffer[24], &l, sizeof(int32_t));
  l = htonl(y1);                               // y1
  memcpy(&buffer[28], &l, sizeof(int32_t));
  ul = 0;
  memcpy(&buffer[userDataLenPosOSD], &ul, sizeof(uint32_t));

  bufUsed = headerLengthOSD;
}

void cResponsePacketOld::finalise()
{
  uint32_t ul = htonl(bufUsed - headerLength);
  memcpy(&buffer[userDataLenPos], &ul, sizeof(uint32_t));
}

void cResponsePacketOld::finaliseStream()
{
  uint32_t ul = htonl(bufUsed - headerLengthStream);
  memcpy(&buffer[userDataLenPosStream], &ul, sizeof(uint32_t));
}

void cResponsePacketOld::finaliseOSD()
{
  uint32_t ul = htonl(bufUsed - headerLengthOSD);
  memcpy(&buffer[userDataLenPosOSD], &ul, sizeof(uint32_t));
}

void cResponsePacketOld::copyin(const uint8_t* src, uint32_t len)
{
  checkExtend(len);

  memcpy(buffer + bufUsed, src, len);
  bufUsed += len;
}

uint8_t* cResponsePacketOld::reserve(uint32_t len) {
  checkExtend(len);
  uint8_t* result = buffer + bufUsed;
  bufUsed += len;
  return result;
}

bool cResponsePacketOld::unreserve(uint32_t len) {
  if(bufUsed < len) return false;
  bufUsed -= len;
  return true;
}

void cResponsePacketOld::add_String(const char* string)
{
  uint32_t len = strlen(string) + 1;
  checkExtend(len);
  memcpy(buffer + bufUsed, string, len);
  bufUsed += len;
}

void cResponsePacketOld::add_U32(uint32_t ul)
{
  checkExtend(sizeof(uint32_t));
  uint32_t tmp = htonl(ul);
  memcpy(&buffer[bufUsed], &tmp, sizeof(uint32_t));
  bufUsed += sizeof(uint32_t);
}

void cResponsePacketOld::add_U8(uint8_t c)
{
  checkExtend(sizeof(uint8_t));
  buffer[bufUsed] = c;
  bufUsed += sizeof(uint8_t);
}

void cResponsePacketOld::add_S32(int32_t l)
{
  checkExtend(sizeof(int32_t));
  int32_t tmp = htonl(l);
  memcpy(&buffer[bufUsed], &tmp, sizeof(int32_t));
  bufUsed += sizeof(int32_t);
}

void cResponsePacketOld::add_U64(uint64_t ull)
{
  checkExtend(sizeof(uint64_t));
  uint64_t tmp = __cpu_to_be64(ull);
  memcpy(&buffer[bufUsed], &tmp, sizeof(uint64_t));
  bufUsed += sizeof(uint64_t);
}

void cResponsePacketOld::add_double(double d)
{
  checkExtend(sizeof(double));
  uint64_t ull;
  memcpy(&ull, &d, sizeof(double));
  ull = __cpu_to_be64(ull);
  memcpy(&buffer[bufUsed], &ull, sizeof(uint64_t));
  bufUsed += sizeof(uint64_t);
}


void cResponsePacketOld::checkExtend(uint32_t by)
{
  // Check if extending of buffer is actually needed.
  // NOTE: We currently also extend an exactly fitting buffer.
  if ((bufUsed + by) < bufSize)
  {
    return;
  }

  // Extend by at least 512 bytes to reduce performance impact of realloc.
  if (by < 512) by = 512;

  uint8_t* newBuf = (uint8_t*)realloc(buffer, bufSize + by);
  if (!newBuf)
  {
    throw std::bad_alloc();
  }
  buffer = newBuf;
  bufSize += by;
}
