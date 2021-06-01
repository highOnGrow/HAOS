/*
 * serialRead_funcion.c
 * This program reads form the serial port of the RPi and writes data to
 * a log file
 */



/*VERSION 6 -- Release
--------------------------------------------------------------------
~  main loop -- do what is has to be done
~  mySQL  - retrieve idPlanta, idUser, idCultivo, idSensores.... 
~  llenar_tanque() - function que completa con agua el tanque del cultivo
~  cambio de nombre de la struct opt_range -> struct c_conf donde estan
~  los ids de todos los sensores reflejados en la base de batos
~  pHmodify() - introduce pH-(1) o pH+(-1)
~  foodCalculator() - calculates the time that a dose needs to be introduced
~  addFoodd()  - activate/deactivate pumps during the time specified by foodCalculator()
*/


/*VERSION 5 --
--------------------------------------------------------------------
~  add mySQL support -- retrieve optimal ec/ph range and write log
~  create serialRead_funcion.h
*/


/*VERSION 4 --
--------------------------------------------------------------------
~ relay setup -> relay_test(), safestate()
~ read and parse AM2302 sensor - humidity / temp
~ variation in log format to include temp and hum
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

 /*VERSION 1 --
 ----------------------------------------------------------
~ read from serial port and store data in correct format
-----------------------------------------------------------
*/


#include "control_inv.h"


