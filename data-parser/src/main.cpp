#include <EvictionNotice.hpp>
#include <algorithm>
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <map>
#include <vector>
#include <statistics.h>
#include <specialfunctions.h>
#include <StatFunctions.hpp>

std::string getJSON(const std::string& filename)
{
    std::ifstream file(filename);

    std::string str;
    file.seekg(0, std::ios::end);
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());

    return str;

}

int main()
{
    std::string evictions = getJSON("eviction-notices.json");

    Json::Value root;
    Json::Reader reader;

    reader.parse(evictions, root);

    Json::Value data = root.get("data", "error");

    std::vector<std::vector<Json::Value>> rawNotices;
    for (unsigned int i = 0; i < data.size(); i++)
    {
        rawNotices.push_back(std::vector<Json::Value>());
        for (auto it = data[i].begin(); it != data[i].end(); ++it)
        {
            rawNotices[i].push_back(*it);
        }
    }

    enum Columns
    {
        ADDRESS = 9, CITY, STATE, ZIP,
        DATE,
        NON_PAYMENT,
        BREACH,
        NUISANCE,
        ILLEGAL,
        FAIL_SIGN_RENEW,
        ACCESS_DENIAL,
        UNAPPROVED_SUBTENANT,
        OWNER_MOVE_IN,
        DEMOLITION,
        CAPITAL_IMPROVEMENT,
        SUBSTANTIAL_REHAB,
        ELLIS_ACT_WITHDRAWAL,
        CONDO_CONVERSION,
        ROOMMATE_SAME_UNIT,
        OTHER,
        LATE_PAY,
        LEAD_REMEDIATION,
        DEVELOPMENT,
        GOOD_SAMARITAN,
        CONSTRAINTS,
        CONSTRAINTS_DATE, SUPERVISOR,
        NEIGHBORHOOD,
        COORDINATES
    };

    std::map<std::string, std::vector<EvictionNotice>> neighborhoodNotices;

    for (const std::vector<Json::Value> &columns : rawNotices)
    {
        if (columns[DATE].asString().compare("2005-01-01T00:00:00") < 0) //only entries recent 10 years
            continue;
        std::string neighName = columns[Columns::NEIGHBORHOOD].asString();
        if (neighName.compare("") == 0 
         || neighName.compare("Golden Gate Park") == 0
         || neighName.compare("Presidio") == 0)
            continue;

        EvictionNotice notice;

        for (int i = 0; i < notice.reasons.size(); i++)
        {
            notice.reasons[i] = columns[i + 14].asBool();
        }
        neighborhoodNotices[columns[Columns::NEIGHBORHOOD].asString()].push_back(notice);
    }

    std::map<std::string, NeighborhoodCounts> neighborhoodsCounts;
    auto it = neighborhoodNotices.begin();
    auto itend = neighborhoodNotices.end();
    for (; it != itend; ++it)
    {
        for (auto jt = it->second.begin(); jt != it->second.end(); ++jt)
        {
            if (!(it->first.compare("Tenderloin") == 0 || it->first.compare("Mission") == 0 || it->first.compare("Portola") == 0)) continue;

            for (int i = 0; i < jt->reasons.size(); i++)
            {
                if (jt->reasons[i])
                    neighborhoodsCounts[it->first].counts[i]++;
            }
        }
    }
    
    /*std::cout << "neighborhood,nonpayment,breach,nuisance,illegal,failsignrenew,accessdenial,unapprovedsubtenant,ownermovein,demolition,capitalimprovement,substantialrehab,ellisactwithdrawal,condoconversion,roommatesameunit,other,latepay,leadremediation,development,goodsamaritan" << std::endl;

    std::for_each (neighborhoodsCounts.begin(), neighborhoodsCounts.end(),
        [] (const std::pair<std::string, NeighborhoodCounts>& counts) {
            std::cout << counts.first << ',';
            for (int i = 0; i < counts.second.counts.size(); i++)
            {
                std::cout << counts.second.counts[i] << ',';
            }
            std::cout << '\b' << std::endl;
        }
    );*/

    std::cout << chiSquareStatistic(neighborhoodsCounts) << std::endl;
}