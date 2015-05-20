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

    for (int j = 0; j < matrix[0].counts.size(); j++)
    {
        bool columnGreaterFive = true;
        for (int i = 0; i < matrix.size(); i++)
        {
            if (matrix[i].counts[j] < 5) columnGreaterFive = false;
        }
        if (columnGreaterFive) numColumns++;
    }

    std::cout << numColumns << '\t' << matrix.size() << std::endl;
        
    return (numColumns - 1) * (matrix.size() - 1);
}