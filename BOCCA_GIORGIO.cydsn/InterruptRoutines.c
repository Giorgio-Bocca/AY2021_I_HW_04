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
uint8 val=0;

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
        
            value_mv = ADC_DelSig_CountsTo_mVolts(value_digit);
            DC_Volt = value_mv;
            DC = (255*DC_Volt)/5000;
            DutyCycle = DC;
            PWM_WriteCompare(DutyCycle);
            val = (100*DutyCycle)/255;
            
            // Format ADC result for transmition
            sprintf(DataBuffer , "DutyCycle: %d\r\n",val);
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
