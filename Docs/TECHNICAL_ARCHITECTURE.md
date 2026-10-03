# Supermarket Mayhem - Technical Architecture

## Engine
Unreal Engine 5.8

## Code
C++ + Blueprints.

Use C++ for core systems, networking and reusable foundations where appropriate. Use Blueprints for suitable gameplay composition, configuration and rapid iteration.

## Main systems
- Player
- Input
- Interaction
- Prop/Disguise
- Weapon
- Damage
- Round/GameState
- Lobby/Party
- Matchmaking
- NPC
- Supermarket interaction
- Physics
- UI
- Audio

## Networking
Design core gameplay for replication and server authority from the beginning.

Avoid client-trusting shortcuts that would require a rewrite later.

### Hunter weapon presentation
`ASupermarketMayhemWeapon` remains the authority for equipment, fire requests,
ammo and reload timing. Fire requests are validated on the server against the
owner's role, elimination state, Hunt phase, reload state, ammo and the
Character's equipped-weapon reference before ammo is spent and the existing
server trace/elimination path runs. Fire during reload/empty ammo, reload
outside Hunt and requests from an unequipped/stale weapon are rejected.
Unequip and Result cancel the server reload timer; Preparation resets ammo
and reload state.

The weapon actor and its gameplay fields replicate. Its first-person and
third-person mesh components are cosmetic local subobjects and are not
replicated: each instance attaches them to the owner's camera and character
hand respectively, with owner-only visibility rules. Equip and reload
presentation follows the existing replicated flags. Accepted fire uses an
unreliable server multicast for transient montage, sound and muzzle-light
feedback; small recoil animates only the owner's weapon mesh and does not
modify camera aim or server hit traces. Dry-fire feedback is an owner-only
cosmetic response to a server-rejected empty shot. Presentation asset
compatibility and multiplayer behavior still require PIE verification.

### Living supermarket customer foundation
`ASupermarketMayhemGameMode` creates the non-replicated
`ASupermarketMayhemCustomerSpawnManager` on the server at the first
Preparation transition. Its editable settings default to four customers
(minimum 0, maximum 8), a generic customer class, optional customer Data
Asset, and actor tags for level-authored spawn/destination points. Tagged
spawn actors are preferred; level PlayerStarts are the fallback.

Each `ASupermarketMayhemCustomer` is a replicated Character using standard
CharacterMovement replication. Customer type and the small Idle/Walk/Shop/
Paused state are replicated. Its `ASupermarketMayhemCustomerAIController`
exists and runs behavior only on the server. Timer callbacks transition
between states; destinations use tagged actors or random reachable NavMesh
points. No AI tick or per-movement RPC is added. The optional
`USupermarketMayhemCustomerData` asset configures speed, idle/shop durations,
preferred destination tag and reaction sensitivity; base defaults support a
single `DefaultCustomer` without requiring an authored Data Asset.

Round hooks enable/reset customers during Preparation, keep them moving
during Hunt and stop movement/behavior during Result. `ReportNoiseToCustomer`
is a server-checked extension hook only; no flee or investigate policy is
implemented. The active blockout's NavMesh coverage, level tags, animation
appearance and client replication still need PIE verification. The blockout
now contains a `NavMeshBoundsVolume`, with dynamic Recast generation enabled
for this small prototype map. A headless server/two-client smoke test
confirmed four customer spawns and accepted initial navigation requests for
all four; the editor commandlet saved the level change but exited nonzero on
the existing protected DisguiseComponent Ensure.

## Content
Products and weapons should be data-driven where useful so new content does not require rewriting core systems.

## Performance
Development PC:
- Ryzen 5 5500
- RTX 2060
- 16 GB RAM
- Windows 11 Pro

32 GB RAM is a useful future upgrade, but not a prerequisite.

Profile before optimizing blindly.

## Source control
Git from the beginning. Do not commit Unreal-generated directories such as:
- Binaries
- Intermediate
- Saved
- DerivedDataCache

## Plugins
Only add plugins with a documented purpose, UE5.8 compatibility and acceptable licensing/cost.

## Testing
Each milestone needs a reproducible test. Multiplayer tests should progress from local clients to real Steam sessions.