int main (){
  int fd ;                             //Serial Port Descriptor
  int fl;                              //Log File descriptor  
  char pHdata[DATA_LEN];               //Cleansed Data Buffer
  char ECdata[DATA_LEN];               //Cleansed Data Buffer  
  char timestamp[TS_LEN];              // Custom timestamp
  char str[STR_LEN];
  MYSQL *conn;                         //Connector to Data base
  
  struct readings rd;     //Interest Value or Value of interest?(!)
  struct c_conf or;       //sistem configuration retrieved from DB
  int planta = 1;         //Default: Jitomate
  int time2run = 0;       //tiempo de activar bomba nutriente
//  int dose_count = 0;     //contador de dosis
 // int t1 = 0;             //tiempo dosis 1...
  //int t2 = 0;             //tiempo dosis 2...


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



conn = connectDB();
//wiringPiSetup();        //Faster but needs superuser permissions
 wiringPiSetupSys();
 //_init_atlas_(fd); 
 //relay_test();
safe_state();
rd = _init_rd();



/*
while(!planta){
  fflush(stdin);
  printf("Que planta quiere cultivar?\n1. Jitomate \n2. Espinaca\n#:");
  scanf("%d",&planta);
  switch(planta){
    case 1: printf("Cultivemos Jitomate!\n");
            strcpy(nombre_planta,"jitomate");
            planta = 1;
            break;
    case 2: printf("Cultivemos Espinaca!\n");
            strcpy(nombre_planta,"espinaca");
          //  planta = 3;
            break;
    default:printf("Opcion errorea. Solo puede elejir el numero 1 o 2\n");
            planta = 0;
            break;       
  }

}
*/

//En este caso coincide el numero de planta con el del cultivo
or.idPlanta  = planta;
or.idCultivo = planta;

//Iniciate buffer with the Query    
sprintf(str,"SELECT * FROM cHortaliza WHERE idHortaliza ='%d'", or.idPlanta);
or = _init_conf(conn, str);
//or.nombre_planta = HORTALIZA[or.idPlanta];
//=============================================================================================
//  final inicializacion
//=============================================================================================

printf("Configuración del Sistema\n");
printf("=========================\n");
printf("=========================\n");
printf("Hortaliza: %s \n", or.nombre_planta);
printf("ph_minimo: %.2f \n", or.pH_min);
printf("ph_maximo: %.2f \n", or.pH_max);
printf("EC_minimo: %.1f \n", or.ec_min);
printf("EC_maximo: %.1f \n", or.ec_max);
printf("=========================\n");
printf("=====--------------======\n");
printf("======--EMPEZANDO--======\n");
printf("===========-A-===========\n");
printf("========--CRECER--=======\n");
printf("===========!!!===========\n");



//the GREAT Loop
while(1){


/* tomar lecturas de los sensores. Estan en el while para asegurar que
va ha haber al menos una lectura buena por parte de los sensores.*/  
 // while(1){
                          
    muX(_y3);      //Multiplex to Y3, read port and export data
    Communicateur(fd, READ, pHdata);
    
    muX(_y0);  //Multiplex to Y0, read port and export data
    Communicateur(fd, READ ,ECdata);


   // if (pHdata !=NULL && ECdata !=NULL){  //Write sensor reading to log
      rd.pH = strtof(pHdata,NULL);
      rd.ec = strtof(ECdata, NULL);
      muX(_ch1);
      rd = am2302_reader(rd);

      muX(_ch3);
      rd.nat = digitalRead(_mux2_SIG);
      printf("NAT (Nivel Alto Tanque): %d\n",digitalRead(_mux2_SIG));
      muX(_ch4);
      rd.nbt = digitalRead(_mux2_SIG);
      printf("NBT (Nivel Bajo Tanque): %d\n",digitalRead(_mux2_SIG));
      muX(_ch5);
      rd.nmc = digitalRead(_mux2_SIG);
      printf("NMC (Nivel Minimo Cisterna): %d\n",digitalRead(_mux2_SIG));
      muX(_ch6);
      rd.nbc = digitalRead(_mux2_SIG);
      printf("NBC (Nivel Bajo Cisterna): %d\n",digitalRead(_mux2_SIG));
      

      //Log to data base=========================================================

      //Log pH value
      sprintf(str,"INSERT INTO tLogLector  VALUES (NULL, 1, NOW(), %.3f, NULL, NULL, NULL, NULL, NULL, NULL)", rd.pH);
    // printf("Query: %s", str);
      Logger(conn, str);

      //Log EC value
      sprintf(str,"INSERT INTO tLogLector  VALUES (NULL, 2, NOW(), NULL, %.2f, NULL, NULL, NULL, NULL, NULL)", rd.ec);
    //  printf("Query: %s", str);
      Logger(conn, str);

      //Log Temp and Hum value
      sprintf(str,"INSERT INTO tLogLector  VALUES (NULL, 2, NOW(), NULL, NULL, %.1f, %.1f, NULL, NULL, NULL)",rd.temp,rd.hum);
    //  printf("Query: %s", str);
      Logger(conn, str);

      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++======++++++++=================
      
      //=================LOG to FILE =========================================
      get_timestamp(timestamp);
      sprintf(str,"%s\t%.3f\t%.2f\t%.1fC\t%.1f%%\n",
        timestamp, rd.pH, rd.ec,rd.temp,rd.hum);//prepare buffer
            write(fl, str, strlen(str));          //write to file
      //=======================================================================
    //  break;  //Exit while()
   // }
  // }

       printf("pH:%.3f  \n", rd.pH); 
       printf("EC:%.2f  \n", rd.ec);
       printf("Temp:%.1f\n", rd.temp);
       printf("Hum:%.1f \n", rd.hum);
       printf("NAT:%d  \n", rd.nat);
       printf("NBT:%d  \n", rd.nbt);
       printf("NMC:%d  \n", rd.nmc);
       printf("NBC:%d  \n", rd.nbc);

       if (!rd.nat && rd.nbt){
        printf("Alarma !!!  Error en los sensores flotadores del tanque\n");
       
        //               idAlarma, idCultivo, idActuador, idLector,Fecha In, Fecha Out, Valor, Severidad                   
        sprintf(str,"INSERT INTO tAlarmas  VALUES (NULL, %d, NULL, %d, NOW(), NULL, %d, %d)",or.idCultivo, or.idLector_nat, rd.nat, _a_GRAVE);
       // printf("Query: %s", str);
        Logger(conn, str);
        sprintf(str,"INSERT INTO tAlarmas  VALUES (NULL, %d, NULL, %d, NOW(), NULL, %d, %d)",or.idCultivo, or.idLector_nbt, rd.nbt, _a_GRAVE);
       // printf("Query: %s", str);
        Logger(conn, str);
        //break;
       }

       if (!rd.nmc && rd.nbc){
        printf("Alarma !!!  Error en los sensores flotadores de la cisterna\n");
        sprintf(str,"INSERT INTO tAlarmas  VALUES (NULL, %d, NULL, %d, NOW(), NULL, %d, %d)",or.idCultivo, or.idLector_nmc, rd.nmc, _a_GRAVE);
       // printf("Query: %s", str);
        Logger(conn, str);
        sprintf(str,"INSERT INTO tAlarmas  VALUES (NULL, %d, NULL, %d, NOW(), NULL, %d, %d)",or.idCultivo, or.idLector_nbc, rd.nbc, _a_GRAVE);
      //  printf("Query: %s", str);
        Logger(conn, str);
        //break;
       }
    //final lectura y logging ================================================================


       if( rd.nbt && !rd.nmc ){
        if(llenar_tanque(1)){

          sprintf(str,"INSERT INTO tAlarmas  VALUES (NULL, %d, NULL, %d, NOW(), NULL, %d, %d)",or.idCultivo, or.idLector_nat, rd.nat, _a_MEDIA);
          //printf("Query: %s", str);
          Logger(conn, str);
          sprintf(str,"INSERT INTO tAlarmas  VALUES (NULL, %d, NULL, %d, NOW(), NULL, %d, %d)",or.idCultivo, or.idLector_nbt, rd.nbt, _a_MEDIA);
          //printf("Query: %s", str);
          Logger(conn, str);
        }  
       }
      if(rd.ec > or.ec_max){
        printf("EC Alto.. Introducir Agua >\n");
        if(llenar_tanque(2)){

          sprintf(str,"INSERT INTO tAlarmas  VALUES (NULL, %d, NULL, %d, NOW(), NULL, %.1f, %d)",or.idCultivo, or.idLector_ec, rd.ec, _a_MEDIA);
          //printf("Query: %s", str);
          Logger(conn, str);
          sprintf(str,"INSERT INTO tAlarmas  VALUES (NULL, %d, NULL, %d, NOW(), NULL, %d, %d)",or.idCultivo, or.idLector_nat, rd.nat, _a_MEDIA);
          //printf("Query: %s", str);
          Logger(conn, str);
          sprintf(str,"INSERT INTO tAlarmas  VALUES (NULL, %d, NULL, %d, NOW(), NULL, %d, %d)",or.idCultivo, or.idLector_nbt, rd.nbt, _a_MEDIA);
          //printf("Query: %s", str);
          Logger(conn, str);
                    sleep(1);              //Give time for the mix to blend..
        }
       // else{
       //   printf("ALARMA! EC alto pero no se permite introducir agua\n");
       // }
      }
      if(rd.ec < or.ec_min){
          printf("EC Bajo.. Introducir Nutriente >\n");
          time2run = food_Calc(rd,or);
          printf("Time to run: %d\n", time2run);
          addFood(time2run);
          printf("Duermo 180s mientras se haga la mezcla\n");
          sleep(180);            //Give time for the mix to blend..
        }

      /*else{
        if(!dose_count){
          time2run = food_Calc(rd,or);
          t1 = time2run / 2;
          addFood(t1);
          dose_count++;
          sleep(1);           //Give time for the mix to blend..
        }
        else{
          addFood(t2);
          sleep(1);           //Give time for the mix to blend..
        }          
      }*/
      if(rd.pH > or.pH_max){
          printf("pH Alto.. Introducir pH- >\n");
          pHmodify(1);
          printf("Duermo 180s mientras se haga la mezcla\n");
          sleep(180);
      }
      if(rd.pH < or.pH_min){
          printf("pH Bajo.. Introducir pH+ >\n");
          pHmodify(-1);
          printf("Duermo 180s mientras se haga la mezcla\n");
          sleep(180); 
      }
      //dose_count = 0;         //Reiniciar Calculador de Dosis
 }


  close(fl);
  close(fd);
  mysql_close(conn);
  return 0;
}


