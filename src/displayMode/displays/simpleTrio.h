#pragma once
#include "Arduino.h"
#include "displayMode/displayConfig.h"
#include "constants.h"
#include "getNeighborStateCounts.h"
#include "shared.h"

static int simpleTrio(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, unsigned long numSteps)
{

  int directions[][3] = {
      {1, 0, 0},
      {0, 1, 0},
      {0, 0, 1},
  };

  NeighborStateCounts neighborStateCounts = getNeighborStateCounts(cube, layer, position, directions);

  if ((neighborStateCounts.state2) == 1)
  {
    return 2;
  }
  if ((neighborStateCounts.state3) == 1)
  {
    return 3;
  }
  else
  {
    return 1;
  }
}

const DisplayConfig SIMPLE_TRIO = {
    .startMode = ALL_RANDOM,
    .startMin = 1,
    .startMax = 3,
    .cycleTimeMs = 1000,
    .getNewStateFn = simpleTrio};
