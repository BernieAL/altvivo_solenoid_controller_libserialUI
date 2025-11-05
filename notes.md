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