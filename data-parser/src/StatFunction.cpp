#include <StatFunctions.hpp>
#include <EvictionNotice.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>

std::string getComponentLabel(const NeighborhoodCounts& count, int col)
{
    const std::vector<std::string> REASONS = 
    {
        "NON_PAYMENT",
        "BREACH",
        "NUISANCE",
        "ILLEGAL",
        "FAIL_SIGN_RENEW",
        "ACCESS_DENIAL",
        "UNAPPROVED_SUBTENANT",
        "OWNER_MOVE_IN",
        "DEMOLITION",
        "CAPITAL_IMPROVEMENT",
        "SUBSTANTIAL_REHAB",
        "ELLIS_ACT_WITHDRAWAL",
        "CONDO_CONVERSION",
        "ROOMMATE_SAME_UNIT",
        "OTHER",
        "LATE_PAY",
        "LEAD_REMEDIATION",
        "DEVELOPMENT",
        "GOOD_SAMARITAN"
    };
    return count.neighborhoodName + ',' + REASONS[col];
} 

double chiSquareStatistic(std::vector<NeighborhoodCounts> counts)
{
    std::vector<int> rowTotals;
    std::vector<int> columnTotals(19);

    std::for_each(counts.cbegin(), counts.cend(),
        [&] (const NeighborhoodCounts& row) {
            int rowTotal = 0;
            for (int i = 0; i < row.counts.size(); i++)
            {
                rowTotal += row.counts[i];
            }
            rowTotals.push_back(rowTotal);

            for (int i = 0; i < columnTotals.size(); i++)
            {
                columnTotals[i] += row.counts[i];                
            }
        }
    );

    int total = 0;
    for (int &e : rowTotals)
        total += e;

    std::vector<NeighborhoodCounts> expecteds(rowTotals.size());
    for (int i = 0; i < expecteds.size(); i++)
    {
        expecteds[i].neighborhoodName = counts[i].neighborhoodName;
        for (int j = 0; j < expecteds[i].counts.size(); j++)
        {
            double expected = rowTotals[i] * columnTotals[j] / static_cast<double>(total);
            expecteds[i].counts[j] = expected;
        }
    }

    std::cout << "Expected matrix," << std::endl;
    for (int i = 0; i < expecteds.size(); i++)
    {
        std::cout << expecteds[i].neighborhoodName << ',';
        for (int j = 0; j < expecteds[i].counts.size(); j++)
        {
            std::cout << expecteds[i].counts[j];
            if (j != expecteds[i].counts.size() - 1)
                std::cout << ',';
        }
        std::cout << std::endl;
    }

    const double EPSILON = 5;
    //remove columns with expecteds less than 5
    std::vector<int> removeColumns;
    int i = 0;
    std::for_each(counts.cbegin(), counts.cend(),
        [&] (const NeighborhoodCounts& row) {
            for (int j = 0; j < row.counts.size(); j++)
            {
                if (expecteds[i].counts[j] < EPSILON)
                    removeColumns.push_back(j);
            }
            i++;
        }
    );

    double chiStatistic = 0;
    i = 0;
    std::for_each (counts.cbegin(), counts.cend(), 
        [&] (const NeighborhoodCounts& row) {
            for (int j = 0; j < row.counts.size(); j++)
            {
                bool shouldContinue = false;
                for (int k = 0; k < removeColumns.size(); k++)
                {
                    if (j == removeColumns[k])
                        shouldContinue = true;
                }

                if (shouldContinue) continue;

                double observed = row.counts[j];
                double expected = expecteds[i].counts[j];

                double component = (observed - expected) * (observed - expected) / expected; 
                chiStatistic += component;
                std::cout << getComponentLabel(row, j) << ',' << component << std::endl;
            }
            i++;
        }
    );

    return chiStatistic;
}