////////////////////////////////////////////////////////////////////////////
////////Definiciones de Funciones
////////////////////////////////////////////////////////////////////////////




//Funcion de Comunicacion Serial UART
//Recibe el descriptor del puerto serie y devuelve un 
//puntero a string con la primera parte del string leido 
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
  
            //Multiplexar con _mux_1
  if( chanState[2] == -1){
    digitalWrite(_mux1_S0, chanState[0]);
    digitalWrite(_mux1_S1, chanState[1]);
  }       //Multiplexar con _mux_2
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
struct c_conf _init_conf( MYSQL* conn, const char * str){
  struct c_conf or;
 
 if (mysql_query(conn, str)) 
  {
      db_error(conn);
  }

  MYSQL_RES *result = mysql_store_result(conn);
  
  if (result == NULL) 
  {
      db_error(conn);
  }

  int num_fields = mysql_num_fields(result);
  if(num_fields)
  { 
    MYSQL_ROW row;
    row = mysql_fetch_row(result);

    strcpy(or.nombre_planta, row[1]);
    or.pH_max   = strtof(row[2], NULL) ;
    or.pH_min   = strtof(row[3], NULL) ;
    or.ec_max   = strtof(row[4], NULL) ;
    or.ec_min   = strtof(row[5], NULL) ;
  }
  else
  {
    printf("No hay valores optimos registrados por 'Jitomate'\n");
    printf("Se usaran valores genericos. \n");
  }


