#ifndef __SERIAL_COMM_HPP__
#define __SERIAL_COMM_HPP__

#include <sys/types.h>
#include <stdbool.h>

// Data size enumeration
enum SerialDataSize
{
    DATA_5B = 0,
    DATA_6B,
    DATA_7B,
    DATA_8B
};

// Baud rate constants (match libserialport values)
enum BaudRate
{
    B9600 = 9600,
    B19200 = 19200,
    B38400 = 38400,
    B57600 = 57600,
    B115200 = 115200
};

// Serial configuration structure
struct serial_config
{
    int baud;
    bool parity;
    bool one_stop_bit;
    enum SerialDataSize data_size;
    bool hw_flow;
};

// Serial ports list structure
struct serial_ports_list
{
    char** list;
    int length;
};

// Function declarations - MUST MATCH the .cpp file exactly!
int init_serial(const char* port_name, unsigned int flags);
int config_serial(struct serial_config* s);
ssize_t write_bytes(unsigned char* bytes, unsigned int length);
ssize_t read_byte(unsigned char* byte);
struct serial_ports_list* get_ports();
bool serial_is_open();
void close_port();

#endif // __SERIAL_COMM_HPP__