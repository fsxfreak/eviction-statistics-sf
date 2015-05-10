#ifndef EVICTION_NOTICE
#define EVICTION_NOTICE

#include <array>

enum Indices
{
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
    GOOD_SAMARITAN
};

struct EvictionNotice
{
    std::array<bool, 19> reasons;
};

struct NeighborhoodCounts
{
    std::array<double, 19> counts;
};

#endif