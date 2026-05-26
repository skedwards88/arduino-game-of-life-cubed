#pragma once
#include "Arduino.h"
#include "displayMode/displayConfig.h"
#include "constants.h"
#include "getNeighborStateCounts.h"
#include "shared.h"

static int succession(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, unsigned long numSteps)
{

  int directions[][3] = {
      {1, 0, 0},
      {0, 1, 0},
      {0, 0, 1},
  };

  uint8_t currentValue = getValueAt(cube, layer, position);

  NeighborStateCounts neighborStateCounts = getNeighborStateCounts(cube, layer, position, directions);

  if (currentValue == 2 && (neighborStateCounts.state1) > 0)
  {
    return 1;
  }
  if (currentValue == 1 && (neighborStateCounts.state1) > 3)
  {
    return 0;
  }
  if (currentValue == 1 && (neighborStateCounts.state0) > 3)
  {
    return 0;
  }
  if (currentValue == 0 && (neighborStateCounts.state2) > 0)
  {
    return 2;
  }
  if (currentValue == 0 && (neighborStateCounts.state0) > 4)
  {
    return 2;
  }
  if (currentValue == 2 && (neighborStateCounts.state2) > 5)
  {
    return random(1, 3);
  }
  else
  {
    return currentValue;
  }
}

const DisplayConfig SUCCESSION = {
    .startMode = ALL_RANDOM,
    .startMin = 1,
    .startMax = 2,
    .cycleTimeMs = 1000,
    .getNewStateFn = succession};
