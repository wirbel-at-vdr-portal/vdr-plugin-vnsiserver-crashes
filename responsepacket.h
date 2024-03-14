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
#pragma once
#include <string>
#include <vector>
#include <cstdint>

class cResponsePacket {
private:
  std::vector<uint8_t> data;
  uint32_t command;
  size_t extra;
  bool finalized;
  void finalize(void);
  cResponsePacket() {}
  template <typename T> void Add(T t, int Pos = -1);
public:
  // for generic response
  cResponsePacket(uint32_t Command, uint32_t OpCode);

  // for VNSI_CHANNEL_STREAM
  cResponsePacket(uint32_t OpCode, uint32_t StreamID, uint32_t Duration,
                  int64_t PTS, int64_t DTS, uint32_t Serial);

  // for VNSI_CHANNEL_OSD
  cResponsePacket(uint32_t OpCode, int32_t Wnd, int32_t Color,
                  int32_t X0, int32_t Y0, int32_t X1, int32_t Y1);

  ~cResponsePacket() {}

  void add_U8(uint8_t c, int pos = -1);
  void add_U32(uint32_t ul, int pos = -1);
  void add_U64(uint64_t ull, int pos = -1);
  void add_S32(int32_t l, int pos = -1);
  void add_double(double d, int pos = -1);
  void add_String(std::string str, int pos = -1);
  void add_Buffer(char* p, size_t len, int pos = -1);

  const uint8_t* getPtr(void);
  size_t getLen(void);
  void ReportExtraData(size_t Extra);
};
