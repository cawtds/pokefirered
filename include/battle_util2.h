#ifndef GUARD_BATTLE_UTIL2_H
#define GUARD_BATTLE_UTIL2_H

#include "global.h"

void AllocateBattleResources(void);
void FreeBattleResources(void);
void AdjustFriendshipOnBattleFaint(u8 bank);
void SwitchPartyOrderInGameMulti(u8 battler, u8 arg1);

#endif // GUARD_BATTLE_UTIL_H
