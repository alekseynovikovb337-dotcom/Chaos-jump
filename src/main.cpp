#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include "chaos_logic.hpp"

using namespace geode::prelude;

class $modify(ChaosJumpPlayerObject, PlayerObject) {
    void updateJump(float dt) {
        bool wasJumping = m_isJumping;
        PlayerObject::updateJump(dt);

        auto mod = Mod::get();
        if (!mod->getSettingValue<bool>("enabled")) return;

        // Apply once when the vanilla jump begins; this keeps gravity and
        // collision handling owned by the original game.
        if (!wasJumping && m_isJumping) {
            chaos_jump::Settings settings;
            settings.minForce = mod->getSettingValue<float>("min-force");
            settings.maxForce = mod->getSettingValue<float>("max-force");
            settings.superChance = mod->getSettingValue<float>("super-chance");
            settings.superForce = mod->getSettingValue<float>("super-force");

            const float intensity = std::clamp(
                mod->getSettingValue<float>("intensity"), 0.0f, 2.0f
            );
            const float rolled = chaos_jump::rollForce(settings);
            const float multiplier = 1.0f + (rolled - 1.0f) * intensity;
            m_yVelocity *= std::max(0.05f, multiplier);
        }
    }
};

$on_mod(Loaded) {
    log::info("Chaos Jump loaded. Random jump force hook active.");
}
