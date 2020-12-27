/*
 * vnsisocket.h
 *
 *  Created on: 26.12.2020
 *      Author: mdrechsler
 */

#pragma once

#include "cxsocket.h"

class ICommandQueue;

class VNSISocket
		: public cThread
		, public ISocket
{
public:
	VNSISocket( int fd,
			    ICommandQueue& queue );

	~VNSISocket() override;

	/**************************************************************************
     * ISocket virtual overrides
	 *************************************************************************/
	void
	Shutdown() override;

	void
	lock() override;

	void
	unlock() override;

	virtual void
	Invalidate() override;

	virtual int
	GetHandle() const override;

	ssize_t
	read( void* buffer,
		  size_t size,
		  int timeout_ms = -1 ) override;

	ssize_t
	write( const void* buffer,
		   size_t size,
		   int timeout_ms = -1,
		   bool more_data = false) override;

	/**************************************************************************
	 * cThread virtual overrides
	 *************************************************************************/
	virtual void
	Action() override;

private:
	cxSocket 		m_socket;
	ICommandQueue& 	m_queue;
};




