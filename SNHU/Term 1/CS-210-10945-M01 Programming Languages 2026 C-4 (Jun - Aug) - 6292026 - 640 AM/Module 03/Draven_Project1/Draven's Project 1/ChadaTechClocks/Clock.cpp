// Clock.cpp
// Author: Draven Chen
// CS 210 - Project One: Chada Tech Clocks
//
// Implements helper, formatting, display, input, and time-manipulation
// functions for the Chada Tech dual-clock application.

#include "Clock.h"

#include <iostream>
#include <string>

using namespace std;

namespace
{
    unsigned int currentHour = 0;
    unsigned int currentMinute = 0;
    unsigned int currentSecond = 0;
}

// ---------------------------------------------------------------------
// Time storage accessors
// ---------------------------------------------------------------------
unsigned int getHour()
{
    return currentHour;
}

void setHour(unsigned int h)
{
    currentHour = h;
}

unsigned int getMinute()
{
    return currentMinute;
}

void setMinute(unsigned int m)
{
    currentMinute = m;
}

unsigned int getSecond()
{
    return currentSecond;
}

void setSecond(unsigned int s)
{
    currentSecond = s;
}

// ---------------------------------------------------------------------
// twoDigitString
// Formats a number as two digits, with a leading 0 if needed.
// ---------------------------------------------------------------------
string twoDigitString(unsigned int n)
{
    string str = "";
    if (n < 10)
    {
        str += "0";
    }
    str += to_string(n);
    return str;
}

// ---------------------------------------------------------------------
// nCharString
// Returns a string of length n where every character is c.
// ---------------------------------------------------------------------
string nCharString(size_t n, char c)
{
    string str = "";
    for (size_t i = 0; i < n; i++)
    {
        str += c;
    }
    return str;
}

// ---------------------------------------------------------------------
// formatTime24
// Returns hh:mm:ss in 24-hour format.
// ---------------------------------------------------------------------
string formatTime24(unsigned int h, unsigned int m, unsigned int s)
{
    return twoDigitString(h) + ":" + twoDigitString(m) + ":" + twoDigitString(s);
}

// ---------------------------------------------------------------------
// formatTime12
// Returns hh:mm:ss A M or hh:mm:ss P M in 12-hour format.
// ---------------------------------------------------------------------
string formatTime12(unsigned int h, unsigned int m, unsigned int s)
{
    unsigned int displayHour = h % 12;
    if (displayHour == 0)
    {
        displayHour = 12;
    }

    string meridiem = (h < 12) ? "A M" : "P M";
    return twoDigitString(displayHour) + ":" + twoDigitString(m) + ":"
        + twoDigitString(s) + " " + meridiem;
}

// ---------------------------------------------------------------------
// printMenu
// Prints a bordered menu built from the provided option strings.
// ---------------------------------------------------------------------
void printMenu(char* menuItems[], unsigned int numItems, unsigned char width)
{
    cout << nCharString(width, '*') << endl;

    for (unsigned int i = 0; i < numItems; i++)
    {
        string optionIndex = to_string(i + 1);
        string line = "* " + optionIndex + " - " + menuItems[i];
        int spacesNeeded = static_cast<int>(width) - static_cast<int>(line.length()) - 1;

        cout << line << nCharString(spacesNeeded, ' ') << "*" << endl;

        if (i != numItems - 1)
        {
            cout << endl;
        }
    }

    cout << nCharString(width, '*') << endl;
}

// ---------------------------------------------------------------------
// displayClocks
// Prints both clocks side-by-side using the Sense layout specification.
// ---------------------------------------------------------------------
void displayClocks(unsigned int h, unsigned int m, unsigned int s)
{
    cout << nCharString(27, '*') << "   " << nCharString(27, '*') << endl;
    cout << "*" << nCharString(6, ' ') << "12-HOUR CLOCK" << nCharString(6, ' ') << "*"
        << "   "
        << "*" << nCharString(6, ' ') << "24-HOUR CLOCK" << nCharString(6, ' ') << "*"
        << endl;
    cout << endl;
    cout << "*" << nCharString(6, ' ') << formatTime12(h, m, s) << nCharString(7, ' ') << "*"
        << "   "
        << "*" << nCharString(8, ' ') << formatTime24(h, m, s) << nCharString(9, ' ') << "*"
        << endl;
    cout << nCharString(27, '*') << "   " << nCharString(27, '*') << endl;
}

