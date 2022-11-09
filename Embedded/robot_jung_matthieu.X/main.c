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
#include "main.h"

unsigned int ADCValue0;
unsigned int ADCValue1;
unsigned int ADCValue2;
unsigned int ADCValue3;
unsigned int ADCValue4;
unsigned char stateRobot;

unsigned char sensorState;

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
    InitTimer1(); //voir fonction ici 250Hz
    InitTimer4(); //voir fonction ici 1000Hz (ms)
    InitADC1();

    // Configuration des PWM **********************************************************************************/
    InitPWM();

    LED_BLANCHE = 0;
    LED_BLEUE = 0;
    LED_ORANGE = 0;

    //PWMSetSpeedConsigne(0, MOTEUR_DROIT);
    //PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
        if (ADCIsConversionFinished()==1){
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            
            float volts = ((float)result[0]) * 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExtremDroit = 34 / volts - 5;
            
            volts = ((float)result[1]) * 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            
            volts = ((float)result[2]) * 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            
            volts = ((float)result[3]) * 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExtremGauche = 34 / volts - 5;
            
            volts = ((float)result[4]) * 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreGauche = 34 / volts - 5;

            if (robotState.distanceTelemetreDroit < 30 || robotState.distanceTelemetreExtremDroit < 30) LED_ORANGE = 1;
            else LED_ORANGE = 0;

            if (robotState.distanceTelemetreCentre < 30) LED_BLEUE = 1;
            else LED_BLEUE = 0;

            if (robotState.distanceTelemetreGauche < 30 || robotState.distanceTelemetreExtremGauche < 30) LED_BLANCHE = 1;
            else LED_BLANCHE = 0;
            
            sensorState = 0;
            if (robotState.distanceTelemetreExtremGauche <= 20)
                sensorState = sensorState | 0b10000 ;
            if (robotState.distanceTelemetreGauche <= 27)
                sensorState = sensorState | 0b01000 ;
            if (robotState.distanceTelemetreCentre <= 33)
                sensorState = sensorState | 0b00100 ;
            if (robotState.distanceTelemetreDroit <= 27)
                sensorState = sensorState | 0b00010 ;
            if (robotState.distanceTelemetreExtremDroit <= 20)
                sensorState = sensorState | 0b00001 ;
            
        }
    }
}

unsigned char stateRobot;
float vitesse = 29;
float vitessemanoeuvre = 25;
float disevitement = 33;
float demi = 0;
float temps_bloque=0;

