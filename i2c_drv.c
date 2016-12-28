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

#define TRUE    1
#define FALSE   0




/*----------------------------------------------------------------------------*/
/* CK_I2CK - RALLENTATORE CLK I2C board                                       */
/*----------------------------------------------------------------------------*/
void CK_I2C (unsigned char ck)
{
    CLK_I2C_B = ck;
    //delay_us(3);
}


static inline __attribute__((always_inline)) void startCondition() {
    DD_DATA_I2C_B = OUTPUT_PIN;
    CLK_I2C_B = HIGH;
    DATA_I2C_B = LOW;
}

static inline __attribute((always_inline)) void stopCondition() {
    DD_DATA_I2C_B = OUTPUT_PIN;//1;      /* dati in uscita dal micro             */
    DATA_I2C_B = 0;         /* stop condition                       */
    CK_I2C(1);
    DATA_I2C_B = 1;         /* stop condition                       */
    CK_I2C(0);
    CK_I2C(1);
}

static inline __attribute__((always_inline)) void I2CByteWrite(unsigned char byte) {
    int x = 0;
    DD_DATA_I2C_B = OUTPUT_PIN;
    for ( x = 0; x < 8; x++)
    {
        CK_I2C(0);
        DATA_I2C_B = (byte >> (7 - x)) & 0x01;
        CK_I2C(1);
    }
}

static inline __attribute__((always_inline)) void I2CByteRead(unsigned char* byte) {
    DD_DATA_I2C_B = INPUT_PIN;//0;  /* dati in ingesso                      */
    unsigned char x = 0;
    for (x = 0; x < 8; x++)
    {
        CK_I2C(1);
        *byte = (*byte << 1) | DATA_I2C_INPUT;//DATA_I2C_B; /* dato in input */
        CK_I2C(0);
    }
    DD_DATA_I2C_B = OUTPUT_PIN;//1;  /* dati in uscita dal micro             */
    DATA_I2C_B = 0;     /* invia ack                            */
    CK_I2C(1);
    CK_I2C(0);
}

static inline __attribute__((always_inline)) unsigned char readAck() {
    unsigned char x;
    CK_I2C(0);
    DATA_I2C_B = 1;         //Set Nack
    DD_DATA_I2C_B = INPUT_PIN;
    CK_I2C(1);
    x = DATA_I2C_INPUT;//DATA_I2C_B;
    CK_I2C(0);
    return x;
}


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
    
    D_WP_TRIS       = OUTPUT_PIN;
    D_WP = 0;
}



/*----------------------------------------------------------------------------*/
/*  I2C_Write_b                                                               */
/*----------------------------------------------------------------------------*/
int I2C_Write_b (unsigned char cDevAddr, unsigned char cRegAddr, const unsigned char* pData, int nLen)
{
    unsigned int y;
    unsigned char ack;
    
    //TODO: REMOVE THIS
    if (nLen > PAGE_SIZE)
        return -2;

    startCondition();
    I2CByteWrite(cDevAddr);
    
    ack = readAck();
    if (ack != 0)
         return (-1);       /* busy                                 */

    I2CByteWrite(cRegAddr);
    ack = readAck();
    if (ack != 0)
         return (-1);
    
    for (y = 0; y < nLen; y++)
    {
        I2CByteWrite(*pData);
        pData ++;
        ack = readAck();        
        if (ack != 0)
            return (-1);
    }
    
    stopCondition();
    return (0);
}



