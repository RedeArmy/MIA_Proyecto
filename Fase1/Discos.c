#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include "Discos.h"
#include "Graficadora.h"

//********************************* ESTRUCTURA PARA EL DISCO *****************************************************
//****************************************************************************************************************
    struct Particion
    {
        int Estado;
        char Tipo[5];
        char TipoAjuste[5];
        int ByteInicial;
        int Tamano;
        char Nombre[50];
        int Orden;
    };


    struct MBR
    {
        int TamanoDisco;
        char Fecha[20];
        int IdIdentif;
        int NumeroPrimaria;
        int NumeroExtendida;
        struct Particion Particion1;
        struct Particion Particion2;
        struct Particion Particion3;
        struct Particion Particion4;
    };

    //**************************************************** ESTRUCTURA PARA ORDENAR LAS PARTICIONES ********************************************************************
    //****************************************************************************************************************************************************
    struct OrdenarParticion
    {
        int Estado;
        char Tipo[5];
        char TipoAjuste[5];
        int ByteInicial;
        int Tamano;
        char Nombre[50];
        int Orden;
        struct OrdenarParticion *Siguiente;
        struct OrdenarParticion *Anterior;

    };
    struct CabeceraParticion
    {
        int NumeroPrimarias;
        int NumeroExtendidas;
        struct OrdenarParticion *Primera;
        struct OrdenarParticion *Ultima;
    };

    //************************************************** ESTRUCTURA DEL EBR ********************************************************************
    //****************************************************************************************************************************************************
    struct EBR
    {

        char Estado[2];
        char Fitt[5];
        int ByteInicial;
        int Tamano;
        int ProximoEBR;
        char Nombre[20];
    };

    //********************************************* ESTRUCTURA DEL SUPER BLOQUE **********************************************************************************
    //************************************************************************************************************************************************************
    struct SuperBloque
    {
        int TotalInodos;
        int TotalBloques;
        int BloquesLibres;
        int InodosLibres;
        time_t UltimaFechaMontura;
        time_t UltimaFechaDesmont;
        int VecesMontado;
        int SistemaArchivos;
        int TamanoInodo;
        int TamanoBloque;
        int PrimerInodoLibre;
        int PrimerBloqueLibre;
        int InicioBitmapInodos;
        int InicioBitmapBloques;
        int InicioTablaInodos;
        int InicioTablaBloques;

    };

    struct TablaInodos
    {
        int UsuarioPropietario;
        int GrupoPropietario;
        int TamanoArchivo;
        time_t UltimaFechaLectura;
        time_t FechaCreacion;
        time_t UltimaFechaModific;
        int Apuntadores[15];
        char Tipo[1];
        int Permisos;

    };

    struct Carpetas
    {
        char Nombre[12];
        int Apuntador;
    };

    struct BloqueCarpetas
    {
        struct Carpetas ApuntadorCarpeta[4];
    };

    struct Archivos
    {
        char Contenido[64];
    };

    struct BloqueApuntadores
    {
        int Apuntadores[16];
    };
    typedef struct Nodo
    {
        int Inicio;
        int Tamano;
        struct Nodo *Siguiente;
        struct Nodo *Anterior;

    }NodoLista;

    typedef struct Cab
    {
        int Tamano;
        NodoLista *Primer;
        NodoLista *Ultimo;
    }Cap;

typedef struct Part
    {
        char NombreParticion[125];
        int NumeroParticion;
        char IdParticion[25];
        struct Part *Siguiente;
        struct Part *Anterior;


    }ParticionId;

    typedef struct Estr
    {
        int NumeroDisco;
        char PathDisco[125];
        int NumeroParticiones;
        ParticionId *Primera;
        ParticionId *Ultima;
        struct Estr *Siguiente;
        struct Estr *Anterior;
    }DiscoMount;

    typedef struct Mon
    {
        int NumeroDiscos;
        DiscoMount *Inicio;
        DiscoMount *Final;

    }MontajeParticiones;

static MontajeParticiones *Cabecera;

//******************************** METODO PARA INICIALIZAR LA LISTA ORDENADA ***************************************************************************************
//******************************************************************************************************************************************************************
void InicializarLista( Cap *Cabec)
{
    Cabec->Primer = NULL;
    Cabec->Ultimo = NULL;
    Cabec->Tamano = 0;
}

//******************************** METODO PARA INSERTAR UN NUEVO DATO A LISTA ***********************************************************************************************
//****************************************************************************************************************************************************************************
void InsertarDato(Cap *Cabec, int Inicio, int Tamano)
{
    NodoLista *Dato;
    Dato = malloc(sizeof(NodoLista));
    Dato->Siguiente = Dato->Anterior = NULL;
    Dato->Inicio = Inicio;
    Dato->Tamano = Tamano;

    if(Cabec->Tamano == 0)
    {
        //Es el primer dato que ingresa
        Cabec->Primer = Dato;
        Cabec->Ultimo = Dato;
        Cabec->Tamano++;

    }
    else
    {
        //Se ordena la Lista de Mayor a menor
         bool Condicion = true;
         NodoLista *Auxiliar = Cabec->Primer;
         Dato->Tamano = Tamano;
         Dato->Inicio = Inicio;
         Dato->Siguiente = Dato->Anterior = NULL;

         while(Condicion)
         {
             if(Tamano >= Auxiliar->Tamano)
             {
                 //Poniendo Nodos Por delante
                 if(Auxiliar == Cabec->Primer)
                 {
                     Dato->Siguiente = Cabec->Primer;
                     Cabec->Primer->Anterior = Dato;
                     Cabec->Primer = Dato;
                     Cabec->Tamano++;

                 }
                 else{
                     //Cuando le nodo insertar debe estar en medio de dos nodos
                     NodoLista *AuxiliarAtrasado = Cabec->Primer;
                     while(AuxiliarAtrasado->Siguiente != Auxiliar){
                         AuxiliarAtrasado = AuxiliarAtrasado->Siguiente;
                     }
                     //Redireccionando Nodos
                     Dato->Siguiente = Auxiliar;
                     Dato->Anterior = AuxiliarAtrasado;
                     Auxiliar->Anterior = Dato;
                     AuxiliarAtrasado->Siguiente = Dato;
                     Cabec->Tamano++;
                 }
                 Condicion = false;
            }
            else if(Auxiliar->Siguiente ==NULL && Tamano < Auxiliar->Tamano){
                 //Significa que va al final de la Cola
                 Dato->Siguiente = NULL;
                 Dato->Anterior = Cabec->Ultimo;
                 Cabec->Ultimo->Siguiente = Dato;
                 Cabec->Ultimo =  Dato;
                 Cabec->Tamano++;

                 Condicion = false;
             }
             //Corriendo los auxiliares
             Auxiliar = Auxiliar->Siguiente;
        }
    }
}

//********************* METODO PARA DEVOLVER EL VALOR DEL PRIMER NODO O DEL ULTIMO NODO ***********************************************************************************
//**********************************************************************************************************************************************************************************
int DevolverValor(Cap *Cabec,int Tipo)
{
    if(Cabec->Tamano!=0)
    {
        if(Tipo ==1)
            return Cabec->Primer->Inicio;
        else
            return Cabec->Ultimo->Inicio;
    }else
        return -1;   //Significa error
}
//***************************** METODO PARA BORRAR LOS NODOS DE UNA LISTA ******************************************************************************************************************************
//****************************************************************************************************************************************************************************************************************
void BorrarNodos(Cap *Cabec)
{
    if(Cabec->Tamano !=0)
    {
        NodoLista *Auxiliar;
        while(Cabec->Tamano !=0)
        {
            Auxiliar = Cabec->Primer;
            if(Cabec->Primer->Siguiente!=NULL)
            {
                Cabec->Primer->Siguiente->Anterior =NULL;
                Cabec->Primer = Cabec->Primer->Siguiente;
                Auxiliar->Siguiente = NULL;
            }

            free(Auxiliar);
            Cabec->Tamano--;
        }
    }else
        return;
}

//********************************* METODO PARA CREAR DISCO*****************************************************
//****************************************************************************************************************
int InsertarNuevoDisco(char* Path)
{
    if(Cabecera!=NULL)
    {
        //La cabecera ya fue creada anteriormente
        int Contador;
        int TotalDiscos;
        bool Estado;
        DiscoMount *NuevoDisco;
        DiscoMount *Auxiliar;

        Contador =0;
        TotalDiscos = Cabecera->NumeroDiscos;
        Estado=true;
        Auxiliar = Cabecera->Inicio;

        while((Contador < TotalDiscos) && (Estado==true))
        {
            if((strcmp(Auxiliar->PathDisco,Path))==0)
            {
                Estado = false;
            }
            Contador++;
            Auxiliar = Auxiliar->Siguiente;
        }

        if(Estado)
        {
            //El Disco No ha sido Creado todavía no existe en la Lista
            NuevoDisco = malloc(sizeof(DiscoMount));
            strcpy(NuevoDisco->PathDisco,Path);
            NuevoDisco->NumeroParticiones =0;
            NuevoDisco->Primera=NULL;
            NuevoDisco->Ultima=NULL;
            NuevoDisco->NumeroDisco = Cabecera->NumeroDiscos +1;

            //Redirigiendo los apuntadores
            NuevoDisco->Anterior = Cabecera->Final;
            NuevoDisco->Siguiente = NULL;
            Cabecera->Final->Siguiente = NuevoDisco;
            Cabecera->Final = NuevoDisco;
            Cabecera->NumeroDiscos++;

            return NuevoDisco->NumeroDisco;
        }
        else
            return Contador;
    }
    else
    {
        //La Cabecera no ha sido Creada
        Cabecera = malloc(sizeof(MontajeParticiones));
        Cabecera->Final=NULL;
        Cabecera->Inicio=NULL;
        Cabecera->NumeroDiscos=0;

        DiscoMount *NuevoDisco;
        NuevoDisco = malloc(sizeof(DiscoMount));
        strcpy(NuevoDisco->PathDisco,Path);
        NuevoDisco->NumeroDisco = Cabecera->NumeroDiscos+1;
        NuevoDisco->NumeroParticiones = 0;
        NuevoDisco->Primera = NULL;
        NuevoDisco->Ultima=NULL;
        NuevoDisco->Siguiente = NULL;
        NuevoDisco->Anterior = NULL;
        Cabecera->Inicio = NuevoDisco;
        Cabecera->Final = NuevoDisco;
        Cabecera->NumeroDiscos++;

        return NuevoDisco->NumeroDisco;
    }
}

//******************************************* METODO PARA OBTENER UNA LETRA *******************************************************+
//********************************************************************************************************************************************
char * DevolverLetra(int Numero)
{
    if(Numero==1)
        return "a";
    else if(Numero==2)
        return "b";
    else if(Numero==3)
        return "c";
    else if(Numero==4)
        return "d";
    else if(Numero==5)
        return "e";
    else if(Numero==6)
        return "f";
    else if(Numero==7)
        return "g";
    else if(Numero==8)
        return "h";
    else if(Numero==9)
        return "i";
    else if(Numero==10)
        return "j";
    else if(Numero==11)
        return "k";
    else if(Numero==12)
        return "l";
    else if(Numero==13)
        return "m";
    else if(Numero==14)
        return "n";
    else if(Numero==15)
        return "o";
    else if(Numero==16)
        return "p";
    else if(Numero==17)
        return "q";
    else if(Numero==18)
        return "r";
    else if(Numero==19)
        return "s";
    else if(Numero==20)
        return "t";
    else if(Numero==21)
        return "u";
    else if(Numero==22)
        return "v";
    else if(Numero==23)
        return "w";
    else if(Numero==24)
        return "x";
    else if(Numero==25)
        return "y";
    else if(Numero==26)
        return "z";
    else
        return "a";
}
//************************************** METODO PARA DEVOLVER UN NUMERO A PARTIR DE UNA LETRA ***********++++*************************************************
//***********************************************************************************************************************************************************************
int DevolverNumero(char *IdParticion)
{
    int NumeroDisco;

    if(*IdParticion=='a')
        NumeroDisco=1;
    else if(*IdParticion=='b')
        NumeroDisco=2;
    else if(*IdParticion=='c')
        NumeroDisco=3;
    else if(*IdParticion=='d')
        NumeroDisco=4;
    else if(*IdParticion=='e')
        NumeroDisco=5;
    else if(*IdParticion=='f')
        NumeroDisco=6;
    else if(*IdParticion=='g')
        NumeroDisco=7;
    else if(*IdParticion=='h')
        NumeroDisco=8;
    else if(*IdParticion=='i')
        NumeroDisco=9;
    else if(*IdParticion=='j')
        NumeroDisco=10;
    else if(*IdParticion=='k')
        NumeroDisco=11;
    else if(*IdParticion=='l')
        NumeroDisco=12;
    else if(*IdParticion=='m')
        NumeroDisco=13;
    else if(*IdParticion=='n')
        NumeroDisco=14;
    else if(*IdParticion=='o')
        NumeroDisco=15;
    else if(*IdParticion=='p')
        NumeroDisco=16;
    else if(*IdParticion=='q')
        NumeroDisco=17;
    else if(*IdParticion=='r')
        NumeroDisco=18;
    else if(*IdParticion=='s')
        NumeroDisco=19;
    else if(*IdParticion=='t')
        NumeroDisco=20;
    else if(*IdParticion=='u')
        NumeroDisco=21;
    else if(*IdParticion=='v')
        NumeroDisco=22;
    else if(*IdParticion=='w')
        NumeroDisco=23;
    else if(*IdParticion=='x')
        NumeroDisco=24;
    else if(*IdParticion=='y')
        NumeroDisco=25;
    else if(*IdParticion=='z')
        NumeroDisco=26;
    else
        NumeroDisco=1;

    return NumeroDisco;

}
//************************************** METODO PARA GENERAR UN ID PARA PARTICION E INSERTAR *************************************************
//*****************************************************************************************************************************************************
void GenerarIdParticion(int numero,char *NombrePar)
{
    int Contador;
    DiscoMount *Auxiliar;

    Auxiliar = Cabecera->Inicio;
    Contador=0;

    while(Contador < numero)
    {
        if(numero == Auxiliar->NumeroDisco)
        {
            if(Auxiliar->Primera==NULL)
            {
                //No ha sido ingresada ninguna particion
                ParticionId *NuevaParticion;

                Auxiliar->NumeroParticiones++;
                NuevaParticion = malloc(sizeof(ParticionId));
                NuevaParticion->Anterior=NULL;
                NuevaParticion->Siguiente=NULL;
                strcpy(NuevaParticion->NombreParticion,NombrePar);
                NuevaParticion->NumeroParticion=1;
                strcpy(NuevaParticion->IdParticion,"vd");
                strcat(NuevaParticion->IdParticion,DevolverLetra(Auxiliar->NumeroDisco));

                Auxiliar->Primera = NuevaParticion;
                Auxiliar->Ultima = NuevaParticion;
                printf("Particion con Id: %s%d montada Correctamente\n",NuevaParticion->IdParticion,NuevaParticion->NumeroParticion);

            }
            else
            {
                //Ya se Han Ingresado Particiones, se van agregando al final
                ParticionId *NuevaParticion;
                Auxiliar->NumeroParticiones++;

                NuevaParticion = malloc(sizeof(ParticionId));
                NuevaParticion->NumeroParticion= Auxiliar->NumeroParticiones;
                strcpy(NuevaParticion->NombreParticion,NombrePar);
                strcpy(NuevaParticion->IdParticion,"vd");
                strcat(NuevaParticion->IdParticion,DevolverLetra(Auxiliar->NumeroDisco));

                //Redirigiendo Punteros
                NuevaParticion->Anterior = Auxiliar->Ultima;
                NuevaParticion->Siguiente = NULL;
                Auxiliar->Ultima->Siguiente = NuevaParticion;
                Auxiliar->Ultima = NuevaParticion;
                printf("Particion con id: %s%d montada correctamente\n",NuevaParticion->IdParticion,NuevaParticion->NumeroParticion);
            }
        }
        Auxiliar = Auxiliar->Siguiente;
        Contador++;
    }
}

//*************************************** METODO PARA DESMONTAR PARTICIONES *******************************************************************************
//****************************************************************************************************************************************************************
void DesmontParticion(int NumeroParticion,char *IdParticion)
{
    int NumeroDisco;
    int Contador;

    if(Cabecera!=NULL)
    {
        if(Cabecera->Inicio==NULL)
        {
            printf(">>>>ERROR: no se ha montado ninguna particion, imposible desmontar<<<<\n");
            return;
        }

    }
    else
    {
        printf(">>>>ERROR: no se ha montado ninguna particion, imposible desmontar <<<<\n");
        return;
    }

    Contador=0;

    DiscoMount *DiscoAuxiliar;
    ParticionId *ParticionAuxiliar;
    DiscoAuxiliar = Cabecera->Inicio;
    NumeroDisco = DevolverNumero(IdParticion);

    while (Contador < Cabecera->NumeroDiscos)
    {
        if(DiscoAuxiliar->NumeroDisco == NumeroDisco)
        {
            if(DiscoAuxiliar->Primera==NULL)
            {
                printf(">>>>ERROR: particion no encontrada en el sistema<<<<\n");
                return;
            }
            else
            {
                ParticionAuxiliar = DiscoAuxiliar->Primera;
                while (ParticionAuxiliar!=NULL)
                {
                    if(ParticionAuxiliar->NumeroParticion == NumeroParticion)
                    {
                        if(DiscoAuxiliar->NumeroParticiones==1)
                        {
                            //UNICO NODO DE LA LISTA
                            DiscoAuxiliar->Primera=NULL;
                            DiscoAuxiliar->Ultima=NULL;
                            DiscoAuxiliar->NumeroParticiones--;
                            free(ParticionAuxiliar);
                            printf("Particion vd%s desmontada correctamente\n",IdParticion);
                            return;
                        }
                        else if(ParticionAuxiliar==DiscoAuxiliar->Primera)
                        {
                            //NODO AL PRINCIPIO DE LA LISTA
                            DiscoAuxiliar->Primera=ParticionAuxiliar->Siguiente;
                            DiscoAuxiliar->Primera->Anterior=NULL;
                            DiscoAuxiliar->NumeroParticiones--;
                            ParticionAuxiliar->Siguiente=NULL;
                            free(ParticionAuxiliar);
                            printf("Particion vd%s desmontada correctamente\n",IdParticion);
                            return;

                        }else if(ParticionAuxiliar == DiscoAuxiliar->Ultima)
                        {
                            //NODO AL FINAL DE LA LISTA
                            DiscoAuxiliar->Ultima = ParticionAuxiliar->Anterior;
                            DiscoAuxiliar->Ultima->Siguiente=NULL;
                            DiscoAuxiliar->NumeroParticiones--;
                            ParticionAuxiliar->Anterior=NULL;
                            free(ParticionAuxiliar);
                            printf("Particion vd%s desmontada correctamente\n",IdParticion);
                            return;
                        }else
                        {
                            //NODO EN MEDIO DE LA LISTA
                            ParticionAuxiliar->Anterior->Siguiente = ParticionAuxiliar->Siguiente;
                            ParticionAuxiliar->Siguiente->Anterior = ParticionAuxiliar->Anterior;
                            ParticionAuxiliar->Siguiente=NULL;
                            ParticionAuxiliar->Anterior=NULL;
                            DiscoAuxiliar->NumeroParticiones--;
                            free(ParticionAuxiliar);
                            printf("Particion vd%s desmontada correctamente\n",IdParticion);
                            return;
                        }
                    }

                    ParticionAuxiliar = ParticionAuxiliar->Siguiente;
                }

            }

        }

        DiscoAuxiliar = DiscoAuxiliar->Siguiente;
        Contador++;
    }

    printf(">>>>ERROR: particion vd%s no encontrada en el sistema de montaje<<<<\n",IdParticion);
}
//******************************** METODO PARA DEVOLVER LA DIRECCION DE UN DISCO ******************************************************
//***********************************************************************************************************************************************
char *DevolverPath(char *IdDisco)
{
    int NumeroDisco;
    int NumeroParticion;
    IdDisco++;
    IdDisco++;

        NumeroDisco = DevolverNumero(IdDisco);
		IdDisco++;
		NumeroParticion = (atoi(IdDisco));

    DiscoMount *Auxiliar;

    if(Cabecera==NULL)
        return "a";  //Significa error

    else
    {
        Auxiliar = Cabecera->Inicio;

        while(Auxiliar!=NULL)
        {
            if(Auxiliar->NumeroDisco==NumeroDisco)
            {
                ParticionId *PartAux;
                PartAux = Auxiliar->Primera;

                while(PartAux!=NULL)
                {
                    if(NumeroParticion == PartAux->NumeroParticion)
                        return Auxiliar->PathDisco;
                    PartAux = PartAux->Siguiente;
                }

            }
            Auxiliar = Auxiliar->Siguiente;
        }

        return "a"; //Significa error
    }
}

//*************************** METODO PARA DEVOLVER EL NOMBRE DE UNA PARTICION ***************************************************************
//**********************************************************************************************************************************************************************
char *Devolverparticion(char *IdDisco)
{
    int NumeroDisco;
    int NumeroParticion;
    IdDisco++;
    IdDisco++;

    NumeroDisco = DevolverNumero(IdDisco);
    IdDisco++;
    NumeroParticion = (atoi(IdDisco));

    DiscoMount *Auxiliar;
    if(Cabecera==NULL)
        return "a";  //Significa error
    else
    {
        Auxiliar = Cabecera->Inicio;

        while(Auxiliar!=NULL)
        {
            if(Auxiliar->NumeroDisco==NumeroDisco)
            {
                ParticionId *PartAux;
                PartAux = Auxiliar->Primera;

                while(PartAux!=NULL)
                {
                    if(NumeroParticion == PartAux->NumeroParticion)
                        return PartAux->NombreParticion;

                    PartAux = PartAux->Siguiente;
                }
            }
            Auxiliar = Auxiliar->Siguiente;
        }
        return "a"; //Significa error
    }

}
//******************************** METODO PARA DEVOLVER LA DIRECCION DEL PRIMER DISCO MONTADO **************************************************************+
//*********************************************************************************************************************************************************************
char *DevolverPrimerDireccion()
{
    if(Cabecera ==NULL)
        return "a";
    else
    {
        if(Cabecera->NumeroDiscos !=0)
        {
            return Cabecera->Inicio->PathDisco;
        }else
            return "a";
    }
}
//******************************* METODO PARA DDEVOLVER EL NOMBRE DE LA PRIMER PARTCION MONTADA ***********************************************************************
//**********************************************************************************************************************************************************************
char *DevolverPrimerParticion()
{
    if(Cabecera==NULL)
        return "a";
    else
    {
        if(Cabecera->NumeroDiscos != 0)
        {
            return Cabecera->Inicio->Primera->NombreParticion;
        }else
            return "a";
    }
}

void InsertarMBR(struct MBR *Estructura,FILE *Archivo);

//********************************* METODO PARA CREAR EL DISCO ************************************************************
//*************************************************************************************************************************

void CrearDisco(char *Path, char *Size, char *Nombre, char *Capacidad)
{
    char Buffer[1024];
    char Direccion[100];
    char carpeta[200];

    int NumeroAleatorio;

    int i=0;
    int Numero;
    struct MBR Estructura;

    //Pasando a Int
    Numero = atoi(Size);

    strcpy(Direccion,Path);

    snprintf(carpeta,sizeof(carpeta),"mkdir -p -v -m777 \"%s\"",Direccion);
    system(carpeta);

    strcat(Direccion,Nombre);


    FILE *Archivo;

    Archivo = fopen(Direccion,"wb+");
    printf("%s", Direccion);

    if(Archivo)
    {
    //Limpiando el Buffer
        for(i=0; i<1025;i++)
        {
            Buffer[i] = '\0';
        }

        if(Capacidad==NULL)
        {
            //Se asume que son  Megabytes
            Numero = Numero * 1024;
            for(i =0;i<Numero;i++)
            {
                fwrite(Buffer,1024,1,Archivo);

            }
            //Pasando el Numero a bytes
            Numero = Numero * 1024;
        }

        NumeroAleatorio = rand()%(1000);

        //Obteniendo la Fecha del sistema
        time_t Tiempo = time(0);
        struct tm *TiempoLocal = localtime(&Tiempo);
        char fechaObtenida[20];
        strftime(fechaObtenida,128,"%d/%m/%y %H:%M:%S",TiempoLocal);
        //printf("%s\n",fechaObtenida);

        Estructura.TamanoDisco = Numero;
        strcpy(Estructura.Fecha,fechaObtenida);
        Estructura.IdIdentif = NumeroAleatorio;
        Estructura.NumeroExtendida =0;
        Estructura.NumeroPrimaria =0;

        Estructura.Particion1.Estado = 0;
        Estructura.Particion1.ByteInicial =0;
        Estructura.Particion1.Tamano =0;
        Estructura.Particion1.Orden =1;

        Estructura.Particion2.Estado = 0;
        Estructura.Particion2.ByteInicial=0;
        Estructura.Particion2.Tamano=0;
        Estructura.Particion2.Orden =2;

        Estructura.Particion3.Estado = 0;
        Estructura.Particion3.Tamano=0;
        Estructura.Particion3.ByteInicial=0;
        Estructura.Particion3.Orden =3;

        Estructura.Particion4.Estado = 0;
        Estructura.Particion4.ByteInicial=0;
        Estructura.Particion4.Tamano=0;
        Estructura.Particion4.Orden=4;

        InsertarMBR(&Estructura,Archivo);
    }

    else
    {
        printf(">>>> ERROR: direccion de disco no valida <<<< \n");
        return -1;
    }
    fclose(Archivo);
}

//************************************* METODO PARA INSERTAR EL STRUCT ************************************************************
//*************************************************************************************************************************************
InsertarMBR(struct MBR *Estructura,FILE *Archivo)
{
    fseek(Archivo,0,SEEK_SET);
    fwrite((Estructura),sizeof(struct MBR),1,Archivo);
}

//************************************* METODO PARA INSERTAR UN EBR ***************************************************************************
//***************************************************************************************************************************************
void InsertarEBR(struct EBR *Estructura, FILE *Archivo,int ByteInicial)
{
    fseek(Archivo,ByteInicial,SEEK_SET);
    fwrite(Estructura,sizeof(struct EBR),1,Archivo);
}

//************************************** METODO PARA BORRAR UN DISCO ******************************************************
//*************************************************************************************************************************************
void EliminarDisco(char *Direccion)
{
    char Pat[128];
    strcpy(Pat,Direccion);

    if(remove(Pat)==0)
        printf("---- Disco Borrado Correctamente ----\n");
    else
        printf(">>>> ERROR: imposible borrar disco, direccion invalida <<<<\n");
}

//************************************ METODO PARA ORDENAR PARTICIONES *************************************************************************
//***************************************************************************************************************************************************
struct CabeceraParticion* OrdenarParticiones(struct MBR *Estructura)
{
    struct CabeceraParticion *CabPa;
    struct OrdenarParticion *A,*B,*C,*D;
    int Contador;
    bool Estado;

    CabPa = malloc(4*sizeof(struct CabeceraParticion));
    A = malloc(4*sizeof(struct OrdenarParticion));
    B = malloc(4*sizeof(struct OrdenarParticion));
    C = malloc(4*sizeof(struct OrdenarParticion));
    D = malloc(4*sizeof(struct OrdenarParticion));
    int i;

    A->ByteInicial = Estructura->Particion1.ByteInicial;
    A->Estado = Estructura->Particion1.Estado;
    strcpy(A->Nombre,Estructura->Particion1.Nombre);
    A->Orden = Estructura->Particion1.Orden;
    A->Tamano = Estructura->Particion1.Tamano;
    strcpy( A->Tipo,Estructura->Particion1.Tipo);
    strcpy(A->TipoAjuste,Estructura->Particion1.TipoAjuste);

    B->ByteInicial = Estructura->Particion2.ByteInicial;
    B->Estado = Estructura->Particion2.Estado;
    strcpy(B->Nombre,Estructura->Particion2.Nombre);
    B->Orden = Estructura->Particion2.Orden;
    B->Tamano = Estructura->Particion2.Tamano;
    strcpy( B->Tipo,Estructura->Particion2.Tipo);
    strcpy(B->TipoAjuste,Estructura->Particion2.TipoAjuste);

    C->ByteInicial = Estructura->Particion3.ByteInicial;
    C->Estado = Estructura->Particion3.Estado;
    strcpy(C->Nombre,Estructura->Particion3.Nombre);
    C->Orden = Estructura->Particion3.Orden;
    C->Tamano = Estructura->Particion3.Tamano;
    strcpy( C->Tipo,Estructura->Particion3.Tipo);
    strcpy(C->TipoAjuste,Estructura->Particion3.TipoAjuste);

    D->ByteInicial = Estructura->Particion4.ByteInicial;
    D->Estado = Estructura->Particion4.Estado;
    strcpy(D->Nombre,Estructura->Particion4.Nombre);
    D->Orden = Estructura->Particion4.Orden;
    D->Tamano = Estructura->Particion4.Tamano;
    strcpy( D->Tipo,Estructura->Particion4.Tipo);
    strcpy(D->TipoAjuste,Estructura->Particion4.TipoAjuste);

    CabPa->Ultima =A;
    CabPa->Primera =A;
    A->Siguiente=NULL;
    A->Anterior = NULL;
    //'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    if(A->Orden>Estructura->Particion2.Orden)
    {

        B->Siguiente = A;
        A->Anterior =B;
        CabPa->Primera = B;
        CabPa->Ultima = A;
    }else
    {
        A->Siguiente =B;
        B->Anterior =A;
        CabPa->Ultima = B;
    }
    // ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    if(CabPa->Primera->Orden > Estructura->Particion3.Orden)
    {
        C->Siguiente = CabPa->Primera;
        CabPa->Primera->Anterior = C;
        CabPa->Primera =C;

    }else
    {
        if(CabPa->Primera->Siguiente->Orden > Estructura->Particion3.Orden)
        {

            C->Siguiente = CabPa->Ultima;
            C->Anterior = CabPa->Primera;
            CabPa->Primera->Siguiente = C;
            CabPa->Ultima->Anterior = C;
        }else
        {
            CabPa->Ultima->Siguiente = C;
            C->Anterior = CabPa->Ultima;
            CabPa->Ultima = C;

        }
    }

    //'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    if(CabPa->Primera->Orden>Estructura->Particion4.Orden)
    {
        CabPa->Primera->Siguiente = D;
        D->Siguiente = CabPa->Primera;
        CabPa->Primera = D;
    }else
    {
        if(CabPa->Primera->Siguiente->Orden > Estructura->Particion4.Orden)
        {
            D->Siguiente = CabPa->Primera->Siguiente;
            D->Anterior = CabPa->Primera;
            CabPa->Primera->Siguiente->Anterior = D;
            CabPa->Primera->Siguiente = D;
        }else
        {
            if(CabPa->Ultima->Orden > Estructura->Particion4.Orden)
            {
                D->Siguiente = CabPa->Ultima;
                D->Anterior = CabPa->Ultima->Anterior;
                CabPa->Ultima->Anterior->Siguiente =D;
                CabPa->Ultima->Anterior = D;


            }else
            {
                CabPa->Ultima->Siguiente = D;
                D->Anterior = CabPa->Ultima;
                CabPa->Ultima = D;
            }
        }
    }

    return CabPa;


}

//*********************************** METODO PARA INSERTAR LAS PARTICIONES ***********************************************
//*************************************************************************************************************************
void InsertarParticion(FILE *Archivo,struct MBR *Estructura,int Tamano,char *Nombre,char *Tipo, char *Fitt,int TamanoMBr)
{

    struct CabeceraParticion *CabPa;
    struct OrdenarParticion *A,*B,*C,*D;
    int Contador;
    bool Estado;

    CabPa = malloc(4*sizeof(struct CabeceraParticion));
    A = malloc(4*sizeof(struct OrdenarParticion));
    B = malloc(4*sizeof(struct OrdenarParticion));
    C = malloc(4*sizeof(struct OrdenarParticion));
    D = malloc(4*sizeof(struct OrdenarParticion));


    int i;

    A->ByteInicial = Estructura->Particion1.ByteInicial;
    A->Estado = Estructura->Particion1.Estado;
    strcpy(A->Nombre,Estructura->Particion1.Nombre);
    A->Orden = Estructura->Particion1.Orden;
    A->Tamano = Estructura->Particion1.Tamano;
    strcpy( A->Tipo,Estructura->Particion1.Tipo);
    strcpy(A->TipoAjuste,Estructura->Particion1.TipoAjuste);

    B->ByteInicial = Estructura->Particion2.ByteInicial;
    B->Estado = Estructura->Particion2.Estado;
    strcpy(B->Nombre,Estructura->Particion2.Nombre);
    B->Orden = Estructura->Particion2.Orden;
    B->Tamano = Estructura->Particion2.Tamano;
    strcpy( B->Tipo,Estructura->Particion2.Tipo);
    strcpy(B->TipoAjuste,Estructura->Particion2.TipoAjuste);

    C->ByteInicial = Estructura->Particion3.ByteInicial;
    C->Estado = Estructura->Particion3.Estado;
    strcpy(C->Nombre,Estructura->Particion3.Nombre);
    C->Orden = Estructura->Particion3.Orden;
    C->Tamano = Estructura->Particion3.Tamano;
    strcpy( C->Tipo,Estructura->Particion3.Tipo);
    strcpy(C->TipoAjuste,Estructura->Particion3.TipoAjuste);

    D->ByteInicial = Estructura->Particion4.ByteInicial;
    D->Estado = Estructura->Particion4.Estado;
    strcpy(D->Nombre,Estructura->Particion4.Nombre);
    D->Orden = Estructura->Particion4.Orden;
    D->Tamano = Estructura->Particion4.Tamano;
    strcpy( D->Tipo,Estructura->Particion4.Tipo);
    strcpy(D->TipoAjuste,Estructura->Particion4.TipoAjuste);

    CabPa->Ultima =A;
    CabPa->Primera =A;
    A->Siguiente=NULL;
    A->Anterior = NULL;
    //'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    if(A->Orden>Estructura->Particion2.Orden)
    {

        B->Siguiente = A;
        A->Anterior =B;
        CabPa->Primera = B;
        CabPa->Ultima = A;
    }else
    {
        A->Siguiente =B;
        B->Anterior =A;
        CabPa->Ultima = B;
    }
    // ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    if(CabPa->Primera->Orden > Estructura->Particion3.Orden)
    {
        C->Siguiente = CabPa->Primera;
        CabPa->Primera->Anterior = C;
        CabPa->Primera =C;

    }else
    {
        if(CabPa->Primera->Siguiente->Orden > Estructura->Particion3.Orden)
        {

            C->Siguiente = CabPa->Ultima;
            C->Anterior = CabPa->Primera;
            CabPa->Primera->Siguiente = C;
            CabPa->Ultima->Anterior = C;
        }else
        {
            CabPa->Ultima->Siguiente = C;
            C->Anterior = CabPa->Ultima;
            CabPa->Ultima = C;

        }
    }

    //'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    if(CabPa->Primera->Orden>Estructura->Particion4.Orden)
    {
        CabPa->Primera->Siguiente = D;
        D->Siguiente = CabPa->Primera;
        CabPa->Primera = D;
    }else
    {
        if(CabPa->Primera->Siguiente->Orden > Estructura->Particion4.Orden)
        {
            D->Siguiente = CabPa->Primera->Siguiente;
            D->Anterior = CabPa->Primera;
            CabPa->Primera->Siguiente->Anterior = D;
            CabPa->Primera->Siguiente = D;
        }else
        {
            if(CabPa->Ultima->Orden > Estructura->Particion4.Orden)
            {
                D->Siguiente = CabPa->Ultima;
                D->Anterior = CabPa->Ultima->Anterior;
                CabPa->Ultima->Anterior->Siguiente =D;
                CabPa->Ultima->Anterior = D;


            }else
            {
                CabPa->Ultima->Siguiente = D;
                D->Anterior = CabPa->Ultima;
                CabPa->Ultima = D;
            }
        }
    }

    //''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

    if(CabPa->Ultima->Estado==1)
    {
        printf(">>>>ERROR:no hay particiones disponibles para crear <<<<\n");
    }else
    {
        Contador=0;
        struct OrdenarParticion *Auxiliar;
        Auxiliar = CabPa->Primera;
        Estado = true;

        while((Estado==true) && (Contador<4))
        {
            if(Auxiliar->Estado==0)
            {
                if(Auxiliar->Anterior==NULL)
                {
                    //Auxiliar es el primero de la lista
                    Auxiliar->ByteInicial = TamanoMBr+1;
                    Auxiliar->Estado = 1;
                    strcpy(Auxiliar->Nombre,Nombre);
                    Auxiliar->Tamano = Tamano;
                    strcpy(Auxiliar->Tipo,Tipo);
                    strcpy(Auxiliar->TipoAjuste,Fitt);
                    Auxiliar->Orden =Contador+1;

                    Estado = false;
                    if(strcasecmp(Tipo,"p")==0)
                        Estructura->NumeroPrimaria++;
                    else if(strcasecmp(Tipo,"e")==0)
                    {
                        Estructura->NumeroExtendida++;
                        struct EBR EstructuraEBR;
                        EstructuraEBR.ByteInicial = Auxiliar->ByteInicial;
                        strcpy(EstructuraEBR.Estado,"0");
                        strcpy(EstructuraEBR.Fitt,Fitt);
                        strcpy(EstructuraEBR.Nombre,"");
                        EstructuraEBR.Tamano =0;
                        EstructuraEBR.ProximoEBR=-1;
                        InsertarEBR(&EstructuraEBR,Archivo,Auxiliar->ByteInicial);


                    }

                }else
                {
                    Auxiliar->ByteInicial = Auxiliar->Anterior->ByteInicial+Auxiliar->Anterior->Tamano+1;
                    Auxiliar->Estado = 1;
                    strcpy(Auxiliar->Nombre,Nombre);
                    Auxiliar->Tamano = Tamano;
                    strcpy(Auxiliar->Tipo,Tipo);
                    strcpy(Auxiliar->TipoAjuste,Fitt);
                    Auxiliar->Orden =Contador+1;

                    Estado = false;

                    if(strcasecmp(Tipo,"p")==0)
                        Estructura->NumeroPrimaria++;
                    else if(strcasecmp(Tipo,"e")==0)
                    {
                        Estructura->NumeroExtendida++;
                        struct EBR EstructuraEBR;
                        EstructuraEBR.ByteInicial = Auxiliar->ByteInicial;
                        strcpy(EstructuraEBR.Estado,"0");
                        strcpy(EstructuraEBR.Fitt,Fitt);
                        strcpy(EstructuraEBR.Nombre,"");
                        EstructuraEBR.Tamano =0;
                        EstructuraEBR.ProximoEBR=-1;
                        InsertarEBR(&EstructuraEBR,Archivo,Auxiliar->ByteInicial);
                    }

                }

            }else
            {
                if(((Auxiliar->Siguiente->ByteInicial-Auxiliar->ByteInicial-Auxiliar->Tamano-1)>=Tamano))
                {
                    //Cuando ahy espacio entre particiones

                    struct OrdenarParticion *AuxFinal;
                    AuxFinal = CabPa->Ultima;
                    CabPa->Ultima = AuxFinal->Anterior;
                    CabPa->Ultima->Siguiente=NULL;
                    AuxFinal->Siguiente = Auxiliar->Siguiente;
                    AuxFinal->Anterior =Auxiliar;
                    Auxiliar->Siguiente->Anterior = AuxFinal;
                    Auxiliar->Siguiente = AuxFinal;

                    //Copiando valores al AuxFin
                    AuxFinal->ByteInicial = Auxiliar->Tamano+Auxiliar->ByteInicial+1;
                    AuxFinal->Estado = 1;
                    strcpy(AuxFinal->Nombre,Nombre);
                    AuxFinal->Tamano = Tamano;
                    strcpy(AuxFinal->Tipo,Tipo);
                    strcpy(AuxFinal->TipoAjuste,Fitt);

                    //Asignando nuevos Valores de orden a las particiones

                    AuxFinal->Orden = Auxiliar->Orden+1;
                    if(AuxFinal->Siguiente!=NULL)
                        AuxFinal->Siguiente->Orden++;

                    CabPa->Ultima->Orden=4;

                    Estado = false;

                    if(strcasecmp(Tipo,"p")==0)
                        Estructura->NumeroPrimaria++;
                    else if(strcasecmp(Tipo,"e")==0)
                    {
                        Estructura->NumeroExtendida++;
                        struct EBR EstructuraEBR;
                        EstructuraEBR.ByteInicial = Auxiliar->ByteInicial;
                        strcpy(EstructuraEBR.Estado,"0");
                        strcpy(EstructuraEBR.Fitt,Fitt);
                        strcpy(EstructuraEBR.Nombre,"");
                        EstructuraEBR.Tamano =0;
                        EstructuraEBR.ProximoEBR=-1;
                        InsertarEBR(&EstructuraEBR,Archivo,Auxiliar->ByteInicial);
                    }
                }
            }


            Auxiliar = Auxiliar->Siguiente;
            Contador++;
        }
    }
    if(Estado==false)
    {
        //Copiando los valores a la estructura orginal
        Estructura->Particion1.ByteInicial = A->ByteInicial;
        Estructura->Particion1.Estado = A->Estado;
        strcpy(Estructura->Particion1.Nombre,A->Nombre);
        Estructura->Particion1.Orden = A->Orden;
        Estructura->Particion1.Tamano =A->Tamano;
        strcpy(Estructura->Particion1.Tipo,A->Tipo);
        strcpy(Estructura->Particion1.TipoAjuste,A->TipoAjuste);

        Estructura->Particion2.ByteInicial = B->ByteInicial;
        Estructura->Particion2.Estado = B->Estado;
        strcpy(Estructura->Particion2.Nombre,B->Nombre);
        Estructura->Particion2.Orden = B->Orden;
        Estructura->Particion2.Tamano =B->Tamano;
        strcpy(Estructura->Particion2.Tipo,B->Tipo);
        strcpy(Estructura->Particion2.TipoAjuste,B->TipoAjuste);

        Estructura->Particion3.ByteInicial = C->ByteInicial;
        Estructura->Particion3.Estado = C->Estado;
        strcpy(Estructura->Particion3.Nombre,C->Nombre);
        Estructura->Particion3.Orden = C->Orden;
        Estructura->Particion3.Tamano =C->Tamano;
        strcpy(Estructura->Particion3.Tipo,C->Tipo);
        strcpy(Estructura->Particion3.TipoAjuste,C->TipoAjuste);

        Estructura->Particion4.ByteInicial = D->ByteInicial;
        Estructura->Particion4.Estado = D->Estado;
        strcpy(Estructura->Particion4.Nombre,D->Nombre);
        Estructura->Particion4.Orden = D->Orden;
        Estructura->Particion4.Tamano =D->Tamano;
        strcpy(Estructura->Particion4.Tipo,D->Tipo);
        strcpy(Estructura->Particion4.TipoAjuste,D->TipoAjuste);
        printf("Particion Creada correctamente\n");

    }else
        printf(">>>>ERROR: no se pueden crear mas particiones, limite alcanzado <<<<\n");


    free(A);
    free(B);
    free(C);
    free(D);
    free(CabPa);
}
//************************************** METODO PARA INSERTAR UNA PARTICION LOGICA ************************************************************************+
//***********************************************************************************************************************************************************
bool InsertarLogica(FILE *Archivo,struct Particion Part,int Tamano, char *Nombre,char *Fitt)
{
    struct EBR EstrucEBR;
    struct EBR *AuxiliarEBR;
    int Contador;
    int ByteFinal;
    int ByteFinalLogica;
    int Indice;
    bool Estado;

    Estado=false;
    Contador=0;
    AuxiliarEBR = malloc(sizeof(struct EBR));
    ByteFinal = Part.ByteInicial + Part.Tamano;

    //Estrayendo el EBR del disco
    fseek(Archivo,Part.ByteInicial,SEEK_SET);
    fread(AuxiliarEBR,sizeof(struct EBR),1,Archivo);
    while((AuxiliarEBR->Tamano!=0)&&(Estado==false))
    {
        if(AuxiliarEBR->ProximoEBR!=(-1))
        {
            //Existe una Particion Adelante se verifica si hay espacio entre ellas
            Indice = Tamano + sizeof(struct EBR);
            if((Indice) < (AuxiliarEBR->ProximoEBR - AuxiliarEBR->ByteInicial - AuxiliarEBR->Tamano))
            {
                //Si Hay Espacio para la nueva Particion
                EstrucEBR.ByteInicial = AuxiliarEBR->ByteInicial + AuxiliarEBR->Tamano + sizeof(struct EBR) +2;
                strcpy(EstrucEBR.Estado,"1");
                strcpy(EstrucEBR.Fitt,Fitt);
                strcpy(EstrucEBR.Nombre,Nombre);
                EstrucEBR.ProximoEBR = AuxiliarEBR->ProximoEBR;
                EstrucEBR.Tamano = Tamano;
                Indice = AuxiliarEBR->ByteInicial+AuxiliarEBR->Tamano+1;
                InsertarEBR(&EstrucEBR,Archivo,Indice);
                printf("Partición Lógica Creada Correctamente\n");
                Estado=true;
                //Actualizando el EBR anterior
                Indice =  AuxiliarEBR->ByteInicial+AuxiliarEBR->Tamano+1;
                AuxiliarEBR->ProximoEBR = Indice;
                Indice = AuxiliarEBR->ByteInicial - sizeof(struct EBR) -1;
                InsertarEBR(AuxiliarEBR,Archivo,Indice);

            }


        }else
        {
            //No Existe ninguna Particion Adelante
            ByteFinalLogica = sizeof(struct EBR) +Tamano +AuxiliarEBR->Tamano +AuxiliarEBR->ByteInicial;
            if(ByteFinalLogica < ByteFinal)
            {
                //Si Hay Espacio para la nueva Particion
                EstrucEBR.ByteInicial = AuxiliarEBR->ByteInicial + AuxiliarEBR->Tamano + sizeof(struct EBR)+2;
                strcpy(EstrucEBR.Estado,"1");
                strcpy(EstrucEBR.Fitt,Fitt);
                strcpy(EstrucEBR.Nombre,Nombre);
                EstrucEBR.ProximoEBR = -1;
                EstrucEBR.Tamano = Tamano;
                Indice = AuxiliarEBR->ByteInicial + AuxiliarEBR->Tamano+1;
                InsertarEBR(&EstrucEBR,Archivo,Indice);
                printf("Partición Lógica Creada Correctamente\n");
                Estado=true;
                //Actualizando el EBR anterior
                Indice =  EstrucEBR.ByteInicial -sizeof(struct EBR)-1;
                AuxiliarEBR->ProximoEBR = Indice;
                Indice = AuxiliarEBR->ByteInicial - sizeof(struct EBR )- 1;
                InsertarEBR(AuxiliarEBR,Archivo,Indice);

            }else  //La Particion NO CABE
                printf(">>>>ERROR: no hay espacio Disponible dentro de la particion extendida <<<<\n");

        }
        fseek(Archivo,AuxiliarEBR->ProximoEBR,SEEK_SET);
        free(AuxiliarEBR);
        AuxiliarEBR = malloc(sizeof(struct EBR));
        fread(AuxiliarEBR,sizeof(struct EBR),1,Archivo);
        Contador++;
    }

    if(Contador==0)
    {
        //No se a ingresado ninguna particion logica, solo se actualiza el primer EBR
        ByteFinalLogica = sizeof(struct EBR) + Tamano + AuxiliarEBR->ByteInicial;
        if(ByteFinalLogica < ByteFinal)
        {
            //La Particion SI cabe
            EstrucEBR.ByteInicial = AuxiliarEBR->ByteInicial + sizeof(struct EBR)+1;
            strcpy(EstrucEBR.Estado,"1");
            strcpy(EstrucEBR.Fitt,Fitt);
            strcpy(EstrucEBR.Nombre,Nombre);
            EstrucEBR.ProximoEBR = -1;
            EstrucEBR.Tamano = Tamano;
            InsertarEBR(&EstrucEBR,Archivo,AuxiliarEBR->ByteInicial);
            printf("Partición Lógica Creada Correctamente\n");
            Estado=true;


        }else   //La Particion NO cabe
            printf(">>>>ERROR: no hay espacio Disponible dentro de la particion extendida <<<<\n");
    }

    free(AuxiliarEBR);
    return Estado;

}
//************************************* METODO PARA CREAR UNA PARTICION LOGICA **********************************************
//************************************************************************************************************************************************
void CrearParticionLogica(FILE *Archivo, struct MBR *Estructura, int Tamano,char *Nombre,char *Fitt)
{

    bool Estado;

    if(Estructura->Particion1.Estado!=0)
    {
        if(strcasecmp(Estructura->Particion1.Tipo,"e")==0)
        {
            Estado = InsertarLogica(Archivo,Estructura->Particion1,Tamano,Nombre,Fitt);
        }
    }
    if(Estructura->Particion2.Estado!=0)
    {
        if(strcasecmp(Estructura->Particion2.Tipo,"e")==0)
        {
            Estado = InsertarLogica(Archivo,Estructura->Particion2,Tamano,Nombre,Fitt);
        }

    }
    if(Estructura->Particion3.Estado!=0)
    {
        if(strcasecmp(Estructura->Particion3.Tipo,"e")==0)
        {
            Estado = InsertarLogica(Archivo,Estructura->Particion3,Tamano,Nombre,Fitt);
        }

    }
    if(Estructura->Particion4.Estado!=0)
    {
        if(strcasecmp(Estructura->Particion4.Tipo,"e")==0)
        {
            Estado = InsertarLogica(Archivo,Estructura->Particion4,Tamano,Nombre,Fitt);
        }

    }

    if(Estado==false)
        printf(">>>>ERROR: no se ha creado ninguna particion extendida, imposible crear lógica <<<<\n");
}

//************************************* METODO PARA CREAR UNA PARTICION *******************************************************
//*************************************************************************************************************************************
void CrearParticion(char *Direccion, char *Nombre, char *Tamano,char *Unit,char *Tipo,char *Fitt)
{
    int TamanoPar,TamanoOcupado;
    char Direcc[125];
    FILE *Disco;


    strcpy(Direcc,Direccion);
    Disco = fopen(Direcc,"rb+");
    struct MBR *EstructuraMBR;

    if(Disco==NULL)
    {
        printf(">>>>ERROR: disco no encontrado <<<<\n");
        return -1;
    }



    if(Tamano!=NULL)
    {
        //Pasando a Int
        TamanoPar = atoi(Tamano);

        if((Unit==NULL)||(strcasecmp(Unit,"k")==0))
        {
            //Las Unidades son en Kilobytes+
            if((Tipo==NULL)||((strcasecmp(Tipo,"p"))==0))
            {

                //Una Particion Primaria
                printf("Particion Primaria (Kilobytes)\n");
                EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
                fread(EstructuraMBR,sizeof(struct MBR),1,Disco);

                fseek(Disco,0,SEEK_SET);
                TamanoOcupado = sizeof(struct MBR) + EstructuraMBR->Particion1.Tamano + EstructuraMBR->Particion2.Tamano + EstructuraMBR->Particion3.Tamano + EstructuraMBR->Particion4.Tamano;
                TamanoPar = TamanoPar * 1024;
                printf("Espacio Disponible antes de insertar particion: %d\n",EstructuraMBR->TamanoDisco- TamanoOcupado);
                if(EstructuraMBR->TamanoDisco>(TamanoPar+TamanoOcupado))
                {
                    if(EstructuraMBR->NumeroPrimaria<=4)
                    {
                        if(Fitt==NULL)
                            InsertarParticion(Disco,EstructuraMBR,TamanoPar,Nombre,"p","WF",TamanoOcupado);
                        else
                            InsertarParticion(Disco,EstructuraMBR,TamanoPar,Nombre,"p",Fitt,TamanoOcupado);

                        InsertarMBR(EstructuraMBR,Disco);
                    }else
                        printf(">>>>ERROR: limite de particiones primarias alcanzadas <<<<\n");
                }
                else
                    printf(">>>>ERROR: espacio no suficiente para la nueva partición <<<<\n");




            }else if((strcasecmp(Tipo,"e"))==0)
            {

                //Una Particion Extendida
                printf("Particion Extendida (Kilobytes) \n");
                EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
                fseek(Disco,0,SEEK_SET);
                fread(EstructuraMBR,sizeof(struct MBR),sizeof(EstructuraMBR),Disco);
                TamanoOcupado = sizeof(struct MBR) + EstructuraMBR->Particion1.Tamano + EstructuraMBR->Particion2.Tamano + EstructuraMBR->Particion3.Tamano + EstructuraMBR->Particion4.Tamano;
                TamanoPar = TamanoPar * 1024;
                printf("Espacio Disponible antes de insertar particion: %d\n",EstructuraMBR->TamanoDisco- TamanoOcupado);
                if(EstructuraMBR->TamanoDisco>(TamanoPar+TamanoOcupado))
                {
                    if(EstructuraMBR->NumeroExtendida==0)
                    {
                        if(Fitt==NULL)
                            InsertarParticion(Disco,EstructuraMBR,TamanoPar,Nombre,Tipo,"WF",TamanoOcupado);
                        else
                            InsertarParticion(Disco,EstructuraMBR,TamanoPar,Nombre,Tipo,Fitt,TamanoOcupado);

                        InsertarMBR(EstructuraMBR,Disco);
                    }else
                        printf(">>>>ERROR: limite de particiones extendidas alcanzadas <<<<\n");
                }
                else
                    printf(">>>>ERROR: espacio no suficiente para la nueva partición <<<<\n");


            }else if(strcasecmp(Tipo,"l")==0)
            {
                //Una Particion Logica
                printf("Particion Logica (Kilobytes)\n");
                EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
                fseek(Disco,0,SEEK_SET);
                fread(EstructuraMBR,sizeof(struct MBR),1,Disco);
                TamanoPar = TamanoPar * 1024;

                if(Fitt==NULL)
                    CrearParticionLogica(Disco,EstructuraMBR,TamanoPar,Nombre,"WF");
                else
                    CrearParticionLogica(Disco,EstructuraMBR,TamanoPar,Nombre,Fitt);


            }

        }else if(strcasecmp(Unit,"b")==0)
        {
            //Unidades en Bytes
            //Las Unidades son en Kilobytes+
            if((Tipo==NULL)||((strcasecmp(Tipo,"p"))==0))
            {
                //Una Particion Primaria

                printf("Particion Primaria (Bytes)\n");
                EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
                fread(EstructuraMBR,sizeof(struct MBR),sizeof(EstructuraMBR),Disco);
                fseek(Disco,0,SEEK_SET);
                TamanoOcupado = sizeof(struct MBR) + EstructuraMBR->Particion1.Tamano + EstructuraMBR->Particion2.Tamano + EstructuraMBR->Particion3.Tamano + EstructuraMBR->Particion4.Tamano;

                printf("Espacio Disponible antes de insertar particion: %d\n",EstructuraMBR->TamanoDisco- TamanoOcupado);
                if(EstructuraMBR->TamanoDisco>(TamanoPar+TamanoOcupado))
                {
                    if(EstructuraMBR->NumeroPrimaria<=4)
                    {
                        if(Fitt==NULL)
                            InsertarParticion(Disco,EstructuraMBR,TamanoPar,Nombre,"p","WF",TamanoOcupado);
                        else
                            InsertarParticion(Disco,EstructuraMBR,TamanoPar,Nombre,"p",Fitt,TamanoOcupado);

                        InsertarMBR(EstructuraMBR,Disco);
                    }else
                        printf(">>>>ERROR: limite de particiones primarias alcanzadas <<<<\n");
                }
                else
                    printf(">>>>ERROR: espacio no suficiente para la nueva partición <<<<\n");


            }else if((strcasecmp(Tipo,"e"))==0)
            {
                //Una Particion Extendida
                printf("Particion Extendida (Bytes)\n");
                EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
                fseek(Disco,0,SEEK_SET);
                fread(EstructuraMBR,sizeof(struct MBR),sizeof(EstructuraMBR),Disco);
                TamanoOcupado = sizeof(struct MBR) + EstructuraMBR->Particion1.Tamano + EstructuraMBR->Particion2.Tamano + EstructuraMBR->Particion3.Tamano + EstructuraMBR->Particion4.Tamano;

                printf("Espacio Disponible antes de insertar particion: %d\n",EstructuraMBR->TamanoDisco- TamanoOcupado);
                if(EstructuraMBR->TamanoDisco>(TamanoPar+TamanoOcupado))
                {
                    if(EstructuraMBR->NumeroExtendida==0)
                    {
                        if(Fitt==NULL)
                            InsertarParticion(Disco,EstructuraMBR,TamanoPar,Nombre,Tipo,"WF",TamanoOcupado);
                        else
                            InsertarParticion(Disco,EstructuraMBR,TamanoPar,Nombre,Tipo,Fitt,TamanoOcupado);

                        InsertarMBR(EstructuraMBR,Disco);
                    }else
                        printf(">>>>ERROR: limite de particiones extendidas alcanzadas <<<<\n");
                }
                else
                    printf(">>>>ERROR: espacio no suficiente para la nueva partición <<<<\n");



            }else if(strcasecmp(Tipo,"l")==0)
            {
                //Una Particion Logica
                 //Una Particion Logica
                printf("Particion Logica (Bytes)\n");
                EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
                fseek(Disco,0,SEEK_SET);
                fread(EstructuraMBR,sizeof(struct MBR),1,Disco);
                if(Fitt==NULL)
                    CrearParticionLogica(Disco,EstructuraMBR,TamanoPar,Nombre,"WF");
                else
                    CrearParticionLogica(Disco,EstructuraMBR,TamanoPar,Nombre,Fitt);


            }

        }else if(strcasecmp(Unit,"m")==0)
        {
            //Unidades en MegaBytes
            //Las Unidades son en Kilobytes+
            if((Tipo==NULL)||((strcasecmp(Tipo,"p"))==0))
            {
                //Una Particion Primaria
                printf("Particion Primaria (Megabytes)\n");
                EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
                fread(EstructuraMBR,sizeof(struct MBR),sizeof(EstructuraMBR),Disco);
                fseek(Disco,0,SEEK_SET);
                TamanoOcupado = sizeof(struct MBR) + EstructuraMBR->Particion1.Tamano + EstructuraMBR->Particion2.Tamano + EstructuraMBR->Particion3.Tamano + EstructuraMBR->Particion4.Tamano;
                TamanoPar = TamanoPar*1024*1024;
                printf("Espacio Disponible antes de insertar particion: %d\n",EstructuraMBR->TamanoDisco- TamanoOcupado);
                if(EstructuraMBR->TamanoDisco>(TamanoPar+TamanoOcupado))
                {
                    if(EstructuraMBR->NumeroPrimaria<=4)
                    {
                        if(Fitt==NULL)
                            InsertarParticion(Disco,EstructuraMBR,TamanoPar,Nombre,"p","WF",TamanoOcupado);
                        else
                            InsertarParticion(Disco,EstructuraMBR,TamanoPar,Nombre,"p",Fitt,TamanoOcupado);

                        InsertarMBR(EstructuraMBR,Disco);
                    }else
                        printf(">>>>ERROR: limite de particiones primarias alcanzadas <<<<\n");
                }
                else
                    printf(">>>>ERROR: espacio no suficiente para la nueva partición <<<<\n");

            }else if((strcasecmp(Tipo,"e"))==0)
            {
                //Una Particion Extendida
                printf("Particion Extendida (Megabytes)\n");
                EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
                fseek(Disco,0,SEEK_SET);
                fread(EstructuraMBR,sizeof(struct MBR),sizeof(EstructuraMBR),Disco);
                TamanoOcupado = sizeof(struct MBR) + EstructuraMBR->Particion1.Tamano + EstructuraMBR->Particion2.Tamano + EstructuraMBR->Particion3.Tamano + EstructuraMBR->Particion4.Tamano;
                TamanoPar = TamanoPar*1024*1024;
                printf("Espacio Disponible antes de insertar particion: %d\n",EstructuraMBR->TamanoDisco- TamanoOcupado);
                if(EstructuraMBR->TamanoDisco>(TamanoPar+TamanoOcupado))
                {
                    if(EstructuraMBR->NumeroExtendida==0)
                    {
                        if(Fitt==NULL)
                            InsertarParticion(Disco,EstructuraMBR,TamanoPar,Nombre,Tipo,"WF",TamanoOcupado);
                        else
                            InsertarParticion(Disco,EstructuraMBR,TamanoPar,Nombre,Tipo,Fitt,TamanoOcupado);

                        InsertarMBR(EstructuraMBR,Disco);
                    }else
                        printf(">>>>ERROR: limite de particiones extendidas alcanzadas <<<<\n");
                }
                else
                    printf(">>>>ERROR: espacio no suficiente para la nueva partición <<<<\n");



            }else if(strcasecmp(Tipo,"l")==0)
            {
                //Una Particion Logica
                printf("Particion Logica (Megabytes)\n");
                EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
                fseek(Disco,0,SEEK_SET);
                fread(EstructuraMBR,sizeof(struct MBR),1,Disco);
                TamanoPar = TamanoPar*1024*1024;

                if(Fitt==NULL)
                    CrearParticionLogica(Disco,EstructuraMBR,TamanoPar,Nombre,"WF");
                else
                    CrearParticionLogica(Disco,EstructuraMBR,TamanoPar,Nombre,Fitt);
            }
        }

    }else
        printf(">>>> ERROR: imposible crear particion, falta el tamano <<<<\n");


    fclose(Disco);
    free(EstructuraMBR);
}

//************************************* FUNCION PARA RESTAR BYTES A UNA PARTICION LOGICA *********************************************************
//*********************************************************************************************************************************************
bool QuitarBytesLogica(FILE *Disco, struct Particion Part,int Tamano, char *Nombre)
{

    struct EBR *EBRAuxiliar;
    int ByteLogica;
    bool Estado = false;

    EBRAuxiliar = malloc(sizeof(struct EBR));
    fseek(Disco,Part.ByteInicial,SEEK_SET);
    fread(EBRAuxiliar,sizeof(struct EBR),1,Disco);

    if(strcasecmp(EBRAuxiliar->Nombre,Nombre)==0)
    {
        if(EBRAuxiliar->Tamano <= Tamano)
        {
            //No se puede Quitar espacio
            printf(">>>>ERROR: el tamaño a borrar es mayor al de la particion <<<<\n");

        }else
        {
            //SI se puede Quitar espacio
            EBRAuxiliar->Tamano = EBRAuxiliar->Tamano - Tamano;
            InsertarEBR(EBRAuxiliar,Disco,Part.ByteInicial);
            printf("Modificación de partición %s realizada correctamente\n",Nombre);

        }
        Estado = true;
    }
    while((EBRAuxiliar->ProximoEBR!=(-1))&& Estado==false)
    {
        ByteLogica = EBRAuxiliar->ProximoEBR;
        fseek(Disco,ByteLogica,SEEK_SET);
        free(EBRAuxiliar);
        EBRAuxiliar = malloc(sizeof(struct EBR));
        fread(EBRAuxiliar,sizeof(struct EBR),1,Disco);
        if(strcasecmp(EBRAuxiliar->Nombre,Nombre)==0)
        {
            if(EBRAuxiliar->Tamano <= Tamano)
            {
                //No se puede Quitar espacio
                printf(">>>>ERROR: el tamaño a borrar es mayor al de la particion <<<<\n");
            }else
            {

                //SI se puede Quitar espacio

                EBRAuxiliar->Tamano = EBRAuxiliar->Tamano - Tamano;
                InsertarEBR(EBRAuxiliar,Disco,ByteLogica);
                printf("Modificación de partición %s realizada correctamente\n",Nombre);
            }
            Estado=true;

        }

    }
    free(EBRAuxiliar);
    return Estado;   //Retorno True, se encontro la logica, false no se encontro

}

//************************************* METODO PARA RESTAR BYTES A UNA PARTICION ***********************************************
//*******************************************************************************************************************************
void QuitarBytes(FILE *Disco, struct MBR *Estructura,int Tamano,char *Nombre)
{
    bool Estado = false;


    if(strcasecmp(Estructura->Particion1.Nombre,Nombre)==0)
    {
        if(Estructura->Particion1.Tamano<=Tamano)
        {
            //La partición
            printf(">>>>ERROR: el tamaño a borrar es mayor al de la particion <<<<\n");

        }else
        {
            //Simplemente se aplicaria una Reduccion en tamaño
            Estructura->Particion1.Tamano = Estructura->Particion1.Tamano-Tamano;
            printf("Modificación de partición %s realizada correctamente\n",Nombre);

        }

        Estado=true;


    }else if(strcasecmp(Estructura->Particion2.Nombre,Nombre)==0)
    {
        if(Estructura->Particion2.Tamano<=Tamano)
        {
            //La partición quedaría con Tamaño CERO
            printf(">>>>ERROR: el tamaño a borrar es mayor al de la particion <<<<\n");

        }else
        {
            //Simplemente se aplicaria una Reduccion en tamaño
            Estructura->Particion2.Tamano = Estructura->Particion2.Tamano - Tamano;
            printf("Modificación de partición %s realizada correctamente\n",Nombre);

        }
        Estado=true;

    }else if(strcasecmp(Estructura->Particion3.Nombre,Nombre)==0)
    {
        if(Estructura->Particion3.Tamano<=Tamano)
        {
            //La partición quedaría con Tamaño CERO
            printf(">>>>ERROR: el tamaño a borrar es mayor al de la particion <<<<\n");

        }else
        {
            //Simplemente se aplicaria una Reduccion en tamaño
            Estructura->Particion3.Tamano = Estructura->Particion3.Tamano - Tamano;
            printf("Modificación de partición %s realizada correctamente\n",Nombre);

        }
        Estado=true;

    }else if(strcasecmp(Estructura->Particion4.Nombre,Nombre)==0)
    {

        if(Estructura->Particion4.Tamano <=Tamano)
        {
            //La partición quedaría con Tamaño CERO
            printf(">>>>ERROR: el tamaño a borrar es mayor al de la particion <<<<\n");

        }else
        {
            //Simplemente se aplicaria una Reduccion en tamaño
            Estructura->Particion4.Tamano = Estructura->Particion4.Tamano -Tamano;
            printf("Modificación de partición %s realizada correctamente\n",Nombre);

        }
        Estado=true;

    }

    //Validando si la particion es una logica
    if(Estado==false)
    {
        //Significa que la particion solicitada no se ha encontrado
        if(strcasecmp(Estructura->Particion1.Tipo,"e")==0)
        {
            Estado = QuitarBytesLogica(Disco,Estructura->Particion1,Tamano,Nombre);

        }else if(strcasecmp(Estructura->Particion2.Tipo,"e")==0)
        {
            Estado = QuitarBytesLogica(Disco,Estructura->Particion2,Tamano,Nombre);
        }else if(strcasecmp(Estructura->Particion3.Tipo,"e")==0)
        {
            Estado = QuitarBytesLogica(Disco,Estructura->Particion3,Tamano,Nombre);
        }else if(strcasecmp(Estructura->Particion4.Tipo,"e")==0)
        {
            Estado = QuitarBytesLogica(Disco,Estructura->Particion4,Tamano,Nombre);
        }

        if(Estado==false)
            printf(">>>>ERROR: no se pudo encontrar la Particion: %s <<<<\n",Nombre);
    }


}

//************************************* METODO PARA AGREGAR BYTES A UNA PARTICION ****************************************************************
//*************************************************************************************************************************************************
void AgregarBytes(FILE *Disco, struct MBR *Estructura,int Tamano,char *Nombre)
{

    struct CabeceraParticion *CabPa;
    struct OrdenarParticion *A,*B,*C,*D;
    int ByteFinal;
    bool Estado;
    int ByteLogica;

    CabPa = malloc(4*sizeof(struct CabeceraParticion));
    A = malloc(4*sizeof(struct OrdenarParticion));
    B = malloc(4*sizeof(struct OrdenarParticion));
    C = malloc(4*sizeof(struct OrdenarParticion));
    D = malloc(4*sizeof(struct OrdenarParticion));


    int i;

    A->ByteInicial = Estructura->Particion1.ByteInicial;
    A->Estado = Estructura->Particion1.Estado;
    strcpy(A->Nombre,Estructura->Particion1.Nombre);
    strcpy(A->Tipo,Estructura->Particion1.Tipo);
    A->Orden = Estructura->Particion1.Orden;
    A->Tamano = Estructura->Particion1.Tamano;


    B->ByteInicial = Estructura->Particion2.ByteInicial;
    B->Estado = Estructura->Particion2.Estado;
    strcpy(B->Nombre,Estructura->Particion2.Nombre);
    strcpy(B->Tipo,Estructura->Particion2.Tipo);
    B->Orden = Estructura->Particion2.Orden;
    B->Tamano = Estructura->Particion2.Tamano;


    C->ByteInicial = Estructura->Particion3.ByteInicial;
    C->Estado = Estructura->Particion3.Estado;
    strcpy(C->Nombre,Estructura->Particion3.Nombre);
    strcpy(C->Tipo,Estructura->Particion3.Tipo);
    C->Orden = Estructura->Particion3.Orden;
    C->Tamano = Estructura->Particion3.Tamano;


    D->ByteInicial = Estructura->Particion4.ByteInicial;
    D->Estado = Estructura->Particion4.Estado;
    strcpy(D->Nombre,Estructura->Particion4.Nombre);
    strcpy(D->Tipo,Estructura->Particion4.Tipo);
    D->Orden = Estructura->Particion4.Orden;
    D->Tamano = Estructura->Particion4.Tamano;


    CabPa->Ultima =A;
    CabPa->Primera =A;
    A->Siguiente=NULL;
    A->Anterior = NULL;
    //'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    if(A->Orden > Estructura->Particion2.Orden)
    {

        B->Siguiente = A;
        A->Anterior =B;
        CabPa->Primera = B;
        CabPa->Ultima = A;
    }else
    {
        A->Siguiente =B;
        B->Anterior =A;
        CabPa->Ultima = B;
    }
    // ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    if(CabPa->Primera->Orden > Estructura->Particion3.Orden)
    {
        C->Siguiente = CabPa->Primera;
        CabPa->Primera->Anterior = C;
        CabPa->Primera =C;

    }else
    {
        if(CabPa->Primera->Siguiente->Orden > Estructura->Particion3.Orden)
        {

            C->Siguiente = CabPa->Ultima;
            C->Anterior = CabPa->Primera;
            CabPa->Primera->Siguiente = C;
            CabPa->Ultima->Anterior = C;
        }else
        {
            CabPa->Ultima->Siguiente = C;
            C->Anterior = CabPa->Ultima;
            CabPa->Ultima = C;

        }
    }

    //'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    if(CabPa->Primera->Orden > Estructura->Particion4.Orden)
    {
        CabPa->Primera->Siguiente = D;
        D->Siguiente = CabPa->Primera;
        CabPa->Primera = D;
    }else
    {
        if(CabPa->Primera->Siguiente->Orden > Estructura->Particion4.Orden)
        {
            D->Siguiente = CabPa->Primera->Siguiente;
            D->Anterior = CabPa->Primera;
            CabPa->Primera->Siguiente->Anterior = D;
            CabPa->Primera->Siguiente = D;
        }else
        {
            if(CabPa->Ultima->Orden > Estructura->Particion4.Orden)
            {
                D->Siguiente = CabPa->Ultima;
                D->Anterior = CabPa->Ultima->Anterior;
                CabPa->Ultima->Anterior->Siguiente =D;
                CabPa->Ultima->Anterior = D;


            }else
            {
                CabPa->Ultima->Siguiente = D;
                D->Anterior = CabPa->Ultima;
                CabPa->Ultima = D;
            }
        }
    }


    i=0;
    struct OrdenarParticion *Auxiliar;
    Auxiliar = CabPa->Primera;
    Estado = false;
    while(i<4)
    {
        if(Auxiliar->Estado!=0)
        {
            //Verificando si la Particion es Extendida e ir a buscar si es una particion Primaria
            if(strcasecmp(Auxiliar->Tipo,"e")==0)
            {
                struct EBR *EBRAuxiliar;
                EBRAuxiliar = malloc(sizeof(struct EBR));
                fseek(Disco,Auxiliar->ByteInicial,SEEK_SET);
                fread(EBRAuxiliar,sizeof(struct EBR),1,Disco);
                if(strcasecmp(EBRAuxiliar->Nombre,Nombre)==0)
                {
                    //La primera Particion Lógica es la que se va a modificar
                    //Validando se existe el espacio necesario
                    if((EBRAuxiliar->ProximoEBR - EBRAuxiliar->ByteInicial - EBRAuxiliar->Tamano - 1)!=0)
                    {
                        //Si hay Espacio
                        EBRAuxiliar->Tamano = EBRAuxiliar->Tamano + Tamano;
                        InsertarEBR(EBRAuxiliar,Disco,Auxiliar->ByteInicial);

                    }else
                        printf(">>>>ERROR: No hay espacio disponible para agrandar partición %s <<<<\n",Nombre);

                    free(EBRAuxiliar);
                }else
                {
                    //Se verifica si entre las demas particiones lógicas esta la solicitada
                     while((EBRAuxiliar->ProximoEBR!=(-1)) && (Estado==false) )
                    {
                         ByteLogica = EBRAuxiliar->ProximoEBR;
                         fseek(Disco,ByteLogica,SEEK_SET);
                         free(EBRAuxiliar);
                         EBRAuxiliar = malloc(sizeof(struct EBR));
                         fread(EBRAuxiliar,sizeof(struct EBR),1,Disco);

                         if(strcasecmp(EBRAuxiliar->Nombre,Nombre)==0)
                         {
                             if((EBRAuxiliar->ProximoEBR - EBRAuxiliar->ByteInicial - EBRAuxiliar->Tamano - 1)!=0)
                            {
                                //Si hay Espacio
                                EBRAuxiliar->Tamano = EBRAuxiliar->Tamano + Tamano;
                                InsertarEBR(EBRAuxiliar,Disco,ByteLogica);
                                Estado=true;
                            }else
                                printf(">>>>ERROR: No hay espacio disponible para agrandar partición %s <<<<\n",Nombre);
                         }

                    }
                    free(EBRAuxiliar);

                }

            }

            if(strcasecmp(Auxiliar->Nombre,Nombre)==0)
            {

                if(Auxiliar->Siguiente!=NULL)
                {
                    //El Particion NO esta al final de la lista
                    ByteFinal = Auxiliar->ByteInicial + Auxiliar->Tamano + Tamano;
                    if(ByteFinal < Auxiliar->Siguiente->ByteInicial)
                    {
                        Auxiliar->Tamano = Auxiliar->Tamano + Tamano;
                        printf("Particion %s, modificada correctamente\n",Nombre);
                        Estado = true;

                    }else
                        printf(">>>>ERROR: No hay espacio disponible para agrandar partición %s <<<<\n",Nombre);

                }else
                {
                    //Particion SI esta al final de la Lista
                    ByteFinal = Auxiliar->ByteInicial + Auxiliar->Tamano + Tamano;
                    if(ByteFinal < Estructura->TamanoDisco)
                    {
                        Auxiliar->Tamano = Auxiliar->Tamano + Tamano;
                        printf("Particion %s, modificada correctamente\n",Nombre);
                        Estado = true;
                    }else
                        printf(">>>>ERROR: No hay espacio disponible para agrandar partición %s <<<<\n",Nombre);



                }
            }

        }

        i++;
        Auxiliar = Auxiliar->Siguiente;
    }

    if(Estado)
    {
        Estructura->Particion1.Tamano = A->Tamano;
        Estructura->Particion2.Tamano = B->Tamano;
        Estructura->Particion3.Tamano = C->Tamano;
        Estructura->Particion4.Tamano = D->Tamano;
    }


    free(A);
    free(B);
    free(C);
    free(D);
    free(CabPa);

}
//************************************* METODO PARA MODIFICAR UNA PARTICION ************************************************************
//***************************************************************************************************************************************
void ModificarParticion(char *ByteModificacion,char *Direccion,char *Nombre,char *Unit)
{
    bool TipoAdd;
    int TamanoReal;
    char Direcc[125];
    struct MBR *EstructuraMBR;
    FILE *Disco;


    strcpy(Direcc,Direccion);
    Disco = fopen(Direcc,"rb+");


    if(Disco==NULL)
    {
        printf(">>>>ERROR: disco no encontrado <<<<\n");
        return -1;
    }

    if(*ByteModificacion=='-')
    {
        TipoAdd = false;   // RESTAR
        ByteModificacion++;
        TamanoReal = atoi(ByteModificacion);
    }
    else
    {
        TipoAdd = true;   // SUMAR
        TamanoReal = atoi(ByteModificacion);
    }

    //'''''''''''''''''''''''''''''''''''''''''''''''''''''''
    if((Unit==NULL)||(strcasecmp(Unit,"k")==0))
    {
        //Modificacion en Kilobytes
        TamanoReal = TamanoReal*1024;
        EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
        fseek(Disco,0,SEEK_SET);
        fread(EstructuraMBR,sizeof(struct MBR),sizeof(EstructuraMBR),Disco);


        if(TipoAdd)
        {
            //sumar
            AgregarBytes(Disco,EstructuraMBR,TamanoReal,Nombre);
            InsertarMBR(EstructuraMBR,Disco);
        }else
        {
            QuitarBytes(Disco,EstructuraMBR,TamanoReal,Nombre);
            InsertarMBR(EstructuraMBR,Disco);
        }


    }else if(strcasecmp(Unit,"b")==0)
    {
        //Modificacion en bytes
        printf("Modificacion en bytes\n");
        EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
        fseek(Disco,0,SEEK_SET);
        fread(EstructuraMBR,sizeof(struct MBR),sizeof(EstructuraMBR),Disco);


        if(TipoAdd)
        {
            //sumar
             AgregarBytes(Disco,EstructuraMBR,TamanoReal,Nombre);
            InsertarMBR(EstructuraMBR,Disco);
        }else
        {
            QuitarBytes(Disco,EstructuraMBR,TamanoReal,Nombre);
            InsertarMBR(EstructuraMBR,Disco);
        }

    }else if(strcasecmp(Unit,"m")==0)
    {
        //Modificacion en Megabytes
        TamanoReal = TamanoReal *1024*1024;
        printf("Modificacion en Megabytes\n");
        EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
        fseek(Disco,0,SEEK_SET);
        fread(EstructuraMBR,sizeof(struct MBR),sizeof(EstructuraMBR),Disco);


        if(TipoAdd)
        {
            //sumar
            AgregarBytes(Disco,EstructuraMBR,TamanoReal,Nombre);
            InsertarMBR(EstructuraMBR,Disco);
        }else
        {
            QuitarBytes(Disco,EstructuraMBR,TamanoReal,Nombre);
            InsertarMBR(EstructuraMBR,Disco);
        }
    }

    fclose(Disco);
    free(EstructuraMBR);


}
//***************************************** METODO PARA BORRAR UNA PARTICION LOGICA *******************************************************
//*******************************************************************************************************************************************
bool BorrarParticionLogica(FILE *Disco,struct Particion Part,char *Nombre,bool TipoBor)
{
    struct EBR *EBRAuxiliar;
    int ByteLogica;
    bool Estado = false;

    EBRAuxiliar = malloc(sizeof(struct EBR));
    fseek(Disco,Part.ByteInicial,SEEK_SET);
    fread(EBRAuxiliar,sizeof(struct EBR),1,Disco);

    if(strcasecmp(EBRAuxiliar->Nombre,Nombre)==0)
    {
        //la Particion se Encuentra en el Primer EBR
        //Solo se actualiza el EBR
        EBRAuxiliar->ByteInicial=Part.ByteInicial;
        strcpy(EBRAuxiliar->Estado,"0");
        strcpy(EBRAuxiliar->Fitt,"");
        strcpy(EBRAuxiliar->Nombre,"");
        EBRAuxiliar->Tamano=0;
        InsertarEBR(EBRAuxiliar,Disco,Part.ByteInicial);
        Estado=true;
    }

    while((EBRAuxiliar->ProximoEBR!=(-1))&& Estado==false)
    {
        struct EBR *EBRSiguiente;

        ByteLogica = EBRAuxiliar->ProximoEBR;
        fseek(Disco,ByteLogica,SEEK_SET);

        EBRSiguiente = malloc(sizeof(struct EBR));
        fread(EBRSiguiente,sizeof(struct EBR),1,Disco);
        if(strcasecmp(EBRSiguiente->Nombre,Nombre)==0)
        {
            //La particion solicitada se encuentra despues del segundo EBR

            //Actualizando los EBR anterior
            EBRAuxiliar->ProximoEBR = EBRSiguiente->ProximoEBR;
            InsertarEBR(EBRAuxiliar,Disco,EBRAuxiliar->ByteInicial - 1 - sizeof(struct EBR));

            //Borrando el EBR
            char Rell[1];
            Rell[0]='\0';
            fseek(Disco,ByteLogica,SEEK_SET);
            while(ByteLogica < EBRSiguiente->ByteInicial)
            {
                fwrite(Rell,1,1,Disco);
                ByteLogica++;
            }

            if(TipoBor)//TipoBor == true, es el borrado FULL y se rellena con \0 el espacio
            {
                ByteLogica = EBRSiguiente->ByteInicial;
                fseek(Disco,ByteLogica,SEEK_SET);
                while(ByteLogica < EBRSiguiente->ByteInicial + EBRSiguiente->Tamano)
                {
                    fwrite(Rell,1,1,Disco);
                    ByteLogica++;
                }

            }
            printf("Particion %s, borrada correctamente\n",Nombre);
            Estado=true;
        }else
        {
            //Corriendo los apuntadores
            fseek(Disco,EBRAuxiliar->ProximoEBR,SEEK_SET);
            free(EBRAuxiliar);
            EBRAuxiliar = malloc(sizeof(struct EBR));
            fread(EBRAuxiliar,sizeof(struct EBR),1,Disco);

        }
        free(EBRSiguiente);
    }
    free(EBRAuxiliar);
    return Estado;   //Retorno True, se encontro la logica, false no se encontro
}

//************************************** METODO PARA BORRAR UNA PARTICION **********************************************************************************
//**********************************************************************************************************************************************************************
void BorrarPar(FILE *Disco,struct MBR *Estructura,char *Nombre,bool TipoBorrado)
{
    bool Estado = false;
    if(strcasecmp(Estructura->Particion1.Nombre,Nombre)==0)
    {
        if(Estructura->Particion1.Orden < Estructura->Particion2.Orden)
        {
            Estructura->Particion2.Orden--;
        }

        if(Estructura->Particion1.Orden < Estructura->Particion3.Orden)
        {
            Estructura->Particion3.Orden--;
        }

        if(Estructura->Particion1.Orden < Estructura->Particion4.Orden)
        {
            Estructura->Particion4.Orden--;
        }

        Estructura->Particion1.Estado=0;
        Estructura->Particion1.ByteInicial=0;
        strcpy(Estructura->Particion1.Nombre,"");
        Estructura->Particion1.Orden =4;
        Estructura->Particion1.Tamano =0;
        strcpy(Estructura->Particion1.Tipo,"");
        strcpy(Estructura->Particion1.TipoAjuste,"");

        printf("Particion de nombre %s Borrada correctamente\n",Nombre);
        Estado=true;

    }else if(strcasecmp(Estructura->Particion2.Nombre,Nombre)==0)
    {
        if(Estructura->Particion2.Orden < Estructura->Particion1.Orden)
            Estructura->Particion1.Orden--;

        if(Estructura->Particion2.Orden < Estructura->Particion3.Orden)
            Estructura->Particion3.Orden--;

        if(Estructura->Particion2.Orden < Estructura->Particion4.Orden)
            Estructura->Particion4.Orden--;

        Estructura->Particion2.Estado=0;
        Estructura->Particion2.ByteInicial=0;
        strcpy(Estructura->Particion2.Nombre,"");
        Estructura->Particion2.Orden =4;
        Estructura->Particion2.Tamano =0;
        strcpy(Estructura->Particion2.Tipo,"");
        strcpy(Estructura->Particion2.TipoAjuste,"");

        printf("Particion de nombre %s Borrada correctamente\n",Nombre);
        Estado=true;

    }else if(strcasecmp(Estructura->Particion3.Nombre,Nombre)==0)
    {
        if(Estructura->Particion3.Orden < Estructura->Particion1.Orden)
            Estructura->Particion1.Orden--;

        if(Estructura->Particion3.Orden < Estructura->Particion2.Orden)
            Estructura->Particion2.Orden--;

        if(Estructura->Particion3.Orden < Estructura->Particion4.Orden)
            Estructura->Particion4.Orden--;

        Estructura->Particion3.Estado=0;
        Estructura->Particion3.ByteInicial=0;
        strcpy(Estructura->Particion3.Nombre,"");
        Estructura->Particion3.Orden =4;
        Estructura->Particion3.Tamano =0;
        strcpy(Estructura->Particion3.Tipo,"");
        strcpy(Estructura->Particion3.TipoAjuste,"");

        printf("Particion de nombre %s Borrada correctamente\n",Nombre);
        Estado=true;


    }else if(strcasecmp(Estructura->Particion4.Nombre,Nombre)==0)
    {
        if(Estructura->Particion4.Orden < Estructura->Particion1.Orden)
            Estructura->Particion1.Orden--;

        if(Estructura->Particion4.Orden < Estructura->Particion2.Orden)
            Estructura->Particion2.Orden--;

        if(Estructura->Particion4.Orden < Estructura->Particion3.Orden)
            Estructura->Particion3.Orden--;

        Estructura->Particion4.Estado=0;
        Estructura->Particion4.ByteInicial=0;
        strcpy(Estructura->Particion4.Nombre,"");
        Estructura->Particion4.Orden =4;
        Estructura->Particion4.Tamano =0;
        strcpy(Estructura->Particion4.Tipo,"");
        strcpy(Estructura->Particion4.TipoAjuste,"");

        printf("Particion de nombre %s Borrada correctamente\n",Nombre);
        Estado=true;
    }

    //Buscando en la Particiones Logicas
    if(Estado==false)
    {
        if(strcasecmp(Estructura->Particion1.Tipo,"e")==0)
        {
            Estado = BorrarParticionLogica(Disco,Estructura->Particion1,Nombre,TipoBorrado);


        }else if(strcasecmp(Estructura->Particion2.Tipo,"e")==0)
        {
            Estado = BorrarParticionLogica(Disco,Estructura->Particion2,Nombre,TipoBorrado);

        }else if(strcasecmp(Estructura->Particion3.Tipo,"e")==0)
        {
            Estado = BorrarParticionLogica(Disco,Estructura->Particion3,Nombre,TipoBorrado);

        }else if(strcasecmp(Estructura->Particion4.Tipo,"e")==0)
        {
            Estado = BorrarParticionLogica(Disco,Estructura->Particion4,Nombre,TipoBorrado);
        }

        if(Estado==false)
            printf(">>>>ERROR: imposible borrar particion %s, no se encuentra en el diso <<<<\n",Nombre);
    }
}

//************************************** METODO PARA ELIMINAR UNA PARTICION ***********************************************************************
//***************************************************************************************************************************************************
void EliminarParticion(char *Direccion,char *Nombre,char *TipoBorrado)
{
    char Direcc[125];
    struct MBR *EstructuraMBR;
    FILE *Disco;


    strcpy(Direcc,Direccion);
    Disco = fopen(Direcc,"rb+");

    if(Disco==NULL)
    {
        printf(">>>>ERROR: disco no encontrado <<<<\n");
        return -1;
    }

    //''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    if(strcasecmp(TipoBorrado,"fast")==0)
    {
        //Borrado tipo Fast
        printf("Borrando en Fast\n");
        EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
        fseek(Disco,0,SEEK_SET);
        fread(EstructuraMBR,sizeof(struct MBR),sizeof(EstructuraMBR),Disco);
        BorrarPar(Disco,EstructuraMBR,Nombre,false);
        InsertarMBR(EstructuraMBR,Disco);

    }else
    {
        printf("Borrado en full\n");
        EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
        fseek(Disco,0,SEEK_SET);
        fread(EstructuraMBR,sizeof(struct MBR),sizeof(EstructuraMBR),Disco);
        BorrarPar(Disco,EstructuraMBR,Nombre,true);
        InsertarMBR(EstructuraMBR,Disco);
    }

    fclose(Disco);
    free(EstructuraMBR);
}




//******************************* METODO PARA DESMONTAR UNA PARTICION *****************************************************************
//*********************************************************************************************************************************************
void DesmontarParticion(char *IdParticion)
{
    char Prueba[12];
    int NumeroParticion;
    IdParticion++;
    IdParticion++;
    IdParticion++;
    NumeroParticion =(atoi(IdParticion));
    IdParticion--;
    DesmontParticion(NumeroParticion,IdParticion);
}


//************************************* METODO PARA GRAFICAR EL MBR **************************************************************************
//********************************************************************************************************************************************************

void CrearReporteMBR(char *Direccion, char *id)
{
    FILE *Grafica;
    FILE *Disco;
    struct MBR *EstrucMBR;
    struct EBR *EBREstruc;
    int Numero;
    int Numero2;
    char DiscoDireccion[125];
    char *Dir;


    Numero=0;
    Numero2=0;
    strcpy(DiscoDireccion,DevolverPath(id));
    if(strcasecmp(DiscoDireccion,"a")!=0)
    {
         Grafica =  fopen("Grafica.dot","w+");
        if(Grafica==NULL)
        {
            printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
            return -1;
        }


         Disco = fopen(DiscoDireccion,"rb");
         if(Disco==NULL)
         {
             printf(">>>>ERROR: no se pudo abrir el Disco para su lectura <<<<\n");
             return -1;
         }

         EstrucMBR = malloc(4*sizeof(struct MBR)+1);
         fseek(Disco,0,SEEK_SET);
         fread(EstrucMBR,sizeof(struct MBR),1,Disco);


        fprintf(Grafica,"digraph G {\n");
        fprintf(Grafica,"nodesep=.1;\n");
        fprintf(Grafica,"\trankdir=LR;\n");
        fprintf(Grafica,"\tnode [shape=record,width=.1,height=.1];\n");
        fprintf(Grafica,"\t subgraph cluster0{\n");
            fprintf(Grafica,"\tlabel=\"ESTRUCTURA DEL DISCO\"\n");
            //Graficando Las Particiones NO vacias



            fprintf(Grafica,"node%d [label = \"<f3> MR | NOMBRE      VALOR  | Tamaño:  %d | Identif.: %d | Fecha %s\", height=1];\n",Numero,EstrucMBR->TamanoDisco,EstrucMBR->IdIdentif,EstrucMBR->Fecha);
            Numero++;
            if(EstrucMBR->Particion1.Estado!=0)
            {
                fprintf(Grafica,"node%d [label = \"<f3> PARTICION 1 |NOMBRE      VALOR | Nombre: %s | Estado: %d | Tipo: %s | Ajuste: %s | Byte Ini.: %d | Tamaño: %d\", height=1];\n",Numero,EstrucMBR->Particion1.Nombre,EstrucMBR->Particion1.Estado,EstrucMBR->Particion1.Tipo,EstrucMBR->Particion1.TipoAjuste,EstrucMBR->Particion1.ByteInicial,EstrucMBR->Particion1.Tamano);
                if(strcasecmp(EstrucMBR->Particion1.Tipo,"e")==0)
                {
                    EBREstruc = malloc(sizeof(struct EBR));
                    fseek(Disco,EstrucMBR->Particion1.ByteInicial,SEEK_SET);
                    fread(EBREstruc,sizeof(struct EBR),1,Disco);
                    fprintf(Grafica,"node%d:f3 -> node%d%d:f%d [dir=both];",Numero,Numero,Numero2,Numero2);
                    fprintf(Grafica,"node%d%d [label = \"<f%d> EBR %d | Nombre: %s |Estado: %s |Ajuste: %s| Byte Inicial: %d|Tamaño: %d|Sguiente EBR: %d|\", height=1];",Numero,Numero2,Numero2,Numero2+1,EBREstruc->Nombre,EBREstruc->Estado,EBREstruc->Fitt,EBREstruc->ByteInicial,EBREstruc->Tamano,EBREstruc->ProximoEBR);
                    while(EBREstruc->ProximoEBR!=(-1))
                    {
                        fseek(Disco,EBREstruc->ProximoEBR,SEEK_SET);
                        free(EBREstruc);
                        EBREstruc = malloc(sizeof(struct EBR));
                        fread(EBREstruc,sizeof(struct EBR),1,Disco);
                        fprintf(Grafica,"node%d%d:f%d -> node%d%d:f%d [dir=both];",Numero,Numero2,Numero2,Numero,Numero2+1,Numero2+1);
                        Numero2++;
                        fprintf(Grafica,"node%d%d [label = \"<f%d> EBR %d | Nombre: %s |Estado: %s |Ajuste: %s| Byte Inicial: %d|Tamaño: %d|Siguiente EBR: %i|\", height=1];",Numero,Numero2,Numero2,Numero2+1,EBREstruc->Nombre,EBREstruc->Estado,EBREstruc->Fitt,EBREstruc->ByteInicial,EBREstruc->Tamano,EBREstruc->ProximoEBR);
                    }
                     free(EBREstruc);

                }
                Numero++;
            }
            if(EstrucMBR->Particion2.Estado!=0)
            {
                fprintf(Grafica,"node%d [label = \"<f3> PARTICION 2 |NOMBRE      VALOR| Nombre: %s | Estado: %d | Tipo: %s | Ajuste: %s | Byte Ini.: %d | Tamaño: %d\", height=1];\n",Numero,EstrucMBR->Particion2.Nombre,EstrucMBR->Particion2.Estado,EstrucMBR->Particion2.Tipo,EstrucMBR->Particion2.TipoAjuste,EstrucMBR->Particion2.ByteInicial,EstrucMBR->Particion2.Tamano);
                if(strcasecmp(EstrucMBR->Particion2.Tipo,"e")==0)
                {
                    EBREstruc = malloc(sizeof(struct EBR));
                    fseek(Disco,EstrucMBR->Particion2.ByteInicial,SEEK_SET);
                    fread(EBREstruc,sizeof(struct EBR),1,Disco);
                    fprintf(Grafica,"node%d:f3 -> node%d%d:f%d [dir=both];",Numero,Numero,Numero2,Numero2);
                    fprintf(Grafica,"node%d%d [label = \"<f%d> EBR %d | Nombre: %s |Estado: %s |Ajuste: %s| Byte Inicial: %d|Tamaño: %d|Siguiente EBR: %i|\", height=1];",Numero,Numero2,Numero2,Numero2+1,EBREstruc->Nombre,EBREstruc->Estado,EBREstruc->Fitt,EBREstruc->ByteInicial,EBREstruc->Tamano,EBREstruc->ProximoEBR);
                    while(EBREstruc->ProximoEBR!=(-1))
                    {
                        fseek(Disco,EBREstruc->ProximoEBR,SEEK_SET);
                        free(EBREstruc);
                        EBREstruc = malloc(sizeof(struct EBR));
                        fread(EBREstruc,sizeof(struct EBR),1,Disco);
                        fprintf(Grafica,"node%d%d:f%d -> node%d%d:f%d [dir=both];",Numero,Numero2,Numero2,Numero,Numero2+1,Numero2+1);
                        Numero2++;
                        fprintf(Grafica,"node%d%d [label = \"<f%d> EBR %d | Nombre: %s |Estado: %s |Ajuste: %s| Byte Inicial: %d|Tamaño: %d|Siguiente EBR: %i|\", height=1];",Numero,Numero2,Numero2,Numero2+1,EBREstruc->Nombre,EBREstruc->Estado,EBREstruc->Fitt,EBREstruc->ByteInicial,EBREstruc->Tamano,EBREstruc->ProximoEBR);
                    }
                    free(EBREstruc);

                }
                Numero++;
            }
            if(EstrucMBR->Particion3.Estado!=0)
            {
                fprintf(Grafica,"node%d [label = \"<f3> PARTICION 3 |NOMBRE      VALOR| Nombre: %s | Estado: %d | Tipo: %s | Ajuste: %s | Byte Ini.: %d | Tamaño: %d\", height=1];\n",Numero,EstrucMBR->Particion3.Nombre,EstrucMBR->Particion3.Estado,EstrucMBR->Particion3.Tipo,EstrucMBR->Particion3.TipoAjuste,EstrucMBR->Particion3.ByteInicial,EstrucMBR->Particion3.Tamano);
                if(strcasecmp(EstrucMBR->Particion3.Tipo,"e")==0)
                {
                    //La Particion es Extendida
                    EBREstruc = malloc(sizeof(struct EBR));
                    fseek(Disco,EstrucMBR->Particion3.ByteInicial,SEEK_SET);
                    fread(EBREstruc,sizeof(struct EBR),1,Disco);
                    fprintf(Grafica,"node%d:f3 -> node%d%d:f%d [dir=both];",Numero,Numero,Numero2,Numero2);
                    fprintf(Grafica,"node%d%d [label = \"<f%d> EBR %d | Nombre: %s |Estado: %s |Ajuste: %s| Byte Inicial: %d|Tamaño: %d|Siguiente EBR: %i|\", height=1];",Numero,Numero2,Numero2,Numero2+1,EBREstruc->Nombre,EBREstruc->Estado,EBREstruc->Fitt,EBREstruc->ByteInicial,EBREstruc->Tamano,EBREstruc->ProximoEBR);
                    while(EBREstruc->ProximoEBR!=(-1))
                    {
                        fseek(Disco,EBREstruc->ProximoEBR,SEEK_SET);
                        free(EBREstruc);
                        EBREstruc = malloc(sizeof(struct EBR));
                        fread(EBREstruc,sizeof(struct EBR),1,Disco);
                        fprintf(Grafica,"node%d%d:f%d -> node%d%d:f%d [dir=both];",Numero,Numero2,Numero2,Numero,Numero2+1,Numero2+1);
                        Numero2++;
                        fprintf(Grafica,"node%d%d [label = \"<f%d> EBR %d | Nombre: %s |Estado: %s |Ajuste: %s| Byte Inicial: %d|Tamaño: %d|Siguiente EBR: %i|\", height=1];",Numero,Numero2,Numero2,Numero2+1,EBREstruc->Nombre,EBREstruc->Estado,EBREstruc->Fitt,EBREstruc->ByteInicial,EBREstruc->Tamano,EBREstruc->ProximoEBR);
                    }
                    free(EBREstruc);

                }
                Numero++;
            }
            if(EstrucMBR->Particion4.Estado!=0)
            {
                fprintf(Grafica,"node%d [label = \"<f3> PARTICION 4 |NOMBRE      VALOR| Nombre: %s | Estado: %d | Tipo: %s | Ajuste: %s | Byte Ini.: %d | Tamaño: %d\", height=1];\n",Numero,EstrucMBR->Particion4.Nombre,EstrucMBR->Particion4.Estado,EstrucMBR->Particion4.Tipo,EstrucMBR->Particion4.TipoAjuste,EstrucMBR->Particion4.ByteInicial,EstrucMBR->Particion4.Tamano);
                if(strcasecmp(EstrucMBR->Particion4.Tipo,"e")==0)
                {
                    //La Particion es Extendida
                    EBREstruc = malloc(sizeof(struct EBR));
                    fseek(Disco,EstrucMBR->Particion4.ByteInicial,SEEK_SET);
                    fread(EBREstruc,sizeof(struct EBR),1,Disco);
                    fprintf(Grafica,"node%d:f3 -> node%d%d:f%d [dir=both];",Numero,Numero,Numero2,Numero2);
                    fprintf(Grafica,"node%d%d [label = \"<f%d> EBR %d | Nombre: %s |Estado: %s |Ajuste: %s| Byte Inicial: %d|Tamaño: %d|Siguiente EBR: %i|\", height=1];",Numero,Numero2,Numero2,Numero2+1,EBREstruc->Nombre,EBREstruc->Estado,EBREstruc->Fitt,EBREstruc->ByteInicial,EBREstruc->Tamano,EBREstruc->ProximoEBR);
                    while(EBREstruc->ProximoEBR!=(-1))
                    {
                        fseek(Disco,EBREstruc->ProximoEBR,SEEK_SET);
                        free(EBREstruc);
                        EBREstruc = malloc(sizeof(struct EBR));
                        fread(EBREstruc,sizeof(struct EBR),1,Disco);
                        fprintf(Grafica,"node%d%d:f%d -> node%d%d:f%d [dir=both];",Numero,Numero2,Numero2,Numero,Numero2+1,Numero2+1);
                        Numero2++;
                        fprintf(Grafica,"node%d%d [label = \"<f%d> EBR %d | Nombre: %s |Estado: %s |Ajuste: %s| Byte Inicial: %d|Tamaño: %d|Siguiente EBR: %i|\", height=1];",Numero,Numero2,Numero2,Numero2+1,EBREstruc->Nombre,EBREstruc->Estado,EBREstruc->Fitt,EBREstruc->ByteInicial,EBREstruc->Tamano,EBREstruc->ProximoEBR);
                    }
                     free(EBREstruc);
                }
                Numero++;
            }





        fprintf(Grafica,"\t}\n");
        fprintf(Grafica,"}");
        fclose(Disco);
        fclose(Grafica);
        free(EstrucMBR);
        char Pa[120];
        strcpy(Pa,"dot -Tjpg Grafica.dot -o ");
        strcat(Pa,Direccion);
        system(Pa);
        printf("Grafica generada correctamente\n");

    }else
        printf(">>>>ERROR: imposible generar grafica, no se ha montado el Disco<<<<\n");

}

//********************************************* METODO PARA CREAR EL REPORTE DEL DISCO ***************************************************************
//****************************************************************************************************************************************************************
void CrearReporteDisco(char *Direccion, char *id)
{

    FILE *Grafica;
    FILE *Disco;
    struct MBR *EstrucMBR;
    struct EBR *EBREstruc;

    struct CabeceraParticion *Cab;
    int Numero;
    char DiscoDireccion[125];
    int Contador;
    int IndiceByte;
    int ByteSiguienteEBR;
    bool Extendida;


    Numero=0;
    Contador=0;
    strcpy(DiscoDireccion,DevolverPath(id));
    if((strcasecmp(DiscoDireccion,"a"))!=0)
    {
        //El Disoco si ha sido montado
         Grafica =  fopen("Grafica2.dot","w+");
        if(Grafica==NULL)
        {
            printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
            return -1;
        }


         Disco = fopen(DiscoDireccion,"rb");
         if(Disco==NULL)
         {
             printf(">>>>ERROR: no se pudo abrir el Disco para su lectura <<<<\n");
             return -1;
         }

         EstrucMBR = malloc(4*sizeof(struct MBR)+1);
         fseek(Disco,0,SEEK_SET);
         fread(EstrucMBR,sizeof(struct MBR),1,Disco);
         Extendida = false;

         fprintf(Grafica,"digraph structs{\n");
            fprintf(Grafica,"\tnode [shape=record,width=.5,height=.5];\n");
            fprintf(Grafica,"\tlabel=\"DISCO\"\n");
            fprintf(Grafica,"struct1 [shape=record, label=\" MBR");
            Cab = OrdenarParticiones(EstrucMBR);
            struct OrdenarParticion *Auxiliar;
            Auxiliar = Cab->Primera;
            while(Contador < 4)
            {
                if(Auxiliar->Estado!=0)
                {
                    if(Auxiliar->Anterior==NULL)
                    {
                        //Verificando si hay espacio antes de la primera particion
                        if( (sizeof(struct MBR)+1) != Auxiliar->ByteInicial)
                            fprintf(Grafica,"| Libre: %d",(Auxiliar->ByteInicial - sizeof(struct MBR)));
                    }

                    //Nodo Con Particion
                    if(strcasecmp(Auxiliar->Tipo,"e")==0)
                    {
                        //Particion Extendida
                        fprintf(Grafica,"|<f1> Extendida.: %s",Auxiliar->Nombre);
                        IndiceByte = Auxiliar->ByteInicial + Auxiliar->Tamano;
                        Extendida=true;
                    }else if(strcasecmp(Auxiliar->Tipo,"p")==0)
                    {
                        //Particion Primaria
                        fprintf(Grafica,"| Primaria.: %s",Auxiliar->Nombre);
                        IndiceByte = Auxiliar->ByteInicial + Auxiliar->Tamano;

                    }

                    if(Auxiliar->Siguiente!=NULL)
                    {
                        if(Auxiliar->Siguiente->Estado!=0)
                        {
                            //El Nodo Siguiente tambien esta ocupado
                            if((Auxiliar->Siguiente->ByteInicial-1 -IndiceByte)!=0)
                            {
                                //Si hay Espacio en las particiones
                                fprintf(Grafica,"| Libre: %d",(Auxiliar->Siguiente->ByteInicial -IndiceByte-1));

                            }
                        }


                    }

                }
                Contador++;
                Auxiliar=Auxiliar->Siguiente;
            }
            if(IndiceByte < EstrucMBR->TamanoDisco)
                fprintf(Grafica,"| Libre: %d",EstrucMBR->TamanoDisco - IndiceByte);

            fprintf(Grafica,"\"];\n");
            //Graficando las Particiones Lógicas si las hubieran
            Contador=0;
            if(Extendida==true)
            {

                Auxiliar=Cab->Primera;
                while(Contador < 4)
                {
                    if(Auxiliar->Estado!=0)
                    {
                        if(strcasecmp(Auxiliar->Tipo,"e")==0)
                        {
                            //La Particion es Extendida
                            EBREstruc = malloc(sizeof(struct EBR));
                            fseek(Disco,Auxiliar->ByteInicial,SEEK_SET);
                            fread(EBREstruc,sizeof(struct EBR),1,Disco);
                            fprintf(Grafica,"struct2 [shape=record, label=\"<f2> Logica: %s",EBREstruc->Nombre);
                            IndiceByte = sizeof(struct EBR);
                            while(EBREstruc->ProximoEBR!=(-1))
                            {
                                ByteSiguienteEBR = EBREstruc->ProximoEBR - EBREstruc->ByteInicial - EBREstruc->Tamano - 1;
                                fseek(Disco,EBREstruc->ProximoEBR,SEEK_SET);
                                free(EBREstruc);
                                EBREstruc = malloc(sizeof(struct EBR));
                                fread(EBREstruc,sizeof(struct EBR),1,Disco);

                                if(ByteSiguienteEBR != 0)
                                {
                                    //SI Hay Espacio entre particiones
                                    fprintf(Grafica,"| Libre: %d",ByteSiguienteEBR);

                                }

                                fprintf(Grafica,"| Logica: %s",EBREstruc->Nombre);





                            }
                            ByteSiguienteEBR = Auxiliar->ByteInicial + Auxiliar->Tamano - EBREstruc->ByteInicial - EBREstruc->Tamano;
                            if((ByteSiguienteEBR)!=0)
                            {
                                //Quedo Espacio libre en la Extendida
                                fprintf(Grafica,"| Libre: %d",ByteSiguienteEBR);
                            }

                            free(EBREstruc);
                            fprintf(Grafica,"\"];\n");
                            fprintf(Grafica,"struct1:f1 -> struct2:f2;\n");
                        }
                    }
                    Contador++;
                    Auxiliar =Auxiliar->Siguiente;
                }
            }





        fprintf(Grafica,"}\n");


        fclose(Disco);
        fclose(Grafica);
        free(EstrucMBR);
        char Pa[120];
        strcpy(Pa,"dot -Tjpg Grafica2.dot -o ");
        strcat(Pa,Direccion);
        system(Pa);
        printf("Grafica generada correctamente\n");

    }else
         printf(">>>>ERROR: imposible generar grafica, no existe el Disco<<<<\n");
}

//********************************* METODO PARA DEVOLVER EL NUMERO DE INODOS QUE SE CREARAN ***********************************************************************
//*****************************************************************************************************************************************************************
int DevolverNumInodos(int TamanoPar)
{
    int Dividendo,Divisor,Resultado;
    Dividendo = TamanoPar - sizeof(struct SuperBloque);
    Divisor = 4 + sizeof(struct TablaInodos) + (3*(sizeof(struct BloqueCarpetas)));
    Resultado = Dividendo/Divisor;
    return Resultado;



}
//********************************** METODO PARA INSERTAR UN SUPERBLOQUE ********************************************************************************************************
//*******************************************************************************************************************************************************************************
void InsertarSuperBloque(struct SuperBloque *Estructura, FILE *Archivo,int ByteInicial)
{
    fseek(Archivo,ByteInicial,SEEK_SET);
    fwrite(Estructura,sizeof(struct SuperBloque),1,Archivo);

}

//*********************************** METODO PARA INSERTAR UNA TABLA DE INODO ****************************************************************************************************************
//***************************************************************************************************************************************************************************************************
void InsertarTablaInodo(struct TablaInodos *Estructura, FILE *Archivo,int ByteInicial)
{
    fseek(Archivo,ByteInicial,SEEK_SET);
    fwrite(Estructura,sizeof(struct TablaInodos),1,Archivo);
}

//********************************** METODO PARA INSERTAR UNA BLOQUE DE CARPETAS ***************************************************************************************************************************
//**************************************************************************************************************************************************************************************************************
void InsertarBloqueCarpetas(struct BloqueCarpetas *Estructura, FILE *Archivo, int ByteInicial)
{
    fseek(Archivo,ByteInicial,SEEK_SET);
    fwrite(Estructura,sizeof(struct BloqueCarpetas),1,Archivo);
}

//******************************** METODO PARA INSERTAR UN BLOQUE DE ARCHIVOS ****************************************************************************************************************************************
//***********************************************************************************************************************************************************************************************************************************
void InsertarBloqueArchivos(struct Archivos *Estructura, FILE *Disco, int ByteInicial)
{
    fseek(Disco,ByteInicial,SEEK_SET);
    fwrite(Estructura,sizeof(struct Archivos),1,Disco);
}
//************************ METODO PARA ACTUALIZA EL BITMAPT DE INODOS Y BLOQUES *****************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************************************
void InsertarFAST(struct Particion *Primaria, struct EBR *Logica,int Tipo,FILE *DiscoLeer)
{
    struct SuperBloque *SBloque;
    SBloque = malloc(sizeof(struct SuperBloque));

    //Verificando que tipo de Particion es

    if(Tipo==1)
    {
        fseek(DiscoLeer,Primaria->ByteInicial,SEEK_SET);
        fread(SBloque,sizeof(struct SuperBloque),1,DiscoLeer);

    }else if(Tipo==2)
    {
        fseek(DiscoLeer,Logica->ByteInicial,SEEK_SET);
        fread(SBloque,sizeof(struct SuperBloque),1,DiscoLeer);

    }

    //Veficando que la Particion ya haya sido formateada como FULL
    if(SBloque->TamanoInodo==0)
    {
        printf(">>>>ERROR: La particion solicitada todavia no ha sido formateado como full<<<<\n");
        free(SBloque);
        return;
    }
    int NumeroInodos;
    int NumeroBloques;
    NumeroInodos = SBloque->TotalInodos;
    NumeroBloques = SBloque->TotalBloques;
    char Inodos[NumeroInodos];
    char Bloques[NumeroBloques];
    int Contador;
    Contador=0;

    //Limpiando el Bitmap de Inodos


    while(Contador <  NumeroInodos)
    {
        Inodos[Contador]='0';
        Contador++;
    }
    fseek(DiscoLeer,SBloque->InicioBitmapInodos,SEEK_SET);
    fwrite(Inodos,NumeroInodos,1,DiscoLeer);

    //Limpiando el Bitmap de bloques
    Contador=0;
    while(Contador < NumeroBloques)
    {
        Bloques[Contador] ='0';
        Contador++;
    }
    fseek(DiscoLeer,SBloque->InicioBitmapBloques,SEEK_SET);
    fwrite(Bloques,NumeroBloques,1,DiscoLeer);

    free(SBloque);
    printf("Formateo tipo Fast realizado correctamente\n");

}

//************************ METODO PARA INSERTAR SUPER_BLOQUE INODOS Y BITMPAS EN UNA PARTICION ***********************************************************************************
//*****************************************************************************************************************************************************************************************
void InsertarFormateo(struct Particion *Primaria, struct EBR *Logica,int Tipo,FILE *DiscoLeer)
{
    int InBitmapInodos,InBitmapBlques,InTabInodos,InBloques,Indice,Acumulador,NumeroInodos;
     int Indice2;
    time_t FechaMontado,FechaDesmon;
    struct SuperBloque SBloque;

    if(Tipo ==1)
    {
        //Se realizara el formateo en una particion PRIMARIA
        NumeroInodos = DevolverNumInodos(Primaria->Tamano);
        InBitmapInodos = Primaria->ByteInicial + sizeof(struct SuperBloque);    // Apartir de este byte se empieza a escribir
        InBitmapBlques = InBitmapInodos + NumeroInodos;    // Apartir de este byte se empieza a escribir
        InTabInodos = InBitmapBlques + (3 * NumeroInodos);
        InBloques = InTabInodos + (NumeroInodos * sizeof(struct TablaInodos));
        FechaMontado = time(0);
        FechaDesmon  = time(0);
        Indice =0;
        Acumulador = 0;

        fseek(DiscoLeer,Primaria->ByteInicial,SEEK_SET);
        SBloque.InicioBitmapBloques = InBitmapBlques;
        SBloque.InicioBitmapInodos = InBitmapInodos;
        SBloque.BloquesLibres = (3*NumeroInodos)-1; //Por el Bloque de la Raiz que se crea abajo
        SBloque.InicioTablaBloques = InBloques;
        SBloque.InicioTablaInodos =InTabInodos;
        SBloque.InodosLibres = NumeroInodos-1;      //Por el Inodo de la Raiz que se crea abajo
        SBloque.PrimerBloqueLibre = 2;  //El Primero sera ocupado por la raiz
        SBloque.PrimerInodoLibre =2;    //El Primero sera ocupado por la raiz
        SBloque.SistemaArchivos = 53;
        SBloque.TamanoBloque = sizeof(struct BloqueCarpetas);
        SBloque.TamanoInodo = sizeof(struct TablaInodos);
        SBloque.TotalBloques = 3*NumeroInodos;
        SBloque.TotalInodos = NumeroInodos;
        SBloque.VecesMontado = 1;
        SBloque.UltimaFechaDesmont = FechaDesmon;
        SBloque.UltimaFechaMontura = FechaMontado;

        //Grabando el SuperBloque en el Disco
        InsertarSuperBloque(&SBloque,DiscoLeer,Primaria->ByteInicial);

        //Introduciendo el Bitmap de Inodos
        char BitInodos[NumeroInodos];
        char BitBloques[3*NumeroInodos];
        Indice=0;
        while(Indice < NumeroInodos)
        {
            BitInodos[Indice] = '0';
            Indice++;
        }
        BitInodos[0] = '1';  //el que Ocupa la Raiz

        fseek(DiscoLeer,InBitmapInodos,SEEK_SET);
        fwrite(BitInodos,NumeroInodos*sizeof(char),1,DiscoLeer);

        //Introduciend el Bitmap de Bloques

        Indice =0;
        while(Indice < (3*NumeroInodos))
        {
            BitBloques[Indice] = '0';
            Indice++;
        }
        BitBloques[0] = '1';  //El que Ocupa la raiz
        fseek(DiscoLeer,InBitmapBlques,SEEK_SET);
        fwrite(BitBloques,3*NumeroInodos*sizeof(char),1,DiscoLeer);


        //Introduciendo las Tablas de Inodos
        Indice=0;
        Acumulador = InTabInodos;
        while(Indice < NumeroInodos)
        {
            struct TablaInodos Inodo;


            Indice2 = 0;

            while(Indice2 < 15)
            {
               Inodo.Apuntadores[Indice2] = -1;
               Indice2++;
            }

            Inodo.FechaCreacion = FechaMontado;
            Inodo.GrupoPropietario = 1;
            Inodo.Permisos = 0;
            Inodo.TamanoArchivo = 0;
            Inodo.Tipo[0] = '0';
            Inodo.UltimaFechaLectura = FechaMontado;
            Inodo.UltimaFechaModific = FechaMontado;
            Inodo.UsuarioPropietario = 1;

            InsertarTablaInodo(&Inodo,DiscoLeer,Acumulador);
            Acumulador = Acumulador + sizeof(struct TablaInodos);
            Indice++;

        }
        //Escribiendo los bloques de carpetas
        Indice =0;
        Acumulador = InBloques;
        while(Indice < (NumeroInodos*3) )
        {
            Indice2 =0;
            struct BloqueCarpetas BloqueCarp;
            while(Indice2 < 4)
            {
                BloqueCarp.ApuntadorCarpeta[Indice2].Apuntador = -1;
                Indice2++;
            }

            InsertarBloqueCarpetas(&BloqueCarp,DiscoLeer,Acumulador);
            Acumulador = Acumulador + sizeof(struct BloqueCarpetas);
            Indice++;
        }

        //Creando a carpeta Raiz
        struct TablaInodos Raiz;
        Raiz.FechaCreacion = FechaMontado;
        Raiz.GrupoPropietario = 1;
        Raiz.Permisos = 4;  //Permiso de lectura
        Raiz.TamanoArchivo = 0;
        Raiz.Tipo[0] = '0';
        Raiz.UltimaFechaLectura = FechaMontado;
        Raiz.UltimaFechaModific = FechaMontado;
        Raiz.UsuarioPropietario = 1;
        Indice = 0;
        while (Indice < 15)
        {
            Raiz.Apuntadores[Indice] = -1;
            Indice++;
        }

        Raiz.Apuntadores[0] = 1; //Para el Bloque Raiz
        //Guardando el Bloque Raiz
        struct BloqueCarpetas CarpetaRaiz;
        Indice = 0;
        while(Indice < 4)
        {
            CarpetaRaiz.ApuntadorCarpeta[Indice].Apuntador = -1;
            Indice++;
        }
        CarpetaRaiz.ApuntadorCarpeta[0].Apuntador = 1;   //Apunta al Inodo 1 que es la raiz
        strcpy(CarpetaRaiz.ApuntadorCarpeta[0].Nombre,"Raiz");
        CarpetaRaiz.ApuntadorCarpeta[1].Apuntador = 1;
        strcpy(CarpetaRaiz.ApuntadorCarpeta[1].Nombre,"Raiz");   //Su padre que es el mismo inodo raiz
        InsertarBloqueCarpetas(&CarpetaRaiz,DiscoLeer,InBloques);
        InsertarTablaInodo(&Raiz,DiscoLeer,InTabInodos);


        printf("Formateo Full realizado correctamente en particion Primaria: %s\n",Primaria->Nombre);

    }else if(Tipo == 2)
    {
        //Se realizara el formateo en una particion LOGICA
        NumeroInodos = DevolverNumInodos(Logica->Tamano);
        InBitmapInodos = Logica->ByteInicial + sizeof(struct SuperBloque);    // Apartir de este byte se empieza a escribir
        InBitmapBlques = InBitmapInodos + NumeroInodos;    // Apartir de este byte se empieza a escribir
        InTabInodos = InBitmapBlques + (3 * NumeroInodos);
        InBloques = InTabInodos + (NumeroInodos * sizeof(struct TablaInodos));
        FechaMontado = time(0);
        FechaDesmon  = time(0);
        Indice =0;
        Acumulador = 0;

        fseek(DiscoLeer,Logica->ByteInicial,SEEK_SET);

        SBloque.InicioBitmapBloques = InBitmapBlques;
        SBloque.InicioBitmapInodos = InBitmapInodos;
        SBloque.BloquesLibres = (3*NumeroInodos)-1;    //Por el Bloque de la Raiz que se crea abajo
        SBloque.InicioTablaBloques = InBloques;
        SBloque.InicioTablaInodos =InTabInodos;
        SBloque.InodosLibres = NumeroInodos-1;         //Por el Bloque de la Raiz que se crea abajo
        SBloque.PrimerBloqueLibre = InBloques;
        SBloque.PrimerInodoLibre =InTabInodos;
        SBloque.SistemaArchivos = 53;
        SBloque.TamanoBloque = sizeof(struct BloqueCarpetas);
        SBloque.TamanoInodo = sizeof(struct TablaInodos);
        SBloque.TotalBloques = 3*NumeroInodos;
        SBloque.TotalInodos = NumeroInodos;
        SBloque.VecesMontado = 1;
        SBloque.UltimaFechaDesmont = FechaDesmon;
        SBloque.UltimaFechaMontura = FechaMontado;

        //Grabando el SuperBloque en el Disco
        InsertarSuperBloque(&SBloque,DiscoLeer,Logica->ByteInicial);

        //Introduciendo el Bitmap de Inodos
        char BitInodos[NumeroInodos];
        char BitBloques[3*NumeroInodos];
        Indice=0;
        while(Indice < NumeroInodos)
        {
            BitInodos[Indice] = '0';
            Indice++;
        }
        BitInodos[0] = '1';   //Para la Raiz
        fseek(DiscoLeer,InBitmapInodos,SEEK_SET);
        fwrite(BitInodos,NumeroInodos*sizeof(char),1,DiscoLeer);

        //Introduciend el Bitmap de Bloques

        Indice =0;
        while(Indice < (3*NumeroInodos))
        {
            BitBloques[Indice] = '0';
            Indice++;
        }
        BitBloques[0] = '1'; //Para la raiz
        fseek(DiscoLeer,InBitmapBlques,SEEK_SET);
        fwrite(BitBloques,3*NumeroInodos*sizeof(char),1,DiscoLeer);


        //Introduciendo las Tablas de Inodos
        Indice=0;
        Acumulador = InTabInodos;
        while(Indice < NumeroInodos)
        {
            struct TablaInodos Inodo;


            Indice2 = 0;

            while(Indice2 < 15)
            {
               Inodo.Apuntadores[Indice2] = -1;
               Indice2++;
            }

            Inodo.FechaCreacion = FechaMontado;
            Inodo.GrupoPropietario = 1;
            Inodo.Permisos = 0;
            Inodo.TamanoArchivo = 0;
            Inodo.Tipo[0] = '0';
            Inodo.UltimaFechaLectura = FechaMontado;
            Inodo.UltimaFechaModific = FechaMontado;
            Inodo.UsuarioPropietario = 1;

            InsertarTablaInodo(&Inodo,DiscoLeer,Acumulador);
            Acumulador = Acumulador + sizeof(struct TablaInodos);
            Indice++;

        }

        //Escribiendo los bloques de carpetas
        Indice =0;
        Acumulador = InBloques;
        while(Indice < (NumeroInodos*3) )
        {
            Indice2 =0;
            struct BloqueCarpetas BloqueCarp;
            while(Indice2 < 4)
            {
                BloqueCarp.ApuntadorCarpeta[Indice2].Apuntador = -1;
                Indice2++;
            }

            InsertarBloqueCarpetas(&BloqueCarp,DiscoLeer,Acumulador);
            Acumulador = Acumulador + sizeof(struct BloqueCarpetas);
            Indice++;
        }

        //Creando a carpeta Raiz
        struct TablaInodos Raiz;
        Raiz.FechaCreacion = FechaMontado;
        Raiz.GrupoPropietario = 1;
        Raiz.Permisos = 4;  //Permiso de lectura
        Raiz.TamanoArchivo = 0;
        Raiz.Tipo[0] = '0';
        Raiz.UltimaFechaLectura = FechaMontado;
        Raiz.UltimaFechaModific = FechaMontado;
        Raiz.UsuarioPropietario = 1;
        Indice = 0;
        while (Indice < 15)
        {
            Raiz.Apuntadores[Indice] = -1;
            Indice++;
        }
        Raiz.Apuntadores[0] = 1; //Para el Bloque Raiz
        //Guardando el Bloque Raiz
        struct BloqueCarpetas CarpetaRaiz;
        Indice = 0;
        while(Indice < 4)
        {
            CarpetaRaiz.ApuntadorCarpeta[Indice].Apuntador = -1;
            Indice++;
        }
        CarpetaRaiz.ApuntadorCarpeta[0].Apuntador = 1;   //Apunta al Inodo 1 que es la raiz
        strcpy(CarpetaRaiz.ApuntadorCarpeta[0].Nombre,"Raiz");
        CarpetaRaiz.ApuntadorCarpeta[1].Apuntador = 1;
        strcpy(CarpetaRaiz.ApuntadorCarpeta[1].Nombre,"Raiz");   //Su padre que es el mismo inodo raiz
        InsertarBloqueCarpetas(&CarpetaRaiz,DiscoLeer,InBloques);
        InsertarTablaInodo(&Raiz,DiscoLeer,InTabInodos);
        printf("Formateo Full realizado correctamente en particion Logica: %s\n",Logica->Nombre);

    }


}

//******************** METODO PARA GRAFICAR EL SUPERBLOQUE EN UNA PARTICION LOGICA O PRIMARIA**********************************************************************************************
//****************************************************************************************************************************************************************************************************

void GraficarSuperBloque(struct Particion *Primaria, struct EBR *Logica,int TipoGrafica,char *DireccionDest,FILE *Disco)
{
    //Verficando que la particion ya tenga un SuperBloque Guardado
    struct SuperBloque *SBEstructura;
    SBEstructura = malloc(sizeof(struct SuperBloque));

    //Verificando si la particion el Logica o primaria
    if(TipoGrafica==1)
    {
        fseek(Disco,Primaria->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);

    }else if(TipoGrafica==2)
    {
        fseek(Disco,Logica->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);
    }

    //Verificando que ya se haya formateado antes la particion
    if(SBEstructura->TamanoInodo==0)
    {
        printf(">>>>ERROR: la particion no se ha formateado, no tiene un SuperBloque<<<<\n");
        free(SBEstructura);
        return;
    }


    //Creando el archivo .dot
    FILE *Grafica;
    Grafica =  fopen("GraficaSB.dot","w+");


    if(Grafica==NULL)
    {
        printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
        return;
    }



        //Grafica para una Primaria
        fprintf(Grafica,"digraph G {\n");
        fprintf(Grafica,"nodesep=.1;\n");
        fprintf(Grafica,"\trankdir=LR;\n");
        fprintf(Grafica,"\tnode [shape=record,width=.1,height=.1];\n");
        fprintf(Grafica,"\t subgraph cluster0{\n");
        if(TipoGrafica==1)
            fprintf(Grafica,"\tlabel=\"SUPER BLOQUE PARTICION: %s\"\n",Primaria->Nombre);
        else if(TipoGrafica==2)
            fprintf(Grafica,"\tlabel=\"SUPER BLOQUE PARTICION: %s\"\n",Logica->Nombre);


            //Obteniendo las fechas

            struct tm *Tiempo1 = localtime(&SBEstructura->UltimaFechaMontura);

            char fechaMontaje[20];
            char FecharDesmontaje[20];
            strftime(fechaMontaje,128,"%d/%m/%y %H:%M:%S",Tiempo1);
            Tiempo1 = localtime(&SBEstructura->UltimaFechaDesmont);
            strftime(FecharDesmontaje,128,"%d/%m/%y %H:%M:%S",Tiempo1);


            fprintf(Grafica,"node1 [label = \"<f3> Super Bloque | Numero Inodos:  %d | Numero Bloques.: %d | Bloques Libres: %d | Inodos Libres: %d | Ult. Fecha Montado: %s | Ult. Fecha Desmont.: %s | Veces Montado: %d | Sistema de Archivos: 0xEF%d | Tamaño Inodo: %d | Tamaño Bloque: %d | Primer Inodo Libre: %d | Primer Bloque Libre: %d | Inicio Bitma Inodos: %d | Inicio Bitmap Bloques: %d | Inicio Inodos: %d | Inicio Bloques: %d \", height=1];\n",SBEstructura->TotalInodos,SBEstructura->TotalBloques,SBEstructura->BloquesLibres,SBEstructura->InodosLibres,fechaMontaje,FecharDesmontaje,SBEstructura->VecesMontado,SBEstructura->SistemaArchivos,SBEstructura->TamanoInodo,SBEstructura->TamanoBloque,SBEstructura->PrimerInodoLibre,SBEstructura->PrimerBloqueLibre,SBEstructura->InicioBitmapInodos,SBEstructura->InicioBitmapBloques,SBEstructura->InicioTablaInodos,SBEstructura->InicioTablaBloques);

        fprintf(Grafica,"\t}\n");
        fprintf(Grafica,"}");




    //Cerrando el archivo .dot y generando la imagen

    free(SBEstructura);
    fclose(Grafica);
    char Pa[120];
    strcpy(Pa,"dot -Tjpg GraficaSB.dot -o ");
    strcat(Pa,DireccionDest);
    system(Pa);
    printf("Grafica generada correctamente\n");
}

//********************************** METODO PARA GRAFICAR EL BITMAP DE INODOS DE UNA LOGICA O PRIMARIA ********************************************************************************
//***********************************************************************************************************************************************************************************************
void GraficarBitmapInodos(struct Particion *Primaria, struct EBR *Logica,int TipoGrafica,char *DireccionDest,FILE *Disco)
{
    //Verficando que la particion ya tenga un SuperBloque Guardado
    struct SuperBloque *SBEstructura;
    SBEstructura = malloc(sizeof(struct SuperBloque));

    if(TipoGrafica==1)
    {
        fseek(Disco,Primaria->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);
    }else if(TipoGrafica==2)
    {
        fseek(Disco,Logica->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);
    }


    if(SBEstructura==NULL)
    {
        printf(">>>>ERROR: la particion no se ha formateado, no tiene un SuperBloque<<<<\n");
        return;
    }


    //Creando el archivo .dot
    FILE *Grafica;
    Grafica =  fopen("GraficaBMI.dot","w+");


    if(Grafica==NULL)
    {
        printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
        return;
    }


        //Graficando
        fprintf(Grafica,"digraph G {\n");
        fprintf(Grafica,"nodesep=.1;\n");
        fprintf(Grafica,"\trankdir=LR;\n");
        fprintf(Grafica,"\tnode [shape=record,width=.1,height=.1];\n");
        fprintf(Grafica,"\t subgraph cluster0{\n");

        if(TipoGrafica==1)
            fprintf(Grafica,"\tlabel=\"BITMAP DE INODOS, PARTICION: %s\"\n",Primaria->Nombre);
        else if(TipoGrafica==2)
            fprintf(Grafica,"\tlabel=\"BITMAP DE INODOS, PARTICION: %s\"\n",Logica->Nombre);
            //Graficando Las Particiones NO vacias
            //Obteniendo el bitmap de Inodos
            int NumeroInodos = SBEstructura->TotalInodos;
            int Contador=0;
            char Bitmpa[NumeroInodos];
            fseek(Disco,SBEstructura->InicioBitmapInodos,SEEK_SET);
            fread(Bitmpa,NumeroInodos,1,Disco);

            fprintf(Grafica,"node1 [label = \"<f3> POSICION  ESTADO");
            while(Contador < NumeroInodos)
            {
                fprintf(Grafica," | %d  :  %c ",Contador+1,Bitmpa[Contador]);
                Contador++;
            }

            fprintf(Grafica,"\", height=1];\n");

        fprintf(Grafica,"\t}\n");
        fprintf(Grafica,"}");




    //Cerrando el archivo .dot y generando la imagen

    free(SBEstructura);
    fclose(Grafica);
    char Pa[120];
    strcpy(Pa,"dot -Tjpg GraficaBMI.dot -o ");
    strcat(Pa,DireccionDest);
    system(Pa);
    printf("Grafica generada correctamente\n");

}

//****************** METODO PARA GRAFICAR EL BITMAP DE BLOQUES DE UNA PARTICION LOGICA  O PRIMARIA******************************************************************************************************************
//**************************************************************************************************************************************************************************************************************************
void GraficarBitmapBloques(struct Particion *Primaria, struct EBR *Logica,int TipoGrafica,char *DireccionDest,FILE *Disco)
{
     struct SuperBloque *SBEstructura;
    SBEstructura = malloc(sizeof(struct SuperBloque));

    if(TipoGrafica==1)
    {
        fseek(Disco,Primaria->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);
    }else if(TipoGrafica==2)
    {
        fseek(Disco,Logica->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);
    }


    if(SBEstructura==NULL)
    {
        printf(">>>>ERROR: la particion: %s no se ha formateado, no tiene un SuperBloque<<<<\n",Primaria->Nombre);
        return;
    }


    //Creando el archivo .dot
    FILE *Grafica;
    Grafica =  fopen("GraficaBMB.dot","w+");


    if(Grafica==NULL)
    {
        printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
        return;
    }


        //Grafica para una Primaria
        fprintf(Grafica,"digraph G {\n");
        fprintf(Grafica,"nodesep=.1;\n");
        fprintf(Grafica,"\trankdir=LR;\n");
        fprintf(Grafica,"\tnode [shape=record,width=.1,height=.1];\n");
        fprintf(Grafica,"\t subgraph cluster0{\n");

        if(TipoGrafica==1)
            fprintf(Grafica,"\tlabel=\"BITMAP DE BLOQUES, PARTICION: %s\"\n",Primaria->Nombre);
        else if(TipoGrafica==2)
            fprintf(Grafica,"\tlabel=\"BITMAP DE BLOQUES, PARTICION: %s\"\n",Logica->Nombre);

            //Graficando Las Particiones NO vacias
            //Obteniendo el bitmap de Inodos
            int NumeroBloques = SBEstructura->TotalBloques;
            int Contador=0;
            char Bitmpa[NumeroBloques];
            fseek(Disco,SBEstructura->InicioBitmapBloques,SEEK_SET);
            fread(Bitmpa,NumeroBloques,1,Disco);

            fprintf(Grafica,"node1 [label = \"<f3> POSICION  ESTADO");
            while(Contador < NumeroBloques)
            {
                fprintf(Grafica," | %d  :  %c ",Contador+1,Bitmpa[Contador]);
                Contador++;
            }

            fprintf(Grafica,"\", height=1];\n");

        fprintf(Grafica,"\t}\n");
        fprintf(Grafica,"}");



    //Cerrando el archivo .dot y generando la imagen

    free(SBEstructura);
    fclose(Grafica);
    char Pa[120];
    strcpy(Pa,"dot -Tjpg GraficaBMB.dot -o ");
    strcat(Pa,DireccionDest);
    system(Pa);
    printf("Grafica generada correctamente\n");
}
//********************** METODO PARA GRAFICAR LOS BLOQUES OCUPADOS DEL SISTEMA DE ARCHIVOS ******************************************************************************************************************************
//*******************************************************************************************************************************************************************************************************************************
void GraficarBloquesOcupados(struct Particion *Primaria, struct EBR *Logica,int TipoGrafica,char *DireccionDest,FILE *Disco)
{
    struct SuperBloque *SBEstructura;
    SBEstructura = malloc(sizeof(struct SuperBloque));

    if(TipoGrafica==1)
    {
        fseek(Disco,Primaria->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);
    }else if(TipoGrafica==2)
    {
        fseek(Disco,Logica->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);
    }


    if(SBEstructura->TamanoInodo==0)
    {
        printf(">>>>ERROR: la particion: %s no se ha formateado, no tiene un SuperBloque<<<<\n",Primaria->Nombre);
        return;
    }


    //Creando el archivo .dot
    FILE *Grafica;
    Grafica =  fopen("GraficaBloque.dot","w+");


    if(Grafica==NULL)
    {
        printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
        return;
    }


        //Grafica para una Primaria
        fprintf(Grafica,"digraph G {\n");
        fprintf(Grafica,"nodesep=.4;\n");
        fprintf(Grafica,"\t rankdir=LR;\n");
        fprintf(Grafica,"\tnode [shape=record,width=.3,height=.3];\n");
        fprintf(Grafica,"\t  subgraph cluster0{\n");

        if(TipoGrafica==1)
            fprintf(Grafica,"\tlabel=\"BLOQUES OCUPADOS, PARTICION: %s\"\n",Primaria->Nombre);
        else if(TipoGrafica==2)
            fprintf(Grafica,"\tlabel=\"BLOQUES OCUPADOS, PARTICION: %s\"\n",Logica->Nombre);


            //Obteniendo el bitmap de Inodos para buscar los Bloques asociados y graficarlos
            int NumeroInodos = SBEstructura->TotalInodos;
            int Contador=0;
            int Acumulador,ByteInicialBloque,TamanoInodo,TamanoBloque,IndiceApunt,IndiceCarp;
            char Bitmpa[NumeroInodos];
            struct TablaInodos *Inodo;

            fseek(Disco,SBEstructura->InicioBitmapInodos,SEEK_SET);
            fread(Bitmpa,NumeroInodos,1,Disco);


            TamanoInodo = SBEstructura->TamanoInodo;
            TamanoBloque = SBEstructura->TamanoBloque;
            while(Contador < NumeroInodos)
            {
                if(Bitmpa[Contador]=='1')
                {
                    //Se Extrae el Inodo para Graficar sus Bloques
                    Acumulador = SBEstructura->InicioTablaInodos + (Contador*TamanoInodo);
                    Inodo = malloc(sizeof(struct TablaInodos));
                    fseek(Disco,Acumulador,SEEK_SET);
                    fread(Inodo,TamanoInodo,1,Disco);

                    //Se extraen los bloques del inodo
                    IndiceApunt =0;
                    while(IndiceApunt < 15)
                    {
                        if(IndiceApunt==12)
                        {
                            //Primer Apuntador Indirec
                        }else if(IndiceApunt==13)
                        {
                            //Apuntador Indirecto Doble
                        }else if(IndiceApunt==14)
                        {
                            //Apuntador Indirecto Triple
                        }else
                        {
                            //Apuntadores Directos
                            if(Inodo->Apuntadores[IndiceApunt]!=-1)
                            {
                                //EXISTE un bloque

                                if(Inodo->Tipo[0]=='0')
                                {
                                    //El Inodo es una Carpeta por lo tanto lo que tendra son bloques de Carpetas
                                    struct BloqueCarpetas *Carp;
                                    Carp = malloc(sizeof(struct BloqueCarpetas));
                                    ByteInicialBloque = SBEstructura->InicioTablaBloques + ((Inodo->Apuntadores[IndiceApunt]-1)*TamanoBloque);
                                    fseek(Disco,ByteInicialBloque,SEEK_SET);
                                    fread(Carp,TamanoBloque,1,Disco);
                                    //Escribiendo en la Grafica;
                                    fprintf(Grafica,"node%d [label=\" <f4>Bloque Carpeta: %d ",Inodo->Apuntadores[IndiceApunt],Inodo->Apuntadores[IndiceApunt]);
                                        //Evaluando los Apuntadores del bloque Carpeta
                                        IndiceCarp = 0;
                                        while(IndiceCarp < 4)
                                        {
                                            if(Carp->ApuntadorCarpeta[IndiceCarp].Apuntador!=-1)
                                            {   //Esta Apuntando a un Inodo
                                                fprintf(Grafica,"| Apuntador: %d  Nombre: %s",Carp->ApuntadorCarpeta[IndiceCarp].Apuntador,Carp->ApuntadorCarpeta[IndiceCarp].Nombre);
                                            }else
                                            {
                                                //No Apunta a nada
                                                fprintf(Grafica,"| Apuntador: -1 Nombre: none");
                                            }

                                            IndiceCarp++;
                                        }
                                    fprintf(Grafica,"\",height=1 ];\n");
                                    free(Carp);

                                }else if(Inodo->Tipo[0]=='1')
                                {
                                    //El Inodo es un Archivo por lo tanto lo que tendra son Bloques de Archivos
                                    struct Archivos *Arch;
                                    Arch = malloc(TamanoBloque);
                                    ByteInicialBloque = SBEstructura->InicioTablaBloques + ((Inodo->Apuntadores[IndiceApunt]-1)*TamanoBloque);
                                    fseek(Disco,ByteInicialBloque,SEEK_SET);
                                    fread(Arch,TamanoBloque,1,Disco);

                                    //Escribiendo en la Grafica
                                    fprintf(Grafica,"node%d [ label=\" <f4> Bloque Archivos: %d ",Inodo->Apuntadores[IndiceApunt],Inodo->Apuntadores[IndiceApunt]);
                                    fprintf(Grafica,"| Contenido: %s",Arch->Contenido);
                                    fprintf(Grafica,"\",height=1 ];\n");
                                    free(Arch);
                                }
                            }
                        }

                        IndiceApunt++;

                    }

                    free(Inodo);

                }

                Contador++;
            }



        fprintf(Grafica,"\t}\n");
        fprintf(Grafica,"}\n");

    //Cerrando el archivo .dot y generando la imagen

    free(SBEstructura);
    fclose(Grafica);
    char Pa[120];
    strcpy(Pa,"dot -Tjpg GraficaBloque.dot -o ");
    strcat(Pa,DireccionDest);
    system(Pa);
    printf("Grafica generada correctamente\n");
}

//********************* METODO PARA GRAFICAR LOS INODOS OCUPADOS DEL SISTEMA DE ARCHIVOS ******************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************************************************************
void GraficarInodosOcupados(struct Particion *Primaria, struct EBR *Logica,int TipoGrafica,char *DireccionDest,FILE *Disco)
{
     struct SuperBloque *SBEstructura;
    SBEstructura = malloc(sizeof(struct SuperBloque));

    if(TipoGrafica==1)
    {
        fseek(Disco,Primaria->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);
    }else if(TipoGrafica==2)
    {
        fseek(Disco,Logica->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);
    }


    if(SBEstructura->TamanoInodo==0)
    {
        printf(">>>>ERROR: la particion: %s no se ha formateado, no tiene un SuperBloque<<<<\n",Primaria->Nombre);
        return;
    }


    //Creando el archivo .dot
    FILE *Grafica;
    Grafica =  fopen("GraficaInodos.dot","w+");


    if(Grafica==NULL)
    {
        printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
        return;
    }


        //Grafica para una Primaria
        fprintf(Grafica,"digraph G {\n");
        fprintf(Grafica,"nodesep=.4;\n");
        fprintf(Grafica,"\t rankdir=LR;\n");
        fprintf(Grafica,"\tnode [shape=record,width=.3,height=.3];\n");
        fprintf(Grafica,"\t  subgraph cluster0{\n");

        if(TipoGrafica==1)
            fprintf(Grafica,"\tlabel=\"INODOS OCUPADOS, PARTICION: %s\"\n",Primaria->Nombre);
        else if(TipoGrafica==2)
            fprintf(Grafica,"\tlabel=\"INODOS OCUPADOS, PARTICION: %s\"\n",Logica->Nombre);


            //Obteniendo el bitmap de Inodos para buscar los inodos y graficarlos
            int NumeroInodos = SBEstructura->TotalInodos;
            int Contador=0;
            int Acumulador,TamanoInodo,Ind;
            char Bitmpa[NumeroInodos];
            struct TablaInodos *Inodo;

            fseek(Disco,SBEstructura->InicioBitmapInodos,SEEK_SET);
            fread(Bitmpa,NumeroInodos,1,Disco);


            TamanoInodo = SBEstructura->TamanoInodo;

            while(Contador < NumeroInodos)
            {
                if(Bitmpa[Contador]=='1')
                {
                    //Se Extrae el Inodo para Graficar sus Bloques
                    Acumulador = SBEstructura->InicioTablaInodos + (Contador*TamanoInodo);
                    Inodo = malloc(sizeof(struct TablaInodos));
                    fseek(Disco,Acumulador,SEEK_SET);
                    fread(Inodo,TamanoInodo,1,Disco);
                    //Obteniendo las fechas

                    struct tm *Tiempo1 = localtime(&Inodo->UltimaFechaLectura);
                    char fechaLectura[20];
                    char FechaCreacion[20];
                    char FechaModif[20];
                    strftime(fechaLectura,128,"%d/%m/%y %H:%M:%S",Tiempo1);
                    Tiempo1 = localtime(&Inodo->FechaCreacion);
                    strftime(FechaCreacion,128,"%d/%m/%y %H:%M:%S",Tiempo1);
                    Tiempo1 = localtime(&Inodo->UltimaFechaModific);
                    strftime(FechaModif,128,"%d/%m/%y %H:%M:%S",Tiempo1);

                    //Escribiendo en la grafica
                    fprintf(Grafica,"node%d [label=\" <f4>Inodo: %d ",Contador+1,Contador+1);

                    fprintf(Grafica,"| UID %d | GID: %d | Tamano: %d | Fecha Lectura: %s | Fecha Creacion: %s | Fecha Modificacion: %s | Tipo: %c | Permisos: %d" ,Inodo->UsuarioPropietario,Inodo->GrupoPropietario,Inodo->TamanoArchivo,fechaLectura,FechaCreacion,FechaModif,Inodo->Tipo[0],Inodo->Permisos);

                    //Escribiendo los apuntadores
                    Ind = 0;
                    while(Ind < 15)
                    {
                        fprintf(Grafica,"| Apuntador %d: %d",Ind,Inodo->Apuntadores[Ind]);
                        Ind++;
                    }

                    fprintf(Grafica,"\",height=1 ];\n");
                    //Se extraen los bloques del inodo

                    free(Inodo);

                }

                Contador++;
            }



        fprintf(Grafica,"\t}\n");
        fprintf(Grafica,"}\n");

    //Cerrando el archivo .dot y generando la imagen

    free(SBEstructura);
    fclose(Grafica);
    char Pa[120];
    strcpy(Pa,"dot -Tjpg GraficaInodos.dot -o ");
    strcat(Pa,DireccionDest);
    system(Pa);
    printf("Grafica generada correctamente\n");

}

//******************************* METODO PARA GRAFICAR EL ARBOL DEL SISTEMA****************************************************************************************************************************************************************
//***************************************************************************************************************************************************************************************************************************************************
void GraficarArbol(struct Particion *Primaria, struct EBR *Logica, int TipoGrafica, char *DireccionDest,FILE *Disco)
{
    struct SuperBloque *SBEstructura;
    SBEstructura = malloc(sizeof(struct SuperBloque));

    if(TipoGrafica==1)
    {
        fseek(Disco,Primaria->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);
    }else if(TipoGrafica==2)
    {
        fseek(Disco,Logica->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);
    }


    if(SBEstructura->TamanoInodo==0)
    {
        printf(">>>>ERROR: la particion: %s no se ha formateado, no tiene un SuperBloque<<<<\n",Primaria->Nombre);
        return;
    }


    //Creando el archivo .dot
    FILE *Grafica;
    Grafica =  fopen("GraficaArbol.dot","w+");


    if(Grafica==NULL)
    {
        printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
        return;
    }


        //Grafica para una Primaria
        fprintf(Grafica,"digraph G {\n");
        fprintf(Grafica,"nodesep=.4;\n");
        fprintf(Grafica,"\t rankdir=LR;\n");
        fprintf(Grafica,"\tnode [shape=record,width=.3,height=.3];\n");
        fprintf(Grafica,"\t  subgraph cluster0{\n");

        if(TipoGrafica==1)
            fprintf(Grafica,"\tlabel=\"ARBOL DEL SISTEMA DE ARCHIVOS, PARTICION: %s\"\n",Primaria->Nombre);
        else if(TipoGrafica==2)
            fprintf(Grafica,"\tlabel=\"ARBOL DEL SISTEMA DE ARCHIVOS, PARTICION: %s\"\n",Logica->Nombre);


            //Obteniendo el bitmap de Inodos para buscar los Bloques asociados y graficarlos
            int NumeroInodos = SBEstructura->TotalInodos;
            int Contador=0;
            int Acumulador,ByteInicialBloque,TamanoInodo,TamanoBloque,IndiceApunt,IndiceCarp,Indice;
            char Bitmpa[NumeroInodos];
            struct TablaInodos *Inodo;

            fseek(Disco,SBEstructura->InicioBitmapInodos,SEEK_SET);
            fread(Bitmpa,NumeroInodos,1,Disco);


            TamanoInodo = SBEstructura->TamanoInodo;
            TamanoBloque = SBEstructura->TamanoBloque;
            while(Contador < NumeroInodos)
            {
                if(Bitmpa[Contador]=='1')
                {
                    //Se Extrae el Inodo
                    Acumulador = SBEstructura->InicioTablaInodos + (Contador*TamanoInodo);
                    Inodo = malloc(sizeof(struct TablaInodos));
                    fseek(Disco,Acumulador,SEEK_SET);
                    fread(Inodo,TamanoInodo,1,Disco);

                    //Obteniendo las fechas del Inodo
                    struct tm *Tiempo1 = localtime(&Inodo->UltimaFechaLectura);
                    char fechaLectura[20];
                    char FechaCreacion[20];
                    char FechaModif[20];
                    strftime(fechaLectura,128,"%d/%m/%y %H:%M:%S",Tiempo1);
                    Tiempo1 = localtime(&Inodo->FechaCreacion);
                    strftime(FechaCreacion,128,"%d/%m/%y %H:%M:%S",Tiempo1);
                    Tiempo1 = localtime(&Inodo->UltimaFechaModific);
                    strftime(FechaModif,128,"%d/%m/%y %H:%M:%S",Tiempo1);


                    //Se Grafica el Inodo
                    fprintf(Grafica,"node%d1 [label=\" <z%d1>Inodo : %d ",Contador+1,Contador+1,Contador+1);
                    fprintf(Grafica,"| Usuario: %d | Grupo: %d | Tamaño: %d |Fecha Lectura: %s | Fecha Creacion: %s |Fecha Modificacion %s |Tipo: %c | Permisos: %d ",Inodo->UsuarioPropietario,Inodo->GrupoPropietario,Inodo->TamanoArchivo,fechaLectura,FechaCreacion,FechaModif,Inodo->Tipo[0],Inodo->Permisos);
                    Indice = 0;
                    while(Indice < 15)
                    {
                        fprintf(Grafica,"|<f%d1> Apuntador %d: %d",Inodo->Apuntadores[Indice],Indice,Inodo->Apuntadores[Indice]);

                        Indice++;
                    }
                    fprintf(Grafica,"\",height=1 ];\n");

                    //Graficando las relaciones entre el Inodo y sus bloque Asociados a el
                    Indice=0;
                    while(Indice<15)
                    {
                        if(Inodo->Apuntadores[Indice]!=-1)
                        {
                            fprintf(Grafica,"node%d1:f%d1 -> node%d2:z%d2;\n",Contador+1,Inodo->Apuntadores[Indice],Inodo->Apuntadores[Indice],Inodo->Apuntadores[Indice]);//Pendiente
                        }
                        Indice++;
                    }

                    //Se extraen los bloques del inodo
                    IndiceApunt =0;
                    while(IndiceApunt < 15)
                    {
                        if(IndiceApunt==12)
                        {
                            //Primer Apuntador Indirec
                        }else if(IndiceApunt==13)
                        {
                            //Apuntador Indirecto Doble
                        }else if(IndiceApunt==14)
                        {
                            //Apuntador Indirecto Triple
                        }else
                        {
                            //Apuntadores Directos
                            if(Inodo->Apuntadores[IndiceApunt]!=-1)
                            {
                                //EXISTE un bloque

                                if(Inodo->Tipo[0]=='0')
                                {
                                    //El Inodo es una Carpeta por lo tanto lo que tendra son bloques de Carpetas
                                    struct BloqueCarpetas *Carp;
                                    Carp = malloc(sizeof(struct BloqueCarpetas));
                                    ByteInicialBloque = SBEstructura->InicioTablaBloques + ((Inodo->Apuntadores[IndiceApunt]-1)*TamanoBloque);
                                    fseek(Disco,ByteInicialBloque,SEEK_SET);
                                    fread(Carp,TamanoBloque,1,Disco);
                                    //Escribiendo en la Grafica;
                                    fprintf(Grafica,"node%d2 [label=\" <z%d2>Bloque Carpeta: %d ",Inodo->Apuntadores[IndiceApunt],Inodo->Apuntadores[IndiceApunt],Inodo->Apuntadores[IndiceApunt]);
                                        //Evaluando los Apuntadores del bloque Carpeta
                                        IndiceCarp = 0;
                                        while(IndiceCarp < 4)
                                        {
                                            if(Carp->ApuntadorCarpeta[IndiceCarp].Apuntador!=-1)
                                            {   //Esta Apuntando a un Inodo
                                                fprintf(Grafica,"|<f%d2> Apuntador: %d  Nombre: %s",Carp->ApuntadorCarpeta[IndiceCarp].Apuntador,Carp->ApuntadorCarpeta[IndiceCarp].Apuntador,Carp->ApuntadorCarpeta[IndiceCarp].Nombre);
                                            }else
                                            {
                                                //No Apunta a nada
                                                fprintf(Grafica,"| Apuntador: -1 Nombre: none");
                                            }

                                            IndiceCarp++;
                                        }
                                        fprintf(Grafica,"\",height=1 ];\n");
                                        //Graficando las Relaciones del Bloque a sus Inodo Asociados
                                        IndiceCarp =0;
                                        while(IndiceCarp < 4)
                                        {
                                            if(IndiceApunt==0)
                                            {
                                                if((Carp->ApuntadorCarpeta[IndiceCarp].Apuntador!=-1) &&(IndiceCarp>1))
                                                {
                                                    fprintf(Grafica,"node%d2:f%d2 -> node%d1:z%d1;\n",Inodo->Apuntadores[IndiceApunt],Carp->ApuntadorCarpeta[IndiceCarp].Apuntador,Carp->ApuntadorCarpeta[IndiceCarp].Apuntador,Carp->ApuntadorCarpeta[IndiceCarp].Apuntador);
                                                }
                                            }else
                                            {
                                                if((Carp->ApuntadorCarpeta[IndiceCarp].Apuntador!=-1))
                                                {
                                                    fprintf(Grafica,"node%d2:f%d2 -> node%d1:z%d1;\n",Inodo->Apuntadores[IndiceApunt],Carp->ApuntadorCarpeta[IndiceCarp].Apuntador,Carp->ApuntadorCarpeta[IndiceCarp].Apuntador,Carp->ApuntadorCarpeta[IndiceCarp].Apuntador);
                                                }
                                            }
                                            IndiceCarp++;
                                        }

                                    free(Carp);

                                }else if(Inodo->Tipo[0]=='1')
                                {
                                    //El Inodo es un Archivo por lo tanto lo que tendra son Bloques de Archivos
                                    struct Archivos *Arch;
                                    Arch = malloc(TamanoBloque);
                                    ByteInicialBloque = SBEstructura->InicioTablaBloques + ((Inodo->Apuntadores[IndiceApunt]-1)*TamanoBloque);
                                    fseek(Disco,ByteInicialBloque,SEEK_SET);
                                    fread(Arch,TamanoBloque,1,Disco);

                                    //Escribiendo en la Grafica
                                    fprintf(Grafica,"node%d2 [ label=\" <z%d2> Bloque Archivos: %d ",Inodo->Apuntadores[IndiceApunt],Inodo->Apuntadores[IndiceApunt],Inodo->Apuntadores[IndiceApunt]);
                                    fprintf(Grafica,"| Contenido: %s",Arch->Contenido);
                                    fprintf(Grafica,"\",height=1 ];\n");
                                    free(Arch);
                                }
                            }
                        }

                        IndiceApunt++;

                    }

                    free(Inodo);

                }

                Contador++;
            }



        fprintf(Grafica,"\t}\n");
        fprintf(Grafica,"}\n");

    //Cerrando el archivo .dot y generando la imagen

    free(SBEstructura);
    fclose(Grafica);
    char Pa[120];
    strcpy(Pa,"dot -Tjpg GraficaArbol.dot -o ");
    strcat(Pa,DireccionDest);
    system(Pa);
    printf("Grafica generada correctamente\n");

}

//********************** METODO PARA MODIFICAR LA ULTIMA FECHA Y LAS VECES QUE HA SIDO MONTADO LA PARTICION ***************************************************************************************************
//*****************************************************************************************************************************************************************************************************************
void ModificarFechaVecesMontaje(struct Particion *Primaria, struct EBR *Logica,int TipoParticion,FILE *Disco)
{
    struct SuperBloque SBloque;
    struct SuperBloque *SBAuxiliar;

    SBAuxiliar = malloc(sizeof(struct SuperBloque));
    if(TipoParticion==1)
    {
        //Es una Particion Primaria
        fseek(Disco,Primaria->ByteInicial,SEEK_SET);
        fread(SBAuxiliar,sizeof(struct SuperBloque),1,Disco);

    }else if(TipoParticion==2)
    {
        //Es una Particion Logica
        fseek(Disco,Logica->ByteInicial,SEEK_SET);
        fread(SBAuxiliar,sizeof(struct SuperBloque),1,Disco);
    }

    //Verificando que la particion ya haya sido formateada
    if(SBAuxiliar->TamanoInodo==0)
        return;

    //Compiando todos los valores al nuevo SuperBloque
    SBloque.BloquesLibres = SBAuxiliar->BloquesLibres;
    SBloque.InicioBitmapBloques = SBAuxiliar->InicioBitmapBloques;
    SBloque.InicioBitmapInodos = SBAuxiliar->InicioBitmapInodos;
    SBloque.InicioTablaBloques = SBAuxiliar->InicioTablaBloques;
    SBloque.InicioTablaInodos = SBAuxiliar->InicioTablaInodos;
    SBloque.InodosLibres = SBAuxiliar->InodosLibres;
    SBloque.PrimerBloqueLibre = SBAuxiliar->PrimerBloqueLibre;
    SBloque.PrimerInodoLibre = SBAuxiliar->PrimerInodoLibre;
    SBloque.SistemaArchivos = SBAuxiliar->SistemaArchivos;
    SBloque.TamanoBloque = SBAuxiliar->TamanoBloque;
    SBloque.TamanoInodo = SBAuxiliar->TamanoInodo;
    SBloque.TotalBloques = SBAuxiliar->TotalBloques;
    SBloque.TotalInodos = SBAuxiliar->TotalInodos;
    SBloque.UltimaFechaDesmont = SBAuxiliar->UltimaFechaDesmont;

    //Obteniendo la Fecha
    time_t FechaMontado;
    FechaMontado = time(0);
    struct tm *Tiempo1 = localtime(&FechaMontado);
    char fechaMontaje[20];
    strftime(fechaMontaje,128,"%d/%m/%y %H:%M:%S",Tiempo1);
    printf("%s\n",fechaMontaje);

    SBloque.UltimaFechaMontura = FechaMontado;
    SBloque.VecesMontado = SBAuxiliar->VecesMontado+1;

    Tiempo1 = localtime(&SBloque.UltimaFechaMontura);
    strftime(fechaMontaje,128,"%d/%m/%y %H:%M:%S",Tiempo1);
    printf("%s\n",fechaMontaje);


    if(TipoParticion==1)
        InsertarSuperBloque(&SBloque,Disco,Primaria->ByteInicial);
    else if(TipoParticion==2)
        InsertarSuperBloque(&SBloque,Disco,Logica->ByteInicial);

    free(SBAuxiliar);
}

//*********************************** METODO PARA BUSCAR UNA PARTICION LOGICA *************************************************************************************************************
//*********************************************************************************************************************************************************************************************
bool BuscarParticionLogica(FILE *Disco,int ByteInicial, char *Nombre,int TipoOperacion,char *DirecGrafica)
{
    //Si TipoOperacion ==1--->> InsertarFormateo
    //Si TipoOperacion ==2--->> GraficaSuperBloque
    int ByteLogica;
    bool Estado = false;

    struct EBR *EBRAuxiliar;
    EBRAuxiliar = malloc(sizeof(struct EBR));
    fseek(Disco,ByteInicial,SEEK_SET);
    fread(EBRAuxiliar,sizeof(struct EBR),1,Disco);

    if(strcmp(EBRAuxiliar->Nombre,Nombre)==0)
    {
        //la Particion se Encuentra en el Primer EBR
        if(TipoOperacion==1)
            InsertarFormateo(NULL,EBRAuxiliar,2,Disco);
        else if(TipoOperacion==2)
            GraficarSuperBloque(NULL,EBRAuxiliar,2,DirecGrafica,Disco);
        else if(TipoOperacion==3)
            GraficarBitmapInodos(NULL,EBRAuxiliar,2,DirecGrafica,Disco);
        else if(TipoOperacion==4)
            GraficarBitmapBloques(NULL,EBRAuxiliar,2,DirecGrafica,Disco);
        else if(TipoOperacion==5)
            InsertarFAST(NULL,EBRAuxiliar,2,Disco);
        else if(TipoOperacion==6)
            ModificarFechaVecesMontaje(NULL,EBRAuxiliar,2,Disco);
        else if(TipoOperacion==7)
            GraficarBloquesOcupados(NULL,EBRAuxiliar,2,DirecGrafica,Disco);
        else if(TipoOperacion==8)
            GraficarInodosOcupados(NULL,EBRAuxiliar,2,DirecGrafica,Disco);
        else if(TipoOperacion==9)
            GraficarArbol(NULL,EBRAuxiliar,2,DirecGrafica,Disco);

        Estado=true;

    }

    while((EBRAuxiliar->ProximoEBR!=(-1))&& Estado==false)
    {
        struct EBR *EBRSiguiente;

        ByteLogica = EBRAuxiliar->ProximoEBR;
        fseek(Disco,ByteLogica,SEEK_SET);

        EBRSiguiente = malloc(sizeof(struct EBR));
        fread(EBRSiguiente,sizeof(struct EBR),1,Disco);
        if(strcmp(EBRSiguiente->Nombre,Nombre)==0)
        {
            //La particion solicitada se encuentra despues del segundo EBR
            if(TipoOperacion==1)
                InsertarFormateo(NULL,EBRAuxiliar,2,Disco);
            else if(TipoOperacion==2)
                GraficarSuperBloque(NULL,EBRAuxiliar,2,DirecGrafica,Disco);
            else if(TipoOperacion==3)
                GraficarBitmapInodos(NULL,EBRAuxiliar,2,DirecGrafica,Disco);
            else if(TipoOperacion==4)
                GraficarBitmapBloques(NULL,EBRAuxiliar,2,DirecGrafica,Disco);
            else if(TipoOperacion==5)
                InsertarFAST(NULL,EBRAuxiliar,2,Disco);
            else if(TipoOperacion==6)
                ModificarFechaVecesMontaje(NULL,EBRAuxiliar,2,Disco);
            else if(TipoOperacion==7)
                GraficarBloquesOcupados(NULL,EBRAuxiliar,2,DirecGrafica,Disco);
            else if(TipoOperacion==8)
                GraficarInodosOcupados(NULL,EBRAuxiliar,2,DirecGrafica,Disco);
            else if(TipoOperacion==9)
                GraficarArbol(NULL,EBRAuxiliar,2,DirecGrafica,Disco);

            Estado=true;

        }else
        {
            //Corriendo los apuntadores
            fseek(Disco,EBRAuxiliar->ProximoEBR,SEEK_SET);
            free(EBRAuxiliar);
            EBRAuxiliar = malloc(sizeof(struct EBR));
            fread(EBRAuxiliar,sizeof(struct EBR),1,Disco);

        }
        free(EBRSiguiente);
    }

    free(EBRAuxiliar);
    return Estado;   //Retorno True, se encontro la logica, false no se encontro
}

//********************************* METODO PARA REALIZAR EL FORMATEO RAPIDO ******************************************************************************************************
//********************************************************************************************************************************************************************************
void RealizarFormateoFast(char *IdRec)
{
    //En este solo se limpa el bitmap de Inodos y bloques

    char *Direcc,*NombreParticion;
    struct MBR *EstrucMBR;

    Direcc = DevolverPath(IdRec);
    NombreParticion = malloc(100*sizeof(char));
    strcpy(NombreParticion,Devolverparticion(IdRec));
    if(strcmp(Direcc,"a")==0)
    {
        printf(">>>>ERROR: el id introducido no ha sido montado <<<<\n");

    }else
    {
        //El id ya se ha montado
        FILE *DiscoLeer;
        DiscoLeer = fopen(Direcc,"rb+");
        if(DiscoLeer==NULL)
        {
            printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
            return;
        }
        EstrucMBR = malloc(4*sizeof(struct MBR)+1);
        fseek(DiscoLeer,0,SEEK_SET);
        fread(EstrucMBR,sizeof(struct MBR),1,DiscoLeer);
        //Buscando la Particion a formatear

        if((strcmp(EstrucMBR->Particion1.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion1.Tipo,"p")==0))
        {
            InsertarFAST(&EstrucMBR->Particion1,NULL,1,DiscoLeer);

        }else if((strcmp(EstrucMBR->Particion2.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion2.Tipo,"p")==0) )
        {
            InsertarFAST(&EstrucMBR->Particion2,NULL,1,DiscoLeer);

        }else if((strcmp(EstrucMBR->Particion3.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion3.Tipo,"p")==0) )
        {
            InsertarFAST(&EstrucMBR->Particion3,NULL,1,DiscoLeer);

        }else if((strcmp(EstrucMBR->Particion4.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion4.Tipo,"p")==0) )
        {
            InsertarFAST(&EstrucMBR->Particion4,NULL,1,DiscoLeer);

        }else if(strcasecmp(EstrucMBR->Particion1.Tipo,"e")==0)
        {
            //Se Busca la particion Logica
            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion1.ByteInicial,NombreParticion,5,NULL))
            {
                printf("Particion Logica\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }
        }else if(strcasecmp(EstrucMBR->Particion2.Tipo,"e")==0)
        {
            //Se Busca la particion Logica
            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion2.ByteInicial,NombreParticion,5,NULL))
            {
                printf("Particion Logica\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }

        }else if(strcasecmp(EstrucMBR->Particion3.Tipo,"e")==0)
        {
            //Se Busca la particion Logica
            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion3.ByteInicial,NombreParticion,5,NULL))
            {
                printf("Particion Logica\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }

        }else if(strcasecmp(EstrucMBR->Particion4.Tipo,"e")==0)
        {
            //Se Busca la particion Logica
            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion4.ByteInicial,NombreParticion,5,NULL))
            {
                printf("Particion Logica\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }
        }else
            printf(">>>>ERROR: no se pudo formatear la particion indicada, no se encontro dentro del disco <<<<\n");

        fclose(DiscoLeer);
        free(EstrucMBR);



    }
    free(NombreParticion);

}

//********************************** METODO PARA REALIZAR EL FORMATEO COMPLETO ***********************************************************************************************
//*************************************************************************************************************************************************************************************
 void RealizarFormateoFull(char *IdRec)
 {
    //Inicio

    char *Direcc,*NombreParticion;
    struct MBR *EstrucMBR;

    int NumeroInodos;

    Direcc = DevolverPath(IdRec);
    NombreParticion = malloc(100*sizeof(char));
    strcpy(NombreParticion,Devolverparticion(IdRec));


    if(strcmp(Direcc,"a")==0)
    {
        printf(">>>>ERROR: el id introducido no ha sido montado <<<<\n");

    }else
    {

        //El id ya se ha montado
        FILE *DiscoLeer;
        DiscoLeer = fopen(Direcc,"rb+");
        if(DiscoLeer==NULL)
        {
            printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
            return;
        }

        EstrucMBR = malloc(4*sizeof(struct MBR)+1);
        fseek(DiscoLeer,0,SEEK_SET);
        fread(EstrucMBR,sizeof(struct MBR),1,DiscoLeer);


        //Buscando la Particion a formatear

        if((strcmp(EstrucMBR->Particion1.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion1.Tipo,"p")==0) )
        {
            InsertarFormateo(&EstrucMBR->Particion1,NULL,1,DiscoLeer);
            fclose(DiscoLeer);
            char Contenido[32];
            char *DireArch,*Taman;
            DireArch = malloc(11);
            Taman = malloc(2);
            strcpy(DireArch,"/users.txt");
            strcpy(Contenido,"1,G,root,\n 1,U,root,root,123\n");
            strcpy(Taman,"29");
            CrearArchivoConDireccion(IdRec,DireArch,NULL,Taman,NULL,Contenido);
            free(DireArch);
            free(Taman);

        }else if((strcmp(EstrucMBR->Particion2.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion2.Tipo,"p")==0) )
        {
            InsertarFormateo(&EstrucMBR->Particion2,NULL,1,DiscoLeer);
            fclose(DiscoLeer);
            char Contenido[32];
            char *DireArch,*Taman;
            DireArch = malloc(11);
            Taman = malloc(2);
            strcpy(DireArch,"/users.txt");
            strcpy(Contenido,"1,G,root,\n 1,U,root,root,123\n");
            strcpy(Taman,"29");
            CrearArchivoConDireccion(IdRec,DireArch,NULL,Taman,NULL,Contenido);
            free(DireArch);
            free(Taman);

        }else if( (strcmp(EstrucMBR->Particion3.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion3.Tipo,"p")==0) )
        {
            InsertarFormateo(&EstrucMBR->Particion3,NULL,1,DiscoLeer);
            fclose(DiscoLeer);
            char Contenido[32];
            char *DireArch,*Taman;
            DireArch = malloc(11);
            Taman = malloc(2);
            strcpy(DireArch,"/users.txt");
            strcpy(Contenido,"1,G,root,\n 1,U,root,root,123\n");
            strcpy(Taman,"29");
            CrearArchivoConDireccion(IdRec,DireArch,NULL,Taman,NULL,Contenido);
            free(DireArch);
            free(Taman);

        }else if( (strcmp(EstrucMBR->Particion4.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion4.Tipo,"p")==0) )
        {

            InsertarFormateo(&EstrucMBR->Particion4,NULL,1,DiscoLeer);
            fclose(DiscoLeer);
            char Contenido[32];
            char *DireArch,*Taman;
            DireArch = malloc(11);
            Taman = malloc(2);
            strcpy(DireArch,"/users.txt");
            strcpy(Contenido,"1,G,root,\n 1,U,root,root,123\n");
            strcpy(Taman,"29");
            CrearArchivoConDireccion(IdRec,DireArch,NULL,Taman,NULL,Contenido);
            free(DireArch);
            free(Taman);

        }else if(strcasecmp(EstrucMBR->Particion1.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            //Se Busca la particion Logica
            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion1.ByteInicial,NombreParticion,1,NULL))
            {
                printf("Particion Logica\n");
                fclose(DiscoLeer);
                char Contenido[32];
                char *DireArch,*Taman;
                DireArch = malloc(11);
                Taman = malloc(2);
                strcpy(DireArch,"/users.txt");
                strcpy(Contenido,"1,G,root,\n 1,U,root,root,123\n");
                strcpy(Taman,"29");
                CrearArchivoConDireccion(IdRec,DireArch,NULL,Taman,NULL,Contenido);
                free(DireArch);
                free(Taman);
            }else
            {
                fclose(DiscoLeer);
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }



        }else if(strcasecmp(EstrucMBR->Particion2.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion2.ByteInicial,NombreParticion,1,NULL))
            {
                printf("Particion Logica\n");
                fclose(DiscoLeer);
                char Contenido[32];
                char *DireArch,*Taman;
                DireArch = malloc(11);
                Taman = malloc(2);
                strcpy(DireArch,"/users.txt");
                strcpy(Contenido,"1,G,root,\n 1,U,root,root,123\n");
                strcpy(Taman,"29");
                CrearArchivoConDireccion(IdRec,DireArch,NULL,Taman,NULL,Contenido);
                free(DireArch);
                free(Taman);
            }else
            {
                fclose(DiscoLeer);
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }


        }else if(strcasecmp(EstrucMBR->Particion3.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion3.ByteInicial,NombreParticion,1,NULL))
            {
                printf("Particion Logicqa\n");
                fclose(DiscoLeer);
                char Contenido[32];
                char *DireArch,*Taman;
                DireArch = malloc(11);
                Taman = malloc(2);
                strcpy(DireArch,"/users.txt");
                strcpy(Contenido,"1,G,root,\n 1,U,root,root,123\n");
                strcpy(Taman,"29");
                CrearArchivoConDireccion(IdRec,DireArch,NULL,Taman,NULL,Contenido);
                free(DireArch);
                free(Taman);
            }else
            {
                fclose(DiscoLeer);
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }


        }else if(strcasecmp(EstrucMBR->Particion4.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion4.ByteInicial,NombreParticion,1,NULL))
            {
                printf("Particion Logica\n");
                fclose(DiscoLeer);
                char Contenido[32];
                char *DireArch,*Taman;
                DireArch = malloc(11);
                Taman = malloc(2);
                strcpy(DireArch,"/users.txt");
                strcpy(Contenido,"1,G,root,\n 1,U,root,root,123\n");
                strcpy(Taman,"29");
                CrearArchivoConDireccion(IdRec,DireArch,NULL,Taman,NULL,Contenido);
                free(DireArch);
                free(Taman);
            }else
            {
                fclose(DiscoLeer);
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }


        }else{
            printf(">>>>ERROR: no se pudo formatear la particion indicada, no se encontro dentro del disco <<<<\n");
            fclose(DiscoLeer);
        }


        free(EstrucMBR);




    }
    free(NombreParticion);

}
 //**************************************** METODO PARA CREAR REPORTE DEL SUPER BLOQUE ******************************************************************************
 //****************************************************************************************************************************************************************************

 void CrearReporteSuperBloque(char *DirecGrafica, char *IdRec)
 {


    struct MBR *EstrucMBR;



    char DiscoDireccion[125];
    char *NombreParticion;
    NombreParticion = malloc(100*sizeof(char));
    strcpy(NombreParticion,Devolverparticion(IdRec));
    strcpy(DiscoDireccion,DevolverPath(IdRec));

    if((strcasecmp(DiscoDireccion,"a"))!=0)
    {
        //El id ya se ha montado
        FILE *DiscoLeer;
        DiscoLeer = fopen(DiscoDireccion,"rb+");
        if(DiscoLeer==NULL)
        {
            printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
            return;
        }

        EstrucMBR = malloc(4*sizeof(struct MBR)+1);
        fseek(DiscoLeer,0,SEEK_SET);
        fread(EstrucMBR,sizeof(struct MBR),1,DiscoLeer);


        //Buscando la Particion a grafica

        if((strcmp(EstrucMBR->Particion1.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion1.Tipo,"p")==0) )
        {
            GraficarSuperBloque(&EstrucMBR->Particion1,NULL,1,DirecGrafica,DiscoLeer);

        }else if((strcmp(EstrucMBR->Particion2.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion2.Tipo,"p")==0) )
        {

             GraficarSuperBloque(&EstrucMBR->Particion2,NULL,1,DirecGrafica,DiscoLeer);

        }else if( (strcmp(EstrucMBR->Particion3.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion3.Tipo,"p")==0) )
        {
            GraficarSuperBloque(&EstrucMBR->Particion3,NULL,1,DirecGrafica,DiscoLeer);

        }else if( (strcmp(EstrucMBR->Particion4.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion4.Tipo,"p")==0) )
        {

            GraficarSuperBloque(&EstrucMBR->Particion4,NULL,1,DirecGrafica,DiscoLeer);

        }else if(strcasecmp(EstrucMBR->Particion1.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            //Se Busca la particion Logica
            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion1.ByteInicial,NombreParticion,2,DirecGrafica))
            {
                printf("Grafica de SuperBloque en particion Logica creada Correctamente\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }



        }else if(strcasecmp(EstrucMBR->Particion2.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion2.ByteInicial,NombreParticion,2,DirecGrafica))
            {
                printf("Grafica de SuperBloque en particion Logica creada Correctamente\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }


        }else if(strcasecmp(EstrucMBR->Particion3.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion3.ByteInicial,NombreParticion,2,DirecGrafica))
            {
                printf("Grafica de SuperBloque en particion Logica creada Correctamente\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }


        }else if(strcasecmp(EstrucMBR->Particion4.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion4.ByteInicial,NombreParticion,2,DirecGrafica))
            {
                printf("Grafica de SuperBloque en particion Logica creada Correctamente\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }


        }else
            printf(">>>>ERROR: no se pudo crear la grafica de la particion indicada, no se encontro dentro del disco <<<<\n");

        fclose(DiscoLeer);
        free(EstrucMBR);


    }else
        printf(">>>>ERROR: imposible graficar Disco no encontrado<<<<\n");

 }


 //**************** METODO PARA BUSCAR LA PARTCION SOLICITADA Y CREAR EL GRAFICO DE BITMAP DE INODOS ***************************************************************
 //*********************************************************************************************************************************************************************************
 void CrearReporteBitmapInodos(char *DirecGrafica, char *Id)
 {
    struct MBR *EstrucMBR;

    char DiscoDireccion[125];
    char *NombreParticion;
    NombreParticion = malloc(100*sizeof(char));
    strcpy(NombreParticion,Devolverparticion(Id));
    strcpy(DiscoDireccion,DevolverPath(Id));

    if((strcasecmp(DiscoDireccion,"a"))!=0)
    {
        //El id ya se ha montado
        FILE *DiscoLeer;
        DiscoLeer = fopen(DiscoDireccion,"rb+");
        if(DiscoLeer==NULL)
        {
            printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
            return;
        }

        EstrucMBR = malloc(4*sizeof(struct MBR)+1);
        fseek(DiscoLeer,0,SEEK_SET);
        fread(EstrucMBR,sizeof(struct MBR),1,DiscoLeer);


        //Buscando la Particion a grafica

        if((strcmp(EstrucMBR->Particion1.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion1.Tipo,"p")==0) )
        {
            GraficarBitmapInodos(&EstrucMBR->Particion1,NULL,1,DirecGrafica,DiscoLeer);

        }else if((strcmp(EstrucMBR->Particion2.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion2.Tipo,"p")==0) )
        {

            GraficarBitmapInodos(&EstrucMBR->Particion2,NULL,1,DirecGrafica,DiscoLeer);

        }else if( (strcmp(EstrucMBR->Particion3.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion3.Tipo,"p")==0) )
        {

            GraficarBitmapInodos(&EstrucMBR->Particion3,NULL,1,DirecGrafica,DiscoLeer);

        }else if( (strcmp(EstrucMBR->Particion4.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion4.Tipo,"p")==0) )
        {

            GraficarBitmapInodos(&EstrucMBR->Particion4,NULL,1,DirecGrafica,DiscoLeer);

        }else if(strcasecmp(EstrucMBR->Particion1.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            //Se Busca la particion Logica
            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion1.ByteInicial,NombreParticion,3,DirecGrafica))
            {
                printf("Grafica de Bitmap de Inodos en particion Logica creada Correctamente\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }



        }else if(strcasecmp(EstrucMBR->Particion2.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion2.ByteInicial,NombreParticion,3,DirecGrafica))
            {
                printf("Grafica de Bitmap de Inodos en particion Logica creada Correctamente\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }


        }else if(strcasecmp(EstrucMBR->Particion3.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion3.ByteInicial,NombreParticion,3,DirecGrafica))
            {
                printf("Grafica de Bitmap de Inodos en particion Logica creada Correctamente\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }


        }else if(strcasecmp(EstrucMBR->Particion4.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion4.ByteInicial,NombreParticion,3,DirecGrafica))
            {
                printf("Grafica de Bimapt de Inodos en la particion Logica creada Correctamente\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }


        }else
            printf(">>>>ERROR: no se pudo crear la grafica de la particion indicada, no se encontro dentro del disco <<<<\n");

        fclose(DiscoLeer);
        free(EstrucMBR);


    }else
        printf(">>>>ERROR: imposible graficar Disco no encontrado<<<<\n");


 }

 //**************** METODO PARA UBICAR LA PARTICION Y LUEGO CREAR EL REPORTE DE BITMAP DE BLOQUES ********************************************************************************************************
 //*****************************************************************************************************************************************************************************************************************
 void CrearReporteBitmapBloques(char *DirecGrafica, char *Id)
 {
    struct MBR *EstrucMBR;

    char DiscoDireccion[125];
    char *NombreParticion;
    NombreParticion = malloc(100*sizeof(char));
    strcpy(NombreParticion,Devolverparticion(Id));
    strcpy(DiscoDireccion,DevolverPath(Id));

    if((strcasecmp(DiscoDireccion,"a"))!=0)
    {
        //El id ya se ha montado
        FILE *DiscoLeer;
        DiscoLeer = fopen(DiscoDireccion,"rb+");
        if(DiscoLeer==NULL)
        {
            printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
            return;
        }

        EstrucMBR = malloc(4*sizeof(struct MBR)+1);
        fseek(DiscoLeer,0,SEEK_SET);
        fread(EstrucMBR,sizeof(struct MBR),1,DiscoLeer);


        //Buscando la Particion a grafica

        if((strcmp(EstrucMBR->Particion1.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion1.Tipo,"p")==0) )
        {
            GraficarBitmapBloques(&EstrucMBR->Particion1,NULL,1,DirecGrafica,DiscoLeer);

        }else if((strcmp(EstrucMBR->Particion2.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion2.Tipo,"p")==0) )
        {

            GraficarBitmapBloques(&EstrucMBR->Particion2,NULL,1,DirecGrafica,DiscoLeer);

        }else if( (strcmp(EstrucMBR->Particion3.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion3.Tipo,"p")==0) )
        {

            GraficarBitmapBloques(&EstrucMBR->Particion3,NULL,1,DirecGrafica,DiscoLeer);

        }else if( (strcmp(EstrucMBR->Particion4.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion4.Tipo,"p")==0) )
        {

            GraficarBitmapBloques(&EstrucMBR->Particion4,NULL,1,DirecGrafica,DiscoLeer);

        }else if(strcasecmp(EstrucMBR->Particion1.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            //Se Busca la particion Logica
            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion1.ByteInicial,NombreParticion,4,DirecGrafica))
            {
                printf("Grafica de Bitmap de Bloques en particion Logica creada Correctamente\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }



        }else if(strcasecmp(EstrucMBR->Particion2.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion2.ByteInicial,NombreParticion,4,DirecGrafica))
            {
                printf("Grafica de Bitmap de Bloques en particion Logica creada Correctamente\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }


        }else if(strcasecmp(EstrucMBR->Particion3.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion3.ByteInicial,NombreParticion,4,DirecGrafica))
            {
                printf("Grafica de Bitmap de Bloques en particion Logica creada Correctamente\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }


        }else if(strcasecmp(EstrucMBR->Particion4.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion4.ByteInicial,NombreParticion,4,DirecGrafica))
            {
                printf("Grafica de Bitmap de Bloques en la particion Logica creada Correctamente\n");
            }else
            {
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            }


        }else
            printf(">>>>ERROR: no se pudo crear la grafica de la particion indicada, no se encontro dentro del disco <<<<\n");

        fclose(DiscoLeer);
        free(EstrucMBR);


    }else
        printf(">>>>ERROR: imposible graficar Disco no encontrado<<<<\n");

 }

 //*************************************** METODO PARA MONTAR UNA PARTICION **************************************************************************
//**********************************************************************************************************************************************************
void MontarParticion(char *Path,char *Nombre)
{
    FILE *Disco;
    struct MBR *EstructuraMBR;
    int PosicionDisco;


    Disco = fopen(Path,"rb+");
    if(Disco==NULL)
    {
        printf(">>>>ERROR: Disco no Encontrado <<<<\n");
        return -1;
    }
    EstructuraMBR = malloc((4*sizeof(struct MBR))+1);
    fseek(Disco,0,SEEK_SET);
    fread(EstructuraMBR,sizeof(struct MBR),sizeof(EstructuraMBR),Disco);
    if(strcasecmp(EstructuraMBR->Particion1.Nombre,Nombre)==0)
    {
        PosicionDisco = InsertarNuevoDisco(Path);
        GenerarIdParticion(PosicionDisco,Nombre);
        ModificarFechaVecesMontaje(&EstructuraMBR->Particion1,NULL,1,Disco);

    }else if((strcasecmp(EstructuraMBR->Particion2.Nombre,Nombre))==0)
    {
        PosicionDisco = InsertarNuevoDisco(Path);
        GenerarIdParticion(PosicionDisco,Nombre);
        ModificarFechaVecesMontaje(&EstructuraMBR->Particion2,NULL,1,Disco);

    }else if((strcasecmp(EstructuraMBR->Particion3.Nombre,Nombre))==0)
    {
        PosicionDisco = InsertarNuevoDisco(Path);
        GenerarIdParticion(PosicionDisco,Nombre);
        ModificarFechaVecesMontaje(&EstructuraMBR->Particion3,NULL,1,Disco);
    }else if((strcasecmp(EstructuraMBR->Particion4.Nombre,Nombre))==0)
    {
        PosicionDisco = InsertarNuevoDisco(Path);
        GenerarIdParticion(PosicionDisco,Nombre);
        ModificarFechaVecesMontaje(&EstructuraMBR->Particion4,NULL,1,Disco);
    }else if(strcasecmp(EstructuraMBR->Particion1.Tipo,"e")==0)
    {
        if(BuscarParticionLogica(Disco,EstructuraMBR->Particion1.ByteInicial,Nombre,6,NULL))
        {
            PosicionDisco = InsertarNuevoDisco(Path);
            GenerarIdParticion(PosicionDisco,Nombre);
        }else
            printf(">>>>ERROR: nombre de particion Logica no encontrada<<<<\n");

    }else if(strcasecmp(EstructuraMBR->Particion2.Tipo,"e")==0)
    {
        if(BuscarParticionLogica(Disco,EstructuraMBR->Particion2.ByteInicial,Nombre,6,NULL))
        {
            PosicionDisco = InsertarNuevoDisco(Path);
            GenerarIdParticion(PosicionDisco,Nombre);
        }else
            printf(">>>>ERROR: nombre de particion Logica no encontrada<<<<\n");

    }else if(strcasecmp(EstructuraMBR->Particion3.Tipo,"e")==0)
    {
        if(BuscarParticionLogica(Disco,EstructuraMBR->Particion3.ByteInicial,Nombre,6,NULL))
        {
            PosicionDisco = InsertarNuevoDisco(Path);
            GenerarIdParticion(PosicionDisco,Nombre);
        }else
            printf(">>>>ERROR: nombre de particion Logica no encontrada<<<<\n");

    }else if(strcasecmp(EstructuraMBR->Particion4.Tipo,"e")==0)
    {
        if(BuscarParticionLogica(Disco,EstructuraMBR->Particion4.ByteInicial,Nombre,6,NULL))
        {
            PosicionDisco = InsertarNuevoDisco(Path);
            GenerarIdParticion(PosicionDisco,Nombre);
        }else
            printf(">>>>ERROR: nombre de particion Logica no encontrada<<<<\n");

    }else
        printf(">>>>ERROR: nombre de particion no encontrada<<<<\n");

    fclose(Disco);
    free(EstructuraMBR);
}

//************************ METODO PARA BUSCAR LA CARPETA PADRE QUE CONTENDRA UN ARCHIVO *************************************************************************************
//************************************************************************************************************************************************************************************************
int BuscarCarpetaContenedora(struct Particion *Primaria, struct EBR *Logica, char *Direccion,FILE *Disco,int TipoPart)
{
    //El metodo devolvera el byte donde se encuentra el inodo padre del archivo

    struct SuperBloque *SBEstructura;
    int ContadorCarpetas;

    SBEstructura = malloc(sizeof(struct SuperBloque));
    ContadorCarpetas = 0;

    //Leyendo el superBloque de la partcion
        if(TipoPart==1)
    {
        fseek(Disco,Primaria->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);
    }else if(TipoPart==2)
    {
        fseek(Disco,Logica->ByteInicial,SEEK_SET);
        fread(SBEstructura,sizeof(struct SuperBloque),1,Disco);
    }

    //Validando que ya exista un sistema ext2 en la particion
    if(SBEstructura->TamanoInodo==0)
    {
        printf(">>>>ERROR: imposible crear archivo en la particion solicitada no existe un sistema de archivos<<<<\n");
        free(SBEstructura);
       return -1;    //significa error
    }

    //Spliteando para buscar si las carpetas ya existen

    char Palabra[80];
    int i;
    int TamanoNom,ByteInodo,ByteBloque,ByteEnviar;
    bool ExisteCarpeta;
    int Indice,IndiceCarpetas;
    Direccion++;
    while(sscanf(Direccion,"%150[^/]%n",Palabra,&i)==1)
    {
        //Se Verifica que lo que spliteo sea una carpeta y no un archivo

        ExisteCarpeta = false;
        Indice=0;
        TamanoNom = strlen(Palabra);



        struct TablaInodos *Inodo;
        Inodo = malloc(sizeof(struct TablaInodos));
        //Lo que se evalua es una carpeta
        //Si es la primera vez se empieza desde el primer inodo
        if(ContadorCarpetas==0)
        {
            //Es empieza en la raiz, que es el primer Inodo
            ByteInodo = SBEstructura->InicioTablaInodos;
            fseek(Disco,ByteInodo,SEEK_SET);
            fread(Inodo,sizeof(struct TablaInodos),1,Disco);

        }else
        {
            //Se sigue buscando en el capeta padre
            ByteInodo = ByteEnviar;
            fseek(Disco,ByteInodo,SEEK_SET);
            fread(Inodo,sizeof(struct TablaInodos),1,Disco);

        }
        //Buscando en los apuntadores(Bloques) si la carpeta existe
        int Ap = 0;
        for(Ap =0; Ap < 15;Ap++)
        {

            if(Inodo->Apuntadores[Ap]!=-1)
            {
                //El apuntador si apunta a un bloque
                ByteBloque = SBEstructura->InicioTablaBloques;
                ByteBloque =  ByteBloque + ((Inodo->Apuntadores[Ap]-1) * sizeof(struct BloqueCarpetas));

                if(Ap==12)
                {
                    //Primer apuntador Indirecto

                }else if(Ap==13)
                {
                            //Apuntador doble Indirecto

                }else if(Ap==14)
                {
                            //Apuntador Triple Indirecto

                }else
                {
                    //Apuntadores DIRECTO
                    //Se verifica si existe la carpeta en el bloque de Carpetas
                    struct BloqueCarpetas *BloqueCarp;
                    IndiceCarpetas = 0;
                    ExisteCarpeta = false;

                    BloqueCarp = malloc(sizeof(struct BloqueCarpetas));
                    fseek(Disco,ByteBloque,SEEK_SET);
                    fread(BloqueCarp,sizeof(struct BloqueCarpetas),1,Disco);

                    //Vericando si existe la carpeta
                    while( (IndiceCarpetas < 4) && (ExisteCarpeta==false) )
                    {
                        if(BloqueCarp->ApuntadorCarpeta[IndiceCarpetas].Apuntador != -1)
                        {
                            if(strcmp(BloqueCarp->ApuntadorCarpeta[IndiceCarpetas].Nombre,Palabra)==0)
                            {
                                //La Carpeta SI EXISTE
                                ExisteCarpeta = true;
                                ByteEnviar = SBEstructura->InicioTablaInodos + ( (BloqueCarp->ApuntadorCarpeta[IndiceCarpetas].Apuntador -1)*sizeof(struct TablaInodos));
                                Ap=15;  //Para que salga del for
                            }
                        }
                        IndiceCarpetas++;
                    }
                    free(BloqueCarp);

                }

            }
        }

        if(ExisteCarpeta==false)
            return -1;   //Significa error

        free(Inodo);



        Direccion+=i;
        ContadorCarpetas++;
        if(*Direccion!= '/')
            break;

        while(*Direccion== '/')
                ++Direccion;


    }

    if((ContadorCarpetas==0))
    {
        //El Archivo se escribira en la raiz, primer Inodo
        ByteEnviar = SBEstructura->InicioTablaInodos;
        free(SBEstructura);
        return ByteEnviar;

    }else if(ExisteCarpeta ==true)
    {
        free(SBEstructura);
        return ByteEnviar;
    }

    free(SBEstructura);
    return -1;   //Significa error


}

//**************** METODO PARA VERIFICAR SI EXITE ALGUN ESPACIO DENTRO DE UN BLOQUE DE APUNTADORES ***********************************************************************************
//************************************************************************************************************************************************************************************
int VerificarEspacioBloqueApuntadores(struct BloqueApuntadores *Apuntadores, int Tipo, struct SuperBloque *SuperBl,FILE *Disco)
{
    int Indice;
    Indice=0;

    if(Tipo==1)
    {
        //SE Busca un espacio dentro del bloque de apuntadores
        while(Indice < 16)
        {
            if(Apuntadores->Apuntadores[Indice] == -1) //Si Existe Espacio dentro del Blque se manda la posicion
                return Indice;
            Indice++;
        }
    }else if(Tipo==2)
    {
        //Se Busca un espacio dentro del bloque de Carpetas contenido dentro del bloque de apuntadores
        while(Indice < 16)
        {
            if(Apuntadores->Apuntadores[Indice] != -1)
            {
                //Apuntador esta ocupado por lo cual se acede a su bloque de carpetas
                struct BloqueCarpetas *BloqueCarp;
                int byteCarpeta,Indice2;
                BloqueCarp = malloc(sizeof(struct Carpetas));
                byteCarpeta = Apuntadores->Apuntadores[Indice] -1;
                byteCarpeta = SuperBl->InicioTablaBloques + (byteCarpeta * (sizeof(struct Carpetas)));
                fseek(Disco,byteCarpeta,SEEK_SET);
                fread(BloqueCarp,sizeof(struct Carpetas),1,Disco);
                //Se Verifica si existe espacio dentro del bloque de carpetas que tiene este apuntador Indirecto
                Indice2 = 0;
                while(Indice2 < 4)
                {
                    if(BloqueCarp->ApuntadorCarpeta[Indice2].Apuntador == -1)
                    {
                        free(BloqueCarp);
                        return BloqueCarp->ApuntadorCarpeta[Indice2].Apuntador;  //devuelve en numero del bloque carpeta donde se guardara el nuevo inodo
                    }
                    Indice2++;
                }
                free(BloqueCarp);
            }
            Indice++;
        }
    }
    return -1;   //Siginfica que no existe espacio dentro del bloque de apuntadores;
}

int CalcularNumeroInodo(struct SuperBloque *SuperBl, char *TipoAjuste, FILE *Disco)
{
    int InicioBitmapInodos,TotalInodos,Indice,Acumulador,Posicion;

    InicioBitmapInodos = SuperBl->InicioBitmapInodos;
    TotalInodos = SuperBl->TotalInodos;
    char BitmapInodos[TotalInodos];
    fseek(Disco,InicioBitmapInodos,SEEK_SET);
    fread(BitmapInodos,TotalInodos,1,Disco);
    if(strcasecmp(TipoAjuste,"BF")==0)
    {
        //Mejor Ajuste
        //Se elije el espacio mas pequeño
        Cap *Cabec;
        Cabec = malloc(sizeof(Cabecera));
        InicializarLista(Cabec);
        Indice = 0;
        Acumulador = 0;
        while(Indice < TotalInodos)
        {
            if(BitmapInodos[Indice]=='0')
                Acumulador++;
            else
            {
                if(Acumulador >= 1)   //Es un Inodo el que se creara
                {
                    InsertarDato(Cabec,Indice-Acumulador,Acumulador);
                    Acumulador=0;
                }
                else
                    Acumulador =0;
            }
            Indice++;

        }
        if(Acumulador!=0)
        {
            if(Acumulador >= 1)
                InsertarDato(Cabec,Indice-Acumulador,Acumulador);
        }

        //Por ser el mejor Ajuste se escoje el espacio mas pequeño, que seria el ultimo nodo de la lista
        Posicion = DevolverValor(Cabec,2);
        //Borrando la Lista
        BorrarNodos(Cabec);
        free(Cabec);
        return Posicion;

    }else if(strcasecmp(TipoAjuste,"FF")==0)
    {
        //Primer Ajuste
        //Se elige el primer espacio donde quepa
        Indice = 0;
        Acumulador =0;
        while(Indice < TotalInodos)
        {
            if(BitmapInodos[Indice]=='0')
            {
                //La posicion ya ha sido ocupada
                Acumulador = Acumulador + 1;
                if(Acumulador >= 1)   //Solo un Inodo es el que se creara
                    return (Indice);
            }else
            {
                //La posicion ya ha sido ocupada
                Acumulador = 0;
            }
            Indice++;
        }
    }
    else if(strcasecmp(TipoAjuste,"WF")==0)
    {
        //Peor Ajuste
        //Se Elije el espacio mas grande
        Cap *Cabec;
        Cabec = malloc(sizeof(Cabecera));
        InicializarLista(Cabec);
        Indice = 0;
        Acumulador = 0;
        while(Indice < TotalInodos)
        {
            if(BitmapInodos[Indice]=='0')
                Acumulador++;
            else
            {
                if(Acumulador >= 1)
                {
                    InsertarDato(Cabec,Indice-Acumulador,Acumulador);
                    Acumulador=0;
                }
                else
                    Acumulador =0;
            }
            Indice++;

        }
        if(Acumulador!=0)
        {
            if(Acumulador >= 1)
                InsertarDato(Cabec,Indice-Acumulador,Acumulador);
        }
        //Por ser el Peor Ajuste se escoje el Tamaño mas grande que seria el primer nodo de la lista
        Posicion = DevolverValor(Cabec,1);
        //Borrando la Lista
        BorrarNodos(Cabec);
        free(Cabec);
        return Posicion;
    }
    return -1;    //Significa error
}

//**************** METODO PARA CALCULAR EL NUMERO DEL BLOQUE DONDE SE EMPEZARA A ESCRIBIR **************************************************************************************************
//***********************************************************************************************************************************************************************************************
int CalcularNumeroBloque(int Tamano, bool NuevoBloque, struct SuperBloque *SuperBl,char *TipoAjuste, FILE *Disco)
{
    //Devuelve la posicion a partir de donde se modificara el bitmap inodo
    int NumeroBloque,Bloques,Indice,Acumulador,InicioBitmapBloques;



    NumeroBloque = Tamano/64;
    if((NumeroBloque*64) < Tamano)
        NumeroBloque++;

    if(NuevoBloque == true)  //Se Creara un nuevo Bloque de Carpetas
        NumeroBloque = NumeroBloque +1;

    Bloques = SuperBl->TotalBloques;
    InicioBitmapBloques = SuperBl->InicioBitmapBloques;
    char BitmaptBl[Bloques];
    fseek(Disco,InicioBitmapBloques,SEEK_SET);
    fread(BitmaptBl,Bloques,1,Disco);

    if(strcasecmp(TipoAjuste,"BF")==0)
    {
        //MEJOR AJUSTE
        Cap *Cabec;
        Cabec = malloc(sizeof(Cabecera));
        InicializarLista(Cabec);
        Indice = 0;
        Acumulador = 0;
        while(Indice < Bloques)
        {
            if(BitmaptBl[Indice]=='0')
                Acumulador++;
            else
            {
                if(Acumulador >= NumeroBloque)
                {
                    InsertarDato(Cabec,Indice-Acumulador,Acumulador);
                    Acumulador=0;
                }
                else
                    Acumulador =0;
            }
            Indice++;
        }
        if(Acumulador!=0)
        {
            if(Acumulador >= NumeroBloque)
                InsertarDato(Cabec,Indice-Acumulador,Acumulador);
        }
        //Por ser el mejor Ajuste se escoje el espacio mas pequeño, que seria el ultimo nodo de la lista
        NumeroBloque = DevolverValor(Cabec,2);
        //Borrando la Lista
        BorrarNodos(Cabec);
        free(Cabec);
        return NumeroBloque;
    }
    else if(strcasecmp(TipoAjuste,"FF")==0)
    {
        //PRIMER AJUSTE
        Indice = 0;
        Acumulador =0;

        while(Indice < Bloques)
        {
            if(BitmaptBl[Indice]=='0')
            {
                //La posicion ya ha sido ocupada
                Acumulador = Acumulador + 1;
                if(Acumulador >= NumeroBloque)
                    return (Indice + 1 - NumeroBloque);
            }else
            {
                //La posicion ya ha sido ocupada
                Acumulador = 0;
            }
            Indice++;
        }
    }
    else if(strcasecmp(TipoAjuste,"WF")==0)
    {
        //PEOR AJUSTE
        Cap *Cabec;
        Cabec = malloc(sizeof(Cabecera));
        InicializarLista(Cabec);
        Indice = 0;
        Acumulador = 0;
        while(Indice < Bloques)
        {
            if(BitmaptBl[Indice]=='0')
                Acumulador++;
            else
            {
                if(Acumulador >= NumeroBloque)
                {
                    InsertarDato(Cabec,Indice-Acumulador,Acumulador);
                    Acumulador=0;
                }
                else
                    Acumulador =0;
            }
            Indice++;
        }

        if(Acumulador!=0)
        {
            if(Acumulador >= NumeroBloque)
                InsertarDato(Cabec,Indice-Acumulador,Acumulador);
        }
        //Por ser el Peor Ajuste se escoje el Tamaño mas grande que seria el primer nodo de la lista
        NumeroBloque = DevolverValor(Cabec,1);
        //Borrando la Lista
        BorrarNodos(Cabec);
        free(Cabec);
        return NumeroBloque;
    }
    return -1;   //Significa ERROR
}

//***************************** METODO PARA CALCULAR EL PRIMER BLOQUE LIBRE DEL SISTEMA ***********************************************************************************************************
//*************************************************************************************************************************************************************************************************
int DevolverPrimerBloqueLibre(struct SuperBloque *SuperBl, FILE *Disco)
{
    int Bloques,Indice,InicioBitmapBloques;

    Bloques = SuperBl->TotalBloques;
    InicioBitmapBloques = SuperBl->InicioBitmapBloques;
    char BitmaptBl[Bloques];
    fseek(Disco,InicioBitmapBloques,SEEK_SET);
    fread(BitmaptBl,Bloques,1,Disco);

    Indice = 0;

    while(Indice < Bloques)
    {
        if(BitmaptBl[Indice]=='0')
        {
            //La posicion ya ha sido ocupada
            return Indice;
        }
        Indice++;
    }
    return -1;   //Significa ERROR
}

//***************************** METODO PARA CALCULAR EL PRIMER INODO LIBRE DEL SISTEMA *********************************************************************************************************************
//**********************************************************************************************************************************************************************************************************
int DevolverPrimerInodoLibre(struct SuperBloque *SuperBl, FILE *Disco)
{
    int InicioBitmapInodos,TotalInodos,Indice;

    InicioBitmapInodos = SuperBl->InicioBitmapInodos;
    TotalInodos = SuperBl->TotalInodos;
    char BitmapInodos[TotalInodos];
    fseek(Disco,InicioBitmapInodos,SEEK_SET);
    fread(BitmapInodos,TotalInodos,1,Disco);

    //Primer Ajuste
    //Se elige el primer espacio donde quepa
    Indice = 0;

    while(Indice < TotalInodos)
    {
        if(BitmapInodos[Indice]=='0')
        {
            //La posicion esta libre
            return (Indice);
        }
        Indice++;
    }
    return -1;    //Significa error
}

//******************* METODO PARA CREA UN INODO Y UN ARCHIVO DENTRO DE UN BLOQUE DE APUNTADORES **********************************************************************************************
//********************************************************************************************************************************************************************************************
void CrearIndirectoCarpetaInodoArchvio(struct SuperBloque *SuperBl, struct BloqueApuntadores *Apuntadores,FILE *Disco,char *TipoAjuste,int Tamano,int Posicion,char Contenido[],int ByteInicialPart)
{
    int NumeroInodo = CalcularNumeroInodo(SuperBl,TipoAjuste,Disco);
    int NumeroBloque = CalcularNumeroBloque(Tamano,false,SuperBl,TipoAjuste,Disco);
    int NumeroBloquesXArchivo,Ind,IndieContenido,AcumuladorCont,ByteBloqueArchivos,ByteinodoNuevo;
    int Indice3;

    //Se Debe de crear un bloque de carpeta
    struct BloqueCarpetas NuevaCarp;

    Indice3 = 0;
    while(Indice3 < 4)
    {
        NuevaCarp.ApuntadorCarpeta[Indice3].Apuntador = -1;
        Indice3++;
    }
    NuevaCarp.ApuntadorCarpeta[Indice3].Apuntador = NumeroInodo+1;

    //Se actualiza el bloque de Apuntadores hacia el nuevo bloque de carpetas creado
    Apuntadores->Apuntadores[Posicion] = NumeroBloque+1;
    NumeroBloque++;

    //Se guarda el nuevo bloque de Carpetas
    Indice3 = Apuntadores->Apuntadores[Posicion] -1;
    Indice3 = SuperBl->InicioTablaBloques +(Indice3*(sizeof(struct BloqueCarpetas)));
    InsertarBloqueCarpetas(&NuevaCarp,Disco,Indice3);

    //Se crea el Nuevo Inodo del Archivo
    struct TablaInodos NuevoInodo;
    time_t Tiempo;
    Tiempo = time(0);
    Ind = 0;
    NuevoInodo.FechaCreacion = Tiempo;
    NuevoInodo.GrupoPropietario = 1; //Pendiente
    NuevoInodo.Permisos = 4;   //Pendiente
    NuevoInodo.TamanoArchivo = Tamano;
    NuevoInodo.Tipo[0] = '1';
    NuevoInodo.UltimaFechaLectura = Tiempo;
    NuevoInodo.UltimaFechaModific = Tiempo;
    NuevoInodo.UsuarioPropietario = 1; //Pendiente
    while(Ind < 15)
    {
        NuevoInodo.Apuntadores[Ind] = -1;
        Ind++;
    }

    //Calculando el numero de bloques que ocupara el archivo
    NumeroBloquesXArchivo = Tamano/64;
    if((NumeroBloquesXArchivo*64) < Tamano)
        NumeroBloquesXArchivo++;
    Ind =0;
    AcumuladorCont =63;
    IndieContenido = 0;
    Indice3 = 0;
    while(Indice3 < NumeroBloquesXArchivo)
    {
        //Se crean los bloques se guarda la informacion y se escriben en el disco
        struct Archivos BloqueArchivos;
        if(Indice3 == 12)
        {
            //Primer ApuntadorIndirecto
        }else if(Indice3 == 13)
        {
            //Apuntador Indirecto Doble
        }else if(Indice3 == 14)
        {
            //Apuntador Indirecto Triple
        }else
        {
            //Apuntadores Directos
            NuevoInodo.Apuntadores[Indice3] = NumeroBloque+1;
            //Copiando el Contenido al Bloque
            if(NumeroBloquesXArchivo == 1)
            {
                //Solo se necesita un bloque
                strcpy(BloqueArchivos.Contenido,Contenido);

            }
            else
            {
                Ind=0;
                while(IndieContenido < AcumuladorCont)
                {
                    if(IndieContenido < Tamano)
                        BloqueArchivos.Contenido[Ind] = Contenido[IndieContenido];
                    else
                        BloqueArchivos.Contenido[Ind] = '\0';

                    IndieContenido++;
                    Ind++;
                }
                AcumuladorCont = AcumuladorCont +63;
                BloqueArchivos.Contenido[Ind]='\0';

            }
            ByteBloqueArchivos = SuperBl->InicioTablaBloques + (NumeroBloque * sizeof(struct Archivos));
            InsertarBloqueArchivos(&BloqueArchivos,Disco,ByteBloqueArchivos);

            NumeroBloque = NumeroBloque+1;
        }
        Indice3++;
    }

    //Se Escribe en Disco el Inodo del Archivo
    ByteinodoNuevo = SuperBl->InicioTablaInodos + (NumeroInodo * sizeof(struct TablaInodos));
    InsertarTablaInodo(&NuevoInodo,Disco,ByteinodoNuevo);

    //Actulizando los bitmpat De Inodo y de Bloqes
    int TotalBlo,TotalIno;
    TotalBlo = SuperBl->TotalBloques;
    TotalIno = SuperBl->TotalInodos;
    char BitmapIno[TotalIno];
    char BitMapBlo[TotalBlo];

    fseek(Disco,SuperBl->InicioBitmapInodos,SEEK_SET);
    fread(BitmapIno,TotalIno,1,Disco);
    fseek(Disco,SuperBl->InicioBitmapBloques,SEEK_SET);
    fread(BitMapBlo,TotalBlo,1,Disco);
    Ind = 0;
    NumeroBloque = CalcularNumeroBloque(Tamano,true,SuperBl,TipoAjuste,Disco);

    while(Ind < NumeroBloquesXArchivo)
    {
         BitMapBlo[NumeroBloque] = '1';
        NumeroBloque++;
        Ind++;
    }

    BitmapIno[NumeroInodo] = '1';
    fseek(Disco,SuperBl->InicioBitmapInodos,SEEK_SET);
    fwrite(BitmapIno,TotalIno,1,Disco);
    fseek(Disco,SuperBl->InicioBitmapBloques,SEEK_SET);
    fwrite(BitMapBlo,TotalBlo,1,Disco);

    //Actualizando el SuperBloque
    SuperBl->BloquesLibres = SuperBl->BloquesLibres - NumeroBloquesXArchivo;
    SuperBl->InodosLibres = SuperBl->InodosLibres -1;
    SuperBl->PrimerBloqueLibre = DevolverPrimerBloqueLibre(SuperBl,Disco) +1;
    SuperBl->PrimerInodoLibre = DevolverPrimerInodoLibre(SuperBl,Disco) +1;

    struct SuperBloque NuevoSu;
    NuevoSu.BloquesLibres = SuperBl->BloquesLibres;
    NuevoSu.InicioBitmapBloques = SuperBl->InicioBitmapBloques;
    NuevoSu.InicioBitmapInodos = SuperBl->InicioBitmapInodos;
    NuevoSu.InicioTablaBloques = SuperBl->InicioTablaBloques;
    NuevoSu.InicioTablaInodos = SuperBl->InicioTablaInodos;
    NuevoSu.InodosLibres = SuperBl->InodosLibres;
    NuevoSu.PrimerBloqueLibre = SuperBl->PrimerBloqueLibre;
    NuevoSu.PrimerInodoLibre = SuperBl->PrimerInodoLibre;
    NuevoSu.SistemaArchivos = SuperBl->SistemaArchivos;
    NuevoSu.TamanoBloque = SuperBl->TamanoBloque;
    NuevoSu.TamanoInodo = SuperBl->TamanoInodo;
    NuevoSu.TotalBloques = SuperBl->TotalBloques;
    NuevoSu.TotalInodos = SuperBl->TotalInodos;
    NuevoSu.UltimaFechaDesmont = SuperBl->UltimaFechaDesmont;
    NuevoSu.UltimaFechaMontura = SuperBl->UltimaFechaMontura;
    NuevoSu.VecesMontado = SuperBl->VecesMontado;
    InsertarSuperBloque(&NuevoSu,Disco,ByteInicialPart);

    printf("Archivo Guardado Correctamente\n");
}

//******************* METODO PARA CREAR UN ARCHIVO A PARTIR DE UN INODO DADO ********************************************************************************************************
//******************************************************************************************************************************************************************************************
void CrearArchivo(int ByteInodo,int ByteInicialParticion,FILE *Disco, char *TipoAjuste, int Tamano, char *NombreArc, char Contenido[])
{
    struct TablaInodos Inodo;
    struct TablaInodos *InodoInicio;
    struct SuperBloque *SuperBl;
    struct BloqueCarpetas *Carpetas;

    int Indice,Indice2,ByteBloque,ByteInicialBloque,ByteBloqueApuntador,PosicionApuntador;
    time_t Tiempo;

    //Extrañendo el Inodo Padre
    InodoInicio = malloc(sizeof(struct TablaInodos));
    fseek(Disco,ByteInodo,SEEK_SET);
    fread(InodoInicio,sizeof(struct TablaInodos),1,Disco);

    //Extrañendo el SuperBloque
    SuperBl = malloc(sizeof(struct SuperBloque));
    fseek(Disco,ByteInicialParticion,SEEK_SET);
    fread(SuperBl,sizeof(struct SuperBloque),1,Disco);

    //Buscando el algun apuntador que este disponible
    Indice = 0;
    while(Indice < 15)
    {
        if(Indice == 12)
        {
            //primer Apuntador Indirecto
            if(InodoInicio->Apuntadores[Indice] != -1)
            {
                //Ya existe una bloque de Apuntadores
                ByteBloqueApuntador = InodoInicio->Apuntadores[Indice] -1;
                ByteBloqueApuntador = SuperBl->InicioTablaBloques + (ByteBloqueApuntador * (sizeof (struct BloqueApuntadores)));
                struct BloqueApuntadores *Apuntadores;
                Apuntadores = malloc(sizeof(struct BloqueApuntadores));
                fseek(Disco,ByteBloqueApuntador,SEEK_SET);
                fread(Apuntadores,sizeof(struct BloqueApuntadores),1,Disco);
                //Primero se verifica si existe espacio dentro de los bloque de carpeta contenido dentro
                // de los bloque se apuntadores
                PosicionApuntador = VerificarEspacioBloqueApuntadores(Apuntadores,2,SuperBl,Disco);
                if(PosicionApuntador != -1)
                {
                    //Si existe espacio dentro de los bloques de carpetas
                    //PosicionApuntador devuelve en numero del bloque de Carpeta donde se colocara el Inodo
                }
                else
                {
                    //NO existe espacio dentro de los blques de Capetas
                    //Se verfica la posicon dentro del bloque de apuntadores donde se crear un nuevo bloque de Capeta
                    PosicionApuntador = VerificarEspacioBloqueApuntadores(Apuntadores,1,NULL,NULL);
                    CrearIndirectoCarpetaInodoArchvio(SuperBl,Apuntadores,Disco,TipoAjuste,Tamano,PosicionApuntador,Contenido,ByteInicialParticion);

                }

            }else
            {
                //No existe un bloque de Apuntadores se debe de crear uno
            }

        }else if(Indice == 13)
        {
            //Apuntador Indirecto Doble
        }else if(Indice == 14)
        {
            //Apuntador Indirecto Triple
        }else
        {

            //Analizando si Existe espacio dentro del bloque de carpetas
            if( InodoInicio->Apuntadores[Indice] != -1 )
            {
                ByteBloque = InodoInicio->Apuntadores[Indice];
                ByteInicialBloque = SuperBl->InicioTablaBloques + ( (ByteBloque -1) * sizeof(struct BloqueCarpetas) );
                Carpetas = malloc(sizeof(struct BloqueCarpetas));
                fseek(Disco,ByteInicialBloque,SEEK_SET);
                fread(Carpetas,sizeof(struct BloqueCarpetas),1,Disco);

                Indice2 = 0;
                while(Indice2 < 4)
                {
                    if(Carpetas->ApuntadorCarpeta[Indice2].Apuntador == -1)
                    {
                        //Significa que aqui va el numero del inodo que representara al archivo
                        int NumeroInodo = CalcularNumeroInodo(SuperBl,TipoAjuste,Disco);
                        int NumeroBloque = CalcularNumeroBloque(Tamano,false,SuperBl,TipoAjuste,Disco);
                        int NumeroBloquesXArchivo,Ind,IndieContenido,AcumuladorCont,ByteBloqueArchivos,ByteinodoNuevo;
                        int Indice3;

                        //Se crea el Nuevo Inodo del Archivo
                        struct TablaInodos NuevoInodo;

                        Tiempo = time(0);
                        Ind = 0;
                        NuevoInodo.FechaCreacion = Tiempo;
                        NuevoInodo.GrupoPropietario = 1; //Pendiente
                        NuevoInodo.Permisos = 664;   //Pendiente
                        NuevoInodo.TamanoArchivo = Tamano;
                        NuevoInodo.Tipo[0] = '1';
                        NuevoInodo.UltimaFechaLectura = Tiempo;
                        NuevoInodo.UltimaFechaModific = Tiempo;
                        NuevoInodo.UsuarioPropietario = 1; //Pendiente
                        while(Ind < 15)
                        {
                            NuevoInodo.Apuntadores[Ind] = -1;
                            Ind++;
                        }

                        //Actualizando el Bloque Padre
                        Carpetas->ApuntadorCarpeta[Indice2].Apuntador = NumeroInodo+1;
                        strcpy(Carpetas->ApuntadorCarpeta[Indice2].Nombre,NombreArc);   //Pendiente


                        //Calculando el numero de bloques que ocupara el archivo
                        NumeroBloquesXArchivo = Tamano/64;
                        if((NumeroBloquesXArchivo*64) < Tamano)
                            NumeroBloquesXArchivo++;
                        Ind =0;
                        AcumuladorCont =63;
                        IndieContenido = 0;
                        Indice3 = 0;
                        while(Indice3 < NumeroBloquesXArchivo)
                        {
                            //Se crean los bloques se guarda la informacion y se escriben en el disco
                            struct Archivos BloqueArchivos;
                            if(Indice3 == 12)
                            {
                                //Primer ApuntadorIndirecto
                            }else if(Indice3 == 13)
                            {
                                //Apuntador Indirecto Doble
                            }else if(Indice3 == 14)
                            {
                                //Apuntador Indirecto Triple
                            }else
                            {
                                //Apuntadores Directos

                                NuevoInodo.Apuntadores[Indice3] = NumeroBloque+1;
                                //Copiando el Contenido al Bloque
                                if(NumeroBloquesXArchivo == 1)
                                {
                                    //Solo se necesita un bloque
                                    strcpy(BloqueArchivos.Contenido,Contenido);


                                }
                                else
                                {
                                    Ind=0;
                                    while(IndieContenido < AcumuladorCont)
                                    {
                                        if(IndieContenido < Tamano)
                                            BloqueArchivos.Contenido[Ind] = Contenido[IndieContenido];
                                        else
                                            BloqueArchivos.Contenido[Ind] = '\0';

                                        IndieContenido++;
                                        Ind++;
                                    }
                                    AcumuladorCont = AcumuladorCont +63;
                                    BloqueArchivos.Contenido[Ind]='\0';

                                }
                                ByteBloqueArchivos = SuperBl->InicioTablaBloques + (NumeroBloque * sizeof(struct Archivos));
                                InsertarBloqueArchivos(&BloqueArchivos,Disco,ByteBloqueArchivos);

                                NumeroBloque = NumeroBloque+1;
                            }
                            Indice3++;
                        }

                        //Se Escribe en Disco el Inodo del Archivo
                        ByteinodoNuevo = SuperBl->InicioTablaInodos + (NumeroInodo * sizeof(struct TablaInodos));
                        InsertarTablaInodo(&NuevoInodo,Disco,ByteinodoNuevo);

                        //Actalizando el Bloque de Capetas
                        Ind = 0;
                        struct BloqueCarpetas NuevaCarp;
                        while(Ind < 4)
                        {
                            NuevaCarp.ApuntadorCarpeta[Ind].Apuntador = Carpetas->ApuntadorCarpeta[Ind].Apuntador;
                            strcpy(NuevaCarp.ApuntadorCarpeta[Ind].Nombre,Carpetas->ApuntadorCarpeta[Ind].Nombre);
                            Ind++;
                        }
                        InsertarBloqueCarpetas(&NuevaCarp,Disco,ByteInicialBloque);

                        //Actulizando los bitmpat De Inodo y de Bloqes
                        int TotalBlo,TotalIno;
                        TotalBlo = SuperBl->TotalBloques;
                        TotalIno = SuperBl->TotalInodos;
                        char BitmapIno[TotalIno];
                        char BitMapBlo[TotalBlo];

                        fseek(Disco,SuperBl->InicioBitmapInodos,SEEK_SET);
                        fread(BitmapIno,TotalIno,1,Disco);
                        fseek(Disco,SuperBl->InicioBitmapBloques,SEEK_SET);
                        fread(BitMapBlo,TotalBlo,1,Disco);
                        Ind = 0;
                        NumeroBloque = CalcularNumeroBloque(Tamano,true,SuperBl,TipoAjuste,Disco);
                        while(Ind < NumeroBloquesXArchivo)
                        {
                            BitMapBlo[NumeroBloque] = '1';
                            NumeroBloque++;
                            Ind++;
                        }

                        BitmapIno[NumeroInodo] = '1';
                        fseek(Disco,SuperBl->InicioBitmapInodos,SEEK_SET);
                        fwrite(BitmapIno,TotalIno,1,Disco);
                        fseek(Disco,SuperBl->InicioBitmapBloques,SEEK_SET);
                        fwrite(BitMapBlo,TotalBlo,1,Disco);

                        //Actualizando el SuperBloque
                        SuperBl->BloquesLibres = SuperBl->BloquesLibres - NumeroBloquesXArchivo;
                        SuperBl->InodosLibres = SuperBl->InodosLibres -1;
                        SuperBl->PrimerBloqueLibre = DevolverPrimerBloqueLibre(SuperBl,Disco) +1;
                        SuperBl->PrimerInodoLibre = DevolverPrimerInodoLibre(SuperBl,Disco) +1;

                        struct SuperBloque NuevoSu;
                        NuevoSu.BloquesLibres = SuperBl->BloquesLibres;
                        NuevoSu.InicioBitmapBloques = SuperBl->InicioBitmapBloques;
                        NuevoSu.InicioBitmapInodos = SuperBl->InicioBitmapInodos;
                        NuevoSu.InicioTablaBloques = SuperBl->InicioTablaBloques;
                        NuevoSu.InicioTablaInodos = SuperBl->InicioTablaInodos;
                        NuevoSu.InodosLibres = SuperBl->InodosLibres;
                        NuevoSu.PrimerBloqueLibre = SuperBl->PrimerBloqueLibre;
                        NuevoSu.PrimerInodoLibre = SuperBl->PrimerInodoLibre;
                        NuevoSu.SistemaArchivos = SuperBl->SistemaArchivos;
                        NuevoSu.TamanoBloque = SuperBl->TamanoBloque;
                        NuevoSu.TamanoInodo = SuperBl->TamanoInodo;
                        NuevoSu.TotalBloques = SuperBl->TotalBloques;
                        NuevoSu.TotalInodos = SuperBl->TotalInodos;
                        NuevoSu.UltimaFechaDesmont = SuperBl->UltimaFechaDesmont;
                        NuevoSu.UltimaFechaMontura = SuperBl->UltimaFechaMontura;
                        NuevoSu.VecesMontado = SuperBl->VecesMontado;
                        InsertarSuperBloque(&NuevoSu,Disco,ByteInicialParticion);
                        free(SuperBl);
                        free(InodoInicio);
                        free(Carpetas);

                        printf("Archivo Guardado Correctamente\n");
                        return;


                    }
                    Indice2++;
                }
                free(Carpetas);
            }else
            {

                //El Bloque de Carpetas esta vacio se crea uno

                int NumeroBloque;  //Indica el numero de bloque inicio para todo el archivo
                int NumeroInodo;
                int BloquesXArchivo,Ind,AcumuladorCont,IndiceContenido,ByteBloqueArchivos,ByteinodoNuevo;


                Tiempo = time(0);
                NumeroBloque = CalcularNumeroBloque(Tamano,true,SuperBl,TipoAjuste,Disco);
                NumeroInodo = CalcularNumeroInodo(SuperBl,TipoAjuste,Disco);

                //Actualizo apuntadores del nodo padre
                InodoInicio->Apuntadores[Indice] = NumeroBloque+1;
                InodoInicio->UltimaFechaLectura = Tiempo;
                InodoInicio->UltimaFechaModific = Tiempo;

                //Copiando los valores al nuevo Inodo para guardarlos en el disco con las modificaiones
                struct TablaInodos ActualizacionInodo;
                Indice2 = 0;
                while(Indice2 < 15)
                {
                    ActualizacionInodo.Apuntadores[Indice2] = InodoInicio->Apuntadores[Indice2];
                    Indice2++;
                }

                ActualizacionInodo.FechaCreacion = InodoInicio->FechaCreacion;
                ActualizacionInodo.GrupoPropietario = InodoInicio->GrupoPropietario;
                ActualizacionInodo.Permisos = InodoInicio->Permisos;
                ActualizacionInodo.TamanoArchivo = InodoInicio->TamanoArchivo;
                ActualizacionInodo.Tipo[0] = InodoInicio->Tipo[0];
                ActualizacionInodo.UltimaFechaLectura = InodoInicio->UltimaFechaLectura;
                ActualizacionInodo.UltimaFechaModific = InodoInicio->UltimaFechaModific;
                ActualizacionInodo.UsuarioPropietario = InodoInicio->UsuarioPropietario;
                InsertarTablaInodo(&ActualizacionInodo,Disco,ByteInodo);

                //Se crea el nuevo Bloque de Carpetas o archivos
                struct BloqueCarpetas BloqueNuevoCarp;
                ByteInicialBloque = SuperBl->InicioTablaBloques + ( (NumeroBloque) * sizeof(struct BloqueCarpetas) );
                if(Indice != 0)
                {
                    //NO es el Primer bloque del Inodo que se crea
                    strcpy(BloqueNuevoCarp.ApuntadorCarpeta[0].Nombre,NombreArc);
                    BloqueNuevoCarp.ApuntadorCarpeta[0].Apuntador = NumeroInodo+1;   //Numero de Inodo al que Apuntara
                    BloqueNuevoCarp.ApuntadorCarpeta[1].Apuntador = -1;
                    BloqueNuevoCarp.ApuntadorCarpeta[2].Apuntador = -1;
                    BloqueNuevoCarp.ApuntadorCarpeta[3].Apuntador = -1;

                    InsertarBloqueCarpetas(&BloqueNuevoCarp,Disco,ByteInicialBloque);

                    //Se crea el Inodo del archivo
                    struct TablaInodos NuevoInodo;
                    Tiempo = time(0);
                    Indice2 = 0;
                    NuevoInodo.FechaCreacion = Tiempo;
                    NuevoInodo.GrupoPropietario = 1; //Pendiente
                    NuevoInodo.Permisos = 4; //Pendiente
                    NuevoInodo.TamanoArchivo = Tamano;
                    NuevoInodo.Tipo[0] = '1';
                    NuevoInodo.UltimaFechaLectura = Tiempo;
                    NuevoInodo.UltimaFechaModific = Tiempo;
                    NuevoInodo.UsuarioPropietario = 1; //Pendiente
                    while(Indice2 < 15)
                    {
                        NuevoInodo.Apuntadores[Indice2] = -1;
                        Indice2++;
                    }

                    //Creando los bloques que contendran la informacion
                    Indice2 = 0;
                    AcumuladorCont = 63;
                    IndiceContenido=0;
                    NumeroBloque++;  //Se Correo uno porque ya se creo el bloque de carpetas

                    BloquesXArchivo = Tamano/64;
                    if((BloquesXArchivo*64) < Tamano)
                        BloquesXArchivo++;

                    while(Indice2 < BloquesXArchivo)
                    {
                        struct Archivos BloqueArchivos;
                        if(Indice2 == 12)
                        {
                            //Primer ApuntadorIndirecto
                        }else if(Indice2 == 13)
                        {
                            //Apuntador Indirecto Doble
                        }else if(Indice2 == 14)
                        {
                            //Apuntador Indirecto Triple
                        }else
                        {
                            //Apuntadores Directos

                            NuevoInodo.Apuntadores[Indice2] = NumeroBloque+1;
                            //Copiando el Contenido al Bloque
                            if(BloquesXArchivo == 1)
                            {
                                //Solo se necesita un bloque
                                strcpy(BloqueArchivos.Contenido,Contenido);


                            }
                            else
                            {
                                Ind =0;
                                while(IndiceContenido < AcumuladorCont)
                                {
                                    if(IndiceContenido < Tamano)
                                        BloqueArchivos.Contenido[Ind] = Contenido[IndiceContenido];
                                    else
                                        BloqueArchivos.Contenido[Ind] = '\0';


                                    IndiceContenido++;
                                    Ind++;
                                }
                                AcumuladorCont = AcumuladorCont +63;
                                BloqueArchivos.Contenido[Ind]='\0';
                            }
                            ByteBloqueArchivos = SuperBl->InicioTablaBloques + (NumeroBloque * sizeof(struct Archivos));
                            InsertarBloqueArchivos(&BloqueArchivos,Disco,ByteBloqueArchivos);

                            NumeroBloque = NumeroBloque+1;
                        }


                        Indice2 = Indice2+1;
                    }
                    //Se Inserta el Inodo del Archivo
                    //Se Escribe en Disco el Inodo del Archivo
                    ByteinodoNuevo = SuperBl->InicioTablaInodos + (NumeroInodo * sizeof(struct TablaInodos));
                    InsertarTablaInodo(&NuevoInodo,Disco,ByteinodoNuevo);

                    //Actulizando los bitmpat De Inodo y de Bloqes
                    int TotalBlo,TotalIno;
                    TotalBlo = SuperBl->TotalBloques;
                    TotalIno = SuperBl->TotalInodos;
                    char BitmapIno[TotalIno];
                    char BitMapBlo[TotalBlo];

                    fseek(Disco,SuperBl->InicioBitmapInodos,SEEK_SET);
                    fread(BitmapIno,TotalIno,1,Disco);
                    fseek(Disco,SuperBl->InicioBitmapBloques,SEEK_SET);
                    fread(BitMapBlo,TotalBlo,1,Disco);
                    Ind = 0;
                    NumeroBloque = CalcularNumeroBloque(Tamano,true,SuperBl,TipoAjuste,Disco);
                    BloquesXArchivo++;   //Porque se Creo el Bloque de Carpetas
                    while(Ind < BloquesXArchivo)
                    {
                        BitMapBlo[NumeroBloque] = '1';
                        NumeroBloque++;
                        Ind++;
                    }

                    BitmapIno[NumeroInodo] = '1';
                    fseek(Disco,SuperBl->InicioBitmapInodos,SEEK_SET);
                    fwrite(BitmapIno,TotalIno,1,Disco);
                    fseek(Disco,SuperBl->InicioBitmapBloques,SEEK_SET);
                    fwrite(BitMapBlo,TotalBlo,1,Disco);

                    //Actualizando el SuperBloque
                    SuperBl->BloquesLibres = SuperBl->BloquesLibres - BloquesXArchivo;
                    SuperBl->InodosLibres = SuperBl->InodosLibres -1;
                    SuperBl->PrimerBloqueLibre = DevolverPrimerBloqueLibre(SuperBl,Disco) +1;
                    SuperBl->PrimerInodoLibre = DevolverPrimerInodoLibre(SuperBl,Disco) +1;

                    struct SuperBloque NuevoSu;
                    NuevoSu.BloquesLibres = SuperBl->BloquesLibres;
                    NuevoSu.InicioBitmapBloques = SuperBl->InicioBitmapBloques;
                    NuevoSu.InicioBitmapInodos = SuperBl->InicioBitmapInodos;
                    NuevoSu.InicioTablaBloques = SuperBl->InicioTablaBloques;
                    NuevoSu.InicioTablaInodos = SuperBl->InicioTablaInodos;
                    NuevoSu.InodosLibres = SuperBl->InodosLibres;
                    NuevoSu.PrimerBloqueLibre = SuperBl->PrimerBloqueLibre;
                    NuevoSu.PrimerInodoLibre = SuperBl->PrimerInodoLibre;
                    NuevoSu.SistemaArchivos = SuperBl->SistemaArchivos;
                    NuevoSu.TamanoBloque = SuperBl->TamanoBloque;
                    NuevoSu.TamanoInodo = SuperBl->TamanoInodo;
                    NuevoSu.TotalBloques = SuperBl->TotalBloques;
                    NuevoSu.TotalInodos = SuperBl->TotalInodos;
                    NuevoSu.UltimaFechaDesmont = SuperBl->UltimaFechaDesmont;
                    NuevoSu.UltimaFechaMontura = SuperBl->UltimaFechaMontura;
                    NuevoSu.VecesMontado = SuperBl->VecesMontado;
                    InsertarSuperBloque(&NuevoSu,Disco,ByteInicialParticion);
                    free(SuperBl);
                    free(InodoInicio);

                    printf("Archivo Guardado Correctamente\n");
                    return;


                }


            }
        }

        Indice++;
    }
    //NO HAY ESPACION EN EL INODO
    free(SuperBl);
    free(InodoInicio);

}

//*************************** METODO DEVOLVER EL NOMBRE DEL ARCHIVO A GUARDAR ************************************************************************************
//*********************************************************************************************************************************************************************************
char * DevolverNombreArchivo(char *Direccion)
{
    char Palabra[80];
    char *PalabraEnvia;
    int i;
    int TamanoNom;
    int Indice;
    Direccion++;
    while(sscanf(Direccion,"%150[^/]%n",Palabra,&i)==1)
    {
        //Se Verifica que lo que spliteo sea una carpeta y no un archivo


        Indice=0;
        TamanoNom = strlen(Palabra);

        while((Indice < TamanoNom))
        {
            if(Palabra[Indice]=='.')
            {//Es un ARCHIVO
                PalabraEnvia = malloc(strlen(Palabra));
                strcpy(PalabraEnvia,Palabra);
                return PalabraEnvia;

            }

            Indice++;
        }

        Direccion+=i;
        if(*Direccion!= '/')
            break;

        while(*Direccion== '/')
                ++Direccion;


    }
    return NULL;
}

//****************************************** METODO PARA DEVOLVER EL EBER DE UNA PARTICION LOGICA *******************************************************************************************
//********************************************************************************************************************************************************************************************************
struct EBR * DevolverEBR(FILE *Disco,int ByteInicial, char *Nombre)
{
    int ByteLogica;

    bool Estado;
    struct EBR *EBRAuxiliar;
    EBRAuxiliar = malloc(sizeof(struct EBR));
    fseek(Disco,ByteInicial,SEEK_SET);
    fread(EBRAuxiliar,sizeof(struct EBR),1,Disco);

    //Verifica si el EBR solicitado es el primero
    if(strcmp(EBRAuxiliar->Nombre,Nombre)==0)
    {
        //Es el EBR solicitado
        return EBRAuxiliar;
    }

    Estado = false;
    while((EBRAuxiliar->ProximoEBR!=(-1))&& Estado==false)
    {
        struct EBR *EBRSiguiente;

        ByteLogica = EBRAuxiliar->ProximoEBR;
        fseek(Disco,ByteLogica,SEEK_SET);

        EBRSiguiente = malloc(sizeof(struct EBR));
        fread(EBRSiguiente,sizeof(struct EBR),1,Disco);
        if(strcmp(EBRSiguiente->Nombre,Nombre)==0)
        {
            //Es el Estado Solicitado
            free(EBRAuxiliar);
            return EBRSiguiente;

        }else
        {
            //Corriendo los apuntadores
            fseek(Disco,EBRAuxiliar->ProximoEBR,SEEK_SET);
            free(EBRAuxiliar);
            EBRAuxiliar = malloc(sizeof(struct EBR));
            fread(EBRAuxiliar,sizeof(struct EBR),1,Disco);

        }
        free(EBRSiguiente);
    }

    free(EBRAuxiliar);
    return NULL;   //Significa que nos se encontro la particion solicitada
}

//**************************** METODO PARA DEVOLVER LA DIRECCION DE LA CARPETA O ARCHIVO A CREARSE ****************************************************************************************************************+
//***************************************************************************************************************************************************************************************************************
char *DevolverDireccionDeCarpeta(char *Direccion,int NumeroCarpetas)
{
    char *DireccionEnviar;
    char Palabra[80];
    int i,Indice;
    Direccion++;

    DireccionEnviar = malloc(strlen(Direccion));
    strcpy(DireccionEnviar,"");
    Indice = 1;
    while(sscanf(Direccion,"%150[^/]%n",Palabra,&i)==1)
    {


        if(Indice <= (NumeroCarpetas-1))
        {
            char *Pal;
            Pal = malloc(strlen(Palabra));
            strcpy(Pal,Palabra);
            strcat(DireccionEnviar,"/");
            strcat(DireccionEnviar,Pal);
            free(Pal);
        }

        Indice++;
        Direccion+=i;
        if(*Direccion!= '/')
            break;

        while(*Direccion== '/')
                ++Direccion;


    }

    return DireccionEnviar;
}


//***************************** METODO PARA OBTENER EL NUMERO DE CARPETAS QUE TRAE LA DIRECCION **********************************************************************************************
//***********************************************************************************************************************************************************************************************
int NumeroCarpetas(char *Direccion)
{
    char Palabra[80];

    int i;
    int Indice =-1;

    if(*Direccion =='/')
    {
        Indice=1;
        Direccion++;
        while(sscanf(Direccion,"%150[^/]%n",Palabra,&i)==1)
        {

            Direccion+=i;
            if(*Direccion!= '/')
                break;

            while(*Direccion== '/')
                ++Direccion;

            Indice++;

        }
    }

    return Indice;
}

//***************************** METODO PARA PASAR PARAMETROS PARA LA CREACION DE UN ARCHIVO EN UNA PARTICON LOGGICA *************************************************************************************************
//***************************************************************************************************************************************************************************************************************************

void PasarParametrosCreacionArchivo(struct Particion *Partic, struct EBR *EbrRecibido,char *Path,char *Tamano, FILE *DISCO, int TipParticion, char Content[])
{
    int ByteInodoPadre;
    char *TipoAjus;
    char *NombreRec;
    char *DireccionArc;
    int Tam,TotalCarpetas;

    //Obtiene el numero de carpetas que trae la direccion
    TotalCarpetas = NumeroCarpetas(Path);

    if(TotalCarpetas==-1)
    {
        printf(">>>>ERROR: la direccion para crear carpetas NO es válida<<<<\n");
        return;
    }



    if(TipParticion==1)
    {
        if(TotalCarpetas==1)
        {
            ByteInodoPadre = BuscarCarpetaContenedora(Partic,NULL,"/",DISCO,1);
            TipoAjus = malloc(strlen(Partic->TipoAjuste));
            strcpy(TipoAjus,Partic->TipoAjuste);

        }else
        {
            DireccionArc = DevolverDireccionDeCarpeta(Path,TotalCarpetas);
            ByteInodoPadre = BuscarCarpetaContenedora(Partic,NULL,DireccionArc,DISCO,1);
            TipoAjus = malloc(strlen(Partic->TipoAjuste));
            strcpy(TipoAjus,Partic->TipoAjuste);
        }
    }
    else if(TipParticion==2)
    {
        if(TotalCarpetas==1)
        {
            ByteInodoPadre = BuscarCarpetaContenedora(NULL,EbrRecibido,"/",DISCO,2);
            TipoAjus = malloc(strlen(EbrRecibido->Fitt));
            strcpy(TipoAjus,EbrRecibido->Fitt);
        }else
        {
            DireccionArc = DevolverDireccionDeCarpeta(Path,TotalCarpetas);
            ByteInodoPadre = BuscarCarpetaContenedora(NULL,EbrRecibido,DireccionArc,DISCO,2);
            TipoAjus = malloc(strlen(EbrRecibido->Fitt));
            strcpy(TipoAjus,EbrRecibido->Fitt);
        }
    }

    //Pasando el Tamano del archivo a una Int
    Tam =  atoi(Tamano);
    NombreRec = DevolverNombreArchivo(Path);
    //No se ingreso ningun nombre
    if(NombreRec==NULL)
    {
        free(TipoAjus);
        printf(">>>>ERROR: no se ingreseo un ningun nombre para el archivo<<<<\n");
        return;

    }
    if(strlen(NombreRec) > 12)
    {
        printf(">>>>ERROR: nombre de archivo demasiado grande, modifiquelo<<<<\n");
        free(TipoAjus);
        free(NombreRec);
        return;
    }

    if(ByteInodoPadre ==-1)
    {
        //La Direccion NO EXISTE
    }
    else
    {
    //la Direccion SI EXISTE
        if(TipParticion ==1)
        {
            CrearArchivo(ByteInodoPadre,Partic->ByteInicial,DISCO,TipoAjus,Tam,NombreRec,Content);
            free(TipoAjus);
            free(NombreRec);
            if(TotalCarpetas!=1)
                free(DireccionArc);
        }
        else if(TipParticion ==2)
        {
            CrearArchivo(ByteInodoPadre,EbrRecibido->ByteInicial,DISCO,TipoAjus,Tam,NombreRec,Content);
            free(TipoAjus);
            free(NombreRec);
            if(TotalCarpetas!=1)
                free(DireccionArc);
        }
    }

}

//**************** METODO PARA CREAR UN ARCHIVO DENTRO DE UNA PARITION CON DIRECCION EXISTENTE*******************************************************************************************************
//*********************************************************************************************************************************************************************************************
void CrearArchivoConDireccion(char *Id, char *Path, char *CrearCarpeta, char *Tamano, char *DirecCont, char Content[])
{

    //Buscando la particion Solicitada por el id
    struct MBR *EstrucMBR;

    char DiscoDireccion[125];
    char *NombreParticion;

    NombreParticion = malloc(100*sizeof(char));
    strcpy(NombreParticion,Devolverparticion(Id));
    strcpy(DiscoDireccion,DevolverPath(Id));

    if((strcasecmp(DiscoDireccion,"a"))!=0)
    {
        //El id ya se ha montado
        FILE *DiscoLeer;
        DiscoLeer = fopen(DiscoDireccion,"rb+");
        if(DiscoLeer==NULL)
        {
            printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
            return;
        }

        EstrucMBR = malloc(4*sizeof(struct MBR)+1);
        fseek(DiscoLeer,0,SEEK_SET);
        fread(EstrucMBR,sizeof(struct MBR),1,DiscoLeer);


        //Buscando la Particion a grafica

        if((strcmp(EstrucMBR->Particion1.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion1.Tipo,"p")==0) )
        {
            PasarParametrosCreacionArchivo(&EstrucMBR->Particion1,NULL,Path,Tamano,DiscoLeer,1,Content);


        }else if((strcmp(EstrucMBR->Particion2.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion2.Tipo,"p")==0) )
        {
            PasarParametrosCreacionArchivo(&EstrucMBR->Particion2,NULL,Path,Tamano,DiscoLeer,1,Content);

        }else if( (strcmp(EstrucMBR->Particion3.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion3.Tipo,"p")==0) )
        {

            PasarParametrosCreacionArchivo(&EstrucMBR->Particion3,NULL,Path,Tamano,DiscoLeer,1,Content);


        }else if( (strcmp(EstrucMBR->Particion4.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion4.Tipo,"p")==0) )
        {
            PasarParametrosCreacionArchivo(&EstrucMBR->Particion4,NULL,Path,Tamano,DiscoLeer,1,Content);

        }else if(strcasecmp(EstrucMBR->Particion1.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion1.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            else
            {
                //Se Crea el Disco
                PasarParametrosCreacionArchivo(NULL,EbrRecibido,Path,Tamano,DiscoLeer,2,Content);
                free(EbrRecibido);

            }



        }else if(strcasecmp(EstrucMBR->Particion2.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion2.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            else
            {
                //Se escribe en el Disco
                PasarParametrosCreacionArchivo(NULL,EbrRecibido,Path,Tamano,DiscoLeer,2,Content);
                free(EbrRecibido);
            }


        }else if(strcasecmp(EstrucMBR->Particion3.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion3.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            else
            {
                //Se escribe en el Disco
                PasarParametrosCreacionArchivo(NULL,EbrRecibido,Path,Tamano,DiscoLeer,2,Content);
                free(EbrRecibido);
            }




        }else if(strcasecmp(EstrucMBR->Particion4.Tipo,"e")==0)
        {
             struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion4.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            else
            {
                //Se escribe en el Disco
                PasarParametrosCreacionArchivo(NULL,EbrRecibido,Path,Tamano,DiscoLeer,2,Content);
                free(EbrRecibido);
            }



        }else
            printf(">>>>ERROR: no se pudo crear la grafica de la particion indicada, no se encontro dentro del disco <<<<\n");

        fclose(DiscoLeer);
        free(EstrucMBR);


    }else
        printf(">>>>ERROR: imposible graficar Disco no encontrado<<<<\n");

  }

//************************* METODO PARA CREAR EL REPORTE DE BLOQUES o INODOS OCUPADOS *************************************************************************************************************************
//****************************************************************************************************************************************************************************************************
void CrearReporteBloquesInodo(char *Direccion, char *IdRec,int TipoRep)
{
    //Si TipoRep es 1 ---->> Grafica de Bloques,   TipoRep es 2 ----->> Grafica Inodos
    struct MBR *EstrucMBR;

    char DiscoDireccion[125];
    char *NombreParticion;
    NombreParticion = malloc(100*sizeof(char));
    strcpy(NombreParticion,Devolverparticion(IdRec));
    strcpy(DiscoDireccion,DevolverPath(IdRec));

    if((strcasecmp(DiscoDireccion,"a"))!=0)
    {
        //El id ya se ha montado
        FILE *DiscoLeer;
        DiscoLeer = fopen(DiscoDireccion,"rb+");
        if(DiscoLeer==NULL)
        {
            printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
            return;
        }

        EstrucMBR = malloc(4*sizeof(struct MBR)+1);
        fseek(DiscoLeer,0,SEEK_SET);
        fread(EstrucMBR,sizeof(struct MBR),1,DiscoLeer);


        //Buscando la Particion a grafica

        if((strcmp(EstrucMBR->Particion1.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion1.Tipo,"p")==0) )
        {
            if(TipoRep==1)
            {
                //Grafica de Bloques
                GraficarBloquesOcupados(&EstrucMBR->Particion1,NULL,1,Direccion,DiscoLeer);
            }else if(TipoRep==2)
            {
                //Grafica de Inodos
                GraficarInodosOcupados(&EstrucMBR->Particion1,NULL,1,Direccion,DiscoLeer);
            }

        }else if((strcmp(EstrucMBR->Particion2.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion2.Tipo,"p")==0) )
        {
            if(TipoRep==1)
            {
                //Grafica de Bloques
                GraficarBloquesOcupados(&EstrucMBR->Particion2,NULL,1,Direccion,DiscoLeer);
            }else if(TipoRep==2)
            {
                //Grafica de Inodos
                GraficarInodosOcupados(&EstrucMBR->Particion2,NULL,1,Direccion,DiscoLeer);
            }


        }else if( (strcmp(EstrucMBR->Particion3.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion3.Tipo,"p")==0) )
        {
            if(TipoRep==1)
            {
                //Grafica de Bloques
                GraficarBloquesOcupados(&EstrucMBR->Particion3,NULL,1,Direccion,DiscoLeer);
            }else if(TipoRep==2)
            {
                //Grafica de Inodos
                GraficarInodosOcupados(&EstrucMBR->Particion3,NULL,1,Direccion,DiscoLeer);
            }


        }else if( (strcmp(EstrucMBR->Particion4.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion4.Tipo,"p")==0) )
        {
            if(TipoRep==1)
            {
                //Grafica de Bloques
                GraficarBloquesOcupados(&EstrucMBR->Particion4,NULL,1,Direccion,DiscoLeer);
            }else if(TipoRep==2)
            {
                //Grafica de Inodos
                GraficarInodosOcupados(&EstrucMBR->Particion4,NULL,1,Direccion,DiscoLeer);
            }

        }else if(strcasecmp(EstrucMBR->Particion1.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica

            //Se Busca la particion Logica
            if(TipoRep==1)
            {   //Grafica de Bloques
                if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion1.ByteInicial,NombreParticion,7,Direccion))
                {
                    printf("Grafica de Bloques, Particion Logica\n");
                }else
                {
                    printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
                }
            }else if(TipoRep==2)
            {   //Grafica de Inodos
                if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion1.ByteInicial,NombreParticion,8,Direccion))
                {
                    printf("Grafica de Inodos, Particion Logica\n");
                }else
                {
                    printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
                }
            }



        }else if(strcasecmp(EstrucMBR->Particion2.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            if(TipoRep==1)
            {   //Grafica de Bloques
                if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion2.ByteInicial,NombreParticion,7,Direccion))
                {
                    printf("Grafica de Bloques, Particion Logica\n");
                }else
                {
                    printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
                }
            }else if(TipoRep==2)
            {
                //Grafica de Inodos
                if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion2.ByteInicial,NombreParticion,8,Direccion))
                {
                    printf("Grafica de Inodos, Partcion Logica\n");
                }else
                {
                    printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
                }
            }


        }else if(strcasecmp(EstrucMBR->Particion3.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            if(TipoRep==1)
            {   //Grafica de Bloques
                if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion3.ByteInicial,NombreParticion,7,Direccion))
                {
                    printf("Grafica de Bloques, Particion Logica\n");
                }else
                {
                    printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
                }
            }else if(TipoRep==2)
            {   //Grafica de Inodos
                if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion3.ByteInicial,NombreParticion,8,Direccion))
                {
                    printf("Grafica de Inodos, Particion Logica\n");
                }else
                {
                    printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
                }
            }


        }else if(strcasecmp(EstrucMBR->Particion4.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            if(TipoRep==1)
            {   //Grafica de Bloques
                if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion4.ByteInicial,NombreParticion,7,Direccion))
                {
                    printf("Grafica de Bloques, Particion Logica\n");
                }else
                {
                    printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
                }
            }else if(TipoRep==2)
            {   //Grafica de Inodos
                if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion4.ByteInicial,NombreParticion,8,Direccion))
                {
                    printf("Grafica de Inodos, Particion Logica\n");
                }else
                {
                    printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
                }

            }


        }else
            printf(">>>>ERROR: no se pudo crear la grafica de la particion indicada, no se encontro dentro del disco <<<<\n");

        fclose(DiscoLeer);
        free(EstrucMBR);


    }else
        printf(">>>>ERROR: imposible graficar Disco no encontrado<<<<\n");

}
//**************************** METODO PARA DEVOLVER EL NOMBRE DE LA CARPETA A CREARSE ****************************************************************************************************************************
//**************************************************************************************************************************************************************************************************************************
char *DevolverNombreCarpeta(char *Direccion)
{
    char Palabra[80];
    char *PalabraEnvia;
    int i;
    int Indice =-1;

    if(*Direccion =='/')
    {
        Indice=1;
        Direccion++;
        while(sscanf(Direccion,"%150[^/]%n",Palabra,&i)==1)
        {

            Direccion+=i;
            if(*Direccion!= '/')
                break;

            while(*Direccion== '/')
                ++Direccion;

            Indice++;

        }
    }
    PalabraEnvia = malloc(strlen(Palabra));
    strcpy(PalabraEnvia,Palabra);
    return PalabraEnvia;

}


//*********************** METODO PARA CREAR UNA CARPETA DENTRO DEL SISTEMA DE ARCHIVOS *************************************************************************************************************************
//***********************************************************************************************************************************************************************************************************************
void CrearCarpeta(int ByteInodo,int ByteInicialParticion,FILE *Disco, char *TipoAjuste, char *NombreCarp)
{
    struct TablaInodos Inodo;
    struct TablaInodos *InodoInicio;
    struct SuperBloque *SuperBl;
    struct BloqueCarpetas *Carpetas;

    int Indice,Indice2,ByteBloque,ByteInicialBloque;
    time_t Tiempo;

    //Extrañendo el Inodo Padre
    InodoInicio = malloc(sizeof(struct TablaInodos));
    fseek(Disco,ByteInodo,SEEK_SET);
    fread(InodoInicio,sizeof(struct TablaInodos),1,Disco);

    //Extrañendo el SuperBloque
    SuperBl = malloc(sizeof(struct SuperBloque));
    fseek(Disco,ByteInicialParticion,SEEK_SET);
    fread(SuperBl,sizeof(struct SuperBloque),1,Disco);

    //Buscando el algun apuntador que este disponible
    Indice = 0;
    while(Indice < 15)
    {
        if(Indice == 12)
        {
            //primer Apuntador Indirecto

        }else if(Indice == 13)
        {
            //Apuntador Indirecto Doble
        }else if(Indice == 14)
        {
            //Apuntador Indirecto Triple
        }else
        {

            //Analizando si Existe espacio dentro del bloque de Apuntadores del Inodo padre
            if( InodoInicio->Apuntadores[Indice] != -1 )
            {
                ByteBloque = InodoInicio->Apuntadores[Indice];   //Numero de Bloque de Carpetas
                ByteInicialBloque = SuperBl->InicioTablaBloques + ( (ByteBloque -1) * sizeof(struct BloqueCarpetas) );
                Carpetas = malloc(sizeof(struct BloqueCarpetas));
                fseek(Disco,ByteInicialBloque,SEEK_SET);
                fread(Carpetas,sizeof(struct BloqueCarpetas),1,Disco);

                Indice2 = 0;
                while(Indice2 < 4)
                {
                    if(Carpetas->ApuntadorCarpeta[Indice2].Apuntador == -1)
                    {
                        //El bloque de carpetas aun tiene espacio por lo cual se escribe en el

                        //Calculando el Numero de bloque e Inodo a partir del cual se empezara a escribir la carpeta
                        int NumeroInodo = CalcularNumeroInodo(SuperBl,TipoAjuste,Disco);
                        int NumeroBloque = CalcularNumeroBloque(1,false,SuperBl,TipoAjuste,Disco);
                        int Ind,ByteBloqueArchivos,ByteinodoNuevo,NumeroInodoPad;

                        //Se crea el Nuevo Inodo de la carpeta
                        struct TablaInodos NuevoInodo;

                        Tiempo = time(0);
                        Ind = 0;
                        NuevoInodo.FechaCreacion = Tiempo;
                        NuevoInodo.GrupoPropietario = 1; //Pendiente
                        NuevoInodo.Permisos = 664;   //Pendiente
                        NuevoInodo.TamanoArchivo = 0;
                        NuevoInodo.Tipo[0] = '0';
                        NuevoInodo.UltimaFechaLectura = Tiempo;
                        NuevoInodo.UltimaFechaModific = Tiempo;
                        NuevoInodo.UsuarioPropietario = 1; //Pendiente
                        while(Ind < 15)
                        {
                            NuevoInodo.Apuntadores[Ind] = -1;
                            Ind++;
                        }

                        //Actualizando el Bloque de Carpetas Padre
                        Carpetas->ApuntadorCarpeta[Indice2].Apuntador = NumeroInodo+1;
                        strcpy(Carpetas->ApuntadorCarpeta[Indice2].Nombre,NombreCarp);   //Pendiente


                        //Por ser una Carpeta solo ocupara un bloque
                        //Al primer apuntador de la Carpeta se le coloca el bloque de la carpeta Nueva
                        NuevoInodo.Apuntadores[0] = NumeroBloque+1;

                        //Extrayendo el bloque de carpetas de apuntador uno del inodo padre para obtener el
                        //Numero de inodo del padre
                        struct BloqueCarpetas *CarpetasAux;
                        NumeroInodoPad = InodoInicio->Apuntadores[0];
                        NumeroInodoPad = SuperBl->InicioTablaBloques + ((NumeroInodoPad -1) * sizeof(struct BloqueCarpetas) );
                        fseek(Disco,NumeroInodoPad,SEEK_SET);
                        CarpetasAux = malloc(sizeof(struct BloqueCarpetas));
                        fread(CarpetasAux,sizeof(struct BloqueCarpetas),1,Disco);


                        //Se crea el Bloque de Carpetas y se guarda en el disco
                        struct BloqueCarpetas BloqCarp;
                        BloqCarp.ApuntadorCarpeta[0].Apuntador = NumeroInodo+1;
                        strcpy(BloqCarp.ApuntadorCarpeta[0].Nombre,NombreCarp);
                        BloqCarp.ApuntadorCarpeta[1].Apuntador = CarpetasAux->ApuntadorCarpeta[0].Apuntador;    //Apuntando a la carpeta Padre
                        strcpy(BloqCarp.ApuntadorCarpeta[1].Nombre,CarpetasAux->ApuntadorCarpeta[0].Nombre);
                        BloqCarp.ApuntadorCarpeta[2].Apuntador = -1;
                        BloqCarp.ApuntadorCarpeta[3].Apuntador = -1;
                        ByteBloqueArchivos = SuperBl->InicioTablaBloques + (NumeroBloque * sizeof(struct BloqueCarpetas));
                        InsertarBloqueCarpetas(&BloqCarp,Disco,ByteBloqueArchivos);
                        free(CarpetasAux);


                        //Se Escribe en Disco el Inodo de la carpeta
                        ByteinodoNuevo = SuperBl->InicioTablaInodos + (NumeroInodo * sizeof(struct TablaInodos));
                        InsertarTablaInodo(&NuevoInodo,Disco,ByteinodoNuevo);

                        //Actalizando el Bloque de Capetas Padre
                        Ind = 0;
                        struct BloqueCarpetas NuevaCarp;
                        while(Ind < 4)
                        {
                            NuevaCarp.ApuntadorCarpeta[Ind].Apuntador = Carpetas->ApuntadorCarpeta[Ind].Apuntador;
                            strcpy(NuevaCarp.ApuntadorCarpeta[Ind].Nombre,Carpetas->ApuntadorCarpeta[Ind].Nombre);
                            Ind++;
                        }
                        InsertarBloqueCarpetas(&NuevaCarp,Disco,ByteInicialBloque);

                        //Actulizando los bitmpat De Inodo y de Bloqes
                        int TotalBlo,TotalIno;
                        TotalBlo = SuperBl->TotalBloques;
                        TotalIno = SuperBl->TotalInodos;
                        char BitmapIno[TotalIno];
                        char BitMapBlo[TotalBlo];

                        fseek(Disco,SuperBl->InicioBitmapInodos,SEEK_SET);
                        fread(BitmapIno,TotalIno,1,Disco);
                        fseek(Disco,SuperBl->InicioBitmapBloques,SEEK_SET);
                        fread(BitMapBlo,TotalBlo,1,Disco);

                        NumeroBloque = CalcularNumeroBloque(1,false,SuperBl,TipoAjuste,Disco);
                        BitMapBlo[NumeroBloque] = '1';
                        BitmapIno[NumeroInodo] = '1';
                        fseek(Disco,SuperBl->InicioBitmapInodos,SEEK_SET);
                        fwrite(BitmapIno,TotalIno,1,Disco);
                        fseek(Disco,SuperBl->InicioBitmapBloques,SEEK_SET);
                        fwrite(BitMapBlo,TotalBlo,1,Disco);

                        //Actualizando el SuperBloque
                        SuperBl->BloquesLibres = SuperBl->BloquesLibres - 1;
                        SuperBl->InodosLibres = SuperBl->InodosLibres -1;
                        SuperBl->PrimerBloqueLibre = DevolverPrimerBloqueLibre(SuperBl,Disco) +1;
                        SuperBl->PrimerInodoLibre = DevolverPrimerInodoLibre(SuperBl,Disco) +1;

                        struct SuperBloque NuevoSu;
                        NuevoSu.BloquesLibres = SuperBl->BloquesLibres;
                        NuevoSu.InicioBitmapBloques = SuperBl->InicioBitmapBloques;
                        NuevoSu.InicioBitmapInodos = SuperBl->InicioBitmapInodos;
                        NuevoSu.InicioTablaBloques = SuperBl->InicioTablaBloques;
                        NuevoSu.InicioTablaInodos = SuperBl->InicioTablaInodos;
                        NuevoSu.InodosLibres = SuperBl->InodosLibres;
                        NuevoSu.PrimerBloqueLibre = SuperBl->PrimerBloqueLibre;
                        NuevoSu.PrimerInodoLibre = SuperBl->PrimerInodoLibre;
                        NuevoSu.SistemaArchivos = SuperBl->SistemaArchivos;
                        NuevoSu.TamanoBloque = SuperBl->TamanoBloque;
                        NuevoSu.TamanoInodo = SuperBl->TamanoInodo;
                        NuevoSu.TotalBloques = SuperBl->TotalBloques;
                        NuevoSu.TotalInodos = SuperBl->TotalInodos;
                        NuevoSu.UltimaFechaDesmont = SuperBl->UltimaFechaDesmont;
                        NuevoSu.UltimaFechaMontura = SuperBl->UltimaFechaMontura;
                        NuevoSu.VecesMontado = SuperBl->VecesMontado;
                        InsertarSuperBloque(&NuevoSu,Disco,ByteInicialParticion);
                        free(SuperBl);
                        free(InodoInicio);
                        free(Carpetas);

                        printf("Carpeta Guardada Correctamente\n");
                        return;


                    }
                    Indice2++;
                }
                free(Carpetas);
            }else
            {

                //El Bloque de Carpetas esta vacio se crea uno
                //Se Crea un Nuevo Bloque de Carpetas Padre

                int NumeroBloque;  //Indica el numero de bloque inicio para todo el archivo
                int NumeroInodo;
                int Ind,ByteBloqueArchivos,ByteinodoNuevo,NumeroInodoPad;


                Tiempo = time(0);
                NumeroBloque = CalcularNumeroBloque(1,true,SuperBl,TipoAjuste,Disco);
                NumeroInodo = CalcularNumeroInodo(SuperBl,TipoAjuste,Disco);

                //Actualizo apuntadores del nodo padre
                InodoInicio->Apuntadores[Indice] = NumeroBloque+1;
                InodoInicio->UltimaFechaLectura = Tiempo;
                InodoInicio->UltimaFechaModific = Tiempo;

                //Copiando los valores al nuevo Inodo Padre para guardarlos en el disco con las modificaiones
                struct TablaInodos ActualizacionInodo;
                Indice2 = 0;
                while(Indice2 < 15)
                {
                    ActualizacionInodo.Apuntadores[Indice2] = InodoInicio->Apuntadores[Indice2];
                    Indice2++;
                }

                ActualizacionInodo.FechaCreacion = InodoInicio->FechaCreacion;
                ActualizacionInodo.GrupoPropietario = InodoInicio->GrupoPropietario;
                ActualizacionInodo.Permisos = InodoInicio->Permisos;
                ActualizacionInodo.TamanoArchivo = InodoInicio->TamanoArchivo;
                ActualizacionInodo.Tipo[0] = InodoInicio->Tipo[0];
                ActualizacionInodo.UltimaFechaLectura = InodoInicio->UltimaFechaLectura;
                ActualizacionInodo.UltimaFechaModific = InodoInicio->UltimaFechaModific;
                ActualizacionInodo.UsuarioPropietario = InodoInicio->UsuarioPropietario;
                InsertarTablaInodo(&ActualizacionInodo,Disco,ByteInodo);

                //Se crea el nuevo Bloque de Carpetas o archivos Padre
                struct BloqueCarpetas BloqueNuevoCarp;
                ByteInicialBloque = SuperBl->InicioTablaBloques + ( (NumeroBloque) * sizeof(struct BloqueCarpetas) );
                if(Indice != 0)
                {
                    //NO es el Primer bloque del Inodo Padre que se crea

                    //ElNuevo Bloque de Carpetas para el Inodo Padre
                    strcpy(BloqueNuevoCarp.ApuntadorCarpeta[0].Nombre,NombreCarp);
                    BloqueNuevoCarp.ApuntadorCarpeta[0].Apuntador = NumeroInodo+1;   //Numero de Inodo al que Apuntara
                    BloqueNuevoCarp.ApuntadorCarpeta[1].Apuntador = -1;
                    BloqueNuevoCarp.ApuntadorCarpeta[2].Apuntador = -1;
                    BloqueNuevoCarp.ApuntadorCarpeta[3].Apuntador = -1;

                    InsertarBloqueCarpetas(&BloqueNuevoCarp,Disco,ByteInicialBloque);

                    //Se crea el Inodo de la Carpeta
                    struct TablaInodos NuevoInodo;
                    Tiempo = time(0);
                    Indice2 = 0;
                    NuevoInodo.FechaCreacion = Tiempo;
                    NuevoInodo.GrupoPropietario = 1; //Pendiente
                    NuevoInodo.Permisos = 664; //Pendiente
                    NuevoInodo.TamanoArchivo = 0;
                    NuevoInodo.Tipo[0] = '0';
                    NuevoInodo.UltimaFechaLectura = Tiempo;
                    NuevoInodo.UltimaFechaModific = Tiempo;
                    NuevoInodo.UsuarioPropietario = 1; //Pendiente
                    while(Indice2 < 15)
                    {
                        NuevoInodo.Apuntadores[Indice2] = -1;
                        Indice2++;
                    }

                    NumeroBloque++;  //Se Correo uno porque ya se creo el bloque de carpetas Padre


                    //Por ser una Carpeta solo ocupara un bloque
                    //Al primer apuntador del Inodo Carpeta se le coloca el bloque de la carpeta Nueva
                    NuevoInodo.Apuntadores[0] = NumeroBloque+1;

                    //Extrayendo el bloque de carpetas de apuntador del inodo padre para obtener el
                    //Numero de inodo del padre
                    struct BloqueCarpetas *CarpetasAux;
                    NumeroInodoPad = InodoInicio->Apuntadores[0];
                    NumeroInodoPad = SuperBl->InicioTablaBloques + ((NumeroInodoPad -1) * sizeof(struct BloqueCarpetas) );
                    fseek(Disco,NumeroInodoPad,SEEK_SET);
                    CarpetasAux = malloc(sizeof(struct BloqueCarpetas));
                    fread(CarpetasAux,sizeof(struct BloqueCarpetas),1,Disco);


                    //Se crea el Bloque de Carpetas y se guarda en el disco
                    struct BloqueCarpetas BloqCarp;
                    BloqCarp.ApuntadorCarpeta[0].Apuntador = NumeroInodo+1;
                    strcpy(BloqCarp.ApuntadorCarpeta[0].Nombre,NombreCarp);
                    BloqCarp.ApuntadorCarpeta[1].Apuntador = CarpetasAux->ApuntadorCarpeta[0].Apuntador;    //Apuntando a la carpeta Padre
                    strcpy(BloqCarp.ApuntadorCarpeta[1].Nombre,CarpetasAux->ApuntadorCarpeta[0].Nombre);
                    BloqCarp.ApuntadorCarpeta[2].Apuntador = -1;
                    BloqCarp.ApuntadorCarpeta[3].Apuntador = -1;
                    ByteBloqueArchivos = SuperBl->InicioTablaBloques + (NumeroBloque * sizeof(struct BloqueCarpetas));
                    InsertarBloqueCarpetas(&BloqCarp,Disco,ByteBloqueArchivos);
                    free(CarpetasAux);

                    //Se Inserta el Inodo del Archivo
                    //Se Escribe en Disco el Inodo del Archivo
                    ByteinodoNuevo = SuperBl->InicioTablaInodos + (NumeroInodo * sizeof(struct TablaInodos));
                    InsertarTablaInodo(&NuevoInodo,Disco,ByteinodoNuevo);

                    //Actulizando los bitmpat De Inodo y de Bloqes
                    int TotalBlo,TotalIno;
                    TotalBlo = SuperBl->TotalBloques;
                    TotalIno = SuperBl->TotalInodos;
                    char BitmapIno[TotalIno];
                    char BitMapBlo[TotalBlo];

                    fseek(Disco,SuperBl->InicioBitmapInodos,SEEK_SET);
                    fread(BitmapIno,TotalIno,1,Disco);
                    fseek(Disco,SuperBl->InicioBitmapBloques,SEEK_SET);
                    fread(BitMapBlo,TotalBlo,1,Disco);
                    Ind = 0;
                    NumeroBloque = CalcularNumeroBloque(1,true,SuperBl,TipoAjuste,Disco);

                    while(Ind < 2)
                    {
                        BitMapBlo[NumeroBloque] = '1';
                        NumeroBloque++;
                        Ind++;
                    }

                    BitmapIno[NumeroInodo] = '1';
                    fseek(Disco,SuperBl->InicioBitmapInodos,SEEK_SET);
                    fwrite(BitmapIno,TotalIno,1,Disco);
                    fseek(Disco,SuperBl->InicioBitmapBloques,SEEK_SET);
                    fwrite(BitMapBlo,TotalBlo,1,Disco);

                    //Actualizando el SuperBloque
                    SuperBl->BloquesLibres = SuperBl->BloquesLibres - 2;   //El Bloque Padre y el bloque Hijo
                    SuperBl->InodosLibres = SuperBl->InodosLibres -1;
                    SuperBl->PrimerBloqueLibre = DevolverPrimerBloqueLibre(SuperBl,Disco) +1;
                    SuperBl->PrimerInodoLibre = DevolverPrimerInodoLibre(SuperBl,Disco) +1;

                    struct SuperBloque NuevoSu;
                    NuevoSu.BloquesLibres = SuperBl->BloquesLibres;
                    NuevoSu.InicioBitmapBloques = SuperBl->InicioBitmapBloques;
                    NuevoSu.InicioBitmapInodos = SuperBl->InicioBitmapInodos;
                    NuevoSu.InicioTablaBloques = SuperBl->InicioTablaBloques;
                    NuevoSu.InicioTablaInodos = SuperBl->InicioTablaInodos;
                    NuevoSu.InodosLibres = SuperBl->InodosLibres;
                    NuevoSu.PrimerBloqueLibre = SuperBl->PrimerBloqueLibre;
                    NuevoSu.PrimerInodoLibre = SuperBl->PrimerInodoLibre;
                    NuevoSu.SistemaArchivos = SuperBl->SistemaArchivos;
                    NuevoSu.TamanoBloque = SuperBl->TamanoBloque;
                    NuevoSu.TamanoInodo = SuperBl->TamanoInodo;
                    NuevoSu.TotalBloques = SuperBl->TotalBloques;
                    NuevoSu.TotalInodos = SuperBl->TotalInodos;
                    NuevoSu.UltimaFechaDesmont = SuperBl->UltimaFechaDesmont;
                    NuevoSu.UltimaFechaMontura = SuperBl->UltimaFechaMontura;
                    NuevoSu.VecesMontado = SuperBl->VecesMontado;
                    InsertarSuperBloque(&NuevoSu,Disco,ByteInicialParticion);
                    free(SuperBl);
                    free(InodoInicio);

                    printf("Carpeta Guardada Correctamente\n");
                    return;


                }


            }
        }

        Indice++;
    }
    //NO HAY ESPACION EN EL INODO
    free(SuperBl);
    free(InodoInicio);

}

//*********************** METODO PARA PASAR LOS PARAMETROS NECESARIOS PARA CREAR UNA CARPETA *********************************************************************************************
//*********************************************************************************************************************************************************************************************************
void PasarParametrosCreaCarpeta(struct Particion *Partic, struct EBR *EbrRecibido,char *Path, FILE *DISCO, int TipParticion,int NumeroCarp)
{
    if(NumeroCarp==1)
    {
        //La carpeta a crearse va en la Raiz no se necesita una direccion especifica
        int ByteInodoPadre;
        char *TipoAjus;
        if(TipParticion==1)
        {
            ByteInodoPadre = BuscarCarpetaContenedora(Partic,NULL,"/",DISCO,1);
            TipoAjus = malloc(strlen(Partic->TipoAjuste));
            strcpy(TipoAjus,Partic->TipoAjuste);
            Path++;
            CrearCarpeta(ByteInodoPadre,Partic->ByteInicial,DISCO,TipoAjus,Path);
            free(TipoAjus);
        }else if(TipParticion==2)
        {
            ByteInodoPadre = BuscarCarpetaContenedora(NULL,EbrRecibido,"/",DISCO,2);
            TipoAjus = malloc(strlen(EbrRecibido->Fitt));
            strcpy(TipoAjus,EbrRecibido->Fitt);
            Path++;
            CrearCarpeta(ByteInodoPadre,EbrRecibido->ByteInicial,DISCO,TipoAjus,Path);
            free(TipoAjus);
        }

    }else
    {
        char *DireccionRecibida;
        char *NombreCarpetaNueva;
        char *TipoAjus;
        int ByteInodoPadre;

        DireccionRecibida = DevolverDireccionDeCarpeta(Path,NumeroCarp);
        NombreCarpetaNueva = DevolverNombreCarpeta(Path);
        printf("La Direccion es: %s\n",DireccionRecibida);
        printf("El nombre de la carpeta es: %s\n",NombreCarpetaNueva);

        //Obteniendo el Byte de inicio del Inodo Padre
        if(TipParticion==1)
        {
            ByteInodoPadre = BuscarCarpetaContenedora(Partic,NULL,DireccionRecibida,DISCO,1);
            TipoAjus = malloc(strlen(Partic->TipoAjuste));
            strcpy(TipoAjus,Partic->TipoAjuste);
        }
        else if(TipParticion==2)
        {
            ByteInodoPadre = BuscarCarpetaContenedora(NULL,EbrRecibido,DireccionRecibida,DISCO,2);
            TipoAjus = malloc(strlen(EbrRecibido->Fitt));
            strcpy(TipoAjus,EbrRecibido->Fitt);
        }


        if(ByteInodoPadre ==-1)
        {
            //La Direccion NO EXISTE
        }
        else
        {
        //la Direccion SI EXISTE
            if(TipParticion ==1)
            {
                //Se Manda a crear la Carpeta
                CrearCarpeta(ByteInodoPadre,Partic->ByteInicial,DISCO,TipoAjus,NombreCarpetaNueva);
            }
            else if(TipParticion ==2)
            {
                //Se Manda a crear la Carpeta
                CrearCarpeta(ByteInodoPadre,EbrRecibido->ByteInicial,DISCO,TipoAjus,NombreCarpetaNueva);

            }
        }


        free(DireccionRecibida);
        free(NombreCarpetaNueva);
        free(TipoAjus);

    }
}

//***************************** METODO PARA CREAR UNA CARPETA CON SU RESPECTIVA DIRECCION ********************************************************************************************
//***********************************************************************************************************************************************************************************************
void CrearCarpetaConDireccion(char *Id, char *Path, char *CrearCarpeta)
{
    //Buscando la particion Solicitada por el id
    struct MBR *EstrucMBR;

    char DiscoDireccion[125];
    char *NombreParticion;
    int TotalCarpetas;

    //Verificando que por lo menos en la direccion venga una carpeta para ser creada
    TotalCarpetas = NumeroCarpetas(Path);
    if(TotalCarpetas==-1)
    {
        printf(">>>>ERROR: la direccion para crear carpetas NO es válida<<<<\n");
        return;
    }
    NombreParticion = malloc(100*sizeof(char));
    strcpy(NombreParticion,Devolverparticion(Id));
    strcpy(DiscoDireccion,DevolverPath(Id));

    if((strcasecmp(DiscoDireccion,"a"))!=0)
    {
        //El id ya se ha montado
        FILE *DiscoLeer;
        DiscoLeer = fopen(DiscoDireccion,"rb+");
        if(DiscoLeer==NULL)
        {
            printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
            return;
        }

        EstrucMBR = malloc(4*sizeof(struct MBR)+1);
        fseek(DiscoLeer,0,SEEK_SET);
        fread(EstrucMBR,sizeof(struct MBR),1,DiscoLeer);


        //Buscando la Particion a grafica

        if((strcmp(EstrucMBR->Particion1.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion1.Tipo,"p")==0) )
        {
           PasarParametrosCreaCarpeta(&EstrucMBR->Particion1,NULL,Path,DiscoLeer,1,TotalCarpetas);

        }else if((strcmp(EstrucMBR->Particion2.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion2.Tipo,"p")==0) )
        {
            PasarParametrosCreaCarpeta(&EstrucMBR->Particion2,NULL,Path,DiscoLeer,1,TotalCarpetas);

        }else if( (strcmp(EstrucMBR->Particion3.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion3.Tipo,"p")==0) )
        {
            PasarParametrosCreaCarpeta(&EstrucMBR->Particion3,NULL,Path,DiscoLeer,1,TotalCarpetas);

        }else if( (strcmp(EstrucMBR->Particion4.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion4.Tipo,"p")==0) )
        {
            PasarParametrosCreaCarpeta(&EstrucMBR->Particion4,NULL,Path,DiscoLeer,1,TotalCarpetas);

        }else if(strcasecmp(EstrucMBR->Particion1.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion1.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            else
            {
                //Se Crea el Disco
                PasarParametrosCreaCarpeta(NULL,EbrRecibido,Path,DiscoLeer,2,TotalCarpetas);
                free(EbrRecibido);
            }



        }else if(strcasecmp(EstrucMBR->Particion2.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion2.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            else
            {
                //Se Crea el Disco
                PasarParametrosCreaCarpeta(NULL,EbrRecibido,Path,DiscoLeer,2,TotalCarpetas);
                free(EbrRecibido);
            }




        }else if(strcasecmp(EstrucMBR->Particion3.Tipo,"e")==0)
        {
            //Vericando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion3.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            else
            {
                //Se Crea el Disco
                PasarParametrosCreaCarpeta(NULL,EbrRecibido,Path,DiscoLeer,2,TotalCarpetas);
                free(EbrRecibido);
            }



        }else if(strcasecmp(EstrucMBR->Particion4.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion4.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");
            else
            {
                //Se Crea el Disco
                PasarParametrosCreaCarpeta(NULL,EbrRecibido,Path,DiscoLeer,2,TotalCarpetas);
                free(EbrRecibido);
            }



        }else
            printf(">>>>ERROR: no se pudo crear la grafica de la particion indicada, no se encontro dentro del disco <<<<\n");

        fclose(DiscoLeer);
        free(EstrucMBR);
        free(NombreParticion);


    }else
        printf(">>>>ERROR: imposible graficar Disco no encontrado<<<<\n");


}

//**************************** METODO PARA CREAR EL ARBOL DEL SISTEMA DE ARCHIVOS ****************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************************************
void CrearReporteArbol(char *Direccion, char *IdRec)
{
    struct MBR *EstrucMBR;

    char DiscoDireccion[125];
    char *NombreParticion;
    NombreParticion = malloc(100*sizeof(char));
    strcpy(NombreParticion,Devolverparticion(IdRec));
    strcpy(DiscoDireccion,DevolverPath(IdRec));

    if((strcasecmp(DiscoDireccion,"a"))!=0)
    {
        //El id ya se ha montado
        FILE *DiscoLeer;
        DiscoLeer = fopen(DiscoDireccion,"rb+");
        if(DiscoLeer==NULL)
        {
            printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
            return;
        }

        EstrucMBR = malloc(4*sizeof(struct MBR)+1);
        fseek(DiscoLeer,0,SEEK_SET);
        fread(EstrucMBR,sizeof(struct MBR),1,DiscoLeer);


        //Buscando la Particion a grafica

        if((strcmp(EstrucMBR->Particion1.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion1.Tipo,"p")==0) )
        {
            GraficarArbol(&EstrucMBR->Particion1,NULL,1,Direccion,DiscoLeer);

        }else if((strcmp(EstrucMBR->Particion2.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion2.Tipo,"p")==0) )
        {
            GraficarArbol(&EstrucMBR->Particion2,NULL,1,Direccion,DiscoLeer);

        }else if( (strcmp(EstrucMBR->Particion3.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion3.Tipo,"p")==0) )
        {
            GraficarArbol(&EstrucMBR->Particion3,NULL,1,Direccion,DiscoLeer);

        }else if( (strcmp(EstrucMBR->Particion4.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion4.Tipo,"p")==0) )
        {
            GraficarArbol(&EstrucMBR->Particion4,NULL,1,Direccion,DiscoLeer);

        }else if(strcasecmp(EstrucMBR->Particion1.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion1.ByteInicial,NombreParticion,9,Direccion))
            {
                printf("Grafica: Arbol del sistema de Archivos, Particion Logica\n");
            }else
                 printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");

        }else if(strcasecmp(EstrucMBR->Particion2.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion2.ByteInicial,NombreParticion,9,Direccion))
            {
                printf("Grafica: Arbol del sistema de Archivos, Particion Logica\n");
            }else
                 printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");


        }else if(strcasecmp(EstrucMBR->Particion3.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion3.ByteInicial,NombreParticion,9,Direccion))
            {
                printf("Grafica: Arbol del sistema de Archivos, Particion Logica\n");
            }else
                 printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");


        }else if(strcasecmp(EstrucMBR->Particion4.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            if(BuscarParticionLogica(DiscoLeer,EstrucMBR->Particion4.ByteInicial,NombreParticion,9,Direccion))
            {
                printf("Grafica: Arbol del sistema de Archivos, Particion Logica\n");
            }else
                 printf(">>>>ERROR: particion NO encontrada en el Disco<<<<\n");


        }else
            printf(">>>>ERROR: no se pudo crear la grafica de la particion indicada, no se encontro dentro del disco <<<<\n");

        fclose(DiscoLeer);
        free(EstrucMBR);


    }else
        printf(">>>>ERROR: imposible graficar Disco no encontrado<<<<\n");
 }
//*************************** METODO PARA OBTENER EL BYTE INICIAL DE UN ARCHIVO O CARPETA *****************************************************************************************************************************************************
//*********************************************************************************************************************************************************************************************************************************************
int DevolverByteInicialCarpetaArchivo(int ByteInodoPadre,char *NombreRec,FILE *Disco,int ByteInicialParticion,int TipoBloqueInodo)
{
    //Si TipoBloqueInodo = 1 ---->> Lo Que Retorna es el byte del Bloque de Carpetas padre del archivo o carpeta
    //Si TipoBloqueInodo = 2 ---->> Lo que retorna es el byte del Inodo del Archivo o carpeta
    int ByteInodo,Indice,ByteBloque,IndiceCarp;
    struct TablaInodos *InodoPadre;
    struct SuperBloque *SuperBlo;

    //Extrallendo el SuperBloque
    SuperBlo = malloc(sizeof(struct SuperBloque));
    fseek(Disco,ByteInicialParticion,SEEK_SET);
    fread(SuperBlo,sizeof(struct SuperBloque),1,Disco);

    if(SuperBlo->TamanoInodo==0)
    {
        //La Particion No ha Sido Formateada
        printf(">>>>ERROR: la particion solicitada no ha sido formateada<<<<\n");
        free(SuperBlo);
        return -1;
    }

    //Extrallendo el Inodo Padre
    InodoPadre = malloc(sizeof(struct TablaInodos));
    fseek(Disco,ByteInodoPadre,SEEK_SET);
    fread(InodoPadre,sizeof(struct TablaInodos),1,Disco);
    ByteInodo = -1;



    Indice=0;
    while(Indice < 15)
    {
        if(Indice ==12)
        {
            //Primer Apuntador Indirecto
        }else if(Indice==13)
        {
            //Apuntador Doble Indirecto
        }else if(Indice==14)
        {
            //Apuntador Tripe Indirecto
        }else
        {
            //Apuntadores Directos
            if(InodoPadre->Apuntadores[Indice]!=-1)
            {


                struct BloqueCarpetas *Carp;
                Carp = malloc(sizeof(struct BloqueCarpetas));
                ByteBloque = InodoPadre->Apuntadores[Indice];
                ByteBloque = SuperBlo->InicioTablaBloques + ((ByteBloque-1)*sizeof(struct BloqueCarpetas));
                fseek(Disco,ByteBloque,SEEK_SET);
                fread(Carp,sizeof(struct BloqueCarpetas),1,Disco);
                IndiceCarp = 0;
                while(IndiceCarp < 4)
                {
                    if(Carp->ApuntadorCarpeta[IndiceCarp].Apuntador != -1)
                    {
                       if(strcmp(Carp->ApuntadorCarpeta[IndiceCarp].Nombre,NombreRec)==0)
                        {

                           if(TipoBloqueInodo==1)
                           {
                               ByteBloque = InodoPadre->Apuntadores[Indice];
                               ByteBloque = SuperBlo->InicioTablaBloques + ((ByteBloque-1)*(sizeof(struct BloqueCarpetas)));
                               free(Carp);
                               free(InodoPadre);
                               free(SuperBlo);
                               return ByteBloque;
                           }else if(TipoBloqueInodo==2)
                           {
                                ByteBloque = Carp->ApuntadorCarpeta[IndiceCarp].Apuntador;
                                ByteBloque = SuperBlo->InicioTablaInodos + ((ByteBloque-1)*(sizeof(struct TablaInodos)));
                                free(Carp);
                                free(InodoPadre);
                                free(SuperBlo);
                                return ByteBloque;
                           }
                        }
                    }
                    IndiceCarp++;
                }
                free(Carp);

            }

        }

        Indice++;
    }

    free(InodoPadre);
    free(SuperBlo);
    return -1;


}
//**************************METODO PARA GRAFICAR EL REPORTE FILE DE UNA ARCHIVO ***************************************************************************************************************************************************
//*********************************************************************************************************************************************************************************************************************************
void GraficarFile(FILE *DISCO, char *DireccionGrafica, char *DireccionArchivo,struct Particion *Primaria,struct EBR *Logica, int TipoPart, int ConsolaGrafica)
{
    int TotalCarpetas,ByteInodoPadre;
    char *DireccionArc,*NombreRec;
    struct SuperBloque *SuperBlo;
    bool EstadoArc;

    EstadoArc = false;
    TotalCarpetas = NumeroCarpetas(DireccionArchivo);
    SuperBlo = malloc(sizeof(struct SuperBloque));
    //Obteniendo el Numero de Carpetas que tiene la direccion
    if(TotalCarpetas==-1)
    {
        printf(">>>>ERROR: la direccion para crear carpetas NO es válida<<<<\n");
        free(SuperBlo);
        return;
    }

    if(TipoPart==1)
    {
        if(TotalCarpetas==1)
        {   //Cuando el Archivo esta en la raiz
            ByteInodoPadre = BuscarCarpetaContenedora(Primaria,NULL,"/",DISCO,1);
            fseek(DISCO,Primaria->ByteInicial,SEEK_SET);
            fread(SuperBlo,sizeof(struct SuperBloque),1,DISCO);
        }else
        {
            DireccionArc = DevolverDireccionDeCarpeta(DireccionArchivo,TotalCarpetas);
            ByteInodoPadre = BuscarCarpetaContenedora(Primaria,NULL,DireccionArc,DISCO,1);
            fseek(DISCO,Primaria->ByteInicial,SEEK_SET);
            fread(SuperBlo,sizeof(struct SuperBloque),1,DISCO);

        }
    }
    else if(TipoPart==2)
    {
        if(TotalCarpetas==1)
        {   //Cuando el Archivo esta en la raiz
            ByteInodoPadre = BuscarCarpetaContenedora(NULL,Logica,"/",DISCO,2);
            fseek(DISCO,Logica->ByteInicial,SEEK_SET);
            fread(SuperBlo,sizeof(struct SuperBloque),1,DISCO);

        }else
        {
            DireccionArc = DevolverDireccionDeCarpeta(DireccionArchivo,TotalCarpetas);
            ByteInodoPadre = BuscarCarpetaContenedora(NULL,Logica,DireccionArc,DISCO,2);
            fseek(DISCO,Logica->ByteInicial,SEEK_SET);
            fread(SuperBlo,sizeof(struct SuperBloque),1,DISCO);
        }
    }

    //SE empieza a buscar el Archivo en su inodo Padre
    if(ByteInodoPadre ==-1)
    {
        if(TotalCarpetas!=1)
            free(DireccionArc);
        free(SuperBlo);
        printf(">>>>ERROR: la direccion del Archivo Solicitado NO existe<<<<\n");
        return;
    }
    else
    {
        int Indice,IndiceCarp,ByteBloque,Contador;
        //La Direccion Si existe se busca en el Inodo el archivo Solicitado

        NombreRec = DevolverNombreArchivo(DireccionArchivo);
        struct TablaInodos *InodoPadre;
        InodoPadre = malloc(sizeof(struct TablaInodos));
        fseek(DISCO,ByteInodoPadre,SEEK_SET);
        fread(InodoPadre,sizeof(struct TablaInodos),1,DISCO);

        //Buscando en los apuntadores del inodo el archivo Solicitado
        Indice = 0;
        while(Indice < 15)
        {
            if(Indice ==12)
            {
                //Primer Apuntador Indirecto
            }else if(Indice==13)
            {
                //Apuntador Doble Indirecto
            }else if(Indice==14)
            {
                //Apuntador Tripe Indirecto
            }else
            {
                //Apuntadores Directos
                if(InodoPadre->Apuntadores[Indice]!=-1)
                {
                    struct BloqueCarpetas *Carp;
                    Carp = malloc(sizeof(struct BloqueCarpetas));
                    ByteBloque = InodoPadre->Apuntadores[Indice];
                    ByteBloque = SuperBlo->InicioTablaBloques + ((ByteBloque-1)*sizeof(struct BloqueCarpetas));
                    fseek(DISCO,ByteBloque,SEEK_SET);
                    fread(Carp,sizeof(struct BloqueCarpetas),1,DISCO);
                    IndiceCarp = 0;
                    while(IndiceCarp < 4)
                    {
                        if(Carp->ApuntadorCarpeta[IndiceCarp].Apuntador != -1)
                        {
                            if(strcmp(Carp->ApuntadorCarpeta[IndiceCarp].Nombre,NombreRec)==0)
                            {
                                EstadoArc=true;
                                struct TablaInodos *InodoArchivo;
                                InodoArchivo = malloc(sizeof(struct TablaInodos));
                                ByteBloque = Carp->ApuntadorCarpeta[IndiceCarp].Apuntador;
                                ByteBloque = SuperBlo->InicioTablaInodos + ((ByteBloque -1)*sizeof(struct TablaInodos));
                                fseek(DISCO,ByteBloque,SEEK_SET);
                                fread(InodoArchivo,sizeof(struct TablaInodos),1,DISCO);

                                //Es el Archivo que se Busca
                                //Se Empieza a Graficar
                                FILE *Grafica;
                                if(ConsolaGrafica==1)
                                {

                                    Grafica =  fopen("GraficaFile.dot","w+");
                                    if(Grafica==NULL)
                                    {
                                        printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
                                        free(InodoArchivo);
                                        free(Carp);
                                        free(InodoPadre);
                                        free(SuperBlo);
                                        return;
                                    }

                                    fprintf(Grafica,"digraph G {\n");
                                    fprintf(Grafica,"nodesep=.1;\n");
                                    fprintf(Grafica,"\trankdir=LR;\n");
                                    fprintf(Grafica,"\tnode [shape=record,width=.1,height=.1];\n");
                                    fprintf(Grafica,"\t subgraph cluster0{\n");
                                    fprintf(Grafica,"\tlabel=\"CONTENIDO DEL ARCHIVO: %s\"\n",NombreRec);
                                }else
                                    printf("Contenido del Archivo: %s\n",NombreRec);   //Para el comando Cat

                                if(InodoArchivo->TamanoArchivo!=0)
                                {
                                    //Se Escriben todos los bloques del Archivo
                                    Contador = 0;
                                    if(ConsolaGrafica==1)
                                        fprintf(Grafica,"node1 [label = \"<f3> Contenido |");

                                    while(Contador < 15)
                                    {
                                        if(InodoArchivo->Apuntadores[Contador]!=-1)
                                        {
                                            if(Contador==12)
                                            {
                                                //Primer Apuntador Indirecto
                                            }else if(Contador==13)
                                            {
                                                //Apuntador Indirecto doble
                                            }else if(Contador==14)
                                            {
                                                //Apuntador Indirecto Triple
                                            }else
                                            {
                                                struct Archivos *BloqueArc;
                                                BloqueArc = malloc(sizeof(struct Archivos));
                                                ByteBloque = InodoArchivo->Apuntadores[Contador];
                                                ByteBloque = SuperBlo->InicioTablaBloques + ((ByteBloque -1)*(sizeof(struct Archivos)));
                                                fseek(DISCO,ByteBloque,SEEK_SET);
                                                fread(BloqueArc,sizeof(struct Archivos),1,DISCO);
                                                if(ConsolaGrafica==1)
                                                    fprintf(Grafica,"%s",BloqueArc->Contenido);
                                                else
                                                    printf("%s\n",BloqueArc->Contenido);

                                                free(BloqueArc);
                                            }
                                        }

                                        Contador++;
                                    }
                                    if(ConsolaGrafica==1)
                                        fprintf(Grafica,"\", height=1];\n");
                                }else
                                {
                                    if(ConsolaGrafica==1)
                                    {
                                        fprintf(Grafica,"node1 [label = \"<f3> El Tamano de archivo %s es 0",NombreRec);
                                        fprintf(Grafica,"\", height=1];\n");
                                    }else
                                        printf("El Tamano del Archivo: %s es 0\n",NombreRec);
                                }



                                if(ConsolaGrafica==1)
                                {
                                    fprintf(Grafica,"\t}\n");
                                    fprintf(Grafica,"}");

                                    //CERRANDO EL ARCHIVO .dot

                                    fclose(Grafica);
                                    char Pa[120];
                                    strcpy(Pa,"dot -Tjpg GraficaFile.dot -o ");
                                    strcat(Pa,DireccionGrafica);
                                    system(Pa);
                                    printf("Grafica generada correctamente\n");
                                }
                                Indice=15;
                                IndiceCarp=15;
                                free(InodoArchivo);
                            }
                        }
                        IndiceCarp++;
                    }
                    free(Carp);
                }

            }

            Indice++;
        }
        free(InodoPadre);
        free(NombreRec);
        free(SuperBlo);
    }

    if(TotalCarpetas!=1)
        free(DireccionArc);
    if(EstadoArc==false)
        printf(">>>>ERROR: el Archivo solicitado no se encuentra en la ruta especificada<<<<\n");
}

//********************** METODO PARA CREAR EL REPORT FILE Y LS DE UNA ARCHIVO O CARPETA **********************************************************************************************************************************
//************************************************************************************************************************************************************************************************************************
void CrearReporteFileLs(char *DireccionGrafica, char *IdRec, char *DireccionArchivo, int TipoGra,int ConsolaGrafica)
{
    //Si tipo = 1 ---->> Sera Grafica file, Si tipoGra = 2 ------->> Sera Grafica ls
    struct MBR *EstrucMBR;

    char DiscoDireccion[125];
    char *NombreParticion;
    NombreParticion = malloc(100*sizeof(char));
    strcpy(NombreParticion,Devolverparticion(IdRec));
    strcpy(DiscoDireccion,DevolverPath(IdRec));

    if((strcasecmp(DiscoDireccion,"a"))!=0)
    {
        //El id ya se ha montado
        FILE *DiscoLeer;
        DiscoLeer = fopen(DiscoDireccion,"rb+");
        if(DiscoLeer==NULL)
        {
            printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
            return;
        }

        EstrucMBR = malloc(4*sizeof(struct MBR)+1);
        fseek(DiscoLeer,0,SEEK_SET);
        fread(EstrucMBR,sizeof(struct MBR),1,DiscoLeer);


        //Buscando la Particion a grafica

        if((strcmp(EstrucMBR->Particion1.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion1.Tipo,"p")==0) )
        {
            if(TipoGra==1)
            {
                //Grafica File
                GraficarFile(DiscoLeer,DireccionGrafica,DireccionArchivo,&EstrucMBR->Particion1,NULL,1,ConsolaGrafica);
            }else if(TipoGra==2)
            {
                //Grafica ls
            }


        }else if((strcmp(EstrucMBR->Particion2.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion2.Tipo,"p")==0) )
        {
            if(TipoGra==1)
            {
                //Grafica File
                GraficarFile(DiscoLeer,DireccionGrafica,DireccionArchivo,&EstrucMBR->Particion2,NULL,1,ConsolaGrafica);
            }else if(TipoGra==2)
            {
                //Grafica ls
            }

        }else if( (strcmp(EstrucMBR->Particion3.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion3.Tipo,"p")==0) )
        {
            if(TipoGra==1)
            {
                //Grafica File
                GraficarFile(DiscoLeer,DireccionGrafica,DireccionArchivo,&EstrucMBR->Particion3,NULL,1,ConsolaGrafica);
            }else if(TipoGra==2)
            {
                //Grafica ls
            }

        }else if( (strcmp(EstrucMBR->Particion4.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion4.Tipo,"p")==0) )
        {
            if(TipoGra==1)
            {
                //Grafica File
                GraficarFile(DiscoLeer,DireccionGrafica,DireccionArchivo,&EstrucMBR->Particion4,NULL,1,ConsolaGrafica);
            }else if(TipoGra==2)
            {
                //Grafica ls
            }

        }else if(strcasecmp(EstrucMBR->Particion1.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion1.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: la partcion no fue Encontrada dentro del disco<<<<\n");
            else
            {
                if(TipoGra==1)
                {
                    //Grafica File
                    GraficarFile(DiscoLeer,DireccionGrafica,DireccionArchivo,NULL,EbrRecibido,2,ConsolaGrafica);
                }else if(TipoGra==2)
                {
                    //Grafica ls
                }
                free(EbrRecibido);
            }

        }else if(strcasecmp(EstrucMBR->Particion2.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion2.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: la partcion no fue Encontrada dentro del disco<<<<\n");
            else
            {
                if(TipoGra==1)
                {
                    //Grafica File
                    GraficarFile(DiscoLeer,DireccionGrafica,DireccionArchivo,NULL,EbrRecibido,2,ConsolaGrafica);
                }else if(TipoGra==2)
                {
                    //Grafica ls
                }
                free(EbrRecibido);
            }

        }else if(strcasecmp(EstrucMBR->Particion3.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion3.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: la partcion no fue Encontrada dentro del disco<<<<\n");
            else
            {
                if(TipoGra==1)
                {
                    //Grafica File
                    GraficarFile(DiscoLeer,DireccionGrafica,DireccionArchivo,NULL,EbrRecibido,2,ConsolaGrafica);
                }else if(TipoGra==2)
                {
                    //Grafica ls
                }
                free(EbrRecibido);
            }

        }else if(strcasecmp(EstrucMBR->Particion4.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion4.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: la partcion no fue Encontrada dentro del disco<<<<\n");
            else
            {
                if(TipoGra==1)
                {
                    //Grafica File
                    GraficarFile(DiscoLeer,DireccionGrafica,DireccionArchivo,NULL,EbrRecibido,2,ConsolaGrafica);
                }else if(TipoGra==2)
                {
                    //Grafica ls
                }
                free(EbrRecibido);
            }

        }else
            printf(">>>>ERROR: no se pudo crear la grafica de la particion indicada, no se encontro dentro del disco <<<<\n");

        fclose(DiscoLeer);
        free(EstrucMBR);


    }else
        printf(">>>>ERROR: imposible graficar Disco no encontrado<<<<\n");

}
//****************************** METODO PARA CAMBIAR LA FECHA A UN INODO *******************************************************************************************************************************************
//******************************************************************************************************************************************************************************************************************
void CambiarFechaInodo(FILE *Disco, int byteInodo,int TipoFecha)
{
    int Indice;
    struct TablaInodos *InodoMod;
    InodoMod = malloc(sizeof(struct TablaInodos));
    fseek(Disco,byteInodo,SEEK_SET);
    fread(InodoMod,sizeof(struct TablaInodos),1,Disco);
    time_t Tiempo = time(0);
    if(TipoFecha==1)
    {
        //Se Modificara la Fecha de Lectura
        InodoMod->UltimaFechaLectura = Tiempo;

    }else if(TipoFecha==2)
    {
        //Se Modificara la Fecha de Creacion
        InodoMod->FechaCreacion = Tiempo;

    }else if(TipoFecha==3)
    {
        //Se Modificara la Fecha de Modificacion
        InodoMod->UltimaFechaModific = Tiempo;
    }

    struct TablaInodos NuevoInodo;
    NuevoInodo.FechaCreacion = InodoMod->FechaCreacion;
    NuevoInodo.GrupoPropietario = InodoMod->GrupoPropietario;
    NuevoInodo.Permisos = InodoMod->Permisos;
    NuevoInodo.TamanoArchivo = InodoMod->TamanoArchivo;
    NuevoInodo.Tipo[0] = InodoMod->Tipo[0];
    NuevoInodo.UltimaFechaLectura = InodoMod->UltimaFechaLectura;
    NuevoInodo.UltimaFechaModific = InodoMod->UltimaFechaModific;
    NuevoInodo.UsuarioPropietario = InodoMod->UsuarioPropietario;

    Indice =0;
    while(Indice < 15)
    {
        NuevoInodo.Apuntadores[Indice] = -1;
        Indice++;
    }

    Indice=0;
    while(Indice < 15)
    {
        NuevoInodo.Apuntadores[Indice] = InodoMod->Apuntadores[Indice];
        Indice++;
    }
    InsertarTablaInodo(&NuevoInodo,Disco,byteInodo);
    free(InodoMod);
}


//****************************** METODO BORRAR EL CONTENIDO DE UNA CARPETA DADA *************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************************************
int BorrarContenidoCarpeta(int ByteInodo, struct SuperBloque *SuperBloRec,FILE *Disco,int NumeroInodo)
{
    int TotalBloques,Indice,ByteEnviar;
    struct TablaInodos *InodoRec;
    InodoRec = malloc(sizeof(struct TablaInodos));
    fseek(Disco,ByteInodo,SEEK_SET);
    fread(InodoRec,sizeof(struct TablaInodos),1,Disco);


    TotalBloques=0;

    //Modificando el Bitmap de Inodos
    char BitmapIn[SuperBloRec->TotalInodos];
    fseek(Disco,SuperBloRec->InicioBitmapInodos,SEEK_SET);
    fread(BitmapIn,SuperBloRec->TotalInodos,1,Disco);
    BitmapIn[NumeroInodo] ='0';
    fseek(Disco,SuperBloRec->InicioBitmapInodos,SEEK_SET);
    fwrite(BitmapIn,SuperBloRec->TotalInodos,1,Disco);

    //Verficando si se trata de una archivo o carpeta
    if(InodoRec->Tipo[0]=='1')
    {
        //Es un Archivo
        Indice = 0;
        while(Indice < 15)
        {
            if(Indice==12)
            {
                //Primer Apuntador Indirecto
            }else if(Indice==13)
            {
                //Apuntador Indirecto Doble
            }else if(Indice==14)
            {
                //Apuntador Indirecto Triple
            }else
            {
                //Apunadores Directos
                if(InodoRec->Apuntadores[Indice] != -1)
                {
                    //Por tratarse de un inodo de archivos, los puntadores apuntan a bloques de archivos
                    TotalBloques++;

                    //Actualizando el bitmap de Bloques
                    char BitmapBlo[SuperBloRec->TotalBloques];
                    fseek(Disco,SuperBloRec->InicioBitmapBloques,SEEK_SET);
                    fread(BitmapBlo,SuperBloRec->TotalBloques,1,Disco);
                    BitmapBlo[InodoRec->Apuntadores[Indice] -1] ='0';
                    fseek(Disco,SuperBloRec->InicioBitmapBloques,SEEK_SET);
                    fwrite(BitmapBlo,SuperBloRec->TotalBloques,1,Disco);


                }

            }
            Indice++;
        }
        free(InodoRec);
        return TotalBloques;

    }else if(InodoRec->Tipo[0]=='0')
    {
        //Es una Carpeta

        Indice = 0;
        while(Indice < 15)
        {
            if(InodoRec->Apuntadores[Indice]!= -1)
            {
                int BytepBloqueHijo,IndiceCarpetas;
                BytepBloqueHijo = SuperBloRec->InicioTablaBloques;
                BytepBloqueHijo =  BytepBloqueHijo + ((InodoRec->Apuntadores[Indice]-1) * sizeof(struct BloqueCarpetas));

                if(Indice==12)
                {
                    //Primer Apuntador Indirecto
                }else if(Indice==13)
                {
                    //Apuntador Indirecto Doble
                }else if(Indice==14)
                {
                    //Apuntador Indirecto Triple
                }else
                {
                //Apunadores Directos


                    TotalBloques++;
                    //Actualizando el bitmap de Bloques
                    char BitmapBlo[SuperBloRec->TotalBloques];
                    fseek(Disco,SuperBloRec->InicioBitmapBloques,SEEK_SET);
                    fread(BitmapBlo,SuperBloRec->TotalBloques,1,Disco);
                    BitmapBlo[InodoRec->Apuntadores[Indice] -1] ='0';
                    fseek(Disco,SuperBloRec->InicioBitmapBloques,SEEK_SET);
                    fwrite(BitmapBlo,SuperBloRec->TotalBloques,1,Disco);

                    struct BloqueCarpetas *BloqueCarp;

                    //Extrallendo el Bloque de Carpetas
                    BloqueCarp = malloc(sizeof(struct BloqueCarpetas));
                    fseek(Disco,BytepBloqueHijo,SEEK_SET);
                    fread(BloqueCarp,sizeof(struct BloqueCarpetas),1,Disco);

                    IndiceCarpetas=0;
                    //Vericando los apuntadores del bloque Carpetas
                    while( (IndiceCarpetas < 4) )
                    {
                        if(BloqueCarp->ApuntadorCarpeta[IndiceCarpetas].Apuntador != -1)
                        {
                            //Se manda a evaluar todo lo que Hay dentro de este Bloque de Carpetas
                            //Eliminando todo el Contenido del nuevo Disco
                            if((Indice !=0) || ((IndiceCarpetas!=0) && (IndiceCarpetas!=1)))
                            {
                                //Se excluyen los dos primero apuntadores del primer Bloque ya que estos
                                //Apuntan al inodo padre de la carpeta y la inodo propio de la carpeta
                                ByteEnviar = BloqueCarp->ApuntadorCarpeta[IndiceCarpetas].Apuntador;
                                ByteEnviar = SuperBloRec->InicioTablaInodos + ((ByteEnviar -1)*(sizeof(struct TablaInodos)));
                                TotalBloques = TotalBloques + BorrarContenidoCarpeta(ByteEnviar,SuperBloRec,Disco,BloqueCarp->ApuntadorCarpeta[IndiceCarpetas].Apuntador-1);
                            }
                        }
                        IndiceCarpetas++;
                    }
                    free(BloqueCarp);




                }

            }

        Indice++;
        }

    }
    free(InodoRec);
    return TotalBloques;

}

//****************************** METODO PARA LA ELIMINACION DE UN ARCHIVO O CARPETA *********************************************************************************************************************************
//*******************************************************************************************************************************************************************************************************************
void MetodoDeEliminacion(struct Particion *Primaria, struct EBR *Logica, int TipoPart,char *Direccion,FILE *DISCO)
{
    int TotalCarpetas,ByteInodoPadre,ByteCarpetaArchivo,ByteBloquePadre,Indice,BytepBloqueHijo,TotalBloques,TotalInodos;
    int ByteEnviar;
    char *DireccionArchCarp,*NombreArchCarp;
    struct SuperBloque *SuperBlo;



    TotalCarpetas = NumeroCarpetas(Direccion);
    SuperBlo = malloc(sizeof(struct SuperBloque));
    //Obteniendo el Numero de Carpetas que tiene la direccion
    if(TotalCarpetas==-1)
    {
        printf(">>>>ERROR: la direccion para crear carpetas NO es válida<<<<\n");
        free(SuperBlo);
        return;
    }

    if(TipoPart==1)
    {
        if(TotalCarpetas==1)
        {   //Cuando el Archivo esta en la raiz
            ByteInodoPadre = BuscarCarpetaContenedora(Primaria,NULL,"/",DISCO,1);
            Direccion++;
            NombreArchCarp = malloc(strlen(Direccion));
            strcpy(NombreArchCarp,Direccion);
            fseek(DISCO,Primaria->ByteInicial,SEEK_SET);
            fread(SuperBlo,sizeof(struct SuperBloque),1,DISCO);
        }else
        {
            DireccionArchCarp = DevolverDireccionDeCarpeta(Direccion,TotalCarpetas);
            NombreArchCarp = DevolverNombreCarpeta(Direccion);
            ByteInodoPadre = BuscarCarpetaContenedora(Primaria,NULL,DireccionArchCarp,DISCO,1);
            fseek(DISCO,Primaria->ByteInicial,SEEK_SET);
            fread(SuperBlo,sizeof(struct SuperBloque),1,DISCO);

        }
    }
    else if(TipoPart==2)
    {
        if(TotalCarpetas==1)
        {   //Cuando el Archivo esta en la raiz
            ByteInodoPadre = BuscarCarpetaContenedora(NULL,Logica,"/",DISCO,2);
            Direccion++;
            NombreArchCarp = malloc(strlen(Direccion));
            strcpy(NombreArchCarp,Direccion);
            fseek(DISCO,Logica->ByteInicial,SEEK_SET);
            fread(SuperBlo,sizeof(struct SuperBloque),1,DISCO);

        }else
        {
            DireccionArchCarp = DevolverDireccionDeCarpeta(Direccion,TotalCarpetas);
            NombreArchCarp = DevolverNombreCarpeta(Direccion);
            ByteInodoPadre = BuscarCarpetaContenedora(NULL,Logica,DireccionArchCarp,DISCO,2);
            fseek(DISCO,Logica->ByteInicial,SEEK_SET);
            fread(SuperBlo,sizeof(struct SuperBloque),1,DISCO);
        }
    }

    //SE empieza a buscar el Archivo en su inodo Padre
    if(ByteInodoPadre ==-1)
    {
        //La Direccion del Archivo no fue Encontrada
        if(TotalCarpetas!=1)
            free(DireccionArchCarp);
        free(SuperBlo);
        printf(">>>>ERROR: la direccion del Archivo Solicitado NO existe<<<<\n");
        return;
    }

    //La Direccion del Archivo SI fue encontrada
    //Se busca al archivo y se extrae el ByteInicial del Inodo

    if(TipoPart==1)
        ByteCarpetaArchivo = DevolverByteInicialCarpetaArchivo(ByteInodoPadre,NombreArchCarp,DISCO,Primaria->ByteInicial,2);
    else if(TipoPart==2)
        ByteCarpetaArchivo = DevolverByteInicialCarpetaArchivo(ByteInodoPadre,NombreArchCarp,DISCO,Logica->ByteInicial,2);


    //Validando que el Archivo o Carpeta Exita
    if(ByteCarpetaArchivo==-1)
    {
        if(TotalCarpetas!=1)
            free(DireccionArchCarp);
        free(SuperBlo);
        printf(">>>>ERROR: el archivo o carpeta solicitado no Existen dentro de la direccion introducida<<<<\n");
        return;
    }

    //El Archivo Si Existe se extrae el Inodo
    struct TablaInodos *InodoArchCarp;
    InodoArchCarp = malloc(sizeof(struct TablaInodos));
    fseek(DISCO,ByteCarpetaArchivo,SEEK_SET);
    fread(InodoArchCarp,sizeof(struct TablaInodos),1,DISCO);

    //El Archivo SI existe se extrae el Blque de Carpetas Padre
    if(TipoPart==1)
        ByteBloquePadre = DevolverByteInicialCarpetaArchivo(ByteInodoPadre,NombreArchCarp,DISCO,Primaria->ByteInicial,1);
    else if(TipoPart==2)
        ByteBloquePadre = DevolverByteInicialCarpetaArchivo(ByteInodoPadre,NombreArchCarp,DISCO,Logica->ByteInicial,1);

    //Se extrae el Bloque de Carpetas Padre del Inodo del archivo o capeta
    struct BloqueCarpetas *BloquePadre;
    BloquePadre = malloc(sizeof(struct BloqueCarpetas));
    fseek(DISCO,ByteBloquePadre,SEEK_SET);
    fread(BloquePadre,sizeof(struct BloqueCarpetas),1,DISCO);

    char BitmapIn[SuperBlo->TotalInodos];
    //Se Busca en la Carpeta Padre el numero de Inodo del archivo o carpeta y se modifica en el Bitmap de Inodos
    Indice=0;
    while(Indice < 4)
    {
        if(BloquePadre->ApuntadorCarpeta[Indice].Apuntador != -1)
        {
            if(strcmp(BloquePadre->ApuntadorCarpeta[Indice].Nombre,NombreArchCarp)==0)
            {
                //Se Modifica el Bitmap de Inodos

                fseek(DISCO,SuperBlo->InicioBitmapInodos,SEEK_SET);
                fread(BitmapIn,SuperBlo->TotalInodos,1,DISCO);
                BitmapIn[BloquePadre->ApuntadorCarpeta[Indice].Apuntador -1] = '0';
                fseek(DISCO,SuperBlo->InicioBitmapInodos,SEEK_SET);
                fwrite(BitmapIn,SuperBlo->TotalInodos,1,DISCO);
                Indice=4;

            }
        }
        Indice++;
    }


    //Se busca cuantos inodos y cuantos bloques tiene asociado el inodo que se va a borrar

    TotalBloques = 0;
    //Verificando si el Inodo  borrar es archivo o carpeta
    if(InodoArchCarp->Tipo[0]=='1')
    {
        //ES un Archivo
        //Actulizando el bitmap de Bloques
        Indice =0;
        while(Indice < 15)
        {
            if(Indice==12)
            {
                //Primer Apuntador Indirecto
            }else if(Indice==13)
            {
                //Apuntador doble indirecto
            }else if(Indice==14)
            {
                //Apuntador Triple Indirecto
            }else
            {
                //Apuntadores Directos
                if(InodoArchCarp->Apuntadores[Indice] != -1)
                {
                    TotalBloques++;

                    //Actualizando el bitmap de Bloques
                    char BitmapBlo[SuperBlo->TotalBloques];
                    fseek(DISCO,SuperBlo->InicioBitmapBloques,SEEK_SET);
                    fread(BitmapBlo,SuperBlo->TotalBloques,1,DISCO);
                    BitmapBlo[InodoArchCarp->Apuntadores[Indice] -1] ='0';
                    fseek(DISCO,SuperBlo->InicioBitmapBloques,SEEK_SET);
                    fwrite(BitmapBlo,SuperBlo->TotalBloques,1,DISCO);

                }

            }
            Indice++;

        }


    }else
    {

        //ES una Carpeta
        int IndiceCarpetas;
        for(Indice =0; Indice < 15; Indice++)
        {

            if(InodoArchCarp->Apuntadores[Indice]!=-1)
            {
                //El apuntador si apunta a un bloque
                BytepBloqueHijo = SuperBlo->InicioTablaBloques;
                BytepBloqueHijo =  BytepBloqueHijo + ((InodoArchCarp->Apuntadores[Indice]-1) * sizeof(struct BloqueCarpetas));

                if(Indice==12)
                {
                    //Primer apuntador Indirecto

                }else if(Indice==13)
                {
                            //Apuntador doble Indirecto

                }else if(Indice==14)
                {
                            //Apuntador Triple Indirecto

                }else
                {
                    //Apuntadores DIRECTO
                    TotalBloques++;

                    //Actualizando el bitmap de Bloques
                    char BitmapBlo[SuperBlo->TotalBloques];
                    fseek(DISCO,SuperBlo->InicioBitmapBloques,SEEK_SET);
                    fread(BitmapBlo,SuperBlo->TotalBloques,1,DISCO);
                    BitmapBlo[InodoArchCarp->Apuntadores[Indice] -1] ='0';
                    fseek(DISCO,SuperBlo->InicioBitmapBloques,SEEK_SET);
                    fwrite(BitmapBlo,SuperBlo->TotalBloques,1,DISCO);

                    struct BloqueCarpetas *BloqueCarp;

                    //Extrallendo el Bloque de Carpetas
                    BloqueCarp = malloc(sizeof(struct BloqueCarpetas));
                    fseek(DISCO,BytepBloqueHijo,SEEK_SET);
                    fread(BloqueCarp,sizeof(struct BloqueCarpetas),1,DISCO);

                    IndiceCarpetas=0;
                    //Vericando los apuntadores del bloque Carpetas
                    while( (IndiceCarpetas < 4) )
                    {
                        if(BloqueCarp->ApuntadorCarpeta[IndiceCarpetas].Apuntador != -1)
                        {
                            //Se manda a evaluar todo lo que Hay dentro de este Bloque de Carpetas
                            //Eliminando todo el Contenido del nuevo Disco
                            if((Indice !=0) || ((IndiceCarpetas!=0) && (IndiceCarpetas!=1)))
                            {
                                //Se excluyen los dos primero apuntadores del primer Bloque ya que estos
                                //Apuntan al inodo padre de la carpeta y la inodo propio de la carpeta
                                ByteEnviar = BloqueCarp->ApuntadorCarpeta[IndiceCarpetas].Apuntador;
                                ByteEnviar = SuperBlo->InicioTablaInodos + ((ByteEnviar -1)*(sizeof(struct TablaInodos)));
                                TotalBloques = TotalBloques + BorrarContenidoCarpeta(ByteEnviar,SuperBlo,DISCO,BloqueCarp->ApuntadorCarpeta[IndiceCarpetas].Apuntador-1);
                            }
                        }
                        IndiceCarpetas++;
                    }
                    free(BloqueCarp);


                }

            }
        }


    }

     //Apuntado el apuntador padre -1 para indicarle que esta libre(borrada)
    Indice=0;
    while(Indice < 4)
    {
        if(BloquePadre->ApuntadorCarpeta[Indice].Apuntador != -1)
        {
            if(strcmp(BloquePadre->ApuntadorCarpeta[Indice].Nombre,NombreArchCarp)==0)
            {
                //El Apuntador del Bloque o Archivo a borrar
                BloquePadre->ApuntadorCarpeta[Indice].Apuntador = -1;
                Indice=4;  //Para que Salga de While
            }

        }
        Indice++;

    }

    //Se Actualiza el Bloque De Carpetas Padre
    struct BloqueCarpetas NuevaCarp;
    Indice = 0;
    while(Indice < 4)              //Se Inicializa el nuevo Bloque de Carpetas Padre
    {
        NuevaCarp.ApuntadorCarpeta[Indice].Apuntador = -1;
        Indice++;
    }

    Indice=0;
    while(Indice<4)
    {
        if(BloquePadre->ApuntadorCarpeta[Indice].Apuntador != -1)
        {
            NuevaCarp.ApuntadorCarpeta[Indice].Apuntador = BloquePadre->ApuntadorCarpeta[Indice].Apuntador;
            strcpy(NuevaCarp.ApuntadorCarpeta[Indice].Nombre,BloquePadre->ApuntadorCarpeta[Indice].Nombre);
        }
        Indice++;
    }
    InsertarBloqueCarpetas(&NuevaCarp,DISCO,ByteBloquePadre);

    if(TotalCarpetas!=1)
        free(DireccionArchCarp);

    //Obtiendo de nuevo el Bitmap de Inodos para ver cuantos inodos fueron eliminados
    char BitmapInUltimo[SuperBlo->TotalInodos];
    fseek(DISCO,SuperBlo->InicioBitmapInodos,SEEK_SET);
    fread(BitmapInUltimo,SuperBlo->TotalInodos,1,DISCO);

    TotalInodos = 0;
    Indice = 0;
    TotalCarpetas = SuperBlo->TotalInodos;
    while(Indice < TotalCarpetas)
    {
        if(BitmapIn[Indice] != BitmapInUltimo[Indice])
            TotalInodos++;

        Indice++;
    }

    //Actualizando el superBloque
    SuperBlo->InodosLibres = SuperBlo->InodosLibres + TotalInodos+1;   //Mas uno por el inodo propio del archivo carpeta
    SuperBlo->BloquesLibres = SuperBlo->BloquesLibres + TotalBloques;
    Indice=0;
    while(Indice < TotalCarpetas)
    {
        if(BitmapInUltimo[Indice]=='0')
        {
            SuperBlo->PrimerInodoLibre = Indice+1;
            Indice=TotalCarpetas;   //para Salir del While
        }
        Indice++;
    }

    Indice = 0;
    TotalBloques = SuperBlo->TotalBloques;
    char BitmapBloUltimo[TotalBloques];;
    fseek(DISCO,SuperBlo->InicioBitmapBloques,SEEK_SET);
    fread(BitmapBloUltimo,TotalBloques,1,DISCO);
    while(Indice < TotalBloques)
    {
        if(BitmapBloUltimo[Indice]=='0')
        {
            SuperBlo->PrimerBloqueLibre = Indice+1;
            Indice= TotalBloques; //Para que salga del while
        }
        Indice++;
    }
    struct SuperBloque NuevoSuper;
    NuevoSuper.BloquesLibres =SuperBlo->BloquesLibres;
    NuevoSuper.InicioBitmapBloques = SuperBlo->InicioBitmapBloques;
    NuevoSuper.InicioBitmapInodos = SuperBlo->InicioBitmapInodos;
    NuevoSuper.InicioTablaBloques = SuperBlo->InicioTablaBloques;
    NuevoSuper.InicioTablaInodos = SuperBlo->InicioTablaInodos;
    NuevoSuper.InodosLibres = SuperBlo->InodosLibres;
    NuevoSuper.PrimerBloqueLibre = SuperBlo->PrimerBloqueLibre;
    NuevoSuper.PrimerInodoLibre = SuperBlo->PrimerInodoLibre;
    NuevoSuper.SistemaArchivos = SuperBlo->SistemaArchivos;
    NuevoSuper.TamanoBloque = SuperBlo->TamanoBloque;
    NuevoSuper.TamanoInodo = SuperBlo->TamanoInodo;
    NuevoSuper.TotalBloques = SuperBlo->TotalBloques;
    NuevoSuper.TotalInodos = SuperBlo->TotalInodos;
    NuevoSuper.UltimaFechaDesmont = SuperBlo->UltimaFechaDesmont;
    NuevoSuper.UltimaFechaMontura = SuperBlo->UltimaFechaMontura;
    NuevoSuper.VecesMontado = SuperBlo->VecesMontado;
    if(TipoPart==1)
        InsertarSuperBloque(&NuevoSuper,DISCO,Primaria->ByteInicial);
    else if(TipoPart==2)
        InsertarSuperBloque(&NuevoSuper,DISCO,Logica->ByteInicial);

    printf("Eliminacion de la Carpeta Realizada correctamente\n");

    //Se Extrae el Inodo pardre para modifcarle su fecha de modificacion
    CambiarFechaInodo(DISCO,ByteInodoPadre,3);


    free(SuperBlo);
    free(InodoArchCarp);
    free(BloquePadre);
    free(NombreArchCarp);






}
//************************************************************************************************************************************************************************************************************************
//************************************************************************************************************************************************************************************************************************
bool ExisteNombreRepetido(struct SuperBloque *SuperBlo,int ByteIndo, char *NombreNuevo, FILE *Disco)
{
    bool Estado;
    int Indice,Indice2;
    int ByteCarpeta;

    struct TablaInodos *InodoBusqueda;
    InodoBusqueda = malloc(sizeof(struct TablaInodos));
    fseek(Disco,ByteIndo,SEEK_SET);
    fread(InodoBusqueda,sizeof(struct TablaInodos),1,Disco);

    Estado = false;
    Indice = 0;
    while(Indice < 15)
    {
        if(InodoBusqueda->Apuntadores[Indice] != -1)
        {
            if(Indice==12)
            {
                //Primer Apuntador Indirecto
            }else if(Indice==13)
            {
                //Appuntador Indirecto Doble
            }else if(Indice==14)
            {
                //Apuntador triple Indirecto;

            }else
            {
                //Apuntadores Directos

                //Obteniendo el bloque de carpetas
                ByteCarpeta = InodoBusqueda->Apuntadores[Indice] -1;
                ByteCarpeta = SuperBlo->InicioTablaBloques + (ByteCarpeta * sizeof(struct BloqueCarpetas));
                struct BloqueCarpetas *CarpetaBusq;
                CarpetaBusq = malloc(sizeof(struct BloqueCarpetas));
                fseek(Disco,ByteCarpeta,SEEK_SET);
                fread(CarpetaBusq,sizeof(struct BloqueCarpetas),1,Disco);

                Indice2 = 0;
                while(Indice2 < 4)
                {
                    if(CarpetaBusq->ApuntadorCarpeta[Indice2].Apuntador != -1)
                    {
                        if((Indice!=0) || ((Indice2!=0) && (Indice2 !=1)))
                        {
                            if(strcmp(CarpetaBusq->ApuntadorCarpeta[Indice2].Nombre,NombreNuevo)==0)
                            {
                                Estado = true;
                                Indice2;
                                Indice=15;
                            }
                        }
                    }
                    Indice2++;
                }


                free(CarpetaBusq);
            }

        }
        Indice++;
    }

    free(InodoBusqueda);
    return Estado;   //Si RETORNA TRUE el nombre SI EXISTE DENTRO DE LA CARPETA
}

//*********************************METODO PARA CAMBIAR EL NOMBRE A UN ARCHIVO O CARPETA **************************************************************************************************************************************************
//*********************************************************************************************************************************************************************************************************************************
void MetodoCambiarNombre(struct Particion *Primaria, struct EBR *Logica, int TipoPart,char *Direccion,FILE *DISCO, char *NuevoNombre)
{
    int TotalCarpetas,ByteInodoPadre,ByteCarpetaArchivo,ByteBloquePadre,Indice,BytepBloqueHijo,Indice2;
    int ByteEnviar;
    char *DireccionArchCarp,*NombreArchCarp;
    struct SuperBloque *SuperBlo;

    TotalCarpetas = NumeroCarpetas(Direccion);
    SuperBlo = malloc(sizeof(struct SuperBloque));
    //Obteniendo el Numero de Carpetas que tiene la direccion
    if(TotalCarpetas==-1)
    {
        printf(">>>>ERROR: la direccion para crear carpetas NO es válida<<<<\n");
        free(SuperBlo);
        return;
    }

    if(TipoPart==1)
    {
        if(TotalCarpetas==1)
        {   //Cuando el Archivo esta en la raiz
            ByteInodoPadre = BuscarCarpetaContenedora(Primaria,NULL,"/",DISCO,1);
            Direccion++;
            NombreArchCarp = malloc(strlen(Direccion));
            strcpy(NombreArchCarp,Direccion);
            fseek(DISCO,Primaria->ByteInicial,SEEK_SET);
            fread(SuperBlo,sizeof(struct SuperBloque),1,DISCO);
        }else
        {
            DireccionArchCarp = DevolverDireccionDeCarpeta(Direccion,TotalCarpetas);
            NombreArchCarp = DevolverNombreCarpeta(Direccion);
            ByteInodoPadre = BuscarCarpetaContenedora(Primaria,NULL,DireccionArchCarp,DISCO,1);
            fseek(DISCO,Primaria->ByteInicial,SEEK_SET);
            fread(SuperBlo,sizeof(struct SuperBloque),1,DISCO);

        }
    }
    else if(TipoPart==2)
    {
        if(TotalCarpetas==1)
        {   //Cuando el Archivo esta en la raiz
            ByteInodoPadre = BuscarCarpetaContenedora(NULL,Logica,"/",DISCO,2);
            Direccion++;
            NombreArchCarp = malloc(strlen(Direccion));
            strcpy(NombreArchCarp,Direccion);
            fseek(DISCO,Logica->ByteInicial,SEEK_SET);
            fread(SuperBlo,sizeof(struct SuperBloque),1,DISCO);

        }else
        {
            DireccionArchCarp = DevolverDireccionDeCarpeta(Direccion,TotalCarpetas);
            NombreArchCarp = DevolverNombreCarpeta(Direccion);
            ByteInodoPadre = BuscarCarpetaContenedora(NULL,Logica,DireccionArchCarp,DISCO,2);
            fseek(DISCO,Logica->ByteInicial,SEEK_SET);
            fread(SuperBlo,sizeof(struct SuperBloque),1,DISCO);
        }
    }

    //SE empieza a buscar el Archivo en su inodo Padre
    if(ByteInodoPadre ==-1)
    {
        //La Direccion del Archivo no fue Encontrada
        if(TotalCarpetas!=1)
            free(DireccionArchCarp);

        free(SuperBlo);
        free(NombreArchCarp);
        printf(">>>>ERROR: la direccion del Archivo Solicitado NO existe<<<<\n");
        return;
    }

    //La Direccion del Archivo SI fue encontrada
    //Se busca al archivo y se extrae el ByteInicial del Inodo

    if(TipoPart==1)
        ByteCarpetaArchivo = DevolverByteInicialCarpetaArchivo(ByteInodoPadre,NombreArchCarp,DISCO,Primaria->ByteInicial,2);
    else if(TipoPart==2)
        ByteCarpetaArchivo = DevolverByteInicialCarpetaArchivo(ByteInodoPadre,NombreArchCarp,DISCO,Logica->ByteInicial,2);

    //Validando que el nombre Nuevo no exista dentro de la carpeta padre
    if(ExisteNombreRepetido(SuperBlo,ByteInodoPadre,NuevoNombre,DISCO))
    {
        //El nuevo nombre SI existe dentro de la carpeta
        if(TotalCarpetas!=1)
            free(DireccionArchCarp);

        free(SuperBlo);
        free(NombreArchCarp);
        printf(">>>>ERROR: el nombre nuevo que desea colocar ya existe dentro de la carpeta, cambie el nuevo nombre<<<<\n");
        return;
    }
    //Validando que el Archivo o Carpeta Exita
    if(ByteCarpetaArchivo==-1)
    {
        if(TotalCarpetas!=1)
            free(DireccionArchCarp);
        free(SuperBlo);
        printf(">>>>ERROR: el archivo o carpeta solicitado no Existen dentro de la direccion introducida<<<<\n");
        return;
    }

    //El Archivo Si Existe se extrae el Inodo
    struct TablaInodos *InodoArchCarp;
    InodoArchCarp = malloc(sizeof(struct TablaInodos));
    fseek(DISCO,ByteCarpetaArchivo,SEEK_SET);
    fread(InodoArchCarp,sizeof(struct TablaInodos),1,DISCO);

    //El Archivo SI existe se extrae el Blque de Carpetas Padre
    if(TipoPart==1)
        ByteBloquePadre = DevolverByteInicialCarpetaArchivo(ByteInodoPadre,NombreArchCarp,DISCO,Primaria->ByteInicial,1);
    else if(TipoPart==2)
        ByteBloquePadre = DevolverByteInicialCarpetaArchivo(ByteInodoPadre,NombreArchCarp,DISCO,Logica->ByteInicial,1);

    //Se extrae el Bloque de Carpetas Padre del Inodo del archivo o capeta
    struct BloqueCarpetas *BloquePadre;
    BloquePadre = malloc(sizeof(struct BloqueCarpetas));
    fseek(DISCO,ByteBloquePadre,SEEK_SET);
    fread(BloquePadre,sizeof(struct BloqueCarpetas),1,DISCO);

    //Se Busca en la Carpeta Padre la carpeta o archivo y se modifica su nombre
    Indice=0;
    while(Indice < 4)
    {
        if(BloquePadre->ApuntadorCarpeta[Indice].Apuntador != -1)
        {
            if(strcmp(BloquePadre->ApuntadorCarpeta[Indice].Nombre,NombreArchCarp)==0)
            {
                //Se modifica su nombre
                strcpy(BloquePadre->ApuntadorCarpeta[Indice].Nombre,NuevoNombre);
                Indice=4;
            }
        }
        Indice++;
    }


    //Se busca cuantos inodos y bloques hijos asociados a la carpeta o archivo


    //Verificando si el inodo es una carpeta, si es archivo no se toma encuenta ya que este ya ha sido modificado
    //arriba en el bloque de carpetas padre
    if(InodoArchCarp->Tipo[0]=='0')
    {
        //ES una Carpeta
        Indice=0;
        int IndiceCarpetas;
        for(Indice =0; Indice < 15; Indice++)
        {

            if(InodoArchCarp->Apuntadores[Indice]!=-1)
            {
                //El apuntador si apunta a un bloque
                BytepBloqueHijo = SuperBlo->InicioTablaBloques;
                BytepBloqueHijo =  BytepBloqueHijo + ((InodoArchCarp->Apuntadores[Indice]-1) * sizeof(struct BloqueCarpetas));

                if(Indice==12)
                {
                    //Primer apuntador Indirecto

                }else if(Indice==13)
                {
                            //Apuntador doble Indirecto

                }else if(Indice==14)
                {
                            //Apuntador Triple Indirecto

                }else
                {
                    //Apuntadores DIRECTO
                    struct BloqueCarpetas *BloqueCarp;

                    //Extrallendo el Bloque de Carpetas
                    BloqueCarp = malloc(sizeof(struct BloqueCarpetas));
                    fseek(DISCO,BytepBloqueHijo,SEEK_SET);
                    fread(BloqueCarp,sizeof(struct BloqueCarpetas),1,DISCO);

                    IndiceCarpetas=0;
                    //Vericando los apuntadores del bloque Carpetas
                    while( (IndiceCarpetas < 4) )
                    {
                        if(BloqueCarp->ApuntadorCarpeta[IndiceCarpetas].Apuntador != -1)
                        {


                            //Se modifica el nombre del carpeta el primer apuntador carpeta y en primer apuntador
                            //de esta
                            if((Indice==0) && (IndiceCarpetas==0))
                            {
                                strcpy(BloqueCarp->ApuntadorCarpeta[IndiceCarpetas].Nombre,NuevoNombre);

                                //Guardando el nuevo bloque de Carpetas
                                struct BloqueCarpetas NuevaCarpetaHijo;
                                Indice2 = 0;
                                while(Indice2 < 4)
                                {
                                    NuevaCarpetaHijo.ApuntadorCarpeta[Indice2].Apuntador = -1;
                                    Indice2++;
                                }

                                Indice2 = 0;
                                while(Indice2 < 4)
                                {
                                    if(BloqueCarp->ApuntadorCarpeta[Indice2].Apuntador != -1)
                                    {
                                        NuevaCarpetaHijo.ApuntadorCarpeta[Indice2].Apuntador = BloqueCarp->ApuntadorCarpeta[Indice2].Apuntador;
                                        strcpy(NuevaCarpetaHijo.ApuntadorCarpeta[Indice2].Nombre,BloqueCarp->ApuntadorCarpeta[Indice2].Nombre);
                                    }
                                    Indice2++;
                                }

                                InsertarBloqueCarpetas(&NuevaCarpetaHijo,DISCO,BytepBloqueHijo);

                            }

                            //Se manda a evaluar todo lo que Hay dentro de este Bloque de Carpetas
                            if((Indice !=0) || ((IndiceCarpetas!=0) && (IndiceCarpetas!=1)))
                            {
                                //Se excluyen el segundo apuntador del primer Bloque ya que este
                                //Apunta al inodo padre de la carpeta
                                ByteEnviar = BloqueCarp->ApuntadorCarpeta[IndiceCarpetas].Apuntador;
                                ByteEnviar = SuperBlo->InicioTablaInodos + ((ByteEnviar -1)*(sizeof(struct TablaInodos)));

                                //Se extrae el Inodo Hijo se verifica si es una carpeta
                                struct TablaInodos *InodoHijo;
                                InodoHijo = malloc(sizeof(struct TablaInodos));
                                fseek(DISCO,ByteEnviar,SEEK_SET);
                                fread(InodoHijo,sizeof(struct TablaInodos),1,DISCO);
                                //Se Verfica si el el Inodo es Carpeta
                                if(InodoHijo->Tipo[0]=='0')
                                {
                                    //Se una Carpeta
                                    //Se modifica la fecha de modificacion del inodo
                                    CambiarFechaInodo(DISCO,ByteEnviar,3);

                                    //Se Verifica el primer apuntador para cambiar el nombre en el segundo apuntador del bloque
                                    //De carpetas
                                    ByteEnviar = InodoHijo->Apuntadores[0] -1;
                                    ByteEnviar = SuperBlo->InicioTablaBloques + ((ByteEnviar)*(sizeof(struct BloqueCarpetas)));


                                    struct BloqueCarpetas *CarpetaHijo;
                                    CarpetaHijo = malloc(sizeof(struct BloqueCarpetas));
                                    fseek(DISCO,ByteEnviar,SEEK_SET);
                                    fread(CarpetaHijo,sizeof(struct BloqueCarpetas),1,DISCO);

                                    strcpy(CarpetaHijo->ApuntadorCarpeta[1].Nombre,NuevoNombre);

                                    //Guardando el nuevo bloque de Carpetas
                                    struct BloqueCarpetas NuevaCarpetaHijo;
                                    Indice2 = 0;
                                    while(Indice2 < 4)
                                    {
                                        NuevaCarpetaHijo.ApuntadorCarpeta[Indice2].Apuntador = -1;
                                        Indice2++;
                                    }

                                    Indice2 = 0;
                                    while(Indice2 < 4)
                                    {
                                        if(CarpetaHijo->ApuntadorCarpeta[Indice2].Apuntador != -1)
                                        {
                                            NuevaCarpetaHijo.ApuntadorCarpeta[Indice2].Apuntador = CarpetaHijo->ApuntadorCarpeta[Indice2].Apuntador;
                                            strcpy(NuevaCarpetaHijo.ApuntadorCarpeta[Indice2].Nombre,CarpetaHijo->ApuntadorCarpeta[Indice2].Nombre);
                                        }
                                        Indice2++;
                                    }

                                    InsertarBloqueCarpetas(&NuevaCarpetaHijo,DISCO,ByteEnviar);
                                    free(CarpetaHijo);
                                }
                                free(InodoHijo);

                            }
                        }
                        IndiceCarpetas++;
                    }
                    free(BloqueCarp);


                }

            }
        }


    }


    //Se Actualiza el Bloque De Carpetas Padre
    struct BloqueCarpetas NuevaCarp;
    Indice = 0;
    while(Indice < 4)              //Se Inicializa el nuevo Bloque de Carpetas Padre
    {
        NuevaCarp.ApuntadorCarpeta[Indice].Apuntador = -1;
        Indice++;
    }

    Indice=0;
    while(Indice<4)
    {
        if(BloquePadre->ApuntadorCarpeta[Indice].Apuntador != -1)
        {
            NuevaCarp.ApuntadorCarpeta[Indice].Apuntador = BloquePadre->ApuntadorCarpeta[Indice].Apuntador;
            strcpy(NuevaCarp.ApuntadorCarpeta[Indice].Nombre,BloquePadre->ApuntadorCarpeta[Indice].Nombre);
        }
        Indice++;
    }
    InsertarBloqueCarpetas(&NuevaCarp,DISCO,ByteBloquePadre);


    if(TotalCarpetas!=1)
        free(DireccionArchCarp);


    //Actualizando el superBloque

    printf("Cambio de nombre al Archivo: %s realizado correctamente\n",NombreArchCarp);

    //Se Extrae el Inodo pardre para modifcarle su fecha de modificacion
    CambiarFechaInodo(DISCO,ByteInodoPadre,3);


    free(SuperBlo);
    free(InodoArchCarp);
    free(BloquePadre);
    free(NombreArchCarp);
}

//******** METODO PARA BUSCAR LA PARTICION PARA LA ELIMINACION O CAMBIO DE NOMBRE DE UN ARCHIVO O CARPETA************************************************************************************************************
//*******************************************************************************************************************************************************************************************************************
void EliminarCarpetaArchivoCambiar(char *Id, char *Direccion, char *NuevoNombre, int TipoOp)
{
    //Si TipoOp = 1 ---->> Significa una eliminacion    Tipo=2 -->> Significa Cambio de Nombre
    struct MBR *EstrucMBR;

    char DiscoDireccion[125];
    char *NombreParticion;
    NombreParticion = malloc(100*sizeof(char));
    strcpy(NombreParticion,Devolverparticion(Id));
    strcpy(DiscoDireccion,DevolverPath(Id));

    if((strcasecmp(DiscoDireccion,"a"))!=0)
    {
        //El id ya se ha montado
        FILE *DiscoLeer;
        DiscoLeer = fopen(DiscoDireccion,"rb+");
        if(DiscoLeer==NULL)
        {
            printf(">>>>ERROR: direccion de almacenamiento no valida <<<<\n");
            return;
        }

        EstrucMBR = malloc(4*sizeof(struct MBR)+1);
        fseek(DiscoLeer,0,SEEK_SET);
        fread(EstrucMBR,sizeof(struct MBR),1,DiscoLeer);


        //Buscando la Particion a grafica

        if((strcmp(EstrucMBR->Particion1.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion1.Tipo,"p")==0) )
        {
            if(TipoOp==1)
            {
                //Eliminacion
                MetodoDeEliminacion(&EstrucMBR->Particion1,NULL,1,Direccion,DiscoLeer);
            }else if(TipoOp==2)
            {
                //Cambiar Nombre
                MetodoCambiarNombre(&EstrucMBR->Particion1,NULL,1,Direccion,DiscoLeer,NuevoNombre);
            }


        }else if((strcmp(EstrucMBR->Particion2.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion2.Tipo,"p")==0) )
        {
            if(TipoOp==1)
            {
                //Eliminar
                MetodoDeEliminacion(&EstrucMBR->Particion2,NULL,1,Direccion,DiscoLeer);

            }else if(TipoOp==2)
            {
                //Cambiar Nombre
                MetodoCambiarNombre(&EstrucMBR->Particion2,NULL,1,Direccion,DiscoLeer,NuevoNombre);
            }

        }else if( (strcmp(EstrucMBR->Particion3.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion3.Tipo,"p")==0) )
        {
            if(TipoOp==1)
            {
                //EliminarArchivo
                MetodoDeEliminacion(&EstrucMBR->Particion3,NULL,1,Direccion,DiscoLeer);

            }else if(TipoOp==2)
            {
                //Cambiar Nombre
                MetodoCambiarNombre(&EstrucMBR->Particion3,NULL,1,Direccion,DiscoLeer,NuevoNombre);
            }

        }else if( (strcmp(EstrucMBR->Particion4.Nombre,NombreParticion)==0) && (strcasecmp(EstrucMBR->Particion4.Tipo,"p")==0) )
        {
            if(TipoOp==1)
            {
                //Eliminar Archivo
                MetodoDeEliminacion(&EstrucMBR->Particion4,NULL,1,Direccion,DiscoLeer);
            }else if(TipoOp==2)
            {
                //Cambiar Nombre
                MetodoCambiarNombre(&EstrucMBR->Particion4,NULL,1,Direccion,DiscoLeer,NuevoNombre);
            }

        }else if(strcasecmp(EstrucMBR->Particion1.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion1.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: la partcion no fue Encontrada dentro del disco<<<<\n");
            else
            {
                if(TipoOp==1)
                {
                    //Eliminar
                    MetodoDeEliminacion(NULL,EbrRecibido,2,Direccion,DiscoLeer);

                }else if(TipoOp==2)
                {
                    //Cambiar Nombre
                    MetodoCambiarNombre(NULL,EbrRecibido,2,Direccion,DiscoLeer,NuevoNombre);
                }
                free(EbrRecibido);
            }

        }else if(strcasecmp(EstrucMBR->Particion2.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion2.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: la partcion no fue Encontrada dentro del disco<<<<\n");
            else
            {
                if(TipoOp==1)
                {
                    //Eliminar
                    MetodoDeEliminacion(NULL,EbrRecibido,2,Direccion,DiscoLeer);

                }else if(TipoOp==2)
                {
                    //Cambiar Nombre
                    MetodoCambiarNombre(NULL,EbrRecibido,2,Direccion,DiscoLeer,NuevoNombre);
                }
                free(EbrRecibido);
            }

        }else if(strcasecmp(EstrucMBR->Particion3.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion3.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: la partcion no fue Encontrada dentro del disco<<<<\n");
            else
            {
                if(TipoOp==1)
                {
                    //Eliminar
                    MetodoDeEliminacion(NULL,EbrRecibido,2,Direccion,DiscoLeer);
                }else if(TipoOp==2)
                {
                    //Cambiar Nombre
                    MetodoCambiarNombre(NULL,EbrRecibido,2,Direccion,DiscoLeer,NuevoNombre);
                }
                free(EbrRecibido);
            }

        }else if(strcasecmp(EstrucMBR->Particion4.Tipo,"e")==0)
        {
            //Verificando si la particion solicitada es logica
            struct EBR *EbrRecibido;
            EbrRecibido = DevolverEBR(DiscoLeer,EstrucMBR->Particion4.ByteInicial,NombreParticion);
            if(EbrRecibido==NULL)
                printf(">>>>ERROR: la partcion no fue Encontrada dentro del disco<<<<\n");
            else
            {
                if(TipoOp==1)
                {
                    //Eliminar
                    MetodoDeEliminacion(NULL,EbrRecibido,2,Direccion,DiscoLeer);
                }else if(TipoOp==2)
                {
                    //Cambiar Nombre
                    MetodoCambiarNombre(NULL,EbrRecibido,2,Direccion,DiscoLeer,NuevoNombre);
                }
                free(EbrRecibido);
            }

        }else
            printf(">>>>ERROR: no se pudo realizar la operacion; la particion indicada no se encontro dentro del disco <<<<\n");

        fclose(DiscoLeer);
        free(EstrucMBR);


    }else
        printf(">>>>ERROR: imposible realizar Operacion Disco no encontrado<<<<\n");

}
