#ifndef _COMUN_
#define _COMUN_

#define PEDIR_DATOS 1
#define INSERTAR_DATO 2
#define FIN_DE_COMUNICACION 3
#define ERROR 4

#define FICHERO ".colas"
#define CLAVE_CCS 'C'
#define CLAVE_CSC 'S'

struct alumno
{
	int alumnoID;
	char nombre[50];
	char apellido1[50];
	char apellido2[50];
	float nota;
};

struct mensaje
{
	long pid;
	int orden;
	struct alumno datos;
};

#define LON_MENSAJE  sizeof(int) + sizeof(struct alumno)

#endif 