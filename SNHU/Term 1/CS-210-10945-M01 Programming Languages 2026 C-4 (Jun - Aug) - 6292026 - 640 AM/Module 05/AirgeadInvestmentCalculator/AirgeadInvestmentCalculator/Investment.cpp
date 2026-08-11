// Investment.cpp
// Implements the Investment class declared in Investment.h.
//
// Airgead Banking | 2019 - CS 210 Project Two

#include "Investment.h"
#include <iostream>
#include <iomanip>

// Constructor: validates all four inputs before storing them.
// Using exceptions (rather than assert) lets the caller decide how
// to recover, per Airgead's maintainability guidelines.
Investment::Investment(double t_initialInvestment,
                        double t_monthlyDeposit,
                        double t_annualInterestRate,
                        int t_numberOfYears)
{
    if (t_initialInvestment <= 0.0)
    {
        throw std::invalid_argument("Initial investment amount must be a positive number.");
    }
    if (t_monthlyDeposit < 0.0)
    {
        throw std::invalid_argument("Monthly deposit cannot be negative.");
    }
    if (t_annualInterestRate <= 0.0)
    {
        throw std::invalid_argument("Annual interest rate must be a positive number.");
    }
    if (t_numberOfYears <= 0)
    {
        throw std::invalid_argument("Number of years must be a positive whole number.");
    }

    m_initialInvestment = t_initialInvestment;
    m_monthlyDeposit = t_monthlyDeposit;
    m_annualInterestRate = t_annualInterestRate;
    m_numberOfYears = t_numberOfYears;
}

double Investment::getInitialInvestment() const
{
    return m_initialInvestment;
}

double Investment::getMonthlyDeposit() const
{
    return m_monthlyDeposit;
}

double Investment::getAnnualInterestRate() const
{
    return m_annualInterestRate;
}

int Investment::getNumberOfYears() const
{
    return m_numberOfYears;
}

// Walks the investment forward one month at a time, applying the
// compound-interest formula supplied in the functional requirements:
//
//   total           = openingAmount + depositAmount
//   monthlyInterest = total * ((annualInterestRate / 100) / 12)
//   closingBalance  = total + monthlyInterest
//
// The closing balance becomes next month's opening amount. Every
// twelfth month, the running interest total and the closing balance
// are printed as a year-end row.
void Investment::displayYearlyReport(bool t_includeMonthlyDeposit) const
{
    double openingAmount = m_initialInvestment;
    double depositAmount = t_includeMonthlyDeposit ? m_monthlyDeposit : 0.0;
    double yearlyInterestTotal = 0.0;
    int totalMonths = m_numberOfYears * 12;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "======================================================\n";
    std::cout << std::left << std::setw(10) << "Year"
               << std::setw(22) << "Year End Balance"
               << "Year End Earned Interest" << "\n";
    std::cout << "------------------------------------------------------\n";

    for (int month = 1; month <= totalMonths; ++month)
    {
        double total = openingAmount + depositAmount;
        double monthlyInterest = total * ((m_annualInterestRate / 100.0) / 12.0);
        double closingBalance = total + monthlyInterest;

        yearlyInterestTotal += monthlyInterest;
        openingAmount = closingBalance;

        if (month % 12 == 0)
        {
            int currentYear = month / 12;
            std::cout << std::left << std::setw(10) << currentYear
                       << "$" << std::setw(21) << closingBalance
                       << "$" << yearlyInterestTotal << "\n";
            yearlyInterestTotal = 0.0;
        }
    }

    std::cout << "======================================================\n";
}
