#pragma once
#include "gameState.h"
#include "../shared.h"

void initializeGameState(GameState &gameState)
{
  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    for (int position = 0; position < NUM_POSITIONS; position++)
    {
      gameState.board[layer][position] = 0;
    }
    for (int b = 0; b < NUM_SHIFT_REGISTERS; b++)
    {
      gameState.cachedLayerBytes[layer][b] = 0;
    }
    gameState.winMask[layer] = 0;
    gameState.cacheIsDirty[layer] = true;
  }

  // random is min inclusive, max exclusive
  gameState.activeLayer = random(0, NUM_LAYERS);
  gameState.cursorPosition = random(0, NUM_POSITIONS);
  gameState.blinkIsOn = false;
  gameState.lastBlinkMs = 0;
  // The board is wired so that red = 1
  // Since I want blue to go first, init isPlayer1Turn as false
  gameState.isPlayer1Turn = false;
  gameState.status = IN_PROGRESS;
  gameState.frozenUntilMs = 0;
}

void updateCursorPosition(GameState &gameState)
{
  static unsigned long lastMoveCheckMs = 0;
  static int lastLeft = HIGH;
  static int lastRight = HIGH;
  static int lastUp = HIGH;
  static int lastDown = HIGH;

  unsigned long now = millis();

  if (now - lastMoveCheckMs >= DEBOUNCE_MS)
  {
    lastMoveCheckMs = now;

    int currentLeft = digitalRead(LEFT_PIN);
    int currentRight = digitalRead(RIGHT_PIN);
    int currentUp = digitalRead(UP_PIN);
    int currentDown = digitalRead(DOWN_PIN);

    const int oldColumn = gameState.cursorPosition % GRID_DIMENSION;
    const int oldRow = gameState.cursorPosition / GRID_DIMENSION; // C++ automatically rounds down for integer division, so no need for something like Math.floor()

    int newColumn = oldColumn;
    int newRow = oldRow;

    if (currentLeft == LOW && lastLeft == HIGH)
    {
      // moving left
      newColumn = max(0, oldColumn - 1);
    }
    else if (currentRight == LOW && lastRight == HIGH)
    {
      // moving right
      newColumn = min(GRID_DIMENSION - 1, oldColumn + 1);
    }
    else if (currentUp == LOW && lastUp == HIGH)
    {
      // moving up
      newRow = max(0, oldRow - 1);
    }
    else if (currentDown == LOW && lastDown == HIGH)
    {
      // moving down
      newRow = min(GRID_DIMENSION - 1, oldRow + 1);
    }

    if (newRow != oldRow || newColumn != oldColumn)
    {
      gameState.cursorPosition = (newRow * GRID_DIMENSION) + newColumn;
      gameState.cacheIsDirty[gameState.activeLayer] = true;
    }

    lastLeft = currentLeft;
    lastRight = currentRight;
    lastUp = currentUp;
    lastDown = currentDown;
  }
}

void freezeGame(GameState &gameState)
{
  // for game over/stalemate, freeze gameplay for a few seconds to render the game over sequence
  gameState.frozenUntilMs = millis() + 3000;
}

// Return 0,1,2 to indicate the number of opponent pieces that are flanked in the given dxyz direction
uint8_t getNumFlanked(const uint8_t board[NUM_LAYERS][NUM_POSITIONS], int x, int y, int z, int dx, int dy, int dz, uint8_t player)
{
  uint8_t opponentStreak = 0;
  bool isFlanked = false;
  uint8_t opponent = (player == 1) ? 2 : 1;

  for (uint8_t step = 1; step < GRID_DIMENSION; step++)
  {
    int x2 = x + (dx * step);
    int y2 = y + (dy * step);
    int z2 = z + (dz * step);

    // stop if outside of cube
    if (x2 < 0 || y2 < 0 || z2 < 0 || x2 >= GRID_DIMENSION || y2 >= GRID_DIMENSION || z2 >= GRID_DIMENSION)
    {
      break;
    }

    uint8_t value = getValueAt(board, x2, y2, z2);

    if (value == opponent)
    {
      opponentStreak++;
    }
    // stop if same color as player, but toggle isFlanked
    else if (value == player)
    {
      isFlanked = true;
      break;
    }
    else
    {
      // stop if empty spot (or for any other case)
      break;
    }
  }
  return isFlanked ? opponentStreak : 0;
}

uint8_t getStreakLength(const uint8_t board[NUM_LAYERS][NUM_POSITIONS], int x, int y, int z, int dx, int dy, int dz, uint8_t player)
{
  // Not counting the starting position as part of the streak,
  // since will call this for both directions,
  // which would result in the start position being double counted
  uint8_t streakLength = 0;

  for (uint8_t step = 1; step < GRID_DIMENSION; step++)
  {
    int x2 = x + (dx * step);
    int y2 = y + (dy * step);
    int z2 = z + (dz * step);

    // stop if outside of cube
    if (x2 < 0 || y2 < 0 || z2 < 0 || x2 >= GRID_DIMENSION || y2 >= GRID_DIMENSION || z2 >= GRID_DIMENSION)
    {
      break;
    }

    uint8_t value = getValueAt(board, x2, y2, z2);

    // stop if different color
    if (value != player)
    {
      break;
    }

    streakLength++;
  }

  return streakLength;
}

