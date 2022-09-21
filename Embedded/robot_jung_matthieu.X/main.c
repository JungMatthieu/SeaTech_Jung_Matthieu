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

int main (void){
/***************************************************************************************************/
//Initialisation de l?oscillateur
/****************************************************************************************************/
InitOscillator();

/****************************************************************************************************/
// Configuration des entr�es sorties
/****************************************************************************************************/
InitIO();

// Configuration des Timers **********************************************************************************/
InitTimer23();
InitTimer1();


LED_BLANCHE = 1;
LED_BLEUE = 1;
LED_ORANGE = 1;

/****************************************************************************************************/
// Boucle Principale
/****************************************************************************************************/
while(1){
}}