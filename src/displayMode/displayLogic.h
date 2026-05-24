#pragma once
#include "Arduino.h"
#include "../constants.h"
#include "displayState.h"
#include "./shared.h"
#include "displays.h"

// Refreshes one layer when called
// Should call as often as possible
void renderCube(DisplayState &displayState)
{
  static uint8_t currentLayer = 0;

  renderLEDsForLayer(currentLayer, displayState.cachedLayerBytes[currentLayer]);

  // Hold it briefly
  delayMicroseconds(LAYER_ON_TIME_US);

  // Next layer
  currentLayer++;
  if (currentLayer >= NUM_LAYERS)
  {
    currentLayer = 0;
  }
}

void updateCube(DisplayState &displayState)
{
  displayState.numSteps++;

  // Make the updates based on the initial state, THEN update the cube state
  uint8_t nextCube[NUM_LAYERS][NUM_POSITIONS];

  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    for (int position = 0; position < NUM_POSITIONS; position++)
    {
      nextCube[layer][position] = DISPLAYS[displayState.currentDisplay].getNewStateFn(displayState.cube, layer, position, displayState.numSteps);
    }
  }

  // todo do I need to track both the cube and cachedLayerBytes?
  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    // clear the cached bytes
    for (uint8_t b = 0; b < NUM_SHIFT_REGISTERS; b++)
    {
      displayState.cachedLayerBytes[layer][b] = 0;
    }

    for (int position = 0; position < NUM_POSITIONS; position++)
    {
      displayState.cube[layer][position] = nextCube[layer][position];

      bool color1IsOn = nextCube[layer][position] == 1 || nextCube[layer][position] == 3;
      bool color2IsOn = nextCube[layer][position] == 2 || nextCube[layer][position] == 3;

      setBytes(position, color1IsOn, color2IsOn, displayState.cachedLayerBytes[layer]);
    }
  }
}

void initializeDisplay()
{
  const DisplayConfig &config = DISPLAYS[displayState.currentDisplay];

  displayState.numSteps = 0;

  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    // Initialize the cube randomly
    for (int position = 0; position < NUM_POSITIONS; position++)
    {
      // all off unless start mode is random
      displayState.cube[layer][position] = config.startMode == ALL_RANDOM ? random(config.startMin, config.startMax + 1) : 0;
    }
  }

  if (config.startMode == TOP_LEFT)
  {
    displayState.cube[3][0] = random(config.startMin, config.startMax + 1);
  }

  if (config.startMode == SINGLE_RANDOM)
  {
    uint8_t randomIndex = random(0, NUM_LAYERS * NUM_POSITIONS);
    uint8_t randomLayer = randomIndex / NUM_POSITIONS;
    uint8_t randomPosition = randomIndex % NUM_POSITIONS;

    displayState.cube[randomLayer][randomPosition] = random(config.startMin, config.startMax + 1);
  }

  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    // clear the cached bytes
    for (uint8_t b = 0; b < NUM_SHIFT_REGISTERS; b++)
    {
      displayState.cachedLayerBytes[layer][b] = 0;
    }

    for (int position = 0; position < NUM_POSITIONS; position++)
    {

      bool color1IsOn = displayState.cube[layer][position] == 1 || displayState.cube[layer][position] == 3;
      bool color2IsOn = displayState.cube[layer][position] == 2 || displayState.cube[layer][position] == 3;

      setBytes(position, color1IsOn, color2IsOn, displayState.cachedLayerBytes[layer]);
    }
  }
}
