#include "eeprom_common.h"
#include "i2c_driver.h"
#include "hardwareprofile.h"

#define WRITE_CB(x)     x & 0xFE
#define READ_CB(x)      x | 0x01


/*********************************************************************
 * Function:        EEAckPolling()
 *
 * Input:		Control byte.
 *
 * Output:		error state.
 *
 * Overview:		polls the bus for an Acknowledge from device
 *
 * Note:			None
 ********************************************************************/
unsigned int EEAckPolling(unsigned char c)
{
    startCondition(); //Generate Start condition
    unsigned long counter  = 0;
     
    
    masterWrite(c);

    while (readAck())
    {
        restartCondition(); //generate restart
        masterWrite(c);
        delay_us(100);
        if (counter++ > 100UL) {
            return -1;
        }
    }
    
    stopCondition(); //send stop condition
    return (0);
}


