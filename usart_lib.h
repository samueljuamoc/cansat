void USART_Init(long BAUD);
void putch(uint8_t data);//if called like this we can use the stdout printf
uint8_t getch();    //if called like this we can use the stdin scanf
void UART_Print(char *data);