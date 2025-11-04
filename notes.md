## Quick Mental Model
```
.hpp = Restaurant Menu
  "We offer: Burger, Fries, Shake"
  
.cpp = Kitchen
  "Here's how we actually make the burger..."
  
main.cpp = Customer
  "I'll have a burger please" (includes the menu, calls the kitchen)


--------------------------------------------------------------------------




SerialComm        → Low-level serial communication (hardware layer)
      ↓
SolenoidController → Business logic (what operations can we do?)
      ↓
SolenoidState     → Data model (what's the current state?)
      ↓
UI Layer          →  UI framework


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

    SERIALCOMM.CPP

        get list of serial ports (usb filtered)
        write bytes to serial port
        read bytes from serial port
        check if serial port is open

    -------------------------

    SOLENOIDCONTROL.CPP

        Makes use of SERIALCOMM functions
        by wrapping in event handlers
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

