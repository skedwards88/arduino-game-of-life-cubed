#pragma once
#include "Arduino.h"
#include "../constants.h"

struct DisplayState
{
  // Cube representation
  // The cube is made of bicolor LEDs
  // 0 = off, 1 = Color 1, 2 = Color 2, 3 = Color 1+2
  uint8_t cube[NUM_LAYERS][NUM_POSITIONS];

  // Cache the bytes to display for each layer
  // since the bytes are rendered much more frequently than they change
  uint8_t cachedLayerBytes[NUM_LAYERS][NUM_SHIFT_REGISTERS];

  unsigned long numSteps;

  int currentDisplay;
};

extern DisplayState displayState;
