/*
 * CommandQueue.h
 *
 *  Created on: 26.12.2020
 *      Author: mdrechsler
 */
#include <chrono>

#include "tools.h"

#pragma once

VNSI_DECLARE_POINTER( ICommand );

class ICommandQueue
{
public:
	virtual void
	enqueue( const ICommandSharedPtr& command ) = 0;


	VNSI_INTERFACE( ICommandQueue );
};
