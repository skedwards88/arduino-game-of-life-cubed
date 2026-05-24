#pragma once

const int GRID_DIMENSION = 4;
const int NUM_LAYERS = GRID_DIMENSION;
const int NUM_POSITIONS = 16;      // Positions (bicolor LEDs) per layer
const int NUM_SHIFT_REGISTERS = 4; // 16 bicolor LEDs = 32 leads; 8 leads controlled by 1 shift register

// Pins for the first shift register
const int CLOCK_PIN = 6;
const int LATCH_PIN = 7;
const int DATA_PIN = 8;

// Pins for the layers
const int LAYER0_PIN = 2;
const int LAYER1_PIN = 3;
const int LAYER2_PIN = 4;
const int LAYER3_PIN = 5;
const int layerPins[NUM_LAYERS] = {LAYER0_PIN, LAYER1_PIN, LAYER2_PIN, LAYER3_PIN};

// Duration that each layer is turned on
// Note this is microseconds, not milliseconds
const unsigned int LAYER_ON_TIME_US = 800;

// Duration that each display is shown for (when in display mode)
const unsigned long DISPlAY_TIME_MS = 6000;

// Minimum elapsed time between recording input
const unsigned long DEBOUNCE_MS = 50;

// Pins for the joystick
const int UP_PIN = A2;
const int DOWN_PIN = A1;
const int LEFT_PIN = 12;
const int RIGHT_PIN = A3;
const int SELECT_PIN = 11;
const int SET_PIN = 10;
const int RESET_PIN = 9;

// These are the possible directions from a position
// (consider a line with between each of these coordinates and
// the coordinate {0,0,0} representing the position).
const int DIRECTIONS[13][3] = {
    // Vary just x, y, or z
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
    // Vary xy, xz, or yz
    {1, 1, 0},
    {1, 0, 1},
    {0, 1, 1},
    {1, -1, 0},
    {1, 0, -1},
    {0, 1, -1},
    // Vary xyz
    {1, 1, 1},
    {1, -1, 1},
    {1, 1, -1},
    {-1, 1, 1}};
