/*
 * ISocket.h
 *
 *  Created on: 26.12.2020
 *      Author: mdrechsler
 */

#pragma once

#include "tools.h"

class ISocket
{
public:
	virtual void
	Shutdown() = 0;

	virtual void
	lock() = 0;

	virtual void
	unlock() = 0;

	virtual void
	Invalidate() = 0;

	virtual int
	GetHandle() const = 0;

	virtual ssize_t
	read( void* buffer,
		  size_t size,
		  int timeout_ms = -1 ) = 0;

	virtual ssize_t
	write( const void* buffer,
		   size_t size,
		   int timeout_ms = -1,
		   bool more_data = false ) = 0;

	VNSI_INTERFACE( ISocket );
};
