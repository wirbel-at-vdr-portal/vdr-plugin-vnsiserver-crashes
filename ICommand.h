/*
 * ICommand.h
 *
 *  Created on: 26.12.2020
 *      Author: mdrechsler
 */

#pragma once

#include "tools.h"

class ICommandVisitor;

class ICommand
{
public:
	/**
	 * Execute a command
	 */
	virtual void
	execute( ICommandVisitor& ) = 0;


	VNSI_INTERFACE( ICommand );
};
