#include "config.h"  //configuration bits

void USART_Init(long BAUD){
    //configuracion de pines
    TRISCbits.RC6 = 0;   //RC6 = Tx -> salida
    TRISCbits.RC7 = 1;   //RC7 = Rx -> entrada
    
    //baudios
    SPBRG = (unsigned char)(((_XTAL_FREQ/BAUD)/64)-1);
    
    //configuración
    TXSTAbits.BRGH = 0; //baja velocidad
    TXSTAbits.SYNC = 0; //asincrono
    RCSTAbits.SPEN = 1; //habilitar Tx y Rx
    TXSTAbits.TXEN = 1; //Activamos transmisión
    //transmisión
    TXSTAbits.TX9 = 0;  //8bits de datos
    //recepción
    RCSTAbits.RC9 = 0;  //8 bits
    RCSTAbits.CREN = 1; //Activamos recepción
}

void putch(uint8_t data){
    while (!PIR1bits.TXIF)  //status of Transmit Shift Register
        continue;
    TXREG = data;
}

uint8_t getch(void){
    while (!PIR1bits.RCIF) 
        continue;  // wait for data receive
    
    if (OERR)  // if there is overrun error
    {  // clear overrun error bit
        CREN = 0;
        CREN = 1;
    }
    
    return RCREG;
}

void UART_Print(char *data){
  uint8_t i = 0;
  while (data[i] != '\0')
    putch(data[i++]);
}