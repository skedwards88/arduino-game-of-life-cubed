#pragma once
#include "Arduino.h"
#include "constants.h"

struct NeighborStateCounts
{
  int state0, state1, state2, state3;
};

template <int N> // this lets the compiler deduce the size of the directions array
NeighborStateCounts getNeighborStateCounts(const uint8_t cube[NUM_LAYERS][NUM_POSITIONS], int layer, int position, const int (&directions)[N][3])
{
  int x = position % GRID_DIMENSION;
  int y = position / GRID_DIMENSION; // C++ automatically rounds down for integer division, so no need for something like Math.floor()

  int counts[4] = {};

  for (const auto &d : directions)
  {
    int dx = d[0];
    int dy = d[1];
    int dz = d[2];

    int offsets[2][3] = {
        {x + dx, y + dy, layer + dz},
        {x - dx, y - dy, layer - dz}};

    for (const auto &offset : offsets)
    {
      int neighborX = offset[0];
      int neighborY = offset[1];
      int neighborZ = offset[2];

      bool neighborIsOffCube = neighborX < 0 || neighborY < 0 || neighborZ < 0 || neighborX >= GRID_DIMENSION || neighborY >= GRID_DIMENSION || neighborZ >= GRID_DIMENSION;

      if (!neighborIsOffCube)
      {
        uint8_t neighborValue = getValueAt(cube, neighborX, neighborY, neighborZ);
        counts[neighborValue]++;
      }
    }
  }

  return {counts[0], counts[1], counts[2], counts[3]};
}
