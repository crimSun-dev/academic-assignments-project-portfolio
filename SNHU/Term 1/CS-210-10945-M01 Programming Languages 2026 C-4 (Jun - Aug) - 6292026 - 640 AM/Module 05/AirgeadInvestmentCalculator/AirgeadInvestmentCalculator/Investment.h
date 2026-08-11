// Investment.h
// Declares the Investment class, which models a compound-interest
// investment account for Airgead Banking's fiscal-responsibility app.
//
// Airgead Banking | 2019 - CS 210 Project Two

#ifndef AIRGEAD_INVESTMENT_H_
#define AIRGEAD_INVESTMENT_H_

#include <stdexcept>

class Investment
{
public:
    // Constructs an Investment and validates every input.
    // Throws std::invalid_argument if any value is out of range.
    Investment(double t_initialInvestment,
               double t_monthlyDeposit,
               double t_annualInterestRate,
               int t_numberOfYears);

    // Accessors (return by value is appropriate here since these
    // are small built-in types; kept const-correct for safety).
    double getInitialInvestment() const;
    double getMonthlyDeposit() const;
    double getAnnualInterestRate() const;
    int getNumberOfYears() const;

    // Prints a year-by-year table of closing balance and interest
    // earned. When t_includeMonthlyDeposit is false, the report is
    // generated as if no monthly deposit is being made, regardless
    // of the stored monthlyDeposit value.
    void displayYearlyReport(bool t_includeMonthlyDeposit) const;

private:
    // Private member data uses the m_ prefix per Airgead's
    // standards document, distinguishing it from local/parameter
    // variables.
    double m_initialInvestment;
    double m_monthlyDeposit;
    double m_annualInterestRate;
    int m_numberOfYears;
};

#endif // AIRGEAD_INVESTMENT_H_
