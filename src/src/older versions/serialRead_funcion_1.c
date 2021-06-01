/*
 * serialRead_funcion.c
 * This program reads form the serial port of the RPi and writes data to
 * a log file
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>

#include <wiringSerial.h>
#include <wiringPi.h>


#define BAUDRATE 38400
#define S0_pin 17
#define S1_pin 27
//#define OUT 1
//#define IN  0

//Serial Reader
void Reader(int, char * );
void muX(int ,int );

int main (int argc, char *argv[])
{
  int fd ;                      //Serial Port Descriptor
  char data[255];               //Cleansed Data Buffer
  FILE * fl;                    //Log File descriptor
  const char delimiter[]= ",";  //Delimiter for token  
  char *finalReading;           //Interest Value



  fl = fopen("../files/readings.log","a+");

  if ((fd = serialOpen ("/dev/ttyAMA0", BAUDRATE)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

  //wiringPiSetup();

  wiringPiSetupSys () ;

int i =4;
while(i>0){
                        
  muX(HIGH, HIGH);      //Multiplex to Y3
  Reader(fd, data);     //Read data from port
  finalReading = data;  //pH - Data to consider
  

  ///Debug Message -- FUTURE VERBOSE MODE////////
  printf("Data:%s \n", data); 
  printf("FinalReading:%s \n", finalReading);
 ////////////////////////////////////////////////
 

 if (finalReading !=NULL)  //Write sensor reading to log
    fprintf(fl, "%s\n", finalReading);

  muX(LOW, LOW);  //Multiplex to Y0
  Reader(fd, data);

  //Break down sensor reading
  finalReading = strtok (data, delimiter);  //
  finalReading = strtok (NULL, delimiter);
  finalReading = strtok (NULL, delimiter);

  printf("Data:%s \n", data);
  printf("FinalReading:%s \n", finalReading);
  if (finalReading !=NULL)
    fprintf(fl, "%s\n", finalReading);

  i--;
}

  fclose(fl);
  return 0;
}



void Reader(int fd, char * data)
{
    char buffer[255];  /* Input buffer */
    char *bufptr;      /* Current char in buffer */
    int  nbytes;       /* Number of bytes read */
    int  tries;        /* Number of tries so far */


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
              printf("BUffer: %s\n",buffer);
            }
           // fflush (stdout) ;
      }

      size_t i;

//INvestigate Strange Behaviour in GDB
/*(gdb) step
99  void Reader(int fd, char * data)
(gdb) step
133                for ( ; i < strlen(buffer); i++)
(gdb) step
99  void Reader(int fd, char * data)
(gdb) step
133                for ( ; i < strlen(buffer); i++)
(gdb) step
99  void Reader(int fd, char * data)
(gdb) step
133                for ( ; i < strlen(buffer); i++)
(gdb) step
99  void Reader(int fd, char * data)
(gdb) step
133                for ( ; i < strlen(buffer); i++)
(gdb) step
99  void Reader(int fd, char * data)
(gdb) step
133                for ( ; i < strlen(buffer); i++)
(gdb) step
99  void Reader(int fd, char * data)
(gdb) step
133                for ( ; i < strlen(buffer); i++)
*/

      for (i = 0; i < strlen(buffer) && buffer[i] != '\r'; i++)
              data[i] = buffer[i];
               for ( ; i < strlen(buffer); i++)
                   data[i] = '\0';
    }
  


void muX(int state_S0,int state_S1)
{
    digitalWrite(S0_pin, state_S0);
    digitalWrite(S1_pin, state_S1);
    printf("S0 sTATE: %d\n", digitalRead(S0_pin));
    printf("S1 sTATE: %d\n", digitalRead(S1_pin));

}


//This function brings the sensors to a state of single reading mode
//and enables or disables parametres for the sensor to output 
void _init_altas_(int fd, const char *circuitSetup){

  muX(LOW,LOW);


}