/*----------------------------------------------------------------------------*/
/* I2C_Read_b                                                                 */
/*----------------------------------------------------------------------------*/
int I2C_Read_b (unsigned char cDevAddr, unsigned char cRegAddr, unsigned char* buffer, int nLen)
{
    unsigned int y;
    unsigned char ack;
    
    startCondition();
    cDevAddr |=0x01;
    
    I2CByteWrite(cDevAddr);
    
    ack = readAck();
    
    if (ack)
         return (-1);       /* busy                                 */
        
    /*--------------------------------------------------------------*/
    /* Legge la ram del dispositivo                                 */
    /*--------------------------------------------------------------*/
    
    for(y = 0; y < nLen; y++)
    {
        I2CByteRead(buffer);    
        buffer++;
    }
    
    stopCondition();
    return (0);
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
        CK_I2C(0);
        delay_us(3);
        
        DATA_I2C_B = (cDevAddr >> (7 - x));
        delay_us(3);
        
        CK_I2C(1);
        delay_us(3);
    }
    CK_I2C(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;      /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack != 0)
    {
        Nop();
        ack = DATA_I2C_I_B;
        delay_us(3);
    }
    
    CK_I2C(0);
    delay_us(3);
    
    DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
    delay_us(10);
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo registro h                                   */
    /*--------------------------------------------------------------*/
    for (x = 0; x < 8; x++)
    {
        CK_I2C(0);
        delay_us(3);
        
        DATA_I2C_B = (cRegAddr_h >> (7 - x));
        delay_us(3);
        
        CK_I2C(1);
        delay_us(3);
    }
    CK_I2C(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;        /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack != 0)
    {
        Nop();
        ack = DATA_I2C_I_B;
        delay_us(3);
    }
    
    CK_I2C(0);
    delay_us(3);
    
    DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
    delay_us(10);
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo registro l                                   */
    /*--------------------------------------------------------------*/
    for (x = 0; x < 8; x++)
    {
        CK_I2C(0);
        delay_us(3);
        
        DATA_I2C_B = (cRegAddr_l >> (7 - x));
        delay_us(3);
        
        CK_I2C(1);
        delay_us(3);
    }
    CK_I2C(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;        /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack != 0)
    {
        Nop();
        ack = DATA_I2C_I_B;
        delay_us(3);
    }
    
    CK_I2C(0);
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
            CK_I2C(0);
            delay_us(3);
            
            DATA_I2C_B = (*pData >> (7 - x));
            delay_us(3);
            
            CK_I2C(1);
            delay_us(3);
        }
        
        pData ++;
        
        CK_I2C(0);
        delay_us(3);
        
        DD_DATA_I2C_B = INPUT_PIN;  /* dati in ingresso                     */
        delay_us(500);
        
        CK_I2C(1);
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
    CK_I2C(0);
    delay_us(3);
    
    DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
    delay_us(50);
    
    DATA_I2C_B = 0;         /* stop condition                       */
    delay_us(3);
    
    CK_I2C(1);
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








