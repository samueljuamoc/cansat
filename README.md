# CanSat
This is the code used for a CanSat system using the PIC18F4550 microcontroller.
All the files included here are the content of the .X MPLAB project directory. XC8 compiler was used.

We have the following libraries:
- i2c_lib.h : This library allows us to communicate with sensors using the I2C protocol.
- usart_lib.h : This library allows us to communicate with the GPS and the SiK Telemetry Radios using the UART protocol.
- bmp280_lib.h : This library allows us to communicate with the BMP280 sensor and get the temperature and pressure variables.
- dht11_lib.h : This library allows us to communicate with the DHT11 sensor and get the humidity variable.
- ina219_lib.h : This library allows us to communicate with the INA219 sensor and get the voltage, current and power variables.
- gps_lib.h : This library allows us to communicate with the GPS NEO 6M and get the time(UTC), date, latitude, longitude, altitude, speed, course, and satellites connected to variables.
- intext_lib.h : This library allows us to use an external interruption, this in order to read when a mechanical switch is pressed.
- lcd_lib.h : This library allows us to use a HD44780 16x2 LCD through the PCF8574T I2C module, this in order to present the data directly on the satellite when the switch's pressed.

The config.h header helps with the processor files of the PIC18F4550, as well as the Configuration Bits, the constant of the clock and all the inclusion of libraries.
The main.c is the code of the system: Inclusion of config.h file, defining variables to be used, initializing everything, and looping reading sensors and transmitting data to the SiK telemetry radio and LCD if necessary.
