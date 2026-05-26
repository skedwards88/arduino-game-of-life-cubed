#pragma once
#include "Arduino.h"
#include "displayMode/displayConfig.h"
#include "constants.h"
#include "getNeighborStateCounts.h"
#include "shared.h"

// Note: sometimes freezes after many cycles

static int diagonalWaves(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, unsigned long numSteps)
{

  int directions[][3] = {
      {1, 0, 0},
      {0, 1, 0},
      {0, 0, 1},
  };

  uint8_t currentValue = getValueAt(cube, layer, position);

  NeighborStateCounts neighborStateCounts = getNeighborStateCounts(cube, layer, position, directions);

  if (currentValue == 0 && (neighborStateCounts.state1) > 0)
  {
    return 1;
  }
  else if (currentValue == 1 && (neighborStateCounts.state2) > 0)
  {
    return 2;
  }
  else if (currentValue == 2 && (neighborStateCounts.state3) > 0)
  {
    return 3;
  }
  else if (currentValue == 3 && (neighborStateCounts.state0) > 0)
  {
    return 0;
  }
  else if (currentValue == 1 && (neighborStateCounts.state1) == 3)
  {
    return 2;
  }
  else if (currentValue == 2 && (neighborStateCounts.state2) == 3)
  {
    return 3;
  }
  else if (currentValue == 3 && (neighborStateCounts.state3) == 3)
  {
    return 0;
  }
  else if (currentValue == 0 && (neighborStateCounts.state0) == 3 && numSteps > 6)
  {
    return 1;
  }
  else if (currentValue == 0 && (neighborStateCounts.state0) == 3)
  {
    return 0;
  }
  else
  {
    return currentValue;
  }
}

const DisplayConfig DIAGONAL_WAVES = {
    .startMode = TOP_LEFT,
    .startMin = 1,
    .startMax = 1,
    .cycleTimeMs = 800,
    .getNewStateFn = diagonalWaves};
