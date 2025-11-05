#include <libserialport.h>
#include <stdio.h>
#include <stdlib.h>
#include <SerialComm.hpp>



static struct sp_port *port = NULL;

//INIT SERIAL
int init_serial(const char* port_name, unsigned int flags){

    enum sp_return result;

    //get port by name
    //takes port by name "/dev/ttyUSB0"
    //fills port var with a port structure
    result = sp_get_port_by_name(port_name, &port);
    
    //check if we successfully found port
    if (result != SP_OK){
        fprintf(stderr, "Error finding port: %s\n", sp_last_error_message());
        return -1;
    }

    //open port for R/W
    result = sp_open(port, SP_MODE_READ_WRITE);

    //check for error opening port
    if (result != SP_OK) {
        fprintf(stderr, "Error opening port: %s\n", sp_last_error_message());
        //clean up mem if open failed
        sp_free_port(port);
        return -1;
    }

    printf("Port opened successfully: %s\n", port_name);
    return 0; // Success

}


//CONFIG SERIAL
//makes copy of struct from SerialComm.hpp
//copy made with pass by pointer
int config_serial(struct serial_config* s){

    if (!port) return -1 // check if port is open


    int baud_value = 0;
    switch(s->baud) {
        case B9600: baud_value = 9600; break;
        case B19200: baud_value = 19200; break;
        case B38400: baud_value = 38400; break;
        case B57600: baud_value = 57600; break;
        case B115200: baud_value = 115200; break;
        default: baud_value = 115200;
    }
}

//get list of serial ports on the system
int main(int argc, char **argv){


    //port_list will hold found ports by sp_list_ports
    struct sp_port **port_list;

    //sp_list_ports() gets us the ports, we store in port_list struct
    enum sp_return result = sp_list_ports(&port_list);

    //check if 
    if (result != SP_OK) {
        printf("sp_list_ports() failed!\n");
        return -1;
    }

    //iterate through ports - do usb filtering 

    
}




//HELPER FUNCTION FOR ERROR HANDLING 
//taken from - https://sigrok.org/api/libserialport/unstable/a00006.html

/* Helper function for error handling. */
int check(enum sp_return result)
{
        /* For this example we'll just exit on any error by calling abort(). */
        char *error_message;
        switch (result) {
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