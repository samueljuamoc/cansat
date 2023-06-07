#include "config.h"

unsigned char T_Byte1, T_Byte2, RH_Byte1, RH_Byte2, CheckSum ;
uint16_t TMR1_VALUE;

// send start signal to the sensor
void start_signal_dht11(void) {
  DHT11_PIN_DIR = 0;     // configure DHT11_PIN as output
  DHT11_PIN = 0;         // clear DHT11_PIN output (logic 0)
 
  __delay_ms(25);        // wait 18 ms at least
  DHT11_PIN = 1;         // set DHT11_PIN output (logic 1)
 
  __delay_us(30);        // wait 30 us
  DHT11_PIN_DIR = 1;     // configure DHT11_PIN as input
}

// Check sensor response
__bit check_response_dht11(void) {
    TMR1H = 0;                 // reset Timer1
    TMR1L = 0;

    T1CONbits.TMR1ON = 1;                // enable Timer1 module
    //here 100us = 500
    
    while(!DHT11_PIN && ((TMR1L  | ((uint16_t)TMR1H << 8)) < 500)); //  // wait until DHT11_PIN becomes high (checking of 80µs low time response)
    if((TMR1L  | ((uint16_t)TMR1H << 8)) > 500){                     // if response time > 99µS  ==> Response error
        return 0;                        // return 0 (Device has a problem with response)
    }
       
    else
    {
        TMR1H = 0;               // reset Timer1 (H & L bytes)
        TMR1L = 0;
        while(DHT11_PIN && ((TMR1L  | ((uint16_t)TMR1H << 8)) < 500));// // wait until DHT11_PIN becomes low (checking of 80µs high time response)
        
        if((TMR1L  | ((uint16_t)TMR1H << 8)) > 500){                   // if response time > 99µS  ==> Response error
            return 0;                      // return 0 (Device has a problem with response)
        }
        else
            return 1;                      // return 1 (response OK)
    }
}

// Data read function
__bit read_data_dht11(unsigned char* dht_data)
{
    *dht_data = 0;

    for(char i = 0; i < 8; i++)
    {
        TMR1H = 0;             // reset Timer1
        TMR1L = 0;

        while(!DHT11_PIN)      // wait until DHT11_PIN becomes high
            continue;
        
        if((TMR1L  | ((uint16_t)TMR1H << 8)) > 300) {    // if low time > 100  ==>  Time out error (Normally it takes 50µs)
            return 1;
        }

        TMR1H = 0;             // reset Timer1
        TMR1L = 0;

        while(DHT11_PIN)       // wait until DHT11_PIN becomes low
            continue;
        if((TMR1L  | ((uint16_t)TMR1H << 8)) > 500) {    // if high time > 100  ==>  Time out error (Normally it takes 26-28µs for 0 and 70µs for 1)
            return 1;          // return 1 (timeout error)
        }

        else if((TMR1L  | ((uint16_t)TMR1H << 8)) > 200)                  // if high time > 50  ==>  Sensor sent 1
            *dht_data |= (1 << (7 - i));  // set bit (7 - i)
        
    }
    return 0;                          // return 0 (data read OK)
}

void dht11_check_humidity(char *ptr_h){
    
    start_signal_dht11();
        
    if(check_response_dht11())    // check if there is a response from sensor (If OK start reading humidity and temperature data)
    {
        // read (and save) data from the DHT11 sensor and check time out errors
        if(read_data_dht11(&RH_Byte1) || read_data_dht11(&RH_Byte2) || read_data_dht11(&T_Byte1) 
                                      || read_data_dht11(&T_Byte2)  || read_data_dht11(&CheckSum))
        {
          UART_Print("Time out!\r\n");   // display "Time out!"
        }

        else         // if there is no time out error
        {
            if(CheckSum == ((RH_Byte1 + RH_Byte2 + T_Byte1 + T_Byte2) & 0xFF))
            {                                       // if there is no checksum error
                *(ptr_h+16)     = RH_Byte1 / 10 + '0'; //give me the value of integers of humidity
                *(ptr_h+17)     = RH_Byte1 % 10 + '0';
            }

            // if there is a checksum error
            else
            {
                UART_Print("Checksum Error!\r\n");
            }
        }
    }
    // if there is a response (from the sensor) problem
    else
    {
      UART_Print("No response from the sensor\r\n");
      //sometimes the communication fails and could be better to comment this line above
    }
    
    T1CONbits.TMR1ON = 0;        // stop Timer1 module
}