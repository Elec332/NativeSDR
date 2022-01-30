/* -*- C -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_VOLK_VERSION_H
#define INCLUDED_VOLK_VERSION_H

#include <volk/volk_common.h>

__VOLK_DECL_BEGIN

/*
 * define macros for the Volk version, which can then be used by any
 * project that #include's this header, e.g., to determine whether
 * some specific API is present and functional.
 */

#define VOLK_VERSION_MAJOR 02
#define VOLK_VERSION_MINOR 05
#define VOLK_VERSION_MAINT 00

/*
 * VOLK_VERSION % 100 is the MAINT version
 * (VOLK_VERSION / 100) % 100 is the MINOR version
 * (VOLK_VERSION / 100) / 100 is the MAJOR version
 */

#define VOLK_VERSION 020500

__VOLK_DECL_END

#endif /* INCLUDED_VOLK_VERSION_H */
