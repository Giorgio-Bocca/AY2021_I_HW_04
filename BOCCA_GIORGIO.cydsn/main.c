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
#include "project.h"
#include "InterruptRoutines.h"
#include "stdio.h"

uint8 channel=1;

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    ADC_DelSig_Start();
    UART_Start();
    isr_ADC_StartEx(Custom_ISR_ADC);
    isr_UART_StartEx(Custom_ISR_RX);
    PWM_Start();
    AMux_Start();
    AMux_FastSelect(channel);
    
    PacketReadyFlag = 0;
    
    ADC_DelSig_StartConvert();
    
    for(;;)
    {
        /* Place your application code here. */
        if(PacketReadyFlag == 1)
        {
            // Send out the data
            UART_PutString(DataBuffer);
            PacketReadyFlag = 0;
        }
    }
}
/* [] END OF FILE */
