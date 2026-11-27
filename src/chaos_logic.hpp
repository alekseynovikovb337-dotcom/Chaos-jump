#pragma once
#include <algorithm>
#include <random>

namespace chaos_jump {
struct Settings {
    float minForce = 0.65f;
    float maxForce = 1.35f;
    float superChance = 8.0f;
    float superForce = 1.8f;
    float weakChance = 12.0f;
    float weakForce = 0.35f;
};

inline float rollForce(Settings const& settings) {
    static thread_local std::mt19937 rng{std::random_device{}()};
    const float lo = std::max(0.05f, std::min(settings.minForce, settings.maxForce));
    const float hi = std::max(lo, std::max(settings.minForce, settings.maxForce));
    const float superChance = std::clamp(settings.superChance / 100.0f, 0.0f, 1.0f);
    const float weakChance = std::clamp(settings.weakChance / 100.0f, 0.0f, 1.0f - superChance);
    std::uniform_real_distribution<float> force(lo, hi);
    std::uniform_real_distribution<float> roll(0.0f, 1.0f);

    const float value = roll(rng);
    if (value < superChance) return std::max(hi, settings.superForce);
    if (value < superChance + weakChance) return std::clamp(settings.weakForce, 0.05f, lo);
    return force(rng);
}
} // namespace chaos_jump