bool isStalemate(const uint8_t board[NUM_LAYERS][NUM_POSITIONS])
{
  for (uint8_t layer = 0; layer < NUM_LAYERS; layer++)
  {
    for (uint8_t position = 0; position < NUM_POSITIONS; position++)
    {
      if (board[layer][position] == 0)
      {
        return false;
      }
    }
  }
  return true;
}

void updatePotentialCaptures(GameState &gameState)
{
  const int x = gameState.cursorPosition % GRID_DIMENSION;
  const int y = gameState.cursorPosition / GRID_DIMENSION; // C++ automatically rounds down for integer division, so no need for something like Math.floor()
  const int z = gameState.activeLayer;
  uint8_t player = getValueAt(gameState.board, x, y, z);

  uint16_t capturedMask[NUM_LAYERS] = {0};

  for (uint8_t d = 0; d < 13; d++)
  {
    const int dx = DIRECTIONS[d][0];
    const int dy = DIRECTIONS[d][1];
    const int dz = DIRECTIONS[d][2];

    // Flanked pieces can be either forwards or backwards from the placed piece
    uint8_t positiveNumFlanked = getNumFlanked(gameState.board, x, y, z, dx, dy, dz, player);
    uint8_t negativeNumFlanked = getNumFlanked(gameState.board, x, y, z, -dx, -dy, -dz, player);

    for (uint8_t step = 1; step <= positiveNumFlanked; step++)
    {
      int currentX = x + dx * step;
      int currentY = y + dy * step;
      int currentZ = z + dz * step;
      int currentPosition = currentY * GRID_DIMENSION + currentX;

      capturedMask[currentZ] |= (uint16_t)(1u << currentPosition);
    }

    for (uint8_t step = 1; step <= negativeNumFlanked; step++)
    {
      int currentX = x - dx * step;
      int currentY = y - dy * step;
      int currentZ = z - dz * step;
      int currentPosition = currentY * GRID_DIMENSION + currentX;

      capturedMask[currentZ] |= (uint16_t)(1u << currentPosition);
    }
  }

  for (uint8_t layer = 0; layer < NUM_LAYERS; layer++)
  {
    if (capturedMask[layer] == 0)
    {
      continue;
    }

    for (uint8_t position = 0; position < NUM_POSITIONS; position++)
    {
      if (capturedMask[layer] & (uint16_t)(1u << position))
      {
        gameState.board[layer][position] = 0;
      }
    }
    gameState.cacheIsDirty[layer] = true;
  }
}

void updatePotentialGameOver(GameState &gameState)
{
  int x = gameState.cursorPosition % GRID_DIMENSION;
  int y = gameState.cursorPosition / GRID_DIMENSION; // C++ automatically rounds down for integer division, so no need for something like Math.floor()
  int z = gameState.activeLayer;
  uint8_t player = getValueAt(gameState.board, x, y, z);

  bool foundWin = false;

  for (uint8_t d = 0; d < 13; d++)
  {
    int dx = DIRECTIONS[d][0];
    int dy = DIRECTIONS[d][1];
    int dz = DIRECTIONS[d][2];

    // Check the streak from both the forwards and backwards direction from the piece
    uint8_t positiveStreak = getStreakLength(gameState.board, x, y, z, dx, dy, dz, player);
    uint8_t negativeStreak = getStreakLength(gameState.board, x, y, z, -dx, -dy, -dz, player);

    uint8_t streakLength = 1 + positiveStreak + negativeStreak;

    if (streakLength == GRID_DIMENSION)
    {
      int startX = x - dx * negativeStreak;
      int startY = y - dy * negativeStreak;
      int startZ = z - dz * negativeStreak;

      for (int step = 0; step < GRID_DIMENSION; step++)
      {
        int currentX = startX + dx * step;
        int currentY = startY + dy * step;
        int currentZ = startZ + dz * step;
        int currentPosition = currentY * GRID_DIMENSION + currentX;

        gameState.winMask[currentZ] |= (1u << (currentPosition));
      }
      foundWin = true;
    }
  }

  if (foundWin)
  {
    gameState.status = WON;
    for (int layer = 0; layer < NUM_LAYERS; layer++)
    {
      gameState.cacheIsDirty[layer] = true;
    }
    freezeGame(gameState);
  }
  else if (isStalemate(gameState.board))
  {
    gameState.status = STALEMATE;
    for (int layer = 0; layer < NUM_LAYERS; layer++)
    {
      gameState.cacheIsDirty[layer] = true;
    }
    freezeGame(gameState);
  }
  else
  {
    gameState.status = IN_PROGRESS;
  }
}

