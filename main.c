#include "config.h"


uint8_t i;

float voltaje,corriente,potencia;

int32_t temp;
uint32_t pres;

char humidity[]    = "Humedad (HR%):  00";
char *ptr_h = humidity; //points to the R of Humidity[]

uint8_t time[12]={0};
uint8_t date[7]={0};
uint8_t nmea_log[7]={0};
uint8_t lat[12]={0};
uint8_t lon[13]={0};
uint8_t alt[10]={0};
uint8_t speed[8]={0};
uint8_t course[8]={0};
uint8_t satel[4]={0};
uint8_t stream[1]={0};  //62 character-stream check nmea_log

bool boton;
char lcd_text[18];

char description[20]={0};
float tx_variable;


void main(void) {
    
    ////////////////DHT11
    PORTA = 0b00000000;// clear latches    
    ADCON1bits.PCFG = 0b1111;     //set D-I/O all pins
    //T1CON es nuestro registro pa configurar TMR1
    //T1CON  = 0b11001000;          // as TMR1 clock=5MHz, each step means 0.2us
    T1CONbits.RD16 = 1;             //16 bit R/W (1)
    T1CONbits.T1RUN = 1;            //TMR1  oscillator device clock (1)
    T1CONbits.T1CKPS = 0b00;        //prescale value (00) bit 5-4(Timer1 clock = 5MHz)
    T1CONbits.T1OSCEN = 1;          //TMR1 oscillator enable (1)
    //T1CONbits.T1SYNC bit ignored  //Ignored (0)  
    T1CONbits.TMR1CS = 0;           //Internal clock (Fosc/4) (0)
    T1CONbits.TMR1ON = 0;           //TMR1 set or stop (0)
    TMR1H  = 0;           // reset Timer1
    TMR1L  = 0;
    __delay_ms(100);
    ////////////////USART
    USART_Init(9600);
    __delay_ms(100);
    ////////////////I2C
    I2C_master_init();
    __delay_ms(100);
    ////////////////INA219
    ina219Init();
    __delay_ms(100);
    ////////////////BMP280
    if(BMP280_begin(MODE_NORMAL,SAMPLING_X1,SAMPLING_X1,FILTER_OFF,STANDBY_0_5) == 0)
    {  // connection error or device address wrong!
        printf("Connection error");
        while (1)   // stay here!
            continue;
    }
    __delay_ms(100);
    ////////////////GPS
    __delay_ms(100);
    ////////////////EXT INTERRUPT
    int_ext_enable();
    __delay_ms(100);
    ////////////////LCD
    LCD_Begin(0x4E);    // Initialize LCD module with I2C address = 0x4E
    __delay_ms(100);
    
    ////////////////CREATOR
    LCD_Goto(1, 1);     // Go to column 1 row 1
    LCD_Print("     CANSAT     ");
    LCD_Goto(1, 2);     // Go to column 1 row 2
    LCD_Print("  BY SAMUEL JM  ");
    UART_Print("     CANSAT     \r\n");
    UART_Print("  BY SAMUEL JM  \r\n\r\n");
    __delay_ms(10000);
    
    
    while(1){
        
        voltaje = ina219GetBusVoltage_V();
        corriente = ina219GetCurrent_mA();
        potencia = ina219GetPower_mW();
        
        BMP280_readTemperature(&temp);  // read temperature (ºC*100)
        BMP280_readPressure(&pres);     // read pressure (Pa)
        
        dht11_check_humidity(ptr_h);
        
        while(!(GPSRead_rmc(time,date,nmea_log,lat,lon,speed,course,stream)))
            continue;
        while(!(GPSRead_gga(alt,satel,nmea_log,stream)))
            continue;

        
        for(i=0;i<5;i++){
            switch(i){
                case 0:
                    tx_variable = voltaje;
                    memcpy(description, "Voltaje (V):    ", 18);
                    break;
                case 1:
                    tx_variable = corriente;
                    memcpy(description, "Corriente (mA): ", 18);
                    break;
                case 2:
                    tx_variable = potencia;
                    memcpy(description, "Potencia (mW):  ", 18);
                    break;
                case 3:
                    tx_variable = (float)temp/(float)100;
                    memcpy(description, "Temperatura (C):", 18);
                    break;
                case 4:
                    tx_variable = (float)pres/(float)100;
                    memcpy(description, "PresionAtm(hPa):", 18);
                    break;
                default:
                    //do nothing
                    break;
            }
            printf("%s  %.4f\r\n",description, tx_variable);
            
            if(boton == 1){ //lcd V,I,Po,T,Pr
                LCD_Goto(1, 1);      // go to column 1, row 1
                LCD_Print(description);     // print 'text'
                
                sprintf(lcd_text,"%.4f         ",tx_variable);
                LCD_Goto(1, 2);      // go to column 1, row 2
                LCD_Print(lcd_text);     // print 'text'
                __delay_ms(4000);
            }
        }
        
        UART_Print(humidity);
        UART_Print("\r\n");
        
        if(boton == 1){ //lcd humidity
            for(i=0;i<16;i++){
                lcd_text[i] = humidity[i];
            }
            LCD_Goto(1, 1);      // go to column 1, row 1
            LCD_Print(lcd_text);     // print 'text'
            
            for(i=16;i<18;i++){
                lcd_text[i-16] = humidity[i];
            }
            for(i=2;i<16;i++){
                lcd_text[i] = ' ';
            }
            LCD_Goto(1, 2);      // go to column 1, row 2
            LCD_Print(lcd_text);     // print 'text'
            __delay_ms(4000);
        }
        
        
        //UTC time       (hh:mm:ss)
        printf("Time(UTC): %c%c:%c%c:%c%c\r\n",time[0],time[1],time[2],time[3],time[4],time[5]);
        //Date           (dd/mm/yy) 
        printf("Date:      %c%c/%c%c/%c%c\r\n",date[0],date[1],date[2],date[3],date[4],date[5]);
        //LAT: 2037.62702,N  ->  50°39.734? N
        printf("Latitude:  %c%c°%c%c.%c%c%c' %c\r\n",lat[0],lat[1],lat[2],lat[3],lat[5],lat[6],lat[7],lat[10]);
        //LON: 10024.18961,W  ->  001°35.500? W
        printf("Longitude: %c%c%c°%c%c.%c%c%c' %c\r\n",lon[0],lon[1],lon[2],lon[3],lon[4],lon[6],lon[7],lon[8],lon[11]);
        // 1040.0 msnm
        printf("Altitude:  %s msnm\r\n",alt);
        //0.716 kn
        printf("Speed:     %s kn\r\n",speed);
        printf("Course:    %s °\r\n",course);
        //09 sat
        printf("Satellites:%s units\r\n",satel);
        
        if(boton == 1){ //lcd GPS variables
            memcpy(lcd_text, "Time(UTC):      ", 17);
            LCD_Goto(1, 1);      // go to column 1, row 1
            LCD_Print(lcd_text);     // print 'text'
            sprintf(lcd_text,"%c%c:%c%c:%c%c        ",time[0],time[1],time[2],time[3],time[4],time[5]);
            LCD_Goto(1, 2);      // go to column 1, row 2
            LCD_Print(lcd_text);     // print 'text'
            __delay_ms(4000);
            
            memcpy(lcd_text, "Date:           ", 17);
            LCD_Goto(1, 1);      // go to column 1, row 1
            LCD_Print(lcd_text);     // print 'text'
            sprintf(lcd_text,"%c%c/%c%c/%c%c        ",date[0],date[1],date[2],date[3],date[4],date[5]);
            LCD_Goto(1, 2);      // go to column 1, row 2
            LCD_Print(lcd_text);     // print 'text'
            __delay_ms(4000);
            
            memcpy(lcd_text, "Latitude:       ", 17);
            LCD_Goto(1, 1);      // go to column 1, row 1
            LCD_Print(lcd_text);     // print 'text'
            sprintf(lcd_text,"%c%c°%c%c.%c%c%c' %c    ",lat[0],lat[1],lat[2],lat[3],lat[5],lat[6],lat[7],lat[10]);
            LCD_Goto(1, 2);      // go to column 1, row 2
            LCD_Print(lcd_text);     // print 'text'
            __delay_ms(4000);
            
            memcpy(lcd_text, "Longitude:      ", 17);
            LCD_Goto(1, 1);      // go to column 1, row 1
            LCD_Print(lcd_text);     // print 'text'
            sprintf(lcd_text,"%c%c%c°%c%c.%c%c%c' %c   ",lon[0],lon[1],lon[2],lon[3],lon[4],lon[6],lon[7],lon[8],lon[11]);
            LCD_Goto(1, 2);      // go to column 1, row 2
            LCD_Print(lcd_text);     // print 'text'
            __delay_ms(4000);
            
            memcpy(lcd_text, "Altitude:       ", 17);
            LCD_Goto(1, 1);      // go to column 1, row 1
            LCD_Print(lcd_text);     // print 'text'
            sprintf(lcd_text,"%s msnm  ",alt);
            LCD_Goto(1, 2);      // go to column 1, row 2
            LCD_Print(lcd_text);     // print 'text'
            __delay_ms(4000);
            
            memcpy(lcd_text, "Speed:          ", 17);
            LCD_Goto(1, 1);      // go to column 1, row 1
            LCD_Print(lcd_text);     // print 'text'
            sprintf(lcd_text,"%s kn      ",speed);
            LCD_Goto(1, 2);      // go to column 1, row 2
            LCD_Print(lcd_text);     // print 'text'
            __delay_ms(4000);
            
            memcpy(lcd_text, "Course:", 17);
            LCD_Goto(1, 1);      // go to column 1, row 1
            LCD_Print(lcd_text);     // print 'text'
            sprintf(lcd_text,"%s degrees ",course);
            LCD_Goto(1, 2);      // go to column 1, row 2
            LCD_Print(lcd_text);     // print 'text'
            __delay_ms(4000);
            
            memcpy(lcd_text, "Satellites:     ", 17);
            LCD_Goto(1, 1);      // go to column 1, row 1
            LCD_Print(lcd_text);     // print 'text'
            sprintf(lcd_text,"%s units       ",satel);
            LCD_Goto(1, 2);      // go to column 1, row 2
            LCD_Print(lcd_text);     // print 'text'
            __delay_ms(4000);
            
            LCD_Goto(1, 1);     // Go to column 1 row 1
            LCD_Print("                ");
            LCD_Goto(1, 2);     // Go to column 1 row 2
            LCD_Print("                ");
            
            boton = 0;
        }
        
        
        UART_Print("\r\n\r\n");
        __delay_ms(5000); 
    }
}


//ISR(Interrupt Service Routine) of XC8 compiler has this format
__interrupt() void ISR1(void){
    boton = 1; //set bit of interrupt
    INTCON3bits.INT2IF = 0; //clean flag
}
// end of code.