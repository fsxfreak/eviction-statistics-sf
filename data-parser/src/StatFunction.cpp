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

 /*  The following c++ functions for calculating normal and
        chi-square probabilities and critical values were adapted by
        John Walker from C implementations
        written by Gary Perlman of Wang Institute, Tyngsboro, MA
        01879.  Both the original C code and this c++ edition
        are in the public domain.  */

/*  POZ  --  probability of normal z value

    Adapted from a polynomial approximation in:
            Ibbetson D, Algorithm 209
            Collected Algorithms of the CACM 1963 p. 616
    Note:
            This routine has six digit accuracy, so it is only useful for absolute
            z values < 6.  For z values >= to 6.0, poz() returns 0.0.
*/ 

//ibbetson D, algorithm 209, from Gary Perlman
double poz(double z)
{
    double y, x, w;
    double Z_MAX = 6.0;

    if (z == 0.0) 
    {
        x = 0.0;
    }
    else
    {
        y = 0.5 * abs(z);
        if (y >= (Z_MAX * 0.5)) 
        {
            x = 1.0;
        }
        else if (y < 1.0)
        {
            w = y * y;
            x = ((((((((0.000124818987 * w
                      - 0.001075204047) * w + 0.005198775019) * w
                      - 0.019198292004) * w + 0.059054035642) * w
                      - 0.151968751364) * w + 0.319152932694) * w
                      - 0.531923007300) * w + 0.797884560593) * y * 2.0;
        }
        else
        {
            y -= 2.0;
            x = (((((((((((((-0.000045255659 * y
                            + 0.000152529290) * y - 0.000019538132) * y
                            - 0.000676904986) * y + 0.001390604284) * y
                            - 0.000794620820) * y - 0.002034254874) * y
                            + 0.006549791214) * y - 0.010557625006) * y
                            + 0.011630447319) * y - 0.009279453341) * y
                            + 0.005353579108) * y - 0.002141268741) * y
                            + 0.000535310849) * y + 0.999936657524;
        }
    }
    return z > 0.0 ? ((x + 1.0) * 0.5) : ((1.0 - x) * 0.5);
}

const double BIGX = 20.0;
double ex(double x) { return (x < -BIGX) ? 0.0 : exp(x); }

/*  POCHISQ  --  probability of chi-square value

              Adapted from:
                      Hill, I. D. and Pike, M. C.  Algorithm 299
                      Collected Algorithms for the CACM 1967 p. 243
              Updated for rounding errors based on remark in
                      ACM TOMS June 1985, page 185
*/
double chiAreaRight(double x, int df)
{
    const double LOG_SQRT_PI = 0.5723649429247000870717135;
    const double I_SQRT_PI   = 0.5641895835477562869480795;

    double a, y, s;
    double e, c, z;
    bool even;

    if (x <= 0.0 || df < 1) return 1.0;

    a = 0.5 * x;
    even = (df % 2 == 0);

    if (df > 1)
        y = ex(-a);

    s = even ? y : (2.0 * poz(-sqrt(x)));

    if (df > 2)
    {
        x = 0.5 * (df - 1.0);
        z = even ? 1.0 : 0.5;
        if (a > BIGX)
        {
            e = even ? 0.0 : LOG_SQRT_PI;
            c = log(a);
            while (z <= x)
            {
                e = log(z) + e;
                s += ex(c * z - a - e);
                z += 1.0;
            }
            return s;
        }
        else
        {
            e = even ? 1.0 : (I_SQRT_PI / sqrt(a));
            c = 0.0;
            while (z <= x) 
            {
                e = e * (a / z);
                c = c + e;
                z += 1.0;
            }
            return c * y + s;
        }
    }
    else
    {
        return s;
    }

}

