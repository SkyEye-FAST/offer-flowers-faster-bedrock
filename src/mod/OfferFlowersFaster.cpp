#include "mod/OfferFlowersFaster.h"

#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/memory/Hook.h"
#include "mc/world/actor/ai/goal/OfferFlowerGoal.h"

namespace offer_flowers_faster {

LL_AUTO_TYPE_INSTANCE_HOOK(
    OfferFlowerGoalCanUseHook,
    ll::memory::HookPriority::Normal,
    OfferFlowerGoal,
    &OfferFlowerGoal::$canUse,
    bool
) {
    static constexpr int SPEEDUP_MULTIPLIER = 4000;

    for (int attempt = 0; attempt < SPEEDUP_MULTIPLIER; ++attempt) {
        if (origin()) {
            return true;
        }
    }

    return false;
}

OfferFlowersFaster& OfferFlowersFaster::getInstance() {
    static OfferFlowersFaster instance;
    return instance;
}

bool OfferFlowersFaster::load() {
    getSelf().getLogger().debug("Loading...");
    return true;
}

bool OfferFlowersFaster::enable() {
    getSelf().getLogger().info("OfferFlowersFaster enabled.");
    return true;
}

bool OfferFlowersFaster::disable() {
    getSelf().getLogger().debug("Disabling...");
    return true;
}

} // namespace offer_flowers_faster

LL_REGISTER_MOD(offer_flowers_faster::OfferFlowersFaster, offer_flowers_faster::OfferFlowersFaster::getInstance());
