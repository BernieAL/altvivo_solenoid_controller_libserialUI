#include <libserialport.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SerialComm.hpp"

static struct sp_port *port = NULL;
// timeout for send/recieve
static int timeout = 1000;

// INIT SERIAL
int init_serial(const char *port_name, unsigned int flags)
{

  enum sp_return result;

  // get port by name
  // takes port by name "/dev/ttyUSB0"
  // fills port var with a port structure
  result = sp_get_port_by_name(port_name, &port);

  // check if we successfully found port
  if (result != SP_OK)
  {
    fprintf(stderr, "Error finding port: %s\n", sp_last_error_message());
    return -1;
  }

  // open port for R/W
  result = sp_open(port, SP_MODE_READ_WRITE);

  // check for error opening port
  if (result != SP_OK)
  {
    fprintf(stderr, "Error opening port: %s\n", sp_last_error_message());
    // clean up mem if open failed
    sp_free_port(port);
    return -1;
  }

  printf("Port opened successfully: %s\n", port_name);
  return 0; // Success
}

// CONFIG SERIAL
// makes copy of struct from SerialComm.hpp
// copy made with pass by pointer
// returns 0 if success, -1 otherwise
int config_serial(struct serial_config *s)
{

  if (!port)
    return -1; // check if port is open

  int baud_value = 0;
  switch (s->baud)
  {
  case B9600:
    baud_value = 9600;
    break;
  case B19200:
    baud_value = 19200;
    break;
  case B38400:
    baud_value = 38400;
    break;
  case B57600:
    baud_value = 57600;
    break;
  case B115200:
    baud_value = 115200;
    break;
  default:
    baud_value = 115200;
  }

  sp_set_baudrate(port, baud_value);

  // set data bits
  int bits = 8;
  switch (s->data_size)
  {
  case DATA_5B:
    bits = 5;
    break;
  case DATA_6B:
    bits = 6;
    break;
  case DATA_7B:
    bits = 7;
    break;
  case DATA_8B:
    bits = 8;
    break;
  }
  sp_set_bits(port, bits);

  // set parity
  sp_set_parity(port, s->parity ? SP_PARITY_EVEN : SP_PARITY_NONE);

  // set stop bits
  sp_set_stopbits(port, s->one_stop_bit ? 1 : 2);

  // set HW flow control
  sp_set_flowcontrol(port, s->hw_flow ? SP_FLOWCONTROL_RTSCTS : SP_FLOWCONTROL_NONE);

  printf("Serial port configured successfully\n");
  return 0;
}

// get list of serial ports on the system
// returns ports as struct of type serial_ports_list
struct serial_ports_list *get_ports()
{

  // port_list will hold found ports by sp_list_ports
  struct sp_port **port_list;

  // sp_list_ports() gets us the ports, we store in port_list struct
  enum sp_return result = sp_list_ports(&port_list);

  if (result != SP_OK)
  {
    printf("sp_list_ports() failed!\n");
    return NULL;
  }

  // allocate return struct
  //must cast because malloc returns type void*
  struct serial_ports_list *ret = (struct serial_ports_list *)malloc(sizeof(struct serial_ports_list));
  
  //create list of chars, allocate space for 1 ptr
  ret->list = (char **)malloc(sizeof(char *));
  ret->length = 0;

  // iterate through ports - do usb/acm filtering
  // checking 4th char of each port in dev/tty, if starts with A or U
  for (int i = 0; port_list[i] != NULL; i++)
  {
    const char *port_name = sp_get_port_name(port_list[i]);

    if (strstr(port_name, "ttyUSB") || strstr(port_name, "ttyACM"))
    {

      // allocate and copy port name
      ret->list[ret->length] = (char *)malloc(strlen(port_name) + 1);
      strcpy(ret->list[ret->length], port_name);

      ret->length++;
      ret->list = (char **)realloc(ret->list, (ret->length + 1) * sizeof(char *));

      printf("Found USB serial port: %s\n", port_name);
    }
  }