//hill, I. D. and Pike, M. C. Algorithm 299
double chiSquareStatistic(std::vector<NeighborhoodCounts> counts
                        , int& df
                        , std::vector<std::string> districts)
{
    bool districtMode = (districts.size() > 0);

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

    int numRows = 0;
    int numCols = 0;

    std::vector<NeighborhoodCounts> districtCounts(districts.size());
    for (int k = 0; k < districts.size(); k++)
    {
        districtCounts[k].neighborhoodName = districts[k];
    }

    double chiStatistic = 0;
    i = 0;
    std::for_each (counts.cbegin(), counts.cend(), 
        [&] (const NeighborhoodCounts& row) {
            numRows++;
            for (int j = 0; j < row.counts.size(); j++)
            {
                bool shouldContinue = false;
                for (int k = 0; k < removeColumns.size(); k++)
                {
                    if (j == removeColumns[k])
                        shouldContinue = true;
                }

                if (shouldContinue) continue;

                numCols++;

                double observed = row.counts[j];
                double expected = expecteds[i].counts[j];

                double component = (observed - expected) * (observed - expected) / expected; 
                chiStatistic += component;

                if (!districtMode)
                    std::cout << getComponentLabel(row, j) << ',' << component << std::endl;
                else
                {
                    for (int k = 0; k < districtCounts.size(); k++)
                    {
                        if (neighborhoodInDistrict(row.neighborhoodName, districtCounts[k].neighborhoodName))
                            districtCounts[k].counts[j] += component;
                    }
                }
            }
            i++;
        }
    );

    for (auto &e : districtCounts)
    {
        for (int i = 0; i < e.counts.size(); i++)
        {
            if (e.counts[i] > 0)
                std::cout << getComponentLabel(e, i) << ',' << e.counts[i] << std::endl;
        }
    }

    numCols /= numRows;

    df = (numCols - 1) * (numRows - 1);

    return chiStatistic;
}

bool neighborhoodInDistrict(const std::string& neighborhoodName, const std::string& district)
{
    std::vector<std::vector<std::string>> DISTRICTS(11);
    DISTRICTS[0].push_back("Outer Richmond");
    DISTRICTS[0].push_back("Inner Richmond");
    DISTRICTS[0].push_back("Lone Mountain/USF");

    DISTRICTS[1].push_back("Presidio Heights");
    DISTRICTS[1].push_back("Marina");
    DISTRICTS[1].push_back("Russian Hill");
    DISTRICTS[1].push_back("Pacific Heights");

    DISTRICTS[2].push_back("North Beach");
    DISTRICTS[2].push_back("Nob Hill");

    DISTRICTS[3].push_back("Sunset/Parkside");

    DISTRICTS[4].push_back("Haight Ashbury");
    DISTRICTS[4].push_back("Hayes Valley");
    DISTRICTS[4].push_back("Western Addition");

    DISTRICTS[5].push_back("Tenderloin");
    DISTRICTS[5].push_back("South of Market");
    DISTRICTS[5].push_back("Financial District/South Beach");

    DISTRICTS[6].push_back("Lakeshore");

    DISTRICTS[7].push_back("Noe Valley");
    DISTRICTS[7].push_back("Twin Peaks");
    DISTRICTS[7].push_back("Glen Park");

    DISTRICTS[8].push_back("Portola");
    DISTRICTS[8].push_back("Bernal Heights");
    DISTRICTS[8].push_back("Mission");

    DISTRICTS[9].push_back("Visitacion Valley");
    DISTRICTS[9].push_back("Bayview Hunters Point");
    DISTRICTS[9].push_back("Portrero Hill");

    DISTRICTS[10].push_back("Oceanview/Merced/Ingleside");
    DISTRICTS[10].push_back("Outer Mission");
    DISTRICTS[10].push_back("Excelsior");

    int districtNum = district[0] - '0';

    for (int i = 0; i < DISTRICTS[districtNum - 1].size(); i++)
    {
        return DISTRICTS[districtNum - 1][i].compare(neighborhoodName) == 0;
    }
}