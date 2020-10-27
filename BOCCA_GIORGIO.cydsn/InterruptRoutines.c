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

#define SOGLIA 4000

// Variables declaration */
int32 value_digit;
int32 value_mv;
uint8 ch_receveid;
uint8 SendBytesFlag=0;
int DC;
int DC_Volt;
uint8 DutyCycle;
int32 value_photo_digit;
int32 value_mv_photo;
extern uint8 channel;
int Flag=0;
uint16 val=0;
uint16 array;

CY_ISR(Custom_ISR_ADC)
{
    // Read Timer status register to bring interrupt line low
   
    Timer_ReadStatusRegister();
    if(SendBytesFlag==1)
    {
        if(Flag==0)
        {
            value_photo_digit = ADC_DelSig_Read32(); 
            value_mv_photo = ADC_DelSig_CountsTo_mVolts(value_photo_digit);            
            if(value_mv_photo >= SOGLIA)
            {
                Flag=1;
                channel=1;
                AMux_FastSelect(channel);
            }
            else
            {
                PWM_WriteCompare(0);
            }
        }
        
        else if(Flag==1)
        {
            value_digit = ADC_DelSig_Read32();
            if(value_digit < 0)
            {
                value_digit = 0;
            }
            else if(value_digit > 65535) 
            {
                value_digit = 65535;
            }   
           
            array = value_digit >> 8; 
            PWM_WriteCompare(array);
            val = (100*array)/255;
            
            DataBuffer1[1]=array;
            DataBuffer1[2]=array >> 8;   
            DataBuffer1[3]=value_photo_digit >> 8;
            DataBuffer1[4]=value_photo_digit & 0xFF;
            // Format ADC result for transmition
            //sprintf(String,"DutyCycle: %d\r\n",val);
            PacketReadyFlag = 1;
         }
    }
}

CY_ISR(Custom_ISR_RX)
{
    // Non blocking call to get the latest data recieved
    ch_receveid = UART_GetChar();
    // Set flags based on UART command
    switch(ch_receveid)
    {
        case 'B':
        
        case 'b': 
            SendBytesFlag = 1;
            Timer_Start();
            break;

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
