/*
 * serial.c:
 *  Example program to read bytes from the Serial line
 *
 * Copyright (c) 2012-2013 Gordon Henderson. <projects@drogon.net>
 ***********************************************************************
 * This file is part of wiringPi:
 *  https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <wiringSerial.h>
#include <wiringPi.h>

int main (int argc, char *argv[])
{
  int fd ;
  
 // FILE *fl;

 // fl = fopen("/tmp/sensorsread.log","a+");

  if ((fd = serialOpen ("/dev/ttyAMA0", 38400)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

  
  char buffer[255];  /* Input buffer */
  char  data[255];    /* Cleanced Data Buffer*/
  char *bufptr;      /* Current char in buffer */
  int  nbytes;       /* Number of bytes read */
  int  tries;        /* Number of tries so far */


  //pinMode (11, OUTPUT) ;
  //pinMode (13, OUTPUT) ;


  for (tries=0;tries<3;tries++)
  {
      serialPuts (fd, "R\r");
      bufptr = buffer;

      while ((nbytes = read(fd, bufptr, buffer + sizeof(buffer) - bufptr - 1)) > 0)
      {
        bufptr += nbytes;
          if (bufptr[-1] == '\n' || bufptr[-1] == '\r')
            break;
        }

   /* nul terminate the string and see if we got an OK response */
        *bufptr = '\0';

        if (strncmp(buffer, "OK", 2) == 0)
          {
            printf("BUffer OK: %s\n",buffer);
          }
         // fflush (stdout) ;
    }

        memcpy(data, buffer, strlen(buffer)-5);
        printf("Data: %s\n",data);
    //Multiplex to Y3
    //digitalWrite(11, 1);
    //digitalWrite(13, 1);

  return 0;
}
