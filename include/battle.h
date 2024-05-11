#ifndef GUARD_BATTLE_H
#define GUARD_BATTLE_H

#include <limits.h>
#include "global.h"
#include "constants/battle.h"
#include "constants/form_change_types.h"
#include "constants/battle_script_commands.h"
#include "battle_util.h"
#include "battle_script_commands.h"
#include "battle_main.h"
#include "battle_ai_switch_items.h"
#include "battle_gfx_sfx_util.h"
#include "battle_util2.h"
#include "battle_bg.h"
#include "battle_dynamax.h"

/*
    Banks are a name given to what could be called a 'battlerId' or 'monControllerId'.
    Each bank has a value consisting of two bits.
    0x1 bit is responsible for the side, 0 = player's side, 1 = opponent's side.
    0x2 bit is responsible for the id of sent out pokemon. 0 means it's the first sent out pokemon, 1 it's the second one. (Triple battle didn't exist at the time yet.)
*/

#define GET_BATTLER_POSITION(battler)((gBattlerPositions[battler]))
#define GET_BATTLER_SIDE(battler)((GetBattlerPosition(battler) & BIT_SIDE))
#define GET_BATTLER_SIDE2(battler)((GET_BATTLER_POSITION(battler) & BIT_SIDE))

// Used to exclude moves learned temporarily by Transform or Mimic
#define MOVE_IS_PERMANENT(battler, moveSlot)                        \
   (!(gBattleMons[battler].status2 & STATUS2_TRANSFORMED)           \
 && !(gDisableStructs[battler].mimickedMoves & gBitTable[moveSlot]))

// Battle Actions
// These determine what each battler will do in a turn
#define B_ACTION_USE_MOVE                  0
#define B_ACTION_USE_ITEM                  1
#define B_ACTION_SWITCH                    2
#define B_ACTION_RUN                       3
#define B_ACTION_SAFARI_WATCH_CAREFULLY    4
#define B_ACTION_SAFARI_BALL               5
#define B_ACTION_SAFARI_BAIT               6
#define B_ACTION_SAFARI_GO_NEAR            7
#define B_ACTION_SAFARI_RUN                8
#define B_ACTION_OLDMAN_THROW              9
#define B_ACTION_EXEC_SCRIPT               10
#define B_ACTION_TRY_FINISH                11
#define B_ACTION_FINISHED                  12
#define B_ACTION_CANCEL_PARTNER            12 // when choosing an action
#define B_ACTION_NOTHING_FAINTED           13 // when choosing an action
#define B_ACTION_NONE                      0xFF


// Special indicator value for shellBellDmg in SpecialStatus
#define IGNORE_SHELL_BELL 0xFFFF


// For defining EFFECT_HIT etc. with battle TV scores and flags etc.
struct __attribute__((packed, aligned(2))) BattleMoveEffect
{
    const u8 *battleScript;
    u16 encourageEncore:1;
    u16 twoTurnEffect:1;
    u16 semiInvulnerableEffect:1;
    u16 usesProtectCounter:1;
    u16 padding:12;
};

#define GET_MOVE_BATTLESCRIPT(move) gBattleMoveEffects[gMovesInfo[move].effect].battleScript

extern const struct Trainer gTrainers[];

struct ResourceFlags
{
    u32 flags[MAX_BATTLERS_COUNT];
};

#define RESOURCE_FLAG_FLASH_FIRE        0x1
#define RESOURCE_FLAG_ROOST             0x2
#define RESOURCE_FLAG_UNBURDEN          0x4
#define RESOURCE_FLAG_UNUSED            0x8
#define RESOURCE_FLAG_TRACED            0x10
#define RESOURCE_FLAG_EMERGENCY_EXIT    0x20
#define RESOURCE_FLAG_NEUTRALIZING_GAS  0x40
#define RESOURCE_FLAG_ICE_FACE          0x80

struct DisableStruct
{
    u32 transformedMonPersonality;
    bool8 transformedMonShininess;
    u16 disabledMove;
    u16 encoredMove;
    u8 protectUses:4;
    u8 stockpileCounter:4;
    s8 stockpileDef;
    s8 stockpileSpDef;
    s8 stockpileBeforeDef;
    s8 stockpileBeforeSpDef;
    u8 substituteHP;
    u8 encoredMovePos;
    u8 disableTimer:4;
    u8 encoreTimer:4;
    u8 perishSongTimer:4;
    u8 furyCutterCounter;
    u8 rolloutTimer:4;
    u8 rolloutTimerStartValue:4;
    u8 chargeTimer:4;
    u8 tauntTimer:4;
    u8 battlerPreventingEscape;
    u8 battlerWithSureHit;
    u8 isFirstTurn;
    u8 truantCounter:1;
    u8 truantSwitchInHack:1;
    u8 mimickedMoves:4;
    u8 rechargeTimer;
    u8 autotomizeCount;
    u8 slowStartTimer;
    u8 embargoTimer;
    u8 magnetRiseTimer;
    u8 telekinesisTimer;
    u8 healBlockTimer;
    u8 laserFocusTimer;
    u8 throatChopTimer;
    u8 wrapTurns;
    u8 tormentTimer:4; // used for G-Max Meltdown
    u8 usedMoves:4;
    u8 noRetreat:1;
    u8 tarShot:1;
    u8 octolock:1;
    u8 cudChew:1;
    u8 spikesDone:1;
    u8 toxicSpikesDone:1;
    u8 stickyWebDone:1;
    u8 stealthRockDone:1;
    u8 syrupBombTimer;
    u8 syrupBombIsShiny:1;
    u8 steelSurgeDone:1;
    u8 weatherAbilityDone:1;
    u8 terrainAbilityDone:1;
    u8 usedProteanLibero:1;
};

extern struct DisableStruct gDisableStructs[MAX_BATTLERS_COUNT];

