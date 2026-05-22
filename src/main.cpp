#include "Arduino.h"

const int GRID_DIMENSION = 4;
const int NUM_LAYERS = GRID_DIMENSION;
const int NUM_POSITIONS = 16;      // Positions (bicolor LEDs) per layer
const int NUM_SHIFT_REGISTERS = 4; // 16 bicolor LEDs = 32 leads; 8 leads controlled by 1 shift register

// Pins for the first shift register
const int CLOCK_PIN = 6;
const int LATCH_PIN = 7;
const int DATA_PIN = 8;

// Pins for the layers
const int LAYER0_PIN = 2;
const int LAYER1_PIN = 3;
const int LAYER2_PIN = 4;
const int LAYER3_PIN = 5;
const int layerPins[NUM_LAYERS] = {LAYER0_PIN, LAYER1_PIN, LAYER2_PIN, LAYER3_PIN};

// todoColin set cycle time
// Duration that each cube state is held for
const unsigned long CYCLE_TIME_MS = 1000;

// Duration that each layer is turned on
// Note this is microseconds, not milliseconds
const unsigned int LAYER_ON_TIME_US = 800;

// These are the possible directions from a position
// (consider a line with between each of these coordinates and
// the coordinate {0,0,0} representing the position).
// todocolin--can omit some directions if desired
const int DIRECTIONS[][3] = {
    // Vary just x, y, or z
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
    // Vary xy, xz, or yz
    // {1, 1, 0},
    // {1, 0, 1},
    // {0, 1, 1},
    // {1, -1, 0},
    // {1, 0, -1},
    // {0, 1, -1},
    // Vary xyz
    // {1, 1, 1},
    // {1, -1, 1},
    // {1, 1, -1},
    // {-1, 1, 1}
};

struct CubeState
{
  // Cube representation
  // The cube is made of bicolor LEDs
  // 0 = off, 1 = Color 1, 2 = Color 2, 3 = Color 1+2
  uint8_t cube[NUM_LAYERS][NUM_POSITIONS];

  // Cache the bytes to display for each layer
  // since the bytes are rendered much more frequently than they change
  uint8_t cachedLayerBytes[NUM_LAYERS][NUM_SHIFT_REGISTERS];

  // todo check that this is the right variable type
  long numSteps;
};

static CubeState cubeState;

uint8_t getValueAtXYZ(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int x, int y, int z)
{
  return cube[z][y * GRID_DIMENSION + x];
}

int getNewState(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, long numSteps)
{
  int x = position % GRID_DIMENSION;
  int y = position / GRID_DIMENSION; // C++ automatically rounds down for integer division, so no need for something like Math.floor()
  uint8_t currentValue = getValueAtXYZ(cube, x, y, layer);

  int numNeighborsWithState0 = 0;
  int numNeighborsWithState1 = 0;
  int numNeighborsWithState2 = 0;
  int numNeighborsWithState3 = 0;

  for (const auto &d : DIRECTIONS)
  {
    // todo instead of storing 13 directions and doing math twice, could store the 26 neighbors and do math once. Or could consolidate the double mathing.
    // todo using all 26 neighbors, but could reduce
    int dx = d[0];
    int dy = d[1];
    int dz = d[2];

    int neighbor1X = x + dx;
    int neighbor1Y = y + dy;
    int neighbor1Z = layer + dz;

    bool neighbor1IsOffCube = neighbor1X < 0 || neighbor1Y < 0 || neighbor1Z < 0 || neighbor1X >= GRID_DIMENSION || neighbor1Y >= GRID_DIMENSION || neighbor1Z >= GRID_DIMENSION;
    if (!neighbor1IsOffCube)
    {

      uint8_t neighbor1Value = getValueAtXYZ(cube, neighbor1X, neighbor1Y, neighbor1Z);

      // todo can probably clean this up
      if (neighbor1Value == 0)
      {
        numNeighborsWithState0++;
      }
      else if (neighbor1Value == 1)
      {
        numNeighborsWithState1++;
      }
      else if (neighbor1Value == 2)
      {
        numNeighborsWithState2++;
      }
      else if (neighbor1Value == 3)
      {
        numNeighborsWithState3++;
      }
    }

    int neighbor2X = x - dx;
    int neighbor2Y = y - dy;
    int neighbor2Z = layer - dz;

    bool neighbor2IsOffCube = neighbor2X < 0 || neighbor2Y < 0 || neighbor2Z < 0 || neighbor2X >= GRID_DIMENSION || neighbor2Y >= GRID_DIMENSION || neighbor2Z >= GRID_DIMENSION;

    if (!neighbor2IsOffCube)
    {
      uint8_t neighbor2Value = getValueAtXYZ(cube, neighbor2X, neighbor2Y, neighbor2Z);

      if (neighbor2Value == 0)
      {
        numNeighborsWithState0++;
      }
      else if (neighbor2Value == 1)
      {
        numNeighborsWithState1++;
      }
      else if (neighbor2Value == 2)
      {
        numNeighborsWithState2++;
      }
      else if (neighbor2Value == 3)
      {
        numNeighborsWithState3++;
      }
    }
  }

  // todocolin--can change the conditions for life here
  // 0 = off, 1 = Color 1, 2 = Color 2, 3 = Color 1+2
  // numSteps can be set
  // Primary rules
  if (currentValue == 0 && (numNeighborsWithState3) == 1)
  {
    return 3;
  }
  else if (currentValue == 0 && (numNeighborsWithState1) == 1)
  {
    return 1;
  }
  else if (currentValue == 0 && (numNeighborsWithState2) == 1)
  {
    return 2;
  }
  // if currently not 0: if has <2 non-0 neighbors -> becomes dead
  // else if (currentValue != 0 && (numNeighborsWithState1 + numNeighborsWithState2 + numNeighborsWithState3) > 5)
  // {
  //   return 0;
  // }
  // // if currently not 0: if has >3 non-0 neighbors -> becomes dead
  else if (currentValue != 0 && (numNeighborsWithState1 + numNeighborsWithState2 + numNeighborsWithState3) > 5)
  {
    return 0;
  }
  //  else if (currentValue == 0)
  //   {
  //     return random(1, 4);
  //   }
  //   else if (currentValue == 0 && (numNeighborsWithState1) >= 4)
  // {
  //   return 2;
  // }
  //   else if (currentValue == 0 && (numNeighborsWithState2) >= 4)
  // {
  //   return 1;
  // }
  // otherwise, unchanged
  else
  {
    return currentValue;
  }
}

