# include "SolenoidController.hpp"


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

    if (index >= state.count()) return;

    bool newState = !=state.isActive(index);

    if (newState){
        activateSolenoid(index);
    } else {
        deactivateSolenoid(index);
    }
}



void SolenoidController::activateSolenoid(size_t index) {
    if (index >= state.count()) return;
    
    state.setActive(index, true);
    sendCommand(CMD_SOLENOID, static_cast<unsigned char>(index), 0x01, 0x00);
}



