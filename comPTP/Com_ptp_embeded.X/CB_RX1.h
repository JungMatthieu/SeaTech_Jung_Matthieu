/* 
 * File:   CB_RX1.h
 * Author: Table2
 *
 * Created on 2 mars 2023, 10:16
 */

#ifndef CB_RX1_H
#define	CB_RX1_H

#define CBTX1_BUFFER_SIZE 128
void CB_RX1_Add(unsigned char value);
unsigned char CB_RX1_Get(void);
unsigned char CB_RX1_IsDataAvailable(void);
int CB_RX1_GetRemainingSize(void);
int CB_RX1_GetDataSize(void);


#endif	/* CB_RX1_H */

