#pragma once
#include "Arduino.h"
#include "../constants.h"
#include "cubeState.h"
#include "./shared.h"
#include "displays.h"

// Refreshes one layer when called
// Should call as often as possible
void renderCube(CubeState &cubeState)
{
  static uint8_t currentLayer = 0;

  renderLEDsForLayer(currentLayer, cubeState.cachedLayerBytes[currentLayer]);

  // Hold it briefly
  delayMicroseconds(LAYER_ON_TIME_US);

  // Next layer
  currentLayer++;
  if (currentLayer >= NUM_LAYERS)
  {
    currentLayer = 0;
  }
}

void updateCube(CubeState &cubeState)
{
  cubeState.numSteps++;

  // Make the updates based on the initial state, THEN update the cube state
  uint8_t nextCube[NUM_LAYERS][NUM_POSITIONS];

  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    for (int position = 0; position < NUM_POSITIONS; position++)
    {
      nextCube[layer][position] = DISPLAYS[cubeState.currentDisplay].getNewStateFn(cubeState.cube, layer, position, cubeState.numSteps);
    }
  }

  // todo do I need to track both the cube and cachedLayerBytes?
  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    // clear the cached bytes
    for (uint8_t b = 0; b < NUM_SHIFT_REGISTERS; b++)
    {
      cubeState.cachedLayerBytes[layer][b] = 0;
    }

    for (int position = 0; position < NUM_POSITIONS; position++)
    {
      cubeState.cube[layer][position] = nextCube[layer][position];

      bool color1IsOn = nextCube[layer][position] == 1 || nextCube[layer][position] == 3;
      bool color2IsOn = nextCube[layer][position] == 2 || nextCube[layer][position] == 3;

      setBytes(position, color1IsOn, color2IsOn, cubeState.cachedLayerBytes[layer]);
    }
  }
}

void initializeDisplay()
{
  const DisplayConfig &config = DISPLAYS[cubeState.currentDisplay];

  cubeState.numSteps = 0;

  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    // Initialize the cube randomly
    for (int position = 0; position < NUM_POSITIONS; position++)
    {
      // all off unless start mode is random
      cubeState.cube[layer][position] = config.startMode == ALL_RANDOM ? random(config.startMin, config.startMax + 1) : 0;
    }
  }

  if (config.startMode == TOP_LEFT)
  {
    cubeState.cube[3][0] = random(config.startMin, config.startMax + 1);
  }

  if (config.startMode == SINGLE_RANDOM)
  {
    uint8_t randomIndex = random(0, NUM_LAYERS * NUM_POSITIONS);
    uint8_t randomLayer = randomIndex / NUM_POSITIONS;
    uint8_t randomPosition = randomIndex % NUM_POSITIONS;

    cubeState.cube[randomLayer][randomPosition] = random(config.startMin, config.startMax + 1);
  }

  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    // clear the cached bytes
    for (uint8_t b = 0; b < NUM_SHIFT_REGISTERS; b++)
    {
      cubeState.cachedLayerBytes[layer][b] = 0;
    }

    for (int position = 0; position < NUM_POSITIONS; position++)
    {

      bool color1IsOn = cubeState.cube[layer][position] == 1 || cubeState.cube[layer][position] == 3;
      bool color2IsOn = cubeState.cube[layer][position] == 2 || cubeState.cube[layer][position] == 3;

      setBytes(position, color1IsOn, color2IsOn, cubeState.cachedLayerBytes[layer]);
    }
  }
}
