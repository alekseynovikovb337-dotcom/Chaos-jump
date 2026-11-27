#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include "main.hpp"
#include "impl.hpp"
#include "chaos_logic.hpp"

using namespace geode::prelude;

namespace {
constexpr int SUPER_TRIGGER = UNIQ_ID("chaos-jump-super-trigger");
constexpr int WEAK_TRIGGER = UNIQ_ID("chaos-jump-weak-trigger");
constexpr int RANDOM_TRIGGER = UNIQ_ID("chaos-jump-random-trigger");

void applyTriggerForce(GJBaseGameLayer* game, float multiplier) {
    if (!game || !game->m_player1) return;
    auto player = game->m_player1;
    if (player->m_isOnGround) {
        player->m_yVelocity = 10.0f * multiplier;
    } else {
        player->m_yVelocity = std::max(0.0f, static_cast<float>(player->m_yVelocity)) * multiplier;
    }
}

void addForceEditor(
    EditTriggersPopup* popup,
    EffectGameObject* trigger,
    const char* title,
    float defaultValue,
    float minValue,
    float maxValue,
    float step
) {
    if (!popup || !popup->m_mainLayer || !trigger) return;
    if (trigger->m_duration <= 0.f) trigger->m_duration = defaultValue;

    // Keep the controls in one coordinate space. The old code mixed
    // mainLayer coordinates with buttonMenu coordinates, placing controls
    // outside the visible/editable part of the popup on Android.
    auto size = popup->m_mainLayer->getContentSize();
    auto controls = CCMenu::create();
    controls->setPosition({size.width / 2.f, size.height / 2.f});
    popup->m_mainLayer->addChild(controls, 100);

    const float rowY = -size.height / 2.f + 42.f;
    auto label = CCLabelBMFont::create("", "bigFont.fnt");
    label->setScale(0.42f);
    label->setAnchorPoint({0.5f, 0.5f});
    label->setPosition({0.f, rowY + 28.f});
    controls->addChild(label);

    auto refresh = [label, trigger, title]() {
        label->setString(fmt::format("{}: {:.2f}x", title, trigger->m_duration).c_str());
    };
    refresh();

    auto minusLabel = CCLabelBMFont::create("-", "bigFont.fnt");
    auto plusLabel = CCLabelBMFont::create("+", "bigFont.fnt");
    auto minus = CCMenuItemExt::createSpriteExtra(minusLabel, [=](auto*) {
        trigger->m_duration = std::max(minValue, trigger->m_duration - step);
        refresh();
    });
    auto plus = CCMenuItemExt::createSpriteExtra(plusLabel, [=](auto*) {
        trigger->m_duration = std::min(maxValue, trigger->m_duration + step);
        refresh();
    });

    minus->setScale(0.7f);
    plus->setScale(0.7f);
    minus->setPosition({-78.f, rowY});
    plus->setPosition({78.f, rowY});
    controls->addChild(minus);
    controls->addChild(plus);
}

void registerChaosTriggers() {
    GameObjectsFactory::registerGameObject(
        GameObjectsFactory::createTriggerConfig(
            SUPER_TRIGGER, "edit_eJumpBtn_001.png",
            [](EffectGameObject* trigger, GJBaseGameLayer* game, int, gd::vector<int> const*) {
                const float force = trigger && trigger->m_duration > 0.f ? trigger->m_duration : 1.8f;
                applyTriggerForce(game, force);
            },
            [](EditTriggersPopup* popup, EffectGameObject* trigger, CCArray*) {
                addForceEditor(popup, trigger, "Super Jump", 1.8f, 1.0f, 8.0f, 0.1f);
            }
        )
    );

    GameObjectsFactory::registerGameObject(
        GameObjectsFactory::createTriggerConfig(
            WEAK_TRIGGER, "edit_eColorBtn_001.png",
            [](EffectGameObject* trigger, GJBaseGameLayer* game, int, gd::vector<int> const*) {
                const float force = trigger && trigger->m_duration > 0.f ? trigger->m_duration : 0.35f;
                applyTriggerForce(game, force);
            },
            [](EditTriggersPopup* popup, EffectGameObject* trigger, CCArray*) {
                addForceEditor(popup, trigger, "Weak Jump", 0.35f, 0.05f, 1.0f, 0.05f);
            }
        )
    );

    GameObjectsFactory::registerGameObject(
        GameObjectsFactory::createTriggerConfig(
            RANDOM_TRIGGER, "edit_eMoveBtn_001.png",
            [](EffectGameObject* trigger, GJBaseGameLayer* game, int, gd::vector<int> const*) {
                const float maxForce = trigger && trigger->m_duration > 0.f ? trigger->m_duration : 2.2f;
                static thread_local std::mt19937 rng{std::random_device{}()};
                std::uniform_real_distribution<float> dist(0.5f, maxForce);
                applyTriggerForce(game, dist(rng));
            },
            [](EditTriggersPopup* popup, EffectGameObject* trigger, CCArray*) {
                addForceEditor(popup, trigger, "Random max", 2.2f, 0.5f, 8.0f, 0.1f);
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

        if (button != PlayerButton::Jump || !wasOnGround || m_isOnGround) return;

        auto mod = Mod::get();
        if (!mod->getSettingValue<bool>("enabled")) return;

        chaos_jump::Settings settings;
        settings.minForce = mod->getSettingValue<float>("min-force");
        settings.maxForce = mod->getSettingValue<float>("max-force");
        settings.superChance = mod->getSettingValue<float>("super-chance");
        settings.superForce = mod->getSettingValue<float>("super-force");
        settings.weakChance = mod->getSettingValue<float>("weak-chance");
        settings.weakForce = mod->getSettingValue<float>("weak-force");

        const float intensity = std::clamp(mod->getSettingValue<float>("intensity"), 0.0f, 2.0f);
        const float rolled = chaos_jump::rollForce(settings);
        const float multiplier = 1.0f + (rolled - 1.0f) * intensity;
        m_yVelocity *= std::max(0.05f, multiplier);
    }
};

$on_mod(Loaded) {
    log::info("Chaos Jump loaded: configurable Super, Weak and Random Jump triggers registered.");
}
