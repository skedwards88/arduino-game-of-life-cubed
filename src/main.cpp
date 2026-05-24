#include "Arduino.h"
#include "constants.h"
#include "shared.h"
#include "displayMode/displays.h"
#include "displayMode/displayState.h"
#include "displayMode/displayLogic.h"
#include "connect4Mode/gameState.h"
#include "connect4Mode/gameLogic.h"

enum CubeMode
{
  GAME_MODE,
  DISPLAY_MODE
};

DisplayState displayState; // don't need static since used extern in the file that defines the struct

static GameState gameState;

static CubeMode currentMode;

void setup()
{
  // initialize the pseudo-random number generator
  randomSeed(analogRead(A4)); // Note: this should use a floating/unused pin

  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(SET_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN, INPUT_PULLUP);
  for (int layer = 0; layer < NUM_LAYERS; layer++)
  {
    pinMode(layerPins[layer], OUTPUT);
  }
  disableAllLayers();

  currentMode = DISPLAY_MODE;

  // Display mode init
  displayState.currentDisplay = 0;
  initializeDisplay();

  // Game mode init
  initializeGameState(gameState);
}

void loop()
{
  unsigned long now = millis();
  static int lastSetButtonValue = HIGH;
  static unsigned long lastSetButtonPressMs = 0;
  static unsigned long lastCycleUpdate = 0;
  static unsigned long lastDisplaySwitch = 0;

  int setButtonValue = digitalRead(SET_PIN);

  if (setButtonValue == LOW && lastSetButtonValue == HIGH && (now - lastSetButtonPressMs) > DEBOUNCE_MS)
  {
    lastSetButtonPressMs = now;

    // toggle between display mode and game mode
    currentMode = currentMode == DISPLAY_MODE ? GAME_MODE : DISPLAY_MODE;
  }

  lastSetButtonValue = setButtonValue;

  if (currentMode == DISPLAY_MODE)
  {
    if (now - lastDisplaySwitch >= DISPlAY_TIME_MS)
    {
      lastDisplaySwitch = now;
      displayState.currentDisplay = (displayState.currentDisplay + 1) % NUM_DISPLAYS;
      initializeDisplay();
    }

    if (now - lastCycleUpdate >= DISPLAYS[displayState.currentDisplay].cycleTimeMs)
    {
      lastCycleUpdate = now;
      updateCube(displayState);
    }

    renderCube(displayState);
  }
  else if (currentMode == GAME_MODE)
  {
    updateBlink(gameState);

    // If frozen for game over, just render and return
    if (now < gameState.frozenUntilMs)
    {
      renderGame(gameState);
      return;
    }

    // If not frozen and game is over, reinitialize and return
    // (This happens right after the game over freeze ends)
    if (gameState.status == WON || gameState.status == STALEMATE)
    {
      initializeGameState(gameState);
      return;
    }

    updateCursorPosition(gameState);
    updateBoard(gameState);
    renderGame(gameState);
  }
}