struct ProtectStruct
{
    u32 protected:1;
    u32 spikyShielded:1;
    u32 kingsShielded:1;
    u32 banefulBunkered:1;
    u32 obstructed:1;
    u32 endured:1;
    u32 noValidMoves:1;
    u32 helpingHand:1;
    u32 bounceMove:1;
    u32 stealMove:1;
    u32 prlzImmobility:1;
    u32 confusionSelfDmg:1;
    u32 targetAffected:1;
    u32 chargingTurn:1;
    u32 fleeType:2; // 0: Normal, 1: FLEE_ITEM, 2: FLEE_ABILITY
    u32 usedImprisonedMove:1;
    u32 loveImmobility:1;
    u32 usedDisabledMove:1;
    u32 usedTauntedMove:1;
    u32 flag2Unknown:1; // Only set to 0 once. Checked in 'WasUnableToUseMove' function.
    u32 flinchImmobility:1;
    u32 notFirstStrike:1;
    u32 palaceUnableToUseMove:1;
    u32 usesBouncedMove:1;
    u32 usedHealBlockedMove:1;
    u32 usedGravityPreventedMove:1;
    u32 powderSelfDmg:1;
    u32 usedThroatChopPreventedMove:1;
    u32 statRaised:1;
    u32 usedMicleBerry:1;
    u32 usedCustapBerry:1;    // also quick claw
    u32 touchedProtectLike:1;
    // End of 32-bit bitfield
    u16 disableEjectPack:1;
    u16 statFell:1;
    u16 pranksterElevated:1;
    u16 quickDraw:1;
    u16 beakBlastCharge:1;
    u16 quash:1;
    u16 shellTrap:1;
    u16 maxGuarded:1;
    u16 silkTrapped:1;
    u16 burningBulwarked:1;
    u16 eatMirrorHerb:1;
    u16 activateOpportunist:2; // 2 - to copy stats. 1 - stats copied (do not repeat). 0 - no stats to copy
    u16 usedAllySwitch:1;
    u32 physicalDmg;
    u32 specialDmg;
    u8 physicalBattlerId;
    u8 specialBattlerId;
    // pokefirered
    /* field_0 */
    /* field_1 */
    u32 targetNotAffected:1;
    /* field_2 */
};

extern struct ProtectStruct gProtectStructs[MAX_BATTLERS_COUNT];

struct SpecialStatus
{
    s32 shellBellDmg;
    s32 physicalDmg;
    s32 specialDmg;
    u8 physicalBattlerId;
    u8 specialBattlerId;
    u8 changedStatsBattlerId; // Battler that was responsible for the latest stat change. Can be self.
    u8 statLowered:1;
    u8 lightningRodRedirected:1;
    u8 restoredBattlerSprite: 1;
    u8 traced:1;
    u8 faintedHasReplacement:1;
    u8 focusBanded:1;
    u8 focusSashed:1;
    // End of byte
    u8 sturdied:1;
    u8 stormDrainRedirected:1;
    u8 switchInAbilityDone:1;
    u8 switchInItemDone:1;
    u8 instructedChosenTarget:3;
    u8 berryReduced:1;
    // End of byte
    u8 gemParam;
    // End of byte
    u8 gemBoost:1;
    u8 rototillerAffected:1;  // to be affected by rototiller
    u8 parentalBondState:2;
    u8 multiHitOn:1;
    u8 announceNeutralizingGas:1;   // See Cmd_switchineffects
    u8 neutralizingGasRemoved:1;    // See VARIOUS_TRY_END_NEUTRALIZING_GAS
    u8 affectionEndured:1;
    // End of byte
    u8 damagedMons:4; // Mons that have been damaged directly by using a move, includes substitute.
    u8 dancerUsedMove:1;
    u8 dancerOriginalTarget:3;
    // End of byte
    u8 emergencyExited:1;
    u8 afterYou:1;
    u8 preventLifeOrbDamage:1; // So that Life Orb doesn't activate various effects.
    // pokeemrald
    u8 intimidatedMon:1;          // 0x8
    u8 ppNotAffectedByPressure:1;
    s32 dmg;
};

extern struct SpecialStatus gSpecialStatuses[MAX_BATTLERS_COUNT];

struct SideTimer
{
    /*0x00*/ u8 reflectTimer;
    /*0x01*/ u8 reflectBattlerId;
    /*0x02*/ u8 lightscreenTimer;
    /*0x03*/ u8 lightscreenBattlerId;
    /*0x04*/ u8 mistTimer;
    /*0x05*/ u8 mistBattlerId;
    /*0x06*/ u8 safeguardTimer;
    /*0x07*/ u8 safeguardBattlerId;
    /*0x08*/ u8 followmeTimer;
    /*0x09*/ u8 followmeTarget;
    /*0x0A*/ u8 spikesAmount;
    /*0x0B*/ u8 fieldB;
    // pokeemerald
    u8 retaliateTimer;
    u8 stealthRockAmount;
    u8 stickyWebAmount;
    u8 stickyWebBattlerId;
    u8 stickyWebBattlerSide; // Used for Court Change
    u8 tailwindTimer;
    u8 tailwindBattlerId;
    u8 auroraVeilTimer;
    u8 auroraVeilBattlerId;
    u8 toxicSpikesAmount;
    u8 followmePowder:1; // Rage powder, does not affect grass type pokemon.
    u8 steelsurgeAmount;
    u8 luckyChantTimer;
    u8 luckyChantBattlerId;
    u8 damageNonTypesTimer;
    u8 damageNonTypesType;
    u8 rainbowTimer;
    u8 seaOfFireTimer;
    u8 swampTimer;
};

extern struct SideTimer gSideTimers[];

struct FieldTimer
{
    u8 mudSportTimer;
    u8 waterSportTimer;
    u8 wonderRoomTimer;
    u8 magicRoomTimer;
    u8 trickRoomTimer;
    u8 terrainTimer;
    u8 gravityTimer;
    u8 fairyLockTimer;
};

struct WishFutureKnock
{
    u8 futureSightCounter[MAX_BATTLERS_COUNT];
    u8 futureSightBattlerIndex[MAX_BATTLERS_COUNT];
    u8 futureSightPartyIndex[MAX_BATTLERS_COUNT];
    u8 futureSightAttacker[MAX_BATTLERS_COUNT]; // pokefirered, remove
    s32 futureSightDmg[MAX_BATTLERS_COUNT]; // pokefirered, remove
    u16 futureSightMove[MAX_BATTLERS_COUNT];
    u8 wishCounter[MAX_BATTLERS_COUNT];
    u8 wishPartyId[MAX_BATTLERS_COUNT];
    u8 weatherDuration;
    u8 knockedOffMons[NUM_BATTLE_SIDES];
};

