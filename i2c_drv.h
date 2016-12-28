/******************************************************************************/
/*                                                                            */
/*  HSW snc - Casalecchio di Reno (BO) ITALY                                  */
/*  ----------------------------------------                                  */
/*                                                                            */
/*  modulo: I2C_drv.h                                                         */
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

// #define CLK_I2C         P8.3
// #define DATA_I2C        P8.2
// #define DD_CLK_I2C      PD8.3
// #define DD_DATA_I2C     PD8.2
// 
// #define CLK_I2C         CLK_I2C
// #define DATA_I2C        D_I2C
// #define DD_CLK_I2C      CLK_I2C_TRIS
// #define DD_DATA_I2C     D_I2C_TRIS



// #define CLK_I2C         P7.1
// #define DATA_I2C        P7.0
// #define DD_CLK_I2C      PD7.1
// #define DD_DATA_I2C     PD7.0

// #define KEY_IN_I2C      P8.6
// #define DD_KEY_IN_I2C   PD8.6



// #define CLK_I2C_B       P7.1
// #define DATA_I2C_B      P7.0
// #define DD_CLK_I2C_B    PD7.1
// #define DD_DATA_I2C_B   PD7.0


#define CLK_I2C_B       CLK_I2C
#define DATA_I2C_B      D_I2C
#define DATA_I2C_I_B    D_I2C_I

#define DD_CLK_I2C_B    CLK_I2C_TRIS
#define DD_DATA_I2C_B   D_I2C_TRIS


// #define CLK_I2C_B       P8.3
// #define DATA_I2C_B      P8.2
// #define DD_CLK_I2C_B    PD8.3
// #define DD_DATA_I2C_B   PD8.2

#define EEPROM_0_ADDR     '\xD0'
#define EEPROM_1_ADDR     '\xD0'
#define EEPROM_2_ADDR     '\xD0'




void Init_I2C (void);
void Init_I2C_b (void);

//int I2C_Write (unsigned char, unsigned char, const unsigned char*, int);
int I2C_Write_b(unsigned char, unsigned char, const unsigned char*, int);
//int I2C_Write_512 (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, const unsigned char* pData, unsigned int nLen);
int I2C_Write_1024_b (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, const unsigned char* pData, unsigned int nLen);
//int I2C_Write_512_b2 (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, const unsigned char* pData, unsigned int nLen);
void I2C_Write_1024_b2(unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, const unsigned char* pData, unsigned int nLen);

//int I2C_Read (unsigned char, unsigned char, unsigned char*, int);
int I2C_Read_b (unsigned char, unsigned char, unsigned char*, int);
//int I2C_Read_512 (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, unsigned char* buffer, unsigned int nLen);
int I2C_Read_1024_b (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, unsigned char* buffer, unsigned int nLen);
//int I2C_Read_512_b2 (unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, unsigned char* buffer, unsigned int nLen);
void I2C_Read_1024_b2(unsigned char cDevAddr, unsigned char cRegAddr_h, unsigned char cRegAddr_l, unsigned char* buffer, unsigned int nLen);

//void CK_I2C (unsigned char ck);
void CK_I2C (unsigned char ck);

//void Set_I2C_Add (unsigned char add, unsigned char slave_add);
void Set_I2C_Add_b (unsigned char add, unsigned char slave_add);
//void Set_I2C_Add_512 (unsigned char add_h, unsigned char add_l, unsigned char slave_add);
void Set_I2C_Add_512_b (unsigned char add_h, unsigned char add_l, unsigned char slave_add);

