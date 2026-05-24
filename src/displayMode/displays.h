#pragma once
#include "displayConfig.h"
#include "displays/simpleDuo.h"
#include "displays/simpleTrio.h"

const DisplayConfig DISPLAYS[] = {
    SIMPLE_DUO,
    SIMPLE_TRIO};
const int NUM_DISPLAYS = sizeof(DISPLAYS) / sizeof(DISPLAYS[0]);
