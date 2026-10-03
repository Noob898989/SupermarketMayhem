# Supermarket Mayhem - Multiplayer

## Requirements
- public matchmaking
- strangers
- friend parties
- private matches
- 2-8 players initially
- Steam-first
- language preference
- region/ping awareness

## Server authority
Gameplay-critical state should be server authoritative:
- roles
- round state
- timers
- disguise state
- weapon state
- damage
- eliminations
- win conditions
- important interactions

Clients request actions; they should not authoritatively declare outcomes.

## Prototype
A host/listen-server approach is acceptable for early development to reduce complexity and cost.

## Round roster implementation
The GameMode reads connected player states from the authoritative GameState
player array. Role assignment uses an editable ordered role-slot array, with
one slot per expected player; the default remains `[Hider, Hunter]` for
backward-compatible two-player play. Matches begin when the connected roster
exactly matches the configured slots and includes at least one Hider and one
Hunter. Configure the active GameMode Blueprint with the intended slots for
3-8 player testing. The final role distribution for those match sizes remains
undecided. Round movement locking, elimination checks, result detection and
per-round reset iterate role-assigned players rather than fixed controller
fields. Roles, elimination and round phase writes remain server-side, while
the existing PlayerState and GameState replication remains in place.

## Weapon authority implementation
The Character owns a replicated reference to its equipped weapon actor, which
is spawned and equipped on the server when a player is assigned Hunter. Clients
send only a fire request through the weapon's Server RPC. The weapon checks
authority, equipped state, Hunter role, elimination state and Hunt phase, then
performs its own server-side `ECC_Camera` trace using the server's camera view
and configured range. Only a validated Hider hit reaches
`GameMode::EliminateHider`; the client never supplies a target or hit result.
The weapon's equipped state and Character's equipped-weapon reference
replicate, along with server-owned current ammo and reloading state. Reload
requests use a server timer; only the server completes/refills them. Reload is
cancelled on unequip, Result, and the next round's Preparation reset. The
runtime Reload action is bound locally to R. This path compiles but still
needs PIE verification.

## Production direction
Dedicated servers are the preferred final direction if the game reaches production scale.

## Steam
Steam is the first platform and should cover identity, friends, invites, lobbies and the selected networking/matchmaking approach.

Do not lock in a specific plugin/API implementation until its UE5.8 compatibility is verified.

## Party
A party remains together when entering public matchmaking.

## Voice
Planned later:
- party/lobby voice
- team voice
- proximity voice

Not required for the first playable prototype.

## Scalability
If the player count later increases, review:
- replication
- NPC count
- physics
- bandwidth
- server tick
- visibility
- matchmaking capacity