// 
// 
// 
// 
// 
// /*----------------------------------------------------------------------------*/
// /* I2C_Read_512                                                              */
// /*----------------------------------------------------------------------------*/
// int I2C_Read_512 (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, unsigned char* buffer, unsigned int nLen)
// {
//     unsigned int x;
//     unsigned int y;
//     unsigned char ack;
//     
//     Set_I2C_Add_512 (cRegAddr_h, cRegAddr_l, cDevAddr);
//     
//     DD_DATA_I2C = OUTPUT_PIN;        /* dati in uscita dal micro             */
//     
//     CK_I2C(1);
//     DATA_I2C = 0;           /* DATI I2C */    /* start condition    */
//     CK_I2C(1);
//     CK_I2C(0);
//     cDevAddr |=1;
//     
//     /*--------------------------------------------------------------*/
//     /* invio indirizzo dispositivo                                  */
//     /*--------------------------------------------------------------*/
//     for (x = 0; x < 8; x++)
//     {
//         CK_I2C(0);
//         DATA_I2C = (cDevAddr >> (7 - x));
//         CK_I2C(1);
//     }
//     
//     CK_I2C(0);
//     DD_DATA_I2C = INPUT_PIN;        /* dati in ingresso                     */
//     CK_I2C(1);
//     ack = DATA_I2C;
//     
// //     if (ack != 0)
// //          return (-1);           /* busy                             */
//          
//     while(ack!=0)
//         Nop();
//     
//     CK_I2C(0);
//     
//     /*--------------------------------------------------------------*/
//     /* Legge la ram del dispositivo                                 */
//     /*--------------------------------------------------------------*/
//     DD_DATA_I2C = INPUT_PIN;        /* dati in ingresso                     */
//     
//     for(y = 0; y < nLen; y++)
//     {
//         for (x = 0; x < 8; x++)
//         {
//             CK_I2C(1);
//             *buffer = (*buffer << 1) | DATA_I2C;   /* dato in input */
//             CK_I2C(0);
//         }
//         
//         buffer++;
//         DD_DATA_I2C = OUTPUT_PIN;    /* dati in uscita dal micro             */
//         DATA_I2C = 0;       /* invia ack                            */
//         
//         if (y == nLen -1)
//             DATA_I2C = 1;
//         
//         CK_I2C(1);
//         CK_I2C(0);
//         DD_DATA_I2C = INPUT_PIN;    /* dati in ingesso                      */
//     }
//     
//     DD_DATA_I2C = OUTPUT_PIN;        /* dati in uscita dal micro             */
//     DATA_I2C = 0;           /* DATI I2C */   /* stop condition      */
//     CK_I2C(1);
//     DATA_I2C = 1;           /* DATI I2C */   /* stop condition      */
//     CK_I2C(0);
//     CK_I2C(1);
//     
//     return (1);
// }





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
        CK_I2C(0);
        delay_us(3);
        
        DATA_I2C_B = (cDevAddr >> (7 - x));
        delay_us(3);
        
        CK_I2C(1);
        delay_us(3);
    }
    
    CK_I2C(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;      /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack !=0)
    {
        Nop ();
        ack = DATA_I2C_I_B;
        delay_us(3);
    }
    
    CK_I2C(0);
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
            CK_I2C(0);
            delay_us(3);
            
            *buffer = (*buffer << 1) | DATA_I2C_I_B; /* dato in input */
            // dato = (dato << 1) | DATA_I2C_I_B; /* dato in input */
            delay_us(3);
            
            CK_I2C(1);
            delay_us(3);
        }
        CK_I2C(0);
        delay_us(3);
        
        
        
        if (y != nLen-1) // CP se ultimo BYTE da RD e s no 
        {
            CK_I2C(1);
            delay_us(3);
            
            while (ack !=0)
            {
                Nop ();
                ack = DATA_I2C_I_B;
                delay_us(3);
            }
            CK_I2C(0);
            delay_us(10);
            
            buffer++;
        }
        Nop();
        Nop();
        Nop();
        Nop();
    }
        
        
// // //         buffer++;
// // //         
// // //         DD_DATA_I2C_B = OUTPUT_PIN;  /* dati in uscita dal micro             */
// // //         delay_us(3);
// // // 
// // //         DATA_I2C_B = 0;     /* invia ack                            */
// // //         delay_us(3);
// // //         
// // //         if (y == nLen -1)
// // //             DATA_I2C_B = 1;
// // //         
// // //         CK_I2C_b(1);
// // //         delay_us(5);
// // //         
// // //         CK_I2C_b(0);
// // //         delay_us(5);
// // //         
// // //         DD_DATA_I2C_B = 1;  /* dati in ingesso                      */
// // //         delay_us(5);

// // //    }
    

    CK_I2C(1);
    delay_us(3);
    
    DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
    delay_us(10);
    
    DATA_I2C_B = 1;         /* stop condition                       */
    delay_us(3);
    
  
    fsw_no_agg_dis  = 0; // -DDDD riabilito altre funzioni
    
    return (1); // NB: ritorno con CLK e DATA H
    
    
    
    
