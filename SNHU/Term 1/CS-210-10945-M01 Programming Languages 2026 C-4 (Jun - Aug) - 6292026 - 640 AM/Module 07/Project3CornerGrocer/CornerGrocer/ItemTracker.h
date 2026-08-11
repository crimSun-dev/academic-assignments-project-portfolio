// ItemTracker.h
// Author: Draven Chen
// Declares the ItemTracker class, which is responsible for reading the
// grocery purchase records, tallying how often each item appears, and
// providing several ways to view or export that frequency data.
//
// Design note: all of the item/frequency data is kept private inside the
// class. Callers interact with it only through the public member
// functions below, which keeps the internal std::map implementation
// detail hidden and the data protected from accidental modification.

#ifndef ITEMTRACKER_H
#define ITEMTRACKER_H

#include <string>
#include <map>

class ItemTracker
{
public:
    // Constructs the tracker and immediately loads/tallies the data
    // found in inputFileName. If the file cannot be opened, the map
    // is left empty and an error is reported to std::cerr.
    explicit ItemTracker(const std::string& inputFileName);

    // Menu Option One: returns how many times a single item appears.
    // The search is case-insensitive so "apples", "Apples", and
    // "APPLES" all return the same result.
    int getFrequency(const std::string& itemName) const;

    // Menu Option Two: prints every item paired with its frequency,
    // e.g. "Potatoes 4".
    void printAllFrequencies() const;

    // Menu Option Three: prints the same frequency data as a simple
    // text-based histogram, e.g. "Potatoes ****".
    void printHistogram() const;

    // Data File Creation requirement: writes every item/frequency pair
    // to outputFileName (defaults to frequency.dat) so the store has a
    // backup of the accumulated data. Returns true on success.
    bool writeBackupFile(const std::string& outputFileName = "frequency.dat") const;

    // Returns true if the input file was read successfully and at
    // least one item was tallied.
    bool isLoaded() const;

private:
    // Reads inputFileName line by line and increments the count for
    // each item encountered. Called once from the constructor.
    void loadFromFile(const std::string& inputFileName);

    // Normalizes an item name (trims whitespace, converts to a
    // consistent case) so lookups and counts are not thrown off by
    // stray spaces or inconsistent capitalization in the input file.
    static std::string normalize(const std::string& itemName);

    // Holds each unique item name paired with how many times it was
    // purchased. A map keeps the items in sorted (alphabetical) order,
    // which makes Menu Options Two and Three easy to display cleanly.
    std::map<std::string, int> itemFrequencies;

    // Tracks whether loadFromFile succeeded, so the menu can warn the
    // user instead of silently showing empty results.
    bool loadedSuccessfully;
};

#endif // ITEMTRACKER_H
