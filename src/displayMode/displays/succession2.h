#pragma once
#include "Arduino.h"
#include "displayMode/displayConfig.h"
#include "constants.h"
#include "getNeighborStateCounts.h"
#include "shared.h"

// Note: eventually blue or red wins out, or stabilizes

static int succession2(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, unsigned long numSteps)
{

  int directions[][3] = {
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
  };

  uint8_t currentValue = getValueAt(cube, layer, position);

  NeighborStateCounts neighborStateCounts = getNeighborStateCounts(cube, layer, position, directions);

  if (currentValue == 2 && (neighborStateCounts.state1) > 1)
  {
    return 1;
  }
  if (currentValue == 1 && (neighborStateCounts.state1) > 4)
  {
    return 0;
  }
  if (currentValue == 0 && (neighborStateCounts.state2) > 0)
  {
    return 2;
  }
  if (currentValue == 0 && (neighborStateCounts.state0) > 15)
  {
    return 2;
  }
  if (currentValue == 2 && (neighborStateCounts.state2) > 18)
  {
    return random(1, 3);
  }
  else
  {
    return currentValue;
  }
}

const DisplayConfig SUCCESSION_2 = {
    .startMode = ALL_RANDOM,
    .startMin = 1,
    .startMax = 2,
    .cycleTimeMs = 1000,
    .getNewStateFn = succession2};
