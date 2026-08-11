// main.cpp
// Author: Draven Chen
// CS 210 - Project One: Chada Tech Clocks
//
// Simulates a 12-hour and a 24-hour clock that always display the
// same underlying time. The user can add an hour, minute, or second
// to both clocks at once, or exit the program, via a simple menu.
// Per the project directions, main() is kept minimal: all logic is
// modularized into functions declared in Clock.h / defined in Clock.cpp.

#include "Clock.h"

int main()
{
    // Get the starting time from the user (validated 24-hour input).
    Time currentTime = getInitialTime();

    int userChoice = 0;

    // Main program loop: show both clocks, show the menu, then act
    // on the user's selection. Repeats until the user chooses Exit.
    do
    {
        displayClocks(currentTime);
        userChoice = displayMenu();

        switch (userChoice)
        {
        case OPTION_ADD_HOUR:
            addOneHour(currentTime);
            break;
        case OPTION_ADD_MINUTE:
            addOneMinute(currentTime);
            break;
        case OPTION_ADD_SECOND:
            addOneSecond(currentTime);
            break;
        case OPTION_EXIT:
            // No time change; loop condition below will end the program.
            break;
        }

    } while (userChoice != OPTION_EXIT);

    return 0;
}
