
Q = @

DEFINES  = -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
DEFINES += -DPLUGIN_NAME_I18N='vnsiserver' -DVNSI_SERVER_VERSION='1.8.3'
DEFINES += -DCONSOLEDEBUG


INCLUDES  = -I../../../..
INCLUDES += -I..

LDFLAGS =
LIBS    = -lrepfunc

PKG_CONFIG ?= pkg-config
PKGCFG = $(if $(VDRDIR),$(shell $(PKG_CONFIG) --variable=$(1) $(VDRDIR)/vdr.pc),$(shell PKG_CONFIG_PATH="$$PKG_CONFIG_PATH:../../../.." $(PKG_CONFIG) --variable=$(1) vdr))
export CXXFLAGS = $(call PKGCFG,cxxflags) -std=c++11

%.o: %.c
	@echo CC $@
	$(Q)$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

%.o: %.cpp
	@echo CC $@
	$(Q)$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

OBJS = test-cResponsePacket.o responsepacket_old.o ../responsepacket.o

#MAKEDEP = $(CXX) -MM -MG
#DEPFILE = .dependencies
#$(DEPFILE): Makefile
#	@$(MAKEDEP) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) test-cResponsePacket.cpp > $@
#
#-include $(DEPFILE)

test-cResponsePacket: $(OBJS)
	@echo LD $@
	$(Q)$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

clean:
	$(Q)rm -f $(OBJS) test-cResponsePacket
