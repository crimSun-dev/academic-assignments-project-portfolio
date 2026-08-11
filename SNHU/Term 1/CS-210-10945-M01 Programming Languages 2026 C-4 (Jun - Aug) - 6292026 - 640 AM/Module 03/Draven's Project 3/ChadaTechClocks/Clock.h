// Clock.h
// Author: Draven Chen
// CS 210 - Project One: Chada Tech Clocks
//
// This header declares the Time data structure and all function
// prototypes used to build, display, and update the 12-hour and
// 24-hour clocks required by the Chada Tech functional requirements.

#ifndef CLOCK_H
#define CLOCK_H

#include <string>

// Menu option constants
// Using named constants instead of "magic numbers" makes the switch
// statement in main() self-documenting and easy to maintain.
const int OPTION_ADD_HOUR = 1;
const int OPTION_ADD_MINUTE = 2;
const int OPTION_ADD_SECOND = 3;
const int OPTION_EXIT = 4;

// Time is stored internally using 24-hour values only.
// Both the 12-hour and 24-hour clocks are derived (formatted) from
// this single source of truth, which guarantees the two clocks can
// never fall out of sync with each other.
struct Time
{
    int hour;   // 0 - 23
    int minute; // 0 - 59
    int second; // 0 - 59
};

// ---------------------------------------------------------------------
// Function Prototypes
// ---------------------------------------------------------------------

// Prompts the user for an initial 24-hour time and returns a validated
// Time object. Re-prompts on any out-of-range input.
Time getInitialTime();

// Displays the four-option user menu and returns the validated
// integer choice (1-4). Re-prompts on invalid input.
int displayMenu();

// Increments the stored time by one hour, minute, or second,
// handling rollover (e.g. 23 -> 0, 59 -> 0) internally.
void addOneHour(Time& currentTime);
void addOneMinute(Time& currentTime);
void addOneSecond(Time& currentTime);

// Builds a formatted 12-hour clock string, e.g. "03:22:01 P M".
std::string formatClock12(const Time& currentTime);

// Builds a formatted 24-hour clock string, e.g. "15:22:01".
std::string formatClock24(const Time& currentTime);

// Prints both the 12-hour and 24-hour clocks side-by-side in the
// bordered box format specified in the functional requirements.
void displayClocks(const Time& currentTime);

// Zero-pads a single time component (hour/minute/second) to two
// digits, e.g. 5 -> "05". Shared helper used by both formatters.
std::string padTwoDigits(int value);

#endif // CLOCK_H
