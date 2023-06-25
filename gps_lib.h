#ifndef GPS_LIB_H
#define GPS_LIB_H

__bit GPSRead_rmc(  uint8_t *time, uint8_t *date,uint8_t *buffer,uint8_t *lat, 
                    uint8_t *lon,uint8_t *speed,uint8_t *course,uint8_t *stream);

__bit GPSRead_gga(uint8_t *alt,uint8_t *satel,uint8_t *buffer,uint8_t *stream);

#endif