extern struct WishFutureKnock gWishFutureKnock;

struct AI_SavedBattleMon
{
    u16 ability;
    u16 moves[MAX_MON_MOVES];
    u16 heldItem;
    u16 species;
    u8 types[3];
};

struct AiPartyMon
{
    u16 species;
    u16 item;
    u16 heldEffect;
    u16 ability;
    u16 gender;
    u16 level;
    u16 moves[MAX_MON_MOVES];
    u32 status;
    bool8 isFainted;
    bool8 wasSentInBattle;
    u8 switchInCount; // Counts how many times this Pokemon has been sent out or switched into in a battle.
};

struct AIPartyData // Opposing battlers - party mons.
{
    struct AiPartyMon mons[NUM_BATTLE_SIDES][PARTY_SIZE]; // 2 parties(player, opponent). Used to save information on opposing party.
    u8 count[NUM_BATTLE_SIDES];
};

struct SwitchinCandidate
{
    struct BattlePokemon battleMon;
    bool8 hypotheticalStatus;
};

// Ai Data used when deciding which move to use, computed only once before each turn's start.
struct AiLogicData
{
    u16 abilities[MAX_BATTLERS_COUNT];
    u16 items[MAX_BATTLERS_COUNT];
    u16 holdEffects[MAX_BATTLERS_COUNT];
    u8 holdEffectParams[MAX_BATTLERS_COUNT];
    u16 predictedMoves[MAX_BATTLERS_COUNT];
    u8 hpPercents[MAX_BATTLERS_COUNT];
    u16 partnerMove;
    u16 speedStats[MAX_BATTLERS_COUNT]; // Speed stats for all battles, calculated only once, same way as damages
    s32 simulatedDmg[MAX_BATTLERS_COUNT][MAX_BATTLERS_COUNT][MAX_MON_MOVES]; // attacker, target, moveIndex
    u8 effectiveness[MAX_BATTLERS_COUNT][MAX_BATTLERS_COUNT][MAX_MON_MOVES]; // attacker, target, moveIndex
    u8 moveAccuracy[MAX_BATTLERS_COUNT][MAX_BATTLERS_COUNT][MAX_MON_MOVES]; // attacker, target, moveIndex
    u8 moveLimitations[MAX_BATTLERS_COUNT];
    bool8 shouldSwitchMon; // Because all available moves have no/little effect. Each bit per battler.
    u8 monToSwitchId[MAX_BATTLERS_COUNT]; // ID of the mon to switch.
    bool8 weatherHasEffect; // The same as WEATHER_HAS_EFFECT. Stored here, so it's called only once.
    u8 mostSuitableMonId[MAX_BATTLERS_COUNT]; // Stores result of GetMostSuitableMonToSwitchInto, which decides which generic mon the AI would switch into if they decide to switch. This can be overruled by specific mons found in ShouldSwitch; the final resulting mon is stored in AI_monToSwitchIntoId.
    struct SwitchinCandidate switchinCandidate; // Struct used for deciding which mon to switch to in battle_ai_switch_items.c
};

struct AI_ThinkingStruct
{
    u8 aiState;
    u8 movesetIndex;
    u16 moveConsidered;
    s32 score[MAX_MON_MOVES];
    u32 funcResult;
    u32 aiFlags[MAX_BATTLERS_COUNT];
    u8 aiAction;
    u8 aiLogicId;
    struct AI_SavedBattleMon saved[MAX_BATTLERS_COUNT];
};

#define AI_MOVE_HISTORY_COUNT 3

struct BattleHistory
{
    /*0x00*/ u16 usedMoves[2][8]; // 0xFFFF means move not used (confuse self hit, etc)
    u16 abilities[MAX_BATTLERS_COUNT];
    /*0x22*/ u8 itemEffects[MAX_BATTLERS_COUNT];
    /*0x24*/ u16 trainerItems[MAX_BATTLERS_COUNT];
    /*0x2C*/ u8 itemsNo;
    u16 moveHistory[MAX_BATTLERS_COUNT][AI_MOVE_HISTORY_COUNT]; // 3 last used moves for each battler
    u8 moveHistoryIndex[MAX_BATTLERS_COUNT];
    u16 heldItems[MAX_BATTLERS_COUNT];
};

struct BattleScriptsStack
{
    const u8 *ptr[8];
    u8 size;
};

struct BattleCallbacksStack
{
    void (*function[8])(void);
    u8 size;
};

struct StatsArray
{
    u16 stats[NUM_STATS];
};

struct BattleResources
{
    struct SecretBaseRecord *secretBase;
    struct ResourceFlags *flags;
    struct BattleScriptsStack *battleScriptsStack;
    struct BattleCallbacksStack *battleCallbackStack;
    struct StatsArray *beforeLvlUp;
    struct AI_ThinkingStruct *ai;
    struct AiLogicData *aiData;
    struct AIPartyData *aiParty;
    struct BattleHistory *battleHistory;
    struct BattleScriptsStack *AI_ScriptsStack;
    u8 bufferA[MAX_BATTLERS_COUNT][0x200];
    u8 bufferB[MAX_BATTLERS_COUNT][0x200];
    u8 transferBuffer[0x100];
};

extern struct BattleResources *gBattleResources;

#define AI_THINKING_STRUCT ((struct AI_ThinkingStruct *)(gBattleResources->ai))
#define AI_DATA ((struct AiLogicData *)(gBattleResources->aiData))
#define AI_PARTY ((struct AIPartyData *)(gBattleResources->aiParty))
#define BATTLE_HISTORY ((struct BattleHistory *)(gBattleResources->battleHistory))

struct BattleResults
{
    u8 playerFaintCounter;    // 0x0
    u8 opponentFaintCounter;  // 0x1
    u8 playerSwitchesCounter; // 0x2
    u8 numHealingItemsUsed;   // 0x3
    u8 numRevivesUsed;        // 0x4
    u8 playerMonWasDamaged:1; // 0x5
    u8 usedMasterBall:1;      // 0x5
    u8 caughtMonBall:4;       // 0x5
    u8 shinyWildMon:1;        // 0x5
    u8 unk5_7:1;              // 0x5
    u16 playerMon1Species;    // 0x6
    u8 playerMon1Name[11];    // 0x8
    u8 battleTurnCounter;     // 0x13
    u8 playerMon2Name[11];    // 0x14
    u8 pokeblockThrows;       // 0x1F
    u16 lastOpponentSpecies;  // 0x20
    u16 lastUsedMovePlayer;   // 0x22
    u16 lastUsedMoveOpponent; // 0x24
    u16 playerMon2Species;    // 0x26
    u16 caughtMonSpecies;     // 0x28
    u8 caughtMonNick[10];     // 0x2A
    u8 filler34[2];
    u8 catchAttempts[11];     // 0x36
};