// // //     DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
// // //     delay_us(5);
// // //     
// // //     DATA_I2C_B = 0;         /* DATI I2C */   /* stop condition      */
// // //     delay_us(5);
// // //     
// // //     CK_I2C_b(1);
// // //     delay_us(5);
// // //     
// // //     DATA_I2C_B = 1;         /* DATI I2C */   /* stop condition      */
// // //     delay_us(5);
// // //     
// // //     CK_I2C_b(0);
// // //     delay_us(5);
// // //     
// // //     CK_I2C_b(1);
// // //     delay_us(5);
// // //     
// // //     f_no_agg_dis  = 0; // -DDDD
// // //     
// // //     return (1);
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






// /*----------------------------------------------------------------------------*/
// /* I2C_Read_512_b2                                                            */
// /*----------------------------------------------------------------------------*/
// int I2C_Read_512_b2 (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, unsigned char* buffer, unsigned int nLen)
// {
//     unsigned int x;
//     unsigned int y;
//     unsigned char ack;
//     
//     f_no_agg_dis  = 1; // -DDDD
//     
//     Set_I2C_Add_512_b (cRegAddr_h, cRegAddr_l, cDevAddr);
//     DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
//     
//     CK_I2C_b(1);
//     DATA_I2C_B = 0;         /* DATI I2C */    /* start condition    */
//     CK_I2C_b(1);
//     CK_I2C_b(0);
//     cDevAddr |=1;
//     
//     /*--------------------------------------------------------------*/
//     /* invio indirizzo dispositivo                                  */
//     /*--------------------------------------------------------------*/
//     for (x = 0; x < 8; x++)
//     {
//         CK_I2C_b(0);
//         DATA_I2C_B = (cDevAddr >> (7 - x));
//         CK_I2C_b(1);
//     }
//     
//     CK_I2C_b(0);
//     DD_DATA_I2C_B = 0;      /* dati in ingresso                     */
//     CK_I2C_b(1);
//     ack = DATA_I2C_B;
//     
//     if (ack)
//          return (-1);       /* busy                                 */
//     
//     CK_I2C_b(0);
//     
//     /*--------------------------------------------------------------*/
//     /* Legge la ram del dispositivo                                 */
//     /*--------------------------------------------------------------*/
//     DD_DATA_I2C_B = 0;      /* dati in ingresso                     */
//     
//     for(y = 0; y < nLen; y++)
//     {
//         for (x = 0; x < 8; x++)
//         {
//             CK_I2C_b(1);
//             *buffer = (*buffer << 1) | DATA_I2C_B; /* dato in input */
//             CK_I2C_b(0);
//         }
//         
//         buffer++;
//         DD_DATA_I2C_B = 1;  /* dati in uscita dal micro             */
//         DATA_I2C_B = 0;     /* invia ack                            */
//         
//         if (y == nLen -1)
//             DATA_I2C_B = 1;
//         
//         CK_I2C_b(1);
//         CK_I2C_b(0);
//         DD_DATA_I2C_B = 0;  /* dati in ingesso                      */
//     }
//     
//     DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
//     DATA_I2C_B = 0;         /* DATI I2C */   /* stop condition      */
//     CK_I2C_b(1);
//     DATA_I2C_B = 1;         /* DATI I2C */   /* stop condition      */
//     CK_I2C_b(0);
//     CK_I2C_b(1);
//     
//     f_no_agg_dis  = 0; // -DDDD
//     
//     return (1);
// }
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// /*----------------------------------------------------------------------------*/
// /*  Set_I2C_Add                                                               */
// /*----------------------------------------------------------------------------*/
// void Set_I2C_Add (unsigned char add, unsigned char slave_add)
// {
//     unsigned int x;
//     unsigned char ack;
//     
//     DD_DATA_I2C = 1;        /* dati in uscita dal micro             */
//     CK_I2C(1);
//     DATA_I2C = 0;           /* start condition                      */
//     CK_I2C(1);
//     CK_I2C(0);
//     
//     /*--------------------------------------------------------------*/
//     /* invio indirizzo dispositivo                                  */
//     /*--------------------------------------------------------------*/
//     for (x = 0; x < 8; x++)
//     {
//         CK_I2C(0);
//         DATA_I2C = (slave_add >> (7 - x));
//         CK_I2C(1);
//     }
//     
//     CK_I2C(0);
//     DD_DATA_I2C = 0;        /* dati in ingresso                     */
//     CK_I2C(1);
//     ack = DATA_I2C;
//     
//     if (ack)
//         return;
//     
//     CK_I2C(0);
//     
//     /*--------------------------------------------------------------*/
//     /* invio indirizzo ram al dispositivo                           */
//     /*--------------------------------------------------------------*/
//     DD_DATA_I2C = 1;        /* dati in uscita dal micro             */
//     
//     for (x = 0; x < 8; x++)
//     {
//         CK_I2C(0);
//         DATA_I2C = (add >> (7 - x));
//         CK_I2C(1);
//     }
//     
//     CK_I2C(0);
//     DD_DATA_I2C = 0;        /* dati in ingresso                     */
//     CK_I2C(1);
//     ack = DATA_I2C;
//     CK_I2C(0);
//     DD_DATA_I2C = 1;        /* dati in uscita dal micro             */
//     DATA_I2C = 1;
// }




