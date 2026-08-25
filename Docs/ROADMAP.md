# Supermarket Mayhem - Roadmap

Status legend used below: Not started / In Progress (uncommitted or
untracked) / In Progress (committed, unverified) / Done (committed,
verified).

AP-to-phase mapping (AP numbers as used in commit messages and source code
comments; no separate AP log file exists elsewhere in the repo, so this
mapping is the current source of truth for it):
- AP1 (implicit, not explicitly named in any commit) -> Phase 0 + the
  "first-person player" part of Phase 1
- AP2 ("supermarket blockout") -> Phase 1, "supermarket blockout" part
- AP3 ("round system") -> Phase 5
- AP4.1 ("prop/disguise, local/non-replicated") -> Phase 1, "basic
  interaction framework" / "placeholder products" parts, and Phase 2
- AP4.2 and beyond: not defined anywhere in the repository (unclear)

## Phase 0 - Foundation
Status: Done (committed, verified)
Last verified: commits 259fe99 (2026-08-07) and 92adf17 (2026-08-08); see
CLAUDE.md "Milestone 0 is working".
- UE5.8 installed
- First Person C++ template working
- WASD verified
- mouse look verified
- Git installed
- documentation package

## Phase 1 - First playable
Status: In Progress (partially done - see items below)
Goal: walk through a basic supermarket.
- first-person player - Done (committed, verified)
- supermarket blockout - Done (committed, verified; commit c6d7746,
  2026-08-09; PIE-tested same day, see Saved/Logs/SupermarketMayhem_2.log)
- basic interaction framework - In Progress (uncommitted/untracked;
  Character interact input + DisguiseComponent implemented and PIE-verified
  across multiple Props, see Phase 2)
- placeholder products - In Progress (uncommitted; 7
  SupermarketMayhemProp instances placed in
  Content/Supermarket/Maps/Lvl_SupermarketBlockout.umap with unique
  PropIds, spread across the checkout area, 4 aisles and the backoffice
  side room so the Hider can choose between multiple hiding spots;
  PIE-verified 2026-08-25, see Phase 2)

Done when: the player can enter and move through a small supermarket. (This
specific condition is met and verified; the phase as a whole is not
complete because of the two "In Progress" items above.)

## Phase 2 - Prop system
Status: In Progress (uncommitted, not yet committed) - compiled and
PIE-verified 2026-08-25 - corresponds to AP4.1
Goal: become a product.
- prop selection - implemented and PIE-verified (camera line trace in
  DisguiseComponent::FindInteractableProp, ECC_Visibility channel; Props
  use the engine-default BlockAll collision profile, which blocks
  Visibility, so the trace reliably hits them)
- transformation - implemented and PIE-verified (mesh swap in
  DisguiseComponent)
- collision - implemented and PIE-verified (toggled via
  SupermarketMayhemProp::SetWornByHider)
- replication - done and PIE-verified across 2 clients: PlayerState's
  bIsDisguised/CurrentPropId are replicated (DOREPLIFETIME +
  ReplicatedUsing=OnRep_DisguiseState) and correctly mirror the disguise
  cosmetics on remote clients. This supersedes the "local/non-replicated"
  scope originally recorded in Docs/DECISIONS.md D014 - that decision
  entry now describes historical AP4.1 scope only, not the current state.
- content - 7 Prop instances placed in Lvl_SupermarketBlockout.umap, each
  with a unique PropId (TestProp, Prop_CerealBox_A, Prop_Can_A,
  Prop_Bottle_A, Prop_CartonCrate_A, Prop_CleaningBottle_A,
  Prop_HouseholdBox_A), spread across the checkout area, 4 aisles and the
  backoffice side room; PIE-verified 2026-08-25.

## Phase 3 - Multiplayer prototype
Status: In Progress (uncommitted) - partially PIE-verified 2026-08-25
Goal: two or more clients see replicated players/props.
- Done and PIE-verified across 2 clients: role (CurrentRole), disguise
  (bIsDisguised/CurrentPropId), elimination (bIsEliminated) and round
  state (CurrentRoundState/RoundTimeRemaining) are all replicated
  (DOREPLIFETIME) and correctly observed on the remote client.
