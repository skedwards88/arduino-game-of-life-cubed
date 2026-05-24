#pragma once
#include "Arduino.h"
#include "constants.h"

inline uint8_t getValueAt(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int x, int y, int z)
{
  return cube[z][y * GRID_DIMENSION + x];
}

inline uint8_t getValueAt(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position)
{
  int x = position % GRID_DIMENSION;
  int y = position / GRID_DIMENSION; // C++ automatically rounds down for integer division, so no need for something like Math.floor()

  return getValueAt(cube, x, y, layer);
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
