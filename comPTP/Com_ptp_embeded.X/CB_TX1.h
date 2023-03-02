/* 
 * File:   CB_TX1.h
 * Author: Jung Matthieu
 *
 * Created on 2 mars 2023, 09:03
 */

#ifndef CB_TX1_H
#define	CB_TX1_H

#define CBTX1_BUFFER_SIZE 128

void CB_TX1_Add(unsigned char value);
unsigned char CB_TX1_Get(void);
unsigned char CB_TX1_IsTranmitting(void);
int CB_TX1_RemainingSize(void);
void SendMessage(unsigned char* message, int lenth);
void SendOne(void);

#endif