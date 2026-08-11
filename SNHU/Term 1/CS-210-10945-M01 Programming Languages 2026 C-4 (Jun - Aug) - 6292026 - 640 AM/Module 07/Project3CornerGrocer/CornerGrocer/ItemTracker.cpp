// ItemTracker.cpp
// Author: Draven Chen
// Implements the ItemTracker class declared in ItemTracker.h.

#include "ItemTracker.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>

// Constructor: kick off loading the data right away so the tracker is
// ready to answer queries as soon as it is created.
ItemTracker::ItemTracker(const std::string& inputFileName)
    : loadedSuccessfully(false)
{
    loadFromFile(inputFileName);
}

// Reads the input file one item per line and builds up the frequency
// map. Using std::map::operator[] means a brand-new item is
// automatically inserted with a starting count of 0 before it is
// incremented, so there is no need to check for existence first.
void ItemTracker::loadFromFile(const std::string& inputFileName)
{
    std::ifstream inputFile(inputFileName);

    if (!inputFile.is_open())
    {
        std::cerr << "Error: could not open input file \"" << inputFileName
                  << "\". Please make sure it is in the same folder as the program.\n";
        return;
    }

    std::string line;
    while (std::getline(inputFile, line))
    {
        std::string item = normalize(line);
        if (!item.empty())
        {
            ++itemFrequencies[item];
        }
    }

    inputFile.close();
    loadedSuccessfully = !itemFrequencies.empty();
}

// Trims leading/trailing whitespace (including any trailing '\r' left
// over from Windows-style line endings) and capitalizes the item the
// same way every time, so "onions", " Onions", and "ONIONS\r" are all
// treated as the same item.
std::string ItemTracker::normalize(const std::string& itemName)
{
    size_t start = 0;
    size_t end = itemName.size();

    while (start < end && std::isspace(static_cast<unsigned char>(itemName[start])))
    {
        ++start;
    }
    while (end > start && std::isspace(static_cast<unsigned char>(itemName[end - 1])))
    {
        --end;
    }

    std::string trimmed = itemName.substr(start, end - start);

    if (trimmed.empty())
    {
        return trimmed;
    }

    // Store with a consistent "Titlecase" style: first letter
    // uppercase, the rest lowercase. This keeps the on-screen output
    // tidy regardless of how the item was typed or stored in the file.
    std::string result = trimmed;
    for (size_t i = 0; i < result.size(); ++i)
    {
        result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
    }
    result[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[0])));

    return result;
}

bool ItemTracker::isLoaded() const
{
    return loadedSuccessfully;
}

// Menu Option One
int ItemTracker::getFrequency(const std::string& itemName) const
{
    std::string key = normalize(itemName);
    auto it = itemFrequencies.find(key);

    if (it == itemFrequencies.end())
    {
        return 0;
    }
    return it->second;
}

// Menu Option Two
void ItemTracker::printAllFrequencies() const
{
    if (itemFrequencies.empty())
    {
        std::cout << "No item data is currently loaded.\n";
        return;
    }

    for (const auto& pair : itemFrequencies)
    {
        std::cout << pair.first << " " << pair.second << "\n";
    }
}

// Menu Option Three
void ItemTracker::printHistogram() const
{
    if (itemFrequencies.empty())
    {
        std::cout << "No item data is currently loaded.\n";
        return;
    }

    for (const auto& pair : itemFrequencies)
    {
        std::cout << pair.first << " " << std::string(pair.second, '*') << "\n";
    }
}

// Data File Creation requirement
bool ItemTracker::writeBackupFile(const std::string& outputFileName) const
{
    std::ofstream outputFile(outputFileName);

    if (!outputFile.is_open())
    {
        std::cerr << "Error: could not create backup file \"" << outputFileName << "\".\n";
        return false;
    }

    for (const auto& pair : itemFrequencies)
    {
        outputFile << pair.first << " " << pair.second << "\n";
    }

    outputFile.close();
    return true;
}
