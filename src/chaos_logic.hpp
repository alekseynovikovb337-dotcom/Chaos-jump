#pragma once
#include <algorithm>
#include <random>

namespace chaos_jump {
struct Settings {
    bool enabled = true;
    float minForce = 0.45f;
    float maxForce = 2.75f;
    float superChance = 0.08f;
    float superForce = 4.0f;
};

inline float rollForce(Settings const& settings) {
    static thread_local std::mt19937 rng{std::random_device{}()};
    const float lo = std::max(0.05f, std::min(settings.minForce, settings.maxForce));
    const float hi = std::max(lo, std::max(settings.minForce, settings.maxForce));
    std::uniform_real_distribution<float> force(lo, hi);
    std::bernoulli_distribution super(std::clamp(settings.superChance, 0.0f, 1.0f));
    return super(rng) ? std::max(hi, settings.superForce) : force(rng);
}
} // namespace chaos_jump
