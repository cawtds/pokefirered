#ifndef GUARD_BATTLE_SCRIPT_COMMANDS_H
#define GUARD_BATTLE_SCRIPT_COMMANDS_H

#include "global.h"

// Arguments for 'flags' in HandleBattleWindow
#define WINDOW_CLEAR (1 << 0)
#define WINDOW_BG1   (1 << 7)

// Arguments for 'xStart, yStart, xEnd, yEnd' in HandleBattleWindow
#define YESNOBOX_X_Y 23, 8, 29, 13

struct StatFractions
{
    u8 dividend;
    u8 divisor;
};

struct PickupItem
{
    u16 itemId;
    u8 percentage[10];
};

void AI_CalcDmg(u8 attacker, u8 defender);
u8 TypeCalc(u16 move, u8 attacker, u8 defender);
u8 AI_TypeCalc(u16 move, u16 targetSpecies, u16 targetAbility);
u8 GetBattlerTurnOrderNum(u8 battlerId);
void SetMoveEffect(bool32 primary, u32 certain);
bool32 IsMonGettingExpSentOut(void);
void BattleDestroyYesNoCursorAt(void);
void BattleCreateYesNoCursorAt(void);
void BufferMoveToLearnIntoBattleTextBuff2(void);
void HandleBattleWindow(u8 xStart, u8 yStart, u8 xEnd, u8 yEnd, u8 flags);
bool8 UproarWakeUpCheck(u8 battlerId);
u8 GetCatchingBattler(void);
bool32 IsMoveAffectedByParentalBond(u32 move, u32 battler);
u32 GetHighestStatId(u32 battlerId);
u32 GetTotalAccuracy(u32 battlerAtk, u32 battlerDef, u32 move, u32 atkAbility, u32 defAbility, u32 atkHoldEffect, u32 defHoldEffect);
bool32 DoesSubstituteBlockMove(u32 battlerAtk, u32 battlerDef, u32 move);
bool32 DoesDisguiseBlockMove(u32 battler, u32 move);
bool32 CanPoisonType(u8 battlerAttacker, u8 battlerTarget);
bool32 CanParalyzeType(u8 battlerAttacker, u8 battlerTarget);
bool32 NoAliveMonsForPlayer(void);
bool32 NoAliveMonsForEitherParty(void);
void StealTargetItem(u8 battlerStealer, u8 battlerItem);
u32 IsFlowerVeilProtected(u32 battler);
u32 IsLeafGuardProtected(u32 battler);
bool32 IsShieldsDownProtected(u32 battler);
u32 IsAbilityStatusProtected(u32 battler);
bool32 TryResetBattlerStatChanges(u8 battler);
bool32 CanBattlerSwitch(u32 battlerId);
u8 GetFirstFaintedPartyIndex(u8 battlerId);
u16 GetNaturePowerMove(void);
bool32 CanCamouflage(u8 battlerId);
bool32 IsTelekinesisBannedSpecies(u16 species);
bool32 CanUseLastResort(u8 battlerId);
bool32 ProteanTryChangeType(u32 battler, u32 ability, u32 move, u32 moveType);
s32 CalcCritChanceStageArgs(u32 battlerAtk, u32 battlerDef, u32 move, bool32 recordAbility, u32 abilityAtk, u32 abilityDef, u32 holdEffectAtk);
s32 GetCritHitChance(s32 critChanceIndex);

extern void (* const gBattleScriptingCommandsTable[])(void);
extern const struct StatFractions gAccuracyStageRatios[];

#endif // GUARD_BATTLE_SCRIPT_COMMANDS_H