// ---------------------------------------------------------------------
// getInitialTime
// Prompts for and validates the starting 24-hour time.
// ---------------------------------------------------------------------
void getInitialTime()
{
    unsigned int hour = 0;
    unsigned int minute = 0;
    unsigned int second = 0;

    cout << "Let's set the initial time (24-hour format)." << endl;

    do
    {
        cout << "Enter the hour (0-23): ";
        cin >> hour;
        if (hour > 23)
        {
            cout << "Invalid hour. Please enter a value between 0 and 23." << endl;
        }
    } while (hour > 23);

    do
    {
        cout << "Enter the minute (0-59): ";
        cin >> minute;
        if (minute > 59)
        {
            cout << "Invalid minute. Please enter a value between 0 and 59." << endl;
        }
    } while (minute > 59);

    do
    {
        cout << "Enter the second (0-59): ";
        cin >> second;
        if (second > 59)
        {
            cout << "Invalid second. Please enter a value between 0 and 59." << endl;
        }
    } while (second > 59);

    setHour(hour);
    setMinute(minute);
    setSecond(second);
}

// ---------------------------------------------------------------------
// getMenuChoice
// Displays the menu and returns a validated choice from 1 to n.
// ---------------------------------------------------------------------
unsigned int getMenuChoice(unsigned int n)
{
    char* menuItems[NUM_MENU_OPTIONS] = {
        (char*)"Add One Hour",
        (char*)"Add One Minute",
        (char*)"Add One Second",
        (char*)"Exit Program"
    };

    unsigned int choice = 0;

    do
    {
        cout << endl;
        printMenu(menuItems, NUM_MENU_OPTIONS, static_cast<unsigned char>(MENU_WIDTH));
        cout << "Enter your choice (1-" << n << "): ";
        cin >> choice;

        if (choice < 1 || choice > n)
        {
            cout << "Invalid selection. Please choose a number 1 through " << n << "." << endl;
        }
    } while (choice < 1 || choice > n);

    return choice;
}

// ---------------------------------------------------------------------
// processMenuChoice
// Applies the selected menu action to the shared clock state.
// ---------------------------------------------------------------------
void processMenuChoice(unsigned int choice)
{
    switch (choice)
    {
    case 1:
        addOneHour();
        break;
    case 2:
        addOneMinute();
        break;
    case 3:
        addOneSecond();
        break;
    case 4:
        break;
    }
}

// ---------------------------------------------------------------------
// mainMenu
// Reads menu choices and updates time until the user selects exit.
// Sense tests this function without displayClocks; main() handles display.
// ---------------------------------------------------------------------
void mainMenu()
{
    unsigned int choice = 0;

    do
    {
        choice = getMenuChoice(NUM_MENU_OPTIONS);
        processMenuChoice(choice);
    } while (choice != 4);
}

// ---------------------------------------------------------------------
// addOneHour
// Adds one hour with rollover from 23 to 0.
// ---------------------------------------------------------------------
void addOneHour()
{
    if (getHour() <= 22)
    {
        setHour(getHour() + 1);
    }
    else
    {
        setHour(0);
    }
}

// ---------------------------------------------------------------------
// addOneMinute
// Adds one minute with rollover into addOneHour when needed.
// ---------------------------------------------------------------------
void addOneMinute()
{
    if (getMinute() <= 58)
    {
        setMinute(getMinute() + 1);
    }
    else
    {
        setMinute(0);
        addOneHour();
    }
}

// ---------------------------------------------------------------------
// addOneSecond
// Adds one second with rollover into addOneMinute when needed.
// ---------------------------------------------------------------------
void addOneSecond()
{
    if (getSecond() <= 58)
    {
        setSecond(getSecond() + 1);
    }
    else
    {
        setSecond(0);
        addOneMinute();
    }
}
