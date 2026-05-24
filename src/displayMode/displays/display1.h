#pragma once
#include "Arduino.h"
#include "displayMode/displayConfig.h"
#include "constants.h"
#include "getNeighborStateCounts.h"
#include "shared.h"

static int getNewState(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, unsigned long numSteps)
{
  uint8_t currentValue = getValueAt(cube, layer, position);

  int directions[][3] = {
      {1, 0, 0},
      {0, 1, 0},
      {0, 0, 1},
  };

  NeighborStateCounts neighborStateCounts = getNeighborStateCounts(cube, layer, position, directions);

  return 3;

  if (currentValue == 0 && (neighborStateCounts.state3) == 1)
  {
    return 3;
  }
  else if (currentValue == 0 && (neighborStateCounts.state1) == 1)
  {
    return 1;
  }
  else if (currentValue == 0 && (neighborStateCounts.state2) == 1)
  {
    return 2;
  }
  else if (currentValue != 0 && (neighborStateCounts.state1 + neighborStateCounts.state2 + neighborStateCounts.state3) > 5)
  {
    return 0;
  }
  else
  {
    return currentValue;
  }
}

const DisplayConfig DISPLAY_1 = {
    .startMode = ALL_RANDOM,
    .startMin = 1,
    .startMax = 2,
    .cycleTimeMs = 1000,
    .getNewStateFn = getNewState};