  or.idCultivo = 1;
  or.idUser    = 1;        //Falta query que lo inicializa
  or.idPlanta  = 1;
  or.idLector_ph   = 1 ;    //Falta query que lo inicializa
  or.idLector_ec   = 2 ;    //Falta query que lo inicializa
  or.idLector_temp = 3 ;    //Falta query que lo inicializa
  or.idLector_nat  = 4 ;    //....
  or.idLector_nbt  = 5 ;    //....
  or.idLector_nmc  = 6 ;    //....
  or.idLector_nbc  = 7 ;    //....
  or.idLector_lux  = 8 ;    //....
  
  or.cap_tanq = 1 ;         //Tanque de un litro
  or.dosis =    2 ;         //dosis = 2 ml/l


  mysql_free_result(result);
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
  rd.lux  = 0  ;  //lumens
  rd.nat  = 0  ;  //nivel alto tanque
  rd.nbt  = 0  ;  //nivel bajo tanque
  rd.nmc  = 0  ;  //nivel minimo cisterna
  rd.nbc  = 0  ;  //nivel bajo cisterna

  return rd;
}



void get_timestamp(char* timebuffer)
{
    time_t ltime;
    struct tm *Tm;

    ltime=time(NULL);
    Tm=localtime(&ltime);

   sprintf(timebuffer, "%d-%d-%d %d:%d:%d", 
        Tm->tm_mday, Tm->tm_mon+1, Tm->tm_year+1900,
        Tm->tm_hour, Tm->tm_min, Tm->tm_sec);
 }



//Funcion de Lectura de sensor de temperatura y humedad AM2302
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

    /* Read the output a line at a time - output line. */
    while (fgets(buffer, sizeof(buffer)-1, fp) != NULL){
      printf("Buffer: %s", buffer);
    }

    char * str;
    str = strtok (buffer,delimiters);


    //Mientras no hay una lectura correcta sigue intentando...
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
              // printf ("%s\n",str);
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


