#pragma once
#include "Arduino.h"
#include "displayMode/displayConfig.h"
#include "constants.h"
#include "getNeighborStateCounts.h"
#include "shared.h"

// Note: eventually freezes or goes into simple cycles

static int emptySpaceSolo(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, unsigned long numSteps)
{

  int directions[][3] = {
      {1, 0, 0},
      {0, 1, 0},
      {0, 0, 1},
  };

  uint8_t currentValue = getValueAt(cube, layer, position);

  NeighborStateCounts neighborStateCounts = getNeighborStateCounts(cube, layer, position, directions);

  if (currentValue == 0 && (neighborStateCounts.state1) > 1)
  {
    return 1;
  }
  if (currentValue == 1 && (neighborStateCounts.state1) > 1)
  {
    return 0;
  }
  else
  {
    return currentValue;
  }
}

const DisplayConfig EMPTY_SPACE_SOLO = {
    .startMode = ALL_RANDOM,
    .startMin = 0,
    .startMax = 0,
    .cycleTimeMs = 200,
    .getNewStateFn = emptySpaceSolo};
