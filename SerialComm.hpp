#ifndef __SERIAL_COMM_HPP__
#define __SERIAL_COMM_HPP__

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>

enum SerialDataSize
{
    DATA_5B = 0,
    DATA_6B,
    DATA_7B,
    DATA_8B
};

struct serial_config
{
    speed_t baud;
    bool parity;
    bool one_stop_bit;
    enum SerialDataSize data_size;
    bool hw_flow;
};

struct serial_ports_list
{
    char** list;
    int length;
    DIR* dir;
};

// Function declarations
int initialize_serial(const char* port, tcflag_t flags);
int configure_serial(struct serial_config* s); 
ssize_t write_bytes(unsigned char* bytes, unsigned int length);  
ssize_t read_byte(unsigned char* byte);
struct serial_ports_list* get_ports();
bool serial_is_open();
void close_port();

#endif