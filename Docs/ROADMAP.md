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
- Variable roster scaling implemented: GameMode resolves connected
  PlayerStates from GameState::PlayerArray, centrally assigns configurable
  role slots, and processes all Hiders/Hunters for round transitions,
  movement locks, elimination and reset. The default two role slots remain
  Hider then Hunter. For 3-8 players, configure one role slot per player in
  the active GameMode Blueprint; a default ratio remains undecided. Source
  review confirms the fixed controller fields are gone. Unreal Engine 5.8
  Editor and Game Development targets both compile successfully. Multiplayer
  PIE verification remains pending; this validation session has no interactive
  Unreal Editor control for starting and observing multi-client PIE.

## Phase 4 - Hunter weapon
Status: In Progress (weapon foundation and feedback implemented and
compiled; runtime validation pending)
Goal: one weapon can aim, fire, hit and eliminate a Hider with server authority.
- Existing two-player PIE verification (2026-08-25) covered the prior
  HunterComponent trace/RPC path through GameMode::EliminateHider.
- Weapon foundation: reusable replicated ASupermarketMayhemWeapon actor,
  server-side Equip/Unequip, replicated equipped state and equipped-weapon
  reference on Character. Existing IA_Eliminate -> DoEliminate input requests
  fire through HunterComponent; the weapon server RPC validates Hunter role,
  equipped state, Hunt phase, attacker/target elimination state and a
  server-side ECC_Camera trace before calling GameMode::EliminateHider.
- Ammo/reload: server-owned six-round default magazine, configurable capacity
  and 1.5-second reload duration. Fire requests spend one round only after
  server validation, including misses; empty magazines and firing during
  reload are rejected. Reload uses a server timer, fills the magazine only on
  valid completion during Hunt and replicates ammo/reload state. Result and
  unequip cancel reload; each Preparation reset refills the weapon.
- A runtime Enhanced Input Reload action is mapped to R locally, without
  changing IA_Eliminate or existing input assets.
- Weapon presentation: the replicated weapon actor now owns two
  non-authoritative static-mesh components using the existing
  `/Game/Weapons/Pistol/Meshes/SM_Pistol` asset. The first-person component
  is owner-only and locally attaches to the character camera; the
  third-person component is hidden from the owner and locally attaches to
  the character's `hand_r` bone. Replicated equipped state drives visibility;
  the authoritative actor equip attachment and gameplay state are unchanged.
- Fire/reload/equip/dry-fire feedback uses the existing mannequin pistol
  montage/animation assets where available. Reload, equip and dry-fire
  sequences are played through local dynamic montages. Accepted fire triggers
  a server-originated unreliable cosmetic multicast for montage playback, the
  existing generic template weapon sound and a short muzzle point-light pulse.
  Reload and equip presentation follows their replicated authoritative
  states. Missing reload, equip and dry-fire sounds remain configurable
  hooks.
- First-person recoil is a small configurable weapon-mesh rotation with a
  timer-driven return; it does not alter camera direction or server trace
  behavior. Empty-magazine requests are rejected by the server and may return
  a throttled owner-only dry-fire cue without spending ammo or tracing.
- Weapon validation now also requires the Character's replicated equipped
  reference to point to the requesting weapon actor, preventing stale or
  unrelated owned weapon actors from acting.
- Unreal Engine 5.8 Editor and Game Development targets compile. Ammo/reload,
  elimination and weapon presentation/feedback have not yet been
  runtime/PIE-verified. Mesh/socket fit, montage compatibility and feedback
  timing still need visual confirmation in the Editor.
- Dedicated muzzle-flash particles and reload/equip/dry-fire audio remain
  future work; the current light pulse and empty audio hooks are placeholders.

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
which deviates from the phase order listed in this document. The verified
original round flow used exactly 2 players. The GameMode now supports
configured variable rosters; the role distribution for 3-8 players remains
open (see Docs/DECISIONS.md O003). Round state itself
(CurrentRoundState, RoundTimeRemaining) is replicated (DOREPLIFETIME),
correcting the "local/non-replicated" note that used to be here. The
phase order below documents the originally planned sequence and is
intentionally not being reordered to match actual implementation history
- this note records the deviation instead.

