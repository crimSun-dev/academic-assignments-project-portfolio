// ModuleFiveAssignment.cpp
// CS 210 - Module Five Assignment
// Author: Draven Chen
// This program reads city names and average yearly temperatures (in
// Fahrenheit) from FahrenheitTemperature.txt, converts each temperature
// to Celsius, and writes the results to CelsiusTemperature.txt.

#include <iostream>
#include <fstream>
#include <string>

int main()
{
    // Declare an input file stream to read the Fahrenheit data
    std::ifstream inputFile;
    // Declare an output file stream to write the Celsius data
    std::ofstream outputFile;

    // Variables to hold each city's name and Fahrenheit temperature
    std::string cityName;
    int fahrenheitTemp = 0;

    // Open the input file that contains the Fahrenheit temperatures
    inputFile.open("FahrenheitTemperature.txt");

    // Open (create) the output file that will contain the Celsius temperatures
    outputFile.open("CelsiusTemperature.txt");

    // Make sure the input file opened successfully before continuing
    if (!inputFile.is_open())
    {
        std::cout << "Error: FahrenheitTemperature.txt could not be opened." << std::endl;
        return 1;
    }

    // Read each city name and its temperature, one pair at a time,
    // until there is no more data left in the file
    while (inputFile >> cityName >> fahrenheitTemp)
    {
        // Convert the Fahrenheit temperature to Celsius
        // Formula: (F - 32) * 5 / 9 = C
        double celsiusTemp = (fahrenheitTemp - 32) * 5.0 / 9.0;

        // Write the city name and its converted Celsius temperature
        // to the output file, with a space between them, one city per line
        outputFile << cityName << " " << celsiusTemp << std::endl;
    }

    // Close the input file now that reading is finished
    inputFile.close();

    // Close the output file now that writing is finished
    outputFile.close();

    std::cout << "Conversion complete. Results written to CelsiusTemperature.txt" << std::endl;

    return 0;
}
