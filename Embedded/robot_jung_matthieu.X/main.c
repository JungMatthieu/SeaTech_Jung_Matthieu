/* 
 * File:   main.c
 * Author: GEII Robot & Jung Matthieu
 *
 * Created on 21 septembre 2022, 14:56
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h" 
#include "timer.h"
#include "Robot.h"
#include "PWM.h"
#include "ADC.h"
unsigned int ADCValue0;
unsigned int ADCValue1;
unsigned int ADCValue2;



int main(void) {
    /***************************************************************************************************/
    //Initialisation de l?oscillateur
    /****************************************************************************************************/
    InitOscillator();

    /****************************************************************************************************/
    // Configuration des entrées sorties
    /****************************************************************************************************/
    InitIO();

    // Configuration des Timers **********************************************************************************/
    InitTimer23();
    InitTimer1();
    InitADC1();

    // Configuration des PWM **********************************************************************************/
    InitPWM();

    LED_BLANCHE = 1;
    LED_BLEUE = 1;
    LED_ORANGE = 1;
    
    //PWMSetSpeedConsigne(0, MOTEUR_DROIT);
    //PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
        unsigned int * result = ADCGetResult();
        ADCValue0 = result[0];
        ADCValue1 = result[1];
        ADCValue2 = result[2];
        
    }
}