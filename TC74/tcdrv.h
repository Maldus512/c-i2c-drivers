/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: tcDrv.h                                                           */
/*                                                                            */
/*      gestione Serial Digital Thermal Sensor TC74                           */
/*                                                                            */
/*  Autore: Mattia MALDINI                                                    */
/*                                                                            */
/*  Data  : 16/09/2016      REV  : 00.0                                       */
/*                                                                            */
/*  U.mod.: 16/09/2016      REV  : 01.0                                       */
/*                                                                            */
/******************************************************************************/

#ifndef TCDRV_H
#define	TCDRV_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MAX_TEMP_LEN        5
    
int readTemperature();

extern int temperature;

extern char readableTemp[MAX_TEMP_LEN];



#ifdef	__cplusplus
}
#endif

#endif	/* TCDRV_H */
