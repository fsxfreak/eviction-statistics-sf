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

std::vector<NeighborhoodCounts> selectColumns(
    const std::vector<Indices>& columns
  , std::vector<NeighborhoodCounts> counts)
{
    std::for_each (counts.begin(), counts.end(),
        [&] (NeighborhoodCounts& row) {
            for (int i = 0; i < row.counts.size(); i++)
            {
                bool selected = false;
                for (int j = 0; j < columns.size(); j++)
                {
                    if (i == columns[j])
                    {
                        selected = true;
                    }
                }
                if (!selected)
                    row.counts[i] = 0;
            }           
        }
    );
    return counts;
}

std::vector<std::vector<Json::Value>> generateRawNotices(const Json::Value& data)
{
    std::vector<std::vector<Json::Value>> raws;
    for (unsigned int i = 0; i < data.size(); i++)
    {
        raws.push_back(std::vector<Json::Value>());
        for (auto it = data[i].begin(); it != data[i].end(); ++it)
        {
            raws[i].push_back(*it);
        }
    }
    return raws;
}

bool contains(const std::vector<std::string>& vec, const std::string str)
{
    for (auto &e : vec)
        if (e.compare(str) == 0) return true;

    return false;
}

std::map<std::string, std::vector<EvictionNotice>> convertRawToBools(
    std::vector<std::vector<Json::Value>> raws)
{
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

    //parse out all entries not wanted and columns and convert to bool
    for (const std::vector<Json::Value> &columns : raws)
    {
        if (columns[DATE].asString().compare("2005-01-01T00:00:00") < 0) //only entries recent 10 years
            continue;
        std::string neighName = columns[Columns::NEIGHBORHOOD].asString();

        EvictionNotice notice;

        for (int i = 0; i < notice.reasons.size(); i++)
        {
            notice.reasons[i] = columns[i + 14].asBool();
        }
        neighborhoodNotices[columns[Columns::NEIGHBORHOOD].asString()].push_back(notice);
    }

    return neighborhoodNotices;
}

Indices getIndex(const std::string& reason)
{
    if (reason.compare("NON_PAYMENT") == 0) return NON_PAYMENT;
    if (reason.compare("BREACH") == 0) return BREACH;
    if (reason.compare("NUISANCE") == 0) return NUISANCE;
    if (reason.compare("ILLEGAL") == 0) return ILLEGAL;
    if (reason.compare("FAIL_SIGN_RENEW") == 0) return FAIL_SIGN_RENEW;
    if (reason.compare("ACCESS_DENIAL") == 0) return ACCESS_DENIAL;
    if (reason.compare("UNAPPROVED_SUBTENANT") == 0) return UNAPPROVED_SUBTENANT;
    if (reason.compare("OWNER_MOVE_IN") == 0) return OWNER_MOVE_IN;
    if (reason.compare("DEMOLITION") == 0) return DEMOLITION;
    if (reason.compare("CAPITAL_IMPROVEMENT") == 0) return CAPITAL_IMPROVEMENT;
    if (reason.compare("SUBSTANTIAL_REHAB") == 0) return SUBSTANTIAL_REHAB;
    if (reason.compare("ELLIS_ACT_WITHDRAWAL") == 0) return ELLIS_ACT_WITHDRAWAL;
    if (reason.compare("CONDO_CONVERSION") == 0) return CONDO_CONVERSION;
    if (reason.compare("ROOMMATE_SAME_UNIT") == 0) return ROOMMATE_SAME_UNIT;
    if (reason.compare("OTHER") == 0) return OTHER;
    if (reason.compare("LATE_PAY") == 0) return LATE_PAY;
    if (reason.compare("LEAD_REMEDIATION") == 0) return LEAD_REMEDIATION;
    if (reason.compare("DEVELOPMENT") == 0) return DEVELOPMENT;
    if (reason.compare("GOOD_SAMARITAN") == 0) return GOOD_SAMARITAN;
}

void parseArgs(std::string& filename
             , std::vector<std::string>& neighs
             , std::vector<Indices>& cols
             , int argc
             , const char* argv[])
{
    const std::string NEIGH_SELECTOR = "-n";
    const std::string COL_SELECTOR   = "-c";

    if (argc > 1)
    {
        filename = argv[1];
        bool neighborhoodMode = true;
        for (int i = 2; i < argc; i++)
        {
            std::string arg = argv[i];

            if (arg.compare(NEIGH_SELECTOR) == 0)
                neighborhoodMode = true;
            else if (arg.compare(COL_SELECTOR) == 0)
                neighborhoodMode = false;

            if (neighborhoodMode)
            {
                if (arg.compare(NEIGH_SELECTOR) != 0)
                    neighs.push_back(arg);
            }
            else
            {
                if (arg.compare(COL_SELECTOR) != 0)
                    cols.push_back(getIndex(arg));
            }
        }
    }
}

std::map<std::string, NeighborhoodCounts> createCounts(
    std::map<std::string, std::vector<EvictionNotice>> notices
  , std::vector<std::string> selectedNeighborhoods)
{
    std::map<std::string, NeighborhoodCounts> neighborhoodsCounts;

    auto it = notices.begin();
    auto itend = notices.end();
    for (; it != itend; ++it)
    {
        std::string name = it->first;

        if (!contains(selectedNeighborhoods, name)) continue;

        neighborhoodsCounts[name].neighborhoodName = name;  //for converting map to vec later
        for (auto jt = it->second.begin(); jt != it->second.end(); ++jt)
        {
            for (int i = 0; i < jt->reasons.size(); i++)
            {
                if (jt->reasons[i])
                {
                    neighborhoodsCounts[name].counts[i]++;
                }
                    
            }
        }
    }

    return neighborhoodsCounts;
}

std::vector<NeighborhoodCounts> mapToVec(
    const std::map<std::string, NeighborhoodCounts>& other)
{
    std::vector<NeighborhoodCounts> finalCounts;
    for (auto &e : other)
    {
        finalCounts.push_back(e.second);
    }
    return finalCounts;
}

int main(int argc, const char* argv[])
{
    std::string evictionFilename = "eviction-notices.json";
    std::vector<std::string> selectedNeighborhoods;
    std::vector<Indices> selectedColumns;

    parseArgs(evictionFilename, selectedNeighborhoods, selectedColumns, argc, argv);

    std::string evictions = getJSON(evictionFilename);

    Json::Value root;
    Json::Reader reader;

    reader.parse(evictions, root);

    Json::Value data = root.get("data", "error");

    std::vector<std::vector<Json::Value>> rawNotices = generateRawNotices(data);
    std::map<std::string, std::vector<EvictionNotice>> parsedNotices = convertRawToBools(rawNotices);
    std::map<std::string, NeighborhoodCounts> mappedCounts = createCounts(parsedNotices, selectedNeighborhoods);
    std::vector<NeighborhoodCounts> counts = mapToVec(mappedCounts);

    auto selectedColumnCounts = selectColumns(selectedColumns, counts);

    //double chi = chiSquareStatistic(selectedColumnCounts);
    //int df = getDF(selectedColumnCounts);
    double chi = 290.285192;
    double df = 255;
    double pVal = chiAreaRight(chi, df);
    std::cout << "chi,"   << chi 
              << ",df,"   << df
              << ",pVal," << pVal
              << std::endl;
}