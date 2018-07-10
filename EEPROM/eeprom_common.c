#include "i2c_driver.h"


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
unsigned int EEAckPolling(unsigned char control)
{
    idle(); //wait for bus Idle
    startCondition(); //Generate Start condition
    
    unsigned long counter  = 0;
     
    
    masterWrite(control);
    idle(); //wait for bus idle

    while (readAck())
    {
        restartCondition(); //generate restart
        if (I2C1STATbits.BCL)
        {
            return (-1); //error return
        }

        masterWrite(control);
        idle();

        if (counter++ > 100000UL) {
            return -1;
        }
    }
    
    stopCondition(); //send stop condition

    return (0);
}