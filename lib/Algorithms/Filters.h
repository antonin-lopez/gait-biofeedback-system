#pragma once

// Filtre moyenne mobile exponentielle (EMA).
inline float computeEMA(float current, float previous, float alpha) {
    return current * alpha + previous * (1.0f - alpha);
}
