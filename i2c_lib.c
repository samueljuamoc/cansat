#include "config.h"

void I2C_master_init(void){
    TRIS_SCL = 1;
    TRIS_SDA = 1;
    //SSPSTAT
    SSPSTATbits.CKE = 0;    //Disable SMBus
    SSPSTATbits.SMP = 1;  //100kHz
    //bits 0-5 are read-only
    
    //SSPCON1
    SSPCON1bits.SSPEN = 1; //SDA y SCL. habilita el puerto serial
    SSPCON1bits.SSPM = 0b1000; //modo de puerto, maestro con clock = FOSC/(4 * (SSPADD + 1))
    SSPCON1bits.SSPOV = 0; //no overflow indicator
    SSPCON1bits.WCOL = 0; //no collision bit
        
    //SSPCON2
    SSPCON2 = 0x00;     //todavìa no se usa, master functions
    //SSPADD = ((Fosc)/(4*BAUDrate)) - 1
    
    //SSPADD = ((_XTAL_FREQ/(4*I2C_BaudRate)) - 1) & 0xFF;   //formula con 100kbps y 20MHz = 49
    SSPADD = 49;
}

void I2C_start(void){
    SSPCON2bits.SEN = 1;
    while(PIR1bits.SSPIF == 0);
}

void I2C_stop(void){
    PIR1bits.SSPIF = 0;//flag cleared
    SSPCON2bits.PEN = 1;
    while(PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
}

void I2C_restart(void){
    SSPCON2bits.SEN = 1;
    while(SSPCON2bits.SEN == 1);
}

void I2C_Rx_ack(void){
    SSPCON2bits.ACKDT = 0;  //start ack sequence
    PIR1bits.SSPIF = 0; //clean flag
    SSPCON2bits.ACKEN = 1;  //envio ack
    while(PIR1bits.SSPIF == 0);  //wait for ack process
    PIR1bits.SSPIF = 0; //clean flag
}

void I2C_Rx_nack(void){
    SSPCON2bits.ACKDT = 1;  //start nack sequence
    PIR1bits.SSPIF = 0; //clean flag
    SSPCON2bits.ACKEN = 1;  //envio nack
    while(PIR1bits.SSPIF == 0);  //wait for nack process
}

void I2C_Tx_data(char data){
    SSPBUF = data;  //address and R/W bit
    PIR1bits.SSPIF = 0;//flag cleared
    while(PIR1bits.SSPIF == 0);
    //ACK
    if(SSPCON2bits.ACKSTAT == 1)
      I2C_stop(); 
}

char I2C_Rx_data(void){
    SSPCON2bits.RCEN = 1;
    PIR1bits.SSPIF = 0; //clean flag
    while(PIR1bits.SSPIF == 0);    
    return SSPBUF;
}