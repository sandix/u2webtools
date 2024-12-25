#****************************************************************
#
# unix2web
#
# Copyright (C) 1999 - 2024, Henning Sander,
#  <sandix@universal-logging-system.org>
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
all:
	BINFILE=unix2webd make -f Makefile.unix2webd${MYSQL}${SQLITE3}
	BINFILE=u2w_interpreter make -f Makefile.interpreter${MYSQL}${SQLITE3}
	BINFILE=u2w_passwd make -f Makefile.u2w_passwd

install:
	BINFILE=unix2webd make -f Makefile.unix2webd${MYSQL}${SQLITE3} install
	BINFILE=u2w_interpreter make -f Makefile.interpreter${MYSQL}${SQLITE3} install
	BINFILE=u2w_passwd make -f Makefile.u2w_passwd install

copybin:
	BINFILE=unix2webd make -f Makefile.unix2webd${MYSQL}${SQLITE3} copybin
	BINFILE=u2w_interpreter make -f Makefile.interpreter${MYSQL}${SQLITE3} copybin
	BINFILE=u2w_passwd make -f Makefile.u2w_passwd copybin

clean:
	BINFILE=unix2webd make -f Makefile.unix2webd clean
	BINFILE=unix2webd make -f Makefile.unix2webd_mysql clean
	BINFILE=u2w_interpreter make -f Makefile.interpreter clean
	BINFILE=u2w_interpreter make -f Makefile.interpreter_mysql clean
	BINFILE=u2w_passwd make -f Makefile.u2w_passwd clean

unix2webd:
	BINFILE=unix2webd make -f Makefile.unix2webd${MYSQL}${SQLITE3}

u2w_interpreter:
	BINFILE=u2w_interpreter make -f Makefile.interpreter${MYSQL}${SQLITE3}

u2w_passwd:
	BINFILE=u2w_passwd make -f Makefile.u2w_passwd

copyunix2webd:
	BINFILE=unix2webd make -f Makefile.unix2webd${MYSQL}${SQLITE3} copybin

copyu2w_interpreter:
	BINFILE=u2w_interpreter make -f Makefile.interpreter${MYSQL}${SQLITE3} copybin

copyu2w_passwd:
	BINFILE=u2w_passwd make -f Makefile.u2w_passwd copybin
