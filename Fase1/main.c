/*EDER YAFETH GACIA QUIROA - 201212621*/
/*Manejo E Implementacion De Archvios- Vacaciones Diciembre 2016*/
/*FASE 1*/

#include "Discos.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>

char *Nombre, *Direccion,*Usuario,*Password,*Grupo,*DirecContenido;
char *parak, *param, *parah, *parai, *paran;
bool Nombre_Direccion, Name;
int Usuario_Pasw_Grup;

//********************************* METODO PARA LEER ARCHIVO SCRIPT*********************************************
//**************************************************************************************************************
 void LeerArchivo(char *Path)
 {
     char Linea[200];
     char Siguiente[200];
     FILE *Archivo;
     int FinArchivo=0;

     Archivo = fopen(Path,"r");

     if(Archivo==NULL)
     {
         printf(">>>> ERROR: ruta de fichero no encontrada <<<<\n");
         return;
     }else
     {
        FinArchivo =  fscanf(Archivo," %[^\n]",&Linea);

        while(FinArchivo != EOF )
        {

            if(*Linea=='#'){
                printf("------------->> Comentario << ------------\n");
                printf("%s\n",Linea);
                FinArchivo = fscanf(Archivo," %[^\n]",&Linea);}
            else{

                if(Linea[strlen(Linea)-1]==' '){
                Linea[strlen(Linea)-1] = '\0';
                }

                if(Linea[strlen(Linea)-1]=='\^'){
                    Linea[strlen(Linea)-1] = '\0';
                    FinArchivo = fscanf(Archivo," %[^\n]",&Siguiente);
                    strcat(Linea,Siguiente);
                    printf("------------->> Comando << ------------\n");
                    printf("%s\n",Linea);
                    Analizador(Linea);
                    FinArchivo = fscanf(Archivo," %[^\n]",&Linea);
                    }
                else{
                    printf("------------->> Comando << ------------\n");
                    printf("%s\n",Linea);
                    Analizador(Linea);
                    FinArchivo = fscanf(Archivo," %[^\n]",&Linea);
                    }
                }
        }
     }
     fclose(Archivo);
 }

 //******************** METODO PARA LEER EL ARCHIVO QUE SE CARGARA A UN BLOQUE DE ARCHIVOS *************************************************************************************************************
 //**************************************************************************************************************************************************************************************************************
 char *LeerArchivoContenido(char *Direcc, int Tamano)
 {
     FILE *Archivo;
     char Datos[400];
     char *DatoEnviar;
     int Indice;

     Archivo = fopen(Direcc,"r");

     if(Archivo==NULL)
     {
         printf(">>>>ERROR: la ruta del contenido no fue encontrada<<<<\n");
         return NULL;
     }

     Indice=0;

     if(Tamano==-1) //no trae especificado un tamano
     {
        while(feof(Archivo)==0)
        {
             Datos[Indice] = getc(Archivo);
             Indice++;
        }
        Indice--;
        Indice--;
     }
     else //Si se trae especificado un parametro
     {
         while((feof(Archivo)==0) && (Indice < Tamano))
        {
             Datos[Indice] = getc(Archivo);
             Indice++;
        }

        if(feof(Archivo)!=0)
        {
            Indice--;
            Indice--;
        }
     }

     Datos[Indice] = '\0';

     fclose(Archivo);
     DatoEnviar = malloc(strlen(Datos));
     strcpy(DatoEnviar,Datos);

     return DatoEnviar;
 }

//****************************METODO PARA DEVOLVER LA PALABRA DEL PARAMETRO*************************************
//**************************************************************************************************************
char * DevolverContenidoParametro(char *Cadena,int Indice)
{
    int Contador;
    Contador =0;

        while(Contador!=Indice)
        {
            Cadena++;
            Contador++;
        }
    return Cadena;
}

//********************************* METODO PARA RECONOCER UN PARAMETRO*********************************************
//**************************************************************************************************************
char * DevolverTipoParametro(char *Cadena)
{
    Cadena++;
    char *Palabra;
    int i;

    while(sscanf(Cadena,"%150[^-]%n",Palabra,&i)==1)
    {
        Cadena+=i;
        if(*Cadena!= "->")
            break;
        while(*Cadena== "->")
                ++Cadena;
    }
    return Palabra;
}

//*****************************METODO PARA RECONOCER El INICIO DEL PARAMETRO************************************
//**************************************************************************************************************
bool Verificar_inicio(char *Cadena)
{
    bool Estado;

    if(*Cadena=='&')
        Estado = true;
    else if(*Cadena=='%')
        Estado = true;
        else if(*Cadena=='#')
        Estado = true;
    else
        Estado = false;

    return Estado;
}

