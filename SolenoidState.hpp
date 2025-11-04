/////////////////////////////////////////////////////////////////////////////
// Name:        SolenoidState.hpp
// Purpose:     Data model for solenoid state management
// Author:      Bernardino Almanzar
// Modified by:
// Created:     2025-11-04
// Copyright:   (c) AltVivo
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// overview
// ============================================================================
//
// This file defines the data model that stores the state of all 16 solenoids
// in the system. It acts as the "memory" or "database" of the application.
//
// Each solenoid has three properties:
//   - active:     Whether the solenoid is currently energized (ON/OFF)
//   - programmed: Whether the user has set timing values
//   - timing:     Open and close time values in milliseconds
//
// The SolenoidState class provides controlled access to read and modify
// these properties, ensuring data consistency across the application.
//
// This is a pure data model with no dependencies on UI or hardware.
// It is used by SolenoidController to track state changes.
//
// ============================================================================

// ============================================================================
// declarations
// ============================================================================

#pragma once
#include <cstddef>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

constexpr size_t NUM_SOLENOIDS = 16;

// ----------------------------------------------------------------------------
// data structures
// ----------------------------------------------------------------------------

/**
 * Timing parameters for solenoid open/close operations
 */
struct SolenoidTiming {
    int openTime;   // Time to keep solenoid open (milliseconds)
    int closeTime;  // Time to keep solenoid closed (milliseconds)
};

/**
 * Complete state information for a single solenoid
 */
struct Solenoid {
    bool active;           // True if solenoid is currently energized
    bool programmed;       // True if timing parameters have been set
    SolenoidTiming timing; // Open and close timing parameters
};

// ----------------------------------------------------------------------------
// SolenoidState class
// ----------------------------------------------------------------------------

/**
 * Manages the state of all solenoids in the system.
 * Serves as the single source of truth for solenoid states.
 */
class SolenoidState {
private:
    Solenoid solenoids[NUM_SOLENOIDS];

public:
    // Constructor
    SolenoidState();
    
    // Getters
    const Solenoid& get(size_t index) const;
    bool isActive(size_t index) const;
    bool isProgrammed(size_t index) const;
    SolenoidTiming getTiming(size_t index) const;
    
    // Setters
    void setActive(size_t index, bool active);
    void setTiming(size_t index, const SolenoidTiming& timing);
    void clearTiming(size_t index);
    
    // Utility
    size_t count() const { return NUM_SOLENOIDS; }
};