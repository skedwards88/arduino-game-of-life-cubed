#pragma once
#include "Arduino.h"
#include "displayMode/displayConfig.h"
#include "constants.h"
#include "getNeighborStateCounts.h"
#include "shared.h"

// Note: sometimes freezes after many cycles

static int tendrils(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, unsigned long numSteps)
{

  int directions[][3] = {
      {1, 0, 0},
      {0, 1, 0},
      {0, 0, 1},
  };

  uint8_t currentValue = getValueAt(cube, layer, position);

  NeighborStateCounts neighborStateCounts = getNeighborStateCounts(cube, layer, position, directions);

  if (currentValue == 0 && (neighborStateCounts.state1) == 1)
  {
    return random(0, 2);
  }
  else if (currentValue == 1)
  {
    return 3;
  }
  else if (currentValue == 3)
  {
    return 2;
  }
  else if (currentValue == 3 && (neighborStateCounts.state0) > 0)
  {
    return 0;
  }
  else if (currentValue == 2 && (neighborStateCounts.state2 + neighborStateCounts.state0) > 2 && (neighborStateCounts.state1 + neighborStateCounts.state3) == 0)
  {
    return 0;
  }
  else if (currentValue == 0 && (neighborStateCounts.state0) > 5 && numSteps > 12)
  {
    return random(0, 2);
  }
  else
  {
    return currentValue;
  }
}

const DisplayConfig TENDRILS = {
    .startMode = SINGLE_RANDOM,
    .startMin = 1,
    .startMax = 1,
    .cycleTimeMs = 1200,
    .getNewStateFn = tendrils};
