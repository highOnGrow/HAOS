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




void main(){

    time_t ltime;
    struct tm *Tm;
    int timebuffer[255];

    ltime=time(NULL);
    Tm=localtime(&ltime);


    timebuffer[0]  = Tm->tm_mday;
    timebuffer[1]  = '/';
    timebuffer[2]  = Tm->tm_mon+1;
    timebuffer[3]  = '/';
    timebuffer[4]  = Tm->tm_year+1900;
    timebuffer[5]  = ' ';
    timebuffer[6]  = Tm->tm_hour;
    timebuffer[7]  = ':';
    timebuffer[8]  = Tm->tm_min;
    timebuffer[9]  = ':';
    timebuffer[10] = Tm->tm_sec;
    timebuffer[11]  = '\t';

    printf("[%d] %d %d %d, %d:%d:%d\n",
            Tm->tm_wday, /* Mon - Sun */
            Tm->tm_mday,
            Tm->tm_mon+1,
            Tm->tm_year+1900,
            Tm->tm_hour,
            Tm->tm_min,
            Tm->tm_sec);

 /*   strcat(timebuffer,(char*)Tm->tm_mday);
    strcat(timebuffer,(char*)Tm->tm_mon);
    strcat(timebuffer,(char*)Tm->tm_year);
    strcat(timebuffer,(char*)Tm->tm_hour);
    strcat(timebuffer,(char*)Tm->tm_min);
    strcat(timebuffer,(char*)Tm->tm_sec);
 
 	printf("Date: [%s]\n", timebuffer);*/

    printf("Date:[ %d%c%d%c%d%c%d%s%d%s%d Done..\n]", 
    		timebuffer[0], timebuffer[2],
    		timebuffer[3], timebuffer[4],
    		timebuffer[5], timebuffer[6],
    		timebuffer[7], timebuffer[8],
    		timebuffer[9], timebuffer[10],
    		timebuffer[11], timebuffer[12]);
}


/*
char timestamp[12];



char * timestamp(char* timebuffer)
{
    time_t ltime;
    struct tm *Tm;

    ltime=time(NULL);
    Tm=localtime(&ltime);
  

  	timebuffer[0]  = Tm->tm_mday;
  	timebuffer[1]  = '/';
  	timebuffer[2]  = Tm->tm_mon;
  	timebuffer[3]  = '/';
  	timebuffer[4]  = Tm->tm_year+1900;
  	timebuffer[5]  = ' ';
  	timebuffer[6]  = Tm->tm_hour;
  	timebuffer[7]  = ':';
  	timebuffer[8]  = Tm->tm_min;
  	timebuffer[9]  = ':';
  	timebuffer[10] = Tm->tm_sec;
  	timebuffer[11]  = '\t';
  
 return timebuffer;
}*/