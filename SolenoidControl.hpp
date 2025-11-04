#include "SolenoidState.hpp"
#include "SerialComm.hpp"








class SolenoidController{

private:
        Solenoid state;


    //protocol constants
    static constexpr unsigned char HEADER[] = {0xFE, 0xED};
    static constexpr unsigned char CMD_SOLENOID = 0x01;
    static constexpr unsigned char CMD_PRESSURE = 0x02;


    //helper - handles low level details
    //of formatting and sending bytes to hardware
    //made private to force use of public self-explanatory methods
    //public methods will make use of this method
    //will make use of write_bytes (serial.hpp)
    void sendCommand()


public:
    SolenoidController();

    //Solenoid operations
    void toggleSolenoid(size_t index);
    void activateSolenoid(size_t index);
    void deactivateSolenoid(size_t index);
    void programSolenoid(size_t index, int openTime, int closeTime);
    void clearSolenoid(size_t index);

    //Pressure operations
    void setPositivePressure();
    void setVacuum(int percentage);

    //state access (read-only)
    const SolenoidState& getState() const { return state; }
}