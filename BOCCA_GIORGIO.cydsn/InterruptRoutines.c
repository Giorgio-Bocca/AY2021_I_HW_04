/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
// Include header
#include "InterruptRoutines.h"
// Include required header files
#include "project.h"

#define SOGLIA 4000 //Setto la soglia (in mV) del fotoresistore

// Variables declaration */
int32 value_pot_digit; //Variabile nella quale salvo il valore campionato dal potenziometro
uint8 ch_received; //Variabile nella quale salvo salvo il valore campionato dal potenziometro e convertito in mV
uint8 SendBytesFlag=0; //Variabile utilizzata per abilitare l'invio dei dati tramite UART
int32 value_photo_digit; //Variabile nella quela salvo il valore campionato dal fotoresistore
int32 value_mv_photo; //Variabile nella quale salvo il valore campionato dal fotoresistore e convertito in mV
extern uint8 channel;
uint16 array_pot; //Variabile utilizzata per ricavare il DutyCycle del PWM in bytes

CY_ISR(Custom_ISR_ADC)
{
    // Read Timer status register to bring interrupt line low
    Timer_ReadStatusRegister();
    if(SendBytesFlag==1)//Se abilito l'invio dei dati tramite UART...
    {
        if(channel==0)//Prima campiono il segnale del fotoresistore...
        {
            value_photo_digit = ADC_DelSig_Read32(); //Leggo il valore dall'ADC
            value_mv_photo = ADC_DelSig_CountsTo_mVolts(value_photo_digit); //Converto il valore letto in mV            
            if(value_mv_photo >= SOGLIA)
            {
                //Se il valore del fotoresistore supera la soglia, abilito il campionamento del segnale del potenziometro
                channel=1;
                AMux_FastSelect(channel);
            }
            else
            {    
                //Se il valore del fotoresistore non supera la soglia, il LED è OFF
                PWM_WriteCompare(0);
            }
        }
        
        else if(channel==1) //Se campiono il segnale del potenziometro...
        {
            value_pot_digit = ADC_DelSig_Read32(); //Leggo il valore dall'ADC
            if(value_pot_digit < 0)
            {
                value_pot_digit = 0;
            }
            else if(value_pot_digit > 65535) 
            {
                value_pot_digit = 65535;
            }   
           
            array_pot = value_pot_digit >> 8; 
            PWM_WriteCompare(array_pot); //Setto il DutyCycle del PWM in base al valore campionato dal potenziometro
            
            // Format ADC result for transmition: creo i bytes del pacchetto da inviare al BCP per il plot dei segnali
            DataBuffer1[1]=array_pot;
            DataBuffer1[2]=array_pot >> 8;   
            DataBuffer1[3]=value_photo_digit >> 8;
            DataBuffer1[4]=value_photo_digit & 0xFF;
                      
            PacketReadyFlag = 1;
         }
    }
}

CY_ISR(Custom_ISR_RX)
{
    // Non blocking call to get the latest data receveid
    ch_received = UART_GetChar();
    // Set flags based on UART command
    switch(ch_received)
    {
        //Se invio 'B' o 'b' abilito il campionamento e l'invio dei dati al BCP
        case 'B':
        
        case 'b': 
            SendBytesFlag = 1; 
            Timer_Start();
            break;
        //Se invio 'S' o 's' blocco il campionamento e l'invio dei dati al BCP
        case 'S':

        case 's':
            SendBytesFlag = 0;
            Timer_Stop();
            break;

        default:
            break;
    }   
}

/* [] END OF FILE */
