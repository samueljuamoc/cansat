#ifndef INA219_LIB_H
#define INA219_LIB_H

void ina219Init(void);

void I2C_INA_write(uint8_t registro,uint16_t valor);
uint16_t I2C_INA_read(uint8_t registro);

float ina219GetBusVoltage_V(void);
//float ina219GetShuntVoltage_mV(void);
float ina219GetCurrent_mA(void);
float ina219GetPower_mW();

#endif