/* ========================================
 *\file main.c
 *\brief main file for project ASSIGNMENT 04:THE DEVELOPMENT
 * OF A SMART LAMP PROTOTYPE THROUGH A LED, A POTENTIOMETER
 * AND A SERIAL PORT COMMUNICATION
 * 
 * The project shows how to regulate the intensity of the light of a LED through a
 * potentiometer. The system turns ON/OFF the LED and samples the value of the potentiometer 
 * on the basis of a reasonable brightness threshold of a photoresistor. The product 
 * is started remotely through a serial port communication. The two analog signals
 * are plotted on the Bridge Control Panel and saved for diagnostic purposes 
 * 
 *\author Giorgio Bocca
 * ========================================
*/
#include "project.h"
#include "InterruptRoutines.h"
#include "stdio.h"

uint8 channel=0; //Variabile che indica il canale del AMux campionato dall'ADC

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    //Inizializzo l'ADC
    ADC_DelSig_Start();
    //Inizializzo l'UART
    UART_Start();
    //Inizializzo le ISR
    isr_ADC_StartEx(Custom_ISR_ADC);
    isr_UART_StartEx(Custom_ISR_RX);
    //Inizializzo il PWM
    PWM_Start();
    //Inizializzo il AMux
    AMux_Start();
    AMux_FastSelect(channel);
    
    PacketReadyFlag = 0;
    
    ADC_DelSig_StartConvert();
    
    //Setto i bytes di header e tail
    DataBuffer1[0] = 0xA0;
    DataBuffer1[TRANSMIT_BUFFER_SIZE-1] = 0xC0;
      
    for(;;)
    {
        /* Place your application code here. */
        if(PacketReadyFlag==1 && channel==1) //Se il pacchetto dati è pronto e sto campionando il potenziometro...
        {
            //Invio i dati al BCP e torno a campionare il fotoresistore
            UART_PutArray(DataBuffer1,TRANSMIT_BUFFER_SIZE);
            PacketReadyFlag = 0;
            channel=0;
            AMux_FastSelect(channel);
        }
    }
}
/* [] END OF FILE */
