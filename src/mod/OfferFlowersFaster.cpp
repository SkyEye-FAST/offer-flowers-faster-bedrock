#include "mod/OfferFlowersFaster.h"

#include <atomic>

#include "ll/api/Config.h"
#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/actor/ai/goal/OfferFlowerGoal.h"

namespace offer_flowers_faster {

namespace {

constexpr int OriginalChanceDenominator = 8000;

struct Config {
    int version           = 1;
    int speedupMultiplier = 4000;
};

std::atomic<float> chanceToStart{0.5F};

LL_TYPE_INSTANCE_HOOK(
    OfferFlowerGoalCanUseHook,
    ll::memory::HookPriority::Normal,
    OfferFlowerGoal,
    &OfferFlowerGoal::$canUse,
    bool
) {
    float const originalChance   = mChanceToStart;
    float const configuredChance = chanceToStart.load(std::memory_order_relaxed);
    mChanceToStart = configuredChance;
    bool const result = origin();
    mChanceToStart = originalChance;
    return result;
}

} // namespace

OfferFlowersFaster& OfferFlowersFaster::getInstance() {
    static OfferFlowersFaster instance;
    return instance;
}

bool OfferFlowersFaster::load() {
    Config config;
    try {
        ll::config::loadConfig(config, getSelf().getConfigDir() / "config.json");
    } catch (std::exception const& exception) {
        getSelf().getLogger().error("Failed to load config.json: {}", exception.what());
        return false;
    }

    if (config.speedupMultiplier < 1 || config.speedupMultiplier > OriginalChanceDenominator) {
        getSelf().getLogger().error(
            "speedupMultiplier must be between 1 and {}, got {}.",
            OriginalChanceDenominator,
            config.speedupMultiplier
        );
        return false;
    }

    float const configuredChance =
        static_cast<float>(config.speedupMultiplier) / static_cast<float>(OriginalChanceDenominator);
    chanceToStart.store(configuredChance, std::memory_order_relaxed);

    getSelf().getLogger().info(
        "Configured flower-offering chance: {:.6f} (speedup: {}x).",
        configuredChance,
        config.speedupMultiplier
    );
    return true;
}

bool OfferFlowersFaster::enable() {
    if (OfferFlowerGoalCanUseHook::hook() != 0) {
        getSelf().getLogger().error("Failed to hook OfferFlowerGoal::canUse().");
        return false;
    }

    getSelf().getLogger().info("OfferFlowersFaster enabled.");
    return true;
}

bool OfferFlowersFaster::disable() {
    if (!OfferFlowerGoalCanUseHook::unhook()) {
        getSelf().getLogger().error("Failed to remove the OfferFlowerGoal::canUse() hook.");
        return false;
    }

    getSelf().getLogger().info("OfferFlowersFaster disabled.");
    return true;
}

} // namespace offer_flowers_faster

LL_REGISTER_MOD(offer_flowers_faster::OfferFlowersFaster, offer_flowers_faster::OfferFlowersFaster::getInstance());
