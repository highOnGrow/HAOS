/*
 * serialRead_funcion.c
 * This program reads form the serial port of the RPi and writes data to
 * a log file
 */

/*VERSION 4 --
--------------------------------------------------------------------
~ relay setup -> relay_test(), safestate()

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
#define _mux1_S0  4
#define _mux1_S1  3
//                MUX_2
#define _mux2_S0  17
#define _mux2_S1  27
#define _mux2_S2  22
#define _mux2_S3  23
#define _mux2_SIG 24
//                Relay
#define _r_IN1    5
#define _r_IN2    6
#define _r_IN3    13
#define _r_IN4    19
#define _r_IN5    26
#define _r_IN6    16
#define _r_IN7    20
#define _r_IN8    21
//                Dosers
#define _dos_1    _r_IN8
#define _dos_2
#define _dos_3
#define _dos_4
#define _dos_5

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
//            S0,S1,S2,S3,SIG
int  _ch0  [] = {0,0,0,0};
int  _ch1  [] = {1,0,0,0};
int  _ch2  [] = {0,1,0,0};
int  _ch3  [] = {1,1,0,0};
int  _ch4  [] = {0,0,1,0};
int  _ch5  [] = {1,0,1,0};
int  _ch6  [] = {0,1,1,0};
int  _ch7  [] = {1,1,1,0};
int  _ch8  [] = {0,0,0,1};
int  _ch9  [] = {1,0,0,1};
int  _ch10 [] = {0,1,0,1};
int  _ch11 [] = {1,1,0,1};
int  _ch12 [] = {0,0,1,1};
int  _ch13 [] = {1,0,1,1};
int  _ch14 [] = {0,1,1,1};
int  _ch15 [] = {1,1,1,1};


//Output File
#define LOG "../log/reading.log"
#define TS_LEN 20
#define DATA_LEN 128
#define STR_LEN 255
#define LOG_HEADER "DATE\t  TIME\t\tph\tEC\tTemp\tHum\n"



struct readings{
    float pH;   //pH
    float ec;   //electroconductividad
    float temp; //temperatura en  C
    float hum;  //relative humidity
    float lux;  //lumens
    int nat;    //nivel alto tanque
    int mbt;    //nivel bajo tanque
    int nmc;    //nivel minimo cisterna
    int nbc;    //nivel bajo cisterna
};


//Rangos de Condiciones Optimas
struct opt_range{
  float pH_min;
  float pH_max;
  float ec_min;
  float ec_max;
  float temp_min;
  float temp_max;
  float hum_min;
  float hum_max;
};

//Serial Communication -- Enviar Comandos + CaptartimeSta Respuestas
void Communicateur(int, const char *, char * );
void muX(const int *);           //multiplexor


void _init_atlas_(int fd);       // Configurar los circuitos EC y pH
struct opt_range _init_or();
struct readings  _init_rd();

char *tokenizer (char*, int );   //Break into tokens
void get_timestamp(char*);        //make timestampp
struct readings am2302_reader(struct readings);  //Read From AM2302 sensor
void relay_test();               //Tests that all relays are working OFF/ON/OFF
void safe_state();              //Turn relays OFF



int main (){
  int fd ;                      //Serial Port Descriptor
  int fl;                       //Log File descriptor  
  char pHdata[DATA_LEN];               //Cleansed Data Buffer
  char ECdata[DATA_LEN];               //Cleansed Data Buffer  
  char timestamp[TS_LEN];                  // Custom timestamp
  char str[STR_LEN];
  
  struct readings rd;         //Interest Value or Value of interest?(!)
  struct opt_range or;


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


//wiringPiSetup();        //Faster but needs superuser permissions
 wiringPiSetupSys();
 //_init_atlas_(fd); 
 //relay_test();
safe_state();

 or = _init_or();
 rd = _init_rd();

printf("pH_max: %.2f\n", or.pH_max);
printf("pH_min: %.2f\n", or.pH_min);


//  final inicializacion

// tomar lecturas y escibir a un fichero

  int i =1;
  while(i>0){
                          
    muX(_y3);      //Multiplex to Y3, read port and export data
    Communicateur(fd, READ, pHdata);
    
    muX(_y0);  //Multiplex to Y0, read port and export data
    Communicateur(fd, READ ,ECdata);


    if (pHdata !=NULL && ECdata !=NULL)  //Write sensor reading to log
      rd.pH = strtof(pHdata,NULL);
      rd.ec = strtof(ECdata, NULL);

      printf("pH Data:%.3f \n", rd.pH); 
      printf("EC Data:%.2f \n", rd.ec);

    i--;
  }

  muX(_ch1);
  rd = am2302_reader(rd);

  get_timestamp(timestamp);
  sprintf(str,"%s\t%.3f\t%.2f\t%.1fC\t%.1f%%\n",
        timestamp, rd.pH, rd.ec,rd.temp,rd.hum);//prepare buffer
            write(fl, str, strlen(str));          //write to file

// final lectura y logging UART
  //muX(_ch1);

  for (i=0; i<10; i++){
    muX(_ch4); 
    printf("SIG test: %d\n",digitalRead(_mux2_SIG));

    sleep(1);
  }

  close(fl);
  return 0;
}


//Funcion de Comunicacion Serial UART
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

//Inicializar con los valores optimos del sistema
struct opt_range _init_or(){
  struct opt_range or;

  or.pH_min   = 5.8 ;
  or.pH_max   = 6.8 ;
  or.ec_min   = 1.5 ;
  or.ec_max   = 2.8 ;
  or.temp_min = 16  ;
  or.temp_max = 32  ;
  or.hum_min  = 30  ;
  or.hum_max  = 70  ;

  return or;
}

//Inicializar con los valores que ponen el sistema en estado seguro
//antes de empezar a tomar lecturas
struct readings _init_rd(){

  struct readings rd;

  rd.pH   = 6.0;  //pH
  rd.ec   = 2.0;  //electroconductividad
  rd.temp = 20 ;  //temperatura en  C
  rd.hum  = 40 ;  //relative humidity
  rd.lux  = 1  ;  //lumens
  rd.nat  = 1  ;  //nivel alto tanque
  rd.mbt  = 1  ;  //nivel bajo tanque
  rd.nmc  = 1  ;  //nivel minimo cisterna
  rd.nbc  = 1  ;  //nivel bajo cisterna

  return rd;
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


struct readings am2302_reader(struct readings rd ){
  
  FILE *fp;
  char buffer[1035];
  char delimiters[] = " ,=";
  int  i=0;
  struct readings aux_r;

  aux_r = rd;

  while(i!=-1){
    /* Open the command for reading. */
    //fp = popen("sudo ../files/AdafruitDHT.py 2302 24", "r");
     fp = popen("sudo ../files/DHT 2302 24", "r");

    if (fp == NULL) {
      printf("Failed to run command\n" );
      exit(1);
    }

    /* Read the output a line at a time - output it. */
    while (fgets(buffer, sizeof(buffer)-1, fp) != NULL){
      printf("Buffer: %s", buffer);
    }

    char * str;
    str = strtok (buffer,delimiters);

      while (str != NULL)
      {
        if(!i){
          if(!strcmp(str,"Temp")){
           // printf ("%s\n",str);
            str = strtok (NULL, delimiters);
            aux_r.temp = strtof(str,NULL);
            printf("Temperatura: \t%.1f\n", aux_r.temp);
            i++;
          }
          else{
            printf("Reading Sensor Failed\n");
            pclose(fp);
            break;
          }
        }
        else{
            if(i==4){
              aux_r.hum = strtof(str,NULL);
              printf("Humedad: \t%.1f\n", aux_r.hum);
              i = -1;
              break;
            }
            else{
          //    printf ("%s\n",str);
              str = strtok (NULL, delimiters);
              i++;
            }
        }
      }
  }
  pclose(fp);
  return aux_r;
}


