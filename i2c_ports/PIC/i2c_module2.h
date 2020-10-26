/* 
 * File:   MyI2C2.h
 * Author: Sabbiolino
 *
 * Created on 1 maggio 2016, 18.21
 */


#ifndef MYI2C2_H
#define	MYI2C2_H

#include "EEPROM/24XX1025.h"
#include "EEPROM/24XX16.h"

void write_protect_disable();
void write_protect_enable();

void Init_I2C(unsigned int brg);

unsigned int EEAckPolling(unsigned char control);
int findAddress();





#endif	/* MYI2C2_H */

