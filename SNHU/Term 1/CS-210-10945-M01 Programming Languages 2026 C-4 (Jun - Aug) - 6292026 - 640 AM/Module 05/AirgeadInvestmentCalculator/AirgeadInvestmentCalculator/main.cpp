// main.cpp
// Console driver for the Airgead Banking Investment Growth Calculator.
// Collects and validates user input, then displays two year-by-year
// reports (with and without additional monthly deposits) using the
// Investment class.
//
// Per Airgead's portability guideline, main() is kept small: it is
// used only to gather input and call into the Investment class.
//
// Airgead Banking | 2019 - CS 210 Project Two

#include <iostream>
#include <limits>
#include <string>
#include "Investment.h"

// Clears any error flags on std::cin and discards the rest of the
// current input line. Used after a failed read so the next prompt
// starts from a clean line.
void clearInputStream()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Prompts the user until a strictly positive double is entered.
double getPositiveDoubleInput(const std::string& t_prompt)
{
    double userValue = 0.0;

    while (true)
    {
        std::cout << t_prompt;
        std::cin >> userValue;

        if (std::cin.fail() || userValue <= 0.0)
        {
            std::cout << "Invalid entry. Please enter a positive number.\n";
            clearInputStream();
        }
        else
        {
            clearInputStream();
            return userValue;
        }
    }
}

// Prompts the user until a double that is zero or greater is entered.
// Used for monthly deposit, which is allowed to be zero.
double getNonNegativeDoubleInput(const std::string& t_prompt)
{
    double userValue = 0.0;

    while (true)
    {
        std::cout << t_prompt;
        std::cin >> userValue;

        if (std::cin.fail() || userValue < 0.0)
        {
            std::cout << "Invalid entry. Please enter a number that is zero or greater.\n";
            clearInputStream();
        }
        else
        {
            clearInputStream();
            return userValue;
        }
    }
}

// Prompts the user until a strictly positive whole number is entered.
int getPositiveIntegerInput(const std::string& t_prompt)
{
    int userValue = 0;

    while (true)
    {
        std::cout << t_prompt;
        std::cin >> userValue;

        if (std::cin.fail() || userValue <= 0)
        {
            std::cout << "Invalid entry. Please enter a positive whole number.\n";
            clearInputStream();
        }
        else
        {
            clearInputStream();
            return userValue;
        }
    }
}

int main()
{
    std::cout << "*****************************************\n";
    std::cout << "********** Data Input *******************\n";

    double initialInvestmentAmount = getPositiveDoubleInput("Initial Investment Amount: $");
    double monthlyDepositAmount = getNonNegativeDoubleInput("Monthly Deposit: $");
    double annualInterestRate = getPositiveDoubleInput("Annual Interest (%): ");
    int numberOfYears = getPositiveIntegerInput("Number of Years: ");

    std::cout << "Press any key to continue . . .";
    std::cin.get();
    std::cout << "\n\n";

    try
    {
        Investment investmentAccount(initialInvestmentAmount,
                                      monthlyDepositAmount,
                                      annualInterestRate,
                                      numberOfYears);

        std::cout << "Balance and Interest Without Additional Monthly Deposits\n";
        investmentAccount.displayYearlyReport(false);

        std::cout << "\n";

        std::cout << "Balance and Interest With Additional Monthly Deposits\n";
        investmentAccount.displayYearlyReport(true);
    }
    catch (const std::exception& t_error)
    {
        // Gracefully report the problem instead of letting the
        // program crash, per Airgead's maintainability guidelines.
        std::cout << "An error occurred while processing your investment: "
                   << t_error.what() << "\n";
        return 1;
    }

    std::cout << "\nPress any key to exit . . .";
    std::cin.get();

    return 0;
}
