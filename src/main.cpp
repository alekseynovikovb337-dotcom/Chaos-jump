#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <user95401.game-objects-factory/include/main.hpp>
#include "chaos_logic.hpp"

using namespace geode::prelude;

namespace {
constexpr int SUPER_TRIGGER = UNIQ_ID("chaos-jump-super-trigger");
constexpr int WEAK_TRIGGER = UNIQ_ID("chaos-jump-weak-trigger");
constexpr int RANDOM_TRIGGER = UNIQ_ID("chaos-jump-random-trigger");

void applyTriggerForce(GJBaseGameLayer* game, float multiplier) {
    if (!game) return;
    auto player = game->m_player1;
    if (!player) return;
    if (player->m_isOnGround) {
        // Start a jump when the trigger is crossed while grounded.
        player->m_yVelocity = 10.0f * multiplier;
    } else {
        player->m_yVelocity = std::max(0.0f, player->m_yVelocity) * multiplier;
    }
}

void registerChaosTriggers() {
    GameObjectsFactory::registerGameObject(
        GameObjectsFactory::createTriggerConfig(
            SUPER_TRIGGER, "edit_eEventLinkBtn_001.png",
            [](EffectGameObject*, GJBaseGameLayer* game, int, gd::vector<int> const*) {
                applyTriggerForce(game, 1.8f);
            }
        )
    );

    GameObjectsFactory::registerGameObject(
        GameObjectsFactory::createTriggerConfig(
            WEAK_TRIGGER, "edit_eEventLinkBtn_001.png",
            [](EffectGameObject*, GJBaseGameLayer* game, int, gd::vector<int> const*) {
                applyTriggerForce(game, 0.35f);
            }
        )
    );

    GameObjectsFactory::registerGameObject(
        GameObjectsFactory::createTriggerConfig(
            RANDOM_TRIGGER, "edit_eEventLinkBtn_001.png",
            [](EffectGameObject*, GJBaseGameLayer* game, int, gd::vector<int> const*) {
                static thread_local std::mt19937 rng{std::random_device{}()};
                std::uniform_real_distribution<float> dist(0.5f, 2.2f);
                applyTriggerForce(game, dist(rng));
            }
        )
    );
}
} // namespace

$execute {
    registerChaosTriggers();
}

class $modify(ChaosJumpPlayerObject, PlayerObject) {
    void pushButton(PlayerButton button) {
        const bool wasOnGround = m_isOnGround;
        PlayerObject::pushButton(button);

        if (button != PlayerButton::Jump || !wasOnGround || m_isOnGround) {
            return;
        }

        auto mod = Mod::get();
        if (!mod->getSettingValue<bool>("enabled")) return;

        chaos_jump::Settings settings;
        settings.minForce = mod->getSettingValue<float>("min-force");
        settings.maxForce = mod->getSettingValue<float>("max-force");
        settings.superChance = mod->getSettingValue<float>("super-chance");
        settings.superForce = mod->getSettingValue<float>("super-force");
        settings.weakChance = mod->getSettingValue<float>("weak-chance");
        settings.weakForce = mod->getSettingValue<float>("weak-force");

        const float intensity = std::clamp(
            mod->getSettingValue<float>("intensity"), 0.0f, 2.0f
        );
        const float rolled = chaos_jump::rollForce(settings);
        const float multiplier = 1.0f + (rolled - 1.0f) * intensity;
        m_yVelocity *= std::max(0.05f, multiplier);
    }
};

$on_mod(Loaded) {
    log::info("Chaos Jump loaded: percentage chances and 3 custom trigger types registered.");
}
