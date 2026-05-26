#pragma once
#include "Arduino.h"
#include "../constants.h"

enum GameStatus : uint8_t
{
  IN_PROGRESS = 0,
  WON = 1,
  STALEMATE = 2
};

enum GameMode
{
  LAYERS, // player is restricted to a randomly selected layer
  DROP    // cursor drops to lowest unoccupied layer
};

struct GameState
{
  // Board representation
  // State 0 = empty, 1 = Player 1, 2 = Player 2
  uint8_t board[NUM_LAYERS][NUM_POSITIONS];

  // Layer on the board (0..3) that is currently active
  uint8_t activeLayer;

  // Position on the layer (0..15) that is currently active
  uint8_t cursorPosition;

  // Blink state to indicate cursor position
  bool blinkIsOn;
  unsigned long lastBlinkMs = 0;

  bool isPlayer1Turn;

  // winMask[z], bits 0..15 correspond to board[z][i]
  // Corresponds to the LEDs that are part of the winning 4-in-a-row(s)
  uint16_t winMask[NUM_LAYERS];

  GameStatus status;

  GameMode gameMode;

  // The game freezes at the end to display the win/stalemate animation
  unsigned long frozenUntilMs;

  // Cache the bytes to display for each layer
  // since the bytes are rendered much more frequently than they change
  uint8_t cachedLayerBytes[NUM_LAYERS][NUM_SHIFT_REGISTERS];
  bool cacheIsDirty[NUM_LAYERS];
};
