// DHT11 sensor connection (here data pin is connected to RA0)
#define DHT11_PIN      PORTAbits.RA0
#define DHT11_PIN_DIR  TRISAbits.TRISA0

void start_signal_dht11(void);
__bit check_response_dht11(void);
__bit read_data_dht11(unsigned char* dht_data);
void dht11_check_humidity(char *ptr_h);

