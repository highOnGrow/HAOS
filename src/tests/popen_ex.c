#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char *argv[] )
{

  FILE *fp;
  char path[1035];
  char delimiters[] = " ,=";
  int  i=0;
  float temp;
  float hum;

  while(i!=-1){
    /* Open the command for reading. */
    fp = popen("sudo ../files/DHT 2302 24", "r");
    if (fp == NULL) {
      printf("Failed to run command\n" );
      exit(1);
    }

    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof(path)-1, fp) != NULL);

    char * pch;
    pch = strtok (path,delimiters);

      while (pch != NULL)
      {
        if(!i){
          if(!strcmp(pch,"Temp")){
           // printf ("%s\n",pch);
            pch = strtok (NULL, delimiters);
            temp = strtof(pch,NULL);
            printf("Temperatura: \t%.1f\n", temp);
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
              hum = strtof(pch,NULL);
              printf("Humedad: \t%.1f\n", hum);
              i = -1;
              break;
            }
            else{
          //    printf ("%s\n",pch);
              pch = strtok (NULL, delimiters);
              i++;
            }
        }
      }
  }
  

  /* close */
  pclose(fp);

  return 0;
}
