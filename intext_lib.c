
#include "config.h"


void int_ext_enable(void){
    
    INTCONbits.GIEH=1;                          //Global Interrupt Enable bit
    INTCONbits.GIEL=1;                          // Enables all unmasked peripheral interrupts
    RCONbits.IPEN=0;                            //Disable priority levels on interrupts
    
    TRISBbits.RB2 = 1; //bit B2 como entrada
    INTCON3bits.INT2IE = 1; //habilitar INT2
    INTCON3bits.INT2IF = 0; //clean flag (0)
    INTCON2bits.INTEDG2 = 0; //interrupt on falling edge 
}


