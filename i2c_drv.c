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

#include <string.h>

#include "HardwareProfile.h"
#include "variabili_parametri_sistema.h"

#include "I2C_drv.h"
#include "system.h"

#define TRUE    1
#define FALSE   0







// /*----------------------------------------------------------------------------*/
// /*  CK_I2C                                                                    */
// /*----------------------------------------------------------------------------*/
// void CK_I2C (unsigned char ck)
// {
//     CLK_I2C = ck; /* CLK I2C */
// }
// /*----------------------------------------------------------------------------*/
// /*  CK_I2C_b                                                                  */
// /*----------------------------------------------------------------------------*/
// void CK_I2C_b (unsigned char ck)
// {
//     CLK_I2C_B = ck; /* CLK I2C */
// }




/*----------------------------------------------------------------------------*/
/* CK_I2CK - RALLENTATORE CLK I2C board                                       */
/*----------------------------------------------------------------------------*/
void CK_I2C_b (unsigned char ck)
{
    CLK_I2C_B = ck;
    delay_us(5);
}





// 
// /*----------------------------------------------------------------------------*/
// /* Init_I2C                                                                   */
// /*----------------------------------------------------------------------------*/
// void Init_I2C (void)
// {
//     CLK_I2C         = 0;    /* CLK I2C  */
//     DATA_I2C        = 1;    /* DATI I2C */
//     
//     DD_CLK_I2C      = 1;    /* abilita la porta come uscita CK      */
//     DD_DATA_I2C     = 1;    /* abilita la porta come uscita DATI    */
//                             /* il pin deve essere riprogrammato per */
//                             /* leggere i dati in ingresso e ACK     */
//                             /* dall' I2C                            */
//                             
// //    DD_KEY_IN_I2C   = 0;    // IN KEY-D inserita
// }
// 
//





/*----------------------------------------------------------------------------*/
/*  Init_I2C_b                                                                */
/*----------------------------------------------------------------------------*/
void Init_I2C_b (void)
{
    CLK_I2C_B       = 1;    /* CLK I2C  */
    DATA_I2C_B      = 1;    /* DATI I2C */
    
    DD_CLK_I2C_B    = OUTPUT_PIN;   /* abilita la porta come uscita CK        */
    DD_DATA_I2C_B   = OUTPUT_PIN;   /* abilita la porta come uscita DATI      */
                                    /* il pin deve essere riprogrammato per   */
                                    /* leggere i dati in ingresso e ACK       */
                                    /* dall' I2C                              */
}




/*----------------------------------------------------------------------------*/
/*  I2C_Write_b                                                               */
/*----------------------------------------------------------------------------*/
int I2C_Write_b (unsigned char cDevAddr, unsigned char cRegAddr, const unsigned char* pData, int nLen)
{
    unsigned int x;
    unsigned int y;
    unsigned char ack;
    
    DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
    CK_I2C_b(1);
    CK_I2C_b(0);
    CK_I2C_b(1);
    DATA_I2C_B = 0;         /* DATI I2C */    /* start condition    */
    CK_I2C_b(1);
    CK_I2C_b(0);
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo dispositivo                                  */
    /*--------------------------------------------------------------*/
    for (x = 0; x < 8; x++)
    {
        CK_I2C_b(0);
        DATA_I2C_B = (cDevAddr >> (7 - x));
        CK_I2C_b(1);
    }
    
    CK_I2C_b(0);
    DD_DATA_I2C_B = 0;      /* dati in ingresso                     */
    CK_I2C_b(1);
    ack = DATA_I2C_B;
    
    if (ack != 0)
         return (-1);       /* busy                                 */
    
    CK_I2C_b(0);
    DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo registro                                     */
    /*--------------------------------------------------------------*/
    for (x = 0; x < 8; x++)
    {
        CK_I2C_b(0);
        DATA_I2C_B = (cRegAddr >> (7 - x));
        CK_I2C_b(1);
    }
    
    CK_I2C_b(0);
    DD_DATA_I2C_B = 0;        /* dati in ingresso                     */
    CK_I2C_b(1);
    ack = DATA_I2C_B;
    
    if (ack != 0)
         return (0);
    
    CK_I2C_b(0);
    
    /*--------------------------------------------------------------*/
    /* invio DATI al dispositivo                                    */
    /*--------------------------------------------------------------*/
    for (y = 0; y < nLen; y++)
    {
        DD_DATA_I2C_B = 1;  /* dati in uscita                       */
        
        for (x = 0; x < 8; x++)
        {
            CK_I2C_b(0);
            DATA_I2C_B = (*pData >> (7 - x));
            CK_I2C_b(1);
        }
        
        pData ++;
        CK_I2C_b(0);
        DD_DATA_I2C_B = 0;  /* dati in ingresso                     */
        CK_I2C_b(1);
        ack = DATA_I2C_B;
        
        if (ack)
            return (0);
        
        CK_I2C_b(0);
    }
    
    DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
    DATA_I2C_B = 0;         /* stop condition                       */
    CK_I2C_b(1);
    DATA_I2C_B = 1;         /* stop condition                       */
    CK_I2C_b(0);
    CK_I2C_b(1);
    
    return (1);
}