- Not yet done: scaling beyond the hardcoded 2 players
  (HiderController/HunterController as single fields on GameMode) - see
  Docs/DECISIONS.md O003 (open, not yet decided).

## Phase 4 - Hunter weapon
Status: In Progress (uncommitted/untracked) - core mechanic PIE-verified
2026-08-25
Goal: one weapon can aim, fire, hit and eliminate a Hider with server authority.
- Implemented and PIE-verified: camera-aimed hit detection (ECC_Camera
  line trace in HunterComponent::FindTargetedCharacter - ECC_Visibility
  was tried first and found to be ignored by the Character capsule/mesh
  collision profiles by engine default, then corrected to ECC_Camera,
  which the capsule blocks), server-authoritative validated elimination
  (ServerTryEliminate_Implementation -> GameMode::EliminateHider).
- Not yet done: a visible weapon actor/mesh, ammo/reload, weapon
  animation - the current implementation is an instant-elimination
  ability proving the mechanic, not yet an art-complete weapon.

## Phase 5 - Round system
Status: Done (committed, verified) - corresponds to AP3; extended
uncommitted with elimination integration and automatic restart,
PIE-verified 2026-08-25 (see below)
Last verified (committed base): 2026-08-09, PIE test with 2 clients on
Lvl_SupermarketBlockout (see Saved/Logs/SupermarketMayhem_2.log: role
assignment, Preparation -> Hunt -> Result transitions, Hunter movement
lock/unlock all observed).
Last verified (uncommitted extensions): 2026-08-25, 2-client PIE test
covering both round outcomes end-to-end:
- Hunter eliminates the Hider: server-authoritative
  ServerTryEliminate_Implementation -> GameMode::EliminateHider, which
  locks the eliminated Hider's movement
  (Character::ApplyEliminatedState, replicated to remote clients via
  PlayerState::OnRep_Eliminated) -> Result phase -> automatic restart to
  Preparation (GameMode::StartResultPhase's timer calls
  StartPreparationPhase), with the Hider's elimination/disguise state and
  movement cleanly reset for the new round
  (GameMode::ResetHiderRoundState, Character::ClearEliminatedState).
- Hider survives the Hunt timer: "Hider escaped!" log + on-screen message
  in StartResultPhase -> Result phase -> the same automatic restart.
Goal: complete Hider vs Hunter round with roles, timers, win condition and results.

Implementation order note: this phase was implemented and verified before
Phase 2 (Prop system) and Phase 3 (Multiplayer prototype) were started,
which deviates from the phase order listed in this document. The round
system is also currently hardcoded to exactly 2 players (see
Docs/DECISIONS.md, "Open" section, O003). Round state itself
(CurrentRoundState, RoundTimeRemaining) is replicated (DOREPLIFETIME),
correcting the "local/non-replicated" note that used to be here. The
phase order below documents the originally planned sequence and is
intentionally not being reordered to match actual implementation history
- this note records the deviation instead.

## Phase 6 - Living supermarket
Status: Not started
NPC customers, ambience and environmental interactions.

## Phase 7 - Steam multiplayer
Status: Not started
Friends, party, lobby, Quick Play, private match and language preference.

## Phase 8 - Content/polish
Status: Not started
More props, weapons, animation, VFX, audio, UI and optimization.

## Phase 9 - Playtesting
Status: Not started
External testers, balance, bug fixing and onboarding.

## Phase 10 - Release preparation
Status: Not started
Steam page, legal/licensing checks, builds, performance and release candidate.

Do not skip the core loop validation. (Validated 2026-08-25: full 2-player
core loop - Preparation -> Hider disguises at one of 7 Props -> Hunt ->
Hunter eliminates the Hider or the Hider escapes -> Result -> automatic
restart to Preparation - PIE-tested end-to-end for both outcomes.)
