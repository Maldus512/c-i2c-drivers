#ifndef TC74_H_INCLUDED
#define	TC74_H_INCLUDED

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
