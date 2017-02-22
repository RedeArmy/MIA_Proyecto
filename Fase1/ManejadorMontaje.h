/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ManejadorMontaje.h
 * Author: eduardo
 *
 * Created on February 15, 2016, 11:02 AM
 */

#ifndef MANEJADORMONTAJE_H
#define MANEJADORMONTAJE_H


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
    
    
    extern int InsertarNuevoDisco(char *Path);
    extern void GenerarIdParticion(int numero,char *NombrePar);
    extern void DesmontParticion(int NumeroParticion,char *IdParticion);
    extern char *DevolverPath(char *IdDisco);
    extern char *Devolverparticion(char *IdRec);
    extern char *DevolverPrimerDireccion();
    extern char *DevolverPrimerParticion();
#endif /* MANEJADORMONTAJE_H */


