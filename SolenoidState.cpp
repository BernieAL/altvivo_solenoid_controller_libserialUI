#include "SolenoidState.hpp"



//init solenoid state 
//default vals, all off.
SolenoidState::SolenoidState() {
    for (size_t i = 0; i < NUM_SOLENOIDS; i++) {
        solenoids[i].active = false;
        solenoids[i].programmed = false;
        solenoids[i].timing = {0, 0};
    }
}


const Solenoid& SolenoidState::get(size_t index) const {
    return solenoids[index];
}


bool SolenoidState::isActive(size_t index) const {
    if(index >= NUM_SOLENOIDS) return false;
    return solenoids[index].active;
}

bool SolenoidState::isProgrammed(size_t index) const {
    if (index >= NUM_SOLENOIDS) return false;
    return solenoids[index].programmed;
}

SolenoidTiming SolenoidState::getTiming(size_t index) const {
    if (index >= NUM_SOLENOIDS) return {0, 0};
    return solenoids[index].timing;
}

void SolenoidState::setActive(size_t index, bool active) {
    if (index >= NUM_SOLENOIDS) return;
    solenoids[index].active = active;
}

void SolenoidState::setTiming(size_t index, const SolenoidTiming& timing) {
    if (index >= NUM_SOLENOIDS) return;
    solenoids[index].timing = timing;
    solenoids[index].programmed = true;
}

void SolenoidState::clearTiming(size_t index) {
    if (index >= NUM_SOLENOIDS) return;
    solenoids[index].timing = {0, 0};
    solenoids[index].programmed = false;
}