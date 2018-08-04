#ifndef __EEPROM_COMMON_H__
#define __EEPROM_COMMON_H__

/* Funzioni che devono essere definite (come macro)
 * Per utilizzare l'implementazione di default di pageWrite
 * e blockRead
 * 
 * idle
 * startCondition
 * masterWrite
 * readAck
 * restartCondition
 * stopCondition
 * disableInt
 * enableInt
 * write_protect_disable
 * write_protect_enable
 * masterRead
 * writeAck
 * 
 */

void startCondition();
char masterWrite(unsigned char byte);
unsigned char readAck();
void restartCondition();
void stopCondition();
void disableInt();
void enableInt();
void write_protect_disable();
void write_protect_enable();
unsigned char masterRead();
void writeAck(unsigned char ack);

#endif