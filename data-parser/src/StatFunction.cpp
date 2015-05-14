#include <StatFunctions.hpp>
#include <EvictionNotice.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cmath>

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

double igf(double s, double z)
{
    if (z < 0.0) return 0.0;

    double sc = (1.0 / s);
    s *= pow(z, s);
    sc *= exp(-z);

    double sum = 1.0;
    double nom = 1.0;
    double denom = 1.0;

    for (int i = 0; i < 200; i++)
    {
        nom *= z;
        s++;
        denom *= s;
        sum += (nom / denom);
    }

    return sum * sc;
}

double fast_gamma(double z)
{
    const double INV_E = 0.36787944117144232159552377016147;
    const double TWOPI = 6.283185307179586476925286766559;

    double d = 1.0 / (10.0 * z);
    d = 1.0 / ((12 * z) - d);
    d = (d + z) * INV_E;
    d = pow(d, z);
    d *= sqrt(TWOPI / z);

    return d;
}

double chiAreaRight(double testStatistic, int df)
{
    if (testStatistic < 0 || df < 1)
    {
        return 0.0;
    }

    double k = df * 0.5;
    double x = testStatistic * 0.5;

    if (df == 2) return exp(-1.0 * x);

    double pVal = igf(k, x);

    if (std::isnan(pVal) || std::isinf(pVal) || pVal <= 1e-8) return 1e-14;

    pVal /= fast_gamma(k);
    return (1.0 - pVal);
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
        std::cout << std::fixed;
        std::cout << std::setprecision(4);
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

int getDF(std::vector<NeighborhoodCounts> matrix)
{
    int numColumns = 0;
    for (int i = 0; i < matrix[0].counts.size(); i++)
    {
        if (matrix[0].counts[i] > 0) numColumns++;
    }
    return (numColumns - 1) * (matrix.size() - 1);
}