#include "SolenoidControl.hpp"


SolenoidController::SolenoidController(){
    //init state already done in SolenoidState constructor

}


//helper for low level details
void SolenoidController::sendCommand(unsigned char cmd, unsigned char device,
                                     unsigned char value_high, unsigned char value_low) {
    unsigned char data[] = {HEADER_BYTE1, HEADER_BYTE2, cmd, device, value_high, value_low};
    write_bytes(data, sizeof(data));
}



// ----------------------------------------------------------------------------
// Solenoid operations
// ----------------------------------------------------------------------------




void SolenoidController::toggleSolenoid(size_t index){

    if (index >= state.solenoid_count()) return;

    bool newState = !=state.isActive(index);

    if (newState){
        activateSolenoid(index);
    } else {
        deactivateSolenoid(index);
    }
}



void SolenoidController::activateSolenoid(size_t index) {
    if (index >= state.solenoid_count()) return;
    
    state.setActive(index, true);
    sendCommand(CMD_SOLENOID, static_cast<unsigned char>(index), 0x01, 0x00);
}

void SolenoidController::programSolenoid(size_t index, int openTime, int closeTime) {
    if (index >= state.solenoid_count()) return;
    
    // Check if clearing (both zero)
    if (openTime == 0 && closeTime == 0) {
        clearSolenoid(index);
        return;
    }
    
    SolenoidTiming timing = {openTime, closeTime};
    state.setTiming(index, timing);
}

void SolenoidController::clearSolenoid(size_t index) {
    if (index >= state.solenoid_count()) return;
    
    state.clearTiming(index);
}

// ----------------------------------------------------------------------------
// Pressure operations
// ----------------------------------------------------------------------------

void SolenoidController::setPositivePressure(int percentage) {
    if (percentage < 0 || percentage > 100) return;
    
    sendCommand(CMD_PRESSURE, 0x01, static_cast<unsigned char>(percentage), 0x00);
}

void SolenoidController::setVacuum(int percentage) {
    if (percentage < 0 || percentage > 100) return;
    
    sendCommand(CMD_PRESSURE, 0x02, static_cast<unsigned char>(percentage), 0x00);
}