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

// Variables declaration */
int32 value_digit;
int32 value_mv;
uint8 ch_receveid;
uint8 SendBytesFlag=0;
int DC;
int DC_Volt;
uint8 DutyCycle;

CY_ISR(Custom_ISR_ADC)
{
    // Read Timer status register to bring interrupt line low
   
    Timer_ReadStatusRegister();
    if(SendBytesFlag==1)
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
        // Format ADC result for transmition
        sprintf(DataBuffer , "Sample: %d\r\n",DutyCycle);
        PacketReadyFlag = 1;
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
