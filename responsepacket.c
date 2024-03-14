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

#include "responsepacket.h"
#include "vnsicommand.h"
#include "config.h"

#include <iostream>
#include <cstdlib>
#include <arpa/inet.h>

#ifndef __FreeBSD__
#include <asm/byteorder.h>
#else
#include <sys/endian.h>
#define __be64_to_cpu be64toh
#define __cpu_to_be64 htobe64
#endif


/*******************************************************************************
 * cResponsepacket
 ******************************************************************************/
cResponsePacket::cResponsePacket(uint32_t Command, uint32_t OpCode)
 : command(Command), extra(0), finalized(false)
{
  data.reserve(12);
  add_U32(Command);
  add_U32(OpCode);
  add_U32(0);        // # of bytes follow
}

// for VNSI_CHANNEL_STREAM
cResponsePacket::cResponsePacket(uint32_t OpCode, uint32_t StreamID, uint32_t Duration,
                                 int64_t PTS, int64_t DTS, uint32_t Serial) 
 : command(VNSI_CHANNEL_STREAM), extra(0), finalized(false)
{
  data.reserve(40);
  add_U32(command);
  add_U32(OpCode);
  add_U32(StreamID); // Stream ID
  add_U32(Duration); // Duration
  add_U64(PTS);      // PTS
  add_U64(DTS);      // DTS
  add_U32(Serial);   // Serial
  add_U32(0);        // # of bytes follow
}

// for VNSI_CHANNEL_OSD
cResponsePacket::cResponsePacket(uint32_t OpCode, int32_t Wnd, int32_t Color,
                                 int32_t X0, int32_t Y0, int32_t X1, int32_t Y1)
 : command(VNSI_CHANNEL_OSD), extra(0), finalized(false)
{
  data.reserve(36);
  add_U32(command);
  add_U32(OpCode);
  add_S32(Wnd);      // Window
  add_S32(Color);    // Color
  add_S32(X0);       // X0
  add_S32(Y0);       // Y0
  add_S32(X1);       // X1
  add_S32(Y1);       // Y1
  add_U32(0);        // # of bytes follow
}


void cResponsePacket::add_Buffer(char* p, size_t len, int pos) {
  const uint8_t* P = reinterpret_cast<const uint8_t*>(p);
  if (pos < 0) {
     for(size_t i=0; i<len; i++)
        data.push_back(*P++);
     }
  else {
     auto it = data.begin() + pos;
     for(size_t i=0; i<len; i++)
        *it++ = *P++;
     }
}

void cResponsePacket::add_String(std::string str, int pos) {
  char* p = (char*) str.data();
  add_Buffer(p, str.size() + 1, pos);
}

template <typename T>
void cResponsePacket::Add(T t, int pos) {
  uint8_t* p = reinterpret_cast<uint8_t*>(&t);
  if (pos < 0) {
     for(size_t i=0; i<sizeof(T); i++)
        data.push_back(*p++);
     }
  else {
     auto it = data.begin() + pos;
     for(size_t i=0; i<sizeof(T); i++)
        *it++ = *p++;
     }
}

template void cResponsePacket::Add<uint8_t>(uint8_t,int);
template void cResponsePacket::Add<uint32_t>(uint32_t,int);
template void cResponsePacket::Add<uint64_t>(uint64_t,int);
template void cResponsePacket::Add<int32_t>(int32_t,int);


void cResponsePacket::add_U8(uint8_t c, int pos) {
  Add(c, pos);
}

void cResponsePacket::add_U32(uint32_t ul, int pos) {
  uint32_t UL = htonl(ul);
  Add(UL, pos);
}

void cResponsePacket::add_U64(uint64_t ull, int pos) {
  uint64_t ULL = htobe64(ull);
  Add(ULL, pos);
}

void cResponsePacket::add_S32(int32_t l, int pos) {
  int32_t I32 = htonl(l);
  Add(I32, pos);
}

void cResponsePacket::add_double(double d, int pos) {
  uint64_t* ULL = reinterpret_cast<uint64_t*>(&d);
  *ULL = htobe64(*ULL);
  Add(*ULL, pos);
}

void cResponsePacket::finalize(void) {
  if (command == VNSI_CHANNEL_STREAM)
     add_U32(data.size() - 40 + extra, 36);
  else if (command == VNSI_CHANNEL_OSD)
     add_U32(data.size() - 36 + extra, 32);
  else
     add_U32(data.size() - 12 + extra, 8);
}

const uint8_t* cResponsePacket::getPtr(void) {
  if (not finalized) {
     finalized = true;
     finalize();
     }
  return data.data();
}

size_t cResponsePacket::getLen(void) {
  if (not finalized) {
     finalized = true;
     finalize();
     }
  return data.size();
}

// used, if this packet is immediatley followed by extra data.
void cResponsePacket::ReportExtraData(size_t Extra) {
  extra = Extra;
}