extern struct BattleResults gBattleResults;

struct LinkBattlerHeader
{
    u8 versionSignatureLo;
    u8 versionSignatureHi;
    u8 vsScreenHealthFlagsLo;
    u8 vsScreenHealthFlagsHi;
    struct BattleEnigmaBerry battleEnigmaBerry;
};

struct MegaEvolutionData
{
    u8 toEvolve; // As flags using gBitTable.
    bool8 alreadyEvolved[4]; // Array id is used for mon position.
    u8 battlerId;
    bool8 playerSelect;
    u8 triggerSpriteId;
};

struct UltraBurstData
{
    u8 toBurst; // As flags using gBitTable.
    bool8 alreadyBursted[4]; // Array id is used for mon position.
    u8 battlerId;
    bool8 playerSelect;
    u8 triggerSpriteId;
};

struct Illusion
{
    u8 on;
    u8 set;
    u8 broken;
    u8 partyId;
    struct Pokemon *mon;
};

struct ZMoveData
{
    u8 viable:1;    // current move can become a z move
    u8 viewing:1;  // if player is viewing the z move name instead of regular moves
    u8 active:1;   // is z move being used this turn
    u8 zStatusActive:1;
    u8 healReplacement:1;
    u8 activeCategory:2;  // active z move category
    u8 zUnused:1;
    u8 triggerSpriteId;
    u8 possibleZMoves[MAX_BATTLERS_COUNT];
    u16 chosenZMove;  // z move of move cursor is on
    u8 effect;
    u8 used[MAX_BATTLERS_COUNT];   //one per bank for multi-battles
    u16 toBeUsed[MAX_BATTLERS_COUNT];  // z moves per battler to be used
    u16 baseMoves[MAX_BATTLERS_COUNT];
    u8 categories[MAX_BATTLERS_COUNT];
};

struct DynamaxData
{
    bool8 playerSelect;
    u8 triggerSpriteId;
    u8 toDynamax; // flags using gBitTable
    bool8 alreadyDynamaxed[NUM_BATTLE_SIDES];
    bool8 dynamaxed[MAX_BATTLERS_COUNT];
    u8 dynamaxTurns[MAX_BATTLERS_COUNT];
    u8 usingMaxMove[MAX_BATTLERS_COUNT];
    u8 activeCategory;
    u8 categories[MAX_BATTLERS_COUNT];
    u16 baseMove[MAX_BATTLERS_COUNT]; // base move of Max Move
    u16 lastUsedBaseMove;
    u16 levelUpHP;
};

struct LostItem
{
    u16 originalItem:15;
    u16 stolen:1;
};

