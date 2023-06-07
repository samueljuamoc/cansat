#include "config.h"

int32_t adc_T, adc_P, t_fine;
int32_t var1, var2;
uint32_t p;

// writes 1 byte '_data' to register 'reg_addr'
void BMP280_Write(uint8_t reg_addr, uint8_t _data)
{
    I2C_start();
    I2C_Tx_data(BMP280_I2C_ADDRESS);
    I2C_Tx_data(reg_addr);
    I2C_Tx_data(_data);
    I2C_stop();
}

// reads 8 bits from register 'reg_addr'
uint8_t BMP280_Read8(uint8_t reg_addr)
{
    uint8_t ret;

    I2C_start();
    I2C_Tx_data(BMP280_I2C_ADDRESS);
    I2C_Tx_data(reg_addr);
    I2C_stop();
    I2C_start();
    I2C_Tx_data(BMP280_I2C_ADDRESS | 1);
    ret = I2C_Rx_data();
    I2C_Rx_nack();
    I2C_stop();
    return ret;
}

// reads 16 bits from register 'reg_addr'
uint16_t BMP280_Read16(uint8_t reg_addr)
{
    union
    {
      uint8_t  b[2];
      uint16_t w;
    } ret;

    I2C_start();
    I2C_Tx_data(BMP280_I2C_ADDRESS);
    I2C_Tx_data(reg_addr);
    I2C_stop();
    I2C_start();
    I2C_Tx_data(BMP280_I2C_ADDRESS | 1);
    ret.b[0] = I2C_Rx_data();
    I2C_Rx_ack();
    ret.b[1] = I2C_Rx_data();
    I2C_Rx_nack();
    I2C_stop();

    return(ret.w);
}
    
// initializes the BMP280 sensor, returns 1 if OK and 0 if error
bool BMP280_begin(uint8_t mode, //=MODE_NORMAL
                  uint8_t T_sampling , //= SAMPLING_X1
                  uint8_t P_sampling , //= SAMPLING_X1
                  uint8_t filter       , //= FILTER_OFF
                  uint8_t standby      ) //= STANDBY_0_5
{

    if(BMP280_Read8(BMP280_REG_CHIPID) != BMP280_CHIP_ID)
      return 0;

    // reset the BMP280 with soft reset
    BMP280_Write(BMP280_REG_SOFTRESET, 0xB6);
    __delay_ms(100);

    // if NVM data are being copied to image registers, wait 100 ms
    while ( (BMP280_Read8(BMP280_REG_STATUS) & 0x01) == 0x01 )
      __delay_ms(100);

    BMP280_calib.dig_T1 = BMP280_Read16(BMP280_REG_DIG_T1);
    BMP280_calib.dig_T2 = (int16_t)BMP280_Read16(BMP280_REG_DIG_T2);
    BMP280_calib.dig_T3 = (int16_t)BMP280_Read16(BMP280_REG_DIG_T3);

    BMP280_calib.dig_P1 = BMP280_Read16(BMP280_REG_DIG_P1);
    BMP280_calib.dig_P2 = (int16_t)BMP280_Read16(BMP280_REG_DIG_P2);
    BMP280_calib.dig_P3 = (int16_t)BMP280_Read16(BMP280_REG_DIG_P3);
    BMP280_calib.dig_P4 = (int16_t)BMP280_Read16(BMP280_REG_DIG_P4);
    BMP280_calib.dig_P5 = (int16_t)BMP280_Read16(BMP280_REG_DIG_P5);
    BMP280_calib.dig_P6 = (int16_t)BMP280_Read16(BMP280_REG_DIG_P6);
    BMP280_calib.dig_P7 = (int16_t)BMP280_Read16(BMP280_REG_DIG_P7);
    BMP280_calib.dig_P8 = (int16_t)BMP280_Read16(BMP280_REG_DIG_P8);
    BMP280_calib.dig_P9 = (int16_t)BMP280_Read16(BMP280_REG_DIG_P9);
    
    // BMP280 sensor configuration function
    uint8_t  _ctrl_meas, _config;
    _config = ((standby << 5) | (filter << 2)) & 0xFC;
    _ctrl_meas = (uint8_t)(T_sampling << 5) | (uint8_t)(P_sampling << 2) | (uint8_t)mode;

    BMP280_Write(BMP280_REG_CONFIG,  _config);
    BMP280_Write(BMP280_REG_CONTROL, _ctrl_meas);

    return 1;
}