//****************************METODO PARA RECONOCER EL VALOR DEL PARAMETRO SIZE*******************************
//**************************************************************************************************************
bool VerificarSize(char *Parametro)
{
    char *AuxiliarCad;
    bool Est;
    Est =false;

    AuxiliarCad = Parametro;

    //Validando que el dato sea un numero
    while((*AuxiliarCad!='\0') && Est == false )
    {
        if(isdigit(*AuxiliarCad))
            Est = false;
        else
            Est = true;

        AuxiliarCad++;
    }
    return Est;
}

//*******************************METODO PARA LEER EL VALOR DEL PARAMETRO TYPE***********************************
//**************************************************************************************************************
bool VerificarType(char *Parametro)
{
    if((strlen(Parametro))==1)
        {
            if((strcasecmp(Parametro,"p")==0)|| (strcasecmp(Parametro,"e")==0) || (strcasecmp(Parametro,"l")==0) )
                return true;

            return false;
        }
        return false;
}

//******************************METODO PARA LEER EL VALOR DEL PARAMETRO UNIT************************************
//**************************************************************************************************************
bool VerificarUnit(char *Parametro,int Tipo)
{
    //Tipo para identificar que tipo de unit es: Tipo=1 acepta valores de k y m Tipo =2 acepta valores de k,m y b
    if(Tipo==1)
    {
        if((strlen(Parametro))==1)
        {
            if((strcasecmp(Parametro,"k")==0) || (strcasecmp(Parametro,"m")==0) )
                return true;

            return false;
        }
        return false;
    }
else
    {
        if((strlen(Parametro))==1)
        {
            if((strcasecmp(Parametro,"k")==0) || (strcasecmp(Parametro,"m")==0) || (strcasecmp(Parametro,"b")==0))
                return true;

            return false;
        }
        return false;
    }
}

//****************************METODO PARA LEER EL VALOR DEL PARAMETRO ADD***************************************
//**************************************************************************************************************
bool VerificarAdd(char *Parametro)
{
    bool Est;
    Est = true;

    if(*Parametro=='-')
    {
        Parametro++;
        //Validando que el dato sea un numero
        while((*Parametro!='\0') && Est == true )
        {
            if(isdigit(*Parametro))
                Est = true;
            else
                Est = false;

            Parametro++;
        }
        return Est;
    }else
    {
        //Validando que el dato sea un numero
        while((*Parametro!='\0') && Est == true )
        {
            if(isdigit(*Parametro))
                Est = true;
            else
                Est = false;

            Parametro++;
        }
        return Est;
    }
}

//*****************************METODO PARA LEER VALOR DE PARAMETRO DELETE***************************************
//**************************************************************************************************************
bool VerificarDelete(char *Parametro)
{
    if(((strcasecmp(Parametro,"fast"))==0) || ((strcasecmp(Parametro,"full"))==0))
        return true;

    return false;
}

//*****************************METODO PARA LEER VALOR DE PARAMETRO REP***************************************
//**************************************************************************************************************
bool VerificarRep(char *Parametro)
{
    if(((strcasecmp(Parametro,"mbr"))==0) || ((strcasecmp(Parametro,"disk"))==0))
        return true;

    return false;
}

//******************************METODO PARA LEER VALOR PARAMETRO FIT********************************************
//**************************************************************************************************************
bool VerificarFit(char *Parametro)
{
    if((strlen(Parametro))==2)
    {
        if(strcasecmp(Parametro,"bf")==0 || strcasecmp(Parametro,"ff")==0 || strcasecmp(Parametro,"wf")==0)
        {
            return true;
        }
        return false;
    }
    return false;
}

//*********************************METODO PARA LEER VALOR DE ID************************************************
//**************************************************************************************************************
bool VerificarId(char *Parametro)
{
    bool Est;

    Est=true;

    if(*Parametro=='v')
    {
        Parametro++;
        if(*Parametro=='d')
        {
            Parametro++;

            if(isalpha(*Parametro))
            {
                Parametro++;
                //Validando que el dato sea un numero
                 while((*Parametro!='\0') && Est == true )
                {
                    if(isdigit(*Parametro))
                        Est = true;
                    else
                        Est = false;

                    Parametro++;
                }
                return Est;
            }

            return false;
        }
        return false;
    }
    return false;
}

//*****************************METODO PARA ELIMINAR ESPACIOS ENTRE PARAMETROS***********************************
//**************************************************************************************************************
bool VerificarInicioEspacio(char *Parametro)
{
    if(*Parametro=='"')
    {
        return true;
    }else
        return false;
}

