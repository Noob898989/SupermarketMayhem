# Supermarket Mayhem - Claude Code Instructions

## Project
Supermarket Mayhem is a first-person multiplayer Prop-Hunt/social action game set in a living supermarket.

Core fantasy:
"Hide as a product while the supermarket keeps running."

## Non-negotiable core decisions
- Platform: PC / Steam
- Engine: Unreal Engine 5.8
- Programming: C++ + Blueprints
- Camera: First Person
- Target match size: 2-8 players
- Public matchmaking: required
- Friends + strangers in the same public match: required
- Private matches: required
- Language: matchmaking preference, not a hard filter
- Roles: Hider and Hunter
- Hiders disguise themselves as supermarket products/props
- Hunters use weapons
- NPC customers keep the supermarket alive
- Physics and environmental chaos are important
- Voice chat is planned, not required for the first prototype
- Ranked mode is not part of V1

## How Claude must work
1. Read this file and relevant files in Docs/ before major implementation.
2. Inspect the existing Unreal project before changing code.
3. Never rewrite working systems without a reason.
4. Never silently change a core design decision.
5. For a major architectural change, explain the impact first.
6. Keep multiplayer readiness in mind from the beginning.
7. Prefer server-authoritative gameplay-critical state.
8. Use C++ for core/networking systems where appropriate and Blueprints for suitable iteration/presentation.
9. Do not add paid services or unnecessary plugins without approval.
10. Do not claim something works unless it was tested or the limitation is clearly stated.

## Source of truth
If documentation conflicts:
1. Latest explicit user decision
2. Docs/DECISIONS.md
3. GAME_DESIGN.md / GAMEPLAY.md
4. MULTIPLAYER.md / TECHNICAL_ARCHITECTURE.md
5. Other docs
6. Existing implementation

## Development order
1. First-person player
2. Supermarket blockout
3. Prop/disguise system
4. Multiplayer replication
5. Hunter weapon
6. Round system
7. NPC supermarket activity
8. Steam party/lobby/matchmaking
9. Content/polish
10. Playtesting/release

## Change protocol
If the user changes a core decision:
- update DECISIONS.md
- update affected documentation
- update ROADMAP.md if necessary
- explain affected systems
- then implement the approved change

## Current state
Docs/ROADMAP.md is the authoritative, up-to-date source for current
phase-by-phase status (including committed/uncommitted/untracked and
verification state). Do not restate detailed status here - update
Docs/ROADMAP.md instead when the project state changes.

Summary only (see Docs/ROADMAP.md for details, verification evidence and
open items):
- Phase 0 (Foundation): Done.
- Phase 1 (First playable): supermarket blockout done; interaction
  framework/placeholder products in progress (uncommitted/untracked, not
  yet compiled or tested).
- Phase 2 (Prop system): in progress (uncommitted/untracked, not yet
  compiled or tested).
- Phase 5 (Round system): implemented and verified in PIE, but local-only
  (non-replicated) and hardcoded to exactly 2 players; implemented ahead of
  Phase 2/3 in the order below (see Docs/ROADMAP.md, Phase 5 note).

Open questions requiring a Game Director decision are tracked in
Docs/DECISIONS.md under "Open (not yet decided)".