/*----------------------------------------------------------------------------*/
/*  I2C_Write_1024_b ....  @ CLK & DATA H                                     */
/*----------------------------------------------------------------------------*/
int I2C_Write_1024_b (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, const unsigned char* pData, unsigned int nLen)
{
    unsigned int x;
    unsigned int y;
    unsigned char ack;
    
    fsw_no_agg_dis  = 1; // -DDDD fermo altre funzioni !!!!
    
    
    
    DATA_I2C_B = 0;         /* DATI I2C */    /* start condition    */
    delay_us(3);
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo dispositivo @ cambio bit con CLK L           */
    /*--------------------------------------------------------------*/
    for (x = 0; x < 8; x++)
    {
        CK_I2C_b(0);
        delay_us(3);
        
        DATA_I2C_B = (cDevAddr >> (7 - x));
        delay_us(3);
        
        CK_I2C_b(1);
        delay_us(3);
    }
    CK_I2C_b(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;      /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C_b(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack != 0)
    {
        Nop();
        ack = DATA_I2C_I_B;
        delay_us(3);
    }
    
    CK_I2C_b(0);
    delay_us(3);
    
    DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
    delay_us(10);
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo registro h                                   */
    /*--------------------------------------------------------------*/
    for (x = 0; x < 8; x++)
    {
        CK_I2C_b(0);
        delay_us(3);
        
        DATA_I2C_B = (cRegAddr_h >> (7 - x));
        delay_us(3);
        
        CK_I2C_b(1);
        delay_us(3);
    }
    CK_I2C_b(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;        /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C_b(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack != 0)
    {
        Nop();
        ack = DATA_I2C_I_B;
        delay_us(3);
    }
    
    CK_I2C_b(0);
    delay_us(3);
    
    DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
    delay_us(10);
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo registro l                                   */
    /*--------------------------------------------------------------*/
    for (x = 0; x < 8; x++)
    {
        CK_I2C_b(0);
        delay_us(3);
        
        DATA_I2C_B = (cRegAddr_l >> (7 - x));
        delay_us(3);
        
        CK_I2C_b(1);
        delay_us(3);
    }
    CK_I2C_b(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;        /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C_b(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack != 0)
    {
        Nop();
        ack = DATA_I2C_I_B;
        delay_us(3);
    }
    
    CK_I2C_b(0);
    delay_us(3);
    
    /*--------------------------------------------------------------*/
    /* invio DATI al dispositivo                                    */
    /*--------------------------------------------------------------*/
    for (y = 0; y < nLen; y++)
    {
        DD_DATA_I2C_B = OUTPUT_PIN;  /* dati in uscita                       */
        delay_us(500);
        
        for (x = 0; x < 8; x++)
        {
            CK_I2C_b(0);
            delay_us(3);
            
            DATA_I2C_B = (*pData >> (7 - x));
            delay_us(3);
            
            CK_I2C_b(1);
            delay_us(3);
        }
        
        pData ++;
        
        CK_I2C_b(0);
        delay_us(3);
        
        DD_DATA_I2C_B = INPUT_PIN;  /* dati in ingresso                     */
        delay_us(500);
        
        CK_I2C_b(1);
        delay_us(3);
        
        ack = DATA_I2C_I_B;
        delay_us(3);
        
        while (ack != 0)
        {
            Nop();
            ack = DATA_I2C_I_B;
            delay_us(3);
        }
        Nop();
        Nop();
        Nop();
        Nop();
    }
    CK_I2C_b(0);
    delay_us(3);
    
    DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
    delay_us(50);
    
    DATA_I2C_B = 0;         /* stop condition                       */
    delay_us(3);
    
    CK_I2C_b(1);
    delay_us(3);
    
    DATA_I2C_B = 1;         /* stop condition                       */
    delay_us(3);
    
    fsw_no_agg_dis  = 0; // -DDDD riabilito altre funzioni
    
    return (1); // NB: ritorno con CLK e DATA H
}

void I2C_Write_1024_b2(unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, const unsigned char* pData, unsigned int nLen)
{
    // INTCON2bits.GIE = 0; // Disable global interrupt
    
    unsigned int addr = cRegAddr_h * 256 + cRegAddr_l;
    unsigned int i = 0;
    
    for(; i < nLen ; i++, addr++)
    {
        I2C_Write_1024_b(cDevAddr, addr >> 8, addr & 255, (pData + i), 1);
        delay_ms(3);
    }
    // INTCON2bits.GIE = 1; // Enable global interrupt
}


/*----------------------------------------------------------------------------*/
/* I2C_Read_b                                                                 */
/*----------------------------------------------------------------------------*/
int I2C_Read_b (unsigned char cDevAddr, unsigned char cRegAddr, unsigned char* buffer, int nLen)
{
//     return;
    
    unsigned int x;
    unsigned int y;
    unsigned char ack;
    
    Set_I2C_Add_b (cRegAddr, cDevAddr);
    DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
    
    CK_I2C_b(1);
    DATA_I2C_B = 0;         /* DATI I2C */    /* start condition    */
    CK_I2C_b(1);
    CK_I2C_b(0);
    cDevAddr |=1;
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo dispositivo                                  */
    /*--------------------------------------------------------------*/
    for (x = 0; x < 8; x++)
    {
        CK_I2C_b(0);
        DATA_I2C_B = (cDevAddr >> (7 - x));
        CK_I2C_b(1);
    }
    
    CK_I2C_b(0);
    DD_DATA_I2C_B = 0;      /* dati in ingresso                     */
    CK_I2C_b(1);
    ack = DATA_I2C_B;
    
    if (ack)
         return (-1);       /* busy                                 */
    
    CK_I2C_b(0);
    
    /*--------------------------------------------------------------*/
    /* Legge la ram del dispositivo                                 */
    /*--------------------------------------------------------------*/
    DD_DATA_I2C_B = 0;      /* dati in ingresso                     */
    
    for(y = 0; y < nLen; y++)
    {
        for (x = 0; x < 8; x++)
        {
            CK_I2C_b(1);
            *buffer = (*buffer << 1) | DATA_I2C_B; /* dato in input */
            CK_I2C_b(0);
        }
        
        buffer++;
        DD_DATA_I2C_B = 1;  /* dati in uscita dal micro             */
        DATA_I2C_B = 0;     /* invia ack                            */
        
        if (y == nLen -1)
            DATA_I2C_B = 1;
        
        CK_I2C_b(1);
        CK_I2C_b(0);
        DD_DATA_I2C_B = 0;  /* dati in ingesso                      */
    }
    
    DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
    DATA_I2C_B = 0;         /* DATI I2C */   /* stop condition      */
    CK_I2C_b(1);
    DATA_I2C_B = 1;         /* DATI I2C */   /* stop condition      */
    CK_I2C_b(0);
    CK_I2C_b(1);
    
    return (1);
}








/*----------------------------------------------------------------------------*/
/*  I2C_Read_1024_b .... @ CLK & DATA H                                       */
/*----------------------------------------------------------------------------*/
int I2C_Read_1024_b (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, unsigned char* buffer, unsigned int nLen)
{
    unsigned int x;
    unsigned int y;
    unsigned char ack;
    
    fsw_no_agg_dis  = 1; // -DDDD fermo altre cose
    
    
    
    Set_I2C_Add_512_b ((unsigned char) cRegAddr_h, (unsigned char) cRegAddr_l, (unsigned char) cDevAddr); // SET DEVICE ADDRESS RD .... RETURN @ CLK & DATA H
    
//     DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
//     delay_us(5);
//
//     CK_I2C_b(1);
//     delay_us(5);
    
    DATA_I2C_B = 0;         /* DATI I2C */    /* start condition    */
    delay_us(3);
    
//     CK_I2C_b(1);
//     delay_us(5);
//     
//     CK_I2C_b(0);
//     delay_us(5);
    
    cDevAddr |=1; // ...... predispongo il READ ...... sull' ADDRESS
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo dispositivo CON READ                         */
    /*--------------------------------------------------------------*/
    for (x = 0; x < 8; x++)
    {
        CK_I2C_b(0);
        delay_us(3);
        
        DATA_I2C_B = (cDevAddr >> (7 - x));
        delay_us(3);
        
        CK_I2C_b(1);
        delay_us(3);
    }
    
    CK_I2C_b(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;      /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C_b(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack !=0)
    {
        Nop ();
        ack = DATA_I2C_I_B;
        delay_us(3);
    }
    
    CK_I2C_b(0);
    delay_us(3);
    
    /*--------------------------------------------------------------*/
    /* Legge la ram del dispositivo x "nLen" volte                  */
    /*--------------------------------------------------------------*/
    for(y = 0; y < nLen; y++)
    {
        DD_DATA_I2C_B = INPUT_PIN;      /* dati in ingresso                     */
        delay_us(10);
        
        for (x = 0; x < 8; x++)
        {
            CK_I2C_b(0);
            delay_us(3);
            
            *buffer = (*buffer << 1) | DATA_I2C_I_B; /* dato in input */
            // dato = (dato << 1) | DATA_I2C_I_B; /* dato in input */
            delay_us(3);
            
            CK_I2C_b(1);
            delay_us(3);
        }
        CK_I2C_b(0);
        delay_us(3);
        
        
        
        if (y != nLen-1) // CP se ultimo BYTE da RD e s no 
        {
            CK_I2C_b(1);
            delay_us(3);
            
            while (ack !=0)
            {
                Nop ();
                ack = DATA_I2C_I_B;
                delay_us(3);
            }
            CK_I2C_b(0);
            delay_us(10);
            
            buffer++;
        }
        Nop();
        Nop();
        Nop();
        Nop();
    }
        

    CK_I2C_b(1);
    delay_us(3);
    
    DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
    delay_us(10);
    
    DATA_I2C_B = 1;         /* stop condition                       */
    delay_us(3);
    
  
    fsw_no_agg_dis  = 0; // -DDDD riabilito altre funzioni
    
    return (1); // NB: ritorno con CLK e DATA H
}

void I2C_Read_1024_b2(unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, unsigned char* pData, unsigned int nLen)
{
    //IEC0bits.T1IE = 0; // Enable Timer1 interrupt
    //INTCON2bits.DISI = 1; // Disable global interrupt
    //INTCON2bits.GIE = 0; // Disable global interrupt
    
    unsigned int addr = cRegAddr_h * 256 + cRegAddr_l;
    unsigned int i = 0;
    
    for(; i < nLen ; i++, addr++)
    {
        I2C_Read_1024_b(cDevAddr, addr >> 8, addr & 255, (pData + i), 1);
        // delay_us(20);
    }
    //INTCON2bits.DISI = 0; // Disable global interrupt
    //INTCON2bits.GIE = 1; // Enable global interrupt
    //IEC0bits.T1IE = 1; // Enable Timer1 interrupt
}



/*----------------------------------------------------------------------------*/
/*  Set_I2C_Add_b                                                             */
/*----------------------------------------------------------------------------*/
void Set_I2C_Add_b (unsigned char add, unsigned char slave_add)
{
    unsigned int x;
    unsigned char ack;
    
    DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
    CK_I2C_b(1);
    DATA_I2C_B = 0;         /* start condition                      */
    CK_I2C_b(1);
    CK_I2C_b(0);
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo dispositivo                                  */
    /*--------------------------------------------------------------*/
    for (x = 0; x < 8; x++)
    {
        CK_I2C_b(0);
        DATA_I2C_B = (slave_add >> (7 - x));
        CK_I2C_b(1);
    }
    
    CK_I2C_b(0);
    DD_DATA_I2C_B = 0;      /* dati in ingresso                     */
    CK_I2C_b(1);
    ack = DATA_I2C_B;
    
    if (ack)
        return;
    
    CK_I2C_b(0);
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo ram al dispositivo                           */
    /*--------------------------------------------------------------*/
    DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
    
    for (x = 0; x < 8; x++)
    {
        CK_I2C_b(0);
        DATA_I2C_B = (add >> (7 - x));
        CK_I2C_b(1);
    }
    
    CK_I2C_b(0);
    DD_DATA_I2C_B = 0;      /* dati in ingresso                     */
    CK_I2C_b(1);
    ack = DATA_I2C_B;
    CK_I2C_b(0);
    DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
    DATA_I2C_B = 1;
}



/*----------------------------------------------------------------------------*/
/*  Set_I2C_Add_512_b .... @ CLK & DATA H                                     */
/*----------------------------------------------------------------------------*/
void Set_I2C_Add_512_b (unsigned char add_h, unsigned char add_l, unsigned char slave_add)
{
    unsigned int x;
    unsigned char ack;
    
//     DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro         ,     */
//     delay_us(3);
//     
// //    CK_I2C_b(1);
// //    delay_us(5);
    
    DATA_I2C_B = 0;         /* start condition                      */
    delay_us(3);
    
//    CK_I2C_b(1);
//    delay_us(5);
//    
//    CK_I2C_b(0);
//    delay_us(5);
    /*--------------------------------------------------------------*/
    /* invio indirizzo dispositivo                                  */
    /*--------------------------------------------------------------*/
    for (x = 0; x < 8; x++)
    {
        CK_I2C_b(0);
        delay_us(3);
        
        DATA_I2C_B = (slave_add >> (7 - x));
        delay_us(3);
        
        CK_I2C_b(1);
        delay_us(3);
    }
    CK_I2C_b(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;      /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C_b(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack != 0)
    {
        Nop();
        ack = DATA_I2C_I_B;
        delay_us(3);
    }
    CK_I2C_b(0);
    delay_us(3);
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo ram al dispositivo h                         */
    /*--------------------------------------------------------------*/
    DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
    delay_us(10);
    
    for (x = 0; x < 8; x++)
    {
        CK_I2C_b(0);
        delay_us(3);
        
        DATA_I2C_B = (add_h >> (7 - x));
        delay_us(3);
        
        CK_I2C_b(1);
        delay_us(3);
    }
    CK_I2C_b(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;      /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C_b(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack != 0)
    {
        Nop();
        ack = DATA_I2C_I_B;
        delay_us(2);
    }
    
    CK_I2C_b(0);
    delay_us(3);
//     DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
//     DATA_I2C_B = 1;
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo ram al dispositivo l                         */
    /*--------------------------------------------------------------*/
    DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
    delay_us(10);

    for (x = 0; x < 8; x++)
    {
        CK_I2C_b(0);
        delay_us(3);
        
        DATA_I2C_B = (add_l >> (7 - x));
        delay_us(3);
        
        CK_I2C_b(1);
        delay_us(3);
    }
    
    CK_I2C_b(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;      /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C_b(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack != 0)
    {
        Nop();
        ack = DATA_I2C_I_B;
        delay_us(3);
    }
    
    CK_I2C_b(0);
    delay_us(3);
    
    DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
    delay_us(10);
    
    DATA_I2C_B = 1;
    delay_us(3);

    CK_I2C_b(1);
    delay_us(3);
}
