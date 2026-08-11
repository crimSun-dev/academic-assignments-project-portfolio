// main.cpp
// Author: Draven Chen
// CS 210 - Project One: Chada Tech Clocks
//
// Entry point for the Chada Tech dual-clock application. Initializes
// the starting time, then hands off to mainMenu(), which displays
// both clocks and the menu in a loop until the user exits.

#include "Clock.h"

int main()
{
    getInitialTime();
    mainMenu();

    return 0;
}
