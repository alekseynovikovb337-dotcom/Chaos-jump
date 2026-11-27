#include <Geode/Geode.hpp>
#include "chaos_logic.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    log::info("Chaos Jump loaded: randomized jump-force core initialized.");
}

// Gameplay hooks and editor trigger integration are being added in the next
// implementation pass. Keep this entry point minimal until the target SDK
// signatures for GD 2.2081 are verified, rather than shipping broken hooks.
