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
#include <signal.h>


//wiringPI -- check website... thnx for being there!
#include <wiringSerial.h>
#include <wiringPi.h>


//mySQL
#include <my_global.h>
#include <mysql.h>


//SERIAL PORT COMMUNICATION SPEED
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
#define _r_IN8    21    //Bomba llenado tanque
//                Dosers
#define _pH_minus  _r_IN2
#define _pH_plus   _r_IN1
#define _nutr_1    _r_IN3
#define _dos_4
#define _dos_5

//GRAVEDAD ALARMAS
//
#define _a_GRAVE   10
#define _a_MEDIA   10
#define _a_LEVE    1


//CΑΝΤΙDΑD de ml/s de las dosidicadoras
#define _carga_s   0.4
//CΑΝΤΙDΑD de s/ml de las dosidicadoras
#define _carga_ml  2.4
 

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
//            S0,S1,S2,S3
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
#define DATE_FORMAT "%d/%m/%Y %H:%i:%s"


//ALERT IF GREATER THAN...
#define MAX_TEMP 31
#define MAX_HUM  75

//Data base Credencials
#define SRV  "localhost"
#define DB_  "control_inv"  //DB name     
#define DB_U "ctrl_inv"     //User
#define DB_P "password"     //Password
#define HORTALIZA {"","Jitomate","","Espinaca"}

struct readings{
    float pH;   //pH
    float ec;   //electroconductividad
    float temp; //temperatura en  C
    float hum;  //relative humidity
    float lux;  //lumens
    int nat;    //nivel alto tanque
    int nbt;    //nivel bajo tanque
    int nmc;    //nivel minimo cisterna
    int nbc;    //nivel bajo cisterna
};


//Rangos de Condiciones Optimas e informacion de cultivo
struct c_conf{
  float pH_min;
  float pH_max;
  float ec_min;
  float ec_max;
  char nombre_planta[64];
  float cap_tanq;
  float dosis;
  int idCultivo;
  int idUser;          //Falta query que los inicializa de aqui en adelante
  int idPlanta;
  int idLector_ph;     //....
  int idLector_ec;     //....
  int idLector_temp;   //....
  int idLector_nat;    //....
  int idLector_nbt;    //....
  int idLector_nmc;    //....
  int idLector_nbc;    //....
  int idLector_lux;    //....
};

//Serial Communication -- Enviar Comandos + CaptartimeSta Respuestas
void Communicateur(int, const char *, char * );
void muX(const int *);           //multiplexor


void _init_atlas_(int fd);       // Configurar los circuitos EC y pH
struct c_conf _init_conf(MYSQL*, const char*);
struct readings  _init_rd();

char *tokenizer (char*, int );            //Break into tokens
void get_timestamp(char*);                //make timestampp
struct readings am2302_reader(struct readings);  //Read From AM2302 sensor
void relay_test();                        //Tests that all relays are working OFF/ON/OFF
void safe_state();                        //Turn relays OFF
MYSQL* connectDB();                       //Connect and init database
void db_error(MYSQL *);                   //Data base error capture and finalize sequence
void shuttingdown(int, int, MYSQL *);     //Close file descriptors and desconnect DB
void Logger(MYSQL* , char *);             //Write to dataBase

int llenar_tanque(int mode);              //fill tank
void pHmodify(int mode);                 //intro pH-(1) or pH+(-1)
int food_Calc( const struct readings, const struct c_conf);     //FOOD cAlculator
void addFood(int);