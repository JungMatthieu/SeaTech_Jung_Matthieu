#include <xc.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"
unsigned char UartCalculateChecksum(int msgFunction,int msgPayloadLength, unsigned char* msgPayload)
{
    unsigned char checksum = 0;
    checksum ^= 0xFE;
    checksum ^= (unsigned char) (msgFunction >> 8);
    checksum ^= (unsigned char) (msgFunction >> 0);
    checksum ^= (unsigned char) (msgPayloadLength >> 8);
    checksum ^= (unsigned char) (msgPayloadLength >> 0);

    for (int i = 0; i < msgPayloadLength; i++) {
        checksum ^= msgPayload[i];
    }

    return checksum;

}
void UartEncodeAndSendMessage(int msgFunction,int msgPayloadLength, unsigned char* msgPayload)
{
//Fonction d?encodage et d?envoi d?un message
        unsigned char message[6 + msgPayloadLength];
        unsigned char checksum = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
        int pos = 0;
        message[pos++] = 0xFE;
        message[pos++] = (unsigned char) (msgFunction >> 8);
        message[pos++] = (unsigned char) (msgFunction >> 0);
        message[pos++] = (unsigned char) (msgPayloadLength >> 8);
        message[pos++] = (unsigned char) (msgPayloadLength >> 0);

        for (int i = 0; i < msgPayloadLength; i++) {
            message[pos++] = msgPayload[i];
        }
        message[pos++] = checksum;
        //serialPort1.WriteLine("0xFE");
        SendMessage(message, pos);
        
}
enum StateReception
  {
           Waiting,
           FunctionMSB,
           FunctionLSB,
          PayloadLengthMSB,
          PayloadLengthLSB,
          Payload,
          CheckSum
  };
int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;
StateReception rcvState = Waiting;

void UartDecodeMessage(unsigned char c)
{
switch(rcvState)
            { 
                case StateReception.Waiting:
                    if (c == 0xFE)
                        rcvState = StateReception.FunctionMSB;
                    break;
                case StateReception.FunctionMSB:
                    msgDecodedFunction = c << 8;
                    rcvState = StateReception.FunctionLSB;
                    break;
                case StateReception.FunctionLSB:
                    msgDecodedFunction += c << 0;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;
                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = c << 8;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;
                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength += c << 0;
                    if (msgDecodedPayloadLength == 0)
                        rcvState = StateReception.CheckSum;
                    else
                    {
                        rcvState = StateReception.Payload;
                        msgDecodedPayload = new byte[msgDecodedPayloadLength];
                        msgDecodedPayloadIndex = 0;
                    }
                    break;
                case StateReception.Payload:                    
                    msgDecodedPayload[msgDecodedPayloadIndex] = c;
                    msgDecodedPayloadIndex++;
                    if (msgDecodedPayloadIndex == msgDecodedPayloadLength)
                        rcvState = StateReception.CheckSum;
                    break;
                case StateReception.CheckSum:
                    unsigned char receivedChecksum = c;
                    unsigned char calculatedChecksum = UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    if (calculatedChecksum == receivedChecksum)
                    {
                        SendMessage(" T'as gagné une glace");
                    }
                    else
                    {
                        SendMessage(" Mange une ATMega 328");
                    }
                    rcvState = StateReception.Waiting;
                    break;
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
    //Fonction prenant en entree un octet et servant a reconstituer les trames
    
}
//void UartProcessDecodedMessage(int function,int payloadLength, unsigned char* payload)
//{
//Fonction appelee apres le decodage pour executer l?action
//correspondant au message recu
//}
//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/
