#
# Makefile:
#	wiringPi - Wiring Compatable library for the Raspberry Pi
#	https://projects.drogon.net/wiring-pi
#
#	Copyright (c) 2012 Gordon Henderson
#################################################################################
# This file is part of wiringPi:
#	Wiring Compatable library for the Raspberry Pi
#
#    wiringPi is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    wiringPi is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
#################################################################################


DEBUG	= -g -O0
#DEBUG	= -O3
CC	= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -g `mysql_config --cflags`

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm `mysql_config --libs`

# Should not alter anything below this line
###############################################################################

SRC	=	serialRead.c serialRead_funcion.c control_inv.c calibrar_ec.c continous_reading.c

OBJ	=	$(SRC:.c=.o)

BINS	=	$(SRC:.c=)

control_inv:	control_inv.o
	@echo [link]
	@$(CC) -o $@ control_inv.o $(LDFLAGS) $(LDLIBS) 

calibrar_atlas:	calibrar_atlas.o
	@echo [link]
	@$(CC) -o $@ calibrar_atlas.o $(LDFLAGS) $(LDLIBS) 

continous_reading:	continous_reading.o
	@echo [link]
	@$(CC) -o $@ continous_reading.o $(LDFLAGS) $(LDLIBS) 
.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@echo "[Clean]"
	@rm -f $(OBJ) *~ core tags $(BINS)

tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

depend:
	makedepend -Y $(SRC)

# DO NOT DELETE
