#pragma once
#include "Arduino.h"
#include "displayMode/displayConfig.h"
#include "constants.h"
#include "getNeighborStateCounts.h"
#include "shared.h"

// Red "shot" propagates as purple and turns blue if it hits the other side (if it doesn't run into another shot).

static int spaceBattle(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, unsigned long numSteps)
{

  int directions[][3] = {
   {1, 0, 0},
   };

  uint8_t currentValue = getValueAt(cube, layer, position);

  NeighborStateCounts neighborStateCounts = getNeighborStateCounts(cube, layer, position, directions);

  if (currentValue == 0 && (neighborStateCounts.state0 + neighborStateCounts.state1 + neighborStateCounts.state2 + neighborStateCounts.state3) == 1 && (neighborStateCounts.state3) == 1 && numSteps % 4 == 0)
  {
    return 2; // corner/edge first propagates purple if present
  }
  else if (currentValue == 0 && (neighborStateCounts.state0 + neighborStateCounts.state1 + neighborStateCounts.state2 + neighborStateCounts.state3) == 1 && (numSteps - 1) % 4 == 0)
  {
    return random(0, 2); // otherwise, corner/edge randomly "sparks" red 1/2 of time on every 3th step if no purples propagating adjacent
  }
  else if (currentValue == 0 && (neighborStateCounts.state0 + neighborStateCounts.state1 + neighborStateCounts.state2 + neighborStateCounts.state3) == 2 && (neighborStateCounts.state1 + neighborStateCounts.state3) == 1 && numSteps % 4 != 0)
  {
    return 3; // red sparks propagate as purple, purple continues to propagate
  }
  else
  {
    return 0; // default is blank, not currentValue
  }
}

const DisplayConfig SPACE_BATTLE = {
    .startMode = ALL_RANDOM,
    .startMin = 0,
    .startMax = 0,
    .cycleTimeMs = 800,
    .getNewStateFn = spaceBattle};
