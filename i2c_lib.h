//MACROS
#ifndef I2C_LIB_H
#define I2C_LIB_H

#define TRIS_SCL TRISBbits.RB1
#define TRIS_SDA TRISBbits.RB0

//FUNCIONES
void I2C_master_init(void);
void I2C_start(void);
void I2C_stop(void);
void I2C_Rx_ack(void);
void I2C_Rx_nack(void);
void I2C_restart(void);
void I2C_Tx_data(char data);
char I2C_Rx_data(void);

#endif