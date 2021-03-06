#ifndef STAT_FUNCTIONS
#define STAT_FUNCTIONS

#include <vector>
#include <EvictionNotice.hpp>
/*
nonpayment,breach,nuisance,illegal,failsignrenew,accessdenial,unapprovedsubtenant,ownermovein,demolition,capitalimprovement,substantialrehab,ellisactwithdrawal,condoconversion,roommatesameunit,other,latepay,leadremediation,development,goodsamaritan
^ columns
rows -> neighborhoods
*/

double chiSquareStatistic(std::vector<NeighborhoodCounts> counts, int& df, std::vector<std::string> districts);
double chiAreaRight(double testStatistic, int df);
int getDF(std::vector<NeighborhoodCounts> matrix);
bool neighborhoodInDistrict(const std::string& neighborhoodName, const std::string& district);



#endif