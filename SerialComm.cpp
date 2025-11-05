/////////////////////////////////////////////////////////////////////////////
// Name:        SerialComm.cpp
// Purpose:     
// Author:      Bernardino Almanzar
// Modified by:
// Created:     2025-11-04
// Copyright:   (c) AltVivo
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// overview
// ============================================================================
//
// This file defines 
//
// ============================================================================

// ============================================================================
// declarations
// ============================================================================



#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SerialComm.hpp"

static int fd;
static struct termios tty;

int initialize_serial (const char* port, unsigned int flags)
{
  
  fd = open(port, flags);

  return fd;
}

/* See https://www.man7.org/linux/man-pages/man3/termios.3.html for more info */
int configure_serial (struct serial_config* s)
{

    // Get current serial port settings
  if (tcgetattr(fd, &tty) != 0)
    {
      perror("Error getting serial attributes");
      close(fd);
      return -1;
    }
  
  // Configure serial port
  cfsetospeed(&tty, s->baud);  // Set baud rate
  cfsetispeed(&tty, s->baud);
  
  if (s->parity)
    {
      tty.c_cflag |= PARENB;    // parity
    }
  else
    {
      tty.c_cflag &= ~PARENB;    // No parity
    }

  if(s->one_stop_bit)
    {
      tty.c_cflag &= ~CSTOPB;    // 1 stop bit
    }
  else
    {
      tty.c_cflag |= CSTOPB;    // 2 stop bit
    }

  tty.c_cflag &= ~CSIZE;     // Clear data size bits

  /* Set data size */
  switch (s->data_size)
    {
    case DATA_5B:
      tty.c_cflag |= CS5;
      break;
    case DATA_6B:
      tty.c_cflag |= CS6;
      break;
    case DATA_7B:
      tty.c_cflag |= CS7;
      break;
    case DATA_8B:
      tty.c_cflag |= CS8;
      break;
    }

  /* Set hardware flow control */
  if (s->hw_flow)
    {
      tty.c_cflag |= CRTSCTS;   // Hardware flow control
    }
  else
    {
      tty.c_cflag &= ~CRTSCTS;   // No hardware flow control
    }

  tty.c_cflag |= CREAD | CLOCAL; // Enable reading, ignore modem controls

  tty.c_lflag &= ~ICANON;    // Disable canonical mode
  tty.c_lflag &= ~ECHO;      // Disable echo
  tty.c_lflag &= ~ECHOE;     // Disable erasure
  tty.c_lflag &= ~ECHONL;    // Disable new-line echo
  tty.c_lflag &= ~ISIG;      // Disable interpretation of INTR, QUIT and SUSP

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off software flow ctrl
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

  tty.c_oflag &= ~OPOST;     // Prevent special interpretation of output bytes
  tty.c_oflag &= ~ONLCR;     // Prevent conversion of newline to carriage return/line feed

  // Set timeout for blocking read
  tty.c_cc[VTIME] = 10;      // Wait for up to 1s (10 deciseconds)
  tty.c_cc[VMIN] = 1;        // Block until 1 character is received

  return tcsetattr(fd, TCSANOW, &tty);
}

ssize_t write_bytes (unsigned char* bytes, unsigned int length)
{
  // Write the bytes
  ssize_t bytes_written = write(fd, bytes, length);
    
  if (bytes_written == length)
    {
      printf("Successfully wrote %d bytes: ", length);
      for (int i = 0; i < length; i++)
	{
	  printf("0x%02x ", bytes[i]);
	}
      printf("\n");
    }
  else
    {
      perror("Error writing to serial port");
    }

  return bytes_written;

}

ssize_t read_byte (unsigned char* byte)
{
  ssize_t bytes_read = read(fd, byte, 1);
  
  if (bytes_read > 0)
    {
      printf("Received byte: 0x%02X (%d)\n", *byte, *byte);
    }
  else if (bytes_read == 0)
    {
      printf("Timeout - no data received\n");
    }
  else
    {
      perror("Error reading from serial port");
    }

  return bytes_read;
}

struct serial_ports_list* get_ports()
{
  DIR* dir;
  struct dirent* entry;

  // open /sys/class/tty to see just serial devices
  dir = opendir("/sys/class/tty");


  
  if (dir == NULL)
    {
      perror("Could not open /sys/class/tty");
      struct serial_ports_list* error_ret
	= (struct serial_ports_list*)malloc(sizeof(struct serial_ports_list));

      error_ret->list = NULL;
      error_ret->length = -1;

      return error_ret;
    }

  
  struct serial_ports_list* ret = (struct serial_ports_list*)malloc(sizeof(struct serial_ports_list));
  ret->list = (char**)malloc(sizeof(char*));
  ret->length = 0;


  //read devices in tty
  //each entry is name of a serial port
  while((entry = readdir(dir)) != NULL)
    { 
      
      
      //skip non USB serial ports
      //if 4th char in name not 'A' or 'U' skip
      //also check serial port string is min length of 4
      
      char* name = entry->d_name;

      if (strlen(name) >= 4 && (name[3] == 'A' || name[3] == 'U'))
      {
          ret->list[ret->length] = name;
          ret->length++;
          ret->list = (char**)realloc(ret->list, (ret->length+1) * sizeof(char*));
      }
    }

  ret->dir = dir;

  return ret;
  
}

bool serial_is_open()
{
  return fd > 0;
}


void close_port()
{
  close(fd);
}
