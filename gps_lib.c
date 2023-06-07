/*
 * GPRMS & GPGGA needed
 * 
 * $GPRMC: Recommended minimum specific GPS/Transit data
 * With RMC log we will receive:
 * - UTC time       (hh:mm:ss.ss)
 * - Date           (dd/mm/yy)
 * - Latitude       (DDmm.mm N/S)
 * - Longitude      (DDDmm.mm E/W)
 * - Speed          (x.x kmph)
 * - Course         (x.x degrees)
 * Example of a log:
 * $GPRMC,161229.487,A,3723.2475,N,12158.3416,W,0.13,309.62,120598,,*10
 * 
 * $GPGGA: System Fix Data and undulation
 * With GGA log we will receive:
 * - Altitude       (xxxx.xx msnm)
 * - No. satellites (xx)
 * Example of a log: 
 * $GPGGA,202530.00,5109.0262,N,11401.8407,W,5,40,0.5,1097.36,M,-17.00,M,18,TSTR*61
 * 
 * As an example of coordinates:
 * The Needles lighthouse is at 50°39.734? N 001°35.500? W
 * 
 * Example of log received by GPS NEO 6M
 * $GPRMC,190332.00,A,1903.81702,N,10038.18961,W,0.716,,160423,,,A*61
 * $GPGGA,190333.00,1903.81709,N,10038.18962,W,1,08,1.01,1869.7,M,-9.0,
*/
#include "config.h"

const char gps_rmc[] = "GPRMC";
const char gps_gga[] = "GPGGA";

uint8_t ch = 0;
uint8_t j = 0;
uint8_t change_array = 0;

__bit GPSRead_rmc(  uint8_t *time, uint8_t *date,uint8_t *nmea_log,uint8_t *lat, 
                    uint8_t *lon, uint8_t *speed,uint8_t *course, uint8_t *stream)
{  
    ch = getch();       // read the uartport, assign value to ch
    
    if (ch == '$')      // check if we have a $-sign, then we know it is the start of a sentence
    {
        for (j=0;j<5;j++) // read the 6 first chars after the $
        {
            ch = getch(); // read one char
            nmea_log[j] = ch; // put it in nmea_log array
        }
        
        change_array = 0;       //variable helping changing of array (type of data)
        
        if (strcmp((const char *)nmea_log, (const char *)gps_rmc)==0) // check if nmea_log is equal to our micros
        {
            while(change_array < 10){ //while the data isnt finished
                
                ch = getch();
                
                if(ch == ','){
                    change_array++;
                    if((change_array != 4)&&(change_array != 6)){//conserving N/S & W/E
                        j=0;
                    }
                }
                else{
                    switch(change_array){
                        case 1:
                            time[j++] = ch;
                        break;
                        case 3:
                        case 4:
                            lat[j++] = ch;
                        break;
                        case 5:
                        case 6:
                            lon[j++] = ch;
                        break;
                        case 7:
                            speed[j++] = ch;
                        break;
                        case 8:
                            course[j++] = ch;
                        break;
                        case 9:
                            date[j++] = ch;
                        break;
                        default:
                            //do nothing
                        break;     
                    }
                }
            }
            return 1; 
        } // end if strcmp       
    } // end if $
    //another character
    return 0;
}

__bit GPSRead_gga(uint8_t *alt,uint8_t *satel,uint8_t *nmea_log,uint8_t *stream)
{  
    ch = getch();       // read the uartport, assign value to ch
    
    if (ch == '$')      // check if we have a $-sign, then we know it is the start of a sentence
    {
        for (j=0;j<5;j++) // read the 6 first chars after the $
        {
            ch = getch(); // read one char
            nmea_log[j] = ch; // put it in nmea_log array
        }
        change_array = 0;       //variable helping changing of array
        //////////////////////////////////////////////////
        //altitude and no. of satellites
        if (strcmp((const char *)nmea_log, (const char *)gps_gga)==0)
        {
            while(change_array < 10){ //while the data isnt finished
                
                ch = getch();
                
                if(ch == ','){
                    change_array++;
                    j=0;
                }
                else{
                    switch(change_array){
                        case 7:
                            satel[j++] = ch;
                        break;
                        case 9:
                            alt[j++] = ch;
                        break;
                        
                        default:
                            //do nothing
                        break;     
                    }
                }     
            }
            return 1;
        } 
    } // end if $
    //another character
    return 0;
}