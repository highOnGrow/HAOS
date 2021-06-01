/*
 * serialRead_funcion.c
 * This program reads form the serial port of the RPi and writes data to
 * a log file
 */



 /*VERSION 2 --  
new functions added

~ muX fuction to multiplex from a 2 or 3 states multiplexor
~ _init_atlas to configure the pH and EC sensors

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
#define EN_SRM "C,0\r"        //Disable continous reading mode
#define EN_LED "L,1\r"        //Enable LED indicator 
#define READ "R\r"            //READ COMMAND
#define DIS_TDS "O,TDS,0\r"   // Disable TDS
#define DIS_S "O,S,0\r"       // Disable Salinity
#define DIS_EC "O,EC,0\r"    //Disable EC
#define DIS_SG "O,SG,0\r"    //Specific Gravity of Sea water

#define EN_TDS "1,TDS,0\r"   //Enable  TDS
#define EN_S "O,1,0\r"       //Enable  Salinity
#define EN_EC "1,EC,0\r"    //Enable EC
#define EN_SG "1,SG,0\r"    //Enable  Specifc Gravity of Sea water


#define MUX_1 -1

//#define OUT 1
//#define IN  0

//Serial Communication -- Enviar Comandos + Captar Respuestas
void Communicateur(int, const char *, char * );
//Multiplexor
void muX(int, int, int);
void _init_altas_(int fd);
char *tokenizer (char*, int );


int main (int argc, char *argv[])
{
  int fd ;                      //Serial Port Descriptor
  char data[255];               //Cleansed Data Buffer
  FILE * fl;                    //Log File descriptor  
  char *finalReading;           //Interest Value

  fl = fopen("../files/readings.log","a+");

  if ((fd = serialOpen ("/dev/ttyAMA0", BAUDRATE)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

  //wiringPiSetup();
  wiringPiSetupSys();


  _init_altas_(fd); // Configurar los circuitos de lectura de EC y pH


  int i =4;
  while(i>0){
                          
    muX(HIGH, HIGH, MUX_1);      //Multiplex to Y3
    Communicateur(fd, READ, data);     //Read data from port
    finalReading = data;  //pH - Data to consider
    

    ///Debug Message -- FUTURE VERBOSE MODE////////
    printf("Data:%s \n", data); 
    printf("FinalReading:%s \n", finalReading);
   ////////////////////////////////////////////////
   

   if (finalReading !=NULL)  //Write sensor reading to log
      fprintf(fl, "%s\n", finalReading);

    
    muX(LOW, LOW, MUX_1);  //Multiplex to Y0
    Communicateur(fd, READ ,data);

   ///Debug Message -- FUTURE VERBOSE MODE////////
    printf("Data:%s \n", data);
    printf("FinalReading:%s \n", finalReading);
   ///////////////////////////////////////////////

    if (finalReading !=NULL)
      fprintf(fl, "%s\n", finalReading);

    i--;
  }

  fclose(fl);
  return 0;
}



void Communicateur(int fd, const char* cmd, char * data)
{
    char buffer[255];  /* Input buffer */
    char *bufptr;      /* Current char in buffer */
    int  nbytes;       /* Number of bytes read */
    int  tries;        /* Number of tries so far */


    for (tries=0;tries<3;tries++)
    {
        //serialPuts (fd, "R\r");
        serialPuts (fd, cmd);
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
              printf("Commando %s Ejecutado con Exito\n", cmd);
            }
           // fflush (stdout) ;
      }

      size_t i;
      for (i = 0; i < strlen(buffer) && buffer[i] != '\r'; i++)
              data[i] = buffer[i];
               for ( ; i < strlen(buffer); i++)
                   data[i] = '\0';
}
  

/*//INvestigate Strange Behavioure od Communicateur
gdb) step
99  void Communicateur(int fd, char * data)
(gdb) step
133                for ( ; i < strlen(buffer); i++)
(gdb) step
99  void Communicateur(int fd, char * data)
(gdb) step
133                for ( ; i < strlen(buffer); i++)
(gdb) step
99  void Communicateur(int fd, char * data)
(gdb) step
133                for ( ; i < strlen(buffer); i++)
(gdb) step
99  void Communicateur(int fd, char * data)
(gdb) step
133                for ( ; i < strlen(buffer); i++)
(gdb) step
99  void Communicateur(int fd, char * data)
(gdb) step
133                for ( ; i < strlen(buffer); i++)
(gdb) step
99  void Communicateur(int fd, char * data)
(gdb) step
133                for ( ; i < strlen(buffer); i++)*/

void muX(int state_S0,int state_S1, int state_S2)
{
  if( state_S2 == -1)
  {
    digitalWrite(S0_pin, state_S0);
    digitalWrite(S1_pin, state_S1);
    printf("S0 sTATE: %d\n", digitalRead(S0_pin));
    printf("S1 sTATE: %d\n", digitalRead(S1_pin));
  }
  else
  {
        ///multiplex with MUX_SPK
  }

}


//This function brings the sensors to a state of single reading mode
//and enables or disables parametres for the sensor to output 


/*void _init_altas_(int fd, int argm, char *argc)           //2nda version a implementar
  argm = numero de paramentos a cambiar
  argc = comandos a enviar, sensor y valores para el muX
  for (;i<argm;i++){Communicateur(fd,argc[i], data); }*/  


void _init_altas_(int fd)
{
  char data[255];

  //Y0 - EC Sensor
  muX(LOW,LOW,MUX_1);

  Communicateur(fd, "C,1\r", data);
  printf("Enable Default Reading Mode: %s\n", data);

  sleep(3);

  Communicateur(fd, EN_SRM, data);
  printf("Enable Single Reading Mode: %s\n", data);

  Communicateur(fd, EN_LED, data);
  printf("Enable LED: %s\n", data);

  Communicateur(fd, DIS_TDS, data);
  printf("Disable TDS: %s\n", data);

  Communicateur(fd, DIS_S, data);
  printf("Disable S: %s\n", data);

  Communicateur(fd, DIS_SG, data);
  printf("Disable SG: %s\n", data);
 

  //Y1 - pH Sensor
  muX(HIGH,HIGH,MUX_1);


  Communicateur(fd, "C,1\r", data);
  printf("Enable Default Reading Mode: %s\n", data);

  sleep(3);


  Communicateur(fd, EN_SRM, data);
  printf("Enable Single Reading Mode: %s", data);

  Communicateur(fd, EN_LED, data);
  printf("Enable LED: %s", data);

}




//             NOT WORKING               //
//                    DO NOT USE         // 
//Break down sensor reading if needed.***//
//Returns an array of the strings readed*//
/*
char *tokenizer (char* data, int indx)
{  
  const char delimiter[]= ",";  //Delimiter for token
  char *str;

  str = (char*)malloc(indx*sizeof(char));

  str = strtok (data, delimiter);
  for(;indx>0 && str!=NULL ;indx--)
    str++ = strtok (NULL, delimiter);
 
  return NULL;
}*/

