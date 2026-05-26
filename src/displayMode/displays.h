#pragma once
#include "displayConfig.h"
#include "displays/simpleDuo.h"
#include "displays/simpleTrio.h"
#include "displays/asymmetricDuo.h"
#include "displays/conditionalTrio.h"
#include "displays/diagonalWaves.h"
#include "displays/emptySpaceDuo.h"
#include "displays/emptySpaceSolo.h"
#include "displays/snake.h"
#include "displays/spaceBattle.h"
#include "displays/succession.h"
#include "displays/succession2.h"
#include "displays/tendrils.h"

const DisplayConfig DISPLAYS[] = {
    SIMPLE_DUO,
    SIMPLE_TRIO,
    ASYMMETRIC_DUO,
    CONDITIONAL_TRIO,
    DIAGONAL_WAVES,
    EMPTY_SPACE_DUO,
    EMPTY_SPACE_SOLO,
    SNAKE,
    SPACE_BATTLE,
    SUCCESSION,
    SUCCESSION_2,
    TENDRILS

};
const int NUM_DISPLAYS = sizeof(DISPLAYS) / sizeof(DISPLAYS[0]);