struct BattleStruct
{
    u8 turnEffectsTracker;
    u8 turnEffectsBattlerId;
    u8 filler2; // unused
    u8 turnCountersTracker;
    u16 wrappedMove[MAX_BATTLERS_COUNT];
    u8 moveTarget[MAX_BATTLERS_COUNT];
    u8 expGetterMonId;
    u8 field_11; // unused
    u8 wildVictorySong;
    u8 dynamicMoveType;
    u8 wrappedBy[MAX_BATTLERS_COUNT];
    u8 focusPunchBattlerId;
    u8 battlerPreventingSwitchout;
    u8 moneyMultiplier;
    u8 savedTurnActionNumber;
    u8 switchInAbilitiesCounter;
    u8 faintedActionsState;
    u8 faintedActionsBattlerId;
    // balign 2
    u32 expValue;
    u8 scriptPartyIdx; // for printing the nickname
    u8 sentInPokes;
    bool8 selectionScriptFinished[MAX_BATTLERS_COUNT];
    u8 battlerPartyIndexes[MAX_BATTLERS_COUNT];
    u8 monToSwitchIntoId[MAX_BATTLERS_COUNT];
    u8 battlerPartyOrders[MAX_BATTLERS_COUNT][3];
    u8 runTries;
    u8 caughtMonNick[POKEMON_NAME_LENGTH + 1];
    u8 field_78; // unused
    u8 safariRockThrowCounter; // safariGoNearCounter in pokeemerald
    u8 safariBaitThrowCounter; // safariPkblThrowCounter in pokeemerald
    u8 safariEscapeFactor;
    u8 safariCatchFactor;
    u8 linkBattleVsSpriteId_V;
    u8 linkBattleVsSpriteId_S;
    u8 formToChangeInto;
    u8 chosenMovePositions[MAX_BATTLERS_COUNT];
    u8 stateIdAfterSelScript[MAX_BATTLERS_COUNT];
    u8 field_88; // unused
    u8 field_89; // unused
    u8 field_8A; // unused
    u8 playerPartyIdx;
    u8 field_8C; // unused
    u8 field_8D; // unused
    u8 stringMoveType;
    u8 expGetterBattlerId;
    u8 field_90; // unused
    u8 absentBattlerFlags;
    u8 AI_monToSwitchIntoId[2];
    u8 simulatedInputState[4];  // used by Oak/Old Man/Pokedude controllers
    u8 lastTakenMove[MAX_BATTLERS_COUNT * 2 * 2]; // ask gamefreak why they declared it that way
    u16 hpOnSwitchout[2];
    u16 abilityPreventingSwitchout;
    u8 hpScale;
    u16 savedBattleTypeFlags;
    void (*savedCallback)(void);
    u16 synchronizeMoveEffect;
    u8 multiplayerId;
    u8 overworldWeatherDone;
    u8 atkCancellerTracker;
    u16 usedHeldItems[PARTY_SIZE][NUM_BATTLE_SIDES]; // For each party member and side. For harvest, recycle
    u8 chosenItem[MAX_BATTLERS_COUNT]; // why is this an u8?
    u8 AI_itemType[2];
    u8 AI_itemFlags[2];
    u16 choicedMove[MAX_BATTLERS_COUNT];
    u16 changedItems[MAX_BATTLERS_COUNT];
    u8 intimidateBattler;
    u8 switchInItemsCounter;
    u8 field_DA; // battle tower related
    u8 turnSideTracker;
    u8 fillerDC[0xDF-0xDC];
    u8 givenExpMons;
    u8 lastTakenMoveFrom[MAX_BATTLERS_COUNT][MAX_BATTLERS_COUNT];
    u8 wishPerishSongState;
    u8 wishPerishSongBattlerId;
    u8 lastAttackerToFaintOpponent;
    // align 4
    union {
        struct LinkBattlerHeader linkBattlerHeader;
        struct MultiPartnerMenuPokemon multiBattleMons[3];
    } multiBuffer;
    u8 itemPartyIndex[MAX_BATTLERS_COUNT];
    u8 itemMoveIndex[MAX_BATTLERS_COUNT];
    u8 padding_1E4[0x14];
    // pokeemerald
    u8 isSkyBattle:1;
    u8 isAtkCancelerForCalledMove:1; // Certain cases in atk canceler should only be checked once, when the original move is called, however others need to be checked the twice.
    u8 swapDamageCategory:1; // Photon Geyser, Shell Side Arm, Light That Burns the Sky
    u8 beatUpSlot:3;
    u8 pledgeMove:1;
    u8 magnitudeBasePower;
    u8 presentBasePower;
    u8 sameMoveTurns[MAX_BATTLERS_COUNT]; // For Metronome, number of times the same moves has been SUCCESFULLY used.
    u8 lastMoveFailed; // as bits for each battler, for the sake of Stomping Tantrum
    bool8 ateBoost[MAX_BATTLERS_COUNT];
    u8 timesGotHit[NUM_BATTLE_SIDES][PARTY_SIZE];
    u8 supremeOverlordCounter[MAX_BATTLERS_COUNT];
    struct Illusion illusion[MAX_BATTLERS_COUNT];
    u8 trainerSlideFirstSTABMoveMsgState:2;
    u8 blunderPolicy:1; // should blunder policy activate
    u8 skyDropTargets[MAX_BATTLERS_COUNT]; // For Sky Drop, to account for if multiple Pokemon use Sky Drop in a double battle.
    u8 bonusCritStages[MAX_BATTLERS_COUNT]; // G-Max Chi Strike boosts crit stages of allies.
    u8 enduredDamage;
    u16 changedSpecies[NUM_BATTLE_SIDES][PARTY_SIZE]; // For forms when multiple mons can change into the same pokemon.
    u8 trainerSlideFirstCriticalHitMsgState:2;
    u8 trainerSlideFirstSuperEffectiveHitMsgState:2;
    u8 additionalEffectsCounter:4; // A counter for the additionalEffects applied by the current move in Cmd_setadditionaleffects
    u8 targetsDone[MAX_BATTLERS_COUNT]; // Each battler as a bit.
    u16 moveEffect2; // For Knock Off
    u8 stolenStats[NUM_BATTLE_STATS]; // hp byte is used for which stats to raise, other inform about by how many stages
    u8 stickySyrupdBy[MAX_BATTLERS_COUNT];
    u8 moneyMultiplierMove:1;
    struct LostItem itemLost[PARTY_SIZE];  // Player's team that had items consumed or stolen (two bytes per party member)
    u8 savedBattlerTarget;
    u8 ateBerry[2]; // array id determined by side, each party pokemon as bit
    u16 overwrittenAbilities[MAX_BATTLERS_COUNT];    // abilities overwritten during battle (keep separate from battle history in case of switching)
    struct MegaEvolutionData mega;
    struct UltraBurstData burst;
    struct ZMoveData zmove;
    struct DynamaxData dynamax;
    u8 appearedInBattle; // Bitfield to track which Pokemon appeared in battle. Used for Burmy's form change
    bool8 allowedToChangeFormInWeather[PARTY_SIZE][NUM_BATTLE_SIDES]; // For each party member and side, used by Ice Face.
    u8 startingStatus; // status to apply at battle start. defined in constants/battle.h
    u8 startingStatusTimer;
    u8 transformZeroToHero[NUM_BATTLE_SIDES];
    u8 supersweetSyrup[NUM_BATTLE_SIDES];
    u8 intrepidSwordBoost[NUM_BATTLE_SIDES];
    u8 dauntlessShieldBoost[NUM_BATTLE_SIDES];
    u16 tracedAbility[MAX_BATTLERS_COUNT];
    u16 hpBefore[MAX_BATTLERS_COUNT]; // Hp of battlers before using a move. For Berserk and Anger Shell.
    u8 friskedBattler; // Frisk needs to identify 2 battlers in double battles.
    bool8 friskedAbility; // If identifies two mons, show the ability pop-up only once.
    bool8 spriteIgnore0Hp;
    u8 moneyMultiplierItem:1;
    u8 expGettersOrder[PARTY_SIZE]; // First battlers which were sent out, then via exp-share
    u32 expShareExpValue;
    u8 expOrderId:3;
    u8 expSentInMons; // As bits for player party mons - not including exp share mons.
    u8 teamGotExpMsgPrinted:1; // The 'Rest of your team got msg' has been printed.
    u8 roostTypes[MAX_BATTLERS_COUNT][2];
    u8 lastMoveTarget[MAX_BATTLERS_COUNT]; // The last target on which each mon used a move, for the sake of Instruct
    u8 attackerBeforeBounce:2;
    bool8 hitSwitchTargetFailed:1;
    u8 storedHealingWish:4; // Each battler as a bit.
    u8 storedLunarDance:4; // Each battler as a bit.
    u8 forcedSwitch:4; // For each battler
    u8 alreadyStatusedMoveAttempt; // As bits for battlers; For example when using Thunder Wave on an already paralyzed Pokémon.
    u8 soulheartBattlerId;
    const u8 *trainerSlideMsg;
    u8 battleBondTransformed[NUM_BATTLE_SIDES]; // Bitfield for each party.
    u8 quickClawBattlerId;
    bool8 effectsBeforeUsingMoveDone:1; // Mega Evo and Focus Punch/Shell Trap effects.
    u8 focusPunchBattlers; // as bits
    u8 quickClawRandom[MAX_BATTLERS_COUNT];
    u8 quickDrawRandom[MAX_BATTLERS_COUNT];
    bool8 throwingPokeBall;
    s32 aiFinalScore[MAX_BATTLERS_COUNT][MAX_BATTLERS_COUNT][MAX_MON_MOVES]; // AI, target, moves to make debugging easier
    u8 aiMoveOrAction[MAX_BATTLERS_COUNT];
    u8 aiChosenTarget[MAX_BATTLERS_COUNT];
    u32 aiDelayTimer; // Counts number of frames AI takes to choose an action.
    u32 aiDelayFrames; // Number of frames it took to choose an action.
    // pokeemerald unknown use
    u8 field_93; // related to choosing pokemon? probably related to recording
};

