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

#ifndef VNSI_RESPONSEPACKET_H
#define VNSI_RESPONSEPACKET_H

#include <stdint.h>

class cResponsePacketOld
{
public:
  cResponsePacketOld();
  ~cResponsePacketOld();

  void init(uint32_t requestID);
  void initScan(uint32_t opCode);
  void initStatus(uint32_t opCode);
  void initStream(uint32_t opCode, uint32_t streamID, uint32_t duration, int64_t pts, int64_t dts, uint32_t serial);
  void initOsd(uint32_t opCode, int32_t wnd, int32_t color, int32_t x0, int32_t y0, int32_t x1, int32_t y1);
  void finalise();
  void finaliseStream();
  void finaliseOSD();
  void copyin(const uint8_t* src, uint32_t len);
  uint8_t* reserve(uint32_t len);
  bool unreserve(uint32_t len);

  void add_String(const char* string);
  void add_U32(uint32_t ul);
  void add_S32(int32_t l);
  void add_U8(uint8_t c);
  void add_U64(uint64_t ull);
  void add_double(double d);

  uint8_t* getPtr() { return buffer; }
  uint32_t getLen() { return bufUsed; }
  uint32_t getStreamHeaderLength() { return headerLengthStream; } ;
  uint32_t getOSDHeaderLength() { return headerLengthOSD; } ;
  void     setLen(uint32_t len) { bufUsed = len; }

private:
  uint32_t bufSize;
  uint8_t* buffer;
  uint32_t bufUsed;

  void checkExtend(uint32_t by);

  const static uint32_t headerLength          = 12;
  const static uint32_t userDataLenPos        = 8;
  const static uint32_t headerLengthStream    = 40;
  const static uint32_t userDataLenPosStream  = 36;
  const static uint32_t headerLengthOSD       = 36;
  const static uint32_t userDataLenPosOSD     = 32;
};

#endif // VNSI_RESPONSEPACKET_H

