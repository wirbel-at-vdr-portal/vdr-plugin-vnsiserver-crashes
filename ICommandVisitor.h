/*
 * ICommandVisitor.h
 *
 *  Created on: 26.12.2020
 *      Author: mdrechsler
 */

#pragma once

#include "tools.h"

class cRequestPacket;
class SocketError;
class StatusRecording;
class StatusOsdStatusMessage;
class StatusChannelChange;
class StatusChannelsChange;
class StatusRecordingsChange;
class StatusSignalTimerChange;
class StatusEpgChange;


class ICommandVisitor
{
public:
	virtual void
	visit( SocketError& command ) = 0;

	virtual void
	visit( cRequestPacket& command ) = 0;

	virtual void
	visit( StatusRecording& command ) = 0;

	virtual void
	visit( StatusOsdStatusMessage& command ) = 0;

	virtual void
	visit( StatusChannelChange& command ) = 0;

	virtual void
	visit( StatusChannelsChange& command ) = 0;

	virtual void
	visit( StatusRecordingsChange& command ) = 0;

	virtual void
	visit( StatusSignalTimerChange& command ) = 0;

	virtual void
	visit( StatusEpgChange& command ) = 0;

	VNSI_INTERFACE( ICommandVisitor );
};
