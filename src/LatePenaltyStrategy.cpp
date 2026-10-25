#include "LatePenaltyStrategy.h"
#include <algorithm>

// Business rule: deduct 10% of the available mark per late calendar day,
// for up to three days. Work more than three days late receives zero.
double StandardLatePenaltyStrategy::apply(double score, double maximumMark,
                                          long long daysLate) const {
    if (daysLate <= 0) return score;
    if (daysLate > 3) return 0.0;
    return std::max(0.0, score - (0.10 * maximumMark * daysLate));
}
