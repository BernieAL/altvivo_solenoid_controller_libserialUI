
TO DO

    DONE - Seperated UI and Logic 

    DONE - converted serialcomm to user libserial

    Second command to clear toggle

    1ms timeout in case msg gets messedup

    comp byte of handshake sent back from MCU

    refresh ports when something gets plugged in

    ensure sliders work and are sending commands

    UI - wxwidgets
    



---------------------------------------
## Quick Mental Model hpp,cpp
```
.hpp = Restaurant Menu
  "We offer: Burger, Fries, Shake"
  
.cpp = Kitchen
  "Here's how we actually make the burger..."
  
main.cpp = Customer
  "I'll have a burger please" (includes the menu, calls the kitchen)


--------------------------------------------------------------------------


SolenoidControl = Operations we can perform (toggle, activate, program, etc.) ✅
SolenoidState = Data model tracking each solenoid's state (active, programmed, timing) ✅
SerialComm = Hardware-level communication (read/write bytes to serial ports) ✅


--------------------------------------------------------------

All SOLENOIDS ON SINGLE SERIAL PORT

    all 16 solenoids use ONE serial port
    we differentiate by using the device ID
    in the command protocol

    Ex.
        - **One serial port** (e.g., `/dev/ttyUSB0`)
        - **16 solenoids** identified by device_id (0-15)
        - Commands sent over that single port specify which solenoid to control


    main.cpp
    ↓
    Initialize serial port once (/dev/ttyUSB0)
    ↓
    SolenoidController
    ↓ (uses)
        ├→ SolenoidState (tracks state of all 16 solenoids in memory)
        └→ SerialComm (sends commands to hardware via ONE serial port)
        ↓
    Hardware receives commands and routes to correct solenoid based on device_id





---------------------------------------------------------------------------
CORE FUNCTIONALITY OF APP


    Serial Communication
        discover and list avail serial ports
        open/config serial port with specific settings
        send/recieve byte commands to HW
        Protocol: [0xFE, 0xED, command_type, device_id, value_high, value_low]
    
    Solenoid Control (grid of buttons - 4x4 Grid with states)
        inactive red - default state
        active green - solenoid energized
        programmed - has timing settings


        left click - toggle on/off (sends ox01/0x00 command)
        right click - open timing dialog to program open/close

    Pressure Control (Sliders)
        2 horizontal sliders (0-100%)
            positive pressure (device ID 0x01)
            vacuum (device ID 0x02)

        we send command to type 0x02 when value changes


    UI Layout

        Menu bar: Serial port selection, File (Exit), Help (About)
        Status bar: Shows connection status and slider values
        Main panel: Solenoid grid (expandable, min 600x500)
        Bottom panel: Pressure sliders with labels

-------------------------------------------------------


FILES AND THEIR FUNCTIONS

    SERIALCOMM.CPP - hw level communication through serial port

        get list of serial ports (usb filtered)
        write bytes to serial port
        read bytes from serial port
        check if serial port is open

    -------------------------

    SOLENOIDCONTROL.CPP

        Makes use of SERIALCOMM functions
        operations we need to perform on a solenoid:
            toggleSolenoid (if on, turn off vice versa)
            activateSolenoid
            deactivate
            programSolenoid
            clearSolenoid
  

    SOLENOIDSTATE

        for keeping track of state
        of each solenoid

        what data do we want to track
        for each solenoid
            is it active
            is it programmed
            get its timing


---------------------------------------


SerialComm FIle breakdown:


    fd - file descriptor
    is small integers that OS uses as a handle to refer to an open file or device
    in unix/linux, serial ports are treated as files - so when you open a serial port
    you get back a file descriptor that we use for all subsequent operations on that port


    initialize_serial() - "opens door to serial port"
        this opens the target serial port device file and return the file descriptor
        

    configure_serial() - "sets ruls of how to communicate through the door"
        uses returned fd from init_serial to configure how the serial comm works
        configures serial port params through termios structure (tty)


    write bytes/read bytes - uses file descriptor to send/recieve data

----------------------------------------------------

COMMAND STRUCTURE

Every command sent looks like this:

[0xFE, 0xED, command_type, device_id, value_high, value_low]
 └─────┬────┘ └────┬────┘ └────┬───┘ └─────────┬──────────┘
   Header      What to do   Which one      Additional data


0xFE vs 0xED - these are header bytes

we start every command with a unique pattern
to indicate that a real command is coming vs random garbage values

this unique pattern is not likely to occur in random noise

Ex for turning on solenoid #3
unsigned char data[] = {0xFE, 0xED, 0x01, 0x03, 0x01, 0x00};
                      //  ^^   ^^   ^^   ^^   ^^   ^^
                      //  |    |    |    |    |    └─ value_low (unused here)
                      //  |    |    |    |    └────── value_high (0x01 = ON)
                      //  |    |    |    └─────────── device_id (solenoid 3)
                      //  |    |    └──────────────── command_type (0x01 = solenoid control)
                      //  |    └───────────────────── Header byte 2
                      //  └────────────────────────── Header byte 1


The MCU on the other end checks for the header bytes
only if header bytes are rec'd that match FEED do we read the rest of the
bytes and execute the command


-------------------------------------------
-------------------------------------------

Finding Serial Ports

first sp_list_ports scans the system and finds all serial ports
it returns them in port_list (enum), this includes USB ports, BT, virtual ports

we then create our structure to hold fitlered results
this will hold all our usb ports that we find

ret is a pointer to a struct
we tell compiler to allocate enough space for a serial_ports_list struct
-malloc returns an address with pointer of type void*
-we cast the returned void* pointer to be type serial_ports_list
-we do this, so the compiler knows what values are available to the pointer
-if the pointer was just void*, then we couldnt acccess the fields that are available to serial_ports_list struct 
    (ex.  struct SolenoidTiming timing;
          bool active;
          bool programed;)

we then loop through all the ports returned by sp_list_ports
if it matches our criteria of being USB or ACM, then we copy 
that port name and store in our return structure

to copy the port name, we allocate mem on our return struct for it
we do this because the name length can vary.
we then grow the array for the next potential port


---


Allocate space for the struct - will hold all filtered port names

(inside struct)
Allocate space for dynamic array - starting with 1 slot
array will hold pointers to strings
    struct serial_ports_list* ret = (
        struct serial_ports_list*) malloc(sizeof(struct serial_ports_list)
    );

(in array)
Allocate space for the string itself (variable length)
    ret->list[i] = malloc(strlen(port_name) + 1);

Copy the string to the array
    strcpy(ret->list[i], port_name);

Grow the array for the next port
    ret->list = realloc(ret->list, (ret->length + 1) * sizeof(char*));



```

---

## Visual Confirmation
```
┌─────────────────────────────────────────┐
│ struct serial_ports_list (on heap)     │
│ ┌─────────────────────────────────────┐ │
│ │ list: → [dynamic array on heap]    │ │ ← Grows with realloc
│ │         ┌─────────────────────────┐ │ │
│ │         │ [0] → "/dev/ttyUSB0\0"  │ │ │ ← Allocated per string
│ │         │ [1] → "/dev/ttyACM0\0"  │ │ │ ← Allocated per string
│ │         │ [2] → "/dev/ttyUSB1\0"  │ │ │ ← Allocated per string
│ │         │ [3] (empty slot)        │ │ │ ← Room for next
│ │         └─────────────────────────┘ │ │
│ │                                     │ │
│ │ length: 3                           │ │
│ └─────────────────────────────────────┘ │
└─────────────────────────────────────────┘