  // Free the port list after copying the names we needed
  sp_free_port_list(port_list);

  return ret;
}

// write bytes with ACK from mcu and 1ms retry
ssize_t write_bytes(unsigned char *bytes, unsigned int length)
{

  if (!port)
  {
    printf("Error: Port not open\n");
    return -1;
  }

  const int MAX_RETRIES = 3;
  const unsigned int WRITE_TIMEOUT = 1000; // 1 second

  //-------------------------!!!!!ASK MARK ABOUT ACK BYTE
  const unsigned char ACK_BYTE = 0xAA;     // Expected ACK from MCU

  for (int attempt = 0; attempt < MAX_RETRIES; attempt++)
  {

    printf("Attempt %d: Sending %d bytes: ", attempt + 1, length);
    // loop through each byte in
    for (int i = 0; i < length; i++)
    {
      printf("0x%02X ", bytes[i]);
    }
    printf("\n");

    // perform byte write
    int result = sp_blocking_write(port, bytes, length, timeout);

    // Check if all bytes were sent
    if (result != length)
    {
      printf("Warning: Only sent %d/%d bytes\n", result, length);
      continue; // Retry
    }

    printf("Successfully sent %d bytes\n", length);

    // Wait for ACK byte from MCU

    //------------!!!!!ASK MARK ABOUT ACK ----------
    unsigned char ack_byte = 0;
    ssize_t ack_result = read_byte(&ack_byte);

    if (ack_result == 1 && ack_byte == ACK_BYTE)
    {
      printf("✓ ACK received - command confirmed\n");
      return length; // Success!
    }
    else if (ack_result == 1)
    {
      printf("✗ Unexpected response: 0x%02X (expected 0x%02X)\n",
             ack_byte, ACK_BYTE);
    }
    else if (ack_result == 0)
    {
      printf("✗ Timeout waiting for ACK\n");
    }
    else
    {
      printf("✗ Error reading ACK\n");
    }

    // If we get here, something went wrong - retry
    if (attempt < MAX_RETRIES - 1)
    {
      printf("Retrying...\n");
      sp_drain(port); // Flush any garbage data
    }
  }

  // All retries failed
  printf("Failed to send command after %d attempts\n", MAX_RETRIES);
  return -1;
}

// read bytes
ssize_t read_byte(unsigned char *byte)
{
  if (!port)
    return -1;

  unsigned int timeout = 1000;

  // perform byte read
  int result = sp_blocking_read(port, byte, 1, timeout);

  if (result == 1)
  {
    printf("Received ACK byte: 0x%02X\n", *byte);
    return 1;
  }
  else if (result == 0)
  {
    printf("Timeout - no ACK received\n");
    return 0;
  }
  else
  {
    printf("Error reading: %s\n", sp_last_error_message());
    return -1;
  }
}

// check if serial is open
bool serial_is_open()
{
  return port != NULL;
}

// close port
void close_port()
{
  if (port)
  {
    sp_close(port);
    sp_free_port(port);
    port = NULL;
    printf("Serial port closed \n");
  }
}

// HELPER FUNCTION FOR ERROR HANDLING
// taken from - https://sigrok.org/api/libserialport/unstable/a00006.html
/* Helper function for error handling. */
int check(enum sp_return result)
{
  /* For this example we'll just exit on any error by calling abort(). */
  char *error_message;
  switch (result)
  {
  case SP_ERR_ARG:
    printf("Error: Invalid argument.\n");
    abort();
  case SP_ERR_FAIL:
    error_message = sp_last_error_message();
    printf("Error: Failed: %s\n", error_message);
    sp_free_error_message(error_message);
    abort();
  case SP_ERR_SUPP:
    printf("Error: Not supported.\n");
    abort();
  case SP_ERR_MEM:
    printf("Error: Couldn't allocate memory.\n");
    abort();
  case SP_OK:
  default:
    return result;
  }
}