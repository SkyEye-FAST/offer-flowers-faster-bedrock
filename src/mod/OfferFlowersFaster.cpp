#include "mod/OfferFlowersFaster.h"

#include "ll/api/memory/Hook.h"
#include "ll/api/mod/RegisterHelper.h"

#include "mc/util/Random.h"
#include "mc/world/actor/ai/goal/OfferFlowerGoal.h"

#include <algorithm>

namespace offer_flowers_faster {
OfferFlowersFaster& OfferFlowersFaster::getInstance() {
    static OfferFlowersFaster instance;
    return instance;
}

namespace {
thread_local bool gOfferFlowerCanUseActive = false;
thread_local bool gRandomHookReentry = false;
constexpr int kOfferFlowerChanceMultiplier = 4000;

class ScopedFlag {
public:
    explicit ScopedFlag(bool& flag) : mFlag(flag) { mFlag = true; }
    ~ScopedFlag() { mFlag = false; }

private:
    bool& mFlag;
};
} // namespace

LL_AUTO_TYPE_INSTANCE_HOOK(
    OfferFlowerGoalCanUseHook,
    ll::memory::HookPriority::Normal,
    OfferFlowerGoal,
    &OfferFlowerGoal::$canUse,
    bool
) {
    ScopedFlag guard(gOfferFlowerCanUseActive);
    return origin();
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    RandomNextIntOfferFlowerHook,
    ll::memory::HookPriority::Normal,
    Random,
    &Random::$nextInt,
    int,
    int bound
) {
    if (gOfferFlowerCanUseActive && !gRandomHookReentry && bound == 8000) {
        ScopedFlag reentryGuard(gRandomHookReentry);

        int adjustedBound = std::max(1, bound / kOfferFlowerChanceMultiplier);
        if (origin(adjustedBound) == 0) {
            return 0;
        }
        int fallback = origin(bound);
        return fallback == 0 ? 1 : fallback;
    }

    return origin(bound);
}

bool OfferFlowersFaster::load() {
    getSelf().getLogger().debug("Loading...");
    return true;
}

bool OfferFlowersFaster::enable() {
    getSelf().getLogger().debug("Enabling...");
    return true;
}

bool OfferFlowersFaster::disable() {
    getSelf().getLogger().debug("Disabling...");
    return true;
}

} // namespace offer_flowers_faster

LL_REGISTER_MOD(offer_flowers_faster::OfferFlowersFaster, offer_flowers_faster::OfferFlowersFaster::getInstance());
