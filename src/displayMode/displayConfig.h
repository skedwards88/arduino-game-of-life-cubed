#pragma once
#include "Arduino.h"
#include "constants.h"

enum StartMode
{
  ALL_RANDOM,   // random seeding for all spots
  TOP_LEFT,     // top left is random color, all others off
  SINGLE_RANDOM // single spot is random color, all others off
};

struct DisplayConfig
{
  StartMode startMode;

  // Control the initial colors
  int startMin;
  int startMax;

  // Duration that each cube state is held for
  unsigned long cycleTimeMs;

  int (*getNewStateFn)(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, unsigned long numSteps);
};
