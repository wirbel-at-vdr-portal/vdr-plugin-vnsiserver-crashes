/*
 * StatusCommans.c
 *
 *  Created on: 26.12.2020
 *      Author: mdrechsler
 */

#include "StatusCommands.h"

#include "ICommandVisitor.h"

void
SocketError::execute( ICommandVisitor& visitor )
{
	visitor.visit(*this);
}

StatusRecording::StatusRecording( int deviceIndex,
								  const std::string& name,
							  	  const std::string& fileName,
								  bool on )
	: mDeviceIndex( deviceIndex )
	, mName( name )
	, mFileName( fileName )
	, mOn( on )
{
}

int
StatusRecording::getDeviceIndex() const
{
	return mDeviceIndex;
}

const std::string&
StatusRecording::getName() const
{
	return mName;
}

const std::string&
StatusRecording::getFileName() const
{
	return mFileName;
}

bool
StatusRecording::isOn() const
{
	return mOn;
}

void
StatusRecording::execute( ICommandVisitor& visitor )
{
	visitor.visit( *this );
}

StatusOsdStatusMessage::StatusOsdStatusMessage( const std::string& message )
	: mMessage( message )
{
}

std::string
StatusOsdStatusMessage::getMessage() const
{
	return mMessage;
}

void
StatusOsdStatusMessage::execute( ICommandVisitor& visitor )
{
	visitor.visit( *this );
}

StatusChannelChange::StatusChannelChange( const cChannel& channel )
	: mChannel( channel )
{
}

const cChannel&
StatusChannelChange::getChannel() const
{
	return mChannel;
}

void
StatusChannelChange::execute( ICommandVisitor& visitor )
{
	visitor.visit(*this);
}

void
StatusChannelsChange::execute( ICommandVisitor& visitor )
{
	visitor.visit(*this);
}

void
StatusRecordingsChange::execute( ICommandVisitor& visitor )
{
	visitor.visit(*this);
}

void
StatusSignalTimerChange::execute( ICommandVisitor& visitor )
{
	visitor.visit(*this);
}

void
StatusEpgChange::execute( ICommandVisitor& visitor )
{
	visitor.visit(*this);
}