//******************************METODO PARA RECONOCER PARAMETRO COMPLETO****************************************
//**************************************************************************************************************
bool VerificarFinEspacio(char *Parametro)
{
    bool Est;
    Est = true;

    while((Est) && (*Parametro!='\0'))
    {
        if(*Parametro=='"')
            Est = false;   //Significa que la cadena aun no a terminado

        Parametro++;
    }
    return Est;
}

//************************METODO PARA QUITAR COMILLAS Y DIFERENCIAR ENTRE NOMBRE Y DIRECCION********************
//**************************************************************************************************************
void InsertarPath(char* path, int Comando)
{
    path++;
    char Palabra[100];
    int i;

    if(Nombre_Direccion){
        //Eliminando las comillas
            while(sscanf(path,"%150[^\"]%n",Palabra,&i)==1)
            {
                path+=i;
                if(*path!='"')
                    break;

                while(*path=='"')
                        ++path;
            }
        }else
            sscanf(path,"%150[^\"]%n",Palabra,&i)==1;

printf("Valor: %s\n",Palabra);

if(Comando==1)
    {
        if(Nombre_Direccion)
        {
            Nombre =(char*)malloc((strlen(Palabra)*sizeof(Palabra))+1);
            strcpy(Nombre,Palabra);
            printf("Nombre del Disco:%s\n",Nombre);
        }else
        {
            Direccion = malloc((strlen(Palabra)*sizeof(Palabra))+1);
            strcpy(Direccion,Palabra);
            printf("Path:%s\n",Direccion);
        }
    }else if(Comando==2)
    {
       Direccion = malloc((strlen(Palabra)*sizeof(Palabra))+1);
       strcpy(Direccion,Palabra);
       printf("Path:%s\n",Direccion);
    }else if(Comando==7)
    {
        Direccion = malloc((strlen(Palabra)*sizeof(Palabra))+1);
        strcpy(Direccion,Palabra);
    }else if(Comando==3)
    {
        if(Nombre_Direccion)
        {
            Nombre =(char*)malloc((strlen(Palabra)*sizeof(Palabra))+1);
            strcpy(Nombre,Palabra);
            printf("Nombre: %s\n",Nombre);
        }else
        {
            Direccion = malloc((strlen(Palabra)*sizeof(Palabra))+1);
            strcpy(Direccion,Palabra);
            printf("Path: %s\n",Direccion);
        }
    }else if(Comando==4)
    {
        if(Nombre_Direccion)
        {
            Nombre =(char*)malloc((strlen(Palabra)*sizeof(Palabra))+1);
            strcpy(Nombre,Palabra);
        }else
        {
            Direccion = malloc((strlen(Palabra)*sizeof(Palabra))+1);
            strcpy(Direccion,Palabra);
        }
    }else if(Comando==6)
    {
        if(Nombre_Direccion)
        {
            Nombre =(char*)malloc((strlen(Palabra)*sizeof(Palabra))+1);
            strcpy(Nombre,Palabra);
        }else
        {
            Direccion = malloc((strlen(Palabra)*sizeof(Palabra))+1);
            strcpy(Direccion,Palabra);

        }
    }else if(Comando==7)
    {
        Direccion = malloc((strlen(Palabra)*sizeof(Palabra))+1);
        strcpy(Direccion,Palabra);
    }
}

