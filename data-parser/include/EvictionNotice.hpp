#ifndef EVICTION_NOTICE
#define EVICTION_NOTICE

struct EvictionNotice
{
    bool nonPayment;
    bool breach;
    bool nuisance;
    bool illegal;
    bool failSignRenew;
    bool accessDenial;
    bool unapprovedSubtenant;
    bool ownerMoveIn;
    bool demolition;
    bool capitalImprovement;
    bool substantialRehab;
    bool ellisActWithdrawal;
    bool condoConversion;
    bool roommateSameUnit;
    bool other;
    bool latePay;
    bool leadRemediation;
    bool development;
    bool goodSamaritan;
};

struct NeighborhoodCounts
{
    int nonPayment;
    int breach;
    int nuisance;
    int illegal;
    int failSignRenew;
    int accessDenial;
    int unapprovedSubtenant;
    int ownerMoveIn;
    int demolition;
    int capitalImprovement;
    int substantialRehab;
    int ellisActWithdrawal;
    int condoConversion;
    int roommateSameUnit;
    int other;
    int latePay;
    int leadRemediation;
    int development;
    int goodSamaritan;
};

#endif