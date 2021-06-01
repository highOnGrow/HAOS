#include <signal.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>


static jmp_buf jb;
sig_atomic_t sig_count = 0;



void ouch ( int signo )
{
	if (sig_count<3)
		printf ( " ouch ! recibi signal % d \n " , signo, sig_count++);
	else{
		printf ( " ouch ! going out\n" );
		longjmp(jb,1);
	}
}




int main ()
{
	pid_t pid ;
	int fd, fl, conn;
	/* registro de ignorar interrupcion : CTRL + C */
	signal ( SIGINT , ouch) ;
	pid = getpid () ;

if (!setjmp(jb)){
	printf("Set jump\n");
	while (1)
	{
		printf ( " PID : %d Si no me deticdsenes volvere a escribir esto !!!\n " , pid ) ;
		sleep (1) ;
	}
}
	else{
	fd = fl = conn = 1;
	printf("%d, %d, %d\n", fd, fl, conn);

	return 0 ; // Nunca se llamara a exit
}
}
