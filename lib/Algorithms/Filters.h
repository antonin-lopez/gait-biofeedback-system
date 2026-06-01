#ifndef FILTERS_H
#define FILTERS_H

inline float computeEMA(float current, float previous, float alpha) {
    return current * alpha + previous * (1.0f - alpha);
}

#endif // FILTERS_H
