#ifndef __GLOBAL_CONSTANTS__
#define __GLOBAL_CONSTANTS__

#define NUM_OF_ROWS 4
#define NUM_OF_COLS 4

#define HORIZONTAL_MARGIN 4
#define VERTICAL_MARGIN 4

#define SOL_ID_BASE 0xA

// Colors
#define BACKGROUND_BLACK wxColour(0x0f,0x0f,0x0f)
#define FOREGROUND_WHITE wxColour(0xf0,0xf0,0xf0)
#define BUTTON_RED wxColour(0xC8, 0x00, 0x00)
#define BUTTON_GREEN wxColour(0x00, 0xC8, 0x00)


struct SolenoidTiming
{
    int openTime;   // in milliseconds
    int closeTime;  // in milliseconds
};

struct Solenoid
{
  struct SolenoidTiming timing;
  bool active;
  bool programed;
};

enum sol_ids
  {
    ID_SOL0 = SOL_ID_BASE,
    ID_SOL1,
    ID_SOL2,
    ID_SOL3,
    ID_SOL4,
    ID_SOL5,
    ID_SOL6,
    ID_SOL7,
    ID_SOL8,
    ID_SOL9,
    ID_SOL10,
    ID_SOL11,
    ID_SOL12,
    ID_SOL13,
    ID_SOL14,
    ID_SOL15
  };

enum sol_idexes
  {
    IDX_SOL0 = 0,
    IDX_SOL1,
    IDX_SOL2,
    IDX_SOL3,
    IDX_SOL4,
    IDX_SOL5,
    IDX_SOL6,
    IDX_SOL7,
    IDX_SOL8,
    IDX_SOL9,
    IDX_SOL10,
    IDX_SOL11,
    IDX_SOL12,
    IDX_SOL13,
    IDX_SOL14,
    IDX_SOL15,
    NUM_OF_SOLENOIDS
  };



#endif
