#include <StatFunctions.hpp>
#include <EvictionNotice.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>

void printRemoval(int row, int col, const std::map<std::string, NeighborhoodCounts>& countsCopy)
{
    std::string neighborhood = (std::next(countsCopy.begin(), row))->first;
    std::string reason;
    switch (col)
    {
    case Indices::NON_PAYMENT:
        reason = "NON_PAYMENT";
        break;
    case Indices::BREACH:
        reason = "BREACH";
        break;
    case Indices::NUISANCE:
        reason = "NUISANCE";
        break;
    case Indices::ILLEGAL:
        reason = "ILLEGAL";
        break;
    case Indices::FAIL_SIGN_RENEW:
        reason = "FAIL_SIGN_RENEW";
        break;
    case Indices::ACCESS_DENIAL:
        reason = "ACCESS_DENIAL";
        break;
    case Indices::UNAPPROVED_SUBTENANT:
        reason = "UNAPPROVED_SUBTENANT";
        break;
    case Indices::OWNER_MOVE_IN:
        reason = "OWNER_MOVE_IN";
        break;
    case Indices::DEMOLITION:
        reason = "DEMOLITION";
        break;
    case Indices::CAPITAL_IMPROVEMENT:
        reason = "CAPITAL_IMPROVEMENT";
        break;
    case Indices::SUBSTANTIAL_REHAB:
        reason = "SUBSTANTIAL_REHAB";
        break;
    case Indices::ELLIS_ACT_WITHDRAWAL:
        reason = "ELLIS_ACT_WITHDRAWAL";
        break;
    case Indices::CONDO_CONVERSION:
        reason = "CONDO_CONVERSION";
        break;
    case Indices::ROOMMATE_SAME_UNIT:
        reason = "ROOMMATE_SAME_UNIT";
        break;
    case Indices::OTHER:
        reason = "OTHER";
        break;
    case Indices::LATE_PAY:
        reason = "LATE_PAY";
        break;
    case Indices::LEAD_REMEDIATION:
        reason = "LEAD_REMEDIATION";
        break;
    case Indices::DEVELOPMENT:
        reason = "DEVELOPMENT";
        break;
    case Indices::GOOD_SAMARITAN:
        reason = "GOOD_SAMARITAN";
        break;
    }
    std::cout << neighborhood << ',' << reason << std::endl;
} 

double chiSquareStatistic(std::map<std::string, NeighborhoodCounts> counts)
{
    std::vector<int> rowTotals;
    std::vector<int> columnTotals(19);

    std::for_each(counts.cbegin(), counts.cend(),
        [&] (const std::pair<std::string, NeighborhoodCounts>& row) {
            int rowTotal = 0;
            for (int i = 0; i < row.second.counts.size(); i++)
            {
                rowTotal += row.second.counts[i];
            }
            rowTotals.push_back(rowTotal);

            for (int i = 0; i < columnTotals.size(); i++)
            {
                columnTotals[i] += row.second.counts[i];                
            }
        }
    );

    int total = 0;
    for (int &e : rowTotals)
        total += e;

    std::vector<NeighborhoodCounts> expecteds(rowTotals.size());
    for (int i = 0; i < expecteds.size(); i++)
    {
        for (int j = 0; j < expecteds[i].counts.size(); j++)
        {
            double expected = rowTotals[i] * columnTotals[j] / static_cast<double>(total);
            expecteds[i].counts[j] = expected;
        }
    }

    for (int i = 0; i < expecteds.size(); i++)
    {
        for (int j = 0; j < expecteds[i].counts.size(); j++)
        {
            std::cout << expecteds[i].counts[j] << ' ';
        }
        std::cout << std::endl;
    }

    const double EPSILON = 5;
    //remove columns with expecteds less than 5
    std::vector<int> removeColumns;
    int i = 0;
    std::for_each(counts.cbegin(), counts.cend(),
        [&] (const std::pair<std::string, NeighborhoodCounts>& row) {
            for (int j = 0; j < row.second.counts.size(); j++)
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
        [&] (const std::pair<std::string, NeighborhoodCounts>& row) {
            for (int j = 0; j < row.second.counts.size(); j++)
            {
                bool shouldContinue = false;
                for (int k = 0; k < removeColumns.size(); k++)
                {
                    if (j == removeColumns[k])
                        shouldContinue = true;
                }

                if (shouldContinue) { printRemoval(i, j, counts); continue; }

                double observed = row.second.counts[j];
                double expected = expecteds[i].counts[j];

                double component = (observed - expected) * (observed - expected) / expected; 
                chiStatistic += component;
                std::cout << component << std::endl;
            }
            i++;
        }
    );

    return chiStatistic;
}
