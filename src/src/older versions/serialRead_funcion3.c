/*
 * serialRead_funcion.c
 * This program reads form the serial port of the RPi and writes data to
 * a log file
 */


/*VERSION 3 --
 --------------------------------------------------------------------
~ timestamp - timestamp actions 
~ pasar readings.log a directorio ../log/
~ use write, read, open, close instead of fopen, fprintf, etc //not finished --\n
~ muX function - multiplex MUX_1 (bi-state) and MUX_2 (quad-state)
---------------------------------------------------------------------
*/


 /*VERSION 2 --
 ----------------------------------------------------------
~ muX fuction - multiplex from a 2 or 3 states multiplexor
~ _init_atlas - init and configure pH and EC sensors
-----------------------------------------------------------
*/

//StandarLib
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

//wiringPI -- check website... thnx for being there!
#include <wiringSerial.h>
#include <wiringPi.h>


#define BAUDRATE 38400



 //GPIO USAGE
//                MUX_1
#define _mux1_S0 17
#define _mux1_S1 27
//                MUX_2
#define _mux2_S0 17
#define _mux2_S1 17
#define _mux2_S2 17
#define _mux2_S3 17


//COMMON COMMANDS 
//
#define EN_SRM "C,0\r"        //Disable continous reading mode
#define EN_LED "L,1\r"        //Enable LED indicator 
#define READ "R\r"            //Read


//EC CIRCUIT COMMANDS
//
#define DIS_TDS "O,TDS,0\r"   //Disable TDS
#define DIS_S "O,S,0\r"       //Disable Salinity
#define DIS_EC "O,EC,0\r"     //Disable EC
#define DIS_SG "O,SG,0\r"     //Disable Specific Gravity of Sea water
#define EN_TDS "1,TDS,0\r"    //Enable  TDS
#define EN_S "O,1,0\r"        //Enable  Salinity
#define EN_EC "1,EC,0\r"      //Enable EC
#define EN_SG "1,SG,0\r"      //Enable  Specifc Gravity of Sea water


//DEFINE CHANNELS MUX_1
//            S0 S1
const int  _y0 [] = {0,0,-1};
const int  _y1 [] = {1,0,-1};
const int  _y2 [] = {0,1,-1};
const int  _y3 [] = {1,1,-1};



//DEFINE CHANNELS MUX_2
//             S0,S1,S2,S3
int  _ch0  [] = {0,0,0,0,0};
int  _ch1  [] = {1,0,0,0,0};
int  _ch2  [] = {0,1,0,0,0};
int  _ch3  [] = {1,1,0,0,0};
int  _ch4  [] = {0,0,1,0,0};
int  _ch5  [] = {1,0,1,0,0};
int  _ch6  [] = {0,1,1,0,0};
int  _ch7  [] = {1,1,1,0,0};
int  _ch8  [] = {0,0,0,1,0};
int  _ch9  [] = {1,0,0,1,0};
int  _ch10 [] = {0,1,0,1,0};
int  _ch11 [] = {1,1,0,1,0};
int  _ch12 [] = {0,0,1,1,0};
int  _ch13 [] = {1,0,1,1,0};
int  _ch14 [] = {0,1,1,1,0};
int  _ch15 [] = {1,1,1,1,0};


//Output File
#define LOG "../log/reading.log"
#define TS_LEN 20
#define DATA_LEN 128
#define STR_LEN 255
#define LOG_HEADER "DATE\t  TIME\t\tph\tEC\n"


//Serial Communication -- Enviar Comandos + CaptartimeSta Respuestas
void Communicateur(int, const char *, char * );
void muX(const int *);           //multiplexor
void _init_atlas_(int fd);       // Configurar los circuitos EC y pH
char *tokenizer (char*, int );   //Break into tokens
void get_timestamp(char*);        //make timestampp


int main (){
  int fd ;                      //Serial Port Descriptor
  int fl;                       //Log File descriptor  
  char pHdata[DATA_LEN];               //Cleansed Data Buffer
  char ECdata[DATA_LEN];               //Cleansed Data Buffer  
  char timestamp[TS_LEN];                  // Custom timestamp
  char str[STR_LEN];
  //char *finalReading;         //Interest Value or Value of interest?(!)


// inicializar SISTEMA
  if ( (fl = open(LOG, O_WRONLY | O_CREAT | O_APPEND, 0666)) <0){
    fprintf (stderr, "Unable to open file: %s\n", strerror (errno));
    return 1 ;
  }

  //File Header
  strcpy(str, LOG_HEADER);//prepare buffer
  write(fl, str, strlen(str));

  if ((fd = serialOpen ("/dev/ttyAMA0", BAUDRATE)) < 0){
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno));
    return 1 ;
  }

  //wiringPiSetup();
  wiringPiSetupSys();