// read (updates) adc_P, adc_T and adc_H from BMP280 sensor
void BMP280_Update()
{
    union
    {
      uint8_t  b[4];
      uint32_t dw;
    } ret;
    ret.b[3] = 0x00;

    I2C_start();
    I2C_Tx_data(BMP280_I2C_ADDRESS);
    I2C_Tx_data(BMP280_REG_PRESS_MSB);
    I2C_stop();
    I2C_start();
    I2C_Tx_data(BMP280_I2C_ADDRESS | 1);
    ret.b[2] = I2C_Rx_data();
    I2C_Rx_ack();
    ret.b[1] = I2C_Rx_data();
    I2C_Rx_ack();
    ret.b[0] = I2C_Rx_data();
    I2C_Rx_ack();

    adc_P = (ret.dw >> 4) & 0xFFFFF;  

    ret.b[2] = I2C_Rx_data();
    I2C_Rx_ack();
    ret.b[1] = I2C_Rx_data();
    I2C_Rx_ack();
    ret.b[0] = I2C_Rx_data();
    I2C_Rx_nack();
    I2C_stop();

    adc_T = (ret.dw >> 4) & 0xFFFFF;
}

// Reads temperature from BMP280 sensor.
// Temperature is stored in hundredths C (output value of "5123" equals 51.23 DegC).
// Temperature value is saved to *temp, returns 1 if OK and 0 if error.
bool BMP280_readTemperature(int32_t *temp)
{
    BMP280_Update();

    // calculate temperature
    var1 = ((((adc_T / 8) - ((int32_t)BMP280_calib.dig_T1 * 2))) *
           ((int32_t)BMP280_calib.dig_T2)) / 2048;

    var2 = (((((adc_T / 16) - ((int32_t)BMP280_calib.dig_T1)) *
           ((adc_T / 16) - ((int32_t)BMP280_calib.dig_T1))) / 4096) *
           ((int32_t)BMP280_calib.dig_T3)) / 16384;

    t_fine = var1 + var2;

    *temp = (t_fine * 5 + 128) / 256;
    

    return 1;
}

// Reads pressure from BMP280 sensor.
// Pressure is stored in Pa (output value of "96386" equals 96386 Pa = 963.86 hPa).
// Pressure value is saved to *pres, returns 1 if OK and 0 if error.
bool BMP280_readPressure(uint32_t *pres)
{
    // calculate pressure
    var1 = (((int32_t)t_fine) / 2) - 64000;
    var2 = (((var1/4) * (var1/4)) / 2048 ) * ((int32_t)BMP280_calib.dig_P6);

    var2 = var2 + ((var1 * ((int32_t)BMP280_calib.dig_P5)) * 2);
    var2 = (var2/4) + (((int32_t)BMP280_calib.dig_P4) * 65536);

    var1 = ((((int32_t)BMP280_calib.dig_P3 * (((var1/4) * (var1/4)) / 8192 )) / 8) +
           ((((int32_t)BMP280_calib.dig_P2) * var1)/2)) / 262144;

    var1 =((((32768 + var1)) * ((int32_t)BMP280_calib.dig_P1)) / 32768);

    if (var1 == 0)
      return 0; // avoid exception caused by division by zero

    p = (((uint32_t)((1048576) - adc_P) - (uint32_t)(var2 / 4096))) * 3125;

    if (p < 0x80000000)
      p = (p * 2) / ((uint32_t)var1);

    else
      p = (p / (uint32_t)var1) * 2;

    var1 = (((int32_t)BMP280_calib.dig_P9) * ((int32_t)(((p/8) * (p/8)) / 8192))) / 4096;
    var2 = (((int32_t)(p/4)) * ((int32_t)BMP280_calib.dig_P8)) / 8192;

    p = (uint32_t)((int32_t)p + ((var1 + var2 + (int32_t)BMP280_calib.dig_P7) / 16));

    *pres = p;

    return 1;
}
// end of code.