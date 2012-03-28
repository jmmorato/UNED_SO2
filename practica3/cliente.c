/************************************************************
 * Practica de Sistemas Operativos II 2011 UNED
 *
 * Nombre: practica3
 * Ejecutable: bin/cliente
 * Alumno: Jose Manuel Morato Escandell
 * Version: 1.0.0
 *
 *************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "comun.h"

void ImprimirMenu();
void RecivirEntrada();
int Conectar();
void InsertarDatos();
void PedirDatos();
void CerrarServidor();
void ProcesarMensajes(int ordenEnviada);

int msqIdCCS;
int msqIdCSC;
key_t llaveCCS;
key_t llaveCSC;

char entrada[512];
long currentPid;

/*******************************************************************************
 * Entrada principal al programa.
 * Argumentos: 
 *	argc -- Numero de parametros con el que se ha llamado al programa
 *  argv -- Array de cadenas que contiene los parametros de entrada al programa
 ******************************************************************************/
main(int argc, char **argv)
{
    //Inicializamos los tokens y los identificador de las colas.
    llaveCCS =(key_t) - 1;
    llaveCSC =(key_t) - 1;
    msqIdCCS = -1;
    msqIdCSC = -1;        
    
    //Guardamos el pid del proceso actual.
    currentPid = getpid();
    
    //Imprimimos el mensaje de bienvenida.
    ImprimirMenu();
    
    //Bucle de recepcion de comandos.
    RecivirEntrada();
    while(strcmp(entrada, "s") != 0)
    {
        if(strcmp(entrada, "i") == 0) {            
            InsertarDatos();
        } else if (strcmp(entrada, "p") == 0) {
            PedirDatos();
        } else if (strcmp(entrada, "c") == 0) {
            CerrarServidor();
        } else {
            printf("\nOpcion incorrecta.\n");
        }
        RecivirEntrada();
    }
    
    //Cerramos el cliente
    printf("\nCliente finalizado\n");
    exit(0);
}

/*******************************************************************************
 * Imprime el mensaje de bienvenida.
 ******************************************************************************/
void ImprimirMenu() 
{
    printf("**************OPCIONES DISPONIBLES****************\n");
    printf("\t- Insertar datos\t\t(Pulsar i)\n");
    printf("\t- Pedir datos al servidor\t(Pulsar p)\n");
    printf("\t- Cerrar el servidor\t\t(Pulsar c)\n");
    printf("\t- Salir de esta aplicación\t(Pulsar s)\n");
    printf("**************************************************\n");
}

/*******************************************************************************
 * Imprime el mensaje de recepcion de comandos y espera por la entrada 
 * del usuario.
 ******************************************************************************/
void RecivirEntrada() 
{
    printf("\nPulse una opción (i, p, c o s):\n");
    scanf("%s", entrada);
}

/*******************************************************************************
 * Conecta a las colas de mensajes.
 * Devuelve 1 si la conexion se realiza satisfactoriamente y 0 
 * en caso contrario.
 ******************************************************************************/
int Conectar()
{            
    if(msqIdCCS == -1 || msqIdCSC == -1)
    {
        if (llaveCCS == (key_t) - 1) 
        {
            //Creamos el token para la cola cliente-servidor.
            llaveCCS = ftok(FICHERO, CLAVE_CCS);
            if(llaveCCS == (key_t) - 1)
                return 0;
        }
        
        if(llaveCSC == (key_t) - 1)
        {
            //Creamos el token para la cola servidor-cliente.
            llaveCSC = ftok(FICHERO, CLAVE_CSC);    
            if(llaveCSC == (key_t) - 1)
                return 0;
        }
        
        msqIdCCS = msgget(llaveCCS, 0666);
        msqIdCSC = msgget(llaveCSC, 0666);    
        if (msqIdCCS != -1 && msqIdCSC != -1)
            return 1;
        else
            return 0;
    }
    
    return 1;
}

/*******************************************************************************
 * Envia la orden INSERTAR_DATOS al servidor.
 ******************************************************************************/