//  _init_atlas_(fd); 
//  final inicializacion

// tomar lecturas y escibir a un fichero
  int i =4;
  while(i>0){
                          
    muX(_y3);      //Multiplex to Y3, read port and print
    Communicateur(fd, READ, pHdata);
    printf("pH Data:%s \n", pHdata); 
    
    muX(_y0);  //Multiplex to Y0, read port and print
    Communicateur(fd, READ ,ECdata);
    printf("EC Data:%s \n", ECdata);

    if (pHdata !=NULL && ECdata !=NULL)  //Write sensor reading to log
      get_timestamp(timestamp);             //get timestamp
      sprintf(str,"%s\t%s\t%s\n",timestamp, pHdata, ECdata);//prepare buffer
      write(fl, str, strlen(str));          //write to file

    i--;
  }
// final lectura y logging


  close(fl);
  return 0;
}



void Communicateur(int fd, const char* cmd, char * data){
  char buffer[255];  /* Input buffer */
  char *bufptr;      /* Current char in buffer */
  int  nbytes;       /* Number of bytes read */
  int  tries;        /* Number of tries so far */

  for (tries=0;tries<3;tries++){
    //serialPuts (fd, "R\r");
    serialPuts (fd, cmd);
    bufptr = buffer;

    while ((nbytes = read(fd, bufptr, buffer + sizeof(buffer) - bufptr - 1)) > 0){
      bufptr += nbytes;
      if (bufptr[-1] == '\n' || bufptr[-1] == '\r')
        break;
    }

     /* nul terminate the string and see if we got an OK response */
    *bufptr = '\0';
    
    if (strncmp(buffer, "OK", 2) == 0)
      printf("Commando %s Ejecutado con Exito\n", cmd);
           // fflush (stdout) ;
  }

  size_t i;
  for (i = 0; i < strlen(buffer) && buffer[i] != '\r'; i++)
      data[i] = buffer[i];
        for ( ; i < strlen(buffer); i++)
          data[i] = '\0';
}
  

void muX(const int* chanState){
  if( chanState[2] == -1){
    digitalWrite(_mux1_S0, chanState[0]);
    digitalWrite(_mux1_S1, chanState[1]);
    //printf("S0 sTATE: %d\n", digitalRead(S0_pin));
    //printf("S1 sTATE: %d\n", digitalRead(S1_pin));
  }
  else{
    digitalWrite(_mux2_S0, chanState[0]);
    digitalWrite(_mux2_S1, chanState[1]);
    digitalWrite(_mux2_S2, chanState[2]);
    digitalWrite(_mux2_S3, chanState[3]);
  }
}


//This function brings the sensors to a state of single reading mode
//and enables or disables parametres for the sensor to output 
//idea de  version a implementar:
/*void _init_altas_(int fd, int argm, char *argc)           
  argm = numero de paramentos a cambiar
  argc = comandos a enviar, sensor y valores para el muX
  for (;i<argm;i++){Communicateur(fd,argc[i], data); }*/  

void _init_atlas_(int fd){
  char data[255];

  //Y0 - EC Sensor
  muX(_y0);

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
 

  //Y3 - pH Sensor
  muX(_y3);


  Communicateur(fd, "C,1\r", data);
  printf("Enable Default Reading Mode: %s\n", data);

  sleep(3);


  Communicateur(fd, EN_SRM, data);
  printf("Enable Single Reading Mode: %s\n", data);

  Communicateur(fd, EN_LED, data);
  printf("Enable LED: %s\n", data);

}



void get_timestamp(char* timebuffer)
{
    time_t ltime;
    struct tm *Tm;

    ltime=time(NULL);
    Tm=localtime(&ltime);

   sprintf(timebuffer, "%d/%d/%d,%d:%d:%d", 
        Tm->tm_mday, Tm->tm_mon+1, Tm->tm_year+1900,
        Tm->tm_hour, Tm->tm_min, Tm->tm_sec);
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


/////////////////////////////////////////////////////////////////
/*//INvestigate Strange Behavioure of Communicateur
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
133                for ( ; i < strlen(buffer); i++)
(gdb) step
99  void Communicateur(int fd, char * data)
(gdb) step
133                for ( ; i < strlen(buffer); i++)*/
//////////////////////////////////////////////////////////////////////

