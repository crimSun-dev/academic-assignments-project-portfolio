// Clock.h
// Author: Draven Chen
// CS 210 - Project One: Chada Tech Clocks
//
// Declares helper, formatting, display, input, and time-manipulation
// functions for the 12-hour and 24-hour Chada Tech clocks.

#ifndef CLOCK_H
#define CLOCK_H

#include <cstddef>
#include <string>

const int MENU_WIDTH = 28;
const int NUM_MENU_OPTIONS = 4;

// ---------------------------------------------------------------------
// Time storage accessors (single shared clock state)
// ---------------------------------------------------------------------
unsigned int getHour();
void setHour(unsigned int h);
unsigned int getMinute();
void setMinute(unsigned int m);
unsigned int getSecond();
void setSecond(unsigned int s);

// ---------------------------------------------------------------------
// Formatting helpers
// ---------------------------------------------------------------------
std::string twoDigitString(unsigned int n);
std::string nCharString(size_t n, char c);
std::string formatTime24(unsigned int h, unsigned int m, unsigned int s);
std::string formatTime12(unsigned int h, unsigned int m, unsigned int s);

// ---------------------------------------------------------------------
// Display and user interaction
// ---------------------------------------------------------------------
void printMenu(char* menuItems[], unsigned int numItems, unsigned char width);
void displayClocks(unsigned int h, unsigned int m, unsigned int s);
void getInitialTime();
unsigned int getMenuChoice(unsigned int n);
void processMenuChoice(unsigned int choice);
void mainMenu();

// ---------------------------------------------------------------------
// Time manipulation
// ---------------------------------------------------------------------
void addOneHour();
void addOneMinute();
void addOneSecond();

#endif // CLOCK_H
