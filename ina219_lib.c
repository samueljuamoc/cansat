#include "config.h"

uint8_t split[2];
uint16_t INA_2bytes = 0;
uint16_t value1;
float value2;
uint16_t calibration_value;

void ina219Init(void) {
    //calibration_value = 32764; // 0b 111 1111 1111 1111 = 0xFF 0xFF
    calibration_value = 0xFFFF; //32767
    //8192 = 0x20 0x00
    // Set Calibration register to 'Cal' calculated above
    I2C_INA_write(0x05,calibration_value);
    // Set Config register to take into account the settings below
    /*NOT RST
     *-
     *16V 400mA
     *+-40mV shunt
     *12bits/1 sample 532 us
     * shunt and bus continue
     */
    uint16_t reg_config = 0b0000000110011111; //0x01 0x9F
    I2C_INA_write(0x00,reg_config);
}

void I2C_INA_write(uint8_t registro,uint16_t valor){
    split[0] =  (valor >> 8) & 0xff;
    split[1] =         valor & 0xff;
    
    I2C_start();
    I2C_Tx_data(0x80);   
    I2C_Tx_data(registro);
    I2C_Tx_data(split[0]);
    I2C_Tx_data(split[1]);
    I2C_stop();
}

uint16_t I2C_INA_read(uint8_t registro){
    I2C_start();
    I2C_Tx_data(0x80);
    I2C_Tx_data(registro);
    I2C_stop();
    __delay_ms(10);
    I2C_start();
    I2C_Tx_data(0x81);  //address and R
    split[0] = I2C_Rx_data();
    I2C_Rx_ack();
    split[1] = I2C_Rx_data();
    I2C_Rx_nack();      //stop com
    I2C_stop();
    
    INA_2bytes = ((uint16_t)split[0] << 8) | split[1];
    
    return INA_2bytes;
}

float ina219GetBusVoltage_V(void) {
    value1 = I2C_INA_read(0x02);
    value1 = (value1 >> 3);
    value2 = ((float)value1)*((float)0.004);
    return value2;
}

float ina219GetCurrent_mA(void) {
    value1 = I2C_INA_read(0x04);

    value2 = ((float)value1)*((float)0.0063);
    return value2;
}
/*not used and not conditioned
float ina219GetShuntVoltage_mV(void) {
    value1 = (float)I2C_INA_read(0x01);
    value1 *= ((float)0.014);
    return value2;
}
*/
float ina219GetPower_mW(void) {
    value1 = I2C_INA_read(0x03);

    value2 = ((float)value1)*((float)0.126);
    return value2;
}