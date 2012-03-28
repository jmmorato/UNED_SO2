/************************************************************
 * Practica de Sistemas Operativos II 2011 UNED
 *
 * Nombre: practica3
 * Ejecutable: bin/servidor
 * Alumno: Jose Manuel Morato Escandell
 * Version: 1.0.0
 *
 *************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "comun.h"

#define DB "./clase.dat"

void CerrarServidor();
void PedirDatos(struct mensaje mensajeRcv);
void InsertarDatos(struct mensaje mensajeRcv);
void EnviarCSCMsg(long pid, int type, struct alumno datos);

int msqIdCCS;
int msqIdCSC;
key_t llaveCCS;
key_t llaveCSC;

/*******************************************************************************
 * Entrada principal al programa.
 * Argumentos: 
 *	argc -- Numero de parametros con el que se ha llamado al programa.
 *  argv -- Array de cadenas que contiene los parametros de entrada al programa.
 ******************************************************************************/
main(int argc, char **argv)
{
    //Truncamos, o creamos si no existe, el archivo para el mecanismo IPC.
    FILE *colas;
    if((colas = fopen(FICHERO, "w")) == NULL)
    {    
        printf("\nServidor: No se puede crear el archivo para las colas de mensajes.\n");
        exit(-1);
    }
    fclose(colas);
    
    //Creamos el token para la cola cliente-servidor.
    llaveCCS = ftok(FICHERO, CLAVE_CCS);
    if(llaveCCS == (key_t) - 1)
    {
        perror("\nServidor");
        exit(-1);
    }
    
    //Creamos el token para la cola servidor-cliente.
    llaveCSC = ftok(FICHERO, CLAVE_CSC);    
    if(llaveCSC == (key_t) - 1)
    {
        perror("\nServidor");
        exit(-1);
    }
    
    //Obtenemos, o creamos si no existe, la cola cliente-servidor.
    msqIdCCS = msgget(llaveCCS, IPC_CREAT | 0666);
    if(msqIdCCS == -1)
    {
        perror("\nServidor");
        exit(-1);
    }
    
    //Obtenemos, o creamos si no existe, la cola servidor-cliente.
    msqIdCSC = msgget(llaveCSC, IPC_CREAT | 0666);
    if(msqIdCSC == -1)
    {
        perror("\nServidor");
        exit(-1);
    }
    
    //Bucle de recepcion y tratamiento de mensajes.
    while(1)
    {
        struct mensaje mensajeRcv;
        if(msgrcv(msqIdCCS, &mensajeRcv, LON_MENSAJE, 0, 0) == -1)
        {
            perror("\nServidor");
            exit(-1);
        }
        
        switch(mensajeRcv.orden)
        {
            case PEDIR_DATOS:
                PedirDatos(mensajeRcv);
                break;
            case INSERTAR_DATO:
                InsertarDatos(mensajeRcv);
                break;
            case FIN_DE_COMUNICACION:
                CerrarServidor();                
            default:
                EnviarCSCMsg(mensajeRcv.pid, ERROR, mensajeRcv.datos);
                break;
        }
    }
}

/*******************************************************************************
 * Envia los registros guardados en en clase.dat al cliente que los solicito.
 ******************************************************************************/
void PedirDatos(struct mensaje mensajeRcv) 
{
    FILE *fd;
    struct alumno datos;
    
    if((fd = fopen(DB, "r")) == NULL)
    {        
        printf("\nServidor: No puedo abrir el fichero clase.dat\n");
        EnviarCSCMsg(mensajeRcv.pid, ERROR, mensajeRcv.datos);
        return;
    }
    
    while(fread(&datos, sizeof(struct alumno), 1, fd) == 1)
        EnviarCSCMsg(mensajeRcv.pid, PEDIR_DATOS, datos);
    
    fclose(fd);
    
    EnviarCSCMsg(mensajeRcv.pid, FIN_DE_COMUNICACION, mensajeRcv.datos);
}

/*******************************************************************************
 * Inserta un nuevo alumno en el archivo clase.dat
 ******************************************************************************/
void InsertarDatos(struct mensaje mensajeRcv) 
{
    FILE *fd;
    if((fd = fopen(DB, "a")) == NULL)
    {
        printf("\nServidor: No puedo abrir o crear el fichero clase.dat\n");
        EnviarCSCMsg(mensajeRcv.pid, ERROR, mensajeRcv.datos);
        return;
    }
    
    fwrite(&mensajeRcv.datos, sizeof(struct alumno), 1, fd);
    fclose(fd);

    EnviarCSCMsg(mensajeRcv.pid, INSERTAR_DATO, mensajeRcv.datos);
    EnviarCSCMsg(mensajeRcv.pid, FIN_DE_COMUNICACION, mensajeRcv.datos);
}

/*******************************************************************************
 * Borra las colas de mensajes y termina el proceso servidor.
 ******************************************************************************/
void CerrarServidor() 
{
    msgctl(msqIdCCS, IPC_RMID, 0);
    msgctl(msqIdCSC, IPC_RMID, 0);
    
    exit(0);
}

/*******************************************************************************
 * Envia un nuevo mensaje a la cola sevidor-cliente
 * Argumentos:
 *   pid: identificador del proceso que debe recibir el mensaje
 *   type: tipo de mensaje a enviar
 *   datos: estructura de datos del alumno
 ******************************************************************************/
void EnviarCSCMsg(long pid, int type, struct alumno datos)
{
    struct mensaje msgCSCMsg;
    msgCSCMsg.pid = pid;
    msgCSCMsg.orden = type;
    msgCSCMsg.datos = datos;

    if (msgsnd(msqIdCSC, &msgCSCMsg, LON_MENSAJE, 0) == -1)
        perror("\nServidor");
}