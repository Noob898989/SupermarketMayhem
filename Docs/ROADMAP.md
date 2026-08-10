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
  Character interact input + DisguiseComponent exist as code but have not
  been compiled or tested; no IA_Interact input action asset exists yet)
- placeholder products - In Progress (untracked; SupermarketMayhemProp
  class exists as code, but no prop instance is placed in the level yet)

Done when: the player can enter and move through a small supermarket. (This
specific condition is met and verified; the phase as a whole is not
complete because of the two "In Progress" items above.)

## Phase 2 - Prop system
Status: In Progress (uncommitted/untracked, not compiled or tested) -
corresponds to AP4.1
Goal: become a product.
- prop selection - implemented in code (line trace from the first-person
  camera in DisguiseComponent), unverified
- transformation - implemented in code (mesh swap in DisguiseComponent),
  unverified
- collision - implemented in code (toggled via
  SupermarketMayhemProp::SetWornByHider), unverified
- replicated-ready state - not done; current implementation is explicitly
  local/non-replicated by design (see Docs/DECISIONS.md D014)

## Phase 3 - Multiplayer prototype
Status: Not started
Goal: two or more clients see replicated players/props.

## Phase 4 - Hunter weapon
Status: Not started
Goal: one weapon can aim, fire, hit and eliminate a Hider with server authority.

## Phase 5 - Round system
Status: Done (committed, verified) - corresponds to AP3
Last verified: 2026-08-09, PIE test with 2 clients on
Lvl_SupermarketBlockout (see Saved/Logs/SupermarketMayhem_2.log: role
assignment, Preparation -> Hunt -> Result transitions, Hunter movement
lock/unlock all observed).
Goal: complete Hider vs Hunter round with roles, timers, win condition and results.

Implementation order note: this phase was implemented and verified before
Phase 2 (Prop system) and Phase 3 (Multiplayer prototype) were started,
which deviates from the phase order listed in this document. The round
system is also currently hardcoded to exactly 2 players and is
local/non-replicated (see Docs/DECISIONS.md, "Open" section, O003, and
D014). The phase order below documents the originally planned sequence and
is intentionally not being reordered to match actual implementation
history - this note records the deviation instead.

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

Do not skip the core loop validation.