void OperatingSystemLoop(void)
    {
        switch (stateRobot)
        {
            case STATE_ATTENTE:
                resetTimeStamp();
                PWMSetSpeedConsigne(0, MOTEUR_DROIT);
                PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
                stateRobot = STATE_ATTENTE_EN_COURS;

            case STATE_ATTENTE_EN_COURS:
                if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
                break;

            case STATE_AVANCE:
                PWMSetSpeedConsigne(vitesse, MOTEUR_DROIT);
                PWMSetSpeedConsigne(vitesse, MOTEUR_GAUCHE);
                stateRobot = STATE_AVANCE_EN_COURS;
                break;
            case STATE_AVANCE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;

            case STATE_TOURNE_GAUCHE:
                PWMSetSpeedConsigne(vitesse, MOTEUR_DROIT);
                PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
                break;
            case STATE_TOURNE_GAUCHE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;
                
            case STATE_TOURNE_GAUCHE_LEGER:
                PWMSetSpeedConsigne(vitesse / 2 , MOTEUR_DROIT);
                PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_GAUCHE_LEGER_EN_COURS;
                break;
            case STATE_TOURNE_GAUCHE_LEGER_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;    

            case STATE_TOURNE_DROITE:
                PWMSetSpeedConsigne(0, MOTEUR_DROIT);
                PWMSetSpeedConsigne(vitesse, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_DROITE_EN_COURS;
                break;
            case STATE_TOURNE_DROITE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;
                
            case STATE_TOURNE_DROITE_LEGER:
                PWMSetSpeedConsigne(0, MOTEUR_DROIT);
                PWMSetSpeedConsigne(vitesse / 2, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_DROITE_LEGER_EN_COURS;
                break;
            case STATE_TOURNE_DROITE_LEGER_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;    

            case STATE_TOURNE_SUR_PLACE_GAUCHE:
                PWMSetSpeedConsigne(vitessemanoeuvre / 2, MOTEUR_DROIT);
                PWMSetSpeedConsigne(-vitessemanoeuvre / 2, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
                break;
            case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;

            case STATE_TOURNE_SUR_PLACE_DROITE:
                PWMSetSpeedConsigne(-vitessemanoeuvre / 2, MOTEUR_DROIT);
                PWMSetSpeedConsigne(vitessemanoeuvre / 2, MOTEUR_GAUCHE);
                stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
                break;
            case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;
                
            case STATE_RECULE:
            PWMSetSpeedConsigne(-vitessemanoeuvre , MOTEUR_DROIT);
            PWMSetSpeedConsigne(-vitessemanoeuvre , MOTEUR_GAUCHE);
            stateRobot = STATE_RECULE_EN_COURS;
            break;
            case STATE_RECULE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break;
            
            case STATE_RECULE_LEGER_DROITE:
            PWMSetSpeedConsigne(-vitessemanoeuvre , MOTEUR_DROIT);
            PWMSetSpeedConsigne(-vitessemanoeuvre / 2, MOTEUR_GAUCHE);
            stateRobot = STATE_RECULE_LEGER_DROITE_EN_COURS;
            break;
            case STATE_RECULE_LEGER_DROITE_EN_COURS:
                SetNextRobotStateInAutomaticMode();
                break; 
               
            case STATE_DEMI_TOUR:
                PWMSetSpeedConsigne(-vitessemanoeuvre / 2, MOTEUR_DROIT);
                PWMSetSpeedConsigne(vitessemanoeuvre / 2, MOTEUR_GAUCHE);
                demi = 0;
                stateRobot = STATE_DEMI_TOUR_EN_COURS;
                break;
            case STATE_DEMI_TOUR_EN_COURS:
                demi = demi + 1;
                if (demi >= 175) {
                    SetNextRobotStateInAutomaticMode();
                }
                break;    

            default :
                stateRobot = STATE_ATTENTE;
                break;
        }
    }

unsigned char nextStateRobot=0;
float tentative = 0;

void SetNextRobotStateInAutomaticMode()
{
    
    switch (sensorState){
        case 0b00000:
            nextStateRobot = STATE_AVANCE;
            break;
        case 0b00001:
            nextStateRobot = STATE_TOURNE_GAUCHE_LEGER; //LEGER
            break;
        case 0b00010:
            nextStateRobot = STATE_TOURNE_GAUCHE;
            break;
        case 0b00011:
            nextStateRobot = STATE_TOURNE_GAUCHE;
            break;
        case 0b00100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b00101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b00111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01000:
            nextStateRobot = STATE_TOURNE_DROITE;
            break;
        case 0b01001:
            nextStateRobot = STATE_TOURNE_DROITE_LEGER; //LEGER
            break; 
        case 0b01010:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE; 
            break;
        case 0b01011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE; 
            break;
        case 0b01100:
            nextStateRobot = STATE_RECULE_LEGER_DROITE; 
            break;
        case 0b01101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE; 
            break;
        case 0b01110:
            nextStateRobot = STATE_RECULE_LEGER_DROITE; 
            break;
        case 0b01111:
//            if(temps_bloque<5000){
//                nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE; 
//                break;
//            }
//            else {
//                nextStateRobot = STATE_DEMI_TOUR;
//            }
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE; 
            break;
        case 0b10000:
            nextStateRobot = STATE_TOURNE_DROITE_LEGER;  //LEGER
            break;
        case 0b10001:
            nextStateRobot = STATE_AVANCE;  
            break;
        case 0b10010:
            nextStateRobot = STATE_TOURNE_GAUCHE_LEGER;  
            break;
        case 0b10011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;  
            break;
        case 0b10100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;  
            break;
        case 0b10101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;  
            break;
        case 0b10110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;  
            break;
        case 0b10111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;  
            break;
        case 0b11000:
            nextStateRobot = STATE_TOURNE_DROITE;  
            break;
        case 0b11001:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;  
            break;
        case 0b11010:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;  
            break;
        case 0b11011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;  
            break;
        case 0b11100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;  
            break;
        case 0b11101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;  
            break;
        case 0b11110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;  
            break;
        case 0b11111:
            nextStateRobot = STATE_RECULE;  
            break;
    }
    
    //Si l?on n?est pas dans la transition de l?étape en cours
    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;
    
//unsigned char positionObstacle = PAS_D_OBSTACLE;
//
////Détermination de la position des obstacles en fonction des télémètres
//if ( robotState.distanceTelemetreDroit < 30 &&
//robotState.distanceTelemetreCentre > 20 &&
//robotState.distanceTelemetreGauche > 30) //Obstacle à droite
//positionObstacle = OBSTACLE_A_DROITE;
//else if(robotState.distanceTelemetreDroit > 30 &&
//robotState.distanceTelemetreCentre > 20 &&
//robotState.distanceTelemetreGauche < 30) //Obstacle à gauche
//positionObstacle = OBSTACLE_A_GAUCHE;
//else if(robotState.distanceTelemetreCentre < 20) //Obstacle en face
//positionObstacle = OBSTACLE_EN_FACE;
//else if(robotState.distanceTelemetreDroit > 30 &&
//robotState.distanceTelemetreCentre > 20 &&
//robotState.distanceTelemetreGauche > 30) //pas d?obstacle
//positionObstacle = PAS_D_OBSTACLE;
//
////Détermination de l?état à venir du robot
//if (positionObstacle == PAS_D_OBSTACLE)
//nextStateRobot = STATE_AVANCE;
//else if (positionObstacle == OBSTACLE_A_DROITE)
//nextStateRobot = STATE_TOURNE_GAUCHE;
//else if (positionObstacle == OBSTACLE_A_GAUCHE)
//nextStateRobot = STATE_TOURNE_DROITE;
//else if (positionObstacle == OBSTACLE_EN_FACE)
//nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
//
////Si l?on n?est pas dans la transition de l?étape en cours
//if (nextStateRobot != stateRobot - 1)
//stateRobot = nextStateRobot;
}