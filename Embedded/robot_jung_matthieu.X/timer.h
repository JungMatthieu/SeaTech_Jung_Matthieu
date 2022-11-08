#ifndef TIMER_H
#define	TIMER_H

extern unsigned long timestamp;

void InitTimer23(void);
void InitTimer1(void);
void InitTimer4(void);
void SetFreqTimer1(float freq);
void SetFreqTimer4(float freq);
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void);
void resetTimeStamp(void);

#endif	/* TIMER_H */

