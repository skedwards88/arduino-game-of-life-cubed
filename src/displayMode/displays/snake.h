#pragma once
#include "Arduino.h"
#include "displayMode/displayConfig.h"
#include "constants.h"
#include "getNeighborStateCounts.h"
#include "shared.h"

// Note: in progress

static int snake(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, unsigned long numSteps)
{

  int directions[][3] = {
      {1, 0, 0},
      {0, 1, 0},
      {0, 0, 1},
  };

  uint8_t currentValue = getValueAt(cube, layer, position);

  NeighborStateCounts neighborStateCounts = getNeighborStateCounts(cube, layer, position, directions);

  // 1 corner starts as red
  if (currentValue == 1)
  {
    return 2;
  } // purple head turns to blue

  else if (currentValue == 0 && neighborStateCounts.state1 > 0 && (neighborStateCounts.state0 + neighborStateCounts.state1 + neighborStateCounts.state2 + neighborStateCounts.state3) == 4 && (numSteps + 6) % 7 == 0)
  {
    return random(0, 2);
  } // at second step in cycle, head moves to edge spot(s) randomly
  else if (currentValue == 0 && neighborStateCounts.state1 > 0 && (neighborStateCounts.state0 + neighborStateCounts.state1 + neighborStateCounts.state2 + neighborStateCounts.state3) == 5 && (numSteps + 5) % 7 == 0)
  {
    return 1;
  } // at first step in cycle, head moves to middle-side spot if able

  else if (currentValue == 0 && neighborStateCounts.state1 > 0 && (neighborStateCounts.state0 + neighborStateCounts.state1 + neighborStateCounts.state2 + neighborStateCounts.state3) == 6 && (numSteps + 4) % 7 == 0)
  {
    return 1;
  } // at first step in cycle, head moves to center spot if able

  else if (currentValue == 0 && neighborStateCounts.state1 > 0 && (neighborStateCounts.state0 + neighborStateCounts.state1 + neighborStateCounts.state2 + neighborStateCounts.state3) == 6 && (numSteps + 3) % 7 == 0)
  {
    return 1;
  } // at first step in cycle, head moves to center spot if able

  else if (currentValue == 0 && neighborStateCounts.state1 > 0 && (neighborStateCounts.state0 + neighborStateCounts.state1 + neighborStateCounts.state2 + neighborStateCounts.state3) == 5 && (numSteps + 2) % 7 == 0)
  {
    return 1;
  } // at first step in cycle, head moves to middle-side spot if able

  else if (currentValue == 0 && neighborStateCounts.state1 > 0 && (neighborStateCounts.state0 + neighborStateCounts.state1 + neighborStateCounts.state2 + neighborStateCounts.state3) == 4 && (numSteps + 1) % 7 == 0)
  {
    return 1;
  } // at second step in cycle, head moves to edge spot(s) randomly

  else if (currentValue == 0 && neighborStateCounts.state1 > 0 && (neighborStateCounts.state0 + neighborStateCounts.state1 + neighborStateCounts.state2 + neighborStateCounts.state3) == 3 && (numSteps) % 7 == 0)
  {
    return 1;
  } // at second step in cycle, head moves to corner spot if able

  {
    return currentValue;
  } // default is blank, not currentValue
}

const DisplayConfig SNAKE = {
    .startMode = ALL_RANDOM,
    .startMin = 1,
    .startMax = 2,
    .cycleTimeMs = 1000,
    .getNewStateFn = snake};
