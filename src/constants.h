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

// Duration that each display is shown for
const unsigned long DISPlAY_TIME_MS = 6000;

// Duration that each layer is turned on
// Note this is microseconds, not milliseconds
const unsigned int LAYER_ON_TIME_US = 800;