void disableAllLayers()
{
  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    digitalWrite(layerPins[layer], LOW);
  }
}

void setBytes(uint8_t position, bool color1IsOn, bool color2IsOn, uint8_t outBytes[NUM_SHIFT_REGISTERS])
{
  // In the cube, the bicolor LEDs are wired to the shift registers in alternating color order
  // (LED 1 color 1 pin, LED 1 color 2 pin, LED 2 color 1 pin, ...)
  // so color 1 pins are even and color2 pins are odd
  uint8_t color1BitIndex = position * 2;     // 0, 2, ...30
  uint8_t color2BitIndex = position * 2 + 1; // 1, 3, ...31

  // Use |= instead of = to write just that bit in the byte
  if (color1IsOn)
  {
    outBytes[color1BitIndex / 8] |= (1u << (color1BitIndex % 8));
  }
  if (color2IsOn)
  {
    outBytes[color2BitIndex / 8] |= (1u << (color2BitIndex % 8));
  }
}

void renderLEDsForLayer(uint8_t layerIndex, uint8_t bytesToRender[NUM_SHIFT_REGISTERS])
{
  disableAllLayers();

  digitalWrite(LATCH_PIN, LOW);

  for (uint8_t b = 0; b < NUM_SHIFT_REGISTERS; b++)
  {
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, bytesToRender[b]);
  }

  digitalWrite(LATCH_PIN, HIGH);

  digitalWrite(layerPins[layerIndex], HIGH);
}

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
      nextCube[layer][position] = getNewState(cubeState.cube, layer, position, cubeState.numSteps);
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

void setup()
{
  // initialize the pseudo-random number generator
  randomSeed(analogRead(A4)); // Note: this should use a floating/unused pin

  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    pinMode(layerPins[layer], OUTPUT);
  }
  disableAllLayers();

  cubeState.numSteps = 0;

  // todocolin -- can be:
  // 0 = random seeding for all spots
  // 1 = top left is random color, all others off
  // 2 = single spot is random color, all others off
  const int START_MODE = 1;
  // todocolin--can change the initial color here. random() is min inclusive, max exclusive
  int randomMin = 1;
  int randomMax = 2;

  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    // Initialize the cube randomly
    for (int position = 0; position < NUM_POSITIONS; position++)
    {
      // all off unless start mode is 0
      cubeState.cube[layer][position] = START_MODE == 0 ? random(randomMin, randomMax) : 0;
    }
  }

  if (START_MODE == 1) {
    cubeState.cube[3][0] = random(randomMin, randomMax);
  }

  if (START_MODE == 2) {
    uint8_t randomIndex = random(0, NUM_LAYERS * NUM_POSITIONS);
    uint8_t randomLayer = randomIndex / NUM_POSITIONS;
    uint8_t randomPosition = randomIndex % NUM_POSITIONS;

    cubeState.cube[randomLayer][randomPosition] = random(randomMin, randomMax);
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

void loop()
{
  unsigned long now = millis();
  static unsigned long lastCycleUpdate = 0;

  if (now - lastCycleUpdate >= CYCLE_TIME_MS)
  {
    lastCycleUpdate = now;
    updateCube(cubeState);
  }

  renderCube(cubeState);
}