extern struct BattleStruct *gBattleStruct;

#define DYNAMIC_TYPE_MASK                 ((1 << 6) - 1)
#define F_DYNAMIC_TYPE_IGNORE_PHYSICALITY  (1 << 6) // If set, the dynamic type's physicality won't be used for certain move effects.
#define F_DYNAMIC_TYPE_SET                 (1 << 7) // Set for all dynamic types to distinguish a dynamic type of Normal (0) from no dynamic type.

#define GET_MOVE_TYPE(move, typeArg)                                  \
{                                                                     \
    if (gBattleStruct->dynamicMoveType)                               \
        typeArg = gBattleStruct->dynamicMoveType & DYNAMIC_TYPE_MASK; \
    else                                                              \
        typeArg = gMovesInfo[move].type;                            \
}

#define IS_MOVE_PHYSICAL(move)(GetBattleMoveCategory(move) == DAMAGE_CATEGORY_PHYSICAL)
#define IS_MOVE_SPECIAL(move)(GetBattleMoveCategory(move) == DAMAGE_CATEGORY_SPECIAL)
#define IS_MOVE_STATUS(move)(gMovesInfo[move].category == DAMAGE_CATEGORY_STATUS)

#define IS_MOVE_RECOIL(move)(gMovesInfo[move].recoil > 0 || gMovesInfo[move].effect == EFFECT_RECOIL_IF_MISS)

#define IS_TYPE_PHYSICAL(moveType)(moveType < TYPE_MYSTERY) // TODO: remove
#define IS_TYPE_SPECIAL(moveType)(moveType > TYPE_MYSTERY)  // TODO: remove

#define BATTLER_MAX_HP(battlerId)(gBattleMons[battlerId].hp == gBattleMons[battlerId].maxHP)
#define TARGET_TURN_DAMAGED ((gSpecialStatuses[gBattlerTarget].physicalDmg != 0 || gSpecialStatuses[gBattlerTarget].specialDmg != 0) || (gBattleStruct->enduredDamage & gBitTable[gBattlerTarget]))
#define BATTLER_TURN_DAMAGED(battlerId) ((gSpecialStatuses[battlerId].physicalDmg != 0 || gSpecialStatuses[battlerId].specialDmg != 0) || (gBattleStruct->enduredDamage & gBitTable[battler]))

#define IS_BATTLER_OF_TYPE(battlerId, type)((GetBattlerType(battlerId, 0) == type || GetBattlerType(battlerId, 1) == type || (GetBattlerType(battlerId, 2) != TYPE_MYSTERY && GetBattlerType(battlerId, 2) == type)))

#define IS_BATTLER_TYPELESS(battlerId)(GetBattlerType(battlerId, 0) == TYPE_MYSTERY && GetBattlerType(battlerId, 1) == TYPE_MYSTERY && GetBattlerType(battlerId, 2) == TYPE_MYSTERY)

#define SET_BATTLER_TYPE(battlerId, type)           \
{                                                   \
    gBattleMons[battlerId].type1 = type;            \
    gBattleMons[battlerId].type2 = type;            \
    gBattleMons[battlerId].type3 = TYPE_MYSTERY;    \
}

#define RESTORE_BATTLER_TYPE(battlerId)                                                     \
{                                                                                           \
    gBattleMons[battlerId].type1 = gSpeciesInfo[gBattleMons[battlerId].species].types[0];   \
    gBattleMons[battlerId].type2 = gSpeciesInfo[gBattleMons[battlerId].species].types[1];   \
    gBattleMons[battlerId].type3 = TYPE_MYSTERY;                                            \
}

#define IS_BATTLER_PROTECTED(battlerId)(gProtectStructs[battlerId].protected                                           \
                                        || gSideStatuses[GetBattlerSide(battlerId)] & SIDE_STATUS_WIDE_GUARD           \
                                        || gSideStatuses[GetBattlerSide(battlerId)] & SIDE_STATUS_QUICK_GUARD          \
                                        || gSideStatuses[GetBattlerSide(battlerId)] & SIDE_STATUS_CRAFTY_SHIELD        \
                                        || gSideStatuses[GetBattlerSide(battlerId)] & SIDE_STATUS_MAT_BLOCK            \
                                        || gProtectStructs[battlerId].spikyShielded                                    \
                                        || gProtectStructs[battlerId].kingsShielded                                    \
                                        || gProtectStructs[battlerId].banefulBunkered                                  \
                                        || gProtectStructs[battlerId].burningBulwarked                                 \
                                        || gProtectStructs[battlerId].obstructed                                       \
                                        || gProtectStructs[battlerId].silkTrapped)

#define GET_STAT_BUFF_ID(n)((n & 7))              // first three bits 0x1, 0x2, 0x4
#define GET_STAT_BUFF_VALUE_WITH_SIGN(n)((n & 0xF8))
#define GET_STAT_BUFF_VALUE(n)(((n >> 3) & 0xF))      // 0x8, 0x10, 0x20, 0x40
#define STAT_BUFF_NEGATIVE 0x80                     // 0x80, the sign bit

#define SET_STAT_BUFF_VALUE(n)((((n) << 3) & 0xF8))

#define SET_STATCHANGER(statId, stage, goesDown)(gBattleScripting.statChanger = (statId) + (stage << 4) + (goesDown << 7))
#define SET_STATCHANGER2(dst, statId, stage, goesDown)(dst = (statId) + ((stage) << 3) + (goesDown << 7))
// TODO: change in battle script(?) from stage << 4 to stage << 3

