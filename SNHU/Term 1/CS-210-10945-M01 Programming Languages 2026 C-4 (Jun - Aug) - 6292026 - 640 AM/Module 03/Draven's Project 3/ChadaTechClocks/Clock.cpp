// Clock.cpp
// Author: Draven Chen
// CS 210 - Project One: Chada Tech Clocks
//
// This file implements all clock-related functions declared in
// Clock.h: input validation, time manipulation (add hour/minute/
// second with rollover), and formatted display of the 12-hour and
// 24-hour clocks.

#include "Clock.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

// ---------------------------------------------------------------------
// padTwoDigits
// Zero-pads a numeric time component to two digits so the clocks
// always render as HH:MM:SS instead of, for example, "3:2:1".
// ---------------------------------------------------------------------
string padTwoDigits(int value)
{
    ostringstream paddedValue;
    paddedValue << setw(2) << setfill('0') << value;
    return paddedValue.str();
}

// ---------------------------------------------------------------------
// getInitialTime
// Collects the starting hour, minute, and second from the user in
// 24-hour form and validates each field before returning the Time.
// Using 24-hour input avoids ambiguity around AM/PM at start-up.
// ---------------------------------------------------------------------
Time getInitialTime()
{
    Time initialTime{ 0, 0, 0 };

    cout << "Let's set the initial time (24-hour format)." << endl;

    // Validate hour (0-23)
    do
    {
        cout << "Enter the hour (0-23): ";
        cin >> initialTime.hour;
        if (initialTime.hour < 0 || initialTime.hour > 23)
        {
            cout << "Invalid hour. Please enter a value between 0 and 23." << endl;
        }
    } while (initialTime.hour < 0 || initialTime.hour > 23);

    // Validate minute (0-59)
    do
    {
        cout << "Enter the minute (0-59): ";
        cin >> initialTime.minute;
        if (initialTime.minute < 0 || initialTime.minute > 59)
        {
            cout << "Invalid minute. Please enter a value between 0 and 59." << endl;
        }
    } while (initialTime.minute < 0 || initialTime.minute > 59);

    // Validate second (0-59)
    do
    {
        cout << "Enter the second (0-59): ";
        cin >> initialTime.second;
        if (initialTime.second < 0 || initialTime.second > 59)
        {
            cout << "Invalid second. Please enter a value between 0 and 59." << endl;
        }
    } while (initialTime.second < 0 || initialTime.second > 59);

    return initialTime;
}

// ---------------------------------------------------------------------
// displayMenu
// Prints the four available actions and returns a validated choice.
// ---------------------------------------------------------------------
int displayMenu()
{
    int userChoice = 0;

    cout << endl;
    cout << "****************************" << endl;
    cout << "* 1 - Add One Hour         *" << endl;
    cout << "* 2 - Add One Minute       *" << endl;
    cout << "* 3 - Add One Second       *" << endl;
    cout << "* 4 - Exit Program         *" << endl;
    cout << "****************************" << endl;

    do
    {
        cout << "Enter your choice (1-4): ";
        cin >> userChoice;
        if (userChoice < OPTION_ADD_HOUR || userChoice > OPTION_EXIT)
        {
            cout << "Invalid selection. Please choose a number 1 through 4." << endl;
        }
    } while (userChoice < OPTION_ADD_HOUR || userChoice > OPTION_EXIT);

    return userChoice;
}

// ---------------------------------------------------------------------
// addOneHour
// Increments the hour by one, rolling from 23 back to 0.
// ---------------------------------------------------------------------
void addOneHour(Time& currentTime)
{
    currentTime.hour = (currentTime.hour + 1) % 24;
}

// ---------------------------------------------------------------------
// addOneMinute
// Increments the minute by one. Rolling past 59 also advances the
// hour by one (which itself rolls over at 23), so the clocks stay
// consistent, matching a real-world clock's behavior.
// ---------------------------------------------------------------------
void addOneMinute(Time& currentTime)
{
    currentTime.minute++;
    if (currentTime.minute > 59)
    {
        currentTime.minute = 0;
        addOneHour(currentTime);
    }
}

// ---------------------------------------------------------------------
// addOneSecond
// Increments the second by one. Rolling past 59 cascades into
// addOneMinute, which in turn cascades into addOneHour as needed.
// ---------------------------------------------------------------------
void addOneSecond(Time& currentTime)
{
    currentTime.second++;
    if (currentTime.second > 59)
    {
        currentTime.second = 0;
        addOneMinute(currentTime);
    }
}

// ---------------------------------------------------------------------
// formatClock12
// Converts the internal 24-hour time into a 12-hour clock string
// with an "A M" / "P M" suffix, per the functional requirements
// (Clock12 never exceeds 12:59:59).
// ---------------------------------------------------------------------
string formatClock12(const Time& currentTime)
{
    int displayHour = currentTime.hour % 12;
    if (displayHour == 0)
    {
        displayHour = 12; // 0 and 12 (24-hr) both display as 12 in 12-hr format
    }

    string meridiem = (currentTime.hour < 12) ? "A M" : "P M";

    ostringstream formatted;
    formatted << padTwoDigits(displayHour) << ":"
        << padTwoDigits(currentTime.minute) << ":"
        << padTwoDigits(currentTime.second) << " " << meridiem;

    return formatted.str();
}

// ---------------------------------------------------------------------
// formatClock24
// Converts the internal 24-hour time into a 24-hour clock string
// (Clock24 never exceeds 23:59:59).
// ---------------------------------------------------------------------
string formatClock24(const Time& currentTime)
{
    ostringstream formatted;
    formatted << padTwoDigits(currentTime.hour) << ":"
        << padTwoDigits(currentTime.minute) << ":"
        << padTwoDigits(currentTime.second);

    return formatted.str();
}

// ---------------------------------------------------------------------
// displayClocks
// Prints the 12-hour and 24-hour clocks side-by-side in the bordered
// box format shown in the Chada Tech functional requirements.
// ---------------------------------------------------------------------
void displayClocks(const Time& currentTime)
{
    string clock12 = formatClock12(currentTime);
    string clock24 = formatClock24(currentTime);

    cout << endl;
    cout << "*************************** " << "***************************" << endl;
    cout << "*      12-Hour Clock      * " << "*      24-Hour Clock      *" << endl;
    cout << "*      " << clock12 << "      * "
        << "*        " << clock24 << "        *" << endl;
    cout << "*************************** " << "***************************" << endl;
}
