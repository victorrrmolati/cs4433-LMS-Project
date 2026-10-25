#ifndef LATE_PENALTY_STRATEGY_H
#define LATE_PENALTY_STRATEGY_H

class LatePenaltyStrategy {
public:
    virtual ~LatePenaltyStrategy() = default;
    virtual double apply(double score, double maximumMark, long long daysLate) const = 0;
};

class StandardLatePenaltyStrategy final : public LatePenaltyStrategy {
public:
    double apply(double score, double maximumMark, long long daysLate) const override;
};
#endif
