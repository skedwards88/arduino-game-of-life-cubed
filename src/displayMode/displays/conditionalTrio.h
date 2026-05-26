#pragma once
#include "Arduino.h"
#include "displayMode/displayConfig.h"
#include "constants.h"
#include "getNeighborStateCounts.h"
#include "shared.h"

// Note: sometimes freezes after many cycles

static int conditionalTrio(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, unsigned long numSteps)
{

  int directions[][3] = {
      {1, 0, 0},
      {0, 1, 0},
      {0, 0, 1},
  };

  uint8_t currentValue = getValueAt(cube, layer, position);

  NeighborStateCounts neighborStateCounts = getNeighborStateCounts(cube, layer, position, directions);

  if (currentValue == 1 && (neighborStateCounts.state2) == 1)
  {
    return 2;
  }
  if (currentValue == 2 && (neighborStateCounts.state3) == 1)
  {
    return 3;
  }
  if (currentValue == 3 && (neighborStateCounts.state1) == 1)
  {
    return 1;
  }
  else
  {
    return currentValue;
  }
}

const DisplayConfig CONDITIONAL_TRIO = {
    .startMode = ALL_RANDOM,
    .startMin = 1,
    .startMax = 3,
    .cycleTimeMs = 250,
    .getNewStateFn = conditionalTrio};
