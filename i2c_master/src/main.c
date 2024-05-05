#include <stm32f4xx.h>
#include <gpio.h>
#include <uart.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <i2c.h>
#include <delay.h>

#ifdef USE_UART_PRINTF
#include <stdio.h>
int _write(int fd, char *ptr, int len)
{
    (void)fd;
    if (fd == 1)
        uart_write(USART2, (uint8_t *)ptr, (uint32_t)len);
    return -1;
}
#define UART_LOG(msg...) printf(msg);
#else
#define UART_LOG(msg...) ;
#endif

#define RX_BUFFER_SIZE 64
#define add_slave1 0x55
#define add_slave2 0x42

// Buffer to store received data
volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint32_t rx_index = 0;
volatile bool rx_buffer_overflow = false;
volatile bool rx_newline = false;



void clear_rx_buffer()
{
    for (size_t i = 0; i < RX_BUFFER_SIZE; i++)
    {
        rx_buffer[i] = 0;
    }
    rx_index = 0;
}

typedef struct Stats
{
    uint8_t received;
    uint8_t invalid;
} stats_t;

// init struct for statistics
stats_t stats = {0, 0};

typedef enum
{
    CMD_INVALID,
    CMD_LED11_ON,
    CMD_LED12_ON,
    CMD_LED21_ON,
    CMD_LED22_ON,
    CMD_LED11_OFF,
    CMD_LED12_OFF,
    CMD_LED21_OFF,
    CMD_LED22_OFF,
    CMD_ALL_LED1_ON,
    CMD_ALL_LED2_ON,
    CMD_ALL_LED1_OFF,
    CMD_ALL_LED2_OFF,
    CMD_slave1_state,
    CMD_slave2_state,
} SERIAL_COMMAND_t;

SERIAL_COMMAND_t process_input(volatile uint8_t *buf)
{
    //Eingabe mit möglichen Befehlen vergleichen und Rückgabe eines Enum Wertes
    if (strncmp((const char *)buf, "slave 1 led 1 on\r\n", 18) == 0)
    {
        return CMD_LED11_ON;
    }
    else if (strncmp((const char *)buf, "slave 1 led 2 on\r\n", 18) == 0)
    {
        return CMD_LED12_ON;
    }
    else if (strncmp((const char *)buf, "slave 2 led 1 on\r\n", 18) == 0)
    {
        return CMD_LED21_ON;
    }
    else if (strncmp((const char *)buf, "slave 2 led 2 on\r\n", 18) == 0)
    {
        return CMD_LED22_ON;
    }
    else if (strncmp((const char *)buf, "slave 1 led 1 off\r\n", 19) == 0)
    {
        return CMD_LED11_OFF;
    }
    else if (strncmp((const char *)buf, "slave 1 led 2 off\r\n", 19) == 0)
    {
        return CMD_LED12_OFF;
    }
    else if (strncmp((const char *)buf, "slave 2 led 1 off\r\n", 19) == 0)
    {
        return CMD_LED21_OFF;
    }
    else if (strncmp((const char *)buf, "slave 2 led 2 off\r\n", 19) == 0)
    {
        return CMD_LED22_OFF;
    }
    else if (strncmp((const char *)buf, "slave 1 all leds on\r\n", 21) == 0)
    {
        return CMD_ALL_LED1_ON;
    }
    else if (strncmp((const char *)buf, "slave 2 all leds on\r\n", 21) == 0)
    {
        return CMD_ALL_LED2_ON;
    }
    else if (strncmp((const char *)buf, "slave 1 all leds off\r\n", 22) == 0)
    {
        return CMD_ALL_LED1_OFF;
    }
    else if (strncmp((const char *)buf, "slave 2 all leds off\r\n", 22) == 0)
    {
        return CMD_ALL_LED2_OFF;
    }
    else if (strncmp((const char *)buf, "slave 1 state\r\n", 15) == 0)
    {
        return CMD_slave1_state;
    }
    else if (strncmp((const char *)buf, "slave 2 state\r\n", 15) == 0)
    {
        return CMD_slave2_state;
    }
    
    else
    {
        return CMD_INVALID;
    }
}

