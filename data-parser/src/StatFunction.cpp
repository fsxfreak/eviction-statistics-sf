#include <StatFunctions.hpp>
#include <EvictionNotice.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>

double chiSquareStatistic(const std::map<std::string, NeighborhoodCounts>& counts)
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

    double chiStatistic = 0;
    int i = 0;
    std::for_each (counts.cbegin(), counts.cend(), 
        [&] (const std::pair<std::string, NeighborhoodCounts>& row) {
            for (int j = 0; j < row.second.counts.size(); j++)
            {
                double component = (row.second.counts[j] - expecteds[i].counts[j]) * (row.second.counts[j] - expecteds[i].counts[j]) / expecteds[i].counts[j]; 
                chiStatistic += component;
                std::cout << component << std::endl;
            }
            i++;
        }
    );

    return chiStatistic;
}
