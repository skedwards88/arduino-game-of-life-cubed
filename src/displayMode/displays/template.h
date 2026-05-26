#pragma once
#include "Arduino.h"
#include "displayMode/displayConfig.h"
#include "constants.h"
#include "getNeighborStateCounts.h"
#include "shared.h"

static int toDo(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, unsigned long numSteps)
{

  // Do some things and return an int
  // Can use things like `getValueAt` and `neighborStateCounts`:
  /*
    uint8_t currentValue = getValueAt(cube, layer, position);

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
    else
    {
      return currentValue;
    }
  */
  return 0;
}

const DisplayConfig TODO = {
    .startMode = ALL_RANDOM,
    .startMin = 1,
    .startMax = 2,
    .cycleTimeMs = 1000,
    .getNewStateFn = toDo};