MYSQL* connectDB()
{
  MYSQL *conn;
  conn = mysql_init(NULL);
  
  if (conn == NULL)
  {
    fprintf(stderr, "mysql_init() failed\n");
    exit(1);
  }  
  
  if (mysql_real_connect(conn, SRV, DB_U, DB_P, 
          DB_, 0, NULL, 0) == NULL) 
  {
     db_error(conn);
  }
  return conn;
}


void db_error(MYSQL *conn)
{
  fprintf(stderr, "%s\n", mysql_error(conn));
  mysql_close(conn);
  exit(1);        
}



void shuttingdown(int fl, int fd, MYSQL *conn){
  printf("Closing Log file descriptor\n");
  close(fl);
  printf("Closing Serial port descriptor\n");
  close(fd);
  printf("Closing Data Base connection\n");
  mysql_close(conn);
}


void Logger(MYSQL *conn, char* str){

    if (mysql_query(conn, str)) 
  {
      db_error(conn);
  }
  //
}


/*Comprovar si hay suficiente Agua en Tanque y Cisterna 
y llenar Tanque si se cumple la condicion  ON:  NBT + !NMC
Apagar bomba cuando se cumple la condicion OFF: !NAT + NBC
*/
int llenar_tanque(int mode){
  int nat, nbc;
      
  muX(_ch3);
  nat = digitalRead(_mux2_SIG);
  muX(_ch6);
  nbc = digitalRead(_mux2_SIG);

  if(nat && !nbc){
    printf("Llenar Tanque...\n");
    printf("Encender Bomba.\n");
    digitalWrite(_r_IN8, LOW);    //Endender Bomba
    switch(mode){
      case 1://llenar hasta tope
          while(nat || nbc){
            muX(_ch3);
            nat = digitalRead(_mux2_SIG);
            muX(_ch6);
            nbc = digitalRead(_mux2_SIG); 
          }
          printf("Apagar Bomba.\n");
          digitalWrite(_r_IN8, HIGH);    //Apagar Bomba
          break;
      case 2:
          sleep(2);
          printf("Apagar Bomba.\n");
          digitalWrite(_r_IN8, HIGH);    //Apagar Bomba
          break;
        }
        return 0;
  }
  else{
    printf("ALARMA!! No hay agua en la cisterna o el tanque esta lleno\n");
    return -1;
  }
}


void pHmodify(int mode){
  switch(mode){
    case 1:     //intro pH-
          digitalWrite(_pH_minus, LOW);  //Activar Bomba
          sleep(2);
          digitalWrite(_pH_minus, HIGH);  //Apagar Bomba
          break;
    case -1:  //intro pH+
          digitalWrite(_pH_plus, LOW);  //Activar Bomba
          sleep(2);
          digitalWrite(_pH_plus, HIGH);  //Apagar Bomba
          break;
  }

}



/*Esta funcion calcula la dosis segun la 
concentracion ml/lt, la capacidad total del tanque
y la ultima medicion de EC, la dosis calculada en ml 
se traduce en tiempo de trabajo para bomba
*/
int food_Calc( const struct readings rd, const struct c_conf or){
  //las dosificadoras transportan 30ml/72s => 0.4ml/s
  
  float mm;     //la media entre el ec_min y ec_max;
  float actualdose;
  int time2run;


  mm = (or.ec_max + or.ec_min) / 2 ;
  
//calcudo de dosis  3/4 ((dosis_nominal * ECactual)/ EC optimo)
  if(rd.ec>0)
    actualdose = (3* (or.dosis * rd.ec)/ mm) / 2 ;
  else      // en caso que EC sea 0 calculamos en base de un EC despreciable
    actualdose = or.dosis; 

  //calculo de tiempo que se activara la bomaba
  // dosis * capacidad tanque * capacidad de carga de bomba 
  time2run = (int) round(actualdose*or.cap_tanq*_carga_ml);

  return time2run;
}


//echar comida...
void addFood(int time2run){
  digitalWrite(_nutr_1,LOW);     //activar bomba nutriente
  sleep(time2run);
  digitalWrite(_nutr_1,HIGH);     //desactivar bomba nutriente
}