/*----------------------------------------------------------------------------*/
/*  Set_I2C_Add_b                                                             */
/*----------------------------------------------------------------------------*/
void Set_I2C_Add_b (unsigned char add, unsigned char slave_add)
{
    unsigned int x;
    unsigned char ack;
    
    DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
    CK_I2C(1);
    DATA_I2C_B = 0;         /* start condition                      */
    CK_I2C(1);
    CK_I2C(0);
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo dispositivo                                  */
    /*--------------------------------------------------------------*/
    for (x = 0; x < 8; x++)
    {
        CK_I2C(0);
        DATA_I2C_B = (slave_add >> (7 - x));
        CK_I2C(1);
    }
    
    CK_I2C(0);
    DD_DATA_I2C_B = 0;      /* dati in ingresso                     */
    CK_I2C(1);
    ack = DATA_I2C_B;
    
    if (ack)
        return;
    
    CK_I2C(0);
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo ram al dispositivo                           */
    /*--------------------------------------------------------------*/
    DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
    
    for (x = 0; x < 8; x++)
    {
        CK_I2C(0);
        DATA_I2C_B = (add >> (7 - x));
        CK_I2C(1);
    }
    
    CK_I2C(0);
    DD_DATA_I2C_B = 0;      /* dati in ingresso                     */
    CK_I2C(1);
    ack = DATA_I2C_B;
    CK_I2C(0);
    DD_DATA_I2C_B = 1;      /* dati in uscita dal micro             */
    DATA_I2C_B = 1;
}




