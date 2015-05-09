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

    std::map<std::string, std::vector<EvictionNotice>> neighborhoodNotices;

    for (const std::vector<Json::Value> &columns : rawNotices)
    {
        if (columns[DATE].asString().compare("2005-01-01T00:00:00") < 0) //only entries recent 10 years
            continue;

        EvictionNotice notice;

        notice.nonPayment = columns[NON_PAYMENT].asBool();
        notice.breach = columns[BREACH].asBool();
        notice.nuisance = columns[NUISANCE].asBool();
        notice.illegal = columns[ILLEGAL].asBool();
        notice.failSignRenew = columns[FAIL_SIGN_RENEW].asBool();
        notice.accessDenial = columns[ACCESS_DENIAL].asBool();
        notice.nonPayment = columns[NON_PAYMENT].asBool();
        notice.breach = columns[BREACH].asBool();
        notice.nuisance = columns[NUISANCE].asBool();
        notice.illegal = columns[ILLEGAL].asBool();
        notice.failSignRenew = columns[FAIL_SIGN_RENEW].asBool();
        notice.accessDenial = columns[ACCESS_DENIAL].asBool();
        notice.unapprovedSubtenant = columns[UNAPPROVED_SUBTENANT].asBool();
        notice.ownerMoveIn = columns[OWNER_MOVE_IN].asBool();
        notice.demolition = columns[DEMOLITION].asBool();
        notice.capitalImprovement = columns[CAPITAL_IMPROVEMENT].asBool();
        notice.substantialRehab = columns[SUBSTANTIAL_REHAB].asBool();
        notice.ellisActWithdrawal = columns[ELLIS_ACT_WITHDRAWAL].asBool();
        notice.condoConversion = columns[CONDO_CONVERSION].asBool();
        notice.roommateSameUnit = columns[ROOMMATE_SAME_UNIT].asBool();
        notice.other = columns[OTHER].asBool();
        notice.latePay = columns[LATE_PAY].asBool();
        notice.leadRemediation = columns[LEAD_REMEDIATION].asBool();
        notice.development = columns[DEVELOPMENT].asBool();
        notice.goodSamaritan = columns[GOOD_SAMARITAN].asBool();

        neighborhoodNotices[columns[Columns::NEIGHBORHOOD].asString()].push_back(notice);
    }

    std::map<std::string, NeighborhoodCounts> neighborhoodsCounts;
    auto it = neighborhoodNotices.begin();
    auto itend = neighborhoodNotices.end();
    for (; it != itend; ++it)
    {
        for (auto jt = it->second.begin(); jt != it->second.end(); ++jt)
        {
            if (jt->nonPayment)
                neighborhoodsCounts[it->first].nonPayment++;
            if (jt->breach)
                neighborhoodsCounts[it->first].breach++;
            if (jt->nuisance)
                neighborhoodsCounts[it->first].nuisance++;
            if (jt->illegal)
                neighborhoodsCounts[it->first].illegal++;
            if (jt->failSignRenew)
                neighborhoodsCounts[it->first].failSignRenew++;
            if (jt->accessDenial)
                neighborhoodsCounts[it->first].accessDenial++;
            if (jt->unapprovedSubtenant)
                neighborhoodsCounts[it->first].unapprovedSubtenant++;
            if (jt->ownerMoveIn)
                neighborhoodsCounts[it->first].ownerMoveIn++;
            if (jt->demolition)
                neighborhoodsCounts[it->first].demolition++;
            if (jt->capitalImprovement)
                neighborhoodsCounts[it->first].capitalImprovement++;
            if (jt->substantialRehab)
                neighborhoodsCounts[it->first].substantialRehab++;
            if (jt->ellisActWithdrawal)
                neighborhoodsCounts[it->first].ellisActWithdrawal++;
            if (jt->condoConversion)
                neighborhoodsCounts[it->first].condoConversion++;
            if (jt->roommateSameUnit)
                neighborhoodsCounts[it->first].roommateSameUnit++;
            if (jt->other)
                neighborhoodsCounts[it->first].other++;
            if (jt->latePay)
                neighborhoodsCounts[it->first].latePay++;
            if (jt->leadRemediation)
                neighborhoodsCounts[it->first].leadRemediation++;
            if (jt->development)
                neighborhoodsCounts[it->first].development++;
            if (jt->goodSamaritan)
                neighborhoodsCounts[it->first].goodSamaritan++;
        }
    }

    std::cout << "neighborhood,nonpayment,breach,nuisance,illegal,failsignrenew,accessdenial,unapprovedsubtenant,ownermovein,demolition,capitalimprovement,substantialrehab,ellisactwithdrawal,condoconversion,roommatesameunit,other,latepay,leadremediation,development,goodsamaritan" << std::endl;

    std::for_each (neighborhoodsCounts.begin(), neighborhoodsCounts.end(),
        [] (const std::pair<std::string, NeighborhoodCounts>& counts) {

            std::cout << counts.first << ','
                      << counts.second.nonPayment << ','
                      << counts.second.breach << ','
                      << counts.second.nuisance << ','
                      << counts.second.illegal << ','
                      << counts.second.failSignRenew << ','
                      << counts.second.accessDenial << ','
                      << counts.second.unapprovedSubtenant << ','
                      << counts.second.ownerMoveIn << ','
                      << counts.second.demolition << ','
                      << counts.second.capitalImprovement << ','
                      << counts.second.substantialRehab << ','
                      << counts.second.ellisActWithdrawal << ','
                      << counts.second.condoConversion << ','
                      << counts.second.roommateSameUnit << ','
                      << counts.second.other << ','
                      << counts.second.latePay << ','
                      << counts.second.leadRemediation << ','
                      << counts.second.development << ','
                      << counts.second.goodSamaritan << std::endl;
        }
    );

    
}