void relay_test(){


  //OFF
  digitalWrite(_r_IN1, HIGH);
  sleep(1);
  digitalWrite(_r_IN2, HIGH);
  sleep(1);
  digitalWrite(_r_IN3, HIGH);
  sleep(1);
  digitalWrite(_r_IN4, HIGH);
  sleep(1);
  digitalWrite(_r_IN5, HIGH);
  sleep(1);
  digitalWrite(_r_IN6, HIGH);
  sleep(1);
  digitalWrite(_r_IN7, HIGH);
  sleep(1);
  digitalWrite(_r_IN8, HIGH);
  sleep(1);

  //ON
  digitalWrite(_r_IN1, LOW);
  sleep(1);
  digitalWrite(_r_IN2, LOW);
  sleep(1);
  digitalWrite(_r_IN3, LOW);
  sleep(1);
  digitalWrite(_r_IN4, LOW);
  sleep(1);
  digitalWrite(_r_IN5, LOW);
  sleep(1);
  digitalWrite(_r_IN6, LOW);
  sleep(1);
  digitalWrite(_r_IN7, LOW);
  sleep(1);
  digitalWrite(_r_IN8, LOW);
  sleep(1);

//LEAVE RELAYS IN SAFE MODE
  digitalWrite(_r_IN1, HIGH);
  sleep(1);
  digitalWrite(_r_IN2, HIGH);
  sleep(1);
  digitalWrite(_r_IN3, HIGH);
  sleep(1);
  digitalWrite(_r_IN4, HIGH);
  sleep(1);
  digitalWrite(_r_IN5, HIGH);
  sleep(1);
  digitalWrite(_r_IN6, HIGH);
  sleep(1);
  digitalWrite(_r_IN7, HIGH);
  sleep(1);
  digitalWrite(_r_IN8, HIGH);
  sleep(1);
}


void safe_state(){

  printf("Turning OFF relays...\n");

  //LEAVE RELAYS IN SAFE MODE
  digitalWrite(_r_IN1, HIGH);
  digitalWrite(_r_IN2, HIGH);
  digitalWrite(_r_IN3, HIGH);
  digitalWrite(_r_IN4, HIGH);
  digitalWrite(_r_IN5, HIGH);
  digitalWrite(_r_IN6, HIGH);
  digitalWrite(_r_IN7, HIGH);
  digitalWrite(_r_IN8, HIGH);

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

