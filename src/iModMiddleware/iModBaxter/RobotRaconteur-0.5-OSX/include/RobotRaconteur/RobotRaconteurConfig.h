//  Robot Raconteur(R) - A communication library for robotics and automation systems
//  Copyright (C) 2014 John Wason <wason@wasontech.com>
//                     Wason Technology, LLC
//
//  This program is released under the terms of the Robot Raconteur(R)
//  license.  Full text can be found at  http://robotraconteur.com/license2 .
//  Attribute to this library as "Robot Raconteur(R)" in documentation or 
//  packaging.  This software is royalty free for commercial use under
//  the conditions of the license.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

#pragma once

#ifndef ROBOTRACONTEUR_VERSION
#define ROBOTRACONTEUR_VERSION "0.5.0"
#endif

#ifndef RR_STD_SHARED_PTR
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>

#define RR_SHARED_PTR boost::shared_ptr
#define RR_MAKE_SHARED boost::make_shared
#define RR_WEAK_PTR boost::weak_ptr
#define RR_ENABLE_SHARED_FROM_THIS boost::enable_shared_from_this
#define RR_DYNAMIC_POINTER_CAST boost::dynamic_pointer_cast
#define RR_STATIC_POINTER_CAST boost::static_pointer_cast

#else

#include <memory>

#define RR_SHARED_PTR std::shared_ptr
#define RR_MAKE_SHARED std::make_shared
#define RR_WEAK_PTR std::weak_ptr
#define RR_ENABLE_SHARED_FROM_THIS std::enable_shared_from_this
#define RR_DYNAMIC_POINTER_CAST std::dynamic_pointer_cast
#define RR_STATIC_POINTER_CAST std::static_pointer_cast

#endif

#define RR_UNORDERED_MAP boost::unordered_map

#include <boost/thread.hpp>
#include <boost/regex.hpp>

#ifdef BOOST_WINDOWS
#define ROBOTRACONTEUR_PATHSEP "\\"
#else
#define ROBOTRACONTEUR_PATHSEP "/"
#endif