## Phase 6 - Living supermarket
Status: In Progress (NPC foundation and customer behavior implemented;
Editor/Game build and network runtime validation pending)
- `ASupermarketMayhemCustomer` provides a replicated Character with a
  replicated `Idle`, `Walk`, `Shop` or `Paused` state and a `DefaultCustomer`
  type. It uses the existing simple Manny mesh/Unarmed animation assets.
- `ASupermarketMayhemCustomerAIController` runs server-only, timer-driven
  behavior. It chooses level actors tagged `CustomerDestination` (optionally
  filtered by the profile's preferred destination tag), then falls back to a
  random reachable NavMesh point. No per-frame AI tick is used.
- `USupermarketMayhemCustomerData` is an optional Data Asset profile for
  movement speed, idle/shopping durations, preferred destination tag and
  reaction sensitivity. Without an assigned asset, one default customer
  profile is used.
- `ASupermarketMayhemCustomerSpawnManager` is created by the server GameMode
  on the first Preparation phase. The default population is 4 (configurable
  minimum 0, maximum 8); it uses actors tagged `CustomerSpawn`, falling back
  to level PlayerStarts. The GameMode exposes the count, class, profile and
  tags for configuration.
- Customers are enabled/reset for Preparation, continue through Hunt, pause
  during Result and reset at the next Preparation. Independent randomized
  idle/shop timers create varied start phases and shopping stays before each
  customer picks another destination. Preferred destination tags filter the
  shared destination-tag set; tagged points are projected and path-checked,
  with random reachable NavMesh locations as fallback.
- Noise events are delivered only by server gameplay code. After an equipped
  Hunter passes server fire validation and the server camera trace runs, a
  shot (including a miss) reports noise to the server customer manager.
  Customers apply profile sensitivity, distance falloff and a short reaction
  cooldown; strong sounds interrupt walking, select a different destination,
  and then rejoin the normal Shop/Idle flow. Result-phase events are ignored.
- The existing Manny simple mesh and Unarmed animation blueprint remain in
  use. Character movement drives locomotion; Shop is a stationary visit using
  the animation blueprint's idle presentation. No dedicated shopping
  animation asset was introduced.
- Added a `NavMeshBoundsVolume` to `Lvl_SupermarketBlockout` through the UE
  Editor scripting workflow, and enabled dynamic Recast generation for the
  small blockout. The single PlayerStart is the spawn fallback; authored
  destination tags remain optional.
- UE 5.8 Editor and Game targets now compile with the behavior expansion. A
  fresh headless local server/two-client smoke run assigned roles, entered
  Preparation, spawned four NPCs, observed all four accept a navigation
  request, and entered Hunt. This verifies startup and initial server-side
  navigation, not visual movement, client-side replication, or an actual
  Hunter shot/noise reaction. Those remain open for PIE/runtime validation.
- No project Automation Tests were present. The level scripting commandlet
  saved the NavMesh bounds successfully but returned exit code 1 because of a
  pre-existing Ensure in the protected local DisguiseComponent.

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

### Phase 6.5 - Interactive supermarket foundation
- Added non-ticking `ASupermarketMayhemInteractiveActor` with a query-only
  interaction volume, replicated enabled/type/display/target fields, server-only
  interaction handling, per-round counter reset and an opt-in physics/movement
  replication hook.
- Added `ASupermarketMayhemShelf` and `ASupermarketMayhemProduct`. Shelves are
  customer destinations and carry replicated product actor references; product
  actors replicate minimal ID/type/weight metadata. The blockout contains four
  placeholder shelves and eight placeholder products.
- Character Interact requests use a component that exposes focused actor/prompt
  queries. Server retraces from the camera and checks target identity, range,
  round, role, elimination and enabled state. Existing disguise interaction
  remains the fallback.
- Customer AI calls the same server interaction on successful shopping-target
  arrival. Preparation resets interaction counts; Result/Waiting block players.
- Editor and Game Win64 Development builds succeeded; source review and
  `git diff --check` passed. The initial headless run found zero active tiles.
  The cause was an empty, map-placed Recast actor: runtime generation did not
  request a full initial build for that existing actor. The server now builds
  navigation once at Preparation only when active tile count is zero. A fresh
  headless two-client run generated six tiles, all four NPC navigation
  requests were accepted, three NPCs reached tagged shelf targets and Hunt was
  reached. PIE and client-side replication remain open.
- No inventory, pickup/throw, product economy, HUD widget or full chaos behavior
  was added. Prompt text is exposed as an API only.
