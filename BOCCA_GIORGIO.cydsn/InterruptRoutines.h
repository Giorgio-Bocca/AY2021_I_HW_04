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
#ifndef __INTERRUPT_ROUTINES_H
    #define __INTERRUPT_ROUTINES_H

    #include "cytypes.h"
    #include "stdio.h"

    #define BYTE_TO_SEND 4
    #define TRANSMIT_BUFFER_SIZE 1+BYTE_TO_SEND+1

    CY_ISR_PROTO(Custom_ISR_ADC);
    CY_ISR_PROTO(Custom_ISR_RX);

    uint8 DataBuffer1[TRANSMIT_BUFFER_SIZE];
    //char String[16];
    volatile uint8 PacketReadyFlag;
#endif

/* [] END OF FILE */