uint8_t selectRandomEmptyIndex(const uint8_t board[NUM_LAYERS][NUM_POSITIONS])
{
  int totalNumPositions = NUM_LAYERS * NUM_POSITIONS;
  uint8_t start = random(0, totalNumPositions);

  for (uint8_t offset = 0; offset < totalNumPositions; offset++)
  {
    uint8_t index = (start + offset) % totalNumPositions;

    uint8_t layer = index / NUM_POSITIONS;
    uint8_t position = index % NUM_POSITIONS;

    if (board[layer][position] == 0)
    {
      return index;
    }
  }

  // technically should never reach here since this should never be called if the board is full (stalemate)
  return 0;
}

void updateBoard(GameState &gameState)
{
  static int lastButtonValue = HIGH;
  static unsigned long lastPressMs = 0;

  unsigned long now = millis();

  int buttonValue = digitalRead(SELECT_PIN);

  if (buttonValue == LOW && lastButtonValue == HIGH && gameState.board[gameState.activeLayer][gameState.cursorPosition] == 0 && (now - lastPressMs) > DEBOUNCE_MS)
  {
    lastPressMs = now;

    // Record the placement on the board
    gameState.board[gameState.activeLayer][gameState.cursorPosition] = gameState.isPlayer1Turn ? 1 : 2;
    gameState.cacheIsDirty[gameState.activeLayer] = true;

    // Record captures
    updatePotentialCaptures(gameState);

    // Check for game over (win or stalemate)
    updatePotentialGameOver(gameState);

    if (gameState.status == IN_PROGRESS)
    {
      // Switch players
      gameState.isPlayer1Turn = !gameState.isPlayer1Turn;

      // Choose a new layer and position
      uint8_t nextIndex = selectRandomEmptyIndex(gameState.board);
      gameState.activeLayer = nextIndex / NUM_POSITIONS;
      gameState.cursorPosition = nextIndex % NUM_POSITIONS;
      gameState.cacheIsDirty[gameState.activeLayer] = true;
    }
  }
  lastButtonValue = buttonValue;
}

void updateBlink(GameState &gameState)
{
  static const unsigned long BLINK_PERIOD_MS = 350;

  unsigned long now = millis();

  if (now - gameState.lastBlinkMs >= BLINK_PERIOD_MS)
  {
    gameState.blinkIsOn = !gameState.blinkIsOn;
    gameState.lastBlinkMs = now;
    if (gameState.status == IN_PROGRESS)
    {
      gameState.cacheIsDirty[gameState.activeLayer] = true;
    }
    else
    {
      for (uint8_t layer = 0; layer < NUM_LAYERS; layer++)
      {
        gameState.cacheIsDirty[layer] = true;
      }
    }
  }
}

// Refreshes one layer when called
// Should call as often as possible
void renderGame(GameState &gameState)
{
  static uint8_t currentLayer = 0;

  if (gameState.cacheIsDirty[currentLayer])
  {
    // clear the cached bytes
    for (uint8_t b = 0; b < NUM_SHIFT_REGISTERS; b++)
    {
      gameState.cachedLayerBytes[currentLayer][b] = 0;
    }

    for (uint8_t position = 0; position < NUM_POSITIONS; position++)
    {
      bool color1IsOn = false;
      bool color2IsOn = false;

      if (gameState.status == IN_PROGRESS)
      {
        // For the cursor position,
        // blink on = player color
        // blink off = bicolor if space is occupied, off otherwise
        if (position == gameState.cursorPosition && currentLayer == gameState.activeLayer)
        {
          if (gameState.blinkIsOn)
          {
            color1IsOn = gameState.isPlayer1Turn;
            color2IsOn = !gameState.isPlayer1Turn;
          }
          else
          {
            if (gameState.board[currentLayer][position] != 0)
            {
              color1IsOn = true;
              color2IsOn = true;
            }
          }
        }
        // For non-cursor position, just show what has been placed
        else
        {
          color1IsOn = (gameState.board[currentLayer][position] == 1);
          color2IsOn = (gameState.board[currentLayer][position] == 2);
        }
      }
      else if (gameState.status == WON)
      {
        // When blink is on, only show winning 4-in-a-row(s)
        // When blink is off, show the full board
        bool highlight = (gameState.winMask[currentLayer] & (1u << position)) != 0;
        if (highlight || !gameState.blinkIsOn)
        {
          color1IsOn = (gameState.board[currentLayer][position] == 1);
          color2IsOn = (gameState.board[currentLayer][position] == 2);
        }
      }
      else if (gameState.status == STALEMATE)
      {
        // When blink is on, show the full board
        // Otherwise, everything is off
        if (gameState.blinkIsOn)
        {
          color1IsOn = (gameState.board[currentLayer][position] == 1);
          color2IsOn = (gameState.board[currentLayer][position] == 2);
        }
      }

      setBytes(position, color1IsOn, color2IsOn, gameState.cachedLayerBytes[currentLayer]);
    }

    gameState.cacheIsDirty[currentLayer] = false;
  }

  renderLEDsForLayer(currentLayer, gameState.cachedLayerBytes[currentLayer]);

  // Hold it briefly
  delayMicroseconds(LAYER_ON_TIME_US);

  // Next layer
  currentLayer++;
  if (currentLayer >= NUM_LAYERS)
  {
    currentLayer = 0;
  }
}
