/*
 * vnsisocket.c
 *
 *  Created on: 26.12.2020
 *      Author: mdrechsler
 */

#include "config.h"
#include "vnsicommand.h"
#include "vnsisocket.h"
#include "requestpacket.h"
#include "ICommandQueue.h"
#include "StatusCommands.h"

#include <memory>

#include <arpa/inet.h>

VNSISocket::VNSISocket(int fd,
					   ICommandQueue& queue)
	: m_socket(fd)
	, m_queue( queue )
{
	SetDescription("VNSI Socket %d", fd);

	Start();
}

VNSISocket::~VNSISocket()
{
	m_socket.Shutdown();
	Cancel(10);
}

void
VNSISocket::Shutdown()
{
	m_socket.Shutdown();
}

void
VNSISocket::lock()
{
	m_socket.lock();
}

void
VNSISocket::unlock()
{
	m_socket.unlock();
}

void
VNSISocket::Invalidate()
{
	Cancel( -1 );
	m_socket.Invalidate();
}

int
VNSISocket::GetHandle() const
{
	return m_socket.GetHandle();
}

ssize_t
VNSISocket::read( void* buffer,
				  size_t size,
				  int timeout_ms )
{
	return m_socket.read( buffer, size, timeout_ms );
}

ssize_t
VNSISocket::write( const void* buffer,
				   size_t size,
				   int timeout_ms,
				   bool more_data )
{
	return m_socket.write(buffer, size, timeout_ms, more_data);
}

void
VNSISocket::Action()
{
	struct Header
	{
		uint32_t channelID;
		uint32_t requestID;
		uint32_t opcode;
		uint32_t dataLength;
	};

	try
	{
		while (Running())
		{
			// Read the header. It is fixed site and contains the remaining
			// bytes to read.
			Header header;
			if ( sizeof(header) != m_socket.read(&header, sizeof(header)) )
			{
				throw std::runtime_error( "Failed to read header" );
			}

			// Header values are given in network byte order
			header.channelID = ntohl(header.channelID);
			header.requestID = ntohl(header.requestID);
			header.opcode = ntohl(header.opcode);
			header.dataLength = ntohl(header.dataLength);

			// This was meant to multiplex multiple streams over a single connection.
			// For now ignore every channel not equal 1
			if (header.channelID != 1)
			{
				throw std::runtime_error("Incoming channel number unknown");
			}

			// a random sanity limit
			if (header.dataLength > 200000)
			{
				throw std::runtime_error("dataLength > 200000!");
			}

			std::unique_ptr<uint8_t[]> data;
			if (header.dataLength)
			{
				data.reset( new uint8_t[header.dataLength] );
				if (header.dataLength != m_socket.read(data.get(), header.dataLength, 10000))
				{
					throw std::runtime_error("Could not read data");
				}
			}

			DEBUGLOG("Received chan=%u, ser=%u, op=%u, edl=%u", header.channelID,
					 header.requestID, header.opcode, header.dataLength);

			auto command = std::make_shared<cRequestPacket>(
			    header.requestID, header.opcode, data.release(), header.dataLength
		        );
			m_queue.enqueue( command );
		}
	}

	catch ( const std::exception& error )
	{
		ERRORLOG( "Socket error: '%s'. Dropping connection", error.what() );
		m_queue.enqueue( std::make_shared<SocketError>() );
	}
	catch ( ... )
	{
		ERRORLOG( "Socket error: 'Undetermined exception'. Dropping connection" );
		m_queue.enqueue( std::make_shared<SocketError>() );
	}
}
