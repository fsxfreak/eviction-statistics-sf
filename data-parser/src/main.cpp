#include <json/json.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <map>
#include <vector>

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

struct EvictionNotice
{


};

int main()
{
    std::string evictions = getJSON("eviction-notices-small.json");

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

    std::cout << rawNotices[4][Columns::ILLEGAL] << std::endl;

    std::map<std::vector<EvictionNotice>> neighborhoodNotices;

}