// NOTE: The members of this struct have hard-coded offsets
//       in include/constants/battle_script_commands.h
struct BattleScripting
{
    s32 painSplitHp;
    s32 bideDmg;
    u8 multihitString[6];
    bool8 expOnCatch;
    u8 unused;
    u8 animArg1;
    u8 animArg2;
    u16 savedStringId;
    u8 moveendState;
    u8 savedStatChanger; // For further use, if attempting to change stat two times(ex. Moody)
    u8 shiftSwitched; // When the game tells you the next enemy's pokemon and you switch. Option for noobs but oh well.
    u8 battler;
    u8 animTurn;
    u8 animTargetsHit;
    u8 statChanger;
    bool8 statAnimPlayed;
    u8 getexpState;
    u8 battleStyle;
    u8 drawlvlupboxState;
    u8 learnMoveState;
    u8 savedBattler;
    u8 reshowMainState;
    u8 reshowHelperState;
    u8 levelUpHP;
    u8 windowsType; // B_WIN_TYPE_*
    u8 multiplayerId;
    u8 specialTrainerBattleType;
    bool8 monCaught;
    s32 savedDmg;
    u16 savedMoveEffect; // For moves hitting multiple targets.
    u16 moveEffect;
    u16 multihitMoveEffect;
    u8 illusionNickHack; // To properly display nick in STRINGID_ENEMYABOUTTOSWITCHPKMN.
    bool8 fixedPopup;   // Force ability popup to stick until manually called back
    u16 abilityPopupOverwrite;
    u8 switchCase;  // Special switching conditions, eg. red card
    u8 overrideBerryRequirements;
    u8 stickyWebStatDrop; // To prevent Defiant activating on a Court Change'd Sticky Web
    // TODO: remove pokefirered members
    u8 dmgMultiplier;
};

struct BattleSpriteInfo
{
    /*0x0*/ u16 invisible : 1; // 0x1
            u16 lowHpSong : 1; // 0x2
            u16 behindSubstitute : 1; // 0x4
            u16 flag_x8 : 1; // 0x8
            u16 hpNumbersNoBars : 1; // 0x10
    /*0x2*/ u16 transformSpecies;
};

struct BattleAnimationInfo
{
    u16 animArg; // to fill up later
    u8 field_2;
    u8 field_3;
    u8 field_4;
    u8 field_5;
    u8 field_6;
    u8 field_7;
    u8 ballThrowCaseId:6;
    u8 isCriticalCapture:1;
    u8 criticalCaptureSuccess:1;
    u8 introAnimActive:1;
    u8 wildMonInvisible:1;
    u8 field_9_x1C:3;
    u8 field_9_x20:1;
    u8 field_9_x40:1;
    u8 field_9_x80:1;
    u8 numBallParticles;
    u8 field_B;
    s16 ballSubpx;
    u8 field_E;
    u8 field_F;
};

struct BattleHealthboxInfo
{
    u8 partyStatusSummaryShown : 1; // x1
    u8 healthboxIsBouncing : 1; // x2
    u8 battlerIsBouncing : 1; // x4
    u8 ballAnimActive : 1; // 0x8
    u8 statusAnimActive : 1; // x10
    u8 animFromTableActive : 1; // x20
    u8 specialAnimActive : 1; // x40
    u8 triedShinyMonAnim : 1; // x80
    u8 finishedShinyMonAnim : 1; // x1
    u8 opponentDrawPartyStatusSummaryDelay : 5; // x2
    u8 healthboxBounceSpriteId;
    u8 battlerBounceSpriteId;
    u8 animationState;
    u8 partyStatusDelayTimer;
    u8 matrixNum;
    u8 shadowSpriteId;
    u8 soundTimer;
    u8 introEndDelay;
};

struct BattleBarInfo
{
    u8 healthboxSpriteId;
    s32 maxValue;
    s32 oldValue;
    s32 receivedValue; // if positive/negative, fills the bar to the left/right respectively
    s32 currValue;
};

struct BattleSpriteData
{
    struct BattleSpriteInfo *battlerData;
    struct BattleHealthboxInfo *healthBoxesData;
    struct BattleAnimationInfo *animationData;
    struct BattleBarInfo *battleBars;
};

extern struct BattleSpriteData *gBattleSpritesDataPtr;

#define BATTLE_BUFFER_LINK_SIZE 0x1000

extern u8 *gLinkBattleSendBuffer;
extern u8 *gLinkBattleRecvBuffer;

// Move this somewhere else

#include "sprite.h"

struct MonSpritesGfx
{
    void *firstDecompressed; // ptr to the decompressed sprite of the first pokemon
    void *sprites[MAX_BATTLERS_COUNT];
    struct SpriteTemplate templates[MAX_BATTLERS_COUNT];
    struct SpriteFrameImage images[MAX_BATTLERS_COUNT][4];
    u8 field_F4[0x80 - (4 * MAX_BATTLERS_COUNT)]; // unused, original - spritesGfx
    // u8 *spritesGfx[MAX_BATTLERS_COUNT];
    u8 *barFontGfx;
    void *field_178; // freed but never allocated
    u16 *multiUseBuffer;
};

struct PokedudeBattlerState
{
    u8 action_idx;
    u8 move_idx;
    u8 timer;
    u8 msg_idx;
    u8 saved_bg0y;
};

struct QueuedStatBoost
{
    u8 stats;   // bitfield for each battle stat that is set if the stat changes
    s8 statChanges[NUM_BATTLE_STATS - 1];    // highest bit being set decreases the stat
}; /* size = 8 */

