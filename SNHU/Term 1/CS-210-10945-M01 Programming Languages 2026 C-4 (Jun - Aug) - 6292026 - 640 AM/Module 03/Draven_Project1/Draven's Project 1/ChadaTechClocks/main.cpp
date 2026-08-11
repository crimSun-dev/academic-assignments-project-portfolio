// main.cpp
// Author: Draven Chen
// CS 210 - Project One: Chada Tech Clocks
//
// Entry point for the Chada Tech dual-clock application. Initializes
// the starting time, then loops: display both clocks, read menu choice,
// and update time until the user exits.

#include "Clock.h"

int main()
{
    getInitialTime();

    unsigned int choice = 0;

    do
    {
        displayClocks(getHour(), getMinute(), getSecond());
        choice = getMenuChoice(NUM_MENU_OPTIONS);
        processMenuChoice(choice);
    } while (choice != 4);

    return 0;
}
