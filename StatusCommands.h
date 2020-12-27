/*
 * StatusCommands.h
 *
 *  Created on: 26.12.2020
 *      Author: mdrechsler
 */

#pragma once

#include <string>

#include <vdr/channels.h>

#include "ICommand.h"

class SocketError : public ICommand
{
public:
	/**************************************************************************
	 * ICommand virtual overrides
	 *************************************************************************/
	virtual void
	execute( ICommandVisitor& visitor );
};

class StatusRecording : public ICommand
{
public:
	StatusRecording( int deviceIndex,
					 const std::string& name,
					 const std::string& fileName,
					 bool on );

	int
	getDeviceIndex() const;

	const std::string&
	getName() const;

	const std::string&
	getFileName() const;

	bool
	isOn() const;

	/**************************************************************************
	 * ICommand virtual overrides
	 *************************************************************************/
	virtual void
	execute( ICommandVisitor& visitor );

private:
	int 		mDeviceIndex;
	std::string mName;
	std::string mFileName;
	bool		mOn;
};

class StatusOsdStatusMessage : public ICommand
{
public:
	StatusOsdStatusMessage( const std::string& message );

	std::string
	getMessage() const;

	/**************************************************************************
     * ICommand virtual overrides
	 *************************************************************************/
	virtual void
	execute( ICommandVisitor& visitor );

private:
	std::string mMessage;
};

class StatusChannelChange : public ICommand
{
public:
	StatusChannelChange( const cChannel& channel );

	const cChannel&
	getChannel() const;

	/**************************************************************************
	 * ICommand virtual overrides
	 *************************************************************************/
	virtual void
	execute( ICommandVisitor& visitor );
private:
	cChannel mChannel;
};

class StatusChannelsChange : public ICommand
{
public:
	/**************************************************************************
     * ICommand virtual overrides
	 *************************************************************************/
	virtual void
	execute( ICommandVisitor& visitor );
};

class StatusRecordingsChange : public ICommand
{
public:
	/**************************************************************************
     * ICommand virtual overrides
	 *************************************************************************/
	virtual void
	execute( ICommandVisitor& visitor );
};

class StatusSignalTimerChange : public ICommand
{
public:
	/**************************************************************************
     * ICommand virtual overrides
	 *************************************************************************/
	virtual void
	execute( ICommandVisitor& visitor );
};

class StatusEpgChange : public ICommand
{
public:
	/**************************************************************************
     * ICommand virtual overrides
	 *************************************************************************/
	virtual void
	execute( ICommandVisitor& visitor );
};
