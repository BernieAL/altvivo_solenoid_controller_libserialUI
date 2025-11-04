## Quick Mental Model
```
.hpp = Restaurant Menu
  "We offer: Burger, Fries, Shake"
  
.cpp = Kitchen
  "Here's how we actually make the burger..."
  
main.cpp = Customer
  "I'll have a burger please" (includes the menu, calls the kitchen)

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
  

    SOLENOIDSTATE

        for keeping track of state
        of each solenoid

        what data do we want to track
        for each solenoid
            is it active
            is it programmed
            get its timing