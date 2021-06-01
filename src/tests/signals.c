#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>





//sig_atomic_t sigusr1_count = 0;


void handler (int signal_number)
{
	printf("Se detecto CTRL+C \n");
}


int main ()
{

	pid_t pid ;
	struct sigaction sa;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &handler;
	/* registro de manenar interrupcion : CTRL + C */
	sigaction (SIGINT, &sa, NULL);
	pid = getpid () ;

	
	while (1)
	{
		printf ( " PID : %d Si no me detienes volvere a escribir esto !!!\n " , pid ) ;
		sleep (1) ;
	}

return 0;
}

