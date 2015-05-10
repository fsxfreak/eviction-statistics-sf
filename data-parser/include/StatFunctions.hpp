#ifndef STAT_FUNCTIONS
#define STAT_FUNCTIONS

#include <map>
#include <EvictionNotice.hpp>
/*
nonpayment,breach,nuisance,illegal,failsignrenew,accessdenial,unapprovedsubtenant,ownermovein,demolition,capitalimprovement,substantialrehab,ellisactwithdrawal,condoconversion,roommatesameunit,other,latepay,leadremediation,development,goodsamaritan
^ columns
rows -> neighborhoods
*/

double chiSquareStatistic(const std::map<std::string, NeighborhoodCounts>& counts);

#endif