void InsertarDatos()
{
            
    if(!Conectar())
    {
        printf("\nNo se puede conectar con el servidor.\n");
        return;
    }    
    
    struct mensaje msgInsertar;
    msgInsertar.pid = currentPid;
    msgInsertar.orden = INSERTAR_DATO;
    
    printf("\n***Insertar registro***\n");
    printf("Identificador: ");
    scanf("%d", &msgInsertar.datos.alumnoID);
    printf("Nombre: ");
    scanf("%s", msgInsertar.datos.nombre);
    printf("Apellido1: ");
    scanf("%s", msgInsertar.datos.apellido1);
    printf("Apellido2: ");
    scanf("%s", msgInsertar.datos.apellido2);
    printf("Nota: ");
    scanf("%f", &msgInsertar.datos.nota);
    
    if (msgsnd(msqIdCCS, &msgInsertar, LON_MENSAJE, 0) == -1)
    {
        perror("\nCliente");
        return;
    }
    
    ProcesarMensajes(INSERTAR_DATO);
}

/*******************************************************************************
 * Envia la orden PEDIR_DATOS al servidor.
 ******************************************************************************/
void PedirDatos()
{
    if(!Conectar())
    {
        printf("\nNo se puede conectar con el servidor.\n");
        return;
    }
        
    struct mensaje msgPedir;
    msgPedir.pid = currentPid;
    msgPedir.orden = PEDIR_DATOS;
    if (msgsnd(msqIdCCS, &msgPedir, LON_MENSAJE, 0) == -1)
    {
        perror("\nCliente");
        return;
    }
    
    ProcesarMensajes(PEDIR_DATOS);
}

/*******************************************************************************
 * Envia la orden FIN_DE_COMUNICACION al servidor que provoca el cierre 
 * de las colas de mensajes y el proceso del servidor.
 ******************************************************************************/
void CerrarServidor()
{            
    if(!Conectar())
    {
        printf("\nNo se puede conectar con el servidor.\n");
        return;
    }
    
    struct mensaje msgCerrar;
    msgCerrar.pid = currentPid;
    msgCerrar.orden = FIN_DE_COMUNICACION;
    if (msgsnd(msqIdCCS, &msgCerrar, LON_MENSAJE, 0) == -1)
    {
        perror("\nCliente");
        return;
    }
    
    msqIdCCS = -1;
    msqIdCSC = -1;
    llaveCCS =(key_t) - 1;
    llaveCSC =(key_t) - 1;
    
    printf("\nServidor cerrado\n");
}

/*******************************************************************************
 * Procesa los mensajes recibidos desde el servidor como respuesta a un mensaje
 * enviado previamente.
 * Argumentos:
 *   orderEnviada: la orden enviada que provocó la llamada a la función
 ******************************************************************************/
void ProcesarMensajes(int ordenEnviada)
{
    struct mensaje mensajeRcv;
    
    while(1)
    {
        if(msgrcv(msqIdCSC, &mensajeRcv, LON_MENSAJE, currentPid, 0) == -1)
        {
            perror("\nCliente");
            return;
        }
        
        switch(mensajeRcv.orden)
        {
            case PEDIR_DATOS:
                printf("\t%d\t%s\t%s\t%s\t%2.2f\n", 
                       mensajeRcv.datos.alumnoID,
                       mensajeRcv.datos.nombre,
                       mensajeRcv.datos.apellido1,
                       mensajeRcv.datos.apellido2,
                       mensajeRcv.datos.nota);
                break;
            case INSERTAR_DATO:
                printf("\nRegistro de datos insertado con éxito\n");
                break;
            case FIN_DE_COMUNICACION:
                if(ordenEnviada == PEDIR_DATOS)
                    printf("\tFin de la operación\n");
                else
                    printf("Fin de la operación\n");
                return;
            case ERROR:
                if(ordenEnviada != PEDIR_DATOS && ordenEnviada != INSERTAR_DATO)
                    printf("Error de comunicación\n");
                return;
            default:
                printf("Mensaje desconocido\n");
                return;
        }
    }
}

