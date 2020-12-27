/*
 * tools.h
 *
 *  Created on: 26.12.2020
 *      Author: mdrechsler
 */

#pragma once

#include <memory>

#define VNSI_DECLARE_POINTER( ClassName ) \
	class ClassName; \
	using ClassName ## SharedPtr = std::shared_ptr<ClassName>; \
	using ClassName ## UniquePtr = std::unique_ptr<ClassName>; \


#define VNSI_INTERFACE( ClassName ) \
	public: \
		virtual ~ClassName() = default; \
	protected: \
	    ClassName() = default; \
	private: \
		ClassName( const ClassName& other ) = delete; \
		\
		ClassName& \
		operator=( const ClassName& other ) = delete;

