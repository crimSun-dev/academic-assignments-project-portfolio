// main.cpp
// Corner Grocer Item-Tracking Program
// CS 210 Project Three
// Author: Draven Chen
//
// This program reads the Corner Grocer's daily purchase records from
// CS210_Project_Three_Input_File.txt, tallies how often each item was
// purchased, and presents a menu so a store employee can:
//   1) Look up how many times a single item was purchased
//   2) See the frequency of every item purchased that day
//   3) See the same frequencies displayed as a text-based histogram
//   4) Exit the program
//
// A backup copy of the frequency data is written to frequency.dat
// automatically when the program starts, before any menu options are
// shown to the user.

#include <iostream>
#include <string>
#include <limits>
#include "ItemTracker.h"

// Displays the menu text and returns the validated integer choice
// (1-4) that the user entered. Re-prompts on any invalid input
// instead of crashing, per the optional input validation challenge.
// Returns -1 if the input stream ends (EOF) before a valid choice is
// entered -- e.g. the user closes/redirects input rather than typing
// 4 -- so the caller can shut down cleanly instead of looping forever
// on a stream that can no longer produce input.
int getMenuChoice()
{
    int choice = 0;

    std::cout << "\n===== Corner Grocer Item Tracker (by Draven Chen) =====\n";
    std::cout << "1. Search for an item's purchase frequency\n";
    std::cout << "2. Display frequencies for all items\n";
    std::cout << "3. Display frequencies as a histogram\n";
    std::cout << "4. Exit the program\n";
    std::cout << "Enter your choice (1-4): ";

    while (!(std::cin >> choice) || choice < 1 || choice > 4)
    {
        if (std::cin.eof())
        {
            return -1;
        }

        std::cout << "Invalid input. Please enter a number from 1 to 4: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Clear the trailing newline left in the input buffer so a later
    // std::getline() call (used when reading the search term) does
    // not immediately read an empty line.
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return choice;
}

int main()
{
    const std::string inputFileName = "CS210_Project_Three_Input_File.txt";

    // Build the tracker. This reads and tallies the input file once,
    // up front, so every menu option afterward is fast.
    ItemTracker tracker(inputFileName);

    if (!tracker.isLoaded())
    {
        std::cout << "The program could not load any item data from \""
                  << inputFileName << "\".\n";
        std::cout << "Please make sure that file is in the same folder as the program, "
                  << "then run the program again.\n";
        return 1;
    }

    // Data File Creation requirement: back up the accumulated data to
    // frequency.dat automatically, before the user interacts with the
    // menu at all.
    tracker.writeBackupFile("frequency.dat");

    bool running = true;
    while (running)
    {
        int choice = getMenuChoice();

        if (choice == -1)
        {
            // Input stream ended (e.g. redirected input ran out, or
            // stdin was closed) before a valid choice was entered.
            // Exit gracefully instead of looping forever.
            std::cout << "\nNo more input available. Exiting.\n";
            break;
        }

        switch (choice)
        {
        case 1:
        {
            std::string searchItem;
            std::cout << "Enter the item you want to look for: ";
            if (!std::getline(std::cin, searchItem))
            {
                std::cout << "\nNo more input available. Exiting.\n";
                running = false;
                break;
            }

            int frequency = tracker.getFrequency(searchItem);
            std::cout << searchItem << " " << frequency << "\n";
            break;
        }
        case 2:
            tracker.printAllFrequencies();
            break;
        case 3:
            tracker.printHistogram();
            break;
        case 4:
            std::cout << "Thank you for using the Corner Grocer Item Tracker. Goodbye!\n";
            running = false;
            break;
        default:
            // Unreachable because getMenuChoice() already validates
            // the range, but kept for defensive completeness.
            std::cout << "Unexpected menu choice.\n";
            break;
        }
    }

    return 0;
}
