#include <iostream>
#include <cstdlib>
#include <repfunc.h>
#include "vnsicommand.h"
#include "responsepacket.h"
#include "responsepacket_old.h"


int main(void) {
  uint32_t requestID;

  std::srand(std::time(nullptr));
  requestID = std::rand();
  /*std::array<char, 16> buffer {{
     'T', 'h', 'i', 's',
     ' ',
     'i', 's',
     ' ',
     'a',
     ' ',
     'B', 'u', 'f', 'f', 'e', 'r'}};*/ 

  {
  cRunTime t;
  t.Start();
  cResponsePacketOld r;
  r.init(requestID);
  r.add_String("hello world");
  r.add_U32(((uint32_t)'U' << 16) | ('3' << 8) | '2');
  r.add_S32(((uint32_t)'S' << 16) | ('3' << 8) | '2');
  r.add_U8 ( 'U' |  '8');
  r.add_U64(((uint32_t)'U' << 16) | ('6' << 8) | '4');
  r.add_double(1234567890.12);
  r.finalise();
  const unsigned char* p = (const unsigned char*) r.getPtr();
  size_t n = r.getLen();
  t.Stop();
  std::string s = "old, time = " + IntToStr(t.MicroSeconds()) + " usec";
  HexDump(s.c_str(), p, n);
  }

  {
  cRunTime t;
  t.Start();
  cResponsePacket r(VNSI_CHANNEL_REQUEST_RESPONSE, requestID);
  r.add_String("hello world");
  r.add_U32(((uint32_t)'U' << 16) | ('3' << 8) | '2');
  r.add_S32(((uint32_t)'S' << 16) | ('3' << 8) | '2');
  r.add_U8 ( 'U' |  '8');
  r.add_U64(((uint32_t)'U' << 16) | ('6' << 8) | '4');
  r.add_double(1234567890.12);
  const unsigned char* p = (const unsigned char*) r.getPtr();
  size_t n = r.getLen();
  t.Stop();
  std::string s = "new, time = " + IntToStr(t.MicroSeconds()) + " usec";
  HexDump(s.c_str(), p, n);
  }



  return 0;
}
