/*******************************************************************************
* Practica de Sistemas Operativos II 2011 UNED
*
* Nombre: practica1
* Ejecutable: bin/mask_sim
* Alumno: Jose Manuel Morato Escandell
* Version: 1.0.0
*
*******************************************************************************/
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

void ImprimirMascara(mode_t st_mode);

/*******************************************************************************
 * Entrada principal al programa.
 * Argumentos: 
 *	argc -- Numero de parametros con el que se ha llamado al programa
 *  argv -- Array de cadenas que contiene los parametros de entrada al programa
 ******************************************************************************/
int main(int argc, char **argv)
{
    struct stat fileStat;
    
    if(argc == 2)
    {
		if(lstat(argv[1], &fileStat) != -1)
			ImprimirMascara(fileStat.st_mode);
		else
			perror(argv[1]);
    }
    else if(argc > 2)
    {
		int i = 1;
		while(i < argc)
		{
			if(lstat(argv[i], &fileStat) != -1)
			{
				printf("%s: ", argv[i]);
				ImprimirMascara(fileStat.st_mode);
			}
			else { perror(argv[i]); }
			
			i++;
		}    
    }
    else
    {
        printf("Error: Numero invalido de argumentos.\n");
        printf("Uso: mask_sim archivo1 [archivo2] [archivo3]...\n");
    }

    return 0;
}

/*******************************************************************************
 * Imprime la mascara equivalente al entero
 * que se le pasa como parametro
 * Argumentos: 
 *	st_mode -- Entero que representa la mascara de un archivo
 ******************************************************************************/
void ImprimirMascara(mode_t st_mode)
{
	if(S_ISDIR(st_mode))
		printf("d");
    else if(S_ISLNK(st_mode))
		printf("l");
    else if(S_ISCHR(st_mode))
		printf("c");
    else if(S_ISBLK(st_mode))
		printf("b");
    else if(S_ISSOCK(st_mode))
		printf("s");
    else if(S_ISFIFO(st_mode))
		printf("p");
	else
		printf("-");
	
	printf( (st_mode & S_IRUSR) ? "r" : "-");
    printf( (st_mode & S_IWUSR) ? "w" : "-");
    printf( (st_mode & S_IXUSR) ? "x" : "-");
    printf( (st_mode & S_IRGRP) ? "r" : "-");
    printf( (st_mode & S_IWGRP) ? "w" : "-");
    printf( (st_mode & S_IXGRP) ? "x" : "-");
    printf( (st_mode & S_IROTH) ? "r" : "-");
    printf( (st_mode & S_IWOTH) ? "w" : "-");
    printf( (st_mode & S_IXOTH) ? "x" : "-"); 
	
    printf("\n");
    return;
}