void execute_command(SERIAL_COMMAND_t cmd)
{
    uint8_t data[1];
    uint8_t datarecieved[1];
    uint8_t datasend[1];
    datarecieved[0] = 0;
    datasend[0] = 30;
    stats.received++;
    switch (cmd)
    {
    //Hier werden die Befehle an den Slave geschickt
    case CMD_LED11_ON:
        UART_LOG("\033[32m DEBUG: Slave 1 LED 1 turned ON.\033[0m \n");
        data[0] = 11;
        i2c_master_transmit(I2C1, add_slave1, data, 1);
        break;
    case CMD_LED12_ON:
        UART_LOG("\033[32m DEBUG: Slave 1 LED 2 turned ON. \033[0m \n");
        data[0] = 21;
        i2c_master_transmit(I2C1, add_slave1, data, 1);
        break;
    case CMD_LED21_ON:
        UART_LOG("\033[32m DEBUG: Slave 2 LED 1 turned ON. \033[0m \n");
        data[0] = 11;
        i2c_master_transmit(I2C1, add_slave2, data, 1);
        break;
    case CMD_LED22_ON:
        UART_LOG("\033[32m DEBUG: Slave 2 LED 2 turned ON. \033[0m \n");
        data[0] = 21;
        i2c_master_transmit(I2C1, add_slave2, data, 1);
        break;
    case CMD_LED11_OFF:
        UART_LOG("\033[32m DEBUG: Slave 1 LED 1 turned OFF.\033[0m \n");
        data[0] = 10;
        i2c_master_transmit(I2C1, add_slave1, data, 1);
        break;
    case CMD_LED12_OFF:
        UART_LOG("\033[32m DEBUG: Slave 1 LED 2 turned OFF. \033[0m \n");
        data[0] = 20;
        i2c_master_transmit(I2C1, add_slave1, data, 1);
        break;
    case CMD_LED21_OFF:
        UART_LOG("\033[32m DEBUG: Slave 2 LED 1 turned OFF. \033[0m \n");
        data[0] = 10;
        i2c_master_transmit(I2C1, add_slave2, data, 1);
        break;
    case CMD_LED22_OFF:
        UART_LOG("\033[32m DEBUG: Slave 2 LED 2 turned OFF. \033[0m \n");
        data[0] = 20;
        i2c_master_transmit(I2C1, add_slave2, data, 1);
        break;
    case CMD_ALL_LED1_ON:
        UART_LOG("\033[32m DEBUG: Both LED on Slave 1 turned ON. \033[0m \n");
        data[0] = 40;
        i2c_master_transmit(I2C1, add_slave1, data, 1);
        break;
    case CMD_ALL_LED2_ON:
        UART_LOG("\033[32m DEBUG: Both LED on Slave 2 turned ON. \033[0m \n");
        data[0] = 40;
        i2c_master_transmit(I2C1, add_slave2, data, 1);
        break;
    case CMD_ALL_LED1_OFF:
        UART_LOG("\033[32m DEBUG: Both LED on Slave 1 turned OFF. \033[0m \n");
        data[0] = 50;
        i2c_master_transmit(I2C1, add_slave1, data, 1);
        break;
    case CMD_ALL_LED2_OFF:
        UART_LOG("\033[32m DEBUG: Both LED on Slave 2 turned OFF. \033[0m \n");
        data[0] = 50;
        i2c_master_transmit(I2C1, add_slave2, data, 1);
        break;

    //Statusabfragen der LEDs beider Slaves
    case CMD_slave1_state:
        //Zuerst schickt der Master eine Request Nachricht an den Slave, danach können die Daten vom Slave empfangen werden
        i2c_master_transmit(I2C1, add_slave1, datasend, 1);
        i2c_master_receive(I2C1, add_slave1, datarecieved, 1);

        if (datarecieved[0] == 0){
            UART_LOG("\033[35m Slave 1 LED 1 and LED 2 off \033[0m \n");
        }
        else if (datarecieved[0] == 1){
            UART_LOG("\033[35m Slave 1 LED 1 off and LED 2 on \033[0m \n");
        }
        else if (datarecieved[0] == 2){
            UART_LOG("\033[35m Slave 1 LED 1 on and LED 2 off \033[0m \n");
        }
        else if (datarecieved[0] == 3){
            UART_LOG("\033[35m Slave 1 LED 1 on and LED 2 on \033[0m \n");
        }
        else{
            UART_LOG("\033[35m Keine Daten zurück bekommen \033[0m \n");
        }
        break;

    case CMD_slave2_state:

        //Zuerst schickt der Master eine Request Nachricht an den Slave, danach können die Daten vom Slave empfangen werden
        i2c_master_transmit(I2C1, add_slave2, datasend, 1);
        i2c_master_receive(I2C1, add_slave2, datarecieved, 1);

        if (datarecieved[0] == 0){
            UART_LOG("\033[35m Slave 2 LED 1 and LED 2 off \033[0m \n");
        }
        else if (datarecieved[0] == 1){
            UART_LOG("\033[35m Slave 2 LED 1 off and LED 2 on \033[0m \n");
        }
        else if (datarecieved[0] == 2){
            UART_LOG("\033[35m Slave 2 LED 1 on and LED 2 off \033[0m \n");
        }
        else if (datarecieved[0] == 3){
            UART_LOG("\033[35m Slave 2 LED 1 on and LED 2 on \033[0m \n");
        }
        else{
            UART_LOG("\033[35m Keine Daten zurück bekommen \033[0m \n");
        }
        break;
    
    //Wenn kein Fall eingetreten ist, ist der Befehl ungültig
    default:
        stats.invalid++;
        UART_LOG("\033[91;1m DEBUG: Invalid command\033[0m \n");
        break;
    }
}

int main()
{
    // Setup UART2 with a baudrate of 115200
    uart_setup(USART2, 115200);
    i2c_master_setup(I2C1, I2C_STANDARD);
    systick_init();


    while (1)
    {

        if (rx_buffer_overflow)
        {
            // handle buffer overflow in uart
            clear_rx_buffer(rx_buffer, RX_BUFFER_SIZE, rx_index);
            rx_buffer_overflow = false; // just clearing the buffer might have impact on next incomming parts.
            UART_LOG("\033[91;1m ERROR: rx_buffer overflow detected.\033[33m \n");
        }

        if (rx_newline)
        {
            // process command
            UART_LOG("\033[0m")
            SERIAL_COMMAND_t cmd = process_input(rx_buffer);
            clear_rx_buffer(rx_buffer, RX_BUFFER_SIZE, rx_index);
            rx_newline = false;
            execute_command(cmd);
        }
    }
}

void USART2_IRQHandler(void)
{
    if (uart_data_available(USART2))
    {
        uint8_t byte = uart_read_byte(USART2);
        uart_write_byte(USART2, byte);

        // check for buffer overflow
        if (rx_index >= RX_BUFFER_SIZE)
        {
            rx_buffer_overflow = true;
            rx_index = 0;
        }
        else
        {
            rx_buffer[rx_index++] = byte;
        }

        if ('\n' == byte)
        {
            rx_newline = true;
        }
    }
}