extern u16 gBattle_BG0_X;
extern u16 gBattle_BG0_Y;
extern u16 gBattle_BG1_X;
extern u16 gBattle_BG1_Y;
extern u16 gBattle_BG2_X;
extern u16 gBattle_BG2_Y;
extern u16 gBattle_BG3_X;
extern u16 gBattle_BG3_Y;
extern u16 gBattle_WIN0H;
extern u16 gBattle_WIN0V;
extern u16 gBattle_WIN1H;
extern u16 gBattle_WIN1V;
extern struct BattleSpritesGfx *gMonSpritesGfx;
extern u8 gBattleOutcome;
extern u16 gLastUsedItem;
extern u32 gBattleTypeFlags;
extern struct MonSpritesGfx *gMonSpritesGfxPtr;
extern u16 gTrainerBattleOpponent_A;
extern u16 gTrainerBattleOpponent_B;
extern u16 gMoveToLearn;
extern u16 gBattleMovePower;
extern struct BattleEnigmaBerry gEnigmaBerries[MAX_BATTLERS_COUNT];
extern u16 gCurrentMove;
extern u16 gChosenMove;
extern u16 gCalledMove;
extern u8 gCritMultiplier;
extern u16 gBattleWeather;
extern u16 gLastUsedAbility;
extern u8 gBattlerInMenuId;
extern u8 gPotentialItemEffectBattler;
extern u8 gBattlersCount;
extern u16 gBattlerPartyIndexes[MAX_BATTLERS_COUNT];
extern s32 gBattleMoveDamage;
extern u16 gIntroSlideFlags;
extern u32 gTransformedPersonalities[MAX_BATTLERS_COUNT];
extern bool8 gTransformedShininess[MAX_BATTLERS_COUNT];
extern u8 gBattlerPositions[MAX_BATTLERS_COUNT];
extern u8 gHealthboxSpriteIds[MAX_BATTLERS_COUNT];
extern u8 gBattleOutcome;
extern u8 gBattleMonForms[MAX_BATTLERS_COUNT];
extern u32 gBattleControllerExecFlags;
// extern u8 gBattleBufferA[MAX_BATTLERS_COUNT][0x200];
// extern u8 gBattleBufferB[MAX_BATTLERS_COUNT][0x200];
extern u8 gActionSelectionCursor[MAX_BATTLERS_COUNT];
extern void (*gPreBattleCallback1)(void);
extern bool8 gDoingBattleAnim;
extern struct PokedudeBattlerState *gPokedudeBattlerStates[MAX_BATTLERS_COUNT];
extern u8 *gBattleAnimBgTileBuffer;
extern u8 *gBattleAnimBgTilemapBuffer;
extern void (*gBattleMainFunc)(void);
extern u8 gMoveSelectionCursor[MAX_BATTLERS_COUNT];
extern u8 gBattlerAttacker;
extern u8 gBattlerTarget;
extern u8 gEffectBattler;
extern u8 gMultiHitCounter;
extern struct BattleScripting gBattleScripting;
extern u8 gBattlerFainted;
extern u32 gStatuses3[MAX_BATTLERS_COUNT];
extern u32 gStatuses4[MAX_BATTLERS_COUNT];
extern u8 gSentPokesToOpponent[2];
extern const u8 *gBattlescriptCurrInstr;
extern const u8 *gSelectionBattleScripts[MAX_BATTLERS_COUNT];
extern u16 gLastMoves[MAX_BATTLERS_COUNT];
extern u8 gBattlerByTurnOrder[MAX_BATTLERS_COUNT];
extern u8 gBattleCommunication[BATTLE_COMMUNICATION_ENTRIES_COUNT];
extern u32 gSideStatuses[NUM_BATTLE_SIDES];
extern u32 gHitMarker;
extern u16 gChosenMoveByBattler[MAX_BATTLERS_COUNT];
extern u8 gMoveResultFlags;
extern s32 gTakenDmg[MAX_BATTLERS_COUNT];
extern u8 gTakenDmgByBattler[MAX_BATTLERS_COUNT];
extern u8 gCurrentActionFuncId;
extern u8 gCurrMovePos;
extern u8 gChosenMovePos;
extern u8 gBattlerStatusSummaryTaskId[MAX_BATTLERS_COUNT];
extern u16 gDynamicBasePower;
extern u16 gLastLandedMoves[MAX_BATTLERS_COUNT];
extern u8 gLastHitBy[MAX_BATTLERS_COUNT];
extern u8 gMultiUsePlayerCursor;
extern u8 gNumberOfMovesToChoose;
extern u16 gLastHitByType[MAX_BATTLERS_COUNT];
extern s32 gHpDealt;
extern u16 gPauseCounterBattle;
extern u16 gPaydayMoney;
extern u16 gLockedMoves[MAX_BATTLERS_COUNT];
extern u8 gCurrentTurnActionNumber;
extern u16 gExpShareExp;
extern u8 gLeveledUpInBattle;
extern u16 gLastResultingMoves[MAX_BATTLERS_COUNT];
extern u16 gLastPrintedMoves[MAX_BATTLERS_COUNT];
extern u8 gActionsByTurnOrder[MAX_BATTLERS_COUNT];
extern u8 gChosenActionByBattler[MAX_BATTLERS_COUNT];
extern u8 gBattleTerrain;
extern struct MultiPartnerMenuPokemon gMultiPartnerParty[MULTI_PARTY_SIZE];
extern u16 gRandomTurnNumber;
extern u8 gPartyCriticalHits[PARTY_SIZE];
extern const struct BattleMoveEffect gBattleMoveEffects[];
extern u32 gFieldStatuses;
extern u8 gPlayerDpadHoldFrames;
extern u8 gBattlerAbility;
extern s32 gBideDmg[MAX_BATTLERS_COUNT];
extern u8 gBideTarget[MAX_BATTLERS_COUNT];
extern u16 gLastUsedMove;
extern u8 gIsCriticalHit;
extern struct FieldTimer gFieldTimers;
extern bool8 gHasFetchedBall;
extern u8 gLastUsedBall;
extern u16 gLastThrownBall;
extern u16 gBallToDisplay;
extern struct QueuedStatBoost gQueuedStatBoosts[MAX_BATTLERS_COUNT];
extern struct BattlePokemon gBattleMons[MAX_BATTLERS_COUNT];
extern u8 gAbsentBattlerFlags;

static inline u32 GetBattlerPosition(u32 battler)
{
    return gBattlerPositions[battler];
}

static inline u32 GetBattlerSide(u32 battler)
{
    return GetBattlerPosition(battler) & BIT_SIDE;
}

static inline struct Pokemon *GetSideParty(u32 side)
{
    return (side == B_SIDE_PLAYER) ? gPlayerParty : gEnemyParty;
}

static inline struct Pokemon *GetBattlerParty(u32 battler)
{
    return GetSideParty(GetBattlerSide(battler));
}


#endif // GUARD_BATTLE_H
