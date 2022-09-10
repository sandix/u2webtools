#****************************************************************
#
# httpget
#
# Copyright (C) 1999 - 2022,
#   Henning Sander, <sandix@universal-logging-system.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
#****************************************************************
#
PRG = httpget

BINFILE ?= httpget${DEBUGEXT}

CC ?= gcc

PREFIX ?= /usr

MINCFLAGS := -D_LARGEFILE64_SOURCE -DWITH_IPV6

TESTCFLAGS := -DWITH_HTTPS -DWITH_GETTEXT

MINLIBS := 

STATIC ?= 

WALL := -Wall


MAININCLUDE := httpget.h

AWK = awk

PRGCS = httpget.c \
	httpget_io.c \
	httpget_http.c \
	httpget_httpget.c \
	httpget_str.c

PRGHS = ${MAININCLUDE}

include Makefile.all
