#include "mod/OfferFlowersFaster.h"

#include <atomic>

#include "ll/api/Config.h"
#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/memory/Hook.h"
#include "mc/util/Random.h"
#include "mc/world/actor/ai/goal/OfferFlowerGoal.h"

namespace offer_flowers_faster {

namespace {

constexpr int OriginalChanceDenominator = 8000;

struct Config {
    int version           = 1;
    int speedupMultiplier = 4000;
};

std::atomic_int  chanceDenominator{OriginalChanceDenominator / 4000};
thread_local int offerFlowerCanUseDepth = 0;

LL_TYPE_INSTANCE_HOOK(
    OfferFlowerGoalCanUseHook,
    ll::memory::HookPriority::Normal,
    OfferFlowerGoal,
    &OfferFlowerGoal::$canUse,
    bool
) {
    ++offerFlowerCanUseDepth;
    bool const result = origin();
    --offerFlowerCanUseDepth;
    return result;
}

LL_TYPE_INSTANCE_HOOK(
    RandomNextIntHook,
    ll::memory::HookPriority::Normal,
    Random,
    static_cast<int (Random::*)(int)>(&Random::$nextInt),
    int,
    int bound
) {
    if (offerFlowerCanUseDepth > 0 && bound == OriginalChanceDenominator) {
        bound = chanceDenominator.load(std::memory_order_relaxed);
    }
    return origin(bound);
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

    int const denominator =
        (OriginalChanceDenominator + config.speedupMultiplier / 2) / config.speedupMultiplier;
    chanceDenominator.store(denominator, std::memory_order_relaxed);

    getSelf().getLogger().info(
        "Configured flower-offering chance: 1/{} (effective speedup: {:.3f}x).",
        denominator,
        static_cast<double>(OriginalChanceDenominator) / denominator
    );
    return true;
}

bool OfferFlowersFaster::enable() {
    if (RandomNextIntHook::hook() != 0) {
        getSelf().getLogger().error("Failed to hook Random::nextInt(int).");
        return false;
    }
    if (OfferFlowerGoalCanUseHook::hook() != 0) {
        RandomNextIntHook::unhook();
        getSelf().getLogger().error("Failed to hook OfferFlowerGoal::canUse().");
        return false;
    }

    getSelf().getLogger().info("OfferFlowersFaster enabled.");
    return true;
}

bool OfferFlowersFaster::disable() {
    bool const canUseUnhooked  = OfferFlowerGoalCanUseHook::unhook();
    bool const nextIntUnhooked = RandomNextIntHook::unhook();
    if (!canUseUnhooked || !nextIntUnhooked) {
        getSelf().getLogger().error("Failed to remove one or more hooks.");
        return false;
    }

    getSelf().getLogger().info("OfferFlowersFaster disabled.");
    return true;
}

} // namespace offer_flowers_faster

LL_REGISTER_MOD(offer_flowers_faster::OfferFlowersFaster, offer_flowers_faster::OfferFlowersFaster::getInstance());
