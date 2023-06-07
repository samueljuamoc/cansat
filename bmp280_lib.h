#ifndef BMP280_I2C_ADDRESS
  #define BMP280_I2C_ADDRESS  0xEC
#endif

#define BMP280_CHIP_ID        0x58

#define BMP280_REG_DIG_T1     0x88
#define BMP280_REG_DIG_T2     0x8A
#define BMP280_REG_DIG_T3     0x8C

#define BMP280_REG_DIG_P1     0x8E
#define BMP280_REG_DIG_P2     0x90
#define BMP280_REG_DIG_P3     0x92
#define BMP280_REG_DIG_P4     0x94
#define BMP280_REG_DIG_P5     0x96
#define BMP280_REG_DIG_P6     0x98
#define BMP280_REG_DIG_P7     0x9A
#define BMP280_REG_DIG_P8     0x9C
#define BMP280_REG_DIG_P9     0x9E

#define BMP280_REG_CHIPID     0xD0
#define BMP280_REG_SOFTRESET  0xE0

#define BMP280_REG_STATUS     0xF3
#define BMP280_REG_CONTROL    0xF4
#define BMP280_REG_CONFIG     0xF5
#define BMP280_REG_PRESS_MSB  0xF7

// BMP280 sensor modes, register ctrl_meas mode[1:0]
#define MODE_NORMAL 0x03   // normal mode

// oversampling setting. osrs_t[2:0], osrs_p[2:0]
#define SAMPLING_X1 0x01  // oversampling x1

// filter setting filter[2:0]
#define FILTER_OFF 0x00  // filter off

// standby (inactive) time in ms (used in normal mode), t_sb[2:0]
#define  STANDBY_0_5  0x00  // standby time = 0.5 ms

struct
{
  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;

  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;
  int16_t  dig_P6;
  int16_t  dig_P7;
  int16_t  dig_P8;
  int16_t  dig_P9;
} BMP280_calib;

void BMP280_Write(uint8_t reg_addr, uint8_t _data);
uint8_t BMP280_Read8(uint8_t reg_addr);
uint16_t BMP280_Read16(uint8_t reg_addr);

bool BMP280_begin(uint8_t mode,uint8_t T_sampling,uint8_t P_sampling,uint8_t filter,uint8_t standby);
bool BMP280_ForcedMeasurement();
void BMP280_Update();

bool BMP280_readTemperature(int32_t *temp);
bool BMP280_readPressure(uint32_t *pres);