// /*----------------------------------------------------------------------------*/
// /*  Set_I2C_Add_512                                                           */
// /*----------------------------------------------------------------------------*/
// void Set_I2C_Add_512 (unsigned char add_h, unsigned char add_l, unsigned char slave_add)
// {
//     unsigned int x;
//     unsigned char ack;
//     
//     DD_DATA_I2C = OUTPUT_PIN;        /* dati in uscita dal micro             */
//     CK_I2C(1);
//     DATA_I2C = 0;           /* start condition                      */
//     CK_I2C(1);
//     CK_I2C(0);
//     
//     /*--------------------------------------------------------------*/
//     /* invio indirizzo dispositivo                                  */
//     /*--------------------------------------------------------------*/
//     for (x = 0; x < 8; x++)
//     {
//         CK_I2C(0);
//         DATA_I2C = (slave_add >> (7 - x));
//         CK_I2C(1);
//     }
//     
//     CK_I2C(0);
//     DD_DATA_I2C = INPUT_PIN;        /* dati in ingresso                     */
//     CK_I2C(1);
//     ack = DATA_I2C;
//     
//     if (ack)//     if (ack != 0)
// //          return (-1);           /* busy                             */
//          
//     while(ack!=0)
//         Nop();
// 
//         return;
//     
//     CK_I2C(0);
//     
//     /*--------------------------------------------------------------*/
//     /* invio indirizzo ram al dispositivo h                         */
//     /*--------------------------------------------------------------*/
//     DD_DATA_I2C = OUTPUT_PIN;        /* dati in uscita dal micro             */
//     
//     for (x = 0; x < 8; x++)
//     {
//         CK_I2C(0);
//         DATA_I2C = (add_h >> (7 - x));
//         CK_I2C(1);
//     }
//     
//     CK_I2C(0);
//     DD_DATA_I2C = INPUT_PIN;        /* dati in ingresso                     */
//     CK_I2C(1);
//     ack = DATA_I2C;
//     
// //     if (ack != 0)
// //          return (-1);           /* busy                             */
//          
//     while(ack!=0)
//         Nop();
//     
//     CK_I2C(0);
// //     DD_DATA_I2C = OUTPUT_PIN;        /* dati in uscita dal micro             */
// //     DATA_I2C = 1;
//     
//     /*--------------------------------------------------------------*/
//     /* invio indirizzo ram al dispositivo l                         */
//     /*--------------------------------------------------------------*/
//     DD_DATA_I2C = OUTPUT_PIN;        /* dati in uscita dal micro             */
//     
//     for (x = 0; x < 8; x++)
//     {
//         CK_I2C(0);
//         DATA_I2C = (add_l >> (7 - x));
//         CK_I2C(1);
//     }
//     
//     CK_I2C(0);
//     DD_DATA_I2C = INPUT_PIN;        /* dati in ingresso                     */
//     CK_I2C(1);
//     ack = DATA_I2C;
// //     
// //     if (ack)
// //         return;
// //     
//     CK_I2C(0);
//     DD_DATA_I2C = OUTPUT_PIN;        /* dati in uscita dal micro             */
//     DATA_I2C = 1;
// }
// 
// 



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
        CK_I2C(0);
        delay_us(3);
        
        DATA_I2C_B = (slave_add >> (7 - x));
        delay_us(3);
        
        CK_I2C(1);
        delay_us(3);
    }
    CK_I2C(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;      /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack != 0)
    {
        Nop();
        ack = DATA_I2C_I_B;
        delay_us(3);
    }
    CK_I2C(0);
    delay_us(3);
    
    /*--------------------------------------------------------------*/
    /* invio indirizzo ram al dispositivo h                         */
    /*--------------------------------------------------------------*/
    DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
    delay_us(10);
    
    for (x = 0; x < 8; x++)
    {
        CK_I2C(0);
        delay_us(3);
        
        DATA_I2C_B = (add_h >> (7 - x));
        delay_us(3);
        
        CK_I2C(1);
        delay_us(3);
    }
    CK_I2C(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;      /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack != 0)
    {
        Nop();
        ack = DATA_I2C_I_B;
        delay_us(2);
    }
    
    CK_I2C(0);
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
        CK_I2C(0);
        delay_us(3);
        
        DATA_I2C_B = (add_l >> (7 - x));
        delay_us(3);
        
        CK_I2C(1);
        delay_us(3);
    }
    
    CK_I2C(0);
    delay_us(3);
    
    DD_DATA_I2C_B = INPUT_PIN;      /* dati in ingresso                     */
    delay_us(10);
    
    CK_I2C(1);
    delay_us(3);
    
    ack = DATA_I2C_I_B;
    delay_us(3);
    
    while (ack != 0)
    {
        Nop();
        ack = DATA_I2C_I_B;
        delay_us(3);
    }
    
    CK_I2C(0);
    delay_us(3);
    
    DD_DATA_I2C_B = OUTPUT_PIN;      /* dati in uscita dal micro             */
    delay_us(10);
    
    DATA_I2C_B = 1;
    delay_us(3);

    CK_I2C(1);
    delay_us(3);
}
