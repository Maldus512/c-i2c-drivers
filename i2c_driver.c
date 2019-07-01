/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: I2C_drv.c                                                         */
/*                                                                            */
/*      gestione I2C                                                          */
/*                                                                            */
/*  Autore: Massimo Zanna                                                     */
/*                                                                            */
/*  Data  : 03/09/2007      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 23/05/2016      REV  : 01.0                                       */
/*                                                                            */
/******************************************************************************/

#include "hardwareprofile.h"

#include <string.h>
#include <libpic30.h>


void write_protect_enable() {
#ifdef WRITE_PROTECT
    WRITE_PROTECT = 1;
#endif
}

void write_protect_disable() {
#ifdef WRITE_PROTECT
    WRITE_PROTECT = 0;
#endif
}