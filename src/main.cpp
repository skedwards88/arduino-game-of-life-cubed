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

// Duration that each cube state is held for
const unsigned long CYCLE_TIME_MS = 1000;

// Duration that each layer is turned on
// Note this is microseconds, not milliseconds
const unsigned int LAYER_ON_TIME_US = 800;

// These are the possible directions from a position
// (consider a line with between each of these coordinates and
// the coordinate {0,0,0} representing the position).
// todocolin--can omit some directions if desired
const int DIRECTIONS[13][3] = {
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
    // Vary xyz
    {1, 1, 1},
    {1, -1, 1},
    {1, 1, -1},
    {-1, 1, 1}};

struct CubeState
{
  // Cube representation
  // The cube is made of bicolor LEDs
  // 0 = off, 1 = Color 1, 2 = Color 2, 3 = Color 1+2
  uint8_t cube[NUM_LAYERS][NUM_POSITIONS];

  // Cache the bytes to display for each layer
  // since the bytes are rendered much more frequently than they change
  uint8_t cachedLayerBytes[NUM_LAYERS][NUM_SHIFT_REGISTERS];
};

static CubeState cubeState;

uint8_t getValueAtXYZ(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int x, int y, int z)
{
  return cube[z][y * GRID_DIMENSION + x];
}

int getNewState(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position)
{
  int x = position % GRID_DIMENSION;
  int y = position / GRID_DIMENSION; // C++ automatically rounds down for integer division, so no need for something like Math.floor()
  uint8_t currentValue = getValueAtXYZ(cube, x, y, layer);

  int numDeadNeighbors = 0;
  int numAliveNeighbors = 0;

  for (uint8_t d = 0; d < 13; d++)
  {
    // todo instead of storing 13 directions and doing math twice, could store the 26 neighbors and do math once. Or could consolidate the double mathing.
    // todo using all 26 neighbors, but could reduce
    int dx = DIRECTIONS[d][0];
    int dy = DIRECTIONS[d][1];
    int dz = DIRECTIONS[d][2];

    int neighbor1X = x + dx;
    int neighbor1Y = y + dy;
    int neighbor1Z = layer + dz;

    uint8_t neighbor1Value = getValueAtXYZ(cube, neighbor1X, neighbor1Y, neighbor1Z);

    if (neighbor1Value == 0)
    {
      numDeadNeighbors++;
    }
    else
    {
      numAliveNeighbors++;
    }

    int neighbor2X = x - dx;
    int neighbor2Y = y - dy;
    int neighbor2Z = layer - dz;

    uint8_t neighbor2Value = getValueAtXYZ(cube, neighbor2X, neighbor2Y, neighbor2Z);

    if (neighbor2Value == 0)
    {
      numDeadNeighbors++;
    }
    else
    {
      numAliveNeighbors++;
    }
  }

  // todocolin--can change the conditions for life here
  // dead = 0, alive = 1. Not using the other colors, but they would be 2
  // if currently dead: if has exactly 2 live neighbors -> becomes alive
  // if currently alive: if has <2 live neighbors -> becomes dead
  // if currently alive: if has >3 live neighbors -> becomes dead
  // otherwise, unchanged
  if (currentValue == 0 && numAliveNeighbors == 2)
  {
    return 1;
  }
  else if (currentValue == 1 && numAliveNeighbors < 2)
  {
    return 0;
  }
  else if (currentValue == 1 && numAliveNeighbors > 3)
  {
    return 0;
  }
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
      cubeState.cube[layer][position] = getNewState(cubeState.cube, layer, position);

      bool color1IsOn = cubeState.cube[layer][position] == 1 || cubeState.cube[layer][position] == 3;
      bool color2IsOn = cubeState.cube[layer][position] == 2 || cubeState.cube[layer][position] == 3;

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

  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    // Initialize the cube randomly
    // random() is min inclusive, max exclusive
    for (int position = 0; position < NUM_POSITIONS; position++)
    {
      // todocolin--can change the initial state here. random() is min inclusive, max exclusive
      cubeState.cube[layer][position] = random(0, 2); // todo can max be set to a variable?
    }

    // Just set the cached bytes to all off; it will update on the schedule
    for (int b = 0; b < NUM_SHIFT_REGISTERS; b++)
    {
      cubeState.cachedLayerBytes[layer][b] = 0;
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
