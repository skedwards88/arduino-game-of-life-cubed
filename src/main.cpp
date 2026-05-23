#include "Arduino.h"
#include "constants.h"
#include "displays.h"

// These are the possible directions from a position
// (consider a line with between each of these coordinates and
// the coordinate {0,0,0} representing the position).
// todo can delete
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

  int currentDisplay;
};

static CubeState cubeState;

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

void initCube()
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

  cubeState.currentDisplay = 0;

  initCube();
}

void loop()
{
  unsigned long now = millis();
  static unsigned long lastCycleUpdate = 0;
  static unsigned long lastDisplaySwitch = 0;

  if (now - lastDisplaySwitch >= DISPlAY_TIME_MS)
  {
    lastDisplaySwitch = now;
    cubeState.currentDisplay = (cubeState.currentDisplay + 1) % NUM_DISPLAYS;
    initCube();
  }

  if (now - lastCycleUpdate >= DISPLAYS[cubeState.currentDisplay].cycleTimeMs)
  {
    lastCycleUpdate = now;
    updateCube(cubeState);
  }

  renderCube(cubeState);
}
