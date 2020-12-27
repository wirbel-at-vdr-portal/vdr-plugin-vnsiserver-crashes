/*
 * ICommandProcessor.h
 *
 *  Created on: 26.12.2020
 *      Author: mdrechsler
 */

#pragma once

#include <chrono>

#include "tools.h"

class ICommand;

class ICommandProcessor
{
public:
	/**
	 * Process a command
	 *
	 * @param [in]	command		The command to process
	 */
	virtual bool
	process( const ICommand& command,
			 const std::chrono::milliseconds& timeout ) = 0;


	VNSI_INTERFACE( ICommandProcessor );
};