//********************************METODO PARA ANALIZADOR DE COMANDOS*******************************************
//*************************************************************************************************************
void Analizador(char *Cadena){
     char Palabra[100];
     char PalabraConEspacio[200];
     char *Tamano,*Unit,*Tipo,*Fitt,*Borrar,*Agregar,*Idd;
     char *Ugo,*Recursivo,*CrearCarpeta;

     int n;
     int Estado,EstadoAnterior;
     int trigger;

     Estado =0;
     trigger =0;
     EstadoAnterior = Estado;


     while(sscanf(Cadena,"%150[^ ]%n",Palabra, &n) ==1)
     {
        if(Palabra[strlen(Palabra)-1]=='\n')
                Palabra[strlen(Palabra)-1] ='\0';

         //printf("La Palabra es: %s\n",Palabra);

       while(trigger!=1)

        switch(Estado)
        {
            //Estado = 0 se reconocen el inicio de todos los comando
            case 0:
                if((strcasecmp(Palabra,"mkdisk"))==0)
                {
                    printf("Comando mkdisk reconocido\n");
                    Estado = 1;
                    Tamano =NULL;
                    Nombre = NULL;
                    Direccion=NULL;
                }else if((strcasecmp(Palabra,"rmdisk"))==0)
                {
                    printf("Comando rmdisk reconocido\n");
                    Estado = 2;
                    Direccion =NULL;
                }else if((strcasecmp(Palabra,"fdisk"))==0)
                {
                    printf("Comando fdisk reconocido\n");
                    Estado = 3;
                    Tamano=NULL;
                    Unit=NULL;
                    Direccion=NULL;
                    Tipo = NULL;
                    Fitt =NULL;
                    Borrar=NULL;
                    Nombre=NULL;
                    Agregar=NULL;
                }else if((strcasecmp(Palabra,"mount"))==0)
                {
                    printf("Comando mount reconocido\n");
                    Estado = 4;
                    Direccion=NULL;
                    Nombre=NULL;
                }else if((strcasecmp(Palabra,"unmount"))==0)
                {
                    printf("Comando unmount reconocido\n");
                    Idd=NULL;
                    Estado = 5;
                }else if((strcasecmp(Palabra,"rep"))==0)
                {
                    printf("Comando rep reconocido\n");
                    Direccion=NULL;
                    Nombre=NULL;
                    Idd=NULL;
                    DirecContenido=NULL;
                    Estado = 6;
                }else if((strcasecmp(Palabra,"exec"))==0)
                {
                    printf("Comando exec reconocido\n");
                    Estado = 7;
                    Direccion=NULL;
                }else if(strcasecmp(Palabra,"mkfs")==0)
                {
                    printf("Comando mkfs reconocido\n");
                    Idd=NULL;
                    Tipo=NULL;
                    Agregar=NULL;
                    Unit=NULL;
                    Estado=9;
                }else if(strcasecmp(Palabra,"df")==0)
                {
                    printf("Comando disk free reconocido\n");
                    parak=NULL;
                    param=NULL;
                    parah=NULL;
                    parai=NULL;
                    Estado=11;
                }else if(strcasecmp(Palabra,"du")==0)
                {
                    printf("Comando disk used reconocido\n");
                    paran=NULL;
                    parah=NULL;
                    Direccion=NULL;
                    Estado=12;
                }else if(strcasecmp(Palabra,"#")==0)
                {
                    Estado=8;
                }else
                {
                    printf(">>>> ERROR: comando no reconocido <<<<\n");
                    Estado=13;

                }
            trigger =1;
            break;

            case 1:  //Estado perteneciente al mkdisk

                if(Verificar_inicio(Palabra))
                {
                    char *Parametro = DevolverTipoParametro(Palabra);
                    char *ContenidoPar;
                    bool Est;

                    //Parametro tamaño
                    if((strcasecmp(Parametro,"size"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Palabra,7);

                        bool Est;
                        Est = VerificarSize(ContenidoPar);

                        if(Est)
                        {
                            printf(">>>> ERROR: valor de tamanio no valido: %s <<<<\n",ContenidoPar);
                            trigger =1;

                        }else
                        {

                            printf("Tamaño De Disco: %s\n",ContenidoPar);
                            Tamano =(char*)malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Tamano,ContenidoPar);
                            fflush(stdin);
                            trigger =1;
                        }

                        //Parametro name
                    }else if((strcasecmp(Parametro,"name"))==0)
                    {
                        bool Est;
                        ContenidoPar = DevolverContenidoParametro(Palabra,7);
                        printf("nombre reconocido!\n");
                        Est = VerificarInicioEspacio(ContenidoPar);

                            if(Est)
                                {
                                    EstadoAnterior = Estado;
                                    Estado = 10;
                                    Nombre_Direccion =true;
                                    strcpy(PalabraConEspacio,ContenidoPar);
                                }
                            else
                                {
                                    Nombre = (char*)malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                                    strcpy(Nombre,ContenidoPar);
                                    trigger = 1;
                                }

                        //PARAMETRO PATH
                    }else if((strcasecmp(Parametro,"path"))==0)
                    {
                        bool Est;
                        ContenidoPar = DevolverContenidoParametro(Cadena,7);
                        Est = VerificarInicioEspacio(ContenidoPar);

                        if(Est)
                        {
                            EstadoAnterior = Estado;
                            Estado = 10;
                            Nombre_Direccion =false;
                            strcpy(PalabraConEspacio,ContenidoPar);
                        }

                        else
                        {
                            Direccion = malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Direccion,ContenidoPar);
                            trigger = 1;
                        }
                    }
                    //Verificando parametro Unit
                    else if((strcasecmp(Parametro,"unit"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Palabra,7);
                        Est = VerificarUnit(ContenidoPar,2);

                        if(Est)
                        {
                            Unit = (char*)malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Unit,ContenidoPar);
                            printf("Unidad: %s\n",ContenidoPar);
                            trigger = 1;

                        }else
                            printf(">>>> ERROR: valor de unit no valido: %s <<<<\n",ContenidoPar);
                            trigger = 1;
                    }
                else {
                        printf(">>>> ERROR: Parametro no reconocido <<<<\n");
                        trigger = 1;}

            }
                else
                {
                    printf(">>>> ERROR en el dentro del parametro <<<<\n");
                    trigger = 1;
                }
                break;

             case 2:

                 if(Verificar_inicio(Palabra))
                 {
                    char *Parametro = DevolverTipoParametro(Palabra);
                    char *ContenidoPar;
                    bool Est;

                    //Reconociendo Parametro Path
                    if((strcasecmp(Parametro,"path"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Cadena,7);
                        Est = VerificarInicioEspacio(ContenidoPar);

                        if(Est)
                        {
                            EstadoAnterior = Estado;
                            Estado = 10;
                            Nombre_Direccion =false;
                            strcpy(PalabraConEspacio,ContenidoPar);

                        }else {
                            Direccion = malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Direccion,ContenidoPar);
                            trigger = 1;}


                    }else{
                        printf(">>>> ERROR: Parametro no reconocido <<<<\n");
                        trigger = 1;}

                 }
                 else
                 {
                     printf(">>>> ERROR en el inicio del parametro <<<<\n");
                     trigger = 1;
                 }
                 break;

                 //Estado para el Comando fdisk
             case 3:

                 if(Verificar_inicio(Palabra))
                 {
                    char *Parametro = DevolverTipoParametro(Palabra);
                    char *ContenidoPar;
                    bool Est;

                    //Verificando parametro Size
                    if((strcasecmp(Parametro,"size"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Palabra,7);
                        Est = VerificarSize(ContenidoPar);

                        if(Est){
                            printf(">>>> ERROR: valor de tamaño no valido: %s <<<<\n",ContenidoPar);
                            Tamano=NULL;
                            trigger = 1;
                        }
                        else
                        {
                            Tamano = (char*)malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Tamano,ContenidoPar);
                            printf("Tamaño: %s\n",ContenidoPar);
                            trigger = 1;
                        }
                    }
                    //Verificando parametro Unit
                    else if((strcasecmp(Parametro,"unit"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Palabra,7);
                        Est = VerificarUnit(ContenidoPar,2);

                        if(Est)
                        {
                            Unit = (char*)malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Unit,ContenidoPar);
                            printf("Unidad: %s\n",ContenidoPar);
                            trigger = 1;

                        }else
                            printf(">>>> ERROR: valor de unit no valido: %s <<<<\n",ContenidoPar);
                            trigger = 1;
                    }

                    else if((strcasecmp(Parametro,"path"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Cadena,7);
                        Est = VerificarInicioEspacio(ContenidoPar);

                        if(Est)
                        {
                            EstadoAnterior = Estado;
                            Estado = 10;
                            Nombre_Direccion =false;
                            strcpy(PalabraConEspacio,ContenidoPar);

                        }else
                        {
                            printf("Valor: %s\n",ContenidoPar);
                            Direccion = malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Direccion,ContenidoPar);
                            trigger = 1;
                        }
                    }
                    //Verificando parametro Tipo
                    else if((strcasecmp(Parametro,"type"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Palabra,7);
                        Est = VerificarType(ContenidoPar);

                        if(Est)
                        {
                            Tipo = (char*)malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Tipo,ContenidoPar);
                            printf("Tipo: %s\n",ContenidoPar);
                            trigger = 1;

                        }else{
                            printf(">>>> ERROR: valor de Type no valido: %s\n",ContenidoPar);
                            trigger = 1;}

                    }else if((strcasecmp(Parametro,"fit"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Palabra,6);
                        Est = VerificarFit(ContenidoPar);

                        if(Est)
                        {
                            Fitt = (char*)malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Fitt,ContenidoPar);
                            printf("Ajuste: %s\n",ContenidoPar);
                            trigger = 1;
                        }else{
                            printf(">>>> ERROR: valor de Fit no valido: %s\n",ContenidoPar);
                            trigger = 1;}

                    }else if((strcasecmp(Parametro,"delete"))==0)
                    {
                        ContenidoPar =DevolverContenidoParametro(Palabra,9);
                        Est = VerificarDelete(ContenidoPar);

                        if(Est)
                        {
                            printf("Valor: %s\n",ContenidoPar);
                            Borrar = (char*)malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Borrar,ContenidoPar);
                            printf("Tipo de Eliminacion: %s\n",ContenidoPar);
                            trigger =1;
                        }
                        else{
                            printf(">>>> ERROR: valor de Delete no valido: %s <<<<\n",ContenidoPar);
                            trigger =1;}

                    }else if((strcasecmp(Parametro,"name"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Palabra,7);
                        Est = VerificarInicioEspacio(ContenidoPar);

                        if(Est)
                        {
                            EstadoAnterior = Estado;
                            Estado = 10;
                            Nombre_Direccion = true;
                            strcpy(PalabraConEspacio,ContenidoPar);

                        }else
                        {
                            printf("Valor: %s\n",ContenidoPar);
                            Nombre = (char*)malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Nombre,ContenidoPar);
                            trigger =1;
                        }

                    }else if((strcasecmp(Parametro,"add"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Palabra,6);
                        Est = VerificarAdd(ContenidoPar);

                        if(Est)
                        {
                            printf("Tamaño add: %s\n",ContenidoPar);
                            Agregar = (char*)malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Agregar,ContenidoPar);
                            trigger =1;
                        }else{
                            printf(">>>> ERROR: valor de Add no valido: %s <<<<\n",ContenidoPar);
                            trigger =1;}

                    }else{
                        printf(">>>> ERROR: Parametro no reconocido <<<<\n");
                        trigger =1;}

                 }else{
                     printf(">>>> ERROR: en el inicio del parametro <<<<\n");
                     trigger =1;}

                break;

                //ESTADO para el comando mount
             case 4:
                 if(Verificar_inicio(Palabra))
                 {

                    char *Parametro = DevolverTipoParametro(Palabra);
                    char *ContenidoPar;
                    bool Est;

                    if((strcasecmp(Parametro,"path"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Cadena,7);

                        Est = VerificarInicioEspacio(ContenidoPar);
                        if(Est)
                        {
                            EstadoAnterior = Estado;
                            Estado = 10;
                            Nombre_Direccion =false;
                            strcpy(PalabraConEspacio,ContenidoPar);

                        }else
                        {
                            printf("Valor: %s\n",ContenidoPar);
                            Direccion = malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Direccion,ContenidoPar);
                            trigger =1;
                        }

                    }else if((strcasecmp(Parametro,"name"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Palabra,7);
                        Est = VerificarInicioEspacio(ContenidoPar);
                        if(Est)
                        {
                            EstadoAnterior = Estado;
                            Estado = 10;
                            Nombre_Direccion =true;
                            strcpy(PalabraConEspacio,ContenidoPar);

                        }else
                        {
                            printf("Valor: %s\n",ContenidoPar);
                            Nombre = (char*)malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Nombre,ContenidoPar);
                            trigger =1;
                        }

                    }else{
                        printf(">>>> ERROR: parametro no reconocido <<<<\n");
                        trigger =1;}
                 }else{
                     printf(">>>> ERROR: en el inicio del parametro <<<<\n");
                     trigger =1;}

                 break;

                 //ESTADO para el parametro unmount
             case 5:
                    if(Verificar_inicio(Palabra))
                    {
                        char *Parametro = DevolverTipoParametro(Palabra);
                        char *ContenidoPar;
                        bool Est;
                        if((strcasecmp(Parametro,"id"))==0)
                        {
                            ContenidoPar = DevolverContenidoParametro(Palabra,5);
                            Est = VerificarId(ContenidoPar);

                            if(Est)
                            {
                                printf("Valor: %s\n",ContenidoPar);
                                Idd = malloc(strlen(ContenidoPar)*sizeof(ContenidoPar)+1);
                                strcpy(Idd,ContenidoPar);
                                trigger =1;
                            }else{
                                printf(">>>> ERROR: valor para Id no valido: %s <<<<\n",ContenidoPar);
                                trigger =1;}
                        }else{
                            printf(">>>> ERROR: parametro no reconocido <<<<\n");
                            trigger =1;}
                    }else{
                        printf(">>>> ERROR: en el inicio del parametro <<<<\n");
                        trigger =1;}
                break;

                //Estado para el parametro rep
             case 6:
                 if(Verificar_inicio(Palabra))
                 {
                     char *Parametro = DevolverTipoParametro(Palabra);
                     char *ContenidoPar;
                     bool Est;

                     if((strcasecmp(Parametro,"name"))==0)
                     {
                        ContenidoPar = DevolverContenidoParametro(Palabra,7);
                        Est = VerificarRep(ContenidoPar);

                        if(Est)
                        {
                            printf("Reporte: %s\n",ContenidoPar);
                            Nombre = (char*)malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Nombre,ContenidoPar);
                            trigger =1;

                        }else
                        {
                            printf(">>>> ERROR: en el inicio del parametro <<<<\n");
                            trigger =1;}

                    }else if((strcasecmp(Parametro,"path"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Cadena,7);
                        Est = VerificarInicioEspacio(ContenidoPar);

                        if(Est)
                        {
                            EstadoAnterior = Estado;
                            Estado = 10;
                            Nombre_Direccion =false;
                            strcpy(PalabraConEspacio,ContenidoPar);

                        }else
                        {
                            printf("Valor: %s\n",ContenidoPar);
                            Direccion = malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Direccion,ContenidoPar);
                            trigger =1;
                        }

                    }else if((strcasecmp(Parametro,"id"))==0)
                    {
                        ContenidoPar = DevolverContenidoParametro(Palabra,5);
                        Est = VerificarId(ContenidoPar);

                        if(Est)
                        {
                            printf("Valor ID: %s\n",ContenidoPar);
                            Idd = malloc(strlen(ContenidoPar)*sizeof(ContenidoPar)+1);
                            strcpy(Idd,ContenidoPar);
                            trigger =1;
                        }else{
                            printf(">>>> ERROR: valor para Id no valido: %s <<<<\n",ContenidoPar);
                            trigger =1;}

                    }else{
                         printf(">>>> ERROR: parametro no reconocido <<<<\n");
                         trigger =1;}
                 }else{
                     printf(">>>> ERROR: en el inicio del parametro <<<<\n");
                     trigger =1;}

                 break;

                 //ESTADO para comando exec
             case 7:
                 if(Verificar_inicio(Palabra))
                 {
                     char *Parametro = DevolverTipoParametro(Palabra);
                     char *ContenidoPar;
                     bool Est;

                     if((strcasecmp(Parametro,"path"))==0)
                     {
                        ContenidoPar = DevolverContenidoParametro(Cadena,7);
                        Est = VerificarInicioEspacio(ContenidoPar);

                        if(Est)
                        {
                            EstadoAnterior = Estado;
                            Estado = 10;
                            Nombre_Direccion =false;
                            strcpy(PalabraConEspacio,ContenidoPar);

                        }else
                        {
                            printf("Valor: %s\n",ContenidoPar);
                            Direccion =(char*)malloc((strlen(ContenidoPar)*sizeof(ContenidoPar))+1);
                            strcpy(Direccion,ContenidoPar);
                            trigger = 1;
                        }

                     }else{
                         printf(">>>> ERROR: parametro no reconocido <<<<\n");
                         trigger =1;}


                }else{
                     printf(">>>> ERROR: en el inicio del parametro <<<<\n");
                     trigger = 1;
                     }

                 break;

                 //ESTADO de comentarios dentro de la linea
            case 8:
                if(Verificar_inicio(Palabra)){
                    char *ContenidoPar;
                    ContenidoPar = DevolverContenidoParametro(Palabra,1);
                    printf("Comentario: %s\n",ContenidoPar);
                    printf(ContenidoPar);
                    trigger = 1;
                }
                    break;

             case 10:

                 if(!VerificarFinEspacio(Palabra))
                 {
                     strcat(PalabraConEspacio," ");
                     strcat(PalabraConEspacio,Palabra);
                     Estado = EstadoAnterior;
                     InsertarPath(PalabraConEspacio,EstadoAnterior);

                 }else
                 {
                     //No ha terminado la cadena = False
                     strcat(PalabraConEspacio," ");
                     strcat(PalabraConEspacio,Palabra);
                 }

                 trigger = 1;
                 break;

               //ESTADO DE ERROR
            case 13:
                 Estado = Estado;
                 trigger = 1;
                 break;

            default:
                printf("Comando no Reconocido\n");
                trigger = 1;
                break;
         }

         Cadena+=n;
         if(*Cadena!=' ')
         {
             trigger=0;
             break;
         }
            while(*Cadena == ' ' || isdigit(*Cadena) || *Cadena == '/' || *Cadena == '"'){
             ++Cadena;
             trigger=0;}
    }

    if(Estado==1)
     {
         if((Direccion!=NULL) && (Tamano!=NULL) && (Nombre!=NULL) )
         {
            CrearDisco(Direccion,Tamano,Nombre,NULL);
            free(Direccion);
            free(Tamano);
            free(Nombre);
         }else
             printf(">>>> ERROR: imposible ejecutar mkdisk, faltan parametros <<<<\n");
     }
     else if(Estado==2)
     {
        if(Direccion==NULL)
            printf(">>>> ERROR: imposible ejecutar rmdisk, faltan parametros <<<<\n");
        else
        {
            EliminarDisco(Direccion);
            free(Direccion);
        }
     }else if(Estado==3)
     {
         //Parametros Obligatorios Tamanio, Path y Nombre
         if((Direccion!=NULL)&&(Nombre!=NULL))
         {
             if(Agregar!=NULL)
            {
                 //LA Operacion seria agregar Espacio a particion
                 if(Agregar!=NULL)
                     ModificarParticion(Agregar,Direccion,Nombre,Unit);
                 else
                     printf(">>>>ERROR: valor de add no valido <<<<\n");

            }else if(Borrar!=NULL)
            {
                 //LA Operacion seria Borrar una particion
                 EliminarParticion(Direccion,Nombre,Borrar);
            }else
            {
                 //La Operacion seria Crear una Nueva Particion
                 if(Tamano!=NULL)
                     CrearParticion(Direccion,Nombre,Tamano,Unit,Tipo,Fitt);
                 else
                     printf(">>>>ERROR: valor no valido <<<<\n");
            }
         }else
             printf(">>>> ERROR: imposible ejecutar fdisk, faltan parametros <<<<\n");
     }else if(Estado==4)
     {
         if((Direccion!=NULL)&&(Nombre!=NULL))
         {
             MontarParticion(Direccion,Nombre);
         }else
             printf(">>>>ERROR: imposible ejecutar mount, faltan parametros <<<<\n");

     }else if(Estado==5)
     {
         if(Idd!=NULL)
             DesmontarParticion(Idd);
         else
             printf(">>>>ERROR: imposible ejecutar unmount, faltan parametros <<<<\n");
     }

        else if(Estado==6)
     {
         if((Direccion!=NULL) && (Nombre!=NULL) && (Idd!=NULL))
         {
             if((strcasecmp(Nombre,"mbr"))==0)
             {
                 CrearReporteMBR(Direccion,Idd);
             }else if(strcasecmp(Nombre,"disk")==0)
             {
                 CrearReporteDisco(Direccion,Idd);
             }else if(strcasecmp(Nombre,"inode")==0)
             {
                 CrearReporteBloquesInodo(Direccion,Idd,2);
             }else if(strcasecmp(Nombre,"block")==0)
             {
                 CrearReporteBloquesInodo(Direccion,Idd,1);
             }else if(strcasecmp(Nombre,"bm_inode")==0)
             {
                CrearReporteBitmapInodos(Direccion,Idd);

             }else if(strcasecmp(Nombre,"bm_block")==0)
             {
                 CrearReporteBitmapBloques(Direccion,Idd);

             }else if(strcasecmp(Nombre,"tree")==0)
             {
                 CrearReporteArbol(Direccion,Idd);
             }else if(strcasecmp(Nombre,"sb")==0)
             {
                 CrearReporteSuperBloque(Direccion,Idd);

             }else if(strcasecmp(Nombre,"file")==0)
             {
                 if(DirecContenido==NULL)
                     printf(">>>>ERROR: falta la ruta del archivo<<<<\n");
                 else
                 {
                    CrearReporteFileLs(Direccion,Idd,DirecContenido,1,1);
                    free(DirecContenido);
                 }
             }else
                 printf(">>>>ERROR: contenido de -nombre no valido para comando rep<<<<\n");

            free(Nombre);
            free(Idd);
            free(Direccion);
        }else
             printf(">>>>ERROR: imposible ejecutar rep, faltan parametros <<<<\n");
    }
     else if(Estado==7)
     {
         if((Direccion)!=NULL)
         {
            LeerArchivo(Direccion);
         }else
             printf(">>>> ERROR: comando exec le faltan parametros <<<<\n");
     }
}

int main()
{
    printf("UNIVERSIDAD SAN CARLOS DE GUATEMALA\n");
    printf("FACULTAD DE INGENIERIA\n");
    printf("ESCUELA DE SISTEMAS\n");
    printf("MANEJO E IMPLEMENTACION DE ARCHIVOS\n");
    printf("PROYECTO - FASE 1\n");
    printf("******************************************************************************");
    printf("\n \n \n");
    printf("Ingrese sentencia:");
    printf("\n");

    char cadena[600];
    char *p;
    bool sta;
    int Salir;

    sta = true;

 while(sta != false){

     fflush(stdin);
     fgets(cadena,300,stdin);

        if(cadena!=NULL)
        {
            if(cadena[strlen(cadena)-1]=='\n')
                cadena[strlen(cadena)-1] ='\0';
            else
                cadena[strlen(cadena)-1] = '\0';
                 Analizador(cadena);
        }
        else
            printf("No has escrito nada\n");

        Salir = strcasecmp(cadena,"fin");
        if(Salir == 0)
        {
            sta = false;
        }

    }
    return (EXIT_SUCCESS);
}
