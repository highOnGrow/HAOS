#include "control_inv.h"


void _calibrate_atlas_(int);


int main (){


  int fd ;                             //Serial Port Descriptor
  char data[256];

  if ((fd = serialOpen ("/dev/ttyAMA0", BAUDRATE)) < 0){
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno));
    return 1 ;
  }

 	wiringPiSetupSys();
 //	_calibrate_atlas_( fd );

while(1){
	muX(_y0);
 	Communicateur(fd, "R\r", data);
  	printf("EC: %s\n", data);

  	muX(_y3);
 	Communicateur(fd, "R\r", data);
  	printf("pH: %s\n", data);


}

  close(fd);
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




void _calibrate_atlas_(int fd){
  char data[1024];

  //Y0 - EC Sensor
  muX(_y0);

/*
  Communicateur(fd, "C,1\r", data);
  printf("Enable Default Reading Mode: %s\n", data);


  Communicateur(fd, "Cal,?\r", data);
  printf("Calibration Done: %s\n", data);

  sleep(2);

  Communicateur(fd, "Cal,dry\r", data);
  printf("Dry Calibration Done: %s\n", data);

	sleep(2);

  int fs;
  printf("Please insert probe to 12.880 solucion:\n");
  scanf("%d",&fs);
  Communicateur(fd, "Cal,one,12.880\r", data);
  printf("One point Calibration Done: %s\n", data);
*/  
  	sleep(2);

// printf("Calibration Done: %s\n", data);


  Communicateur(fd, "Cal,?\r", data);
  printf("Calibration Done: %s\n", data);


  	sleep(2);
  Communicateur(fd, "Cal,?\r", data);
  printf("Calibration Done: %s\n", data);

  //Y3 - pH Sensor
  muX(_y3);

/*
  Communicateur(fd, "C,1\r", data);
  printf("Enable Default Reading Mode: %s\n", data);

  sleep(3);


  Communicateur(fd, "Cal, mid, 7.00\r", data);
  printf("One point Calibration Done: %s\n", data);
*/
 sleep(3);

  Communicateur(fd,"Cal,?\r", data);
  printf("EC:Calibrations Done: %s\n", data);


}