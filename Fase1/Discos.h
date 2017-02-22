/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Discos.h
 * Author: eduardo
 *
 * Created on February 5, 2016, 10:14 PM
 */

#ifndef DISCOS_H
#define DISCOS_H

#ifdef __cplusplus
extern "C" {
#endif

    extern void CrearDisco(char *Path, char *Size, char *Nombre, char *Capacidad);
    extern void EliminarDisco(char *Direccion);
    extern void CrearParticion(char *Direccion, char *Nombre, char *Tamano,char *Unit,char *Tipo,char *Fitt);
    extern void ModificarParticion(char *ByteModificacion,char *Direccion,char *Nombre,char *Unit);
    extern void EliminarParticion(char *Direccion,char *Nombre,char *TipoBorrado);
    extern void MontarParticion(char *Path,char *Nombre);
    extern void DesmontarParticion(char *IdParticion);
    extern void CrearReporteMBR(char *Direccion, char *id);
    extern void CrearReporteBitmapInodos(char *Direccion, char *Id);
    extern void CrearReporteBitmapBloques(char *Direccion, char *Id);
    extern void CrearReporteDisco(char *Direccion, char *id);
    extern void CrearReporteSuperBloque(char *Direccion, char *id);
    extern void RealizarFormateoFast(char *IdRec);
    extern void RealizarFormateoFull(char *IdRec);
    extern void CrearArchivoConDireccion(char *Id, char *Path, char *CrearCarpeta, char *Tamano, char *DirecCont, char Content[]);
    extern void CrearReporteBloquesInodo(char *Direccion, char *id, int TipoRe);
    extern void CrearCarpetaConDireccion(char *Id, char *Path, char *CrearCarpeta);
    extern void CrearReporteArbol(char *Direccion, char *IdRec);
    extern void CrearReporteFileLs(char *DireccionGrafica, char *IdRec, char *DireccionArchivo, int TipoGra, int ConsolaGrafica);
    extern void EliminarCarpetaArchivoCambiar(char *Id, char *Direccion, char *NuevoNombre, int TipoOp);
    extern void CrearEliminarGrupoUsuario(char *Id, char *NombreGrupo, char *NombreUsuario, char *Paswoord, int TipoOperacion,char Conten[],int Tamano);
    extern void LoguearUsuario(char *NombreUsuario, char *Contrasena);

#ifdef __cplusplus
}
#endif

#endif /* DISCOS_H */

