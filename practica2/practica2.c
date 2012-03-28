/************************************************************
* Practica de Sistemas Operativos II 2011 UNED
*
* Nombre: practica2
* Ejecutable: bin/creasin
* Alumno: Jose Manuel Morato Escandell
*
*************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int n;
int pid;
int pid_padre;
int pid_hijo;
int pid_patriarca;
int id = 0;
int contador = 3;

void Manejador_SIGUSR1_1 (int sig);
void Manejador_SIGUSR1_2 (int sig);
void Manejador_SIGTERM_1 (int sig);
void Manejador_SIGTERM_2 (int sig);

/*******************************************************************************
 * Entrada principal al programa.
 * Argumentos: 
 *	argc -- Numero de parametros con el que se ha llamado al programa
 *  argv -- Array de cadenas que contiene los parametros de entrada al programa
 ******************************************************************************/
main (int argc, char **argv) {
	
	if (argc != 2) {
		printf("Error: Numero invalido de argumentos.\n");
        printf("Uso: creasin n (donde n es un entero 1 <= n <= 15)\n");
		exit(-1);
	}
	
	n = atoi(argv[1]);	
	if (n < 1 || n > 15) {
		printf("Error: Argumento fuera de rango.\n");
        printf("Uso: creasin n (donde n es un entero 1 <= n <= 15)\n");
		exit(-1);
	}
	
    pid_patriarca = getpid();
    
	if (signal(SIGUSR1, Manejador_SIGUSR1_1) == SIG_ERR ||
		signal(SIGTERM, Manejador_SIGTERM_1) == SIG_ERR) 
	{
		perror("Error");
		exit(-1);
	}
	
	printf("*** PARTE A: Creación de procesos ***\n");
    int i = 0;
	while (i < n && i == id) 
    {
		if ((pid_hijo=fork()) == -1) {
            perror("Error");
            exit(-1);
		}
		else if (pid_hijo == 0) {
			id++;
			pid = getpid();
			pid_padre = getppid();
			printf("Creado proceso hijo no %d (pid= %d, pid_proceso_padre= %d)\n", id, pid, pid_padre);			
		}
		i++;		
	}
    
    if (id == n) {
        sleep(1);
        printf("\n*** PARTE B: Creación de un punto de sincronismo ***\n");
        raise(SIGUSR1);        
    }
    
	while (1) { }
}

/*******************************************************************************
 * Manejador principal para la senyal SIGUSR1.
 * Argumentos: 
 *	sig -- Senyal recibida
 ******************************************************************************/
void Manejador_SIGUSR1_1 (int sig) {

	if (signal(SIGUSR1, SIG_IGN) == SIG_ERR) {
		perror("Error");
		exit(-1);
	}
	
	sleep(1);
	if (id != 0) {
		printf("Proceso hijo no %d (pid= %d) preparado\n", id, pid);
		kill(pid_padre, SIGUSR1);
        if (signal(SIGUSR1, Manejador_SIGUSR1_2) == SIG_ERR) {
            perror("Error");
            exit(-1);
        }
	} else {
		printf("Proceso patriarca (pid= %d) preparado\n", pid);
		printf("\n*** PARTE C: Comunicación y sincronismo ***\n");
        if (signal(SIGUSR1, Manejador_SIGUSR1_2) == SIG_ERR) {
            perror("Error");
            exit(-1);
        }
        raise(SIGUSR1);
	}
    
    

}

/*******************************************************************************
 * Manejador secundario para la senyal SIGUSR1.
 * Argumentos: 
 *	sig -- Senyal recibida
 ******************************************************************************/
void Manejador_SIGUSR1_2 (int sig) {
	
    if (signal(SIGUSR1, SIG_IGN) == SIG_ERR) {
		perror("Error");
		exit(-1);
	}
    
    sleep(1);
    
    if (contador > 0){
        if (id == 0)
            printf("Proceso patriarca. Contador=%d\n", contador);
        else
            printf("Proceso hijo no %d\n", id);
        
        if (id != n)
            kill(pid_hijo, SIGUSR1);
        else
            kill(pid_patriarca, SIGUSR1);
    } else if (contador == 0) {
        printf("Proceso patriarca. Contador=0\n");
        printf("\n*** PARTE D: Secuencia de terminación ***\n");
        raise(SIGTERM);
    }
    
    contador--;
    
    if (signal(SIGUSR1, Manejador_SIGUSR1_2) == SIG_ERR) {
		perror("Error");
		exit(-1);
	}
	
}

/*******************************************************************************
 * Manejador principal para la senyal SIGTERM.
 * Argumentos: 
 *	sig -- Senyal recibida
 ******************************************************************************/
void Manejador_SIGTERM_1 (int sig) {
    
	if (signal(SIGTERM, SIG_IGN) == SIG_ERR) {
		perror("Error");
		exit(-1);
	}
		
	sleep(1);
    
    if (id == 0) {        
        printf("Patriarca envía la señal SIGTERM al Hijo no 1.\n");        
        kill(pid_hijo, SIGTERM);
        if (signal(SIGTERM, Manejador_SIGTERM_2) == SIG_ERR) {
            perror("Error");
            exit(1);
        }
    } else if (id != n) {
		printf("Hijo no %d recibe y envía la señal SIGTERM al Hijo no %d.\n", id, id + 1);
		kill(pid_hijo, SIGTERM);
        if (signal(SIGTERM, Manejador_SIGTERM_2) == SIG_ERR) {
            perror("Error");
            exit(-1);
        }
	} else {
		printf("Hijo no %d recibe la señal SIGTERM.\n\n", id);
        if (signal(SIGTERM, Manejador_SIGTERM_2) == SIG_ERR) {
            perror("Error");
            exit(-1);
        }
		raise(SIGTERM);
	}
}

/*******************************************************************************
 * Manejador secundario para la senyal SIGTERM.
 * Argumentos: 
 *	sig -- Senyal recibida
 ******************************************************************************/
void Manejador_SIGTERM_2 (int sig) {
	
    if (signal(SIGTERM, SIG_IGN) == SIG_ERR) {
		perror("Error");
		exit(-1);
	}
    
	sleep(1);
	if (id != 0) {
		printf("Proceso hijo no %d terminado.\n", id);
		kill(pid_padre, SIGTERM);
	} else {
		printf("Proceso patriarca terminado.\n");
	}

	exit(0);
	
}


